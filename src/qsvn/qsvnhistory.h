#ifndef QSVNHISTORY_H
#define QSVNHISTORY_H

#include <QStringList>
#include <QString>


class QSvnHistory
{
public:
    QSvnHistory(const QString &url);
    virtual ~QSvnHistory();

    bool add(const QString &item);
    void remove(int index);
    void setMaxItems(int max);
    int count() const;
    QString item(int index);
    QStringList items() const;

private:
    int load();
    bool save() const;

    QString m_url;
    QStringList m_items;
    int m_maxItems;
    bool m_modified;
};

#endif // QSVNHISTORY_H
