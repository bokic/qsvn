#include "qsvn.h"


QSvn::QSvn(QObject *parent)
    : QObject(parent)
    , pool(nullptr)
    , ctx(nullptr)
    , cancelOperation(false)
{
}

QSvn::~QSvn()
{
    ctx = nullptr;

    if (pool)
    {
        apr_pool_destroy(pool);
        pool = nullptr;
    }
}

void QSvn::init()
{
    svn_error_t *err;

    pool = svn_pool_create(NULL);

    err = svn_fs_initialize(pool);
    if (err)
    {
        emit error("Error calling svn_fs_initialize.");

        return;
    }

    err = svn_config_ensure(NULL, pool);
    if (err)
    {
        emit error("Error calling svn_config_ensure.");

        return;
    }

    err = svn_client_create_context(&ctx, pool);
    if (err)
    {
        emit error("Error calling svn_client_create_context.");

        return;
    }

    err = svn_config_get_config(&(ctx->config), NULL, pool);
    if (err)
    {
        emit error("Error calling svn_config_get_config.");

        return;
    }

    ctx->log_msg_func3 = log_msg_func3;
    ctx->log_msg_baton3 = this;
    ctx->notify_func2 = notify_func2;
    ctx->notify_baton2 = this;
    ctx->notify_func = nullptr;
    ctx->notify_baton = nullptr;
    ctx->conflict_func = nullptr;
    ctx->conflict_baton = nullptr;
    ctx->conflict_func2 = conflict_func2;
    ctx->conflict_baton2 = this;
    ctx->cancel_func = cancel_func;
    ctx->cancel_baton = this;
    ctx->progress_func = progress_func;
    ctx->progress_baton = this;
    ctx->client_name = "qsvn";

    svn_auth_provider_object_t *provider;
    apr_array_header_t *providers = apr_array_make (pool, 4, sizeof (svn_auth_provider_object_t *));

    svn_auth_get_simple_prompt_provider (&provider,
                                         NULL,
                                         NULL, /* baton */
                                         2, /* retry limit */ pool);
    APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;

    svn_auth_get_username_prompt_provider (&provider,
                                           NULL,
                                           NULL, /* baton */
                                           2, /* retry limit */ pool);
    APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;

    /* Register the auth-providers into the context's auth_baton. */
    svn_auth_open (&ctx->auth_baton, providers, pool);
}

void QSvn::cancel()
{
    cancelOperation = true;
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
    svn_error_t *err;

    svn_opt_revision_t rev;
    rev.kind = svn_opt_revision_working;
    rev.value.number = 0;

    apr_pool_t *scratch_pool = svn_pool_create(NULL);

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
                                QString *ret = (QString *)baton;

                                *ret = QString::fromUtf8(status->repos_root_url);

                                if (strlen(status->repos_relpath) > 0)
                                {
                                    *ret += "/" + QString::fromUtf8(status->repos_relpath);
                                }

                                return NULL;
                             },
                             (void *)&ret,
                             scratch_pool);

    apr_pool_destroy (scratch_pool);

    return ret;
}

void QSvn::repoBrowser(QString url, svn_opt_revision_t revision, bool recursion)
{
    QRepoBrowserResult ret;
    apr_hash_t *dirents;
    apr_hash_index_t *hi;

    m_operation = QSvn::QSVNOperationRepoBrowser;
    cancelOperation = false;

    const char *l_url = svn_uri_canonicalize(url.toUtf8().constData(), pool);

    // TODO: Replace svn_client_ls call with svn_client_list2
    svn_error_t *err = svn_client_ls (&dirents,
                                      l_url,
                                      &revision,
                                      recursion,
                                      ctx, pool);

    /*svn_error_t *err = svn_client_list2(l_url,
                                        &revision,
                                        &revision,
                                        svn_depth_infinity,
                                        SVN_DIRENT_ALL,
                                        FALSE,
                                        NULL,
                                        this,
                                        ctx, pool);*/


    //emit repoBrowserResult(QSvnFSList(pool, dirents), QSvnError(err, QObject::tr("svn_client_ls call failed.")));

    if (err)
    {
        ret.error = QObject::tr("svn_client_ls call failed.");

        emit repoBrowserResult(ret);

        return;
    }

    for (hi = apr_hash_first (pool, dirents); hi; hi = apr_hash_next (hi))
    {
        const char *entryname;
        svn_dirent_t *val;

        apr_hash_this (hi, (const void **)&entryname, NULL, (void **)&val);

        QRepoBrowserFile file;

        file.filename = QString::fromUtf8(entryname);
        file.isdir = val->kind == svn_node_dir;
        file.revision = val->created_rev;
        file.author = QString::fromUtf8(val->last_author);
        file.size = val->size;
        file.modified = QDateTime::fromMSecsSinceEpoch(val->time / 1000);

        ret.files.append(file);
    }

    emit repoBrowserResult(ret);
}

void QSvn::update(QStringList pathList, svn_opt_revision_t revision, svn_depth_t depth, bool depthIsSticky, bool ignoreExternals, bool allowUnverObstructions, bool addsAsModification, bool makeParents)
{
    svn_error_t *err;

    m_operation = QSvn::QSVNOperationUpdate;
    cancelOperation = false;

    apr_array_header_t *paths = apr_array_make(pool, 0, pathList.length());

    for(const QString path : pathList)
    {
        APR_ARRAY_PUSH(paths, const char *) = apr_pstrdup(pool, path.toUtf8().constData());
    }

    cancelOperation = false;

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
                             pool);

    emit finished(err == nullptr);
}

void QSvn::checkout(QString url, QString path, svn_opt_revision_t peg_revision, svn_opt_revision_t revision, svn_depth_t depth, bool ignore_externals, bool allow_unver_obstructions)
{
    svn_error_t *err;
    svn_revnum_t result_rev;

    m_operation = QSvn::QSVNOperationCheckout;
    cancelOperation = false;

    err = svn_client_checkout3(&result_rev,
                               url.toUtf8().constData(),
                               path.toUtf8().constData(),
                               &peg_revision,
                               &revision,
                               depth,
                               ignore_externals,
                               allow_unver_obstructions,
                               ctx,
                               pool);

    emit finished(err == nullptr);
}

void QSvn::status(QString path, svn_opt_revision_t revision, svn_depth_t depth, svn_boolean_t get_all, svn_boolean_t update, svn_boolean_t no_ignore, svn_boolean_t ignore_externals, svn_boolean_t depth_as_sticky)
{
    svn_error_t *err;

    m_operation = QSvn::QSVNOperationStatus;
    cancelOperation = false;

    apr_pool_t *scratch_pool = svn_pool_create(NULL);

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
                             scratch_pool);

    apr_pool_destroy (scratch_pool);

    emit statusFinished(items, err == nullptr);
}

svn_error_t * QSvn::log_msg_func3(const char **log_msg,
                                  const char **tmp_file,
                                  const apr_array_header_t *commit_items,
                                  void *baton,
                                  apr_pool_t * pool)
{
    Q_UNUSED(log_msg);
    Q_UNUSED(tmp_file);
    Q_UNUSED(commit_items);
    Q_UNUSED(pool);

    QSvn *svn = (QSvn *)baton;

    Q_UNUSED(svn);
    Q_UNIMPLEMENTED();

    /*log_msg_baton3 *lmb = (log_msg_baton3 *) baton;
    *tmp_file = NULL;
    if (lmb->message)
    {
        *log_msg = apr_pstrdup (pool, lmb->message);
    }*/

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
    Q_UNUSED(result_pool);
    Q_UNUSED(scratch_pool);

    QSvn *svn = (QSvn *)baton;

    Q_UNUSED(svn);
    Q_UNIMPLEMENTED();

    return SVN_NO_ERROR;
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

    if ((svn)&&(svn->cancelOperation))
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
