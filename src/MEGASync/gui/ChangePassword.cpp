#include "ChangePassword.h"
#include "ui_ChangePassword.h"
#include "MegaApplication.h"
#include "QMegaMessageBox.h"
#include "gui/Login2FA.h"

using namespace mega;

ChangePassword::ChangePassword(QWidget* parent) :
    QDialog(parent),
    mUi(new Ui::ChangePassword),
    mMegaApi (((MegaApplication*)qApp)->getMegaApi()),
    mDelegateListener (new QTMegaRequestListener(mMegaApi, this))
{
    mUi->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    mUi->bOk->setDefault(true);
}

QString ChangePassword::newPassword()
{
    return mUi->lNewPassword->text();
}

QString ChangePassword::confirmNewPassword()
{
    return mUi->lConfirmNewPassword->text();
}

void ChangePassword::onRequestFinish(mega::MegaApi* api, mega::MegaRequest* req, mega::MegaError* e)
{
    Q_UNUSED (api)
    switch(req->getType())
    {
        case MegaRequest::TYPE_MULTI_FACTOR_AUTH_CHECK:
        {
            if (e->getErrorCode() == MegaError::API_OK)
            {
                if (req->getFlag()) //2FA enabled
                {
                    QPointer<ChangePassword> dialog = this;
                    QPointer<Login2FA> verification = new Login2FA(this);
                    int result = verification->exec();
                    if (!dialog || !verification || result != QDialog::Accepted)
                    {
                        if (dialog)
                        {
                            mUi->bOk->setEnabled(true);
                        }
                        delete verification;
                        return;
                    }

                    QString pin = verification->pinCode();
                    delete verification;

                    mMegaApi->multiFactorAuthChangePassword(nullptr,
                                                            newPassword().toUtf8().constData(),
                                                            pin.toUtf8().constData(),
                                                            mDelegateListener);
                }
                else
                {
                    mMegaApi->changePassword(nullptr,
                                             newPassword().toUtf8().constData(),
                                             mDelegateListener);
                }
            }
            else
            {
                QMegaMessageBox::critical(this, tr("Error"),
                                          QCoreApplication::translate("MegaError",
                                                                      e->getErrorString()));
            }
            break;
        }
        case MegaRequest::TYPE_CHANGE_PW:
        {
            if (e->getErrorCode() == MegaError::API_OK)
            {
                mUi->bOk->setEnabled(true);
                accept();
            }
            else if (e->getErrorCode() == MegaError::API_EFAILED
                     || e->getErrorCode() == MegaError::API_EEXPIRED)
            {
                QPointer<ChangePassword> dialog = this;
                QPointer<Login2FA> verification = new Login2FA(this);
                verification->invalidCode(true);
                int result = verification->exec();
                if (!dialog || !verification || result != QDialog::Accepted)
                {
                    if (dialog)
                    {
                        mUi->bOk->setEnabled(true);
                    }
                    delete verification;
                    return;
                }

                QString pin = verification->pinCode();
                delete verification;

                mMegaApi->multiFactorAuthChangePassword(nullptr,
                                                        newPassword().toUtf8().constData(),
                                                        pin.toUtf8().constData(),
                                                        mDelegateListener);
            }
            else if (e->getErrorCode() == MegaError::API_ETOOMANY)
            {
                mUi->bOk->setEnabled(true);
                QMegaMessageBox::critical(nullptr, tr("Error"),
                                          tr("Too many requests. Please wait."));
            }
            else
            {
                mUi->bOk->setEnabled(true);
                QMegaMessageBox::critical(this, tr("Error"),
                                          QCoreApplication::translate("MegaError",
                                                                      e->getErrorString()));
            }
            break;
        }
    }
}

ChangePassword::~ChangePassword()
{
    delete mUi;
    delete mDelegateListener;
}

void ChangePassword::on_bOk_clicked()
{
    const bool fieldIsEmpty{newPassword().isEmpty() || confirmNewPassword().isEmpty()};
    const bool passwordsAreEqual{!newPassword().compare(confirmNewPassword())};
    const bool newAndOldPasswordsAreTheSame{mMegaApi->checkPassword(newPassword().toUtf8())};
    const bool passwordIsWeak{mMegaApi->getPasswordStrength(newPassword().toUtf8().constData())
                == MegaApi::PASSWORD_STRENGTH_VERYWEAK};

    if (fieldIsEmpty)
    {
        QMegaMessageBox::warning(this, tr("Error"), tr("Please enter your password"));
        return;
    }
    else if (!passwordsAreEqual)
    {
        QMegaMessageBox::warning(this, tr("Error"), tr("The entered passwords don't match"));
        return;
    }
    else if (newAndOldPasswordsAreTheSame)
    {
        QMegaMessageBox::warning(this, tr("Error"), tr("You have entered your current password,"
                                                       " please enter a new password."));
        return;
    }
    else if (passwordIsWeak)
    {
        QMegaMessageBox::warning(this, tr("Error"), tr("Please, enter a stronger password"));
        return;
    }

    mUi->bOk->setEnabled(false);

    char* email = mMegaApi->getMyEmail();
    if (email)
    {
        mMegaApi->multiFactorAuthCheck(email, mDelegateListener);
        delete [] email;
    }
    else
    {
        mMegaApi->multiFactorAuthCheck(Preferences::instance()->email().toUtf8().constData(),
                                       mDelegateListener);
    }
}

void ChangePassword::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        mUi->retranslateUi(this);
    }
    QDialog::changeEvent(event);
}
