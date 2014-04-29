#include "qsvnhistory.h"

#include <QSettings>
#include <stdlib.h>


QSvnHistory::QSvnHistory(const QString &url)
    : m_url(url)
    , m_maxItems(25)
    , m_modified(false)
{
}

QSvnHistory::~QSvnHistory()
{
    if (m_modified)
    {
        save();
    }
}

bool QSvnHistory::add(const QString &item)
{
    if (item.isEmpty())
    {
        return false;
    }

    if (m_items.isEmpty())
    {
        load();
    }

    if (m_items.contains(item))
    {
        m_items.removeAll(item);
    }

    m_items.prepend(item);

    m_modified = true;

    return true;
}

void QSvnHistory::remove(int index)
{
    if (m_items.isEmpty())
    {
        load();
    }

    if (m_items.count() > index)
    {
        m_items.removeAt(index);

        m_modified = true;
    }
}

void QSvnHistory::setMaxItems(int max)
{
    m_maxItems = max;
}

int QSvnHistory::count() const
{
    return m_items.count();
}

QString QSvnHistory::item(int index)
{
    if (m_items.isEmpty())
    {
        load();
    }

    if (m_items.count() > index)
    {
        return m_items.at(index);
    }

    return QString();
}

QStringList QSvnHistory::items() const
{
    return m_items;
}

int QSvnHistory::load()
{
    if (m_url.isEmpty())
    {
        return -1;
    }

    m_items.clear();

    QSettings settings;
    settings.beginGroup("CommitMessageHistory");

    for(int c = 0; c < m_maxItems; c++)
    {
        const QString &key = m_url + "_" + QString::number(c + 1);

        if (settings.contains(key))
        {
            break;
        }

        const QString &value = settings.value(key).toByteArray();

        m_items.append(value);
    }

    settings.endGroup();

    return m_items.count();
}

bool QSvnHistory::save() const
{
    int count = m_items.count();

    if (count == 0)
    {
        return false;
    }

    if (count > m_maxItems)
    {
        count = m_maxItems;
    }

    QSettings settings;
    settings.beginGroup("CommitMessageHistory");

    for(int c = 0; c < count; c++)
    {
        const QString &key = m_url + "_" + QString::number(c + 1);

        settings.setValue(key, m_items.at(c));
    }

    for(int c = count; ; c++)
    {
        const QString &key = m_url + "_" + QString::number(c + 1);

        if (settings.contains(key))
        {
            settings.remove(key);
        }
    }

    return true;
}
