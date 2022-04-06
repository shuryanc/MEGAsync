#include "StalledIssuesModel.h"

#include "MegaApplication.h"

StalledIssuesReceiver::StalledIssuesReceiver(QObject *parent) : QObject(parent), mega::MegaRequestListener()
{

}

QList<QExplicitlySharedDataPointer<StalledIssueData>> StalledIssuesReceiver::processStalledIssues()
{
    if(mCacheMutex.tryLock())
    {
        if(mCacheStalledIssues.size() > 2000)
        {
            StalledIssuesDataList auxList;
            for(auto index = 0; index < 2000
                && !mCacheStalledIssues.isEmpty(); ++index)
            {
                auto& firstItem = mCacheStalledIssues.first();
                if(firstItem)
                {
                    auxList.append(firstItem);
                    mCacheStalledIssues.removeOne(firstItem);
                }
            }

            mCacheMutex.unlock();
            return auxList;
        }
        else
        {
            auto auxList = mCacheStalledIssues;
            mCacheStalledIssues.clear();

            mCacheMutex.unlock();
            return auxList;
        }
    }

    return StalledIssuesDataList();
}

void StalledIssuesReceiver::onRequestFinish(mega::MegaApi*, mega::MegaRequest *request, mega::MegaError*)
{
    if (auto ptr = request->getMegaSyncProblems())
    {
        QMutexLocker lock(&mCacheMutex);

        if (mega::MegaSyncNameConflictList* cl = ptr->nameConflicts())
        {
            for (int i = 0; i < cl->size(); ++i)
            {
                auto nameConflictStall = cl->get(i);

                auto conflictNameItem = new ConflictedNamesStalledIssueData(nameConflictStall);
                StalledIssueDataPtr d (conflictNameItem);
                mCacheStalledIssues.append(d);
            }
        }

        if (mega::MegaSyncStallList* sl = ptr->stalls())
        {
            for (int i = 0; i < sl->size(); ++i)
            {
                auto stall = sl->get(i);

                StalledIssueDataPtr d (new StalledIssueData(stall));
                mCacheStalledIssues.append(d);
            }
        }
    }
}

StalledIssuesModel::StalledIssuesModel(QObject *parent) : QAbstractItemModel(parent),
     mMegaApi (MegaSyncApp->getMegaApi())
{
    mStalledIssuesThread = new QThread();
    mStalledIssuedReceiver = new StalledIssuesReceiver();
    mDelegateListener = new mega::QTMegaRequestListener(mMegaApi, mStalledIssuedReceiver);
    mStalledIssuedReceiver->moveToThread(mStalledIssuesThread);
    mDelegateListener->moveToThread(mStalledIssuesThread);
    mMegaApi->addRequestListener(mDelegateListener);

    mStalledIssuesTimer.setInterval(100);
    QObject::connect(&mStalledIssuesTimer, &QTimer::timeout, this, &StalledIssuesModel::onProcessStalledIssues);
    mStalledIssuesTimer.start();

    mStalledIssuesThread->start();
}

StalledIssuesModel::~StalledIssuesModel()
{
    mMegaApi->removeRequestListener(mDelegateListener);
    mStalledIssuesThread->quit();
    mStalledIssuesThread->deleteLater();
    mStalledIssuedReceiver->deleteLater();

    mDelegateListener->deleteLater();
}

void StalledIssuesModel::onProcessStalledIssues()
{
    auto stalledIssues = mStalledIssuedReceiver->processStalledIssues();
    if(!stalledIssues.isEmpty())
    {
        foreach(auto issue, stalledIssues)
        {
            if(mAddedStalledIssues.contains(issue->mFileName))
            {
                stalledIssues.removeOne(issue);
            }
        }

        if(!stalledIssues.isEmpty())
        {
            auto totalRows = rowCount(QModelIndex());
            auto rowsToBeInserted(static_cast<int>(stalledIssues.size()));

            beginInsertRows(QModelIndex(), totalRows, totalRows + rowsToBeInserted - 1);

            for (auto it = stalledIssues.begin(); it != stalledIssues.end();)
            {
                mStalledIssues.append((*it));
                mStalledIssuesByOrder.insert((*it), rowCount(QModelIndex()) - 1);
                mAddedStalledIssues.append((*it)->mFileName);

                stalledIssues.removeOne((*it));
                it++;
            }

            endInsertRows();

            emit stalledIssuesReceived(true);
        }
    }
    else
    {
        if (mMegaApi->isSyncStalled())
        {
            mMegaApi->getSyncProblems(nullptr, true);
        }
    }
}

Qt::DropActions StalledIssuesModel::supportedDropActions() const
{
    return Qt::IgnoreAction;
}

bool StalledIssuesModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.parent().isValid())
    {
        return true;
    }

    return false;
}

int StalledIssuesModel::rowCount(const QModelIndex &parent) const
{
   if(!parent.isValid())
   {
       return mStalledIssues.size();
   }
   else
   {
       return 1;
   }

   return 0;
}

int StalledIssuesModel::columnCount(const QModelIndex &parent) const
{
   return 1;
}

QVariant StalledIssuesModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if(index.parent().isValid())
        {
            return QVariant::fromValue(StalledIssue(mStalledIssues.at(index.parent().row())));
        }
        else
        {
            return QVariant::fromValue(StalledIssue(mStalledIssues.at(index.row())));
        }
    }

    return QVariant();
}

QModelIndex StalledIssuesModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
    {
        return QModelIndex();
    }

    auto stalledIssueItem = static_cast<QExplicitlySharedDataPointer<StalledIssueData>*>(index.internalPointer());
    if (!stalledIssueItem)
    {
        return QModelIndex();
    }

    auto row = mStalledIssuesByOrder.value((*stalledIssueItem),-1);
    if(row >= 0)
    {
        return createIndex(row, 0);
    }

    return QModelIndex();
}

QModelIndex StalledIssuesModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid())
    {
        auto stalledIssue = mStalledIssues.value(parent.row());
        return createIndex(0, 0, &stalledIssue);
    }
    else
    {
        return (row < rowCount(QModelIndex())) ?  createIndex(row, column) : QModelIndex();
    }
}

Qt::ItemFlags StalledIssuesModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

void StalledIssuesModel::finishStalledIssues(const QModelIndexList &indexes)
{
    auto indexesToFinish(indexes);
    removeRows(indexesToFinish);
}

void StalledIssuesModel::removeRows(QModelIndexList &indexesToRemove)
{
    std::sort(indexesToRemove.begin(), indexesToRemove.end(),[](QModelIndex check1, QModelIndex check2){
        return check1.row() > check2.row();
    });

    // First clear finished transfers (remove rows), then cancel the others.
    // This way, there is no risk of messing up the rows order with cancel requests.
    int count (0);
    int row (indexesToRemove.last().row());
    for (auto index : indexesToRemove)
    {
        // Init row with row of first tag
        if (count == 0)
        {
            row = index.row();
        }

        // If rows are non-contiguous, flush and start from item
        if (row != index.row())
        {
            QAbstractItemModel::removeRows(row + 1, count, QModelIndex());
            count = 0;
            row = index.row();
        }

        // We have at least one row
        count++;
        row--;
    }
    // Flush pooled rows (start at row + 1).
    // This happens when the last item processed is in a finished state.
    if (count > 0)
    {
        QAbstractItemModel::removeRows(row + 1, count, QModelIndex());
    }

    updateStalledIssuedByOrder();
}

void StalledIssuesModel::updateStalledIssuedByOrder()
{
    mStalledIssuesByOrder.clear();

    //Recalculate rest of items
    for(int row = 0; row < rowCount(QModelIndex()); ++row)
    {
        auto item = mStalledIssues.at(row);
        mStalledIssuesByOrder.insert(item, row);
    }
}
