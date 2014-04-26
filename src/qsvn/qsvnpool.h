#ifndef QSVNPOOL_H
#define QSVNPOOL_H

#include "svn_pools.h"


class QSvnPool
{
public:
    QSvnPool(apr_pool_t *parent = nullptr);
    virtual ~QSvnPool();

    operator apr_pool_t *();

private:
    apr_pool_t *m_pool;
};

#endif // QSVNPOOL_H
