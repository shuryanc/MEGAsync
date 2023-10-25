// System
import QtQuick 2.15
import QtQuick.Window 2.15

// Local
import Onboarding 1.0

// C++
import LoginController 1.0

ConfirmEmailPageForm {
    id: confirmEmailPage

    changeEmailLinkText.onLinkActivated: {
        loginControllerAccess.state = LoginController.CHANGING_REGISTER_EMAIL;
    }

    Connections {
        target: loginControllerAccess

        function onEmailConfirmed() {
            // The following four lines are required by Ubuntu to bring the window to the front and
            // move it to the center
            onboardingWindow.hide();
            onboardingWindow.show();
            onboardingWindow.x = Screen.width / 2 - onboardingWindow.width / 2;
            onboardingWindow.y = Screen.height / 2 - onboardingWindow.height / 2;

            // The following two lines are required by Windows (activate) and macOS (raise)
            onboardingWindow.requestActivate();
            onboardingWindow.raise();
        }
    }
}
