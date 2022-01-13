#include "MacXPlatform.h"
#include <unistd.h>

using namespace std;

int MacXPlatform::fd = -1;
MacXSystemServiceTask* MacXPlatform::systemServiceTask = NULL;
QPointer<MacXExtServerService> MacXPlatform::extService;

static const QString kFinderSyncBundleId = QString::fromUtf8("mega.mac.MEGAShellExtFinder");
static const QString kFinderSyncPath = QString::fromUtf8("/Applications/MEGAsync.app/Contents/PlugIns/MEGAShellExtFinder.appex/");

void MacXPlatform::initialize(int argc, char *argv[])
{
#ifdef QT_DEBUG
    return;
#endif

    setMacXActivationPolicy();
    SetProcessName(QString::fromUtf8("MEGAsync"));

    fd = -1;
    if (argc)
    {
        long int value = strtol(argv[argc - 1], NULL, 10);
        if (value > 0 && value < INT_MAX)
        {
            fd = value;
        }
    }

    if (fd < 0)
    {
        if (!enableSetuidBit())
        {
            MacXPlatform::disableSignalHandler();
            ::exit(0);
        }

        //Reboot
        QString app = MegaApplication::applicationDirPath();
        QString launchCommand = QString::fromUtf8("open");
        QStringList args = QStringList();
        QDir appPath(app);
        appPath.cdUp();
        appPath.cdUp();
        args.append(QString::fromAscii("-n"));
        args.append(appPath.absolutePath());
        QProcess::startDetached(launchCommand, args);
        sleep(2);
        MacXPlatform::disableSignalHandler();
        ::exit(0);
    }
}

void MacXPlatform::prepareForSync()
{

}

QStringList MacXPlatform::multipleUpload(QString uploadTitle)
{
    return uploadMultipleFiles(uploadTitle);
}

bool MacXPlatform::enableTrayIcon(QString executable)
{
    return false;
}

bool MacXPlatform::startOnStartup(bool value)
{
   return startAtLogin(value);
}

bool MacXPlatform::isStartOnStartupActive()
{
    return isStartAtLoginActive();
}

void MacXPlatform::addFinderExtensionToSystem()
{
    QStringList scriptArgs;
    scriptArgs << QString::fromUtf8("-a")
               << kFinderSyncPath;

    QProcess::startDetached(QString::fromUtf8("pluginkit"), scriptArgs);
}

bool MacXPlatform::isFinderExtensionEnabled()
{
    QStringList scriptArgs;
    scriptArgs << QString::fromUtf8("-m")
               << QString::fromUtf8("-i")
               << kFinderSyncBundleId;

    QProcess p;
    p.start(QString::fromAscii("pluginkit"), scriptArgs);
    if (!p.waitForFinished(2000))
    {
        return false;
    }

    QString out = QString::fromUtf8(p.readAllStandardOutput().trimmed());
    if (out.isEmpty())
    {
        return false;
    }

    if (out.at(0) != QChar::fromAscii('?') && out.at(0) != QChar::fromAscii('+'))
    {
        return false;
    }

    return true;
}

void MacXPlatform::reinstallFinderExtension()
{
    QStringList scriptArgs;
    scriptArgs << QString::fromUtf8("-r")
               << kFinderSyncPath;

    QProcess::startDetached(QString::fromUtf8("pluginkit"), scriptArgs);
}

void MacXPlatform::reloadFinderExtension()
{
    bool finderExtEnabled = isFinderExtensionEnabled();
    if (!finderExtEnabled) // No need to reload, extension is currenctly disabled and next time user enable it, it will launch updated version
    {
        return;
    }

    QStringList scriptArgs;
    scriptArgs << QString::fromUtf8("-e")
               << QString::fromUtf8("tell application \"MEGAShellExtFinder\" to quit");

    QProcess p;
    p.start(QString::fromAscii("osascript"), scriptArgs);
    if (!p.waitForFinished(2000))
    {
        return;
    }

    scriptArgs.clear();
    scriptArgs << QString::fromUtf8("-c")
               << QString::fromUtf8("pluginkit -e ignore -i mega.mac.MEGAShellExtFinder && sleep 1 && pluginkit -e use -i mega.mac.MEGAShellExtFinder");
    QProcess::startDetached(QString::fromUtf8("bash"), scriptArgs);
}

void MacXPlatform::enableFinderExtension(bool value)
{
    QStringList scriptArgs;
    scriptArgs << QString::fromUtf8("-e")
               << (value ? QString::fromUtf8("use") : QString::fromUtf8("ignore")) //Enable or disable extension plugin
               << QString::fromUtf8("-i")
               << kFinderSyncBundleId;

    QProcess::startDetached(QString::fromUtf8("pluginkit"), scriptArgs);
}

void MacXPlatform::showInFolder(QString pathIn)
{

    //Escape possible double quotes from osascript command to avoid syntax errors and stop parsing arguments
    pathIn.replace(QString::fromLatin1("\""), QString::fromLatin1("\\\""));

    QStringList scriptArgs;
    scriptArgs << QString::fromUtf8("-e")
               << QString::fromUtf8("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(pathIn);
    QProcess::startDetached(QString::fromUtf8("osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QString::fromUtf8("-e")
               << QString::fromUtf8("tell application \"Finder\" to activate");
    QProcess::startDetached(QString::fromAscii("osascript"), scriptArgs);
}

void MacXPlatform::startShellDispatcher(MegaApplication *receiver)
{
    if (!systemServiceTask)
    {
        systemServiceTask = new MacXSystemServiceTask(receiver);
    }

    if (!extService)
    {
        extService = new MacXExtServerService(receiver);
    }
}

void MacXPlatform::stopShellDispatcher()
{
    if (systemServiceTask)
    {
        delete systemServiceTask;
        systemServiceTask = NULL;
    }

    if (extService)
    {
        delete extService;
    }
}

void MacXPlatform::notifyItemChange(string *localPath, int newState)
{
    if (extService && localPath && localPath->size())
    {
        emit extService->itemChange(QString::fromStdString(*localPath), newState);
    }
}

void MacXPlatform::syncFolderAdded(QString syncPath, QString syncName, QString syncID)
{
    addPathToPlaces(syncPath,syncName);
    setFolderIcon(syncPath);

    if (extService)
    {
        emit extService->syncAdd(syncPath, syncName);
    }
}

void MacXPlatform::syncFolderRemoved(QString syncPath, QString syncName, QString syncID)
{
    removePathFromPlaces(syncPath);
    unSetFolderIcon(syncPath);

    if (extService)
    {
        emit extService->syncDel(syncPath, syncName);
    }
}

void MacXPlatform::notifyRestartSyncFolders()
{
    notifyAllSyncFoldersRemoved();
    notifyAllSyncFoldersAdded();
}

void MacXPlatform::notifyAllSyncFoldersAdded()
{
    if (extService)
    {
        emit extService->allClients(MacXExtServer::NOTIFY_ADD_SYNCS);
    }
}

void MacXPlatform::notifyAllSyncFoldersRemoved()
{
    if (extService)
    {
        emit extService->allClients(MacXExtServer::NOTIFY_DEL_SYNCS);
    }
}

QByteArray MacXPlatform::encrypt(QByteArray data, QByteArray key)
{
    return data;
}

QByteArray MacXPlatform::decrypt(QByteArray data, QByteArray key)
{
    return data;
}

QByteArray MacXPlatform::getLocalStorageKey()
{
    return QByteArray(128, 0);
}

QString MacXPlatform::getDefaultOpenApp(QString extension)
{
    return defaultOpenApp(extension);
}

void MacXPlatform::enableDialogBlur(QDialog *dialog)
{

}

bool MacXPlatform::enableSetuidBit()
{
    QString command = QString::fromUtf8("do shell script \"chown root /Applications/MEGAsync.app/Contents/MacOS/MEGAsync && chmod 4755 /Applications/MEGAsync.app/Contents/MacOS/MEGAsync && echo true\"");
    char *response = runWithRootPrivileges((char *)command.toUtf8().constData());
    if (!response)
    {
        return false;
    }
    bool result = strlen(response) >= 4 && !strncmp(response, "true", 4);
    delete [] response;
    return result;
}

void MacXPlatform::activateBackgroundWindow(QDialog *)
{

}

bool MacXPlatform::registerUpdateJob()
{
    return registerUpdateDaemon();
}

void MacXPlatform::execBackgroundWindow(QDialog *window)
{
    window->exec();
}

void MacXPlatform::uninstall()
{

}

bool MacXPlatform::shouldRunHttpServer()
{
    return runHttpServer();
}

bool MacXPlatform::shouldRunHttpsServer()
{
    return runHttpsServer();
}

bool MacXPlatform::isUserActive()
{
    return userActive();
}

double MacXPlatform::getUpTime()
{
    return uptime();
}

void MacXPlatform::disableSignalHandler()
{
    signal(SIGSEGV, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
}

// Platform-specific strings
const char* MacXPlatform::settingsString {QT_TRANSLATE_NOOP("Platform", "Preferences")};
const char* MacXPlatform::exitString {QT_TRANSLATE_NOOP("Platform", "Quit")};
const char* MacXPlatform::fileExplorerString {QT_TRANSLATE_NOOP("Platform","Show in Finder")};
