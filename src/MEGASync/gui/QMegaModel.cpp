#include "QMegaModel.h"
#include "MegaApplication.h"

#include <QBrush>
#include <QApplication>
#include "control/Utilities.h"

using namespace mega;

QMegaModel::QMegaModel(mega::MegaApi *megaApi, QObject *parent) :
    QAbstractItemModel(parent)
{
    this->megaApi = megaApi;
    this->root = ((MegaApplication*)qApp)->getRootNode();
    this->rootItem = new MegaItem(root.get());
    this->folderIcon =  QIcon(QString::fromAscii("://images/small_folder.png"));

    MegaUserList *contacts = megaApi->getContacts();
    for (int i = 0; i < contacts->size(); i++)
    {
        MegaUser *contact = contacts->get(i);
        MegaNodeList *folders = megaApi->getInShares(contact);
        for (int j = 0; j < folders->size(); j++)
        {
            MegaNode *folder = folders->get(j)->copy();
            ownNodes.append(folder);
            inshareItems.append(new MegaItem(folder));
            inshareOwners.append(QString::fromUtf8(contact->getEmail()));
        }
        delete folders;
    }
    delete contacts;

    this->requiredRights = MegaShare::ACCESS_READ;
    this->displayFiles = false;
    this->disableFolders = false;
}

int QMegaModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant QMegaModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    MegaItem *item = (MegaItem *)index.internalPointer();
    if (!item->getNode())
    {
        return QVariant();
    }

    switch(role)
    {
        case Qt::DecorationRole:
        {
            MegaNode *node = item->getNode();
            if (node->getType() >= MegaNode::TYPE_FOLDER)
            {
                if (node->isInShare())
                {
                    return QIcon(QString::fromAscii("://images/small_folder_incoming.png"));;
                }
                else if (node->isOutShare())
                {
                    return QIcon(QString::fromAscii("://images/small_folder_outgoing.png"));;
                }

                return folderIcon;
            }

            return Utilities::getExtensionPixmapSmall(QString::fromUtf8(node->getName()));
        }
        case Qt::ForegroundRole:
        {
            int access = megaApi->getAccess(item->getNode());
            if (access < requiredRights || (disableFolders && item->getNode()->isFolder()))
            {
                return QVariant(QBrush(QColor(170,170,170, 127)));
            }

            return QVariant();
        }
        case Qt::DisplayRole:
        {
            if (item->getParent())
            {
                return QVariant(QString::fromUtf8(item->getNode()->getName()));
            }
            else if (item->getNode()->getType() == MegaNode::TYPE_ROOT)
            {
                return QVariant(QCoreApplication::translate("MegaNodeNames",item->getNode()->getName()));
            }

            int inshareIndex = index.row() - 1;
            return QVariant(QString::fromUtf8("%1 (%2)")
                            .arg(QString::fromUtf8(inshareItems.at(inshareIndex)->getNode()->getName()))
                            .arg(inshareOwners.at(inshareIndex)));
        }
        default:
        {
            return QVariant();
        }
    }
#ifndef WIN32
    return QVariant(); // warning C4702: unreachable code
#endif
}

QModelIndex QMegaModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    if (parent.isValid())
    {
        MegaItem * item = NULL;
        item = (MegaItem *)parent.internalPointer();

        if (!item->areChildrenSet())
        {
            item->setChildren(megaApi->getChildren(item->getNode()));
        }

        return createIndex(row, column, item->getChild(row));
    }

    if (row == 0)
    {
        return createIndex(row, column, rootItem);
    }

    return createIndex(row, column, inshareItems.at(row - 1));
}

QModelIndex QMegaModel::parent(const QModelIndex &index) const
{
    MegaItem *item = (MegaItem *)index.internalPointer();
    if (!item)
    {
        return QModelIndex();
    }

    MegaItem *parent = item->getParent();
    if (!parent)
    {
        return QModelIndex();
    }

    MegaItem *grandparent = parent->getParent();
    if (!grandparent)
    {
        if (parent == rootItem)
        {
            return createIndex(0, 0, parent);
        }

        return createIndex(1 + inshareItems.indexOf(parent), 0, parent);
    }

    return createIndex(grandparent->indexOf(parent), 0, parent);
}

int QMegaModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        MegaItem *item = (MegaItem *)parent.internalPointer();
        if (!item->areChildrenSet())
        {
            item->setChildren(megaApi->getChildren(item->getNode()));
        }

        return item->getNumChildren();
    }

    return inshareItems.size() + 1;
}

void QMegaModel::setRequiredRights(int requiredRights)
{
    this->requiredRights = requiredRights;
}

void QMegaModel::setDisableFolders(bool option)
{
    this->disableFolders = option;
}

void QMegaModel::showFiles(bool show)
{
    this->displayFiles = show;
    this->rootItem->displayFiles(show);
    for (int i = 0; i < inshareItems.size(); i++)
    {
        inshareItems.at(i)->displayFiles(show);
    }
}

QModelIndex QMegaModel::insertNode(MegaNode *node, const QModelIndex &parent)
{
    MegaItem *item = (MegaItem *)parent.internalPointer();
    int index = item->insertPosition(node);

    beginInsertRows(parent, index, index);
    item->insertNode(node, index);
    endInsertRows();

    return this->index(index, 0, parent);
}

void QMegaModel::removeNode(QModelIndex &item)
{
    MegaNode *node = ((MegaItem *)item.internalPointer())->getNode();
    MegaItem *parent = (MegaItem *)item.parent().internalPointer();
    if (!node || !parent)
    {
        return;
    }
    int index = parent->indexOf((MegaItem *)item.internalPointer());

    beginRemoveRows(item.parent(), index, index);
    parent->removeNode(node);
    endRemoveRows();
}

MegaNode *QMegaModel::getNode(const QModelIndex &index)
{
    MegaItem *item = (MegaItem *)index.internalPointer();
    if (!item)
    {
        return NULL;
    }
    return item->getNode();
}

QMegaModel::~QMegaModel()
{
    delete rootItem;
    qDeleteAll(inshareItems);
    qDeleteAll(ownNodes);
}
