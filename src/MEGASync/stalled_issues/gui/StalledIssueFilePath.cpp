#include "StalledIssueFilePath.h"
#include "ui_StalledIssueFilePath.h"

#include "Utilities.h"
#include "Platform.h"
#include "QMegaMessageBox.h"

#include <QPainter>
#include <QPoint>

static const char* ITS_HOVER = "ItsHover";
static const char* HAS_PROBLEM = "hasProblem";

StalledIssueFilePath::StalledIssueFilePath(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StalledIssueFilePath),
    mShowFullPath(false)
{
    ui->setupUi(this);

    ui->filePathAction->hide();
    ui->moveFilePathAction->hide();

    ui->file->hide();
    ui->moveFile->hide();

    ui->lines->installEventFilter(this);
    ui->moveLines->installEventFilter(this);
    ui->movePathProblemLines->installEventFilter(this);

    ui->filePathContainer->installEventFilter(this);
    ui->moveFilePathContainer->installEventFilter(this);

    auto openIcon = Utilities::getCachedPixmap(QLatin1Literal(":/images/StalledIssues/ic-open-outside.png"));
    ui->filePathAction->setPixmap(openIcon.pixmap(ui->filePathAction->size()));
    ui->moveFilePathAction->setPixmap(openIcon.pixmap(ui->moveFilePathAction->size()));

    connect(ui->helpIcon, &QPushButton::clicked, this, &StalledIssueFilePath::onHelpIconClicked);
}

StalledIssueFilePath::~StalledIssueFilePath()
{
    delete ui;
}

void StalledIssueFilePath::setIndent(int indent)
{
    ui->indent->changeSize(indent,0,QSizePolicy::Fixed, QSizePolicy::Preferred);
    ui->gridLayout->invalidate();
}

void StalledIssueFilePath::updateUi(StalledIssueDataPtr data)
{
    mData = data;

    if(mData->isCloud())
    {
        auto remoteIcon = Utilities::getCachedPixmap(QLatin1Literal(":/images/StalledIssues/cloud_default.png"));
        ui->LocalOrRemoteIcon->setPixmap(remoteIcon.pixmap(QSize(16,16)));

        ui->LocalOrRemoteText->setText(tr("on MEGA:"));
    }
    else
    {
        auto localIcon = Utilities::getCachedPixmap(QLatin1Literal(":/images/StalledIssues/monitor_default.png"));
        ui->LocalOrRemoteIcon->setPixmap(localIcon.pixmap(QSize(16,16)));

        ui->LocalOrRemoteText->setText(tr("Local:"));
    }

    fillFilePath();
    fillMoveFilePath();
    updateFileIcons();
    updateMoveFileIcons();
}

void StalledIssueFilePath::showFullPath()
{
    mShowFullPath = true;
}

void StalledIssueFilePath::hideLocalOrRemoteTitle()
{
    ui->LocalOrRemoteIcon->hide();
    ui->LocalOrRemoteText->hide();
}

void StalledIssueFilePath::fillFilePath()
{
    if(!mData->getPath().isEmpty())
    {
        ui->file->show();

        auto filePath = getFilePath();
        if(!filePath.isEmpty())
        {
            ui->filePath->installEventFilter(this);
            ui->filePath->setText(filePath);
        }
        else
        {
            ui->filePath->setText(QString::fromUtf8("-"));
        }

        auto hasProblem(mData->getPath().mPathProblem != mega::MegaSyncStall::SyncPathProblem::NoProblem);

        if(hasProblem)
        {
            ui->pathProblemMessage->setText(getSyncPathProblemString(mData->getPath().mPathProblem));
            ui->filePathContainer->setCursor(Qt::ArrowCursor);
            auto helpLink = getHelpLink(mData->getPath().mPathProblem);
            helpLink.isEmpty() ? ui->helpIcon->hide() : ui->helpIcon->show();
        }
        else
        {
            ui->pathProblemContainer->hide();
            ui->filePathContainer->setCursor(Qt::PointingHandCursor);
            ui->helpIcon->hide();
        }

        ui->filePathContainer->setProperty(HAS_PROBLEM,hasProblem);
        setStyleSheet(styleSheet());
    }
}

QString StalledIssueFilePath::getFilePath()
{
    auto filePath = mShowFullPath? mData->getNativeFilePath() : mData->getNativePath();
    mData->checkTrailingSpaces(filePath);
    return filePath;
}

void StalledIssueFilePath::fillMoveFilePath()
{
    if(!mData->getMovePath().isEmpty())
    {
        ui->moveFile->show();

        auto filePath = getMoveFilePath();
        if(!filePath.isEmpty())
        {
            ui->moveFilePath->installEventFilter(this);
            ui->moveFilePath->setText(filePath);
        }
        else
        {
            ui->moveFilePath->setText(QString::fromUtf8("-"));
        }

        auto hasProblem(mData->getMovePath().mPathProblem != mega::MegaSyncStall::SyncPathProblem::NoProblem);
        hasProblem ?  ui->movePathProblemMessage->setText(getSyncPathProblemString(mData->getMovePath().mPathProblem)) : ui->movePathProblemContainer->hide();
        hasProblem ? ui->moveFilePathContainer->setCursor(Qt::ArrowCursor) : ui->moveFilePathContainer->setCursor(Qt::PointingHandCursor);
        ui->moveFilePathContainer->setProperty(HAS_PROBLEM,hasProblem);
        setStyleSheet(styleSheet());
    }
}

QString StalledIssueFilePath::getMoveFilePath()
{
    return mShowFullPath? mData->getNativeMoveFilePath() : mData->getNativeMovePath();
}

void StalledIssueFilePath::updateFileIcons()
{
    QIcon fileTypeIcon;
    QSize iconSize(ui->filePathIcon->size());

    QFileInfo fileInfo(getFilePath());
    auto hasProblem(mData->getPath().mPathProblem != mega::MegaSyncStall::SyncPathProblem::NoProblem);
    if(mData->isCloud())
    {
        std::unique_ptr<mega::MegaNode> node(MegaSyncApp->getMegaApi()->getNodeByPath(fileInfo.filePath().toUtf8().constData()));
        fileTypeIcon = StalledIssuesUtilities::getRemoteFileIcon(node.get(), fileInfo, hasProblem);
        if(!node)
        {
            iconSize = QSize(16,16);
        }
    }
    else
    {
        fileTypeIcon = StalledIssuesUtilities::getLocalFileIcon(fileInfo, hasProblem);
    }

    ui->filePathIcon->setPixmap(fileTypeIcon.pixmap(iconSize));
}

void StalledIssueFilePath::updateMoveFileIcons()
{
    QIcon fileTypeIcon;
    QSize iconSize(ui->moveFilePathIcon->size());

    QFileInfo fileInfo(getMoveFilePath());
    auto hasProblem(mData->getMovePath().mPathProblem != mega::MegaSyncStall::SyncPathProblem::NoProblem);

    if(mData->isCloud())
    {
        std::unique_ptr<mega::MegaNode> node(MegaSyncApp->getMegaApi()->getNodeByPath(fileInfo.filePath().toUtf8().constData()));
        fileTypeIcon = StalledIssuesUtilities::getRemoteFileIcon(node.get(), fileInfo, hasProblem);
        if(!node)
        {
            iconSize = QSize(16,16);
        }
    }
    else
    {
        fileTypeIcon = StalledIssuesUtilities::getLocalFileIcon(fileInfo, hasProblem);
    }

    ui->moveFilePathIcon->setPixmap(fileTypeIcon.pixmap(iconSize));
}

bool StalledIssueFilePath::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Enter || event->type() == QEvent::Leave || event->type() == QEvent::MouseButtonRelease)
    {
        if(watched == ui->filePathContainer && !ui->filePathContainer->property(HAS_PROBLEM).toBool())
        {
            showHoverAction(event->type(), ui->filePathAction, getFilePath());
        }
        else if(watched == ui->moveFilePathContainer && !ui->moveFilePathContainer->property(HAS_PROBLEM).toBool())
        {
            showHoverAction(event->type(), ui->moveFilePathAction,  getMoveFilePath());
        }
    }
    else if(event->type() == QEvent::Resize)
    {
        if(mData)
        {
            if(watched == ui->filePath || watched == ui->moveFilePath)
            {
                auto label = dynamic_cast<QLabel*>(watched);
                if(label)
                {
                    QString fullPath;

                    if(watched == ui->filePath)
                    {
                        fullPath = getFilePath();
                    }
                    else if(watched == ui->moveFilePath)
                    {
                        fullPath = getMoveFilePath();
                    }

                    if(!fullPath.isEmpty())
                    {
                        label->setText(label->fontMetrics().elidedText(fullPath, Qt::ElideMiddle,label->width()));
                    }
                }
            }
            else if(watched == ui->lines)
            {
                auto hasProblem(mData->getPath().mPathProblem != mega::MegaSyncStall::SyncPathProblem::NoProblem);
                if(hasProblem)
                {
                    auto fileTypeIcon = Utilities::getCachedPixmap(QLatin1Literal(":/images/StalledIssues/tree_link_end_default.png"));
                    ui->lines->setPixmap(fileTypeIcon.pixmap(ui->lines->size()));
                }
                else
                {
                    auto fileTypeIcon = Utilities::getCachedPixmap(QLatin1Literal(":/images/StalledIssues/tree_end_default.png"));
                    ui->lines->setPixmap(fileTypeIcon.pixmap(ui->lines->size()));
                }
            }
            else if(watched == ui->movePathProblemLines)
            {
                auto hasProblem(mData->getMovePath().mPathProblem != mega::MegaSyncStall::SyncPathProblem::NoProblem);
                if(hasProblem)
                {
                    auto fileTypeIcon = Utilities::getCachedPixmap(QLatin1Literal(":/images/StalledIssues/tree_double_link_default.png"));
                    //TODO avoid using a fixed size
                    ui->movePathProblemLines->setPixmap(fileTypeIcon.pixmap(QSize(24,24)));
                }
                else
                {
                    ui->movePathProblemLines->setPixmap(QPixmap());
                }
            }
            else if(watched == ui->moveLines)
            {
                auto fileTypeIcon = Utilities::getCachedPixmap(QLatin1Literal(":/images/StalledIssues/tree_link_default.png"));
                ui->moveLines->setPixmap(fileTypeIcon.pixmap(ui->moveLines->size()));
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void StalledIssueFilePath::onHelpIconClicked()
{
    auto helpLink = QUrl(getHelpLink(mData->getPath().mPathProblem));
    Utilities::openUrl(helpLink);
}

void StalledIssueFilePath::showHoverAction(QEvent::Type type, QWidget *actionWidget, const QString& path)
{
    if(type == QEvent::Enter)
    {
        actionWidget->show();
        actionWidget->parent()->setProperty(ITS_HOVER, true);
        setStyleSheet(styleSheet());
    }
    else if(type == QEvent::Leave)
    {
        actionWidget->hide();
        actionWidget->parent()->setProperty(ITS_HOVER, false);
        setStyleSheet(styleSheet());
    }
    else if(type == QEvent::MouseButtonRelease)
    {
        if(mData->isCloud())
        {
            mega::MegaNode* node (MegaSyncApp->getMegaApi()->getNodeByPath(path.toUtf8().constData()));
            if (node)
            {
                const char* handle = node->getBase64Handle();
                QString url = QString::fromUtf8("mega://#fm/") + QString::fromUtf8(handle);
                QtConcurrent::run(QDesktopServices::openUrl, QUrl(url));
                delete [] handle;
                delete node;
            }
            else
            {
                QMegaMessageBox::warning(nullptr, QString::fromUtf8("MEGAsync"), QString::fromUtf8("Node %1 does not exist.").arg(path));
            }
        }
        else
        {
            QFile file(path);
            if(file.exists())
            {
                QtConcurrent::run([=]
                {
                    Platform::getInstance()->showInFolder(path);
                });
            }
            else
            {
                QMegaMessageBox::warning(nullptr, QString::fromUtf8("MEGAsync"), QString::fromUtf8("Path %1 does not exist.").arg(path));
            }
        }
    }
}

QString StalledIssueFilePath::getSyncPathProblemString(mega::MegaSyncStall::SyncPathProblem pathProblem)
{
    switch(pathProblem)
    {
        case mega::MegaSyncStall::FileChangingFrequently:
        {
            return tr("File is being frequently changing.");
        }
        case mega::MegaSyncStall::IgnoreRulesUnknown:
        {
            return tr("Ignore rules unknown.");
        }
        case mega::MegaSyncStall::DetectedHardLink:
        {
            return tr("Hard link or Reparse Point detected.");
        }
        case mega::MegaSyncStall::DetectedSymlink:
        {
            return tr("Detected Sym link.");
        }
        case mega::MegaSyncStall::DetectedSpecialFile:
        {
            return tr("Detected special file.");
        }
        case mega::MegaSyncStall::DifferentFileOrFolderIsAlreadyPresent:
        {
            return tr("Different file or folder is already present.");
        }
        case mega::MegaSyncStall::ParentFolderDoesNotExist:
        {
            return tr("Parent folder does not exist.");
        }
        case mega::MegaSyncStall::FilesystemErrorDuringOperation:
        {
            return tr("Filesystem error during operation.");
        }
        case mega::MegaSyncStall::NameTooLongForFilesystem:
        {
            return tr("Name too long for filesystem.");
        }
        case mega::MegaSyncStall::CannotFingerprintFile:
        {
            return tr("Cannot fingerprint file.");
        }
        case mega::MegaSyncStall::DestinationPathInUnresolvedArea:
        {
            return tr("Destination path is in an unresolved area.");
        }
        case mega::MegaSyncStall::MACVerificationFailure:
        {
            return tr("MAC verification failure.");
        }
        case mega::MegaSyncStall::DeletedOrMovedByUser:
        {
            return tr("Deleted or moved by user.");
        }
        case mega::MegaSyncStall::FileFolderDeletedByUser:
        {
            return tr("Deleted by user.");
        }
        case mega::MegaSyncStall::MoveToDebrisFolderFailed:
        {
            return tr("Move to debris folder failed.");
        }
        case mega::MegaSyncStall::IgnoreFileMalformed:
        {
            return tr("Ignore file malformed.");
        }
        case mega::MegaSyncStall::FilesystemErrorListingFolder:
        {
            return tr("Error Listing folder in filesystem.");
        }
        case mega::MegaSyncStall::FilesystemErrorIdentifyingFolderContent:
        {
            return tr("Error identifying folder content in filesystem.");
        }
        case mega::MegaSyncStall::UndecryptedCloudNode:
        {
            return tr("Cloud node undecrypted.");
        }
        case mega::MegaSyncStall::WaitingForScanningToComplete:
        {
            return tr("Waiting for scanning to complete.");
        }
        case mega::MegaSyncStall::WaitingForAnotherMoveToComplete:
        {
            return tr("Waiting for another move to complete.");
        }
        case mega::MegaSyncStall::SourceWasMovedElsewhere:
        {
            return tr("Source was moved elsewhere.");
        }
        case mega::MegaSyncStall::FilesystemCannotStoreThisName:
        {
            return tr("Local filesystem cannot store this name.");
        }
        case mega::MegaSyncStall::CloudNodeInvalidFingerprint:
        {
            return tr("Fingerprint is missing or invalid.");
        }
        case mega::MegaSyncStall::SyncPathProblem_LastPlusOne:
        {
            break;
        }
    }
    return tr("Error not detected");
}

QString StalledIssueFilePath::getHelpLink(mega::MegaSyncStall::SyncPathProblem pathProblem)
{
    switch(pathProblem)
    {
        case mega::MegaSyncStall::FilesystemCannotStoreThisName:
        {
            return tr("https://help.mega.io/");
        }
    }

    return QString();
}
