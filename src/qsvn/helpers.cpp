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
        return QObject::tr("%1 KB").arg(QString::number((float)bytes / 1024, 'f', 2));
    }
    else if (bytes < (1024 * 1024 * 1024))
    {
        return QObject::tr("%1 MB").arg(QString::number((float)bytes / (1024 * 1024), 'f', 2));
    }
    else
    {
        return QObject::tr("%1 GB").arg(QString::number((float)bytes / (1024 * 1024 * 1024), 'f', 2));
    }
}

QString getCommonDir(const QStringList &items)
{
    QString ret;

    foreach (const QString &item, items)
    {
        QFileInfo fi = QFileInfo(item);
        QString path;

        if (fi.isDir())
        {
            path = item;
        }
        else
        {
            path = QFileInfo(item).dir().path();
        }

        if ((ret.isEmpty())||(path.length() < ret.length()))
        {
            ret = path;
        }
    }

    return ret;
}
