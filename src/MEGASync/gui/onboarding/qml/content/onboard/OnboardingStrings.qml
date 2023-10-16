pragma Singleton
import QtQuick 2.12

QtObject {

    readonly property string accountTypeFree: qsTr("Free")
    readonly property string accountTypeProI: qsTr("Pro I")
    readonly property string accountTypeProII: qsTr("Pro II")
    readonly property string accountTypeProIII: qsTr("Pro III")
    readonly property string accountTypeLite: qsTr("Pro Lite")
    readonly property string accountTypeBusiness: qsTr("Business")
    readonly property string accountTypeProFlexi: qsTr("Pro Flexi")
    readonly property string addFolder: qsTr("Add folder")
    readonly property string agreeTerms: qsTr("I agree with MEGA's [B][A]Terms of service[/A][/B]")
    readonly property string backUp: qsTr("Back up")
    readonly property string backup: qsTr("Backup")
    readonly property string backupButtonDescription: qsTr("Data from your device will automatically and consistently back up to MEGA in real-time.")
    readonly property string backupConfirm: qsTr("Backup: Confirm")
    readonly property string backupFolders: qsTr("Backup Folders")
    readonly property string backupSelectFolders: qsTr("Backup: Select folders")
    readonly property string backupTo: qsTr("Backup to:")
    readonly property string cancel: qsTr("Cancel")
    readonly property string choose: qsTr("Choose")
    readonly property string changeFolder: qsTr("Change folder")
    readonly property string changeEmailBodyText: qsTr("Enter the new email address and click Resend. We will then send the verification email to this new email address for you to activate your account.")
    readonly property string changeEmailTitle: qsTr("Change your email address")
    readonly property string deviceName: qsTr("Device name")
    readonly property string deviceNameDescription: qsTr("Add the name of your device.")
    readonly property string deviceNameTitle: qsTr("Add device name")
    readonly property string confirm: qsTr("Confirm")
    readonly property string confirmBackupErrorDuplicated: qsTr("There is already a folder with the same name in this backup")
    readonly property string confirmBackupErrorRemote: qsTr("A folder with the same name already exists on your backups")
    readonly property string confirmBackupFoldersTitle: qsTr("Confirm folders to back up")
    readonly property string confirmEmailTitle: qsTr("Account created")
    readonly property string confirmEmailBodyText: qsTr("To activate your account, you need to verify your email address. We've sent you an email with a confirmation link. Click on the link to verify your email address, then you will be able to log in.")
    readonly property string confirmEmailBodyText2: qsTr("If you don't receive the email within 1 hour, please [B][A]contact support[/A][/B].")
    readonly property string confirmEmailChangeText: qsTr("If you have misspelled your email address, [B][A]change it here[/A][/B].")
    readonly property string confirmPassword: qsTr("Confirm password")
    readonly property string done: qsTr("Done")
    readonly property string email: qsTr("Email")
    readonly property string errorConfirmPassword: qsTr("Please confirm your password")
    readonly property string errorEmptyPassword: qsTr("Enter your password")
    readonly property string errorName: qsTr("Please enter your name")
    readonly property string errorLastName: qsTr("Please enter your last name")
    readonly property string errorPasswordsMatch: qsTr("Passwords don't match. Check and try again.")
    readonly property string errorValidEmail: qsTr("Enter a valid email address")
    readonly property string finalStepBackup: qsTr("Your backup has been setup and selected data will automatically backup whenever the desktop app is running. You can view your syncs and their statuses under the Sync tab in Settings.")
    readonly property string finalStepBackupTitle: qsTr("Your backup is set up")
    readonly property string finalStepQuestion: qsTr("What else do you want do?")
    readonly property string finalStepSync: qsTr("Your sync has been set up and will automatically sync selected data whenever the MEGA Desktop App is running.")
    readonly property string finalStepSyncTitle: qsTr("Your sync has been set up")
    readonly property string firstName: qsTr("First name")
    readonly property string fullSyncButtonDescription: qsTr("Sync your entire MEGA with your local device.")
    readonly property string fullSync: qsTr("Full sync")
    readonly property string fullSyncDescription: qsTr("Sync your entire MEGA Cloud drive with a local device.")
    readonly property string canNotSyncPermissionError: qsTr("Folder can’t be synced as you don’t have permissions to create a new folder. To continue, select an existing folder.")
    readonly property string forgotPassword: qsTr("Forgot password?")
    readonly property string invalidLocalPath: qsTr("Select a local folder to sync.")
    readonly property string invalidRemotePath: qsTr("Select a MEGA folder to sync.")
    readonly property string couldNotCreateDirectory: qsTr("Couldn't create directory");
    readonly property string setUpOptions: qsTr("Setup options")
    readonly property string chooseInstallation: qsTr("Choose:")
    readonly property string welcomeToMEGA: qsTr("Welcome to MEGA")
    readonly property string lastName: qsTr("Last name")
    readonly property string login: qsTr("Log in")
    readonly property string loginTitle: qsTr("Log in to your [B]MEGA account[/B]")
    readonly property string next: qsTr("Next")
    readonly property string skip: qsTr("Skip")
    readonly property string viewInSettings: qsTr("View in Settings")
    readonly property string password: qsTr("Password")
    readonly property string previous: qsTr("Previous")
    readonly property string rename: qsTr("Rename")
    readonly property string resend: qsTr("Resend")
    readonly property string selectAll: qsTr("[B]Select all[/B]");
    readonly property string selectBackupFoldersTitle: qsTr("Select folders to back up")
    readonly property string selectBackupFoldersDescription: qsTr("Selected folders will automatically back up to the cloud when the desktop app is running.")
    readonly property string selectLocalFolder: qsTr("Select a local folder")
    readonly property string selectMEGAFolder: qsTr("Select a MEGA folder")
    readonly property string selectiveSyncButtonDescription: qsTr("Sync selected folders in your MEGA with a local device.")
    readonly property string selectiveSyncDescription: qsTr("Sync specific folders in your MEGA Cloud drive with a local device.")
    readonly property string selectiveSync: qsTr("Selective sync")
    readonly property string setUpMEGA: qsTr("Set up MEGA")
    readonly property string signUp: qsTr("Sign up")
    readonly property string signUpTitle: qsTr("Create your [B]MEGA account[/B]")
    readonly property string availableStorage: qsTr("Available storage:")
    readonly property string syncButtonDescription: qsTr("Sync your device with MEGA and any changes will automatically and instantly apply to MEGA and vice versa.")
    readonly property string syncTitle: qsTr("Choose sync type")
    readonly property string sync: qsTr("Sync")
    readonly property string syncChooseType: qsTr("Sync: Choose type")
    readonly property string syncSetUp: qsTr("Sync set up")
    readonly property string twoFANeedHelp: qsTr("Problem with two-factor authentication?")
    readonly property string twoFASubtitle: qsTr("Enter the 6-digit code generated by your authenticator app.")
    readonly property string twoFATitle: qsTr("Continue with [B]two factor authentication[/B]")
    readonly property string statusLogin: qsTr("Logging in...")
    readonly property string statusFetchNodes: qsTr("Fetching file list...")
    readonly property string statusSignUp: qsTr("Creating account...")
    readonly property string status2FA: qsTr("Validating 2FA code...")
    readonly property string statusWaitingForEmail: qsTr("Waiting for email confirmation...")
    readonly property string cancelLoginTitle: qsTr("Stop logging in?")
    readonly property string cancelLoginBodyText: qsTr("Closing this window will stop you logging in.")
    readonly property string cancelLoginPrimaryButton: qsTr("Stop Loggin in")
    readonly property string cancelLoginSecondaryButton: qsTr("Don’t stop")
    readonly property string cancelAccountCreationTitle: qsTr("Abort account creation?")
    readonly property string cancelAccountCreationBody: qsTr("Closing this window will cancel the sign up process.")
    readonly property string cancelAccountAcceptButton: qsTr("Abort account")
    readonly property string cancelAccountCancelButton: qsTr("Don’t abort")
    readonly property string passwordAtleast8Chars: qsTr("Password needs to be at least 8 characters")
    readonly property string passwordStrengthVeryWeak: qsTr("Password easily guessed")
    readonly property string passwordStrengthWeak: qsTr("Weak password")
    readonly property string passwordStrengthMedium: qsTr("Average password")
    readonly property string passwordStrengthGood: qsTr("Good password")
    readonly property string passwordStrengthStrong: qsTr("Strong password")
    readonly property string itsBetterToHave: qsTr("It’s better to have:")
    readonly property string upperAndLowerCase: qsTr("Upper and lower case letters")
    readonly property string numberOrSpecialChar: qsTr("At least one number or special character")
    readonly property string longerPassword: qsTr("A longer password")
    readonly property string minimum8Chars: qsTr("Enter a minimum of 8 characters.")
    readonly property string passwordEasilyGuessedError: qsTr("Your password is easily guessed. You need to make it stronger.")
    readonly property string passwordEasilyGuessed: qsTr("Your password could be easily guessed. Try making it stronger.")
    readonly property string finalPageButtonBackup: qsTr("Automatically update your files from your computers to MEGA cloud. Backup items in MEGA cloud can't be modified or deleted from MEGA cloud.")
    readonly property string finalPageButtonSelectiveSync: qsTr("Sync selected folders between your computer with MEGA cloud, any change from one side will apply to another side.")
    readonly property string finalPageButtonSync: qsTr("Sync your files between your computers with MEGA cloud, any change from one side will apply to another side.")
    readonly property string letsGetYouSetUp: qsTr("Let's get you set up")
    readonly property string confirmEmailAndPassword: qsTr("Confirm your email and password")
    readonly property string accountWillBeActivated: qsTr("Once confirmed, your account will be activated.")
    readonly property string errorEmptyEmail: qsTr("Enter your email address")
    readonly property string errorEmptyDeviceName: qsTr("Enter a device name")
    readonly property string errorDeviceNameLimit: qsTr("Names longer that 32 characters are not supported")
    readonly property string storageSpace: qsTr("Storage space:")
    readonly property string authFailed: qsTr("Authentication failed")
    readonly property string tryAgain: qsTr("Try again")
}
