/* fichier qui s'occupe de la gestion des comptes */
/*           gestion_comptes.c */

/*     Copyright (C) 2000-2003  C�dric Auger */
/* 			cedric@grisbi.org */
/* 			http://www.grisbi.org */

/*     This program is free software; you can redistribute it and/or modify */
/*     it under the terms of the GNU General Public License as published by */
/*     the Free Software Foundation; either version 2 of the License, or */
/*     (at your option) any later version. */

/*     This program is distributed in the hope that it will be useful, */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/*     GNU General Public License for more details. */

/*     You should have received a copy of the GNU General Public License */
/*     along with this program; if not, write to the Free Software */
/*     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */


#include "include.h"
#include "structures.h"
#include "variables-extern.c"
#include "comptes_gestion.h"
#include "comptes_onglet.h"
#include "comptes_traitements.h"
#include "gtk_list_button.h"


/*****************************************************************************************************/
GtkWidget *creation_onglet_comptes ( void )
{
    GtkWidget *onglet, *frame;

    onglet = gtk_hpaned_new ( );
    gtk_paned_set_position ( GTK_PANED(onglet), 200 );
    gtk_container_set_border_width ( GTK_CONTAINER ( onglet ), 10 );
    gtk_widget_show ( onglet );


    /*   cr�ation de la fenetre des comptes / ventilation / �quilibrage � gauche */

    frame = gtk_frame_new ( NULL );
    gtk_frame_set_shadow_type ( GTK_FRAME ( frame ), GTK_SHADOW_IN );
    gtk_paned_pack1 ( GTK_PANED(onglet), frame, TRUE, TRUE );
    gtk_widget_show (frame);

    gtk_container_add ( GTK_CONTAINER ( frame ), creation_liste_comptes_onglet ());


    /* cr�ation de la partie droite */

    frame = gtk_frame_new ( NULL );
    gtk_frame_set_shadow_type ( GTK_FRAME ( frame ), GTK_SHADOW_IN );
    gtk_paned_pack2 ( GTK_PANED(onglet), frame, TRUE, TRUE );
    gtk_widget_show (frame);

    gtk_container_add ( GTK_CONTAINER ( frame ), creation_details_compte ());

    /*  Cr�ation d'une icone et du nom par compte, et placement dans la
	liste selon l'ordre d�sir� et met le livre ouvert pour le compte
	0 */

    compte_courant_onglet = 0;
    reaffiche_liste_comptes_onglet ();
    remplissage_details_compte ();

    return ( onglet );
}
/*****************************************************************************************************/


/*****************************************************************************************************/
GtkWidget *creation_liste_comptes_onglet ( void )
{
    GtkWidget *onglet;
    GtkWidget *frame;
    GtkWidget *bouton;
    GtkWidget *vbox_frame;
    GtkWidget *scrolled_window;

    /*  Cr�ation d'une fen�tre g�n�rale*/

    onglet = gtk_vbox_new ( FALSE, 10);
    gtk_container_set_border_width ( GTK_CONTAINER ( onglet ), 10 );
    gtk_widget_show ( onglet );

    /*  Cr�ation du label Comptes en haut */
    /*   on place le label dans une frame */

    frame = gtk_frame_new ( NULL );
    gtk_frame_set_shadow_type ( GTK_FRAME ( frame ),
				GTK_SHADOW_ETCHED_OUT );
    gtk_box_pack_start ( GTK_BOX (onglet),
			 frame,
			 FALSE,
			 TRUE,
			 0);
    gtk_widget_show (frame);


    /*   on ne met rien dans le label, il sera rempli ensuite */

    label_compte_courant_onglet = gtk_label_new ( "" );
    gtk_misc_set_alignment ( GTK_MISC (label_compte_courant_onglet  ), 0.5, 0.5);
    gtk_container_add ( GTK_CONTAINER ( frame ),
			label_compte_courant_onglet );
    gtk_widget_show (label_compte_courant_onglet);


    /*  Cr�ation de la fen�tre des comptes */
    /*  qui peut contenir des barres de d�filement si */
    /*  n�cessaire */

    scrolled_window = gtk_scrolled_window_new ( NULL, NULL);
    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_window ),
				     GTK_POLICY_NEVER,
				     GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start ( GTK_BOX ( onglet ), scrolled_window, TRUE, TRUE, 0);
    gtk_widget_show ( scrolled_window );


    /*  cr�ation d'une vbox contenant la liste des comptes */

    vbox_liste_comptes_onglet = gtk_vbox_new ( FALSE, 10);
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW (scrolled_window ),
					    vbox_liste_comptes_onglet );
    gtk_viewport_set_shadow_type ( GTK_VIEWPORT ( GTK_BIN ( scrolled_window )  -> child ),
				   GTK_SHADOW_NONE );
    gtk_widget_show ( vbox_liste_comptes_onglet );


    /* ajoute les boutons nouveau et supprimer */
    /* les 2 seront int�gr�s dans une frame */

    frame = gtk_frame_new ( NULL );
    gtk_frame_set_shadow_type ( GTK_FRAME ( frame ), GTK_SHADOW_ETCHED_IN );
    gtk_box_pack_start ( GTK_BOX ( onglet ), frame, FALSE, TRUE, 0);
    gtk_widget_show ( frame );


    vbox_frame = gtk_vbox_new ( FALSE, 5 );
    gtk_container_add ( GTK_CONTAINER  ( frame ), vbox_frame );
    gtk_widget_show ( vbox_frame );

    /* mise en place des boutons du bas */

    bouton = gtk_button_new_with_label ( _("New account") );
    gtk_button_set_relief ( GTK_BUTTON ( bouton ), GTK_RELIEF_NONE);
    gtk_box_pack_start ( GTK_BOX ( vbox_frame ), bouton, FALSE, TRUE, 0);
    gtk_signal_connect ( GTK_OBJECT (bouton), "clicked",
			 GTK_SIGNAL_FUNC ( nouveau_compte ), NULL );
    gtk_widget_show ( bouton );


    /* mise en place du bouton "supprimer un compte" */

    bouton_supprimer_compte = gtk_button_new_with_label ( _("Remove an account") );
    gtk_button_set_relief ( GTK_BUTTON ( bouton_supprimer_compte ), GTK_RELIEF_NONE);
    gtk_box_pack_start ( GTK_BOX ( vbox_frame ), bouton_supprimer_compte,
			 FALSE, TRUE, 0);
    gtk_signal_connect ( GTK_OBJECT (bouton_supprimer_compte), "clicked",
			 GTK_SIGNAL_FUNC ( supprimer_compte ), NULL );
    gtk_widget_show ( bouton_supprimer_compte );

    if ( !nb_comptes )
	gtk_widget_set_sensitive ( bouton_supprimer_compte, FALSE );

    return ( onglet );
}
/*****************************************************************************************************/



/* ********************************************************************************************************** */
/** Fonction qui renvoie un widget contenant un bouton **/
/** de compte associ� � son nom **/
/* ********************************************************************************************************** */

GtkWidget *comptes_appel_onglet ( gint no_de_compte )
{
    GtkWidget *bouton;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_de_compte;

    bouton = gtk_list_button_new ( NOM_DU_COMPTE, 1 );
    gtk_signal_connect ( GTK_OBJECT (bouton), "clicked",
			 GTK_SIGNAL_FUNC ( changement_compte_onglet ),
			 GINT_TO_POINTER ( no_de_compte ) );

    return ( bouton );
}
/* ********************************************************************************************************** */



/* ********************************************************************************************************** */
void changement_compte_onglet ( GtkWidget *bouton,
				gint compte )
{
    /* demande si n�cessaire si on enregistre */
    sort_du_detail_compte ();

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte;

    /* change le nom du compte courant */
    gtk_label_set_text ( GTK_LABEL ( label_compte_courant_onglet),
			 NOM_DU_COMPTE);

    compte_courant_onglet = compte;

    remplissage_details_compte ();
}
/* ********************************************************************************************************** */



/* *********************************************************************************************************** */
/*   on r�affiche la liste des comptes s'il y a eu un changement */
/* *********************************************************************************************************** */

void reaffiche_liste_comptes_onglet ( void )
{
    GSList *ordre_comptes_variable;
    GtkWidget *bouton;

    /* commence par effacer tous les comptes */

    while ( GTK_BOX ( vbox_liste_comptes_onglet ) -> children )
	gtk_container_remove ( GTK_CONTAINER ( vbox_liste_comptes_onglet ),
			       (( GtkBoxChild *) ( GTK_BOX ( vbox_liste_comptes_onglet ) -> children -> data )) -> widget );


    /*  Cr�ation d'une icone et du nom par compte, et placement dans la liste selon l'ordre d�sir�  */

    if ( nb_comptes )
    {
	ordre_comptes_variable = ordre_comptes;

	do
	{
	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + GPOINTER_TO_INT ( ordre_comptes_variable->data );

	    bouton = comptes_appel_onglet ( GPOINTER_TO_INT ( ordre_comptes_variable->data ));
	    gtk_box_pack_start (GTK_BOX (vbox_liste_comptes_onglet), bouton, FALSE, FALSE, 0);
	    gtk_widget_show_all (bouton);
	}
	while ( (  ordre_comptes_variable = ordre_comptes_variable->next ) );
    }

    remplissage_details_compte ();

}
/* *********************************************************************************************************** */
