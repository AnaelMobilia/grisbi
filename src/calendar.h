/* fichier d'en t�te gsbcalendar.h */

/* Constantes utilis�es par la fonction inc_dec_date */
# define ONE_DAY 1
# define ONE_WEEK 7
# define ONE_MONTH 30
# define ONE_YEAR 365

GtkWidget *gsb_calendar_new ( GtkWidget *entry ) ;
gboolean clavier_calendrier ( GtkCalendar *pCalendar,
			      GdkEventKey *ev,
			      GtkWidget *entry );
void date_selection ( GtkCalendar *pCalendar,
		      GtkWidget *entry );
gchar *gsb_today ( void ) ;
gboolean format_date ( GtkWidget *entree );
void inc_dec_date ( GtkWidget *entree, gint demande );
void ferme_calendrier ( GtkWidget *entree );

double gsb_strtod ( char *nptr, char **endptr );
