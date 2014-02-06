#include "svn-dir-status.h"

#include "svn_client.h"
#include "svn_dirent_uri.h"
#include "svn_cmdline.h"
#include "svn_pools.h"
#include "svn_config.h"
#include "svn_fs.h"

static int status_added;
static int status_conflicted;
static int status_deleted;
static int status_ignored;
static int status_locked;
static int status_modified;
static int status_normal;
static int status_readonly;
static int status_unversioned;

svn_error_t * status_funct(void *baton,
                           const char *path,
                           const svn_client_status_t *status,
                           apr_pool_t *scratch_pool)
{
    const char *requested_path = (const char *)baton;

    switch(status->node_status)
    {
    case svn_wc_status_none:
        break;
    case svn_wc_status_unversioned:
        if (strcmp(requested_path, status->local_abspath) == 0)
        {
            status_unversioned = TRUE;
        }
        else
        {
            status_normal = TRUE;
        }
        break;
    case svn_wc_status_normal:
        status_normal = TRUE;
        break;
    case svn_wc_status_modified:
    case svn_wc_status_replaced:
        status_modified = TRUE;
        break;
    case svn_wc_status_deleted:
        if (strcmp(requested_path, status->local_abspath) == 0)
        {
            status_deleted = TRUE;
        }
        else
        {
            status_normal = TRUE;
        }
        break;
    case svn_wc_status_conflicted:
        status_conflicted = TRUE;
        break;
    case svn_wc_status_ignored:
        if (strcmp(requested_path, status->local_abspath) == 0)
        {
            status_ignored = TRUE;
        }
        else
        {
            status_normal = TRUE;
        }
        break;
    default:
        break;
    }

    return NULL;
}

enum path_status svn_get_dir_status(const char *path)
{
    apr_pool_t *pool;
    svn_client_ctx_t *ctx;
    svn_opt_revision_t rev;
    svn_error_t *err;

    pool = svn_pool_create(NULL);

    err = svn_fs_initialize(pool);

    err = svn_client_create_context(&ctx, pool);

    err = svn_config_get_config(&(ctx->config), NULL, pool);

    apr_array_header_t *providers = apr_array_make (pool, 0, sizeof (svn_auth_provider_object_t *));
    svn_auth_open (&ctx->auth_baton, providers, pool);

    apr_pool_t *scratch = svn_pool_create(pool);

    status_added = FALSE;
    status_conflicted = FALSE;
    status_deleted = FALSE;
    status_ignored = FALSE;
    status_locked = FALSE;
    status_modified = FALSE;
    status_normal = FALSE;
    status_readonly = FALSE;
    status_unversioned = FALSE;

    rev.kind = svn_opt_revision_working;
    rev.value.number = 0;

    err = svn_client_status5(NULL, ctx, path, &rev, svn_depth_infinity, TRUE, FALSE, TRUE, TRUE, TRUE, NULL, &status_funct, (void *)path, scratch);

    apr_pool_destroy(scratch);

    apr_pool_destroy(pool);

    if (status_conflicted)
    {
        return STATUS_CONFLICTED;
    }

    if (status_locked)
    {
        return STATUS_LOCKED;
    }

    if (status_added)
    {
        return STATUS_ADDED;
    }

    if (status_readonly)
    {
        return STATUS_READONLY;
    }

    if (status_modified)
    {
        return STATUS_MODIFIED;
    }

    if (status_ignored)
    {
        return STATUS_IGNORED;
    }

    if (status_unversioned)
    {
        return STATUS_UNVERSIONED;
    }

    if (status_normal)
    {
        return STATUS_NORMAL;
    }


    return STATUS_NO_CHANGE;
}
