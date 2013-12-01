#include <gtk/gtk.h>

#include "svn-extension.h"

static GType type_list[1] = {0};

void
nautilus_module_initialize(GTypeModule *module)
{
    svn_extension_register_type(module);
    type_list[0] = SVN_TYPE_EXTENSION;
}

void
nautilus_module_shutdown(void)
{
}

void
nautilus_module_list_types(const GType **types, int *num_types)
{
    *types = type_list;
    *num_types = G_N_ELEMENTS(type_list);
}
