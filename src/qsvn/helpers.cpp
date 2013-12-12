#include "helpers.h"

#include <QString>
#include <QObject>

QString bytesToString(int bytes)
{
    if (bytes < 1024)
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
