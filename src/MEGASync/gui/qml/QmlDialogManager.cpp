#include "QmlDialogManager.h"

#include "QmlDialogWrapper.h"
#include "Backups.h"
#include "Onboarding.h"
#include "GuestContent.h"
#include "OnboardingQmlDialog.h"
#include "GuestQmlDialog.h"
#include "WhatsNewWindow.h"
#include "DialogOpener.h"
#include "LoginController.h"
#include "AccountStatusController.h"
#include "SyncsComponent.h"

std::shared_ptr<QmlDialogManager> QmlDialogManager::instance()
{
    static std::shared_ptr<QmlDialogManager> manager(new QmlDialogManager());
    return manager;
}

void QmlDialogManager::openGuestDialog()
{
    if(MegaSyncApp->finished())
    {
        return;
    }

    auto dialogWrapper = DialogOpener::findDialog<QmlDialogWrapper<GuestContent>>();
    if(dialogWrapper == nullptr)
    {
        QPointer<QmlDialogWrapper<GuestContent>> guest = new QmlDialogWrapper<GuestContent>();
        DialogOpener::addDialog(guest)->setIgnoreRaiseAllAction(true);
        dialogWrapper = DialogOpener::findDialog<QmlDialogWrapper<GuestContent>>();
    }

    DialogOpener::showDialog(dialogWrapper->getDialog());
}

bool QmlDialogManager::openOnboardingDialog()
{
    if(MegaSyncApp->finished() || Preferences::instance()->logged())
    {
        return false;
    }

    if(auto dialog = DialogOpener::findDialog<QmlDialogWrapper<Onboarding>>())
    {
        DialogOpener::showDialog(dialog->getDialog());
    }
    else
    {
        QPointer<QmlDialogWrapper<Onboarding>> onboarding = new QmlDialogWrapper<Onboarding>();
        DialogOpener::showDialog(onboarding)->setIgnoreCloseAllAction(true);
    }
    return true;
}

bool QmlDialogManager::raiseGuestDialog()
{
    bool raisedGuestDialog = false;
    if(MegaSyncApp->getAccountStatusController()->isAccountBlocked()
        || MegaSyncApp->getLoginController()->getState() != LoginController::FETCH_NODES_FINISHED)
    {
        openGuestDialog();
        raisedGuestDialog = true;
    }
    return raisedGuestDialog;
}

void QmlDialogManager::raiseOnboardingDialog()
{
    if(MegaSyncApp->getAccountStatusController()->isAccountBlocked()
        || (MegaSyncApp->getLoginController()
                && MegaSyncApp->getLoginController()->getState() != LoginController::FETCH_NODES_FINISHED))
    {
        if (Preferences::instance()->getSession().isEmpty())
        {
            openOnboardingDialog();
        }
        else if(auto dialog = DialogOpener::findDialog<QmlDialogWrapper<Onboarding>>())
        {
            DialogOpener::showDialog(dialog->getDialog());
            dialog->getDialog()->raise();
        }
    }
}

void QmlDialogManager::raiseOrHideInfoGuestDialog(QTimer* dialogTimer, int msec)
{
    if(!dialogTimer || msec < 0)
    {
        return;
    }

    auto guestDialogWrapper = DialogOpener::findDialog<QmlDialogWrapper<GuestContent>>();
    if(guestDialogWrapper == nullptr) //dialog still not built
    {
        dialogTimer->start(msec);
        return;
    }

    auto dialog = dynamic_cast<GuestQmlDialog*>(guestDialogWrapper->getDialog()->window());
    if(dialog && dialog->isHiddenForLongTime())
    {
        dialogTimer->start(msec);
    }
}

void QmlDialogManager::forceCloseOnboardingDialog()
{
    if(auto dialog = DialogOpener::findDialog<QmlDialogWrapper<Onboarding>>())
    {
        static_cast<OnboardingQmlDialog*>(dialog->getDialog()->window())->forceClose();
    }
}

bool QmlDialogManager::openWhatsNewDialog()
{
    if(MegaSyncApp->finished())
    {
        return false;
    }

    if(auto dialog = DialogOpener::findDialog<QmlDialogWrapper<WhatsNewWindow>>())
    {
        DialogOpener::showDialog(dialog->getDialog());
        dialog->getDialog()->raise();
    }
    else
    {
        QPointer<QmlDialogWrapper<WhatsNewWindow>> whatsNew = new QmlDialogWrapper<WhatsNewWindow>();
        DialogOpener::showDialog(whatsNew);
        whatsNew->raise();
    }
    return true;
}

void QmlDialogManager::openAddSync(const QString& remoteFolder, bool fromSettings)
{
    auto overQuotaDialog = MegaSyncApp->showSyncOverquotaDialog();
    auto addSyncLambda = [overQuotaDialog, fromSettings, remoteFolder]()
    {
        if (!overQuotaDialog || overQuotaDialog->result() == QDialog::Rejected)
        {
            QPointer<QmlDialogWrapper<SyncsComponent>> syncsDialog;
            if (auto dialog = DialogOpener::findDialog<QmlDialogWrapper<SyncsComponent>>())
            {
                syncsDialog = dialog->getDialog();
            }
            else
            {
                syncsDialog = new QmlDialogWrapper<SyncsComponent>();
            }

            syncsDialog->wrapper()->setComesFromSettings(fromSettings);
            syncsDialog->wrapper()->setRemoteFolder(remoteFolder);
            DialogOpener::showDialog(syncsDialog);
        }
    };

    if (overQuotaDialog)
    {
        DialogOpener::showDialog(overQuotaDialog, addSyncLambda);
    }
    else
    {
        addSyncLambda();
    }
}

