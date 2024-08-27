#include "SyncModel.h"

#include "Utilities.h"

#include <algorithm>
#include <QCoreApplication>
#include <QDate>

SyncModel::SyncModel(QObject* parent):
    QAbstractListModel(parent)
{}

QHash<int, QByteArray> SyncModel::roleNames() const
{
    static QHash<int, QByteArray> roles{
        {TYPE,          "type"        },
        {NAME,          "name"        },
        {SIZE,          "size"        },
        {DATE_ADDED,    "dateAdded"   },
        {DATE_MODIFIED, "dateModified"},
        {STATUS,        "status"      }
    };
    return roles;
}

void SyncModel::add(const QmlSyncData& newSync)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    mSyncObjects.append(newSync);
    endInsertRows();
}

void SyncModel::addOrUpdate(const QmlSyncData& newSync)
{
    const int index = findRowByHandle(newSync.handle);
    if (index == -1)
    {
        add(newSync);
    }
    else
    {
        mSyncObjects[index].updateFields(newSync);
        emit dataChanged(QAbstractListModel::index(index), QAbstractListModel::index(index));
    }
}

void SyncModel::remove(mega::MegaHandle handle)
{
    int index = findRowByHandle(handle);
    beginRemoveRows(QModelIndex(), index, index);
    auto it = mSyncObjects.begin();
    for (; it != mSyncObjects.end(); ++it)
    {
        if (it->handle == handle)
        {
            mSyncObjects.erase(it);
            break;
        }
    }
    endRemoveRows();
}

void SyncModel::clear()
{
    mSyncObjects.clear();
}

int SyncModel::findRowByHandle(mega::MegaHandle handle) const
{
    for (int i = 0; i < mSyncObjects.size(); ++i)
    {
        if (mSyncObjects[i].handle == handle)
        {
            return i;
        }
    }
    return -1;
}

SyncStatus::Value SyncModel::computeDeviceStatus() const
{
    SyncStatus::Value deviceStatus = SyncStatus::UP_TO_DATE;
    for (const auto& sync: mSyncObjects)
    {
        const SyncStatus::Value currentStatus = sync.status;
        if (deviceStatus < currentStatus)
        {
            deviceStatus = currentStatus;
        }
    }
    return deviceStatus;
}

qint64 SyncModel::computeTotalSize() const
{
    const qint64 startValue = 0;
    auto accumulator = [](qint64 total, const auto& sync) {
        return total + sync.size;
    };
    return std::accumulate(mSyncObjects.begin(), mSyncObjects.end(), startValue, accumulator);
}

void SyncModel::setStatus(mega::MegaHandle handle, const SyncStatus::Value status)
{
    const int row = findRowByHandle(handle);
    if (row != -1)
    {
        mSyncObjects[row].status = status;
        emit dataChanged(index(row), index(row));
    }
}

bool SyncModel::hasUpdatingStatus() const
{
    auto finder = [](const QmlSyncData& obj) {
        return obj.status == SyncStatus::UPDATING;
    };
    return std::any_of(mSyncObjects.begin(), mSyncObjects.end(), finder);
}

int SyncModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return mSyncObjects.size();
}

int SyncModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant SyncModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    const auto row = index.row();
    const auto coulumn = index.column();
    if (row >= rowCount() || coulumn >= columnCount())
    {
        return result;
    }

    switch (role)
    {
        case NAME:
            result = getName(row);
            break;
        case TYPE:
            result = getType(row);
            break;
        case SIZE:
            result = getSize(row);
            break;
        case DATE_ADDED:
            result = getDateAdded(row);
            break;
        case DATE_MODIFIED:
            result = getDateModified(row);
            break;
        case STATUS:
            result = getStatus(row);
            break;

        default:
            break;
    }
    return result;
}

QString SyncModel::getName(int row) const
{
    return mSyncObjects[row].name;
}

QString SyncModel::getSize(int row) const
{
    return Utilities::getSizeString(mSyncObjects[row].size);
}

QmlSyncType::Type SyncModel::getType(int row) const
{
    return mSyncObjects[row].type == QString::fromUtf8("Sync") ? QmlSyncType::SYNC :
                                                                 QmlSyncType::BACKUP;
}

QDate SyncModel::getDateAdded(int row) const
{
    return mSyncObjects[row].dateAdded.date();
}

QDate SyncModel::getDateModified(int row) const
{
    return mSyncObjects[row].dateModified.date();
}

SyncStatus::Value SyncModel::getStatus(int row) const
{
    return mSyncObjects[row].status;
}
