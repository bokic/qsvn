#include "svn-extension.h"

#include <libnautilus-extension/nautilus-menu-provider.h>
#include <libnautilus-extension/nautilus-info-provider.h>
#include <gtk/gtk.h>

#include <svn_client.h>
#include <svn_dirent_uri.h>
#include <svn_cmdline.h>
#include <svn_pools.h>
#include <svn_config.h>
#include <svn_fs.h>
#include <svn_wc.h>

#include <apr_poll.h>

enum GSVNOperationType {
	G_SVN_EXTENSION_OPERATION_NO_FILES,
	G_SVN_EXTENSION_OPERATION_ONE_FILE_UNDER_SVN,
	G_SVN_EXTENSION_OPERATION_ONE_FILE_NOT_UNDER_SVN,
	G_SVN_EXTENSION_OPERATION_TWO_FILES_UNDER_SVN,
	G_SVN_EXTENSION_OPERATION_TWO_FILES_NOT_UNDER_SVN,
	G_SVN_EXTENSION_OPERATION_MORE_FILES_UNDER_SVN,
	G_SVN_EXTENSION_OPERATION_MORE_FILES_NOT_UNDER_SVN,
	G_SVN_EXTENSION_OPERATION_UNKNOWN
};

static gboolean svn_extension_are_items_under_svn  (GList                *files);
static void     svn_extension_checkout_callback    (GtkWidget            *widget,    GList     *files);
static void     svn_extension_update_callback      (GtkWidget            *widget,    GList     *files);
static void     svn_extension_commit_callback      (GtkWidget            *widget,    GList     *files);
static void     svn_extension_repo_browse_callback (GtkWidget            *widget,    GList     *files);
static GList   *svn_extension_get_background_items (NautilusMenuProvider *provider,  GtkWidget *window,       NautilusFileInfo *file_info);
static void     svn_extension_instance_init        (SvnExtension         *snv);
static void     svn_extension_class_init           (SvnExtensionClass    *class);
static GList   *svn_extension_get_file_items       (NautilusMenuProvider *provider, GtkWidget  *window,       GList *files);

static GType svn_type = 0;

static gboolean
svn_extension_are_items_under_svn(GList *files)
{
	svn_error_t *error = NULL;
 	svn_wc_status3_t *status = NULL;
 	svn_wc_context_t *ctx = NULL;
 	char *local_abspath = NULL;
 	apr_pool_t *result_pool = NULL;
 	apr_pool_t *scratch_pool = NULL;
 	guint items;
 	gboolean ret;

	if ((files == NULL)||((items = g_list_length (files)) == 0))
	{
		return FALSE;
	}

	result_pool = svn_pool_create(NULL);
	scratch_pool = svn_pool_create(NULL);

	error = svn_wc_context_create (&ctx, NULL, result_pool, scratch_pool);

	NautilusFileInfo *file_info = g_list_nth_data (files, 0); // TODO: Currently only checks the first item.

	local_abspath = g_filename_from_uri (nautilus_file_info_get_uri (file_info), NULL, NULL);

	if (local_abspath)
	{
		error = svn_wc_status3 (&status, ctx, local_abspath, result_pool, scratch_pool);

		g_free (local_abspath); local_abspath = NULL;

		ret = (error == NULL);
	}
	else
	{
		ret = FALSE;
	}

	apr_pool_destroy (scratch_pool);
	apr_pool_destroy (result_pool);

	return ret;
}

static void
svn_extension_checkout_callback(GtkWidget *widget, GList *files)
{
	gchar *command = NULL, *path = NULL;
	NautilusFileInfo *file_info = NULL;
	GError *error = NULL;
	GString *str = NULL;

	str = g_string_new ("qsvn checkout");

	if ((files != NULL)||(g_list_length (files) > 0))
	{
		file_info = g_list_nth_data (files, 0);

		path = g_filename_from_uri (nautilus_file_info_get_uri (file_info), NULL, NULL);

		g_string_append_printf (str, " \"%s\"", path);

		g_free (path); path = NULL;
	}

	command = g_string_free (str, FALSE); str = NULL;

	g_spawn_command_line_async (command, &error);

	g_free (command); command = NULL;
}

static void
svn_extension_update_callback(GtkWidget *widget, GList *files)
{
	NautilusFileInfo *file_info = NULL;
	gchar *command = NULL, *path = NULL;
	GError *error = NULL;
	GString *str = NULL;
	guint lcount, c;


	if ((files == NULL)||((lcount = g_list_length (files)) == 0))
	{
		return;
	}

	str = g_string_new ("qsvn");

	str = g_string_append (str, " update");

	for (c = 0; c < lcount; c++)
	{
		file_info = g_list_nth_data (files, c);

		path = g_filename_from_uri (nautilus_file_info_get_uri (file_info), NULL, NULL);

		str = g_string_append (str, " \"");
		str = g_string_append (str, path);
		str = g_string_append (str, "\"");

		g_free (path); path = NULL;
	}

	command = g_string_free (str, FALSE); str = NULL;

	g_spawn_command_line_async (command, &error);

	g_free (command); command = NULL;
}

static void
svn_extension_commit_callback(GtkWidget *widget, GList *files)
{
	NautilusFileInfo *file_info = NULL;
	gchar *command = NULL, *path = NULL;
	GError *error = NULL;
	GString *str = NULL;
	guint lcount, c;


	if ((files == NULL)||((lcount = g_list_length (files)) == 0))
	{
		return;
	}

	str = g_string_new ("qsvn");

	str = g_string_append (str, " commit");

	for (c = 0; c < lcount; c++)
	{
		str = g_string_append (str, " \"");

		file_info = g_list_nth_data (files, c);

		path = g_filename_from_uri (nautilus_file_info_get_uri (file_info), NULL, NULL);

		str = g_string_append (str, path);

		g_free (path); path = NULL;

		str = g_string_append (str, "\"");
	}

	command = g_string_free (str, FALSE); str = NULL;

	g_spawn_command_line_async (command, &error);

	g_free (command); command = NULL;
}

static void
svn_extension_repo_browse_callback(GtkWidget *widget, GList *files)
{
	gchar *command = NULL;
	GError *error = NULL;
	GString *str = NULL;


	str = g_string_new ("qsvn");

	g_string_append (str, " browse");

	command = g_string_free (str, FALSE); str = NULL;

	g_spawn_command_line_async (command, &error);

	g_free (command); command = NULL;
}

static void
svn_extension_updatetorevision_callback(GtkWidget *widget, GList *files)
{
	gchar *command = NULL;
	GError *error = NULL;
	GString *str = NULL;


	str = g_string_new ("qsvn");

	g_string_append (str, " update-to-revision");

	command = g_string_free (str, FALSE); str = NULL;

	g_spawn_command_line_async (command, &error);

	g_free (command); command = NULL;
}

static enum GSVNOperationType
svn_extension_get_operation_type(GList *files)
{
	guint items;

	if ((files == NULL)||((items = g_list_length (files)) == 0))
	{
		return G_SVN_EXTENSION_OPERATION_NO_FILES;
	}

	if (items == 1)
	{
		if (svn_extension_are_items_under_svn (files) == TRUE)
		{
			return G_SVN_EXTENSION_OPERATION_ONE_FILE_UNDER_SVN;
		}
		else
		{
			return G_SVN_EXTENSION_OPERATION_ONE_FILE_NOT_UNDER_SVN;
		}
	}
	else  if (items == 2)
	{
		if (svn_extension_are_items_under_svn (files) == TRUE)
		{
			return G_SVN_EXTENSION_OPERATION_TWO_FILES_UNDER_SVN;
		}
		else
		{
			return G_SVN_EXTENSION_OPERATION_TWO_FILES_NOT_UNDER_SVN;
		}
	}
	else
	{
		if (svn_extension_are_items_under_svn (files) == TRUE)
		{
			return G_SVN_EXTENSION_OPERATION_MORE_FILES_UNDER_SVN;
		}
		else
		{
			return G_SVN_EXTENSION_OPERATION_MORE_FILES_NOT_UNDER_SVN;
		}
	}
}

static GList *
svn_extension_create_menu_no_files(GList *files)
{
	NautilusMenuItem *item             = NULL;
	GList            *items            = NULL;
	NautilusMenu     *submenu          = NULL;

	items = g_list_append (NULL, nautilus_menu_item_new ("NautilusSVN::separator1", "-", "", ""));

	//<!---
    item = nautilus_menu_item_new ("NautilusSVN::checkout",
                                  "SVN _Checkout...",
                                  "Checkout project from SVN server",
                                  "svn-checkout");

	g_signal_connect (item,
			          "activate",
			          G_CALLBACK (svn_extension_checkout_callback),
			          nautilus_file_info_list_copy (files));

	items = g_list_append (items, item);
	//--->

	submenu = nautilus_menu_new ();

	//<!---
	item = nautilus_menu_item_new ("NautilusSVN::repo-browser",
                                  "Repo-browser...",
                                  "Repository browse",
                                  "svn-repo-browser");

	g_signal_connect (item,
			          "activate",
			          G_CALLBACK (svn_extension_repo_browse_callback),
			          nautilus_file_info_list_copy (files));

	nautilus_menu_append_item (submenu, item);
	//--->

	item = nautilus_menu_item_new ("NautilusSVN::submenu1",
	                               "QSVN",
	                               "qsvn",
	                               "svn-submenu");

	nautilus_menu_item_set_submenu (item, submenu);

	items = g_list_append (items, item);

	items = g_list_append (items, nautilus_menu_item_new ("NautilusSVN::separator2", "-", "", ""));

    return items;
}

static GList *
svn_extension_create_menu_one_file_under_svn(GList *files)
{
	NautilusMenuItem *item             = NULL;
	GList            *items            = NULL;
	NautilusMenu     *submenu          = NULL;

	items = g_list_append (NULL, nautilus_menu_item_new ("NautilusSVN::separator1", "-", "", ""));

	//<!---
    item = nautilus_menu_item_new ("NautilusSVN::update",
                                  "SVN _Update...",
                                  "Updates local project from SVN server",
                                  "svn-update");

	g_signal_connect (item,
			          "activate",
			          G_CALLBACK (svn_extension_update_callback),
			          nautilus_file_info_list_copy (files));

	items = g_list_append (items, item);
	//--->

	//<!---
    item = nautilus_menu_item_new ("NautilusSVN::commit",
                                  "SVN _Commit...",
                                  "Saves local changes to SVN server",
                                  "svn-commit");

	g_signal_connect (item,
			          "activate",
			          G_CALLBACK (svn_extension_commit_callback),
			          nautilus_file_info_list_copy (files));

	items = g_list_append (items, item);
	//--->

	submenu = nautilus_menu_new ();

    ////////////////////////////////////////////////////////////////////////////

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::showlog",
                                  "Show log",
                                  "showlog...",
                                  "svn-showlog");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_showlog_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    nautilus_menu_append_item (submenu, nautilus_menu_item_new ("NautilusSVN::sub_separator1", "-", "", ""));

	//<!---
	item = nautilus_menu_item_new ("NautilusSVN::repo-browser",
                                  "Repository browser",
                                  "repo-browser...",
                                  "svn-repo-browser");

	g_signal_connect (item,
			          "activate",
			          G_CALLBACK (svn_extension_repo_browse_callback),
			          nautilus_file_info_list_copy (files));

	nautilus_menu_append_item (submenu, item);
	//--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::checkformodifications",
                                  "Check for modifications",
                                  "check for modifications...",
                                  "svn-checkformodifications");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_checkformodifications_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::revisiongraph",
                                  "Revision graph",
                                  "revision graph...",
                                  "svn-revisiongraph");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_revisiongraph_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    nautilus_menu_append_item (submenu, nautilus_menu_item_new ("NautilusSVN::sub_separator2", "-", "", ""));

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::resolve",
                                  "Resolve...",
                                  "resolve...",
                                  "svn-resolve");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_resolve_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::updatetorevision",
                                  "Update to revision...",
                                  "update to revision...",
                                  "svn-updatetorevision");

    g_signal_connect (item,
                      "activate",
                      G_CALLBACK (svn_extension_updatetorevision_callback),
                      nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::revert",
                                  "Revert...",
                                  "revert...",
                                  "svn-revert");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_revert_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::cleanup",
                                  "Cleanup...",
                                  "cleanup...",
                                  "svn-cleanup");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_cleanup_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::getlock",
                                  "Get lock...",
                                  "get lock...",
                                  "svn-getlock");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_getlock_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::releaselock",
                                  "Release lock...",
                                  "release lock...",
                                  "svn-releaselock");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_releaselock_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    nautilus_menu_append_item (submenu, nautilus_menu_item_new ("NautilusSVN::sub_separator3", "-", "", ""));

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::branchtag",
                                  "Branch/tag...",
                                  "branch/tag...",
                                  "svn-branchtag");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_branchtag_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::switch",
                                  "Switch...",
                                  "switch...",
                                  "svn-switch");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_switch_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::merge",
                                  "Merge...",
                                  "merge...",
                                  "svn-merge");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_merge_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::export",
                                  "Export...",
                                  "export...",
                                  "svn-export");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_export_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::relocate",
                                  "Relocate...",
                                  "relocate...",
                                  "svn-relocate");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_relocate_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    nautilus_menu_append_item (submenu, nautilus_menu_item_new ("NautilusSVN::sub_separator4", "-", "", ""));

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::add",
                                  "Add...",
                                  "add...",
                                  "svn-add");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_add_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::ignorelist",
                                  "Unversion and add to ignore list",
                                  "unversion and add to ignore list...",
                                  "svn-ignorelist");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_createpatch_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    nautilus_menu_append_item (submenu, nautilus_menu_item_new ("NautilusSVN::sub_separator5", "-", "", ""));

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::createpatch",
                                  "Create patch...",
                                  "create patch...",
                                  "svn-createpatch");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_createpatch_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::applypatch",
                                  "Apply patch...",
                                  "apply patch...",
                                  "svn-applypatch");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_applypatch_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::properties",
                                  "Properties",
                                  "properties...",
                                  "svn-properties");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_properties_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    nautilus_menu_append_item (submenu, nautilus_menu_item_new ("NautilusSVN::sub_separator6", "-", "", ""));

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::settings",
                                  "Settings",
                                  "settings...",
                                  "svn-settings");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_settings_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::help",
                                  "Help",
                                  "help...",
                                  "svn-help");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_help_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

    //<!---
    item = nautilus_menu_item_new ("NautilusSVN::about",
                                  "About",
                                  "about...",
                                  "svn-about");

    //g_signal_connect (item,
    //                  "activate",
    //                  G_CALLBACK (svn_extension_about_callback),
    //                  nautilus_file_info_list_copy (files));

    nautilus_menu_append_item (submenu, item);
    //--->

	item = nautilus_menu_item_new ("NautilusSVN::submenu1",
	                               "QSVN",
	                               "qsvn",
	                               "svn-submenu");

	nautilus_menu_item_set_submenu (item, submenu);

	items = g_list_append (items, item);

    ////////////////////////////////////////////////////////////////////////////

	items = g_list_append (items, nautilus_menu_item_new ("NautilusSVN::separator2", "-", "", ""));

    return items;
}

static GList *
svn_extension_get_background_items (NautilusMenuProvider  *provider,
                    GtkWidget             *window,
                    NautilusFileInfo      *file_info)
{
    return NULL;
}

static GList *
svn_extension_get_file_items (NautilusMenuProvider *provider,
				  GtkWidget            *window,
				  GList                *files)
{
	switch(svn_extension_get_operation_type(files))
	{
	case G_SVN_EXTENSION_OPERATION_ONE_FILE_NOT_UNDER_SVN:
		return svn_extension_create_menu_no_files(files);
	case G_SVN_EXTENSION_OPERATION_ONE_FILE_UNDER_SVN:
		return svn_extension_create_menu_one_file_under_svn(files);
	default:
		return NULL;
	}
}

NautilusOperationResult svn_extension_update_file_info (NautilusInfoProvider     *provider,
                                                        NautilusFileInfo         *file,
                                                        GClosure                 *update_complete,
                                                        NautilusOperationHandle **handle)
{
    svn_error_t *error = NULL;
    svn_wc_status3_t *status = NULL;
    svn_wc_context_t *ctx = NULL;
    gchar *filename = NULL;
    char *emblem = NULL;
    apr_pool_t *result_pool = NULL;
    apr_pool_t *scratch_pool = NULL;

    if (file)
    {
        error = NULL;
        filename = g_filename_from_uri (nautilus_file_info_get_uri (file), NULL, NULL);

        result_pool = svn_pool_create(NULL);
        scratch_pool = svn_pool_create(NULL);

        error = svn_wc_context_create (&ctx, NULL, result_pool, scratch_pool);

        if (!error)
        {
            error = svn_wc_status3 (&status, ctx, filename, result_pool, scratch_pool);

            if (!error)
            {
                switch(status->node_status)
                {
                case svn_wc_status_none:
                    break;
                case svn_wc_status_unversioned:
                    emblem = "qsvn-unversioned";
                    break;
                case svn_wc_status_normal:
                    emblem = "qsvn-normal";
                    break;
                case svn_wc_status_added:
                    emblem = "qsvn-added";
                    break;
                case svn_wc_status_missing:
                    emblem = "qsvn-unversioned"; // ????
                    break;
                case svn_wc_status_deleted:
                    emblem = "qsvn-deleted";
                    break;
                case svn_wc_status_replaced:
                    emblem = "qsvn-unversioned"; // ????
                    break;
                case svn_wc_status_modified:
                    emblem = "qsvn-modified";
                    break;
                case svn_wc_status_merged:
                    emblem = "qsvn-unversioned"; // ????
                    break;
                case svn_wc_status_conflicted:
                    emblem = "qsvn-conflict";
                    break;
                case svn_wc_status_ignored:
                    emblem = "qsvn-ignored";
                    break;
                case svn_wc_status_obstructed:
                    emblem = "qsvn-unversioned"; // ????
                    break;
                case svn_wc_status_external:
                    emblem = "qsvn-unversioned"; // ????
                    break;
                case svn_wc_status_incomplete:
                    emblem = "qsvn-unversioned"; // ????
                    break;
                default:
                    emblem = "qsvn-unversioned"; // ????
                }
            }
        }

        apr_pool_destroy (scratch_pool);
        apr_pool_destroy (result_pool);

        g_free (filename); filename = NULL;

        if (emblem)
        {
            nautilus_file_info_add_emblem(file, emblem);
        }
    }

    return NAUTILUS_OPERATION_COMPLETE;
}

void svn_extension_cancel_update (NautilusInfoProvider     *provider,
                                  NautilusOperationHandle  *handle)
{

}

static void
svn_extension_menu_provider_iface_init (NautilusMenuProviderIface *iface)
{
	iface->get_background_items = svn_extension_get_background_items;
	iface->get_file_items = svn_extension_get_file_items;
}

static void
svn_extension_info_provider_iface_init (NautilusInfoProviderIface *iface)
{
    iface->update_file_info = svn_extension_update_file_info;
    iface->cancel_update = svn_extension_cancel_update;
}

static void
svn_extension_instance_init (SvnExtension *svn)
{
}

static void
svn_extension_class_init (SvnExtensionClass *class)
{
}

GType
svn_extension_get_type (void)
{
	return svn_type;
}

void
svn_extension_register_type (GTypeModule *module)
{
	svn_cmdline_init("svn-nautilus", NULL);

	static const GTypeInfo info = {
					sizeof (SvnExtensionClass),
					(GBaseInitFunc) NULL,
					(GBaseFinalizeFunc) NULL,
					(GClassInitFunc) svn_extension_class_init,
					NULL,
					NULL,
					sizeof (SvnExtension),
					0,
					(GInstanceInitFunc) svn_extension_instance_init,
					NULL
			};

	static const GInterfaceInfo menu_provider_iface_info = {
		(GInterfaceInitFunc) svn_extension_menu_provider_iface_init,
		NULL,
		NULL,
	};

    static const GInterfaceInfo info_provider_iface_info = {
        (GInterfaceInitFunc) svn_extension_info_provider_iface_init,
        NULL,
        NULL,
    };

	svn_type = g_type_module_register_type (module, G_TYPE_OBJECT, "svnExtension", &info, 0);

	g_type_module_add_interface (module,
					 svn_type,
					 NAUTILUS_TYPE_MENU_PROVIDER,
					 &menu_provider_iface_info);

    g_type_module_add_interface (module,
                     svn_type,
                     NAUTILUS_TYPE_INFO_PROVIDER,
                     &info_provider_iface_info);

}
