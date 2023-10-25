// System
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// QML common
import Components.Buttons 1.0 as MegaButtons
import Components.Texts 1.0 as MegaTexts
import Components.TextFields 1.0 as MegaTextFields
import Common 1.0

// Local
import Onboard 1.0

// C++
import Onboarding 1.0
import LoginController 1.0
import ApiEnums 1.0

StackViewPage {
    id: root

    property alias signUpButton: signUpButtonItem
    property alias loginButton: loginButtonItem
    property alias email: emailItem
    property alias password: passwordItem

    Column {
        anchors.verticalCenter: root.verticalCenter
        anchors.left: root.left
        anchors.right: root.right
        spacing: contentSpacing

        MegaTexts.RichText {
            anchors.left: parent.left
            anchors.right: parent.right
            font.pixelSize: MegaTexts.Text.Size.Large
            rawText: loginControllerAccess.newAccount
                     ? OnboardingStrings.confirmEmailAndPassword
                     : OnboardingStrings.loginTitle
        }

        MegaTexts.RichText {
            visible: loginControllerAccess.newAccount
            anchors.left: parent.left
            anchors.right: parent.right
            font.pixelSize: MegaTexts.Text.Size.Medium
            rawText: OnboardingStrings.accountWillBeActivated
        }

        MegaTextFields.EmailTextField {
            id: emailItem

            width: parent.width + 2 * emailItem.sizes.focusBorderWidth
            anchors.left: parent.left
            anchors.leftMargin: -emailItem.sizes.focusBorderWidth
            title: OnboardingStrings.email
            text: loginControllerAccess.email
            error: loginControllerAccess.emailError
            hint.text: loginControllerAccess.emailErrorMsg
            hint.visible: loginControllerAccess.emailErrorMsg.length !== 0
        }

        MegaTextFields.PasswordTextField {
            id: passwordItem

            width: parent.width + 2 * passwordItem.sizes.focusBorderWidth
            anchors.left: parent.left
            anchors.leftMargin: -passwordItem.sizes.focusBorderWidth
            title: OnboardingStrings.password
            hint.icon: Images.alertTriangle
            error: loginControllerAccess.passwordError
            hint.text: loginControllerAccess.passwordErrorMsg
            hint.visible: loginControllerAccess.passwordErrorMsg.length !== 0
        }

        MegaButtons.HelpButton {
            anchors.left: parent.left
            text: OnboardingStrings.forgotPassword
            url: Links.recovery
            visible: !loginControllerAccess.newAccount
        }
    }

    RowLayout {
        anchors.right: root.right
        anchors.bottom: root.bottom
        anchors.bottomMargin: 29
        anchors.left: root.left

        MegaButtons.OutlineButton {
            id: signUpButtonItem

            text: OnboardingStrings.signUp
            Layout.alignment: Qt.AlignLeft
            Layout.leftMargin: -signUpButtonItem.sizes.focusBorderWidth
            visible: !loginControllerAccess.newAccount
        }

        MegaButtons.PrimaryButton {
            id: loginButtonItem

            text: loginControllerAccess.newAccount ? OnboardingStrings.next : OnboardingStrings.login
            Layout.alignment: Qt.AlignRight
            progressValue: loginControllerAccess.progress
            Layout.rightMargin: -loginButtonItem.sizes.focusBorderWidth//TODO: poner flecha
            icons.source: loginControllerAccess.newAccount ? Images.arrowRight : Images.none
        }
    }
}
