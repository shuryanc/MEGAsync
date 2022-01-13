#ifndef TRANSFERMANAGER_H
#define TRANSFERMANAGER_H

#include <QDialog>
#include <QMenu>
#include "megaapi.h"
#include "Preferences.h"
#include "MenuItemAction.h"
#include <QGraphicsEffect>
#include <QTimer>
#include "QTMegaTransferListener.h"
#include "Utilities.h"

namespace Ui {
class TransferManager;
}

class TransferManager : public QDialog, public mega::MegaTransferListener
{
    Q_OBJECT

public:
    enum {
        ALL_TRANSFERS_TAB = 0,
        DOWNLOADS_TAB   = 1,
        UPLOADS_TAB = 2,
        COMPLETED_TAB = 3
    };

    static const int COMPLETED_ITEMS_LIMIT = 999;

    explicit TransferManager(mega::MegaApi *megaApi, QWidget *parent = 0);
    void setActiveTab(int t);
    void updatePauseState();
    void disableGetLink(bool disable);
    void updateNumberOfCompletedTransfers(int num);
    ~TransferManager();

    virtual void onTransferStart(mega::MegaApi *api, mega::MegaTransfer *transfer);
    virtual void onTransferFinish(mega::MegaApi* api, mega::MegaTransfer *transfer, mega::MegaError* e);
    virtual void onTransferUpdate(mega::MegaApi *api, mega::MegaTransfer *transfer);
    virtual void onTransferTemporaryError(mega::MegaApi *api, mega::MegaTransfer *transfer, mega::MegaError* e);

signals:
    void viewedCompletedTransfers();
    void completedTransfersTabActive(bool);
    void userActivity();

private:
    Ui::TransferManager *ui;
    mega::MegaApi *megaApi;
    QMenu *addMenu;
    MenuItemAction *settingsAction;
    MenuItemAction *importLinksAction;
    MenuItemAction *uploadAction;
    MenuItemAction *downloadAction;
    Preferences *preferences;
    QPoint dragPosition;
    long long notificationNumber;
    QTimer *refreshTransferTime;
    ThreadPool* mThreadPool;

    void createAddMenu();
    void onTransfersActive(bool exists);  

public slots:
    void updateState();

private slots:
    void on_tCompleted_clicked();
    void on_tDownloads_clicked();
    void on_tUploads_clicked();
    void on_tAllTransfers_clicked();
    void on_bAdd_clicked();
    void on_bClose_clicked();
    void on_bPause_clicked();
    void on_bClearAll_clicked();

    void refreshFinishedTime();

protected:
    void changeEvent(QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // TRANSFERMANAGER_H
