/* ************************************************************************** */
/* Ce fichier s'occupe de la gestion des calendriers                          */
/* 			gsbcalendar.c                                         */
/*                                                                            */
/*     Copyright (C)	2000-2003 C�dric Auger (cedric@grisbi.org)	      */
/*			2004 Alain Portal (dionysos@grisbi.org) 	      */
/*			http://www.grisbi.org   			      */
/*                                                                            */
/*  This program is free software; you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation; either version 2 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*                                                                            */
/* ************************************************************************** */

#include <gnome.h>
#include <ctype.h>
#include "calendar.h"

#define SIZEOF_FORMATTED_STRING_DATE 11

/******************************************************************************/
/* Cr�e une fen�tre (popup) calendrier.                                       */
/* Cette fen�tre est � rattach�e � au widget (une entr�e de texte) que l'on   */
/* passe en param�tre et qui r�cup�rera une date, � moins que l'on abandonne  */
/* la saisie de la date par un appui sur la touche ESC lorsque le calendrier  */
/* est ouvert.                                                                */
/******************************************************************************/
GtkWidget *gsb_calendar_new ( GtkWidget *entry )
{
  GtkWidget *popup;
  GtkWidget *pVBox;
  GtkRequisition *taille_popup;
  gint x, y;
  GtkWidget *pCalendar;
  int jour, mois, annee;
  GtkWidget *bouton;
  GtkWidget *frame;

  /* cr�ation de la popup */

  popup = gtk_window_new ( GTK_WINDOW_POPUP );
  gtk_window_set_modal ( GTK_WINDOW ( popup ), TRUE );
  gtk_signal_connect_object ( GTK_OBJECT ( popup ),
			      "destroy",
			      GTK_SIGNAL_FUNC ( gdk_pointer_ungrab ),
			      GDK_CURRENT_TIME );

  /* cr�ation de l'int�rieur de la popup */

  frame = gtk_frame_new ( NULL );
  gtk_container_add ( GTK_CONTAINER ( popup ), frame );
  gtk_widget_show ( frame );

  pVBox = gtk_vbox_new ( FALSE, 5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( pVBox ), 5 );
  gtk_container_add ( GTK_CONTAINER ( frame ), pVBox );
  gtk_widget_show ( pVBox );

  if ( !( strlen ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entry )))) &&
	  sscanf ( ( gchar * ) gtk_entry_get_text ( GTK_ENTRY ( entry )),
		   "%d/%d/%d",
		   &jour,
		   &mois,
		   &annee )))
    sscanf ( gsb_today(),
	     "%d/%d/%d",
	     &jour,
	     &mois,
	     &annee);
      
  pCalendar = gtk_calendar_new();
  
  /* parce qu'il y a une diff�rence de type et un d�calage de valeur
     pour identifier le mois :
       - pour g_date, janvier = 1
       - pour gtk_calendar, janvier = 0 */
  gtk_calendar_select_month ( GTK_CALENDAR ( pCalendar ), mois - 1, annee );
  gtk_calendar_select_day ( GTK_CALENDAR ( pCalendar ), jour );
  gtk_calendar_display_options ( GTK_CALENDAR ( pCalendar ),
				 GTK_CALENDAR_SHOW_HEADING |
				 GTK_CALENDAR_SHOW_DAY_NAMES |
				 GTK_CALENDAR_WEEK_START_MONDAY );

  gtk_signal_connect ( GTK_OBJECT ( pCalendar ),
		       "day_selected_double_click",
		       GTK_SIGNAL_FUNC ( date_selection ),
		       entry );
  gtk_signal_connect ( GTK_OBJECT ( pCalendar ),
		       "key-press-event",
		       GTK_SIGNAL_FUNC ( clavier_calendrier ),
		       entry );
  gtk_box_pack_start ( GTK_BOX ( pVBox ),
		       pCalendar,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( pCalendar );

  /* ajoute le bouton annuler */
  bouton = gtk_button_new_with_label ( _("Cancel") );
  gtk_signal_connect_object ( GTK_OBJECT ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( gtk_widget_destroy ),
			      GTK_OBJECT ( popup ));

  gtk_box_pack_start ( GTK_BOX ( pVBox ),
		       bouton,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( bouton );

  /* cherche la position o� l'on va mettre la popup */
  /* on r�cup�re la position de l'entr�e date par rapport � laquelle on va placer la popup */

  gdk_window_get_origin ( GTK_WIDGET ( entry ) -> window,
			  &x,
			  &y );

  /* on r�cup�re la taille de la popup */

  taille_popup = malloc ( sizeof ( GtkRequisition ));
  gtk_widget_size_request ( GTK_WIDGET ( popup ), taille_popup );

  /* pour la soustraire � la position de l'entr�e date */

  y -= taille_popup -> height;

  /* si une des coordonn�es est n�gative, alors la fonction
  gtk_widget_set_uposition �choue et affiche la popup en 0,0 */

  if ( x < 0 )
  	x = 0 ;

  if ( y < 0 )
  	y = 0 ;

  /* on place la popup */

  gtk_widget_set_uposition ( GTK_WIDGET ( popup ),
			     x,
			     y );

  /* et on la montre */

  gtk_widget_show ( popup );
  gtk_widget_grab_focus ( GTK_WIDGET ( pCalendar ) );
  return ( popup );
}
/******************************************************************************/

/******************************************************************************/
/* Fonction date_selectionnee */
/* appel�e lorsqu'on a click� 2 fois sur une date du calendrier */
/******************************************************************************/
void date_selection ( GtkCalendar *pCalendar,
		      GtkWidget *entry )
{
  guint annee, mois, jour;
  GtkWidget *pTopLevelWidget;

  pTopLevelWidget = gtk_widget_get_toplevel ( GTK_WIDGET ( pCalendar ) );
  gtk_calendar_get_date ( pCalendar, &annee, &mois, &jour);

  gtk_entry_set_text ( GTK_ENTRY ( entry ),
		       g_strdup_printf ( "%02d/%02d/%04d",
					 jour,
					 mois + 1,
					 annee ));
  if ( GTK_WIDGET_TOPLEVEL ( pTopLevelWidget ) )
     gtk_widget_destroy ( pTopLevelWidget );
}
/******************************************************************************/

/******************************************************************************/
/* Fonction clavier_calendrier                                                */
/* cette fonction est appel�e � chaque appui sur une touche du clavier        */
/* lorsque qu'une fen�tre popup calendrier est ouverte.                       */
/******************************************************************************/
gboolean clavier_calendrier ( GtkCalendar *pCalendar,
			      GdkEventKey *ev,
			      GtkWidget *entry )
{
  gint jour, mois, annee;
  gint offset = 7;
  GtkWidget *pTopLevelWidget;

  /* on r�cup�re la popup du calendrier pour pouvoir la d�truire en temps voulu */
  pTopLevelWidget = gtk_widget_get_toplevel ( GTK_WIDGET ( pCalendar ) );

  gtk_calendar_get_date ( pCalendar, &annee, &mois, &jour );

  switch ( ev -> keyval )
    {
     case GDK_Escape :

       /* on quitte le calendrier et on le ferme sans rien faire */

       gtk_signal_emit_stop_by_name ( GTK_OBJECT ( pCalendar ),
				      "key-press-event");
       if ( GTK_WIDGET_TOPLEVEL ( pTopLevelWidget ) )
	  gtk_widget_destroy ( pTopLevelWidget );
       break ;

     case GDK_Return :		/* touches entr�e */
     case GDK_KP_Enter :

       /* on valide la date choisie */

       gtk_signal_emit_stop_by_name ( GTK_OBJECT ( pCalendar ),
				      "key-press-event");
       gtk_entry_set_text ( GTK_ENTRY ( entry ),
			    g_strdup_printf ( "%02d/%02d/%04d",
			    jour,
			    mois + 1,
			    annee ));
       if ( GTK_WIDGET_TOPLEVEL ( pTopLevelWidget ) )
	  gtk_widget_destroy ( pTopLevelWidget );
       break ;

     case GDK_Left :		/* touche fl�che gauche */
     case GDK_minus:		/* touches - */
     case GDK_KP_Subtract:

       /* on passe au jour pr�c�dent */

       jour--;
       /* si la date n'est pas valide, c'est parce qu'on a chang� de mois */
       if ( g_date_valid_day ( jour ) != TRUE )
         {
	  /* donc, dernier jour ...*/
	  jour = 31 ;
	  /* ... du mois pr�c�dent */
	  mois-- ;
	  /* parce qu'il y a une diff�rence de type et un d�calage de valeur
	     pour identifier le mois :
	       - pour g_date, janvier = 1
	       - pour gtk_calendar, janvier = 0 */
	  /* si le mois n'est pas valide, c'est parce qu'on a chang� d'ann�e */
	  if ( g_date_valid_month ( mois + 1 ) != TRUE )
	    {
	     /* donc mois de d�cembre... */
	     mois = 11;
	     /* de l'ann�e pr�c�dente */
	     annee--;
	     /* si l'ann�e n'est pas valide, �a craint !!!!, alors on choisit
	        le d�but de notre �re chr�tienne :-) */
	     if ( g_date_valid_year ( annee ) != TRUE )
	       annee = 1;
	    }
	  /* on cherche le dernier jour du mois */
	  while ( g_date_valid_dmy ( jour, mois + 1, annee ) != TRUE )
	    jour--;
	 }
       /* on positionne le calendrier au milieu du mois pour �viter des effets
          de bord de la fonction gtk_calendar_select_month */
       gtk_calendar_select_day( pCalendar , 15 );
       gtk_calendar_select_month ( pCalendar , mois, annee );
       gtk_calendar_select_day( pCalendar , jour );
       break ;

     case GDK_Right :		/* touche fl�che droite */
     case GDK_plus:		/* touches + */
     case GDK_KP_Add:

       /* on passe au jour suivant */

       jour++;
       /* si la date n'est pas valide, c'est parce qu'on a chang� de mois */
       if ( g_date_valid_dmy ( jour, mois + 1, annee ) != TRUE )
         {
	  /* donc, premier jour ...*/
	  jour = 1 ;
	  /* ... du mois suivant */
	  mois++ ;
	  /* si le mois n'est pas valide, c'est parce qu'on a chang� d'ann�e */
	  if ( g_date_valid_month ( mois + 1 ) != TRUE )
	    {
	     /* donc mois de janvier... */
	     mois = 0;
	     /* de l'ann�e suivante */
	     annee++;
	     /* si l'ann�e n'est pas valide, c'est qu'on a fait le tour du temps :-))),
	        alors on choisit le d�but de notre �re chr�tienne :-) */
	     if ( g_date_valid_year ( annee ) != TRUE )
	       annee = 1;
	    }
	 }
       gtk_calendar_select_day( pCalendar , 15 );
       gtk_calendar_select_month ( pCalendar , mois, annee );
       gtk_calendar_select_day( pCalendar, jour );
       break ;

     case GDK_Up :		/* touche fl�che haut */

       /* on passe � la semaine pr�c�dente */

       jour -= 7;
       /* si la date n'est pas valide, c'est parce qu'on a chang� de mois */
       if ( g_date_valid_day ( jour ) != TRUE )
         {
	  /* on revient donc en arri�re */
	  jour += 7;
	  offset = 7;
	  /* pour calculer le nombre de jours qu'il faut soustraire au mois
	     pr�c�dent */
	  while ( g_date_valid_day ( jour - 7 + offset  ) == TRUE )
	    offset--;
	  mois-- ;
	  /* si le mois n'est pas valide, c'est parce qu'on a chang� d'ann�e */
	  if ( g_date_valid_month ( mois + 1 ) != TRUE )
	    {
	     /* donc mois de d�cembre... */
	     mois = 11;
	     /* de l'ann�e pr�c�dente */
	     annee--;
	     if ( g_date_valid_year ( annee ) != TRUE )
	       annee = 1;
	    }
	  /* on cherche quel est le dernier du mois */
	  jour = 31;
	  while ( g_date_valid_dmy ( jour, mois + 1, annee ) != TRUE )
	    jour--;
	  /* pour lui soustraire le nombre de jours pr�c�demment calcul� */
	  jour -= offset;
	 }
       gtk_calendar_select_day( pCalendar , 15 );
       gtk_calendar_select_month ( pCalendar , mois, annee );
       gtk_calendar_select_day( pCalendar, jour );
       break ;

     case GDK_Down :		/* touche fl�che bas */

       /* on passe � la semaine suivante */

       jour += 7 ;
       if ( g_date_valid_dmy ( jour, mois + 1, annee ) != TRUE )
         {
	  jour -= 7;
	  offset = 0;
	  while ( g_date_valid_dmy ( jour + offset, mois + 1, annee ) == TRUE )
	    offset++;
	  offset--;
	  mois++ ;
	  /* parce qu'il y a une diff�rence de type et un d�calage de valeur
	     pour identifier le mois :
	       - pour g_date, janvier = 1
	       - pour gtk_calendar, janvier = 0 */
	  if ( g_date_valid_month ( mois + 1 ) != TRUE )
	    {
	     mois = 0;
	     annee++;
	     if ( g_date_valid_year ( annee ) != TRUE )
	       annee = 1;
	    }
	  jour = 7 - offset;
	 }
       gtk_calendar_select_day( pCalendar , 15 );
       gtk_calendar_select_month ( pCalendar , mois, annee );
       gtk_calendar_select_day( pCalendar, jour );
       break ;

     case GDK_Home :		/* touche Home */

       /* on passe au 1er jour du mois */
       
       gtk_calendar_select_day( pCalendar, 1 );
       break ;

     case GDK_End :		/* touche End */

       /* on passe au dernier jour du mois */

       jour = 31;
       while ( g_date_valid_dmy ( jour, mois + 1, annee ) != TRUE )
	 jour--;
       gtk_calendar_select_day( pCalendar, jour );
       break ;

     case GDK_Page_Up :		/* touche PgUp */
     case GDK_KP_Page_Up :

       /* on passe au mois pr�c�dent */

       gtk_calendar_select_day( pCalendar , 15 );
       mois-- ;
       if ( g_date_valid_month ( mois + 1 ) != TRUE )
	 {
	  mois = 11;
	  annee--;
	  if ( g_date_valid_year ( annee ) != TRUE )
	    annee = 1;
	 }
       while ( g_date_valid_dmy ( jour, mois + 1, annee ) != TRUE )
	 jour--;
       gtk_calendar_select_day( pCalendar , 15 );
       gtk_calendar_select_month ( pCalendar , mois, annee );
       gtk_calendar_select_day( pCalendar, jour );
       break ;

     case GDK_Page_Down :		/* touche PgDn */
     case GDK_KP_Page_Down :

       /* on passe au mois suivant */

       gtk_calendar_select_day( pCalendar , 15 );
       mois++ ;
       if ( g_date_valid_month ( mois + 1 ) != TRUE )
	 {
	  mois = 0;
	  annee++;
	  if ( g_date_valid_year ( annee ) != TRUE )
	    annee = 1;
	 }
       while ( g_date_valid_dmy ( jour, mois + 1, annee ) != TRUE )
	 jour--;
       gtk_calendar_select_day( pCalendar , 15 );
       gtk_calendar_select_month ( pCalendar , mois, annee );
       gtk_calendar_select_day( pCalendar, jour );
       break ;

     default :

       break ;
    }
  return TRUE;
}
/******************************************************************************/

/******************************************************************************/
/* fonction qui retourne la date du jour sous forme de string                 */
/******************************************************************************/
gchar *gsb_today ( void )
{
  time_t temps;
  gchar date[SIZEOF_FORMATTED_STRING_DATE];

  time ( &temps );
  strftime ( date,
	     SIZEOF_FORMATTED_STRING_DATE,
	     "%d/%m/%Y",
	     localtime ( &temps ) );
  return ( g_strdup ( date ) );
}
/******************************************************************************/

/******************************************************************************/
/* fonction qui retourne la date du jour au format GDate                      */
/******************************************************************************/
GDate *gdate_today ( void )
{
  time_t today_time;
  GDate *date;

  time ( &today_time );
  date = g_date_new_dmy ( gmtime ( &today_time ) -> tm_mday,
			  gmtime ( &today_time ) -> tm_mon + 1,
			  gmtime ( &today_time ) -> tm_year + 1900 );
  return ( date );
}
/******************************************************************************/

/******************************************************************************/
/* Fonction format_date                                                       */
/* Prend en argument une entr�e contenant une date                            */
/* V�rifie la validit� et la modifie si seulement une partie est donn�e       */
/* Met la date du jour si l'entr�e est vide                                   */
/* Renvoie TRUE si la date est correcte                                       */
/******************************************************************************/
gboolean format_date ( GtkWidget *entree )
{
  gchar *pEntry;
  int jour, mois, annee;
  GDate *date;
  gchar **tab_date;

  pEntry = g_strstrip ( ( gchar * ) gtk_entry_get_text ( GTK_ENTRY ( entree ) ) );

  date = g_date_new();
  g_date_set_time ( date, time( NULL ) );

  tab_date = g_strsplit ( pEntry, "/", 3 );

  if ( tab_date[2] && tab_date[1] )
    {
     /* on a rentr� les 3 chiffres de la date */
     jour = gsb_strtod ( tab_date[0],  NULL );
     mois = gsb_strtod ( tab_date[1], NULL );
     annee = gsb_strtod ( tab_date[2], NULL );

     if ( annee < 100 )
       {
        if ( annee < 80 )
          annee = annee + 2000;
        else
          annee = annee + 1900;
       }
    }
  else
    {
     if ( tab_date[1] )
       {
        /* on a rentr� la date sous la forme xx/xx,
           il suffit de mettre l'ann�e courante */
        jour = gsb_strtod ( tab_date[0], NULL );
        mois = gsb_strtod ( tab_date[1], NULL );
        annee = g_date_year ( date );
       }
     else
       {
        /* on a rentr� que le jour de la date,
           il faut mettre le mois et l'ann�e courante
           ou bien on a rentr� la date sous forme
           jjmm ou jjmmaa ou jjmmaaaa */
        gchar buffer[3];

        switch ( strlen ( tab_date[0] ) )
          {
           /* forme jj ou j */
           case 1:
           case 2:
	     jour = gsb_strtod ( tab_date[0], NULL );
	     mois = g_date_month ( date );
	     annee = g_date_year ( date );
	     break;

	   /* forme jjmm */

	   case 4 :
	     buffer[0] = tab_date[0][0];
	     buffer[1] = tab_date[0][1];
	     buffer[2] = 0;

	     jour = gsb_strtod ( buffer, NULL );
	     mois = gsb_strtod ( tab_date[0] + 2, NULL );
	     annee = g_date_year ( date );
	     break;

	   /* forme jjmmaa */

	   case 6:
	     buffer[0] = tab_date[0][0];
	     buffer[1] = tab_date[0][1];
	     buffer[2] = 0;

	     jour = gsb_strtod ( buffer, NULL );
	     buffer[0] = tab_date[0][2];
	     buffer[1] = tab_date[0][3];

	     mois = gsb_strtod ( buffer, NULL );
	     annee = gsb_strtod ( tab_date[0] + 4, NULL ) + 2000;
	     break;

	   /* forme jjmmaaaa */

	   case 8:
	     buffer[0] = tab_date[0][0];
	     buffer[1] = tab_date[0][1];
	     buffer[2] = 0;

	     jour = gsb_strtod ( buffer, NULL );
	     buffer[0] = tab_date[0][2];
	     buffer[1] = tab_date[0][3];

	     mois = gsb_strtod ( buffer, NULL );
	     annee = gsb_strtod ( tab_date[0] + 4, NULL );
	     break;

	   default :
	     jour = 0;
	     mois = 0;
	     annee = 0;
	  }
       }
    }
  g_strfreev ( tab_date );

  if ( g_date_valid_dmy ( jour, mois, annee) )
    gtk_entry_set_text ( GTK_ENTRY ( entree ),
			 g_strdup_printf ( "%02d/%02d/%04d", jour, mois, annee ));
  else
    return ( FALSE );

  return ( TRUE );
}
/******************************************************************************/

/******************************************************************************/
/* appel�e lors de l'appui des touche + ou - sur les formulaires              */
/* augmente ou diminue la date entr�e de 1 jour, 1 semaine, 1 mois, 1 an,     */
/* suivant la valeur du param�tre � demande �                                 */
/******************************************************************************/
void inc_dec_date ( GtkWidget *entree, gint demande )
{
  gchar **tableau_char;
  GDate *date;
  gint jour, mois, annee;

  /* on commence par v�rifier que la date est valide */

  if ( !format_date ( entree ) )
    return;

  tableau_char = g_strsplit ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entree ))),
			      "/",
			      3 );

  jour = gsb_strtod ( tableau_char[0], NULL );
  mois = gsb_strtod ( tableau_char[1], NULL );
  annee = gsb_strtod ( tableau_char[2], NULL );

  date = g_date_new_dmy ( jour, mois, annee);

  switch ( demande )
    {
     case ONE_DAY :
     case ONE_WEEK :

       g_date_add_days ( date, demande ) ;
       break ;

     case -ONE_DAY :
     case -ONE_WEEK :

       g_date_subtract_days ( date, -demande ) ;
       break ;

     case ONE_MONTH :

       g_date_add_months ( date, 1 ) ;
       break ;

     case -ONE_MONTH :

       g_date_subtract_months ( date, 1 ) ;
       break ;

     case ONE_YEAR :

       g_date_add_years ( date, 1 ) ;
       break ;

     case -ONE_YEAR :

       g_date_subtract_years ( date, 1 ) ;
       break ;

     default :
       break ;
    }

  gtk_entry_set_text ( GTK_ENTRY ( entree ),
  g_strdup_printf ( "%02d/%02d/%04d",
		    g_date_day ( date ),
		    g_date_month ( date ),
		    g_date_year ( date )));
}
/******************************************************************************/

/******************************************************************************/
void ferme_calendrier ( GtkWidget *entree )
{
  gtk_grab_remove ( entree );
}
/******************************************************************************/

/******************************************************************************/
/* Fonction gsb_strtod (string to double)                                     */
/* Convertie une chaine de caract�res en un nombre                            */
/* Param�tres d'entr�e :                                                      */
/*   - nptr : pointeur sur la chaine de caract�res � convertir                */
/*   - endptr : n'est pas utilis�, alors � quoi peut-il bien servir ?         */
/* Valeur de retour :                                                         */
/*   - resultat : le r�sultat de la conversion                                */
/* Variables locales :                                                        */
/*   - entier : la partie enti�re du r�sultat                                 */
/*   - mantisse : la partie d�cimale du r�sultat                              */
/*   - invert : le signe du r�sultat (0 -> positif, 1 -> n�gatif)             */
/*   - p, m : pointeurs locaux sur la chaine de caract�res � convertir        */
/******************************************************************************/
double gsb_strtod ( char *nptr, char **endptr )
{
  double entier=0, mantisse=0, resultat=0;
  int invert = 0;
  char *p;

  if (!nptr)
    return 0;

  /* Pour chacun des caract�res de la chaine, du d�but � la fin de la chaine,
     faire : */
  for ( p = nptr; p < nptr + strlen(nptr); p++ )
    {
      /* si c'est un espace ou le signe +, on passe au caract�re suivant */
      if ( isspace(*p) || *p == '+' )
	continue;

      /* si c'est le signe -, on positionne invert � 1 et on passe
         au caract�re suivant */
      if ( *p == '-' )
	{
	  invert = 1;
	  continue;
	}

      /* si c'est un point ou une virgule, alors : */
      if ( *p == ',' || *p == '.' )
	{
	  char *m;
	  /* aller � la fin de la chaine */
	  for ( m = p+1; m <= nptr+strlen(nptr) &&
		  (isdigit(*m) || isspace(*m)); m++)
	  /* de la fin de la chaine au dernier caract�re avant le point
	     ou la virgule, faire : */
	  for ( --m; m > p; m-- )
	    {
	      /* si c'est un chiffre, alors : */
	      if (isdigit(*m))
		{
		  /* "d�calage" � droite de la variable mantisse */
		  mantisse /= 10;
		  /* ajout � la variable mantisse de la valeur d�cimale
		     du caract�re point� par m */
		  mantisse += (*m - '0');
		}
	    }
	  /* "d�calage" � droite de la variable mantisse */
	  mantisse /= 10;
	}

      /* si c'est un chiffre, alors : */
      if ( isdigit(*p) )
	{
	  /* "d�calage" � gauche de la variable entier */
	  entier = entier * 10;
	  /* ajout � la variable entier de la valeur d�cimale
	     du caract�re point� par p */
	  entier += (*p - '0');
	}
      else
	{
	  break;
	}
    }

  resultat = entier + mantisse;
  if ( invert )
    resultat = - resultat;

  return resultat;
}
/******************************************************************************/
