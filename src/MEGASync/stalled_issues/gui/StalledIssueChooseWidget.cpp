#include "StalledIssueChooseWidget.h"
#include "ui_StalledIssueChooseWidget.h"

#include "Utilities.h"
#include "PlatformStrings.h"
#include "MegaApplication.h"
#include "StalledIssueHeader.h"
#include "StalledIssuesModel.h"

const int StalledIssueChooseWidget::BUTTON_ID = 0;

StalledIssueChooseWidget::StalledIssueChooseWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::StalledIssueChooseWidget),
    mPathDisableEffect(nullptr)
{
    ui->setupUi(this);

    ui->name->removeBackgroundColor();

    connect(MegaSyncApp->getStalledIssuesModel(), &StalledIssuesModel::showRawInfoChanged, this, &StalledIssueChooseWidget::onRawInfoToggled);

    ui->path->setIndent(StalledIssueHeader::GROUPBOX_CONTENTS_INDENT);
    ui->path->hideLocalOrRemoteTitle();

    ui->chooseTitle->addActionButton(QIcon(), tr("Choose"), BUTTON_ID, true);
    connect(ui->chooseTitle, &StalledIssueActionTitle::actionClicked, this, &StalledIssueChooseWidget::onActionClicked);
}

StalledIssueChooseWidget::~StalledIssueChooseWidget()
{
    delete ui;
}

void StalledIssueChooseWidget::setActionButtonVisibility(bool state)
{
    ui->chooseTitle->setActionButtonVisibility(BUTTON_ID, state);
}

void StalledIssueChooseWidget::onActionClicked(int button_id)
{
    QApplication::postEvent(this, new QMouseEvent(QEvent::MouseButtonPress, QPointF(), Qt::LeftButton, Qt::NoButton, Qt::KeyboardModifier::AltModifier));
    qApp->processEvents();

    emit chooseButtonClicked(button_id);
}

void StalledIssueChooseWidget::setSolved(bool isSolved, bool isSelected)
{
    if (isSolved)
    {
        ui->chooseTitle->setDisable(!isSelected);
        ui->name->setDisable(!isSelected);

        if (!isSelected && !ui->pathContainer->graphicsEffect())
        {
            mPathDisableEffect = new QGraphicsOpacityEffect(this);
            mPathDisableEffect->setOpacity(0.3);
            ui->pathContainer->setGraphicsEffect(mPathDisableEffect);
        }
    }
    else
    {
        ui->chooseTitle->setDisable(false);
        ui->name->setDisable(false);

        ui->pathContainer->setGraphicsEffect(nullptr);
    }
}

//Generic options
QString GenericChooseWidget::solvedString() const
{
    return mInfo.solvedText;
}

void GenericChooseWidget::setSolved(bool isSolved, bool isSelected)
{
    if(isSelected)
    {
        QIcon solvedIcon(QString::fromUtf8(":/images/StalledIssues/check_default.png"));
        ui->chooseTitle->setMessage(mInfo.solvedText, solvedIcon.pixmap(16,16));
    }
    else
    {
        ui->chooseTitle->setMessage(QString());
    }

    StalledIssueChooseWidget::setSolved(isSolved, isSelected);
    setActionButtonVisibility(!isSolved);
}

void GenericChooseWidget::setInfo(const GenericInfo &info)
{
    mInfo = info;

    ui->pathContainer->hide();
    ui->nameContainer->hide();

    auto margins(ui->titleContainer->layout()->contentsMargins());
    margins.setTop(0);
    margins.setBottom(0);
    ui->titleContainer->layout()->setContentsMargins(margins);
    ui->chooseTitle->removeBackgroundColor();

    QIcon icon(info.icon);
    auto iconPixmap(icon.pixmap(QSize(16,16)));
    ui->chooseTitle->setHTML(info.title, iconPixmap);
    ui->chooseTitle->setActionButtonInfo(QIcon(), info.buttonText, BUTTON_ID);
}
