#include "svn-extension.h"

#include <nautilus-menu-provider.h>
#include <gtk/gtk.h>

#include <string.h> /* For strcmp */

static void   svn_extension_instance_init  (SvnExtension         *snv);
static void   svn_extension_class_init     (SvnExtensionClass    *class);
static GList *svn_extension_get_file_items (NautilusMenuProvider *provider,
                                            GtkWidget            *window,
                                            GList                *files);

static GType svn_type = 0;

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
    NautilusMenuItem *item;
    GList            *scan = NULL;
    GList            *items = NULL;

    if (files == NULL)
        return NULL;

    __asm("int $3");

    //g_type_init();

    gboolean svn_checkout_only = FALSE;

    if ((g_list_length(files) == 1)&&(nautilus_file_info_is_directory(files->data)))
    {
        GString *str = g_string_new(nautilus_file_info_get_uri(files->data));
        GString *str2 = g_string_append(str, "/.svn");

        gchar* strbuf = g_string_free(str2, FALSE);

        GFile *dir = g_file_new_for_path(strbuf);

        if (g_file_query_exists(dir, NULL) == TRUE)
        {
            GFileType type = g_file_query_file_type(dir, G_FILE_QUERY_INFO_NONE, NULL);

            if (type != G_FILE_TYPE_DIRECTORY)
            {
                svn_checkout_only = TRUE;
            }
        }
        else
        {
            svn_checkout_only = TRUE;
        }
    }

    if(svn_checkout_only)
    {
        item = nautilus_menu_item_new ("NautilusSVN::separator1", "-", "", "");
        items = g_list_prepend (items, item);

        item = nautilus_menu_item_new ("NautilusSVN::checkout",
                                       "SVN _Checkout...",
                                       "Checkout new project from SVN server",
                                       "svn-checkout");

        //g_signal_connect (item, "activate",
        //                  G_CALLBACK (checkout_callback),
        //                  nautilus_file_info_list_copy (files));

        items = g_list_prepend (items, item);

        item = nautilus_menu_item_new ("NautilusSVN::separator2", "-", "", "");
        items = g_list_prepend (items, item);
    }
    else
    {
        item = nautilus_menu_item_new ("NautilusSVN::update",
                                      "SVN _Update...",
                                      "Updates local files from SVN server",
                                      "svn-update");

        //g_signal_connect (item, "activate",
        //                  G_CALLBACK (sound_convert_callback),
        //                  nautilus_file_info_list_copy (files));

        items = g_list_prepend (items, item);

        item = nautilus_menu_item_new("NautilusSVN::commit",
                                      "SVN _Commit...",
                                      "Commit changes to SVN server",
                                      "svn-commit");

        //g_signal_connect (item, "activate",
        //                  G_CALLBACK (sound_convert_callback),
        //                  nautilus_file_info_list_copy (files));

        items = g_list_prepend (items, item);
    }

    items = g_list_reverse (items);

    return items;
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
