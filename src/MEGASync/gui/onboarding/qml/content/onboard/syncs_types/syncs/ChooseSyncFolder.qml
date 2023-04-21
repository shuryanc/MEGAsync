// System
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3

// QML common
import Common 1.0
import Components 1.0 as Custom
import ChooseLocalFolder 1.0
import ChooseRemoteFolder 1.0

// Local
import Onboard 1.0

RowLayout {

    property bool local: true
    property url selectedUrl: selectedUrl
    property double selectedNode: selectedNode
    property bool isValid: false


    function getSyncData() {
        return local ? localFolderChooser.getFolder() : remoteFolderChooser.getHandle();
    }

    function folderSelectionChanged(folder)
    {
        isValid = folder.length;
        folderField.text = isValid ? folder : "/MEGA";
    }

    function reset()
    {
        local ? localFolderChooser.reset() : remoteFolderChooser.reset();
    }

    width: parent.width
    spacing: 8

    Custom.TextField {
        id: folderField

        Layout.preferredWidth: parent.width
        Layout.leftMargin: -folderField.textField.focusBorderWidth
        title: local ? OnboardingStrings.selectLocalFolder : OnboardingStrings.selectMEGAFolder
        text: "/MEGA"
        leftIcon.source: local ? Images.pc : Images.mega
    }

    Custom.Button {
        Layout.alignment: Qt.AlignBottom
        Layout.preferredHeight: folderField.textFieldRawHeight
        Layout.bottomMargin: folderField.textField.focusBorderWidth
        text: OnboardingStrings.choose
        onClicked: {
            var folderChooser = local ? localFolderChooser : remoteFolderChooser;
            folderChooser.openFolderSelector();
        }
    }

    ChooseLocalFolder {
        id: localFolderChooser
        onFolderChanged: {
            folderSelectionChanged(folder);
        }
    }

    ChooseRemoteFolder {
        id: remoteFolderChooser
        onFolderChanged: {
            folderSelectionChanged(folder);
        }
    }
    
    
}
