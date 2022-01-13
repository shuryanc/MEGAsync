#include "LinuxPlatform.h"

#include <QSet>
#include <QX11Info>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include <cstdlib>
#include <cstring>
#include <map>

using namespace std;
using namespace mega;

static xcb_atom_t getAtom(xcb_connection_t * const connection, const char *name);

static std::string getProperty(xcb_connection_t * const connection,
                               const xcb_window_t window,
                               const char *name);

ExtServer *LinuxPlatform::ext_server = NULL;
NotifyServer *LinuxPlatform::notify_server = NULL;

static QString autostart_dir = QDir::homePath() + QString::fromAscii("/.config/autostart/");

QString LinuxPlatform::desktop_file = autostart_dir + QString::fromAscii("megasync.desktop");
QString LinuxPlatform::set_icon = QString::fromUtf8("gvfs-set-attribute -t string \"%1\" metadata::custom-icon file://%2");
QString LinuxPlatform::remove_icon = QString::fromUtf8("gvfs-set-attribute -t unset \"%1\" metadata::custom-icon");
QString LinuxPlatform::custom_icon = QString::fromUtf8("/usr/share/icons/hicolor/256x256/apps/mega.png");

void LinuxPlatform::initialize(int argc, char *argv[])
{
}

void LinuxPlatform::prepareForSync()
{

}

bool LinuxPlatform::enableTrayIcon(QString executable)
{
    return false;
}

void LinuxPlatform::notifyItemChange(string *localPath, int)
{
    if (notify_server && localPath && localPath->size()
            && !Preferences::instance()->overlayIconsDisabled())
    {
        notify_server->notifyItemChange(localPath);
    }
}

// enable or disable MEGASync launching at startup
// return true if operation succeeded
bool LinuxPlatform::startOnStartup(bool value)
{
    // copy desktop file into autostart directory
    if (value)
    {
        if (QFile(desktop_file).exists())
        {
            return true;
        }
        else
        {
            // make sure directory exist
            if (!QDir(autostart_dir).exists())
            {
                if (!QDir().mkdir(autostart_dir))
                {
                    //LOG_debug << "Failed to create autostart dir: " << autostart_dir;
                    return false;
                }
            }
            QString app_desktop = QString::fromAscii("/usr/share/applications/megasync.desktop");
            if (QFile(app_desktop).exists())
            {
                return QFile::copy(app_desktop, desktop_file);
            }
            else
            {
                //LOG_debug << "Desktop file does not exist: " << app_desktop;
                return false;
            }
        }
    }
    else
    {
        // remove desktop file if it exists
        if (QFile(desktop_file).exists())
        {
            return QFile::remove(desktop_file);
        }
    }
    return true;
}

bool LinuxPlatform::isStartOnStartupActive()
{
    return QFile(desktop_file).exists();
}

bool LinuxPlatform::isTilingWindowManager()
{
    static const QSet<QString> tiling_wms = {
        QString::fromUtf8("i3")
    };

    return getValue("MEGASYNC_ASSUME_TILING_WM", false)
           || tiling_wms.contains(getWindowManagerName());
}

void LinuxPlatform::showInFolder(QString pathIn)
{
    QString filebrowser = getDefaultFileBrowserApp();
    // Nautilus on Gnome, does not open the directory if argument is given without surrounding double-quotes;
    // Path is passed through QUrl which properly escapes special chars in native platform URIs
    // which takes care of path names also containing double-quotes withing, which will stop
    // Nautilus from parsing the argument string all-together
    QProcess::startDetached(filebrowser + QString::fromLatin1(" \"")
                            + QUrl::fromLocalFile(pathIn).toString() + QString::fromLatin1("\""));
}

void LinuxPlatform::startShellDispatcher(MegaApplication *receiver)
{
    if (!ext_server)
    {
        ext_server = new ExtServer(receiver);
    }

    if (!notify_server)
    {
        notify_server = new NotifyServer();
    }
}

void LinuxPlatform::stopShellDispatcher()
{
    if (ext_server)
    {
        delete ext_server;
        ext_server = NULL;
    }

    if (notify_server)
    {
        delete notify_server;
        notify_server = NULL;
    }
}

void LinuxPlatform::syncFolderAdded(QString syncPath, QString syncName, QString syncID)
{
    if (QFile(custom_icon).exists())
    {
        QFile *folder = new QFile(syncPath);
        if (folder->exists())
        {
            QProcess::startDetached(set_icon.arg(folder->fileName()).arg(custom_icon));
        }
        delete folder;

    }

    if (notify_server)
    {
        notify_server->notifySyncAdd(syncPath);
    }
}

void LinuxPlatform::syncFolderRemoved(QString syncPath, QString syncName, QString syncID)
{
    QFile *folder = new QFile(syncPath);
    if (folder->exists())
    {
        QProcess::startDetached(remove_icon.arg(folder->fileName()));
    }
    delete folder;

    if (notify_server)
    {
        notify_server->notifySyncDel(syncPath);
    }
}

void LinuxPlatform::notifyRestartSyncFolders()
{

}

void LinuxPlatform::notifyAllSyncFoldersAdded()
{

}

void LinuxPlatform::notifyAllSyncFoldersRemoved()
{

}

QByteArray LinuxPlatform::encrypt(QByteArray data, QByteArray key)
{
    return data;
}

QByteArray LinuxPlatform::decrypt(QByteArray data, QByteArray key)
{
    return data;
}

QByteArray LinuxPlatform::getLocalStorageKey()
{
    return QByteArray(128, 0);
}

QString LinuxPlatform::getDefaultFileBrowserApp()
{
    return getDefaultOpenAppByMimeType(QString::fromUtf8("inode/directory"));
}

QString LinuxPlatform::getDefaultOpenApp(QString extension)
{
    char *mimeType = MegaApi::getMimeType(extension.toUtf8().constData());
    if (!mimeType)
    {
        return QString();
    }
    QString qsMimeType(QString::fromUtf8(mimeType));
    delete mimeType;
    return getDefaultOpenAppByMimeType(qsMimeType);
}

QString LinuxPlatform::getDefaultOpenAppByMimeType(QString mimeType)
{
    QString getDefaultAppDesktopFileName = QString::fromUtf8("xdg-mime query default ") + mimeType;

    QProcess process;
    process.start(getDefaultAppDesktopFileName,
                  QIODevice::ReadWrite | QIODevice::Text);
    if(!process.waitForFinished(5000))
    {
        return QString();
    }

    QString desktopFileName = QString::fromUtf8(process.readAllStandardOutput());
    desktopFileName = desktopFileName.trimmed();
    desktopFileName.replace(QString::fromUtf8(";"), QString::fromUtf8(""));
    if (!desktopFileName.size())
    {
        return QString();
    }

    QFileInfo desktopFile(QString::fromUtf8("/usr/share/applications/") + desktopFileName);
    if (!desktopFile.exists())
    {
        return QString();
    }

    QFile f(desktopFile.absoluteFilePath());
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        return QString();
    }

    QTextStream in(&f);
    QStringList contents = in.readAll().split(QString::fromUtf8("\n"));
    contents = contents.filter(QRegExp(QString::fromUtf8("^Exec=")));
    if (!contents.size())
    {
        return QString();
    }

    QString line = contents.first();
    int index = line.indexOf(QChar::fromAscii('%'));
    int size = -1;
    if (index != -1)
    {
        size = index - 6;
    }

    if (!size)
    {
        return QString();
    }

    return line.mid(5, size);
}

bool LinuxPlatform::getValue(const char * const name, const bool default_value)
{
    const char * const value = getenv(name);

    if (!value)
        return default_value;

    return strcmp(value, "0") != 0;
}

std::string LinuxPlatform::getValue(const char * const name, const std::string &default_value)
{
    const char * const value = getenv(name);

    if (!value)
        return default_value;

    return value;
}

QString LinuxPlatform::getWindowManagerName()
{
    static std::string window_manager_name;
    static bool cached = false;

    if (!cached)
    {
        window_manager_name =
          getProperty(QX11Info::connection(),
                      QX11Info::appRootWindow(),
                      "_NET_WM_NAME");

        cached = true;
    }

    return QString::fromStdString(window_manager_name);
}

void LinuxPlatform::enableDialogBlur(QDialog *dialog)
{

}

void LinuxPlatform::activateBackgroundWindow(QDialog *)
{

}

void LinuxPlatform::execBackgroundWindow(QDialog *window)
{
    window->exec();
}

bool LinuxPlatform::registerUpdateJob()
{
    return true;
}

void LinuxPlatform::uninstall()
{

}

QStringList LinuxPlatform::getListRunningProcesses()
{
    QProcess p;
    p.start(QString::fromUtf8("ps ax -o comm"));
    p.waitForFinished(2000);
    QString output = QString::fromUtf8(p.readAllStandardOutput().constData());
    QString e = QString::fromUtf8(p.readAllStandardError().constData());
    if (e.size())
    {
        MegaApi::log(MegaApi::LOG_LEVEL_ERROR, "Error for \"ps ax -o comm\" command:");
        MegaApi::log(MegaApi::LOG_LEVEL_ERROR, e.toUtf8().constData());
    }

    p.start(QString::fromUtf8("/bin/bash -c \"readlink /proc/*/exe\""));
    p.waitForFinished(2000);
    QString output2 = QString::fromUtf8(p.readAllStandardOutput().constData());
    e = QString::fromUtf8(p.readAllStandardError().constData());
    if (e.size())
    {
        MegaApi::log(MegaApi::LOG_LEVEL_ERROR, "Error for \"readlink /proc/*/exe\" command:");
        MegaApi::log(MegaApi::LOG_LEVEL_ERROR, e.toUtf8().constData());
    }
    QStringList data = output.split(QString::fromUtf8("\n"));

    data.append(output2.split(QString::fromUtf8("\n")));

    return data;
}

// Check if it's needed to start the local HTTP server
// for communications with the webclient
bool LinuxPlatform::shouldRunHttpServer()
{
    QStringList data = getListRunningProcesses();
    if (data.size() > 1)
    {
        for (int i = 1; i < data.size(); i++)
        {
            // The MEGA webclient sends request to MEGAsync to improve the
            // user experience. We check if web browsers are running because
            // otherwise it isn't needed to run the local web server for this purpose.
            // Here is the list or web browsers that allow HTTP communications
            // with 127.0.0.1 inside HTTPS webs.
            QString command = data.at(i).trimmed();
            if (command.contains(QString::fromUtf8("firefox"), Qt::CaseInsensitive)
                    || command.contains(QString::fromUtf8("chrome"), Qt::CaseInsensitive)
                    || command.contains(QString::fromUtf8("chromium"), Qt::CaseInsensitive)
                    )
            {
                return true;
            }
        }
    }
    return false;
}

// Check if it's needed to start the local HTTPS server
// for communications with the webclient
bool LinuxPlatform::shouldRunHttpsServer()
{
    QStringList data = getListRunningProcesses();

    if (data.size() > 1)
    {
        for (int i = 1; i < data.size(); i++)
        {
            // The MEGA webclient sends request to MEGAsync to improve the
            // user experience. We check if web browsers are running because
            // otherwise it isn't needed to run the local web server for this purpose.
            // Here is the list or web browsers that don't allow HTTP communications
            // with 127.0.0.1 inside HTTPS webs and therefore require a HTTPS server.
            QString command = data.at(i).trimmed();
            if (command.contains(QString::fromUtf8("safari"), Qt::CaseInsensitive)
                    || command.contains(QString::fromUtf8("iexplore"), Qt::CaseInsensitive)
                    || command.contains(QString::fromUtf8("opera"), Qt::CaseInsensitive)
                    || command.contains(QString::fromUtf8("iceweasel"), Qt::CaseInsensitive)
                    || command.contains(QString::fromUtf8("konqueror"), Qt::CaseInsensitive)
                    )
            {
                return true;
            }
        }
    }
    return false;
}

bool LinuxPlatform::isUserActive()
{
    return true;
}

xcb_atom_t getAtom(xcb_connection_t * const connection, const char *name)
{
    xcb_intern_atom_cookie_t cookie =
      xcb_intern_atom(connection, 0, strlen(name), name);
    xcb_intern_atom_reply_t *reply =
      xcb_intern_atom_reply(connection, cookie, nullptr);

    if (!reply)
        return XCB_ATOM_NONE;

    xcb_atom_t result = reply->atom;
    free(reply);

    return result;
}

std::string getProperty(xcb_connection_t * const connection,
                        const xcb_window_t window,
                        const char *name)
{
    static xcb_atom_t atom_type = XCB_ATOM_NONE;
    static const size_t buffer_length = 255;

    char buffer[buffer_length + 1];

    if (!atom_type)
    {
        atom_type = getAtom(connection, "UTF8_STRING");
        if (!atom_type)
            return std::string();
    }

    xcb_atom_t atom_name = getAtom(connection, name);
    if (!atom_name)
        return std::string();

    xcb_get_property_cookie_t cookie =
      xcb_get_property(connection,
                       0,
                       window,
                       atom_name,
                       atom_type,
                       0,
                       buffer_length);

    xcb_get_property_reply_t *reply =
      xcb_get_property_reply(connection, cookie, nullptr);
    if (!reply)
        return std::string();

    const int value_length = xcb_get_property_value_length(reply);
    if (value_length > 0)
    {
        const char *value =
          static_cast<const char *>(xcb_get_property_value(reply));

        memcpy(buffer, value, value_length);
        buffer[value_length] = '\0';
    }

    free(reply);

    return std::string(buffer);
}

// Platform-specific strings
const char* LinuxPlatform::settingsString {QT_TRANSLATE_NOOP("Platform", "Settings")};
const char* LinuxPlatform::exitString {QT_TRANSLATE_NOOP("Platform", "Exit")};
const char* LinuxPlatform::fileExplorerString {QT_TRANSLATE_NOOP("Platform", "Show in folder")};
