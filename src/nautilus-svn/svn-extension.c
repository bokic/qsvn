#include "svn-extension.h"

#include <nautilus-menu-provider.h>
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

	if ((files == NULL)||((items = g_list_length(files)) == 0))
	{
		return FALSE;
	}

	result_pool = svn_pool_create(NULL);
	scratch_pool = svn_pool_create(NULL);

	error = svn_wc_context_create (&ctx, NULL, result_pool, scratch_pool);

	NautilusFileInfo *file_info = g_list_nth_data (files, 0); // TODO: Currently only checks the first item.

	local_abspath = g_filename_from_uri (nautilus_file_info_get_uri (file_info), NULL, NULL);

	error = svn_wc_status3 (&status, ctx, local_abspath, result_pool, scratch_pool);

	g_free (local_abspath); local_abspath = NULL;

	apr_pool_destroy(scratch_pool);
	apr_pool_destroy(result_pool);

	return (error == NULL);
}

static void
svn_extension_checkout_callback(GtkWidget *widget, GList *files)
{

}

static void
svn_extension_update_callback(GtkWidget *widget, GList *files)
{
	NautilusFileInfo *file_info = NULL;
	gchar *command = NULL, *path = NULL;
	GError *error = NULL;
	GString *str = NULL;
	guint lcount, c;


	if ((files == NULL)||((lcount = g_list_length(files)) == 0))
	{
		return;
	}

	str = g_string_new ("qsvn");

	str = g_string_append (str, " update");

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
svn_extension_commit_callback(GtkWidget *widget, GList *files)
{
	NautilusFileInfo *file_info = NULL;
	gchar *command = NULL, *path = NULL;
	GError *error = NULL;
	GString *str = NULL;
	guint lcount, c;


	if ((files == NULL)||((lcount = g_list_length(files)) == 0))
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

static GList *
svn_extension_get_background_items (NautilusMenuProvider  *provider,
					GtkWidget             *window,
					NautilusFileInfo      *file_info)
{
	return NULL;
}

static enum GSVNOperationType
svn_extension_get_operation_type(GList *files)
{
	guint items;

	if ((files == NULL)||((items = g_list_length(files)) == 0))
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

static void
svn_extension_menu_provider_iface_init (NautilusMenuProviderIface *iface)
{
	iface->get_background_items = svn_extension_get_background_items;
	iface->get_file_items = svn_extension_get_file_items;
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
			};

	static const GInterfaceInfo menu_provider_iface_info = {
		(GInterfaceInitFunc) svn_extension_menu_provider_iface_init,
		NULL,
		NULL,
	};

	svn_type = g_type_module_register_type (module, G_TYPE_OBJECT, "svnExtension", &info, 0);

	g_type_module_add_interface (module,
					 svn_type,
					 NAUTILUS_TYPE_MENU_PROVIDER,
					 &menu_provider_iface_info);
}
