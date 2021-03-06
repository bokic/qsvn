#include "qsvncommititemsmodel.h"
#include "qsvn.h"

#include <QAbstractItemModel>
#include <QColor>
#include <QList>


QSVNCommitItemsModel::QSVNCommitItemsModel(const QList<QSvnStatusItem> &items, const QString &dir, QObject *parent)
    : QAbstractItemModel(parent)
    , m_items(items)
    , m_dir(dir)
    , m_showUnversionedFiles(true)
{
    if ((m_items.count() > 0)&&(m_items.first().m_filename == m_dir.absolutePath())&&(m_items.first().m_nodeStatus == svn_wc_status_normal))
    {
        m_items.removeFirst();
    }

    for(int c = 0; c < m_items.count(); c++)
    {
        QSvnStatusItem *item = &m_items[c];

        if (item->m_nodeStatus != svn_wc_status_unversioned)
        {
            m_versionedItems.append(item);
        }
    }
}

QModelIndex QSVNCommitItemsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return createIndex(row, column);
}

QModelIndex QSVNCommitItemsModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);

    return QModelIndex();
}

int QSVNCommitItemsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    int count;

    if (m_showUnversionedFiles)
    {
        count = m_items.length();
    }
    else
    {
        count = m_versionedItems.length();
    }

    return count;
}

int QSVNCommitItemsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 5;
}

Qt::ItemFlags QSVNCommitItemsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return 0;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable |  Qt::ItemIsEditable;
}

QVariant QSVNCommitItemsModel::data(const QModelIndex &index, int role) const
{
    QString filename;

    if (role == Qt::DisplayRole)
    {
        const QSvnStatusItem *item = nullptr;


        if (m_showUnversionedFiles)
        {
            item = &m_items.at(index.row());
        }
        else
        {
            item = m_versionedItems.at(index.row());
        }
        int pos;

        switch(index.column()) {
        case 0:
            filename = m_dir.relativeFilePath(item->m_filename);

            if (filename.isEmpty())
            {
                filename = ".";
            }

            return filename;
            break;
        case 1:
            pos = item->m_filename.lastIndexOf('.');
            if (pos > 0)
            {
                return item->m_filename.right(item->m_filename.length() - pos);
            }
            break;
        case 2:
            switch(item->m_nodeStatus) {
            case svn_wc_status_none:
                return tr("none");
                break;
            case svn_wc_status_unversioned:
                return tr("non-versioned");
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
    else if (role == Qt::TextColorRole)
    {
        // SVNStatusListCtrl.cpp:4583 - CSVNStatusListCtrl::OnNMCustomdraw
        QColor color(Qt::black);
        const QSvnStatusItem *item = nullptr;

        if (m_showUnversionedFiles)
        {
            item = &m_items[index.row()];
        }
        else
        {
            item = m_versionedItems[index.row()];
        }

        switch (item->m_nodeStatus)
        {
        case svn_wc_status_added:
            color = QColor(160, 32, 240); // 160-32-240
        case svn_wc_status_modified:
            color = QColor(Qt::blue);
            break;
        case svn_wc_status_missing:
        case svn_wc_status_deleted:
        case svn_wc_status_replaced:
            color = QColor(139, 69, 19); // brown
        case svn_wc_status_merged:
            color = QColor(Qt::green);
        case svn_wc_status_conflicted:
            color = QColor(Qt::red);
        default:
            break;
        }

        return color;
    }
    else if (role == Qt::CheckStateRole)
    {
        if (index.column() == 0)
        {
            bool checked;
            if (m_showUnversionedFiles)
            {
                checked = m_items[index.row()].m_checked;
            }
            else
            {
                checked = m_versionedItems[index.row()]->m_checked;
            }

            if (checked)
            {
                return Qt::Checked;
            }
            else
            {
                return Qt::Unchecked;
            }
        }
    }

    return QVariant();
}

bool QSVNCommitItemsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ((index.column() == 0)&&(role==Qt::CheckStateRole))
    {
        QSvnStatusItem *item = nullptr;

        if (m_showUnversionedFiles)
        {
            item = &m_items[index.row()];
        }
        else
        {
            item = m_versionedItems[index.row()];
        }

        if (value == Qt::Checked)
        {
            item->m_checked = true;
        }
        else
        {
            item->m_checked = false;
        }

        emit dataChanged(index, index);

        emit checked(index.row(), item->m_checked);

        return true;
    }

    return false;
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

void QSVNCommitItemsModel::sort(int column, Qt::SortOrder order)
{
    switch(column)
    {
    case 0:
    case 3: // TODO: Implement commit property column
    case 4: // TODO: Implement commit lock column
        if (order == Qt::AscendingOrder)
        {
            qSort(m_items.begin(), m_items.end(),[](const QSvnStatusItem &item1, const QSvnStatusItem &item2)
            {
                return item1.m_filename > item2.m_filename;
            });
        }
        else
        {
            qSort(m_items.begin(), m_items.end(),[](const QSvnStatusItem &item1, const QSvnStatusItem &item2)
            {
                return item1.m_filename < item2.m_filename;
            });
        }
        break;
    case 1:
        if (order == Qt::AscendingOrder)
        {
            qSort(m_items.begin(), m_items.end(),[](const QSvnStatusItem &item1, const QSvnStatusItem &item2)
            {
                QString ext1 = QFileInfo(item1.m_filename).suffix();
                QString ext2 = QFileInfo(item2.m_filename).suffix();

                if (ext1 == ext2)
                {
                    return item1.m_filename > item2.m_filename;
                }

                return ext1 > ext2;
            });
        }
        else
        {
            qSort(m_items.begin(), m_items.end(),[](const QSvnStatusItem &item1, const QSvnStatusItem &item2)
            {
                QString ext1 = QFileInfo(item1.m_filename).suffix();
                QString ext2 = QFileInfo(item2.m_filename).suffix();

                if (ext1 == ext2)
                {
                    return item1.m_filename < item2.m_filename;
                }

                return ext1 < ext2;
            });
        }
        break;
    case 2:
        if (order == Qt::AscendingOrder)
        {
            qSort(m_items.begin(), m_items.end(),[](const QSvnStatusItem &item1, const QSvnStatusItem &item2)
            {
                if (item1.m_nodeStatus == item2.m_nodeStatus)
                {
                    return item1.m_filename > item2.m_filename;
                }

                return item1.m_nodeStatus > item2.m_nodeStatus;
            });
        }
        else
        {
            qSort(m_items.begin(), m_items.end(),[](const QSvnStatusItem &item1, const QSvnStatusItem &item2)
            {
                if (item1.m_nodeStatus == item2.m_nodeStatus)
                {
                    return item1.m_filename < item2.m_filename;
                }

                return item1.m_nodeStatus < item2.m_nodeStatus;
            });
        }
        break;
    }

    int current_rows = (m_showUnversionedFiles?m_items.count():m_versionedItems.count());

    emit dataChanged(index(0, 0), index(current_rows, 4));
}

void QSVNCommitItemsModel::showUnversionedFiles(bool state)
{
    int current_rows = (m_showUnversionedFiles?m_items.count():m_versionedItems.count());

    m_showUnversionedFiles = state;

    emit dataChanged(index(0, 0), index(current_rows, 4));
}

void QSVNCommitItemsModel::checkAllItems()
{
    bool changed = false;

    if (m_showUnversionedFiles)
    {
        for(int c = 0; c < m_items.count(); c++)
        {
            if (!m_items[c].m_checked)
            {
                m_items[c].m_checked = true;
                changed = true;
            }
        }
    }
    else
    {
        for(int c = 0; c < m_versionedItems.count(); c++)
        {
            if (!m_versionedItems[c]->m_checked)
            {
                m_versionedItems[c]->m_checked = true;
                changed = true;
            }
        }
    }

    if (changed)
    {
        int current_rows = (m_showUnversionedFiles?m_items.count():m_versionedItems.count());
        emit dataChanged(index(0, 0), index(current_rows, 4));
    }
}

void QSVNCommitItemsModel::uncheckAllItems()
{
    bool changed = false;

    if (m_showUnversionedFiles)
    {
        for(int c = 0; c < m_items.count(); c++)
        {
            if (m_items[c].m_checked)
            {
                m_items[c].m_checked = false;
                changed = true;
            }
        }
    }
    else
    {
        for(int c = 0; c < m_versionedItems.count(); c++)
        {
            if (m_versionedItems[c]->m_checked)
            {
                m_versionedItems[c]->m_checked = false;
                changed = true;
            }
        }
    }

    if (changed)
    {
        int current_rows = (m_showUnversionedFiles?m_items.count():m_versionedItems.count());
        emit dataChanged(index(0, 0), index(current_rows, 4));
    }
}

void QSVNCommitItemsModel::checkNonVersionedItems()
{
    bool changed = false;

    if (m_showUnversionedFiles)
    {
        for(int c = 0; c < m_items.count(); c++)
        {
            if (m_items[c].m_nodeStatus == svn_wc_status_unversioned)
            {
                m_items[c].m_checked = true;
                changed = true;
            }
        }
    }

    if (changed)
    {
        int current_rows = (m_showUnversionedFiles?m_items.count():m_versionedItems.count());
        emit dataChanged(index(0, 0), index(current_rows, 4));
    }
}

void QSVNCommitItemsModel::checkVersionedItems()
{
    bool changed = false;

    if (m_showUnversionedFiles)
    {
        for(int c = 0; c < m_items.count(); c++)
        {
            if (m_items[c].m_nodeStatus != svn_wc_status_unversioned)
            {
                m_items[c].m_checked = true;
                changed = true;
            }
        }
    }
    else
    {
        for(int c = 0; c < m_versionedItems.count(); c++)
        {
            if (m_versionedItems[c]->m_nodeStatus != svn_wc_status_unversioned)
            {
                m_versionedItems[c]->m_checked = true;
                changed = true;
            }
        }
    }

    if (changed)
    {
        int current_rows = (m_showUnversionedFiles?m_items.count():m_versionedItems.count());
        emit dataChanged(index(0, 0), index(current_rows, 4));
    }
}

void QSVNCommitItemsModel::checkAddedItems()
{
    bool changed = false;

    if (m_showUnversionedFiles)
    {
        for(int c = 0; c < m_items.count(); c++)
        {
            if (m_items[c].m_nodeStatus == svn_wc_status_added)
            {
                m_items[c].m_checked = true;
                changed = true;
            }
        }
    }
    else
    {
        for(int c = 0; c < m_versionedItems.count(); c++)
        {
            if (m_versionedItems[c]->m_nodeStatus == svn_wc_status_added)
            {
                m_versionedItems[c]->m_checked = true;
                changed = true;
            }
        }
    }

    if (changed)
    {
        int current_rows = (m_showUnversionedFiles?m_items.count():m_versionedItems.count());
        emit dataChanged(index(0, 0), index(current_rows, 4));
    }
}

void QSVNCommitItemsModel::checkDeletedItems()
{
    bool changed = false;

    if (m_showUnversionedFiles)
    {
        for(int c = 0; c < m_items.count(); c++)
        {
            if (m_items[c].m_nodeStatus == svn_wc_status_missing)
            {
                m_items[c].m_checked = true;
                changed = true;
            }
        }
    }
    else
    {
        for(int c = 0; c < m_versionedItems.count(); c++)
        {
            if (m_versionedItems[c]->m_nodeStatus == svn_wc_status_deleted)
            {
                m_versionedItems[c]->m_checked = true;
                changed = true;
            }
        }
    }

    if (changed)
    {
        int current_rows = (m_showUnversionedFiles?m_items.count():m_versionedItems.count());
        emit dataChanged(index(0, 0), index(current_rows, 4));
    }
}

void QSVNCommitItemsModel::checkModifiedItems()
{
    bool changed = false;

    if (m_showUnversionedFiles)
    {
        for(int c = 0; c < m_items.count(); c++)
        {
            if (m_items[c].m_nodeStatus == svn_wc_status_modified)
            {
                m_items[c].m_checked = true;
                changed = true;
            }
        }
    }
    else
    {
        for(int c = 0; c < m_versionedItems.count(); c++)
        {
            if (m_versionedItems[c]->m_nodeStatus == svn_wc_status_modified)
            {
                m_versionedItems[c]->m_checked = true;
                changed = true;
            }
        }
    }

    if (changed)
    {
        int current_rows = (m_showUnversionedFiles?m_items.count():m_versionedItems.count());
        emit dataChanged(index(0, 0), index(current_rows, 4));
    }
}

void QSVNCommitItemsModel::checkFileItems()
{
    bool changed = false;

    if (m_showUnversionedFiles)
    {
        for(int c = 0; c < m_items.count(); c++)
        {
            if ((m_items[c].m_kind == svn_node_file)||(QFileInfo(m_items[c].m_filename).isFile()))
            {
                m_items[c].m_checked = true;
                changed = true;
            }
        }
    }
    else
    {
        for(int c = 0; c < m_versionedItems.count(); c++)
        {
            if ((m_versionedItems[c]->m_kind == svn_node_file)||(QFileInfo(m_versionedItems[c]->m_filename).isFile()))
            {
                m_versionedItems[c]->m_checked = true;
                changed = true;
            }
        }
    }

    if (changed)
    {
        int current_rows = (m_showUnversionedFiles?m_items.count():m_versionedItems.count());
        emit dataChanged(index(0, 0), index(current_rows, 4));
    }
}

void QSVNCommitItemsModel::checkDirItems()
{
    bool changed = false;

    if (m_showUnversionedFiles)
    {
        for(int c = 0; c < m_items.count(); c++)
        {
            if ((m_items[c].m_kind == svn_node_dir)||(QFileInfo(m_items[c].m_filename).isDir()))
            {
                m_items[c].m_checked = true;
                changed = true;
            }
        }
    }
    else
    {
        for(int c = 0; c < m_versionedItems.count(); c++)
        {
            if ((m_versionedItems[c]->m_kind == svn_node_dir)&&(QFileInfo(m_versionedItems[c]->m_filename).isDir()))
            {
                m_versionedItems[c]->m_checked = true;
                changed = true;
            }
        }
    }

    if (changed)
    {
        int current_rows = (m_showUnversionedFiles?m_items.count():m_versionedItems.count());
        emit dataChanged(index(0, 0), index(current_rows, 4));
    }
}

QList<QSvnStatusItem> QSVNCommitItemsModel::checkedItems() const
{
    QList<QSvnStatusItem> ret;

    if (m_showUnversionedFiles)
    {
        foreach(const QSvnStatusItem &item, m_items)
        {
            if (item.m_checked)
            {
                ret.append(item);
            }
        }
    }
    else
    {
        foreach(const QSvnStatusItem *item, m_versionedItems)
        {
            if (item->m_checked)
            {
                ret.append(*item);
            }
        }
    }

    return ret;
}

QList<QSvnStatusItem> QSVNCommitItemsModel::items() const
{
    QList<QSvnStatusItem> ret;

    if (m_showUnversionedFiles)
    {
        return  m_items;
    }

    foreach(const QSvnStatusItem *item, m_versionedItems)
    {
        ret.append(*item);
    }

    return ret;
}

int QSVNCommitItemsModel::totalItemCount() const
{
    if (m_showUnversionedFiles)
    {
        return m_items.count();
    }

    return m_versionedItems.count();
}

int QSVNCommitItemsModel::checkItemCount() const
{
    int ret = 0;

    if (m_showUnversionedFiles)
    {
        foreach(const QSvnStatusItem &item, m_items)
        {
            if (item.m_checked)
            {
                ret++;
            }
        }
    }
    else
    {
        foreach(const QSvnStatusItem *item, m_versionedItems)
        {
            if (item->m_checked)
            {
                ret++;
            }
        }
    }

    return ret;
}

QDir QSVNCommitItemsModel::dir() const
{
    return m_dir;
}
