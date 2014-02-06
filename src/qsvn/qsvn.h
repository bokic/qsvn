#ifndef QSVN_H
#define QSVN_H

#include <QStringList>
#include <QDateTime>
#include <QString>
#include <QList>

#include "svn_client.h"
#include "svn_dirent_uri.h"
#include "svn_cmdline.h"
#include "svn_pools.h"
#include "svn_config.h"
#include "svn_fs.h"


struct QRepoBrowserFile
{
    QRepoBrowserFile()
        : isdir(false)
        , revision(-1)
        , size(-1)
    {
    }

    QString filename;
    bool isdir;
    int revision;
    QString author;
    int size;
    QDateTime modified;
};

struct QRepoBrowserResult
{
    QList<QRepoBrowserFile> files;
    QString error;
};

class QSvnStatusItem;

class QSvn : public QObject
{
    Q_OBJECT

public:
    enum QSVNOperationType {QSVNOperationNone, QSVNOperationRepoBrowser, QSVNOperationCommit, QSVNOperationUpdate, QSVNOperationCheckout, QSVNOperationStatus};

    QSvn(QObject *parent=0);
    ~QSvn();
    void init();
    void cancel();
    QSVNOperationType operation();
    bool isBusy();

signals:
    void error(QString text);
    void finished(bool result);
    void repoBrowserResult(QRepoBrowserResult items);
    void logMsg();
    void notify(svn_wc_notify_t notify);
    void progress(int progress, int total);
    void statusFinished(QList<QSvnStatusItem> items, bool error);

public slots:
    void repoBrowser(QString url, svn_opt_revision_t revision, bool recursion);
    void update(QStringList pathList, svn_opt_revision_t revision, svn_depth_t depth, bool depthIsSticky, bool ignoreExternals, bool allowUnverObstructions, bool addsAsModification, bool makeParents);
    void checkout(QString url, QString path, svn_opt_revision_t peg_revision, svn_opt_revision_t revision, svn_depth_t depth, bool ignore_externals, bool allow_unver_obstructions);
    void status(QString path, svn_opt_revision_t revision, svn_depth_t depth, svn_boolean_t get_all, svn_boolean_t update, svn_boolean_t no_ignore, svn_boolean_t ignore_externals, svn_boolean_t depth_as_sticky);

private:
    static svn_error_t * log_msg_func3(const char **log_msg,
                                       const char **tmp_file,
                                       const apr_array_header_t *commit_items,
                                       void *baton,
                                       apr_pool_t * pool);

    static void notify_func2(void *baton,
                             const svn_wc_notify_t *notify,
                             apr_pool_t *pool);

    static svn_error_t * conflict_func2(svn_wc_conflict_result_t **result,
                                        const svn_wc_conflict_description2_t *description,
                                        void *baton,
                                        apr_pool_t *result_pool,
                                        apr_pool_t *scratch_pool);

    static svn_error_t * status_funct(void *baton,
                                      const char *path,
                                      const svn_client_status_t *status,
                                      apr_pool_t *scratch_pool);

    static svn_error_t * cancel_func(void *baton);

    static void progress_func(apr_off_t progress,
                              apr_off_t total,
                              void *baton,
                              apr_pool_t *pool);

private:
    apr_pool_t *pool;
    svn_client_ctx_t *ctx;
    volatile QSVNOperationType m_operation;
    volatile bool cancelOperation;
};

class QSvnStatusItem
{

public:

    QSvnStatusItem()
    : m_nodeStatus(svn_wc_status_none)
    {

    }

    QSvnStatusItem(const QSvnStatusItem &other)
    {
        m_nodeStatus = other.m_nodeStatus;
        m_filename = other.m_filename;
    }

    QSvnStatusItem(const svn_client_status_t *item)
    {
        m_nodeStatus = item->node_status;
        m_filename = QString::fromUtf8(item->local_abspath);
    }

#ifdef Q_COMPILER_RVALUE_REFS
    QSvnStatusItem &operator=(QSvnStatusItem &&other)
    {
        qSwap(m_nodeStatus, other.m_nodeStatus);
        qSwap(m_filename, other.m_filename);

        return *this;
    }
#endif

    QSvnStatusItem &operator=(const QSvnStatusItem &other)
    {
        m_nodeStatus = other.m_nodeStatus;
        m_filename = other.m_filename;

        return *this;
    }

    svn_wc_status_kind m_nodeStatus;
    QString m_filename;
};

#endif // QSVN_H
