/* fichier d'en t�te gsbcalendar.h */
GtkWidget *gsb_calendar_new ( GtkWidget *entry ) ;
gboolean clavier_calendrier ( GtkCalendar *pCalendar,
			      GdkEventKey *ev,
			      GtkWidget *entry );
void date_selection ( GtkCalendar *pCalendar,
		      GtkWidget *entry );
gchar *gsb_today ( void ) ;

