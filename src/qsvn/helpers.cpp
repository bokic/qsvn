#include "helpers.h"

#include <QString>
#include <QObject>
#include <QDir>


QString bytesToString(int bytes)
{
    if (bytes == 1)
    {
        return QObject::tr("1 byte");
    }
    else if (bytes < 1024)
    {
        return QObject::tr("%1 bytes").arg(bytes);
    }
    else if (bytes < (1024 * 1024))
    {
        return QObject::tr("%1 KB").arg((float)bytes / 1024);
    }
    else if (bytes < (1024 * 1024 * 1024))
    {
        return QObject::tr("%1 MB").arg((float)bytes / (1024 * 1024));
    }
    else
    {
        return QObject::tr("%1 GB").arg((float)bytes / (1024 * 1024 * 1024));
    }
}

QString getCommonDir(const QStringList &items)
{
    QString ret;

    foreach (const QString &item, items)
    {
        const QString &path = QDir(item).path();

        if ((ret.isEmpty())||(path.length() < ret.length()))
        {
            ret = path;
        }
    }

    return ret;
}
