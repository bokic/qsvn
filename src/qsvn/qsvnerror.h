#ifndef QSVNERROR_H
#define QSVNERROR_H

#include <QStringList>
#include "svn_client.h"


class QSvnError
{
public:
    QSvnError(svn_error_t *err = nullptr);
    QStringList error() const;
    bool isError() const;

private:
    QStringList m_error;
};

#endif // QSVNERROR_H
