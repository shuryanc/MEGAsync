#ifndef STALLEDISSUESMODEL_H
#define STALLEDISSUESMODEL_H

#include "QTMegaRequestListener.h"
#include "QTMegaGlobalListener.h"
#include "StalledIssue.h"
#include "StalledIssuesUtilities.h"

#include <QObject>
#include <QMutex>
#include <QAbstractItemModel>
#include <QTimer>

class StalledIssuesReceiver : public QObject, public mega::MegaRequestListener
{
    Q_OBJECT
public:
    struct StalledIssuesReceived
    {
        StalledIssuesVariantList stalledIssues;

        bool isEmpty(){return stalledIssues.isEmpty();}
        void clear()
        {
            stalledIssues.clear();
        }
    };

    explicit StalledIssuesReceiver(QObject *parent = nullptr);
    ~StalledIssuesReceiver(){}

signals:
    void stalledIssuesReady(StalledIssuesReceiver::StalledIssuesReceived);

protected:
    void onRequestFinish(::mega::MegaApi*, ::mega::MegaRequest *request, ::mega::MegaError*);

private:
    QMutex mCacheMutex;
    StalledIssuesReceived mCacheStalledIssues;
};

Q_DECLARE_METATYPE(StalledIssuesReceiver::StalledIssuesReceived);

class StalledIssuesModel : public QAbstractItemModel, public mega::MegaGlobalListener
{
    Q_OBJECT

public:
    static const int ADAPTATIVE_HEIGHT_ROLE;

    explicit StalledIssuesModel(QObject* parent = 0);
    ~StalledIssuesModel();

    virtual Qt::DropActions supportedDropActions() const override;
    bool hasChildren(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool isEmpty() const;

    int getCountByFilterCriterion(StalledIssueFilterCriterion criterion);

    void finishStalledIssues(const QModelIndexList& indexes);
    void updateStalledIssues();
    void updateStalledIssuesWhenReady();;

    void lockModelMutex(bool lock);

    void blockUi();
    void unBlockUi();

    void updateIndex(const QModelIndex& index);

    StalledIssuesVariantList getIssuesByReason(QList<mega::MegaSyncStall::SyncStallReason> reasons);
    StalledIssuesVariantList getIssues(std::function<bool (const std::shared_ptr<const StalledIssue>)> checker);

    //SOLVE PROBLEMS
    //Name conflicts
    bool solveLocalConflictedNameByRemove(int conflictIndex, const QModelIndex& index);
    bool solveLocalConflictedNameByRename(const QString& renameTo, int conflictIndex, const QModelIndex& index);

    bool solveCloudConflictedNameByRemove(int conflictIndex, const QModelIndex& index);
    bool solveCloudConflictedNameByRename(const QString &renameTo, int conflictIndex, const QModelIndex& index);

    void semiAutoSolveNameConflictIssues(const QModelIndexList& list, int option);

    //LocalOrRemoteConflicts
    void chooseSideManually(bool remote, const QModelIndexList& list);
    void semiAutoSolveLocalRemoteIssues(const QModelIndexList& list);

    //IgnoreConflicts
    void ignoreItems(const QModelIndexList& list);

signals:
    void stalledIssuesChanged();
    void stalledIssuesCountChanged();

    void uiBlocked();
    void uiUnblocked();

protected slots:
    void onGlobalSyncStateChanged(mega::MegaApi *api) override;

private slots:
    void onProcessStalledIssues(StalledIssuesReceiver::StalledIssuesReceived issuesReceived);

private:
    void removeRows(QModelIndexList &indexesToRemove);
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    void updateStalledIssuedByOrder();
    void reset();
    QModelIndex getSolveIssueIndex(const QModelIndex& index);
    void quitReceiverThread();
    
    StalledIssuesModel(const StalledIssuesModel&) = delete;
    void operator=(const StalledIssuesModel&) = delete;
    

    QThread* mStalledIssuesThread;
    StalledIssuesReceiver* mStalledIssuedReceiver;
    std::atomic_bool mThreadFinished { false };
    mega::QTMegaRequestListener* mRequestListener;
    mega::QTMegaGlobalListener* mGlobalListener;
    mega::MegaApi* mMegaApi;
    bool mUpdateWhenGlobalStateChanges;
    bool mIssuesRequested;
    StalledIssuesUtilities mUtilities;
    QStringList ignoredItems;

    mutable QMutex mModelMutex;

    mutable StalledIssuesVariantList mStalledIssues;
    mutable QHash<StalledIssueVariant*, int> mStalledIssuesByOrder;

    QHash<int, int> mCountByFilterCriterion;
};

#endif // STALLEDISSUESMODEL_H
