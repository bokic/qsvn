#include "qsvn.h"
#include "qsvnpool.h"
#include "qsvnerror.h"

#include <svn_props.h>


struct log_msg_baton3
{
  const char *message;  /* the message. */
  const char *message_encoding; /* the locale/encoding of the message. */
  const char *base_dir; /* the base directory for an external edit. UTF-8! */
  const char *tmpfile_left; /* the tmpfile left by an external edit. UTF-8! */
  apr_pool_t *pool; /* a pool. */
};

QSvn::QSvn(QObject *parent)
    : QObject(parent)
    , pool(nullptr)
    , ctx(nullptr)
    , m_operation(QSVNOperationNone)
    , m_cancelOperation(false)
    , m_validUserPass(false)
    , m_saveCredentials(false)
    , m_localVar(nullptr)
{
}

QSvn::~QSvn()
{
    clearCredentials();

    apr_pool_destroy(pool);

    ctx = nullptr;
    pool = nullptr;
}

void QSvn::init()
{
    svn_error_t *err = nullptr;

    pool = svn_pool_create(NULL);

    err = svn_fs_initialize(pool);
    if (err)
    {
        emit error(tr("Error calling svn_fs_initialize."));

        return;
    }

    err = svn_config_ensure(NULL, pool);
    if (err)
    {
        emit error(tr("Error calling svn_config_ensure."));

        return;
    }

    err = svn_client_create_context(&ctx, pool);
    if (err)
    {
        emit error(tr("Error calling svn_client_create_context."));

        return;
    }

    err = svn_config_get_config(&(ctx->config), NULL, pool);
    if (err)
    {
        emit error(tr("Error calling svn_config_get_config."));

        return;
    }

    ctx->log_msg_func3 = log_msg_func3;
    ctx->log_msg_baton3 = nullptr;
    ctx->notify_func2 = notify_func2;
    ctx->notify_baton2 = this;
    ctx->notify_func = nullptr;
    ctx->notify_baton = this;
    ctx->conflict_func = nullptr;
    ctx->conflict_baton = this;
    ctx->conflict_func2 = conflict_func2;
    ctx->conflict_baton2 = this;
    ctx->cancel_func = cancel_func;
    ctx->cancel_baton = this;
    ctx->progress_func = progress_func;
    ctx->progress_baton = this;
    ctx->client_name = "qsvn";

    svn_auth_provider_object_t *provider;
    apr_array_header_t *providers = apr_array_make (pool, 3, sizeof (svn_auth_provider_object_t *));

    svn_auth_get_platform_specific_client_providers (&providers, nullptr, pool);

    svn_auth_get_simple_prompt_provider (&provider,
                                         &svn_login_callback,
                                         this, /* baton */
                                         3, /* retry limit */
                                         pool);
    APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;

    /* Register the auth-providers into the context's auth_baton. */
    svn_auth_open (&ctx->auth_baton, providers, pool);
}

void QSvn::cancel()
{
    m_cancelOperation = true;
}

QSvn::QSVNOperationType QSvn::operation()
{
    return m_operation;
}

bool QSvn::isBusy()
{
    return m_operation != QSVNOperationNone;
}

QString QSvn::urlFromPath(const QString &path)
{
    QString ret;
    svn_error_t *err = nullptr;
    QSvnPool localpool(pool);

    svn_opt_revision_t rev;
    rev.kind = svn_opt_revision_working;
    rev.value.number = 0;

    err = svn_client_status5(nullptr,
                             ctx,
                             path.toUtf8().constData(),
                             &rev,
                             svn_depth_empty,
                             true,
                             false,
                             true,
                             true,
                             false,
                             nullptr,
                             [](void *baton, const char *path, const svn_client_status_t *status, apr_pool_t *scratch_pool) -> svn_error_t * {
                                Q_UNUSED(path);
                                Q_UNUSED(status);
                                Q_UNUSED(scratch_pool);

                                QString *ret = (QString *)baton;

                                *ret = QString::fromUtf8(status->repos_root_url);

                                if (strlen(status->repos_relpath) > 0)
                                {
                                    *ret += "/" + QString::fromUtf8(status->repos_relpath);
                                }

                                return SVN_NO_ERROR;
                             },
                             (void *)&ret,
                             localpool);

    if (err)
    {
        emit error(err->message);
    }

    return ret;
}

void QSvn::clearCredentials()
{
    m_validUserPass = false;

    m_username.fill(0); m_username.clear();
    m_password.fill(0); m_password.clear();
}

QString QSvn::username()
{
    return m_username;
}

QString QSvn::password()
{
    return m_password;
}

bool QSvn::shouldSaveCredentials()
{
    return m_saveCredentials;
}

void QSvn::setCredentials(const QString &username, const QString &password, bool saveCredentials, bool validUserPass)
{
    clearCredentials();

    m_username = username;
    m_password = password;
    m_saveCredentials = saveCredentials;

    m_validUserPass = validUserPass;
}

bool QSvn::validCredentials()
{
    return m_validUserPass;
}

void QSvn::repoBrowser(QString url, svn_opt_revision_t revision, bool recursion)
{
    QRepoBrowserResult ret;
    QSvnPool localpool(pool);
    svn_opt_revision_t peg;

    m_operation = QSvn::QSVNOperationRepoBrowser;
    m_cancelOperation = false;

    peg.kind = svn_opt_revision_unspecified;
    peg.value.number = 0;

    const char *l_url = svn_uri_canonicalize(url.toUtf8().constData(), localpool);

    m_localVar = &ret;

    svn_depth_t depth = recursion?svn_depth_infinity:svn_depth_immediates;

    svn_error_t *err = svn_client_list2(l_url,
                                        &peg,
                                        &revision,
                                        depth,
                                        0xFFFFFFFF, // all fields
                                        false,
                                        [](void *baton, const char *path, const svn_dirent_t *dirent, const svn_lock_t *lock, const char *abs_path, apr_pool_t *pool) -> svn_error_t * {

                                            Q_UNUSED(lock);
                                            Q_UNUSED(abs_path);
                                            Q_UNUSED(pool);

                                            if (strlen(path) != 0)
                                            {
                                                QRepoBrowserResult *ret = (QRepoBrowserResult *)(((QSvn *)baton)->m_localVar);
                                                QRepoBrowserFile file;

                                                file.filename = QString::fromUtf8(path);
                                                file.isdir = dirent->kind == svn_node_dir;
                                                file.revision = dirent->created_rev;
                                                file.author = QString::fromUtf8(dirent->last_author);
                                                file.size = dirent->size;
                                                file.modified = QDateTime::fromMSecsSinceEpoch(dirent->time / 1000);

                                                ret->files.append(file);
                                            }

                                            return SVN_NO_ERROR;
                                        },
                                        this,
                                        ctx,
                                        localpool);

    m_localVar = nullptr;

    emit repoBrowserResult(ret, QSvnError(err));
}

void QSvn::update(QStringList pathList, svn_opt_revision_t revision, svn_depth_t depth, bool depthIsSticky, bool ignoreExternals, bool allowUnverObstructions, bool addsAsModification, bool makeParents)
{
    svn_error_t *err = nullptr;
    QSvnPool localpool(pool);

    m_operation = QSvn::QSVNOperationUpdate;
    m_cancelOperation = false;

    apr_array_header_t *paths = apr_array_make (localpool, pathList.length(), sizeof(char *));

    for(const QString path : pathList)
    {
        APR_ARRAY_PUSH(paths, char *) = apr_pstrdup (localpool, path.toUtf8().constData());
    }

    m_cancelOperation = false;

    err = svn_client_update4(nullptr,
                             paths,
                             &revision,
                             depth,
                             depthIsSticky,
                             ignoreExternals,
                             allowUnverObstructions,
                             addsAsModification,
                             makeParents,
                             ctx,
                             localpool);

    emit finished(QSvnError(err));
}

void QSvn::checkout(QString url, QString path, svn_opt_revision_t peg_revision, svn_opt_revision_t revision, svn_depth_t depth, bool ignore_externals, bool allow_unver_obstructions)
{
    svn_error_t *err = nullptr;
    QSvnPool localpool(pool);
    svn_revnum_t result_rev;

    m_operation = QSvn::QSVNOperationCheckout;
    m_cancelOperation = false;

    err = svn_client_checkout3(&result_rev,
                               svn_uri_canonicalize(url.toUtf8().constData(), localpool),
                               path.toUtf8().constData(),
                               &peg_revision,
                               &revision,
                               depth,
                               ignore_externals,
                               allow_unver_obstructions,
                               ctx,
                               localpool);

    emit finished(QSvnError(err));
}

void QSvn::commit(QStringList pathlist, QString message, QStringList changelists, bool keepchangelist, svn_depth_t depth, bool keep_locks, QHash<QString, QString> revProps)
{
    svn_error_t *err = nullptr;
    QSvnPool localpool(pool);

    ctx->log_msg_baton3 = logMessage(message);

    err = svn_client_commit5 (
                             makePathList(pathlist, localpool),
                             depth,
                             keep_locks,
                             keepchangelist,
                             true,       // commit_as_operations
                             makeChangeListArray(changelists, localpool),
                             makeRevPropHash(revProps, localpool),
                             commit_func2,
                             this,
                             ctx,
                             localpool);

    ctx->log_msg_baton3 = nullptr;

    emit finished(QSvnError(err));
}

void QSvn::status(QString path, svn_opt_revision_t revision, svn_depth_t depth, svn_boolean_t get_all, svn_boolean_t update, svn_boolean_t no_ignore, svn_boolean_t ignore_externals, svn_boolean_t depth_as_sticky)
{
    svn_error_t *err = nullptr;
    QSvnPool localpool(pool);

    m_operation = QSvn::QSVNOperationStatus;
    m_cancelOperation = false;

    QList<QSvnStatusItem> items;

    err = svn_client_status5(nullptr,
                             ctx,
                             path.toUtf8().constData(),
                             &revision,
                             depth,
                             get_all,
                             update,
                             no_ignore,
                             ignore_externals,
                             depth_as_sticky,
                             nullptr,
                             status_funct,
                             (void *)&items,
                             localpool);

    emit statusFinished(items, QSvnError(err));
}

void QSvn::messageLog(QStringList locations, svn_opt_revision_t start, svn_opt_revision_t end, svn_opt_revision_t peg)
{
    apr_array_header_t *revision_ranges = nullptr;
    svn_error_t *err = nullptr;
    QSvnPool localpool(pool);

    m_operation = QSvn::QSVNOperationMessageLog;
    m_cancelOperation = false;

    QList<QMessageLogItem> list;

    apr_array_header_t *paths = apr_array_make (localpool, locations.count(), sizeof(const char *));
    foreach(const QString &location, locations)
    {
        APR_ARRAY_PUSH(paths, char *) = (char *)svn_uri_canonicalize(location.toUtf8().constData(), localpool);
    }

    m_localVar = &list;

    svn_opt_revision_range_t revision_range = {start, end};

    revision_ranges = apr_array_make (localpool, 1, sizeof(apr_array_header_t*));
    *(svn_opt_revision_range_t**)apr_array_push (revision_ranges) = &revision_range;

    err = svn_client_log5 (paths,
                           &peg,
                           revision_ranges,
                           0,    // limit
                           TRUE, // discover_changed_paths
                           TRUE, // strict_node_history
                           TRUE, // include merged revisions
                           nullptr, // revprops
                           [] (void *baton, svn_log_entry_t *log_entry, apr_pool_t *pool) -> svn_error_t * {
                                Q_UNUSED(pool);
                                QList<QMessageLogItem> *items = (QList<QMessageLogItem> *)(((QSvn *)baton)->m_localVar);

                                QMessageLogItem item;

                                char *author = nullptr;
                                char *date = nullptr;
                                char *message = nullptr;

                                if (log_entry->revprops)
                                {
                                    void *tmp = nullptr;

                                    tmp = apr_hash_get (log_entry->revprops, SVN_PROP_REVISION_AUTHOR, APR_HASH_KEY_STRING);
                                    if (tmp) author = *(char **)tmp;

                                    tmp = apr_hash_get (log_entry->revprops, SVN_PROP_REVISION_DATE, APR_HASH_KEY_STRING);
                                    if (tmp) date = *(char **)tmp;

                                    tmp = apr_hash_get (log_entry->revprops, SVN_PROP_REVISION_LOG, APR_HASH_KEY_STRING);
                                    if (tmp) message = *(char **)tmp;
                                }

                                item.revision = log_entry->revision;
                                item.author = QString::fromUtf8(author);
                                item.date = QDateTime::fromString(QString::fromUtf8(date), Qt::ISODate);
                                item.message = QString::fromUtf8(message);

                                items->append(item);

                               return SVN_NO_ERROR;
                           },
                           this, // receiver_baton
                           ctx,
                           localpool);

    m_localVar = nullptr;

    emit messageLogFinished(list, QSvnError(err));
}

svn_error_t * QSvn::log_msg_func3(const char **log_msg,
                                  const char **tmp_file,
                                  const apr_array_header_t *commit_items,
                                  void *baton,
                                  apr_pool_t *pool)
{
    Q_UNUSED(commit_items);

    log_msg_baton3 *lmb = (log_msg_baton3 *) baton;
    *tmp_file = NULL;
    if (lmb->message)
    {
        *log_msg = apr_pstrdup (pool, lmb->message);
    }

    return SVN_NO_ERROR;
}

void QSvn::notify_func2(void *baton,
                        const svn_wc_notify_t *notify,
                        apr_pool_t * pool)
{
    Q_UNUSED(notify);
    Q_UNUSED(pool);

    QSvn *svn = (QSvn *)baton;

    if (svn)
    {
        emit svn->notify(*notify);
    }
}

svn_error_t * QSvn::conflict_func2(svn_wc_conflict_result_t **result,
                                   const svn_wc_conflict_description2_t *description,
                                   void *baton,
                                   apr_pool_t *result_pool,
                                   apr_pool_t *scratch_pool)
{
    Q_UNUSED(result);
    Q_UNUSED(description);
    Q_UNUSED(baton);
    Q_UNUSED(result_pool);
    Q_UNUSED(scratch_pool);

    Q_UNIMPLEMENTED();

    return SVN_NO_ERROR;
}

svn_error_t * QSvn::commit_func2(const svn_commit_info_t *commit_info,
                                 void *baton,
                                 apr_pool_t *pool)
{
    Q_UNUSED(commit_info);
    Q_UNUSED(baton);
    Q_UNUSED(pool);

    Q_UNIMPLEMENTED();

    return NULL;
}

svn_error_t * QSvn::status_funct(void *baton,
                                 const char *path,
                                 const svn_client_status_t *status,
                                 apr_pool_t *scratch_pool)
{
    Q_UNUSED(path);
    Q_UNUSED(scratch_pool);

    QList<QSvnStatusItem> *items = (QList<QSvnStatusItem> *)baton;

    items->append(QSvnStatusItem(status));

    return SVN_NO_ERROR;
}

svn_error_t * QSvn::cancel_func(void *baton)
{
    QSvn *svn = (QSvn *)baton;

    if ((svn)&&(svn->m_cancelOperation))
    {
        return svn_error_create(SVN_ERR_CANCELLED, NULL, tr("User has canceled the operation.").toUtf8().constData());
    }

    return SVN_NO_ERROR;
}

void QSvn::progress_func(apr_off_t progress,
                         apr_off_t total,
                         void *baton,
                         apr_pool_t *pool)
{
    Q_UNUSED(pool);

    QSvn *svn = (QSvn *)baton;

    if (svn)
    {
        emit svn->progress((int)progress, (int)total);
    }
}

svn_error_t *QSvn::svn_login_callback(svn_auth_cred_simple_t **cred,
                                      void *baton,
                                      const char *realm,
                                      const char *username,
                                      svn_boolean_t may_save,
                                      apr_pool_t *pool)
{
    Q_UNUSED(realm);

    QSvn *sender = (QSvn *)baton;

    sender->setCredentials(QString::fromUtf8(username), "", may_save, false);

    emit sender->credentials();

    if (sender->validCredentials())
    {
        svn_auth_cred_simple_t * ret = (svn_auth_cred_simple_t *) apr_pcalloc (pool, sizeof (*ret));

        ret->username = apr_pstrdup(pool, sender->username().toUtf8().constData());
        ret->password = apr_pstrdup(pool, sender->password().toUtf8().constData());
        ret->may_save = sender->shouldSaveCredentials();

        *cred = ret;
    }

    return SVN_NO_ERROR;
}

void *QSvn::logMessage(QString message, char *baseDirectory)
{
    message.remove('\r');

    // TODO: Check the pool usage in this function(might delete pool member)
    log_msg_baton3 *baton = (log_msg_baton3 *) apr_palloc (pool, sizeof (log_msg_baton3));
    baton->message = apr_pstrdup(pool, message.toUtf8().constData());
    baton->base_dir = baseDirectory ? baseDirectory : "";

    baton->message_encoding = NULL;
    baton->tmpfile_left = NULL;
    baton->pool = pool;

    return baton;
}

apr_hash_t *QSvn::makeRevPropHash(const QHash<QString, QString> &revProps, apr_pool_t *pool)
{
    apr_hash_t * revprop_table = NULL;

    QList<QString> keys = revProps.keys();

    if (!keys.isEmpty())
    {
        revprop_table = apr_hash_make(pool);

        foreach(const QString &key, keys)
        {
            svn_string_t *k = svn_string_create(key.toUtf8().constData(), pool);
            svn_string_t *v = svn_string_create(revProps.value(key).toUtf8().constData(), pool);

            apr_hash_set (revprop_table, k, APR_HASH_KEY_STRING, v);
        }
    }

    return revprop_table;
}

apr_array_header_t *QSvn::makePathList(const QStringList &paths, apr_pool_t *pool)
{
    apr_array_header_t *ret = nullptr;

    ret = apr_array_make (pool, paths.count(), sizeof(const char *));

    foreach(const QString path, paths)
    {
        char *path_item = apr_pstrdup(pool, path.toUtf8().constData());
        APR_ARRAY_PUSH(ret, char *) = path_item;
    }

    return ret;
}

apr_array_header_t *QSvn::makeChangeListArray(const QStringList &changelists, apr_pool_t *pool)
{
    int count = changelists.length();

    // special case: the changelist array contains one empty string
    if ((count == 1)&&(changelists.at(0).isEmpty()))
    {
        count = 0;
    }

    apr_array_header_t * ret = apr_array_make (pool, count, sizeof(const char *));

    if (count == 0)
    {
        return ret;
    }

    foreach(const QString &item, changelists)
    {
        const char *new_item = apr_pstrdup(pool, item.toUtf8().constData());
        (*((const char **) apr_array_push(ret))) = new_item;
    }

    return ret;
}
