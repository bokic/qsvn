#include "qsvnerror.h"
#include "svn_client.h"


QSvnError::QSvnError(svn_error_t *err)
{
    while (err)
    {
        QString errorString = QString::fromUtf8(err->message);

        m_error.append(errorString);

        err = err->child;
    }
}

QStringList QSvnError::error() const
{
    return m_error;
}

bool QSvnError::isError() const
{
    return !m_error.isEmpty();
}
