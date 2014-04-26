#include "qsvnpool.h"
#include "svn_pools.h"


QSvnPool::QSvnPool(apr_pool_t *parent)
{
    m_pool = svn_pool_create(parent);
}

QSvnPool::~QSvnPool()
{
    apr_pool_destroy(m_pool);
    m_pool = nullptr;
}

QSvnPool::operator apr_pool_t *()
{
    return m_pool;
}
