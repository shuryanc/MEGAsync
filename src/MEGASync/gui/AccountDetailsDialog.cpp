#include "AccountDetailsDialog.h"
#include "ui_AccountDetailsDialog.h"

#include "control/Utilities.h"
#include "MegaApplication.h"
#include "TransferQuota.h"

#include <QStyle>

using namespace mega;

// Precision to use for progressbars (100 for %, 1000 for ppm...)
static constexpr int PRECISION{100};
static constexpr int DEFAULT_MIN_PERCENTAGE{1};

AccountDetailsDialog::AccountDetailsDialog(QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::AccountDetailsDialog)
{
    // Setup UI
    mUi->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Set progressbars precision
    mUi->pbCloudDrive->setMaximum(PRECISION);
    mUi->pbInbox->setMaximum(PRECISION);
    mUi->pbRubbish->setMaximum(PRECISION);

    // Set transfer quota progress bar color to blue
    mUi->wCircularTransfer->setProgressBarGradient(QColor(96, 209, 254), QColor(88, 185, 243));

    // Get fresh data
    refresh(Preferences::instance());

    // Init HiDPI
    mHighDpiResize.init(this);

    // Subscribe to data updates (but detach after 1 callback)
    MegaSyncApp->attachStorageObserver(*this);
}

AccountDetailsDialog::~AccountDetailsDialog()
{
    MegaSyncApp->dettachStorageObserver(*this);
    delete mUi;
}

void AccountDetailsDialog::refresh(Preferences* preferences)
{
    // Get account type
    auto accType(preferences->accountType());

    // Check if we have valid data. If not, tell the UI.
    if (preferences->totalStorage() == 0)
    {
        // We don't have data, so enable loading property
        setProperty("loading", true);
    }
    else
    {
        // We have data, so disable loading property
        setProperty("loading", false);

        // Separator between used and total.
        QString sep(QLatin1String(" / "));

        // ---------- Process storage usage

        // Get useful data
        auto totalStorage(preferences->totalStorage());
        auto usedStorage(preferences->usedStorage());

        if (accType == Preferences::ACCOUNT_TYPE_BUSINESS)
        {
            // Set unused fields to 0
            mUi->wCircularStorage->setValue(0);
            mUi->lTotalTransfer->setText(QString());

            // Disable over quota and warning
            mUi->wCircularStorage->setState(CircularUsageProgressBar::STATE_OK);
            setProperty("storageState", QLatin1String("ok"));
        }
        else
        {
            // Check storage state and set property accordingly
            switch (preferences->getStorageState())
            {
                case MegaApi::STORAGE_STATE_UNKNOWN:
                // Fallthrough
                case MegaApi::STORAGE_STATE_GREEN:
                {
                    mUi->wCircularStorage->setState(CircularUsageProgressBar::STATE_OK);
                    setProperty("storageState", QLatin1String("ok"));
                    break;
                }
                case MegaApi::STORAGE_STATE_ORANGE:
                {
                    mUi->wCircularStorage->setState(CircularUsageProgressBar::STATE_WARNING);
                    setProperty("storageState", QLatin1String("warning"));
                    break;
                }
                case MegaApi::STORAGE_STATE_PAYWALL:
                // Fallthrough
                case MegaApi::STORAGE_STATE_RED:
                {
                    mUi->wCircularStorage->setState(CircularUsageProgressBar::STATE_OVER);
                    setProperty("storageState", QLatin1String("full"));
                    break;
                }
            }

            auto usedStoragePercentage (usedStorage ?
                                std::max(Utilities::partPer(usedStorage, totalStorage),
                                         DEFAULT_MIN_PERCENTAGE)
                              : 0);
            mUi->wCircularStorage->setValue(usedStoragePercentage);
            mUi->lTotalStorage->setText(sep + Utilities::getSizeString(totalStorage));
        }

        mUi->lUsedStorage->setText(Utilities::getSizeString(usedStorage));

        // ---------- Process transfer usage

        // Get useful data
        auto totalTransfer(preferences->totalBandwidth());
        auto usedTransfer(preferences->usedBandwidth());
        auto transferQuotaState(MegaSyncApp->getTransferQuotaState());

        // Set UI according to state
        switch (transferQuotaState) {
            case QuotaState::OK:
            {
                setProperty("transferState", QLatin1String("ok"));
                mUi->wCircularTransfer->setState(CircularUsageProgressBar::STATE_OK);
                break;
            }
            case QuotaState::WARNING:
            {
                setProperty("transferState", QLatin1String("warning"));
                mUi->wCircularTransfer->setState(CircularUsageProgressBar::STATE_WARNING);
                break;
            }
            case QuotaState::OVERQUOTA:
            // Fallthrough
            case QuotaState::FULL:
            {
                setProperty("transferState", QLatin1String("full"));
                mUi->wCircularTransfer->setState(CircularUsageProgressBar::STATE_OVER);
                break;
            }
        }

        // Set progress bar
        switch (accType)
        {
            case Preferences::ACCOUNT_TYPE_BUSINESS:
            {
                setProperty("accountType", QLatin1String("business"));
                mUi->wCircularStorage->setValue(0);
                break;
            }
            case Preferences::ACCOUNT_TYPE_FREE:
            {
                setProperty("accountType", QLatin1String("free"));
                mUi->wCircularTransfer->setTotalValueUnknown(transferQuotaState != QuotaState::FULL
                                                        && transferQuotaState != QuotaState::OVERQUOTA);
                break;
            }
            case Preferences::ACCOUNT_TYPE_LITE:
            // Fallthrough
            case Preferences::ACCOUNT_TYPE_PROI:
            // Fallthrough
            case Preferences::ACCOUNT_TYPE_PROII:
            // Fallthrough
            case Preferences::ACCOUNT_TYPE_PROIII:
            {
                setProperty("accountType", QLatin1String("pro"));

                auto usedQuotaPercentage (usedTransfer ?
                                    std::max(Utilities::partPer(usedTransfer, totalTransfer, PRECISION),
                                             DEFAULT_MIN_PERCENTAGE)
                                  : 0);
                mUi->wCircularTransfer->setValue(usedQuotaPercentage);
                mUi->lTotalTransfer->setText(sep + Utilities::getSizeString(totalTransfer));
                break;
            }
        }

        mUi->lUsedTransfer->setText(Utilities::getSizeString(usedTransfer));

        // ---------- Process detailed storage usage

        // ---- Cloud drive storage
        auto usedCloudDriveStorage = preferences->cloudDriveStorage();
        auto parts (usedCloudDriveStorage ?
                        std::max(Utilities::partPer(usedCloudDriveStorage, totalStorage, PRECISION),
                                 DEFAULT_MIN_PERCENTAGE)
                      : 0);
        mUi->pbCloudDrive->setValue(std::min(PRECISION, parts));

        mUi->lUsedCloudDrive->setText(Utilities::getSizeString(usedStorage));

        // ---- Inbox usage
        auto usedInboxStorage = preferences->inboxStorage();
        parts = usedInboxStorage ?
                    std::max(Utilities::partPer(usedInboxStorage, totalStorage, PRECISION),
                             DEFAULT_MIN_PERCENTAGE)
                  : 0;
        mUi->pbInbox->setValue(std::min(PRECISION, parts));

        // Display only if not empty. Resize dialog to adequate height.
        if (usedInboxStorage > 0)
        {
            mUi->lUsedInbox->setText(Utilities::getSizeString(usedInboxStorage));
            mUi->wInbox->setVisible(true);
        }
        else
        {
            mUi->wInbox->setVisible(false);
        }

        // ---- Rubbish bin usage
        auto usedRubbishStorage = preferences->rubbishStorage();
        parts = usedRubbishStorage ?
                    std::max(Utilities::partPer(usedRubbishStorage, totalStorage, PRECISION),
                             DEFAULT_MIN_PERCENTAGE)
                  : 0;
        mUi->pbRubbish->setValue(std::min(PRECISION, parts));

        mUi->lUsedRubbish->setText(Utilities::getSizeString(usedRubbishStorage));

        // ---- Versions usage
        mUi->lUsedByVersions->setText(Utilities::getSizeString(preferences->versionsStorage()));

        // ---------- Refresh display
        updateGeometry();
        style()->unpolish(this);
        style()->polish(this);
        update();
    }
}

void AccountDetailsDialog::updateStorageElements()
{
    // Prevent other updates of these fields (due to events) after the first one
    MegaSyncApp->dettachStorageObserver(*this);

    refresh(Preferences::instance());
}
