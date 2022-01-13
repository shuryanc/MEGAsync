#include "mega/types.h"
#include "StreamingFromMegaDialog.h"
#include "ui_StreamingFromMegaDialog.h"
#include "NodeSelector.h"

#include "QMegaMessageBox.h"
#include "platform/Platform.h"
#include "control/Utilities.h"
#include "HighDpiResize.h"

#include <QCloseEvent>

#if QT_VERSION >= 0x050000
#include <QtConcurrent/QtConcurrent>
#endif

#define MAX_STREAMING_BUFFER_SIZE 8242880 // 8 MB

using namespace mega;

StreamingFromMegaDialog::StreamingFromMegaDialog(mega::MegaApi *megaApi, QWidget *parent) :
    QDialog(parent),
    ui(::mega::make_unique<Ui::StreamingFromMegaDialog>()),
    lastStreamSelection{LastStreamingSelection::NOT_SELECTED}
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, true);
    Qt::WindowFlags flags =  Qt::Window | Qt::WindowSystemMenuHint
                                | Qt::WindowMinimizeButtonHint
                                | Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);

    this->megaApi = megaApi;
    this->selectedMegaNode = NULL;
    this->megaApi->httpServerSetMaxBufferSize(MAX_STREAMING_BUFFER_SIZE);

    int port = 4443;
    while (!megaApi->httpServerStart(true, port) && port < 4448)
    {
        port++;
    }

    setWindowTitle(tr("Stream from MEGA"));
    ui->bCopyLink->setEnabled(false);
    ui->sFileInfo->setCurrentWidget(ui->pNothingSelected);
    delegateListener = ::mega::make_unique<QTMegaRequestListener>(this->megaApi, this);
    delegateTransferListener = ::mega::make_unique<QTMegaTransferListener>(this->megaApi, this);
    megaApi->addTransferListener(delegateTransferListener.get());
    highDpiResize.init(this);
    hideStreamingError();
}

StreamingFromMegaDialog::~StreamingFromMegaDialog()
{
    megaApi->httpServerStop();
}

void StreamingFromMegaDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QDialog::changeEvent(event);
}

void StreamingFromMegaDialog::closeEvent(QCloseEvent *event)
{
    if (!event->spontaneous() || !selectedMegaNode)
    {
        event->accept();
        return;
    }

    const unique_ptr<QMessageBox> messageBox{::mega::make_unique<QMessageBox>(this)};
    messageBox->setIcon(QMessageBox::Question);
    messageBox->setWindowTitle(tr("Stream from MEGA"));
    messageBox->setText(tr("Are you sure that you want to stop the streaming?"));
    messageBox->addButton(QMessageBox::Yes);
    messageBox->addButton(QMessageBox::No);
    messageBox->setDefaultButton(QMessageBox::No);
    int button = messageBox->exec();

    if (button == QMessageBox::Yes)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void StreamingFromMegaDialog::on_bFromCloud_clicked()
{
    const unique_ptr<NodeSelector> nodeSelector{::mega::make_unique<NodeSelector>(megaApi, NodeSelector::STREAM_SELECT, this->parentWidget())};
    int result = nodeSelector->exec();
    if (!nodeSelector || result != QDialog::Accepted)
    {
        return;
    }
    MegaNode *node = megaApi->getNodeByHandle(nodeSelector->getSelectedFolderHandle());
    updateFileInfoFromNode(node);
}

void StreamingFromMegaDialog::on_bFromPublicLink_clicked()
{
    const unique_ptr<QInputDialog> inputDialog{::mega::make_unique<QInputDialog>(this)};
    inputDialog->setWindowFlags(inputDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    inputDialog->setWindowTitle(tr("Open link"));
    inputDialog->setLabelText(tr("Enter a MEGA file link:"));
    inputDialog->resize(470, inputDialog->height());
    int result = inputDialog->exec();
    if (!inputDialog || !result)
    {
        return;
    }
    mPublicLink = inputDialog->textValue();
    lastStreamSelection = LastStreamingSelection::FROM_PUBLIC_NODE;
    requestPublicNodeInfo();
}

void StreamingFromMegaDialog::on_bCopyLink_clicked()
{
    if (!streamURL.isEmpty())
    {
        QApplication::clipboard()->setText(streamURL);
        ((MegaApplication *)qApp)->showInfoMessage(tr("The link has been copied to the clipboard"));
    }
}

void StreamingFromMegaDialog::on_bClose_clicked()
{
    if (!selectedMegaNode)
    {
        close();
        return;
    }

    QPointer<StreamingFromMegaDialog> currentDialog = this;
    if (QMegaMessageBox::question(nullptr,
                             tr("Stream from MEGA"),
                             tr("Are you sure that you want to stop the streaming?"),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
    {
        if (currentDialog)
        {
            done(QDialog::Accepted);
        }
    }
}

void StreamingFromMegaDialog::on_bOpenDefault_clicked()
{
    if (streamURL.isEmpty())
    {
        return;
    }

    QFileInfo fi(streamURL);
    QString app = Platform::getDefaultOpenApp(fi.suffix());
    openStreamWithApp(app);
}

void StreamingFromMegaDialog::on_bOpenOther_clicked()
{
    QString defaultPath;

    Preferences *preferences = Preferences::instance();
    QString lastPath = preferences->lastCustomStreamingApp();
    QFileInfo lastFile(lastPath);
    if (!lastPath.size() || !lastFile.exists())
    {
        MegaApi::log(MegaApi::LOG_LEVEL_DEBUG, "Getting Applications path");
    #ifdef WIN32
        WCHAR buffer[MAX_PATH];
        if (SHGetFolderPath(0, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, buffer) == S_OK)
        {
            defaultPath = QString::fromUtf16((ushort *)buffer);
        }
    #else
        #ifdef __APPLE__
            #if QT_VERSION < 0x050000
                defaultPath = QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation);
            #else
                QStringList paths = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
                if (paths.size())
                {
                    defaultPath = paths.at(0);
                }
            #endif
        #else
            defaultPath = QString::fromUtf8("/usr/bin");
        #endif
    #endif
    }
    else
    {
        defaultPath = lastPath;
    }

    defaultPath = QDir::toNativeSeparators(defaultPath);
    MegaApi::log(MegaApi::LOG_LEVEL_DEBUG, QString::fromUtf8("Result: %1").arg(defaultPath).toUtf8().constData());

    QPointer<StreamingFromMegaDialog> dialog(this);
    QString path = QDir::toNativeSeparators(QFileDialog::getOpenFileName(0, tr("Choose application"), defaultPath));
    if (dialog && path.length() && !streamURL.isEmpty())
    {
        QFileInfo info(path);
        if (!info.exists())
        {
            return;
        }
        preferences->setLastCustomStreamingApp(path);
        openStreamWithApp(path);
    }
}

void StreamingFromMegaDialog::updateStreamingState()
{
    if(lastStreamSelection == LastStreamingSelection::FROM_PUBLIC_NODE)
    {
        requestPublicNodeInfo();
    }
    else if(lastStreamSelection == LastStreamingSelection::FROM_LOCAL_NODE)
    {
        MegaNode *node = megaApi->getNodeByHandle(selectedMegaNode->getHandle());
        updateFileInfoFromNode(node);
    }
}

bool StreamingFromMegaDialog::generateStreamURL()
{
    if (!selectedMegaNode)
    {
        return false;
    }

    char *link = megaApi->httpServerGetLocalLink(selectedMegaNode.get());
    if (!link)
    {
        QMegaMessageBox::warning(nullptr, tr("Error"), tr("Error generating streaming link"), QMessageBox::Ok);
        return false;
    }
    streamURL = QString::fromUtf8(link);
    delete [] link;
    return true;
}

void StreamingFromMegaDialog::onLinkInfoAvailable()
{
    if (selectedMegaNode)
    {
        QString name = QString::fromUtf8(selectedMegaNode->getName());
        if (!name.compare(QString::fromAscii("NO_KEY")) || !name.compare(QString::fromAscii("CRYPTO_ERROR")))
        {
            updateFileInfo(tr("Decryption error"), LinkStatus::WARNING);
            selectedMegaNode.reset();
            streamURL.clear();
        }
        else
        {
            updateFileInfo(name, LinkStatus::CORRECT);
            generateStreamURL();
        }
    }
}

void StreamingFromMegaDialog::openStreamWithApp(QString app)
{
    if (app.isEmpty())
    {
        QtConcurrent::run(QDesktopServices::openUrl, QUrl::fromEncoded(streamURL.toUtf8()));
        return;
    }

#ifndef __APPLE__
#ifdef _WIN32
    QString command = QString::fromUtf8("\"%1\" \"%2\"").arg(QDir::toNativeSeparators(app)).arg(streamURL);
#else
    QString command = QString::fromUtf8("%1 \"%2\"").arg(QDir::toNativeSeparators(app)).arg(streamURL);
#endif
    QProcess::startDetached(command);
#else
    QString args;
    args = QString::fromUtf8("-a ");
    args += QDir::toNativeSeparators(QString::fromUtf8("\"")+ app + QString::fromUtf8("\"")) + QString::fromAscii(" \"%1\"").arg(streamURL);
    QProcess::startDetached(QString::fromAscii("open ") + args);
#endif
}

void StreamingFromMegaDialog::showStreamingError()
{
    ui->toolButtonError->setVisible(true);
    ui->labelError->setVisible(true);
}

void StreamingFromMegaDialog::hideStreamingError()
{
    ui->toolButtonError->setVisible(false);
    ui->labelError->setVisible(false);
}

void StreamingFromMegaDialog::updateFileInfoFromNode(MegaNode *node)
{
    if (!node)
    {
        QMegaMessageBox::warning(nullptr, tr("Error"), tr("File not found"), QMessageBox::Ok);
        return;
    }
    lastStreamSelection = LastStreamingSelection::FROM_LOCAL_NODE;
    selectedMegaNode = std::unique_ptr<MegaNode>(node);
    updateFileInfo(QString::fromUtf8(selectedMegaNode->getName()), LinkStatus::CORRECT);
    generateStreamURL();
    hideStreamingError();
}

void StreamingFromMegaDialog::requestPublicNodeInfo()
{
    QString url{mPublicLink.trimmed()};
    if (url.isEmpty())
    {
        return;
    }
    megaApi->getPublicNode(url.toUtf8().constData(), delegateListener.get());
    hideStreamingError();
}

void StreamingFromMegaDialog::updateFileInfo(QString fileName, LinkStatus status)
{
    ui->lFileName->ensurePolished();
    ui->lFileName->setText(ui->lFileName->fontMetrics().elidedText(fileName,Qt::ElideMiddle,ui->lFileName->maximumWidth()));
    ui->lFileSize->setText(Utilities::getSizeString(static_cast<unsigned long long>(selectedMegaNode->getSize())));

    QIcon typeIcon = Utilities::getExtensionPixmapMedium(fileName);

    ui->lFileType->setIcon(typeIcon);
    ui->lFileType->setIconSize(QSize(48, 48));

    QIcon statusIcon;
    switch (status)
    {
    case LinkStatus::LOADING:
        break;
    case LinkStatus::CORRECT:
        statusIcon.addFile(QString::fromUtf8(":/images/streaming_on_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->bOpenDefault->setEnabled(true);
        ui->bOpenOther->setEnabled(true);
        ui->bCopyLink->setEnabled(true);
        ui->bCopyLink->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                                       "stop: 0 rgba(246,247,250), stop: 1 rgba(232,233,235));"));
        break;
    case LinkStatus::TRANSFER_OVER_QUOTA:
        statusIcon.addFile(QString::fromUtf8(":/images/streaming_error_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->bOpenDefault->setEnabled(true);
        ui->bOpenOther->setEnabled(true);
        ui->bCopyLink->setEnabled(true);
        ui->bCopyLink->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                                       "stop: 0 rgba(246,247,250), stop: 1 rgba(232,233,235));"));
        break;
    default:
        statusIcon.addFile(QString::fromUtf8(":/images/streaming_off_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->bOpenDefault->setEnabled(false);
        ui->bOpenOther->setEnabled(false);
        ui->bCopyLink->setEnabled(false);
        ui->bCopyLink->setStyleSheet(QString());
        break;
    }

    ui->lState->setIcon(statusIcon);
    ui->lState->setIconSize(QSize(8, 8));
    ui->sFileInfo->setCurrentWidget(ui->pFileInfo);
}

void StreamingFromMegaDialog::onRequestFinish(MegaApi *, MegaRequest *request, MegaError *e)
{
    const bool isPublicNode{request->getType() == MegaRequest::TYPE_GET_PUBLIC_NODE};
    if(isPublicNode)
    {
        if (e->getErrorCode() == MegaError::API_OK)
        {
            selectedMegaNode = std::unique_ptr<MegaNode>(request->getPublicMegaNode());
            onLinkInfoAvailable();
        }
        else
        {
            QMegaMessageBox::warning(nullptr, tr("Error"), tr("Error getting link information"), QMessageBox::Ok);
        }
    }
}

void StreamingFromMegaDialog::onTransferTemporaryError(mega::MegaApi*, mega::MegaTransfer* transfer, mega::MegaError* e)
{
    const bool errorIsOverQuota{e->getErrorCode() == MegaError::API_EOVERQUOTA};
    if(transfer->isStreamingTransfer() && errorIsOverQuota)
    {
        updateFileInfo(QString::fromUtf8(selectedMegaNode->getName()), LinkStatus::TRANSFER_OVER_QUOTA);
        showStreamingError();

        show();
        raise();
        activateWindow();
    }
}
