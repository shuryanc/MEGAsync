// System
import QtQuick 2.12
import QtQuick.Controls 2.12

// Local
import Onboard 1.0
import Onboard.Syncs_types.Left_panel 1.0

// C++
import BackupsProxyModel 1.0

Item {
    id: root

    signal backupFlowMoveToFinal
    signal backupFlowMoveToBack

    readonly property string selectBackup: "selectBackup"
    readonly property string confirmBackup: "confirmBackup"

    state: selectBackup
    states: [
        State {
            name: selectBackup
            StateChangeScript {
                script: view.replace(selectBackupFoldersPage);
            }
            PropertyChanges {
                target: stepPanel;
                state: stepPanel.step3;
                step3Text: OnboardingStrings.backupSelectFolders;
                step4Text: OnboardingStrings.confirm;
            }
        },
        State {
            name: confirmBackup
            StateChangeScript {
                script: view.replace(confirmBackupFoldersPage);
            }
            PropertyChanges {
                target: stepPanel;
                state: stepPanel.step4;
                step3Text: OnboardingStrings.backupSelectFolders;
                step4Text: OnboardingStrings.backupConfirm;
            }
        }
    ]

    StackView {
        id: view
        anchors.fill: parent

        replaceEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to:1
                duration: 100
                easing.type: Easing.OutQuad
            }
        }
        replaceExit: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to:0
                duration: 100
                easing.type: Easing.InQuad
            }
        }
    }

    BackupsProxyModel {
        id: backupsProxyModel
    }

    Component {
        id: selectBackupFoldersPage

        SelectFoldersPage {}
    }

    Component {
        id: confirmBackupFoldersPage

        ConfirmFoldersPage {}
    }

    /*
    * Navigation connections
    */
    Connections {
        id: confirmFolderBackupNavigationConnection
        target: view.currentItem
        ignoreUnknownSignals: true

        function onConfirmFoldersMoveToSelect() {
            root.state = root.selectBackup
        }

        function onConfirmFoldersMoveToSuccess() {
            root.backupFlowMoveToFinal()
        }
    }

    Connections {
        id: selectFolderBackupNavigationConnection
        target: view.currentItem
        ignoreUnknownSignals: true

        function onSelectFolderMoveToBack() {
            if(syncsPanel.navInfo.comesFromResumePage) {
                syncsPanel.navInfo.typeSelected = syncsPanel.navInfo.previousTypeSelected;
                root.backupFlowMoveToFinal()
            } else {
                root.backupFlowMoveToBack()
            }
        }

        function onSelectFolderMoveToConfirm() {
            root.state = root.confirmBackup
        }
    }
}
