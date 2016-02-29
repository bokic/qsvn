#ifndef QSVN_H
#define QSVN_H

#include "qsvnerror.h"
#include <QStringList>
#include <QDateTime>
#include <QString>
#include <QObject>
#include <QList>

#include "svn_client.h"
#include "svn_dirent_uri.h"
#include "svn_cmdline.h"
#include "svn_pools.h"
#include "svn_config.h"
#include "svn_fs.h"

struct QMessageLogItem;

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
    enum QSVNOperationType {QSVNOperationNone, QSVNOperationRepoBrowser, QSVNOperationCommit, QSVNOperationUpdate, QSVNOperationCheckout, QSVNOperationStatus, QSVNOperationMessageLog};

    QSvn(QObject *parent=0);
    ~QSvn();
    void init();
    void cancel();
    QSVNOperationType operation();
    bool isBusy();
    QString urlFromPath(const QString &path);
    void clearCredentials();
    QString username();
    QString password();
    bool shouldSaveCredentials();
    void setCredentials(const QString &m_username, const QString &m_password, bool m_saveCredentials = false, bool m_validUserPass = true);
    bool validCredentials();
    QSvnError add(const QStringList &items, svn_depth_t depth, bool force, bool no_ignore, bool addparents);
    QSvnError remove(const QStringList &items, bool force, bool keep_local = true, const QString &message = "");

signals:
    void error(QString text);
    void finished(QSvnError error);
    void repoBrowserResult(QRepoBrowserResult items, QSvnError error);
    void logMsg();
    void notify(svn_wc_notify_t notify);
    void progress(int progress, int total);
    void statusFinished(QList<QSvnStatusItem> items, QSvnError error);
    void messageLogFinished(QList<QMessageLogItem> items, QSvnError error);
    void credentials();

public slots:
    void repoBrowser(QString url, svn_opt_revision_t revision, bool recursion);
    void update(QStringList pathList, svn_opt_revision_t revision, svn_depth_t depth, bool depthIsSticky, bool ignoreExternals, bool allowUnverObstructions, bool addsAsModification, bool makeParents);
    void checkout(QString url, QString path, svn_opt_revision_t peg_revision, svn_opt_revision_t revision, svn_depth_t depth, bool ignore_externals, bool allow_unver_obstructions);
    void commit(QStringList pathlist, QString message, QStringList changelists, bool keepchangelist, svn_depth_t depth, bool keep_locks, QHash<QString, QString> revProps);
    void status(QString path, svn_opt_revision_t revision, svn_depth_t depth, svn_boolean_t get_all, svn_boolean_t update, svn_boolean_t no_ignore, svn_boolean_t ignore_externals, svn_boolean_t depth_as_sticky);
    void messageLog(QStringList locations, svn_opt_revision_t start, svn_opt_revision_t end, svn_opt_revision_t peg);

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

    static svn_error_t * commit_func2(const svn_commit_info_t *commit_info,
                                      void *baton,
                                      apr_pool_t *pool);

    static svn_error_t * status_funct(void *baton,
                                      const char *path,
                                      const svn_client_status_t *status,
                                      apr_pool_t *scratch_pool);

    static svn_error_t * cancel_func(void *baton);

    static void progress_func(apr_off_t progress,
                              apr_off_t total,
                              void *baton,
                              apr_pool_t *pool);

    static svn_error_t * svn_login_callback(svn_auth_cred_simple_t **cred,
                                            void *baton,
                                            const char *realm,
                                            const char *username,
                                            svn_boolean_t may_save,
                                            apr_pool_t *pool);

    /// Private helper functions
    void *logMessage(QString message, char *baseDirectory = nullptr);
    static apr_array_header_t *makePathList(const QStringList &paths, apr_pool_t *pool);
    static apr_array_header_t *makeChangeListArray(const QStringList &changelists, apr_pool_t *pool);
    static apr_hash_t *makeRevPropHash(const QHash<QString, QString> &revProps, apr_pool_t *pool);

private:
    apr_pool_t *pool;
    svn_client_ctx_t *ctx;
    volatile QSVNOperationType m_operation;
    volatile bool m_cancelOperation;
    volatile bool m_validUserPass;
    QString m_username;
    QString m_password;
    volatile bool m_saveCredentials;

public:
    void *m_localVar;
};

class QSvnStatusItem
{

public:

    QSvnStatusItem()
    : m_nodeStatus(svn_wc_status_none)
    , m_kind(svn_node_none)
    , m_checked(false)
    {
    }

    QSvnStatusItem(const QSvnStatusItem &other)
    {
        m_nodeStatus = other.m_nodeStatus;
        m_filename = other.m_filename;
        m_kind = other.m_kind;
        m_checked = other.m_checked;
    }

    QSvnStatusItem(const svn_client_status_t *item)
    {
        m_nodeStatus = item->node_status;
        m_filename = QString::fromUtf8(item->local_abspath);
        m_kind = item->kind;
        m_checked = false;
    }

#ifdef Q_COMPILER_RVALUE_REFS
    QSvnStatusItem &operator=(QSvnStatusItem &&other)
    {
        qSwap(m_nodeStatus, other.m_nodeStatus);
        qSwap(m_filename, other.m_filename);
        qSwap(m_kind, other.m_kind);
        qSwap(m_checked, other.m_checked);

        return *this;
    }
#endif

    QSvnStatusItem &operator=(const QSvnStatusItem &other)
    {
        m_nodeStatus = other.m_nodeStatus;
        m_filename = other.m_filename;
        m_kind = other.m_kind;
        m_checked = other.m_checked;

        return *this;
    }

    svn_wc_status_kind m_nodeStatus;
    QString m_filename;
    svn_node_kind_t m_kind;
    bool m_checked;
};

struct QMessageLogItemOperation
{
public:
    char operation;
    QString filename;
};

struct QMessageLogItem
{
public:
    int revision;
    QString author;
    QDateTime date;
    QString message;
    QList<QMessageLogItemOperation> files;
};

#endif // QSVN_H
