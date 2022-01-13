QT       += network

SOURCES += $$PWD/SettingsDialog.cpp \
    $$PWD/BalloonToolTip.cpp \
    $$PWD/InfoDialog.cpp \
    $$PWD/OverQuotaDialog.cpp \
    $$PWD/SetupWizard.cpp \
    $$PWD/NodeSelector.cpp \
    $$PWD/FolderBinder.cpp \
    $$PWD/BindFolderDialog.cpp \
    $$PWD/UploadToMegaDialog.cpp \
    $$PWD/PasteMegaLinksDialog.cpp \
    $$PWD/ImportMegaLinksDialog.cpp \
    $$PWD/ImportListWidgetItem.cpp \
    $$PWD/CrashReportDialog.cpp \
    $$PWD/MultiQFileDialog.cpp \
    $$PWD/MegaProxyStyle.cpp \
    $$PWD/AccountDetailsDialog.cpp \
    $$PWD/DownloadFromMegaDialog.cpp \
    $$PWD/UsageWidget.cpp \
    $$PWD/QMegaModel.cpp \
    $$PWD/MegaItem.cpp \
    $$PWD/ChangeLogDialog.cpp \
    $$PWD/GuestWidget.cpp \
    $$PWD/StreamingFromMegaDialog.cpp \
    $$PWD/MegaProgressCustomDialog.cpp \
    $$PWD/ConfirmSSLexception.cpp \
    $$PWD/UpgradeDialog.cpp \
    $$PWD/PlanWidget.cpp \
    $$PWD/InfoWizard.cpp \
    $$PWD/TransferManager.cpp \
    $$PWD/TransfersWidget.cpp \
    $$PWD/QTransfersModel.cpp \
    $$PWD/QActiveTransfersModel.cpp \
    $$PWD/QFinishedTransfersModel.cpp \
    $$PWD/MegaTransferDelegate.cpp \
    $$PWD/MegaTransferView.cpp \
    $$PWD/QMegaMessageBox.cpp \
    $$PWD/TransfersStateInfoWidget.cpp \
    $$PWD/MegaSpeedGraph.cpp \
    $$PWD/ActiveTransfersWidget.cpp \
    $$PWD/AvatarWidget.cpp \
    $$PWD/MenuItemAction.cpp \
    $$PWD/AddExclusionDialog.cpp \
    $$PWD/TransferManagerItem.cpp \
    $$PWD/TransferItem.cpp \
    $$PWD/InfoDialogTransfersWidget.cpp \
    $$PWD/QCustomTransfersModel.cpp \
    $$PWD/StatusInfo.cpp \
    $$PWD/ChangePassword.cpp \
    $$PWD/CustomTransferItem.cpp \
    $$PWD/PSAwidget.cpp \
    $$PWD/ElidedLabel.cpp \
    $$PWD/UpgradeOverStorage.cpp \
    $$PWD/Login2FA.cpp \
    $$PWD/QRWidget.cpp \
    $$PWD/QSyncItemWidget.cpp \
    $$PWD/TransfersStatusWidget.cpp \
    $$PWD/TransfersSummaryWidget.cpp \
    $$PWD/CircularUsageProgressBar.cpp \
    $$PWD/HighDpiResize.cpp \
    $$PWD/AlertItem.cpp \
    $$PWD/QAlertsModel.cpp \
    $$PWD/MegaAlertDelegate.cpp \
    $$PWD/QFilterAlertsModel.cpp \
    $$PWD/FilterAlertWidget.cpp \
    $$PWD/AlertFilterType.cpp \
    $$PWD/BugReportDialog.cpp \
    $$PWD/VerifyLockMessage.cpp \
    $$PWD/MegaInfoMessage.cpp \
    $$PWD/WaitingSpinnerWidget.cpp \
    $$PWD/ProxySettings.cpp \
    $$PWD/BandwidthSettings.cpp

HEADERS  += $$PWD/SettingsDialog.h \
    $$PWD/BalloonToolTip.h \
    $$PWD/InfoDialog.h \
    $$PWD/OverQuotaDialog.h \
    $$PWD/SetupWizard.h \
    $$PWD/NodeSelector.h \
    $$PWD/FolderBinder.h \
    $$PWD/BindFolderDialog.h \
    $$PWD/UploadToMegaDialog.h \
    $$PWD/PasteMegaLinksDialog.h \
    $$PWD/ImportMegaLinksDialog.h \
    $$PWD/ImportListWidgetItem.h \
    $$PWD/CrashReportDialog.h \
    $$PWD/MultiQFileDialog.h \
    $$PWD/MegaProxyStyle.h \
    $$PWD/AccountDetailsDialog.h \
    $$PWD/DownloadFromMegaDialog.h \
    $$PWD/UsageWidget.h \
    $$PWD/QMegaModel.h \
    $$PWD/MegaItem.h \
    $$PWD/ChangeLogDialog.h \
    $$PWD/GuestWidget.h \
    $$PWD/StreamingFromMegaDialog.h \
    $$PWD/MegaProgressCustomDialog.h \
    $$PWD/ConfirmSSLexception.h \
    $$PWD/UpgradeDialog.h \
    $$PWD/PlanWidget.h \
    $$PWD/InfoWizard.h \
    $$PWD/TransferManager.h \
    $$PWD/TransfersWidget.h \
    $$PWD/QTransfersModel.h \
    $$PWD/QActiveTransfersModel.h \
    $$PWD/QFinishedTransfersModel.h \
    $$PWD/MegaTransferDelegate.h \
    $$PWD/MegaTransferView.h \
    $$PWD/QMegaMessageBox.h \
    $$PWD/TransfersStateInfoWidget.h \
    $$PWD/MegaSpeedGraph.h \
    $$PWD/ActiveTransfersWidget.h \
    $$PWD/AvatarWidget.h \
    $$PWD/MenuItemAction.h \
    $$PWD/AddExclusionDialog.h \
    $$PWD/TransferManagerItem.h \
    $$PWD/TransferItem.h \
    $$PWD/InfoDialogTransfersWidget.h \
    $$PWD/QCustomTransfersModel.h \
    $$PWD/StatusInfo.h \
    $$PWD/CustomTransferItem.h \
    $$PWD/PSAwidget.h \
    $$PWD/ElidedLabel.h \
    $$PWD/UpgradeOverStorage.h \
    $$PWD/ChangePassword.h \
    $$PWD/Login2FA.h \
    $$PWD/QRWidget.h \
    $$PWD/QSyncItemWidget.h \
    $$PWD/TransfersStatusWidget.h \
    $$PWD/TransfersSummaryWidget.h \
    $$PWD/CircularUsageProgressBar.h \
    $$PWD/HighDpiResize.h \
    $$PWD/AlertItem.h \
    $$PWD/QAlertsModel.h \
    $$PWD/MegaAlertDelegate.h \
    $$PWD/QFilterAlertsModel.h \
    $$PWD/FilterAlertWidget.h \
    $$PWD/AlertFilterType.h \
    $$PWD/BugReportDialog.h \
    $$PWD/VerifyLockMessage.h \
    $$PWD/MegaInfoMessage.h \
    $$PWD/WaitingSpinnerWidget.h \
    $$PWD/ProxySettings.h \
    $$PWD/BandwidthSettings.h

INCLUDEPATH += $$PWD

debug {
    DEFINES += SHOW_LOGS
}

win32 {
    RESOURCES += $$PWD/Resources_win.qrc
    INCLUDEPATH += $$PWD/win
    FORMS    += $$PWD/win/InfoDialog.ui \
                $$PWD/win/CustomTransferItem.ui \
                $$PWD/win/NodeSelector.ui \
                $$PWD/win/FolderBinder.ui \
                $$PWD/win/BindFolderDialog.ui \
                $$PWD/win/UploadToMegaDialog.ui \
                $$PWD/win/PasteMegaLinksDialog.ui \
                $$PWD/win/ImportMegaLinksDialog.ui \
                $$PWD/win/ImportListWidgetItem.ui \
                $$PWD/win/CrashReportDialog.ui \
                $$PWD/win/SetupWizard.ui \
                $$PWD/win/SettingsDialog.ui \
                $$PWD/win/AccountDetailsDialog.ui \
                $$PWD/win/DownloadFromMegaDialog.ui \
                $$PWD/win/ChangeLogDialog.ui \
                $$PWD/win/GuestWidget.ui \
                $$PWD/win/StreamingFromMegaDialog.ui \
                $$PWD/win/ConfirmSSLexception.ui \
                $$PWD/win/MegaProgressCustomDialog.ui \
                $$PWD/win/PlanWidget.ui \
                $$PWD/win/UpgradeDialog.ui \
                $$PWD/win/InfoWizard.ui \
                $$PWD/win/TransferManagerItem.ui \
                $$PWD/win/TransferManager.ui \
                $$PWD/win/TransfersWidget.ui \
                $$PWD/win/TransfersStateInfoWidget.ui \
                $$PWD/win/MegaSpeedGraph.ui \
                $$PWD/win/ActiveTransfersWidget.ui \
                $$PWD/win/AddExclusionDialog.ui \
                $$PWD/win/InfoDialogTransfersWidget.ui \
                $$PWD/win/StatusInfo.ui \
                $$PWD/win/PSAwidget.ui \
                $$PWD/win/UpgradeOverStorage.ui \
                $$PWD/win/ChangePassword.ui \
                $$PWD/win/Login2FA.ui \
                $$PWD/win/QSyncItemWidget.ui \
                $$PWD/win/TransfersStatusWidget.ui \
                $$PWD/win/AlertItem.ui \
                $$PWD/win/TransfersSummaryWidget.ui \
                $$PWD/win/FilterAlertWidget.ui \
                $$PWD/win/AlertFilterType.ui \
                $$PWD/win/BugReportDialog.ui \
                $$PWD/win/LockedPopOver.ui \
                $$PWD/win/VerifyLockMessage.ui \
                $$PWD/win/MegaInfoMessage.ui \
                $$PWD/win/OverQuotaDialog.ui \
                $$PWD/win/ProxySettings.ui \
                $$PWD/win/BandwidthSettings.ui \
                $$PWD/win/NewFolderDialog.ui
}

macx {
    RESOURCES += $$PWD/Resources_macx.qrc
    INCLUDEPATH += $$PWD/macx
    FORMS    += $$PWD/macx/InfoDialog.ui \
                $$PWD/macx/NodeSelector.ui \
                $$PWD/macx/FolderBinder.ui \
                $$PWD/macx/BindFolderDialog.ui \
                $$PWD/macx/UploadToMegaDialog.ui \
                $$PWD/macx/PasteMegaLinksDialog.ui \
                $$PWD/macx/ImportMegaLinksDialog.ui \
                $$PWD/macx/ImportListWidgetItem.ui \
                $$PWD/macx/CrashReportDialog.ui \
                $$PWD/macx/SetupWizard.ui \
                $$PWD/macx/SettingsDialog.ui \
                $$PWD/macx/AccountDetailsDialog.ui \
                $$PWD/macx/DownloadFromMegaDialog.ui \
                $$PWD/macx/ChangeLogDialog.ui \
                $$PWD/macx/GuestWidget.ui \
                $$PWD/macx/StreamingFromMegaDialog.ui \
                $$PWD/macx/PermissionsDialog.ui \
                $$PWD/macx/PermissionsWidget.ui \
                $$PWD/macx/MegaProgressCustomDialog.ui \
                $$PWD/macx/ConfirmSSLexception.ui \
                $$PWD/macx/PlanWidget.ui \
                $$PWD/macx/UpgradeDialog.ui \
                $$PWD/macx/InfoWizard.ui \
                $$PWD/macx/TransferManagerItem.ui \
                $$PWD/macx/TransferManager.ui \
                $$PWD/macx/TransfersWidget.ui \
                $$PWD/macx/TransfersStateInfoWidget.ui \
                $$PWD/macx/MegaSpeedGraph.ui \
                $$PWD/macx/ActiveTransfersWidget.ui \
                $$PWD/macx/AddExclusionDialog.ui \
                $$PWD/macx/InfoDialogTransfersWidget.ui \
                $$PWD/macx/StatusInfo.ui \
                $$PWD/macx/CustomTransferItem.ui \
                $$PWD/macx/PSAwidget.ui \
                $$PWD/macx/UpgradeOverStorage.ui \
                $$PWD/macx/ChangePassword.ui \
                $$PWD/macx/Login2FA.ui \
                $$PWD/macx/QSyncItemWidget.ui \
                $$PWD/macx/TransfersStatusWidget.ui \
                $$PWD/macx/AlertItem.ui \
                $$PWD/macx/TransfersSummaryWidget.ui \
                $$PWD/macx/FilterAlertWidget.ui \
                $$PWD/macx/AlertFilterType.ui \
                $$PWD/macx/BugReportDialog.ui \
                $$PWD/macx/LockedPopOver.ui \
                $$PWD/macx/VerifyLockMessage.ui \
                $$PWD/macx/MegaInfoMessage.ui \
                $$PWD/macx/OverQuotaDialog.ui \
                $$PWD/macx/ProxySettings.ui \
                $$PWD/macx/BandwidthSettings.ui \
                $$PWD/macx/NewFolderDialog.ui

    #Asset catalog need to load SF symbol images of toolbar items for custom NSToolbar
    QMAKE_ASSET_CATALOGS += $$PWD/images/Images.xcassets

    QT += macextras

    OBJECTIVE_SOURCES +=    $$PWD/CocoaHelpButton.mm \
                            $$PWD/MegaSystemTrayIcon.mm \
                            $$PWD/QMacSpinningProgressIndicator.mm \
                            $$PWD/QSegmentedControl.mm

    HEADERS += $$PWD/CocoaHelpButton.h $$PWD/MegaSystemTrayIcon.h $$PWD/QSegmentedControl.h

    HEADERS += $$PWD/PermissionsDialog.h \
               $$PWD/PermissionsWidget.h \
               $$PWD/QMacSpinningProgressIndicator.h
    SOURCES += $$PWD/PermissionsDialog.cpp \
               $$PWD/PermissionsWidget.cpp
}
 else {
    HEADERS += $$PWD/LockedPopOver.h
    SOURCES += $$PWD/LockedPopOver.cpp
}

unix:!macx {
    RESOURCES += $$PWD/Resources_linux.qrc
    INCLUDEPATH += $$PWD/linux
    FORMS    += $$PWD/linux/InfoDialog.ui \
                $$PWD/linux/CustomTransferItem.ui \
                $$PWD/linux/NodeSelector.ui \
                $$PWD/linux/FolderBinder.ui \
                $$PWD/linux/BindFolderDialog.ui \
                $$PWD/linux/UploadToMegaDialog.ui \
                $$PWD/linux/PasteMegaLinksDialog.ui \
                $$PWD/linux/ImportMegaLinksDialog.ui \
                $$PWD/linux/ImportListWidgetItem.ui \
                $$PWD/linux/CrashReportDialog.ui \
                $$PWD/linux/SetupWizard.ui \
                $$PWD/linux/SettingsDialog.ui \
                $$PWD/linux/AccountDetailsDialog.ui \
                $$PWD/linux/DownloadFromMegaDialog.ui \
                $$PWD/linux/ChangeLogDialog.ui \
                $$PWD/linux/GuestWidget.ui \
                $$PWD/linux/StreamingFromMegaDialog.ui \
                $$PWD/linux/PermissionsDialog.ui \
                $$PWD/linux/PermissionsWidget.ui \
                $$PWD/linux/MegaProgressCustomDialog.ui \
                $$PWD/linux/ConfirmSSLexception.ui \
                $$PWD/linux/PlanWidget.ui \
                $$PWD/linux/UpgradeDialog.ui \
                $$PWD/linux/InfoWizard.ui \
                $$PWD/linux/TransferManagerItem.ui \
                $$PWD/linux/TransferManager.ui \
                $$PWD/linux/TransfersWidget.ui \
                $$PWD/linux/TransfersStateInfoWidget.ui \
                $$PWD/linux/MegaSpeedGraph.ui \
                $$PWD/linux/ActiveTransfersWidget.ui \
                $$PWD/linux/AddExclusionDialog.ui \
                $$PWD/linux/InfoDialogTransfersWidget.ui \
                $$PWD/linux/StatusInfo.ui \
                $$PWD/linux/PSAwidget.ui \
                $$PWD/linux/UpgradeOverStorage.ui \
                $$PWD/linux/ChangePassword.ui \
                $$PWD/linux/Login2FA.ui \
                $$PWD/linux/QSyncItemWidget.ui \
                $$PWD/linux/TransfersStatusWidget.ui \
                $$PWD/linux/AlertItem.ui \
                $$PWD/linux/TransfersSummaryWidget.ui \
                $$PWD/linux/FilterAlertWidget.ui \
                $$PWD/linux/AlertFilterType.ui \
                $$PWD/linux/BugReportDialog.ui \
                $$PWD/linux/LockedPopOver.ui \
                $$PWD/linux/VerifyLockMessage.ui \
                $$PWD/linux/MegaInfoMessage.ui \
                $$PWD/linux/OverQuotaDialog.ui \
                $$PWD/linux/ProxySettings.ui \
                $$PWD/linux/BandwidthSettings.ui \
                $$PWD/linux/NewFolderDialog.ui

    HEADERS += $$PWD/PermissionsDialog.h \
               $$PWD/PermissionsWidget.h
    SOURCES += $$PWD/PermissionsDialog.cpp \
               $$PWD/PermissionsWidget.cpp
}
