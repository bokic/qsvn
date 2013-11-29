#ifndef SVN_EXTENSION_H
#define SVN_EXTENSION_H

#include <glib-object.h>

G_BEGIN_DECLS

#define SVN_TYPE_EXTENSION         (svn_extension_get_type ())
#define SVN_EXTENSION(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), SVN_TYPE_EXTENSION, SvnExtension))
#define SVN_IS_EXTENSION(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), SVN_TYPE_EXTENSION))

typedef struct _SvnExtension       SvnExtension;
typedef struct _SvnExtensionClass  SvnExtensionClass;

struct _SvnExtension {
    GObject parent_slot;
};

struct _SvnExtensionClass {
    GObjectClass parent_slot;
};

GType svn_extension_get_type      (void);
void  svn_extension_register_type (GTypeModule *module);

G_END_DECLS

#endif

