/* *******************************************************************************/
/*                                 GRISBI                                        */
/* Programme de gestion financi�re personnelle                                   */
/*           	  license : GPL                                                  */
/*                                                                               */
/* 	          Version : 0.3.2                                                */
/*      Auteur : C�dric Auger   ( cedric@grisbi.org )                            */
/*                                http://www.grisbi.org                          */
/* *******************************************************************************/

/* *******************************************************************************/
/*     Copyright (C) 2000-2002  C�dric Auger                                     */
/*                                                                               */
/*     This program is free software; you can redistribute it and/or modify      */
/*     it under the terms of the GNU General Public License as published by      */
/*     the Free Software Foundation; either version 2 of the License, or         */
/*     (at your option) any later version.                                       */
/*                                                                               */
/*     This program is distributed in the hope that it will be useful,           */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*     GNU General Public License for more details.                              */
/*                                                                               */
/*     You should have received a copy of the GNU General Public License         */
/*     along with this program; if not, write to the Free Software               */
/*     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/* *******************************************************************************/



/* Fichier de base contenant la proc�dure main */


#include "include.h"
#include "structures.h"
#include "variables.c"
#include "en_tete.h"




/***********************************************************************************************
** D�but de la proc�dure principale **
***********************************************************************************************/

int main (int argc, char *argv[])
{
  gnome_init ("Grisbi", VERSION, argc, argv);

  /*  Cr�ation de la fen�tre principale */

  window = gnome_app_new ("Grisbi", "Grisbi");


  gtk_signal_connect ( GTK_OBJECT (window),
		       "delete_event",
		       GTK_SIGNAL_FUNC ( fermeture_grisbi ),
		       NULL );

  gtk_signal_connect (GTK_OBJECT (window),
		      "destroy",
		      GTK_SIGNAL_FUNC ( fermeture_grisbi ),
		      NULL );

  gtk_signal_connect ( GTK_OBJECT ( window ),
		       "size-allocate",
		       GTK_SIGNAL_FUNC ( tente_modif_taille ),
		       NULL );
  gtk_window_set_policy ( GTK_WINDOW ( window ),
			  TRUE,
			  TRUE,
			  FALSE );

/*   cr�ation des menus */

  gnome_app_create_menus ( GNOME_APP ( window ), 
			   menu_principal );

  /* on grise les fonctions inutiles au d�part */

  init_variables ( FALSE );

  charge_configuration();

  /* on met dans le menu les derniers fichiers ouverts */

  affiche_derniers_fichiers_ouverts ();

/*   si la taille avait d�j� �t� sauv�e, on remet l'ancienne taille � la fenetre */

  if ( largeur_window + hauteur_window )
    gtk_window_set_default_size ( GTK_WINDOW ( window ),
				  largeur_window,
				  hauteur_window );


  gtk_widget_show ( window );

  /* on met la fonte choisie */

  if ( fonte_general )
    {
      gtk_widget_get_default_style () -> font = gdk_font_load ( fonte_general );
      gtk_widget_get_style (window) -> font = gdk_font_load ( fonte_general );
    }

    if ( argc > 1 )
     {
       nom_fichier_comptes = argv[1];
       ouverture_confirmee();
     }
    else
      if ( etat.dernier_fichier_auto
	   &&
	   strlen ( nom_fichier_comptes ) )
	ouverture_confirmee();


  gtk_main ();

  exit(0);
}
/************************************************************************************************/



/************************************************************************************************/
gint tente_modif_taille ( GtkWidget *win,
			  GtkAllocation *requisition,
			  gpointer null )
{

  if ( requisition -> width < 800 )
    gtk_signal_emit_stop_by_name ( GTK_OBJECT ( window ),
				   "size-allocate" );
/*     gtk_widget_set_usize ( window, */
/* 			   850, */
/* 			   requisition -> height ); */

  return ( TRUE );

}
/************************************************************************************************/
