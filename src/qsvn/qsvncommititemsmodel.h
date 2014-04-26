#ifndef QSVNCOMMITITEMSMODEL_H
#define QSVNCOMMITITEMSMODEL_H

#include "qsvn.h"

#include <QAbstractItemModel>
#include <QString>
#include <QList>
#include <QDir>


class QSVNCommitItemsModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit QSVNCommitItemsModel(const QList<QSvnStatusItem> &checkedItems, const QString &dir, QObject *parent = 0);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void showUnversionedFiles(bool state);

    QList<QSvnStatusItem> checkedItems() const;
    QDir dir() const;

signals:

public slots:

private:
    QList<QSvnStatusItem> m_items;
    QList<QSvnStatusItem *> m_versionedItems;
    QDir m_dir;
    bool m_showUnversionedFiles;
};

#endif // QSVNCOMMITITEMSMODEL_H
