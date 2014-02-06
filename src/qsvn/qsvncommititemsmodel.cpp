#include "qsvncommititemsmodel.h"
#include "qsvn.h"

#include <QAbstractItemModel>
#include <QList>

QSVNCommitItemsModel::QSVNCommitItemsModel(const QList<QSvnStatusItem> &items, const QString &dir, QObject *parent)
    : QAbstractItemModel(parent)
    , m_items(items)
    , m_dir(dir)
{
}

QModelIndex QSVNCommitItemsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return createIndex(row, column);
}

QModelIndex QSVNCommitItemsModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int QSVNCommitItemsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_items.length();
}

int QSVNCommitItemsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 5;
}

QVariant QSVNCommitItemsModel::data(const QModelIndex &index, int role) const
{
    QString filename;

    if (role == Qt::DisplayRole)
    {
        const QSvnStatusItem &item = m_items.at(index.row());
        int pos;

        switch(index.column()) {
        case 0:
            filename = m_dir.relativeFilePath(item.m_filename);

            if (filename.isEmpty())
            {
                filename = ".";
            }

            return filename;
            break;
        case 1:
            pos = item.m_filename.lastIndexOf('.');
            if (pos > 0)
            {
                return item.m_filename.right(item.m_filename.length() - pos);
            }
            break;
        case 2:
            switch(item.m_nodeStatus) {
            case svn_wc_status_none:
                return tr("none");
                break;
            case svn_wc_status_unversioned:
                return tr("unversioned");
                break;
            case svn_wc_status_normal:
                return tr("normal");
                break;
            case svn_wc_status_added:
                return tr("added");
                break;
            case svn_wc_status_missing:
                return tr("missing");
                break;
            case svn_wc_status_deleted:
                return tr("deleted");
                break;
            case svn_wc_status_replaced:
                return tr("replaced");
                break;
            case svn_wc_status_modified:
                return tr("modified");
                break;
            case svn_wc_status_merged:
                return tr("merged");
                break;
            case svn_wc_status_conflicted:
                return tr("conflicted");
                break;
            case svn_wc_status_ignored:
                return tr("ignored");
                break;
            case svn_wc_status_obstructed:
                return tr("obstructed");
                break;
            case svn_wc_status_external:
                return tr("external");
                break;
            case svn_wc_status_incomplete:
                return tr("incomplete");
                break;
            }

            break;
        }
    }
    if (role == Qt::CheckStateRole)
    {
        if (index.column() == 0)
        {
            return Qt::Unchecked;
        }
    }

    return QVariant();
}

QVariant QSVNCommitItemsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole)&&(orientation == Qt::Horizontal))
    {
        switch (section) {
        case 0:
            return tr("Path");
            break;
        case 1:
            return tr("Extension");
            break;
        case 2:
            return tr("Status");
            break;
        case 3:
            return tr("Property status");
            break;
        case 4:
            return tr("Lock");
            break;
        }
    }

    return QVariant();
}
