#ifndef _GSB_FILE_CONFIG_H
#define _GSB_FILE_CONFIG_H (1)

#include <glib.h>

#define ETAT_WWW_BROWSER "xdg-open"

/* START_DECLARATION */
gboolean grisbi_conf_load_app_config (void);
gboolean grisbi_conf_save_app_config (void);
/* END_DECLARATION */

#endif
