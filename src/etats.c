/*  Fichier qui s'occupe des �tats */
/*      etats.c */

/*     Copyright (C) 2000-2002  C�dric Auger */
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
#include "en_tete.h"

gchar *liste_plages_dates[] = {
  N_("Toutes"),
  N_("Personnalis�"),
  N_("Cumul � ce jour"),
  N_("Mois en cours"),
  N_("Ann�e en cours"),
  N_("Cumul mensuel"),
  N_("Cumul annuel"),
  N_("Mois pr�c�dent"),
  N_("Ann�e pr�c�dente"),
  N_("30 derniers jours"),
  N_("3 derniers mois"),
  N_("6 derniers mois"),
  N_("12 derniers mois"),
  N_("30 prochains jours"),
  N_("3 prochains mois"),
  N_("6 prochains mois"),
  N_("12 prochains mois"),
  NULL };

gchar *jours_semaine[] = {
  N_("Lundi"),
  N_("Mardi"),
  N_("Mercredi"),
  N_("Jeudi"),
  N_("Vendredi"),
  N_("Samedi"),
  N_("Dimanche"),
  NULL };

#include "etats_gtktable.h"
#include "etats_gnomeprint.h"

/*****************************************************************************************************/
GtkWidget *creation_onglet_etats ( void )
{
  GtkWidget *onglet;
  GtkWidget *frame;
  GtkWidget *vbox;


  onglet = gtk_hbox_new ( FALSE,
			  10 );
  gtk_container_set_border_width ( GTK_CONTAINER ( onglet ),
				   10 );
  gtk_widget_show ( onglet );


  /*   cr�ation de la fenetre des noms des �tats */
  /* on reprend le principe des comptes dans la fenetre des op�s */

  frame = gtk_frame_new ( NULL );
  gtk_frame_set_shadow_type ( GTK_FRAME ( frame ),
			      GTK_SHADOW_IN );
  gtk_box_pack_start ( GTK_BOX ( onglet ),
		       frame,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show (frame);

  /* on y met les rapports et les boutons */

  gtk_container_add ( GTK_CONTAINER ( frame ),
		      creation_liste_etats ());


  /* cr�ation de la partie droite */

  vbox = gtk_vbox_new ( FALSE,
			10 );
  gtk_box_pack_start ( GTK_BOX ( onglet ),
		       vbox,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox );


  /*  Cr�ation de la partie contenant l'�tat */

  frame = gtk_frame_new ( NULL );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       frame,
		       TRUE,
		       TRUE,
		       0 );
  gtk_frame_set_shadow_type ( GTK_FRAME ( frame ),
			      GTK_SHADOW_IN );
  gtk_widget_show (frame);

  /* on y met une scrolled window qui sera remplit par l'�tat */

  scrolled_window_etat = gtk_scrolled_window_new ( FALSE,
						   FALSE );
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_window_etat ),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC );
  gtk_container_add ( GTK_CONTAINER ( frame ),
		      scrolled_window_etat );
  gtk_widget_show ( scrolled_window_etat );

  /* cr�ation de la partie contenant les boutons (personnaliser ...) */

  frame = gtk_frame_new ( NULL );
  gtk_frame_set_shadow_type ( GTK_FRAME ( frame_droite_bas ),
			      GTK_SHADOW_IN );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       frame,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show (frame);

  /* on y met les boutons */

  gtk_container_add ( GTK_CONTAINER ( frame ),
		      creation_barre_boutons_etats ());


  /*   au d�part, aucun �tat n'est ouvert */

  bouton_etat_courant = NULL;
  etat_courant = NULL;

  return ( onglet );
}
/*****************************************************************************************************/




/*****************************************************************************************************/
/* vontion creation_liste_etats */
/* renvoie la partie gauche de l'onglet rapports financiers */
/*****************************************************************************************************/

GtkWidget *creation_liste_etats ( void )
{
  GtkWidget *onglet;
  GtkWidget *frame;
  GtkWidget *bouton;
  GtkWidget *vbox;
  GtkWidget *scrolled_window;


  /*  Cr�ation d'une fen�tre g�n�rale*/

  onglet = gtk_vbox_new ( FALSE,
			  10);
  gtk_container_set_border_width ( GTK_CONTAINER ( onglet ),
				   10 );
  gtk_widget_show ( onglet );


  /*  Cr�ation du label contenant le rapport courant en haut */
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

  label_etat_courant = gtk_label_new ( "" );
  gtk_misc_set_alignment ( GTK_MISC (label_etat_courant  ),
			   0.5,
			   0.5);
  gtk_container_add ( GTK_CONTAINER ( frame ),
		      label_etat_courant );
  gtk_widget_show (label_etat_courant);


  /*  Cr�ation de la fen�tre des etats */
  /*  qui peut contenir des barres de d�filement si */
  /*  n�cessaire */

  scrolled_window = gtk_scrolled_window_new ( NULL,
					      NULL);
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_window ),
				   GTK_POLICY_NEVER,
				   GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start ( GTK_BOX ( onglet ),
		       scrolled_window,
		       TRUE,
		       TRUE,
		       0);
  gtk_widget_show ( scrolled_window );
  

  /*  cr�ation de la vbox qui contient la liste des �tats */

  vbox_liste_etats = gtk_vbox_new ( FALSE,
				    10);
  gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW (scrolled_window ),
					  vbox_liste_etats);
  gtk_viewport_set_shadow_type ( GTK_VIEWPORT ( GTK_BIN ( scrolled_window )  -> child ),
				 GTK_SHADOW_NONE );
  gtk_widget_show (vbox_liste_etats);
  

  /*  ajout des diff�rents �tats */

  remplissage_liste_etats ();


  /* ajout des boutons pour supprimer / ajouter un �tat */

  frame = gtk_frame_new ( NULL );
  gtk_frame_set_shadow_type ( GTK_FRAME ( frame ),
			      GTK_SHADOW_ETCHED_IN );
  gtk_box_pack_start ( GTK_BOX ( onglet ),
		       frame,
		       FALSE,
		       TRUE,
		       0);
  gtk_widget_show ( frame );


  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_container_add ( GTK_CONTAINER  ( frame ),
		      vbox );
  gtk_widget_show ( vbox );


  /* mise en place du bouton ajouter */

  bouton = gtk_button_new_with_label ( _("Ajouter un �tat ...") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE);
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton,
		       FALSE,
		       TRUE,
		       0);
  gtk_signal_connect ( GTK_OBJECT (bouton),
		       "clicked",
		       GTK_SIGNAL_FUNC ( ajout_etat ),
		       NULL );
  gtk_widget_show ( bouton );


  /* mise en place du bouton �quilibrage */

  bouton_effacer_etat = gtk_button_new_with_label ( _("Effacer un �tat ...") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton_effacer_etat ),
			  GTK_RELIEF_NONE);
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_effacer_etat,
		       FALSE,
		       TRUE,
		       0);
  gtk_signal_connect ( GTK_OBJECT (bouton_effacer_etat),
		       "clicked",
		       GTK_SIGNAL_FUNC ( efface_etat ),
		       NULL );
  gtk_widget_show ( bouton_effacer_etat );

  if ( !liste_struct_etats )
    gtk_widget_set_sensitive ( bouton_effacer_etat,
			       FALSE );


  return ( onglet );

}
/*****************************************************************************************************/


/*****************************************************************************************************/
/* Fonction creation_barre_boutons_etats */
/* renvoie la barre des boutons de la partie droite-bas des rapports financiers */
/*****************************************************************************************************/

GtkWidget *creation_barre_boutons_etats ( void )
{
  GtkWidget *widget_retour;
  GtkWidget *bouton;

  widget_retour = gtk_hbox_new ( FALSE,
				 5 );
  gtk_widget_show ( widget_retour );

  /* on met le bouton personnaliser */

  bouton_personnaliser_etat = gtk_button_new_with_label ( _("Personnaliser ...") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton_personnaliser_etat ),
			  GTK_RELIEF_NONE );
  gtk_widget_set_sensitive ( bouton_personnaliser_etat,
			     FALSE );
  gtk_signal_connect ( GTK_OBJECT ( bouton_personnaliser_etat ),
		       "clicked",
		       GTK_SIGNAL_FUNC ( personnalisation_etat ),
		       NULL );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_personnaliser_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_personnaliser_etat );

  /* on met le bouton rafraichir */

  bouton = gtk_button_new_with_label ( _("Rafraichir") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( rafraichissement_etat ),
			      NULL );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );

  bouton_imprimer_etat = gtk_button_new_with_label ( _("Imprimer") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton_imprimer_etat ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT ( bouton_imprimer_etat ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( impression_etat ),
			      NULL );
  gtk_widget_set_sensitive ( bouton_imprimer_etat,
			     FALSE );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_imprimer_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_imprimer_etat );

  return ( widget_retour );
}
/*****************************************************************************************************/


/*****************************************************************************************************/
/* Fontion remplissage_liste_etats */
/* vide et remplit la liste des �tats */
/*****************************************************************************************************/

void remplissage_liste_etats ( void )
{
  GList *pointeur;
  GSList *liste_tmp;


  /* on commence par d�truire tous les enfants de la vbox */

  pointeur = GTK_BOX ( vbox_liste_etats ) -> children;

  while ( pointeur )
    {
      GtkBoxChild *child;

      child = pointeur -> data;

      pointeur = pointeur -> next;

      gtk_container_remove ( GTK_CONTAINER ( vbox_liste_etats ),
			     child -> widget );
    }

  /* on remplit maintenant avec tous les �tats */

  liste_tmp = liste_struct_etats;

  while ( liste_tmp )
    {
      struct struct_etat *etat;
      GtkWidget *hbox;
      GtkWidget *bouton;
      GtkWidget *icone;
      GtkWidget *label;

      etat = liste_tmp -> data;

      hbox = gtk_hbox_new ( FALSE,
			    10);
      gtk_box_pack_start ( GTK_BOX ( vbox_liste_etats ),
			   hbox,
			   FALSE,
			   FALSE,
			   0 );
      gtk_widget_show ( hbox );


      /*   on cr�e le bouton contenant le livre ferm� et ouvert, seul le ferm� est affich� pour l'instant */

      bouton = gtk_button_new ();
      gtk_button_set_relief ( GTK_BUTTON (bouton),
			      GTK_RELIEF_NONE);
      gtk_signal_connect ( GTK_OBJECT (bouton),
			   "clicked",
			   GTK_SIGNAL_FUNC ( changement_etat ),
			   etat );
      gtk_box_pack_start ( GTK_BOX (hbox),
			   bouton,
			   FALSE,
			   TRUE,
			   0);
      gtk_widget_show (bouton);

      /* cr�ation de l'icone ferm�e */

      if ( etat_courant
	   &&
	   etat -> no_etat == etat_courant -> no_etat )
	icone = gnome_stock_pixmap_widget ( GTK_WIDGET ( bouton ),
					    GNOME_STOCK_PIXMAP_BOOK_OPEN);
      else
	icone = gnome_stock_pixmap_widget ( GTK_WIDGET ( bouton ),
					    GNOME_STOCK_PIXMAP_BOOK_BLUE);
      gtk_container_add ( GTK_CONTAINER ( bouton ),
			  icone );
      gtk_widget_show ( icone );

      /* on cr�e le label � cot� du bouton */

      label = gtk_label_new ( etat -> nom_etat );
      gtk_box_pack_start ( GTK_BOX (hbox),
			   label,
			   FALSE,
			   TRUE,
			   0);
      gtk_widget_show (label);

      liste_tmp = liste_tmp -> next;
    }
}
/*****************************************************************************************************/



/*****************************************************************************************************/
void ajout_etat ( void )
{
  struct struct_etat *etat;

  /* on ajoute un �tat vierge appel� nouvel �tat */
  /*   pour modifier le nom, il faudra aller dans la */
  /* personnalisation */

  etat = calloc ( 1,
		  sizeof ( struct struct_etat ));

  etat -> no_etat = ++no_dernier_etat;
  etat -> nom_etat = g_strdup ( _("Nouvel �tat") );

  /*   le classement de base est 1-2-3-4-5-6 (cf structure.h) */

  etat -> type_classement = g_list_append ( etat -> type_classement,
					    GINT_TO_POINTER ( 1 ));
  etat -> type_classement = g_list_append ( etat -> type_classement,
					    GINT_TO_POINTER ( 2 ));
  etat -> type_classement = g_list_append ( etat -> type_classement,
					    GINT_TO_POINTER ( 3 ));
  etat -> type_classement = g_list_append ( etat -> type_classement,
					    GINT_TO_POINTER ( 4 ));
  etat -> type_classement = g_list_append ( etat -> type_classement,
					    GINT_TO_POINTER ( 5 ));
  etat -> type_classement = g_list_append ( etat -> type_classement,
					    GINT_TO_POINTER ( 6 ));

  /*   les devises sont � 1 (euro) */

  etat -> devise_de_calcul_general = 1;
  etat -> devise_de_calcul_categ = 1;
  etat -> devise_de_calcul_ib = 1;
  etat -> devise_de_calcul_tiers = 1;

  /*   tout le reste est � NULL, ce qui est tr�s bien */
  /* on l'ajoute � la liste */

  liste_struct_etats = g_slist_append ( liste_struct_etats,
					etat );

  /* on r�affiche la liste des �tats */

  remplissage_liste_etats ();

  gtk_widget_set_sensitive ( bouton_effacer_etat,
			     TRUE );

  modification_fichier ( TRUE );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
void efface_etat ( void )
{
  GtkWidget *dialog;
  GtkWidget *label;
  GtkWidget *option_menu;
  gint resultat;
  GSList *liste_tmp;
  GtkWidget *menu;
  struct struct_etat *etat;

  if ( !liste_struct_etats )
    return;

  dialog = gnome_dialog_new ( _("S�lection de l'�tat � effacer :"),
			      GNOME_STOCK_BUTTON_OK,
			      GNOME_STOCK_BUTTON_CANCEL,
			      NULL );
  gtk_window_set_transient_for ( GTK_WINDOW ( dialog ),
				 GTK_WINDOW ( window ));

  label = gtk_label_new ( _("Choisir l'�tat � effacer :") );
  gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );


  option_menu = gtk_option_menu_new ();
  menu = gtk_menu_new ();

  liste_tmp = liste_struct_etats;

  while ( liste_tmp )
    {
      GtkWidget *menu_item;

      etat = liste_tmp -> data;

      menu_item = gtk_menu_item_new_with_label ( etat -> nom_etat );
      gtk_object_set_data ( GTK_OBJECT ( menu_item ),
			    "adr_etat",
			    etat );
      gtk_menu_append ( GTK_MENU ( menu ),
			menu_item );
      gtk_widget_show ( menu_item );

      liste_tmp = liste_tmp -> next;
    }

  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( option_menu ),
			     menu );
  gtk_widget_show ( menu );

  gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
		       option_menu,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( option_menu );

  if ( etat_courant )
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( option_menu ),
				  g_slist_position ( liste_struct_etats,
						     g_slist_find_custom ( liste_struct_etats,
									   GINT_TO_POINTER ( etat_courant -> no_etat ),
									   (GCompareFunc) recherche_etat_par_no )));

  resultat = gnome_dialog_run ( GNOME_DIALOG ( dialog ));

  if ( !resultat )
    {
     
      etat = gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( option_menu ) -> menu_item ),
				   "adr_etat" );

      /*   si l'�tat courant �tait celui qu'on efface, on met l'�tat courant � -1 et */
      /* le bouton � null, et le label de l'�tat en cours � rien */

      if ( etat_courant -> no_etat == etat -> no_etat )
	{
	  etat_courant = NULL;
	  bouton_etat_courant = NULL;
	  gtk_label_set_text ( GTK_LABEL ( label_etat_courant ),
			       "" );
	  gtk_widget_set_sensitive ( bouton_personnaliser_etat,
				     FALSE );
	  gtk_widget_set_sensitive ( bouton_imprimer_etat,
				     FALSE );

	  /* il faut aussi ici virer l'�tat affich� quand ce sera fait */

	  if ( GTK_BIN ( scrolled_window_etat ) -> child )
	    gtk_container_remove ( GTK_CONTAINER ( scrolled_window_etat ),
				   GTK_BIN ( scrolled_window_etat ) -> child );
	}

      liste_struct_etats = g_slist_remove ( liste_struct_etats,
					    etat );


      /* on r�affiche la liste des �tats */

      remplissage_liste_etats ();

      /*   s'il ne reste plus d'�tat, on grise le bouton */

      if ( !liste_struct_etats )
	gtk_widget_set_sensitive ( bouton_effacer_etat,
				   FALSE );
      modification_fichier ( TRUE );

    }

  if ( GNOME_IS_DIALOG ( dialog ))
    gnome_dialog_close ( GNOME_DIALOG ( dialog ));

}
/*****************************************************************************************************/


/*****************************************************************************************************/
void changement_etat ( GtkWidget *bouton,
		       struct struct_etat *etat )
{
  GtkWidget *icone;

  /* on commence par refermer l'ancien bouton */

  if ( bouton_etat_courant )
    {
      icone = gnome_stock_pixmap_widget ( GTK_WIDGET ( bouton ),
					  GNOME_STOCK_PIXMAP_BOOK_BLUE);
      gtk_container_remove ( GTK_CONTAINER ( bouton_etat_courant ),
			     GTK_BIN ( bouton_etat_courant ) -> child );
      gtk_container_add ( GTK_CONTAINER ( bouton_etat_courant ),
			  icone );
      gtk_widget_show ( icone );
    }

  /* on ouvre le nouveau */

  bouton_etat_courant = bouton;
  etat_courant = etat;
  gtk_widget_set_sensitive ( bouton_personnaliser_etat,
			     TRUE );
  gtk_widget_set_sensitive ( bouton_imprimer_etat,
			     TRUE );

  icone = gnome_stock_pixmap_widget ( GTK_WIDGET ( bouton ),
				      GNOME_STOCK_PIXMAP_BOOK_OPEN);
  gtk_container_remove ( GTK_CONTAINER ( bouton_etat_courant ),
			 GTK_BIN ( bouton_etat_courant ) -> child );
  gtk_container_add ( GTK_CONTAINER ( bouton_etat_courant ),
		      icone );
  gtk_widget_show ( icone );

  /* on met le nom de l'�tat dans la frame du haut */

  gtk_label_set_text ( GTK_LABEL ( label_etat_courant ),
		       etat -> nom_etat );

  /* on affiche l'�tat */

  rafraichissement_etat ( etat );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
gint recherche_etat_par_no ( struct struct_etat *etat,
			     gint *no_etat )
{
  return ( etat -> no_etat != GPOINTER_TO_INT (no_etat) );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* Fontion personnalistation_etat */
/* affiche la fenetre de personnalisation */
/*****************************************************************************************************/

void personnalisation_etat (void)
{
  GtkWidget *dialog;
  gint resultat;
  GtkWidget *notebook;
  gchar *pointeur_char;
  GList *pointeur_liste;
  gint i;

  if ( !etat_courant )
    return;

  /* la fenetre affich�e est une gnome dialog */

  dialog = gnome_dialog_new ( g_strconcat ( _("Personnalisation de l'�tat : "),
					    etat_courant -> nom_etat,
					    NULL ),
			      GNOME_STOCK_BUTTON_OK,
			      GNOME_STOCK_BUTTON_CANCEL,
			      NULL );
  gtk_window_set_transient_for ( GTK_WINDOW ( dialog ),
				 GTK_WINDOW ( window ));
  gnome_dialog_set_default ( GNOME_DIALOG ( dialog ),
			     0 );

  notebook = gtk_notebook_new ();
  
  gtk_notebook_append_page ( GTK_NOTEBOOK ( notebook ),
			     onglet_etat_generalites (etat_courant),
			     gtk_label_new (_(" G�n�ralit�s ")) );

  gtk_notebook_append_page ( GTK_NOTEBOOK ( notebook ),
			     onglet_etat_dates (etat_courant),
			     gtk_label_new (_(" Dates ")) );

  gtk_notebook_append_page ( GTK_NOTEBOOK ( notebook ),
			     onglet_etat_comptes (etat_courant),
			     gtk_label_new (_(" Comptes ")) );

  gtk_notebook_append_page ( GTK_NOTEBOOK ( notebook ),
			     onglet_etat_categories (etat_courant),
			     gtk_label_new (_(" Cat�gories ")) );

  gtk_notebook_append_page ( GTK_NOTEBOOK ( notebook ),
			     onglet_etat_ib (etat_courant),
			     gtk_label_new (_(" Imputation budg�taire ")) );

  gtk_notebook_append_page ( GTK_NOTEBOOK ( notebook ),
			     onglet_etat_tiers (etat_courant),
			     gtk_label_new (_(" Tiers ")) );

  gtk_notebook_append_page ( GTK_NOTEBOOK ( notebook ),
			     onglet_etat_texte (etat_courant),
			     gtk_label_new (_(" Texte ")) );

  gtk_notebook_append_page ( GTK_NOTEBOOK ( notebook ),
			     onglet_etat_montant (etat_courant),
			     gtk_label_new (_(" Montant ")) );

  gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
		       notebook,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( notebook );

 retour_etat:

  resultat = gnome_dialog_run ( GNOME_DIALOG ( dialog ));

  if ( resultat )
    {
      if ( GNOME_IS_DIALOG ( dialog ))
	gnome_dialog_close ( GNOME_DIALOG ( dialog ));
      return;
    }

  /* v�rification que les dates init et finales sont correctes */

  if ( strlen ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entree_date_init_etat ))))
       &&
       !modifie_date ( entree_date_init_etat ))
    {
      dialogue ( _("La date initiale personnelle est invalide") );
      goto retour_etat;
    }

  if ( strlen ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entree_date_finale_etat ))))
       &&
       !modifie_date ( entree_date_finale_etat ))
    {
      dialogue ( _("La date finale personnelle est invalide") );
      goto retour_etat;
    }


  /* on r�cup�re maintenant toutes les donn�es */
  /* r�cup�ration du nom du rapport */

  pointeur_char = g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entree_nom_etat )));

  if ( strlen ( pointeur_char )
       &&
       strcmp ( pointeur_char,
		etat_courant -> nom_etat ))
    {
      etat_courant -> nom_etat = g_strdup ( pointeur_char );

      /* on r�affiche la liste des �tats */

      remplissage_liste_etats ();
    }

  /* r�cup�ration du type de classement */

  g_list_free ( etat_courant -> type_classement );

  etat_courant -> type_classement = NULL;

  for ( i=0 ; i<GTK_CLIST ( liste_type_classement_etat ) -> rows ; i++ )
    {
      gint no;

      no = GPOINTER_TO_INT ( gtk_ctree_node_get_row_data ( GTK_CTREE ( liste_type_classement_etat ),
							   gtk_ctree_node_nth ( GTK_CTREE ( liste_type_classement_etat ),
										i )));

      etat_courant -> type_classement = g_list_append ( etat_courant -> type_classement,
							GINT_TO_POINTER ( no ));

      /* rajoute les ss categ et ss ib */

      if ( no == 1 )
	etat_courant -> type_classement = g_list_append ( etat_courant -> type_classement,
							  GINT_TO_POINTER ( 2 ));
      if ( no == 3 )
	etat_courant -> type_classement = g_list_append ( etat_courant -> type_classement,
							  GINT_TO_POINTER ( 4 ));
    }

  /* r�cup�ration de l'affichage des op�s */

  etat_courant -> afficher_opes = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_opes ));

  etat_courant -> afficher_no_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_no_ope ));
  etat_courant -> afficher_date_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_date_opes ));
  etat_courant -> afficher_tiers_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_tiers_opes ));
  etat_courant -> afficher_categ_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_categ_opes ));
  etat_courant -> afficher_sous_categ_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_sous_categ_opes ));
  etat_courant -> afficher_type_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_type_ope ));
  etat_courant -> afficher_ib_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_ib_opes ));
  etat_courant -> afficher_sous_ib_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_sous_ib_opes ));
  etat_courant -> afficher_cheque_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_no_cheque ));
  etat_courant -> afficher_notes_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_notes_opes ));
  etat_courant -> afficher_pc_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_pc_opes ));
  etat_courant -> afficher_rappr_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_no_rappr ));
  etat_courant -> afficher_infobd_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_infobd_opes ));
  etat_courant -> pas_detailler_ventilation = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_pas_detailler_ventilation ));
  etat_courant -> afficher_exo_ope = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_exo_opes ));
  etat_courant -> afficher_titre_colonnes = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_titres_colonnes ));
  etat_courant -> type_affichage_titres = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_titre_changement ));

  etat_courant -> devise_de_calcul_general = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( bouton_devise_general_etat ) -> menu_item ),
										   "no_devise" ));
  /* r�cup�ration des dates */

  etat_courant -> exo_date = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( radio_button_utilise_exo ));
  etat_courant -> utilise_detail_exo = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_detaille_exo_etat ));

  if ( etat_courant -> no_exercices )
    {
      g_slist_free ( etat_courant -> no_exercices );
      etat_courant -> no_exercices = NULL;
    }

  pointeur_liste = GTK_CLIST ( liste_exo_etat ) -> selection;

  while ( pointeur_liste )
    {
      etat_courant -> no_exercices = g_slist_append ( etat_courant -> no_exercices,
						      gtk_clist_get_row_data ( GTK_CLIST ( liste_exo_etat ),
									       GPOINTER_TO_INT ( pointeur_liste -> data )));
      pointeur_liste = pointeur_liste -> next;
    }

  /*   si tous les exos ont �t� s�lectionn�s, on met bouton_detaille_exo_etat � 0 (plus rapide) */

  if ( ( g_list_length ( GTK_CLIST ( liste_exo_etat ) -> selection )
	 ==
	 GTK_CLIST ( liste_exo_etat ) -> rows )
       &&
       etat_courant -> utilise_detail_exo )
    {
      dialogue ( _("Tous les exercices ont �t� s�lectionn�s ; Grisbi sera plus rapide\nen retirant l'option \"D�tailler les exercices utilis�s\"") );
      etat_courant -> utilise_detail_exo = FALSE;
    }
  

  etat_courant -> separation_par_exo = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_separe_exo_etat ));

  etat_courant -> no_plage_date = GPOINTER_TO_INT ( GTK_CLIST ( liste_plages_dates_etat ) -> selection -> data );

  if ( strlen ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entree_date_init_etat ))))
       &&
       modifie_date ( entree_date_init_etat ))
    {
      gint jour, mois, annee;

      sscanf ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entree_date_init_etat ))),
	       "%d/%d/%d",
	       &jour,
	       &mois,
	       &annee );

      etat_courant -> date_perso_debut = g_date_new_dmy ( jour,
							  mois,
							  annee );
    }

  if ( strlen ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entree_date_finale_etat ))))
       &&
       modifie_date ( entree_date_finale_etat ))
    {
      gint jour, mois, annee;

      sscanf ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entree_date_finale_etat ))),
	       "%d/%d/%d",
	       &jour,
	       &mois,
	       &annee );

      etat_courant -> date_perso_fin = g_date_new_dmy ( jour,
							mois,
							annee );
    }

  etat_courant -> separation_par_plage = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_separe_plages_etat ));
  etat_courant -> type_separation_plage = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( bouton_type_separe_plages_etat ) -> menu_item ),
										  "type" ));
  etat_courant -> jour_debut_semaine = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( bouton_debut_semaine ) -> menu_item ),
									       "jour" ));
  etat_courant -> type_separation_perso = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( bouton_type_separe_perso_etat ) -> menu_item ),
										  "type" ));
  etat_courant -> delai_separation_perso = atoi ( gtk_entry_get_text ( GTK_ENTRY ( entree_separe_perso_etat )));

  /* r�cup�ration des comptes */

  etat_courant -> utilise_detail_comptes = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_detaille_comptes_etat ));

  if ( etat_courant -> no_comptes )
    {
      g_slist_free ( etat_courant -> no_comptes );
      etat_courant -> no_comptes = NULL;
    }

  pointeur_liste = GTK_CLIST ( liste_comptes_etat ) -> selection;

  while ( pointeur_liste )
    {
      etat_courant -> no_comptes = g_slist_append ( etat_courant -> no_comptes,
						    gtk_clist_get_row_data ( GTK_CLIST ( liste_comptes_etat ),
									     GPOINTER_TO_INT ( pointeur_liste -> data )));
      pointeur_liste = pointeur_liste -> next;
    }

  /*   si tous les comptes ont �t� s�lectionn�s, on met utilise_detail_comptes � 0 (plus rapide) */

  if ( ( g_list_length ( GTK_CLIST ( liste_comptes_etat ) -> selection )
	 ==
	 GTK_CLIST ( liste_comptes_etat ) -> rows )
       &&
       etat_courant -> utilise_detail_comptes )
    {
      dialogue ( _("Tous les comptes ont �t� s�lectionn�s ; Grisbi sera plus rapide\nen retirant l'option \"D�tailler les comptes utilis�s\"") );
      etat_courant -> utilise_detail_comptes = FALSE;
    }
  
  etat_courant -> regroupe_ope_par_compte = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_regroupe_ope_compte_etat ));
  etat_courant -> affiche_sous_total_compte = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_affiche_sous_total_compte ));

  /*   r�cup�ration des cat�gories */

  etat_courant -> utilise_categ = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_utilise_categ_etat ));

  etat_courant -> utilise_detail_categ = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_detaille_categ_etat ));

  if ( etat_courant -> no_categ )
    {
      g_slist_free ( etat_courant -> no_categ );
      etat_courant -> no_categ = NULL;
    }

  pointeur_liste = GTK_CLIST ( liste_categ_etat ) -> selection;

  while ( pointeur_liste )
    {
      etat_courant -> no_categ = g_slist_append ( etat_courant -> no_categ,
						  gtk_clist_get_row_data ( GTK_CLIST ( liste_categ_etat ),
									   GPOINTER_TO_INT ( pointeur_liste -> data )));
      pointeur_liste = pointeur_liste -> next;
    }

  /*   si tous les categ ont �t� s�lectionn�s, on met utilise_detail_categ � 0 (plus rapide) */

  if ( ( g_list_length ( GTK_CLIST ( liste_categ_etat ) -> selection )
	 ==
	 GTK_CLIST ( liste_categ_etat ) -> rows )
       &&
       etat_courant -> utilise_detail_categ )
    {
      dialogue ( _("Toutes les cat�gories ont �t� s�lectionn�es ; Grisbi sera plus rapide\nen retirant l'option \"D�tailler les cat�gories utilis�es\"") );
      etat_courant -> utilise_detail_categ = FALSE;
    }
  
  etat_courant -> exclure_ope_sans_categ = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_exclure_ope_sans_categ ));
  etat_courant -> affiche_sous_total_categ = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_affiche_sous_total_categ ));
  etat_courant -> afficher_sous_categ = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_sous_categ ));
  etat_courant -> affiche_sous_total_sous_categ = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_affiche_sous_total_sous_categ ));
  etat_courant -> afficher_pas_de_sous_categ = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_pas_de_sous_categ ));

  if ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_inclusion_virements_actifs_etat )))
    etat_courant -> type_virement = 1;
  else
    if ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_inclusion_virements_hors_etat )))
      etat_courant -> type_virement = 2;
    else
      etat_courant -> type_virement = 0;

  etat_courant -> devise_de_calcul_categ = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( bouton_devise_categ_etat ) -> menu_item ),
										   "no_devise" ));

  /*   r�cup�ration des ib */

  etat_courant -> utilise_ib = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_utilise_ib_etat ));

  etat_courant -> utilise_detail_ib = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_detaille_ib_etat ));

  if ( etat_courant -> no_ib )
    {
      g_slist_free ( etat_courant -> no_ib );
      etat_courant -> no_ib = NULL;
    }

  pointeur_liste = GTK_CLIST ( liste_ib_etat ) -> selection;

  while ( pointeur_liste )
    {
      etat_courant -> no_ib = g_slist_append ( etat_courant -> no_ib,
					       gtk_clist_get_row_data ( GTK_CLIST ( liste_ib_etat ),
									GPOINTER_TO_INT ( pointeur_liste -> data )));
      pointeur_liste = pointeur_liste -> next;
    }

  etat_courant -> afficher_sous_ib = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_sous_ib ));

  /*   si toutes les ib ont �t� s�lectionn�s, on met utilise_detail_ib � 0 (plus rapide) */

  if ( ( g_list_length ( GTK_CLIST ( liste_ib_etat ) -> selection )
	 ==
	 GTK_CLIST ( liste_ib_etat ) -> rows )
       &&
       etat_courant -> utilise_detail_ib )
    {
      dialogue ( _("Toutes les imputations budg�taires ont �t� s�lectionn�es ; Grisbi sera plus rapide\nen retirant l'option \"D�tailler les imputations budg�taires utilis�es\"") );
      etat_courant -> utilise_detail_ib = FALSE;
    }
  
  etat_courant -> exclure_ope_sans_ib = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_exclure_ope_sans_ib ));
  etat_courant -> affiche_sous_total_ib = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_affiche_sous_total_ib ));
  etat_courant -> affiche_sous_total_sous_ib = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_affiche_sous_total_sous_ib ));
  etat_courant -> afficher_pas_de_sous_ib = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_pas_de_sous_ib ));

  etat_courant -> devise_de_calcul_ib = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( bouton_devise_ib_etat ) -> menu_item ),
										   "no_devise" ));



  /*   r�cup�ration des tiers */

  etat_courant -> utilise_tiers = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_utilise_tiers_etat ));

  etat_courant -> utilise_detail_tiers = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_detaille_tiers_etat ));

  if ( etat_courant -> no_tiers )
    {
      g_slist_free ( etat_courant -> no_tiers );
      etat_courant -> no_tiers = NULL;
    }

  pointeur_liste = GTK_CLIST ( liste_tiers_etat ) -> selection;

  while ( pointeur_liste )
    {
      etat_courant -> no_tiers = g_slist_append ( etat_courant -> no_tiers,
						  gtk_clist_get_row_data ( GTK_CLIST ( liste_tiers_etat ),
									   GPOINTER_TO_INT ( pointeur_liste -> data )));
      pointeur_liste = pointeur_liste -> next;
    }

  /*   si tous les tiers ont �t� s�lectionn�s, on met utilise_detail_tiers � 0 (plus rapide) */

  if ( ( g_list_length ( GTK_CLIST ( liste_tiers_etat ) -> selection )
	 ==
	 GTK_CLIST ( liste_tiers_etat ) -> rows )
       &&
       etat_courant -> utilise_detail_tiers )
    {
      dialogue ( _("Tous les tiers ont �t� s�lectionn�s ; Grisbi sera plus rapide\nen retirant l'option \"D�tailler les tiers utilis�s\"") );
      etat_courant -> utilise_detail_tiers = FALSE;
    }
  
  etat_courant -> affiche_sous_total_tiers = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_affiche_sous_total_tiers ));

  etat_courant -> devise_de_calcul_tiers = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( bouton_devise_tiers_etat ) -> menu_item ),
										   "no_devise" ));


  /* r�cup�ration du texte */

  if ( strlen ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entree_texte_etat )))))
    etat_courant -> texte = g_strdup ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entree_texte_etat ))));
  else
    etat_courant -> texte = NULL;

  /* r�cup�ration du montant */

  if ( strlen ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entree_montant_etat )))))
    etat_courant -> montant = g_strtod ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entree_montant_etat ))),
					 NULL );
  else
    etat_courant -> montant = 0;


  gnome_dialog_close ( GNOME_DIALOG ( dialog ));
  modification_fichier ( TRUE );

  /* on r�affiche l'�tat */

  rafraichissement_etat ( etat_courant );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
GtkWidget *onglet_etat_generalites ( struct struct_etat *etat )
{
  GtkWidget *widget_retour;
  GtkWidget *label;
  GtkWidget *hbox;
  GtkWidget *scrolled_window;
  GtkWidget *frame;
  GList *pointeur_liste;
  GtkCTreeNode *parent;
  GtkWidget *vbox;
  GtkWidget *fleche;
  GtkWidget *separateur;
  GtkWidget * table;

  widget_retour = gtk_vbox_new ( FALSE,
				 5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( widget_retour ),
				   10 );
  gtk_widget_show ( widget_retour );

  /* choix du nom du rapport */

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  label = gtk_label_new ( _("Nom de l'�tat : ") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );

  entree_nom_etat = gtk_entry_new ();
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       entree_nom_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( entree_nom_etat );

  /* choix du type de classement */

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  frame = gtk_frame_new ( _("Choix du type de classement :") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       frame,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( frame );

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( hbox ),
				   5 );
  gtk_container_add ( GTK_CONTAINER ( frame ),
		      hbox );
  gtk_widget_show ( hbox );

  scrolled_window = gtk_scrolled_window_new ( FALSE,
					      FALSE );
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_window ),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC );
  gtk_widget_set_usize ( scrolled_window,
			 200,
			 100 );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       scrolled_window,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( scrolled_window );


  liste_type_classement_etat = gtk_ctree_new ( 1,
					       0 );
  gtk_clist_set_column_auto_resize ( GTK_CLIST ( liste_type_classement_etat ),
				     0,
				     TRUE );
  gtk_ctree_set_line_style ( GTK_CTREE ( liste_type_classement_etat ),
			     GTK_CTREE_LINES_NONE );
  gtk_ctree_set_expander_style ( GTK_CTREE ( liste_type_classement_etat ),
				 GTK_CTREE_EXPANDER_NONE );

  gtk_signal_connect ( GTK_OBJECT ( liste_type_classement_etat ),
		       "button_press_event",
		       GTK_SIGNAL_FUNC ( click_liste_etat ),
		       GINT_TO_POINTER (1) );
  gtk_container_add ( GTK_CONTAINER ( scrolled_window ),
		      liste_type_classement_etat );
  gtk_widget_show ( liste_type_classement_etat );

  /* on place ici les fl�ches sur le c�t� de la liste */

  vbox = gtk_vbutton_box_new ();

  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       vbox,
		       FALSE,
		       FALSE,
		       0);

  fleche = gnome_stock_button ( GNOME_STOCK_BUTTON_UP );
  gtk_button_set_relief ( GTK_BUTTON ( fleche ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect ( GTK_OBJECT ( fleche ),
		       "clicked",
		       GTK_SIGNAL_FUNC ( click_haut_classement_etat ),
		       NULL );
  gtk_container_add ( GTK_CONTAINER ( vbox ),
		      fleche  );

  fleche = gnome_stock_button ( GNOME_STOCK_BUTTON_DOWN );
  gtk_button_set_relief ( GTK_BUTTON ( fleche ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect ( GTK_OBJECT ( fleche ),
		       "clicked",
		       GTK_SIGNAL_FUNC ( click_bas_classement_etat ),
		       NULL);
  gtk_container_add ( GTK_CONTAINER ( vbox ),
		      fleche  );
  gtk_widget_show_all ( vbox );


  bouton_afficher_opes = gtk_check_button_new_with_label ( _("Afficher les op�rations") );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_afficher_opes,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_opes );

  /* demande les d�tails affich�s dans les op�rations */

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  frame = gtk_frame_new ( _("Afficher les informations sur") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       frame,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( frame );


  /* connection pour rendre sensitif la frame */

  gtk_signal_connect ( GTK_OBJECT ( bouton_afficher_opes ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       frame );


  table = gtk_table_new ( 9,
			  3,
			  FALSE );
  gtk_container_add ( GTK_CONTAINER ( frame ),
		      table );
  gtk_widget_show ( table );


  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      0, 1,
			      0, 1 );
  gtk_widget_show ( hbox );

  bouton_afficher_no_ope = gtk_check_button_new_with_label ( _("le n� d'op�ration") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_no_ope,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_no_ope );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      1, 2,
			      0, 1 );
  gtk_widget_show ( hbox );

  bouton_afficher_date_opes = gtk_check_button_new_with_label ( _("la date") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_date_opes,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_date_opes );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      2, 3,
			      0, 1 );
  gtk_widget_show ( hbox );

  bouton_afficher_tiers_opes = gtk_check_button_new_with_label ( _("le tiers") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_tiers_opes,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_tiers_opes );


  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      0, 1,
			      1, 2 );
  gtk_widget_show ( hbox );

  bouton_afficher_categ_opes = gtk_check_button_new_with_label ( _("la cat�gorie") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_categ_opes,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_categ_opes );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      1, 2,
			      1, 2 );
  gtk_widget_show ( hbox );

  bouton_afficher_sous_categ_opes = gtk_check_button_new_with_label ( _("la sous-cat�gorie") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_sous_categ_opes,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_sous_categ_opes );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      2, 3,
			      1, 2 );
  gtk_widget_show ( hbox );

  bouton_afficher_type_ope = gtk_check_button_new_with_label ( _("le type d'op�ration") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_type_ope,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_type_ope );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      0, 1,
			      2, 3 );
  gtk_widget_show ( hbox );

  bouton_afficher_ib_opes = gtk_check_button_new_with_label ( _("l'imputation budg�taire") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_ib_opes,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_ib_opes );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      1, 2,
			      2, 3 );
  gtk_widget_show ( hbox );

  bouton_afficher_sous_ib_opes = gtk_check_button_new_with_label ( _("la sous-imputation budg�taire") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_sous_ib_opes,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_sous_ib_opes );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      2, 3,
			      2, 3 );
  gtk_widget_show ( hbox );

  bouton_afficher_no_cheque = gtk_check_button_new_with_label ( _("le n� de ch�que/virement") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_no_cheque,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_no_cheque );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      0, 1,
			      3, 4 );
  gtk_widget_show ( hbox );

  bouton_afficher_notes_opes = gtk_check_button_new_with_label ( _("les notes") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_notes_opes,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_notes_opes );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      1, 2,
			      3, 4 );
  gtk_widget_show ( hbox );

  bouton_afficher_pc_opes = gtk_check_button_new_with_label ( _("la pi�ce comptable") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_pc_opes,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_pc_opes );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      2, 3,
			      3, 4 );
  gtk_widget_show ( hbox );

  bouton_afficher_no_rappr = gtk_check_button_new_with_label ( _("le n� de rapprochement") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_no_rappr,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_no_rappr );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      0, 1,
			      4, 5 );
  gtk_widget_show ( hbox );

  bouton_afficher_infobd_opes = gtk_check_button_new_with_label ( _("l'information banque-guichet") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_infobd_opes,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_infobd_opes );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      1, 2,
			      4, 5 );
  gtk_widget_show ( hbox );

  bouton_afficher_exo_opes = gtk_check_button_new_with_label ( _("l'exercice") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_exo_opes,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_exo_opes );


  /* on propose d'afficher le titre des colonnes */

  separateur = gtk_hseparator_new ();
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			     separateur,
			      0, 3,
			      5, 6 );
  gtk_widget_show ( separateur );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      0, 3,
			      6, 7 );
  gtk_widget_show ( hbox );

  bouton_afficher_titres_colonnes = gtk_check_button_new_with_label ( _("Afficher les titres des colonnes") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_afficher_titres_colonnes,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_titres_colonnes );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      0, 3,
			      7, 8 );
  gtk_widget_show ( hbox );

  bouton_titre_changement = gtk_radio_button_new_with_label ( NULL,
							      _("� chaque changement de section") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_titre_changement,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_titre_changement );

  hbox = gtk_hbox_new ( FALSE,
			0 );
  gtk_table_attach_defaults ( GTK_TABLE ( table ),
			      hbox,
			      0, 3,
			      8, 9 );
  gtk_widget_show ( hbox );

  bouton_titre_en_haut = gtk_radio_button_new_with_label ( gtk_radio_button_group ( GTK_RADIO_BUTTON ( bouton_titre_changement )),
							   _("en haut de l'�tat") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_titre_en_haut,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_titre_en_haut );


  /* on met les connections */

  gtk_signal_connect ( GTK_OBJECT ( bouton_afficher_categ_opes ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       bouton_afficher_sous_categ_opes );
  gtk_signal_connect ( GTK_OBJECT ( bouton_afficher_ib_opes ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       bouton_afficher_sous_ib_opes );
  gtk_signal_connect ( GTK_OBJECT ( bouton_afficher_titres_colonnes ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       bouton_titre_changement );
  gtk_signal_connect ( GTK_OBJECT ( bouton_afficher_titres_colonnes ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       bouton_titre_en_haut );


  bouton_pas_detailler_ventilation = gtk_check_button_new_with_label ( _("Ne pas d�tailler les op�rations ventil�es") );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_pas_detailler_ventilation,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_pas_detailler_ventilation );

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  label = gtk_label_new ( _("Devise utilis�e pour les totaux g�n�raux :") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );

  bouton_devise_general_etat = gtk_option_menu_new ();
  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( bouton_devise_general_etat ),
			     creation_option_menu_devises ( 0,
							    liste_struct_devises ));
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_devise_general_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_devise_general_etat );




  /* on met le nom de l'�tat */

  gtk_entry_set_text ( GTK_ENTRY ( entree_nom_etat ),
		       etat -> nom_etat );

  /* on remplit le ctree en fonction du classement courant */

  pointeur_liste = etat -> type_classement;
  parent = NULL;

  while ( pointeur_liste )
    {
      gchar *text[1];

      text[0] = NULL;

      switch ( GPOINTER_TO_INT ( pointeur_liste -> data ))
	{
	case 1:
	  text[0] = _("Cat�gorie");
	  break;

	case 3:
	  text[0] = _("Imputation budg�taire");
	  break;

	case 5:
	  text[0] = _("Compte");
	  break;

	case 6:
	  text[0] = _("Tiers");
	  break;

	default:
	}

      if ( text[0] )
	{
	  parent = gtk_ctree_insert_node ( GTK_CTREE ( liste_type_classement_etat ),
					   parent,
					   NULL,
					   text,
					   5,
					   NULL,
					   NULL,
					   NULL,
					   NULL,
					   FALSE,
					   TRUE );
	  gtk_ctree_node_set_row_data ( GTK_CTREE ( liste_type_classement_etat ),
					GTK_CTREE_NODE ( parent ),
					pointeur_liste -> data );
	}

      pointeur_liste = pointeur_liste -> next;
    }

  gtk_clist_select_row ( GTK_CLIST ( liste_type_classement_etat ),
			 0,
			 0 );

  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_opes ),
				 etat -> afficher_opes );

  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_no_ope ),
				 etat -> afficher_no_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_date_opes ),
				 etat -> afficher_date_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_tiers_opes ),
				 etat -> afficher_tiers_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_categ_opes ),
				 etat -> afficher_categ_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_sous_categ_opes ),
				 etat -> afficher_sous_categ_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_type_ope ),
				 etat -> afficher_type_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_ib_opes ),
				 etat -> afficher_ib_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_sous_ib_opes ),
				 etat -> afficher_sous_ib_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_no_cheque ),
				 etat -> afficher_cheque_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_notes_opes ),
				 etat -> afficher_notes_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_pc_opes ),
				 etat -> afficher_pc_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_no_rappr ),
				 etat -> afficher_rappr_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_infobd_opes ),
				 etat -> afficher_infobd_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_pas_detailler_ventilation ),
				 etat -> pas_detailler_ventilation );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_exo_opes ),
				 etat -> afficher_exo_ope );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_titres_colonnes ),
				 etat -> afficher_titre_colonnes );
  if ( !etat_courant -> type_affichage_titres )
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_titre_en_haut ),
				   TRUE );

  /* on rend insensitif les sous qque choses si n�cessaire */

  sens_desensitive_pointeur ( bouton_afficher_opes,
			      frame );
  sens_desensitive_pointeur ( bouton_afficher_categ_opes,
			      bouton_afficher_sous_categ_opes );
  sens_desensitive_pointeur ( bouton_afficher_ib_opes,
			      bouton_afficher_sous_ib_opes );
  sens_desensitive_pointeur ( bouton_afficher_titres_colonnes,
			      bouton_titre_changement );
  sens_desensitive_pointeur ( bouton_afficher_titres_colonnes,
			      bouton_titre_en_haut );

  /* mise en forme de la devise */

  gtk_option_menu_set_history ( GTK_OPTION_MENU ( bouton_devise_general_etat ),
				g_slist_position ( liste_struct_devises,
						   g_slist_find_custom ( liste_struct_devises,
									 GINT_TO_POINTER ( etat -> devise_de_calcul_general ),
									 ( GCompareFunc ) recherche_devise_par_no )));

  return ( widget_retour );
}
/*****************************************************************************************************/


/*****************************************************************************************************/
void click_haut_classement_etat ( void )
{
  GtkCTreeNode *node_parent;
  GtkCTreeNode *node;
  GtkCTreeNode *node_enfant;
  GtkCTreeNode *nouveau_parent;

  node = GTK_CLIST ( liste_type_classement_etat )->selection -> data;

  /*   si on est au niveau 1, peut pas plus haut */

  if ( GTK_CTREE_ROW ( node ) -> level == 1 )
    return;

  node_parent = GTK_CTREE_ROW ( GTK_CLIST ( liste_type_classement_etat )->selection -> data )->parent;
  node_enfant = GTK_CTREE_ROW ( GTK_CLIST ( liste_type_classement_etat )->selection -> data )->children;

  nouveau_parent = GTK_CTREE_ROW ( node_parent )->parent;

  /* on remonte le node */

  gtk_ctree_move ( GTK_CTREE ( liste_type_classement_etat ),
		   node,
		   nouveau_parent,
		   NULL );

  /* on descend celui du dessus */

  gtk_ctree_move ( GTK_CTREE ( liste_type_classement_etat ),
		   node_parent,
		   node,
		   NULL );

  /* on attache l'enfant � son nouveau parent */

  if ( node_enfant )
    gtk_ctree_move ( GTK_CTREE ( liste_type_classement_etat ),
		     node_enfant,
		     node_parent,
		     NULL );

  gtk_ctree_expand_recursive ( GTK_CTREE ( liste_type_classement_etat ),
			       node );

}
/*****************************************************************************************************/


/*****************************************************************************************************/
void click_bas_classement_etat ( void )
{
  GtkCTreeNode *node_parent;
  GtkCTreeNode *node;
  GtkCTreeNode *node_enfant;
  GtkCTreeNode *node_enfant_enfant;

  node = GTK_CLIST ( liste_type_classement_etat )->selection -> data;

  /*   si on est au niveau 4, peut pas plus bas */

  if ( GTK_CTREE_ROW ( node ) -> level == 4 )
    return;

  node_parent = GTK_CTREE_ROW ( GTK_CLIST ( liste_type_classement_etat )->selection -> data )->parent;
  node_enfant = GTK_CTREE_ROW ( GTK_CLIST ( liste_type_classement_etat )->selection -> data )->children;

  node_enfant_enfant = GTK_CTREE_ROW ( node_enfant )->children;

  /* on remonte le node enfant */

  gtk_ctree_move ( GTK_CTREE ( liste_type_classement_etat ),
		   node_enfant,
		   node_parent,
		   NULL );

  /* on descend le node */

  gtk_ctree_move ( GTK_CTREE ( liste_type_classement_etat ),
		   node,
		   node_enfant,
		   NULL );

  /* on attache l'enfant de l'enfant � son nouveau parent */

  if ( node_enfant_enfant )
    gtk_ctree_move ( GTK_CTREE ( liste_type_classement_etat ),
		     node_enfant_enfant,
		     node,
		     NULL );

  gtk_ctree_expand_recursive ( GTK_CTREE ( liste_type_classement_etat ),
			       node );

}
/*****************************************************************************************************/



/*****************************************************************************************************/
void sens_desensitive_pointeur ( GtkWidget *bouton,
				 GtkWidget *widget )
{
  gtk_widget_set_sensitive ( widget,
			     gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton )));

}
/*****************************************************************************************************/



/*****************************************************************************************************/
GtkWidget *onglet_etat_dates ( struct struct_etat *etat )
{
  GtkWidget *widget_retour;
  GtkWidget *separateur;
  GtkWidget *vbox;
  GtkWidget *scrolled_window;
  GSList *pointeur_liste;
  gchar **plages_dates;
  gint i;
  GtkWidget *hbox;
  GtkWidget *label;
  GtkWidget *frame;
  GtkWidget *menu;
  GtkWidget *menu_item;

  widget_retour = gtk_hbox_new ( FALSE,
				 5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( widget_retour ),
				   10 );
  gtk_widget_show ( widget_retour );


  /*   on met en forme la partie de gauche : utilisation des exercices */

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       vbox,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox );

  radio_button_utilise_exo = gtk_radio_button_new_with_label ( NULL,
							       _("Utiliser les exercices") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       radio_button_utilise_exo,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( radio_button_utilise_exo );

  /* on met la liste des exos sous ce radio button */

  frame = gtk_frame_new ( FALSE );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       frame,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( frame );


  /* on met la connection pour rendre sensitif cette frame */

  gtk_signal_connect ( GTK_OBJECT (radio_button_utilise_exo ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       frame );

  vbox_utilisation_exo = gtk_vbox_new ( FALSE,
					5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( vbox_utilisation_exo ),
				   10 );
  gtk_container_add ( GTK_CONTAINER ( frame ),
		      vbox_utilisation_exo );
  gtk_widget_show ( vbox_utilisation_exo );

  bouton_detaille_exo_etat = gtk_check_button_new_with_label ( _("D�tailler les exercices utilis�s") );
  gtk_box_pack_start ( GTK_BOX ( vbox_utilisation_exo ),
		       bouton_detaille_exo_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_detaille_exo_etat );

  vbox_generale_exo_etat = gtk_vbox_new ( FALSE,
					  5 );
  gtk_box_pack_start ( GTK_BOX ( vbox_utilisation_exo ),
		       vbox_generale_exo_etat,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox_generale_exo_etat );

  gtk_signal_connect ( GTK_OBJECT ( bouton_detaille_exo_etat ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       vbox_generale_exo_etat );

  scrolled_window = gtk_scrolled_window_new ( FALSE,
					      FALSE );
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_window ),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC );
  gtk_box_pack_start ( GTK_BOX ( vbox_generale_exo_etat ),
		       scrolled_window,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( scrolled_window );

  liste_exo_etat = gtk_clist_new ( 1 );
  gtk_clist_set_selection_mode ( GTK_CLIST ( liste_exo_etat ),
				 GTK_SELECTION_MULTIPLE );
  gtk_clist_set_column_auto_resize ( GTK_CLIST ( liste_exo_etat ),
				     0,
				     TRUE );
  gtk_container_add ( GTK_CONTAINER ( scrolled_window ),
		      liste_exo_etat );
  gtk_widget_show ( liste_exo_etat );

  /* on remplit la liste des exercices */

  pointeur_liste = liste_struct_exercices;

  while ( pointeur_liste )
    {
      struct struct_exercice *exercice;
      gchar *nom[1];
      gint ligne;

      exercice = pointeur_liste -> data;

      nom[0] = exercice -> nom_exercice;

      ligne = gtk_clist_append ( GTK_CLIST ( liste_exo_etat ),
				 nom );

      gtk_clist_set_row_data ( GTK_CLIST ( liste_exo_etat ),
			       ligne,
			       GINT_TO_POINTER ( exercice -> no_exercice ));

      pointeur_liste = pointeur_liste -> next;
    }

  bouton_separe_exo_etat = gtk_check_button_new_with_label ( _("S�parer les r�sultats par exercice") );
  gtk_box_pack_start ( GTK_BOX ( vbox_utilisation_exo ),
		       bouton_separe_exo_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_separe_exo_etat );

  separateur = gtk_vseparator_new ();
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       separateur,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( separateur );


  /* mise en place de la plage de dates */

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       vbox,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox );

  radio_button_utilise_dates = gtk_radio_button_new_with_label ( gtk_radio_button_group ( GTK_RADIO_BUTTON ( radio_button_utilise_exo )),
								 _("Utiliser des plages de dates") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       radio_button_utilise_dates,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( radio_button_utilise_dates );

  /* on met en dessous une liste avec les plages de date propos�es */

  frame = gtk_frame_new (NULL);
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       frame,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( frame );

  vbox_utilisation_date = gtk_vbox_new ( FALSE,
					 5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( vbox_utilisation_date ),
				   10 );
  gtk_container_add ( GTK_CONTAINER ( frame ),
		      vbox_utilisation_date );
  gtk_widget_show ( vbox_utilisation_date );

  /* on met la connection pour rendre sensitif cette frame */

  gtk_signal_connect ( GTK_OBJECT ( radio_button_utilise_dates ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       vbox_utilisation_date );

  scrolled_window = gtk_scrolled_window_new ( FALSE,
					      FALSE );
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_window ),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC );
  gtk_box_pack_start ( GTK_BOX ( vbox_utilisation_date ),
		       scrolled_window,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( scrolled_window );


  liste_plages_dates_etat = gtk_clist_new ( 1 );
  gtk_clist_set_column_auto_resize ( GTK_CLIST ( liste_plages_dates_etat ),
				     0,
				     TRUE );
  gtk_signal_connect ( GTK_OBJECT ( liste_plages_dates_etat ),
		       "button_press_event",
		       GTK_SIGNAL_FUNC ( click_liste_etat ),
		       NULL );
  gtk_container_add ( GTK_CONTAINER ( scrolled_window ),
		      liste_plages_dates_etat );
  gtk_widget_show ( liste_plages_dates_etat );

  /* on remplit la liste des exercices */

  plages_dates = liste_plages_dates;

  i = 0;

  while ( plages_dates[i] )
    {
      gint ligne;

      ligne = gtk_clist_append ( GTK_CLIST ( liste_plages_dates_etat ),
				 &plages_dates[i] );
      i++;
    }

  /* on met ensuite la date perso de d�but */

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( vbox_utilisation_date ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  label = gtk_label_new ( _("Date initiale : ") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );

  entree_date_init_etat = gtk_entry_new_with_max_length ( 11 );
  gtk_widget_set_usize ( entree_date_init_etat,
			 100,
			 FALSE );
  gtk_signal_connect ( GTK_OBJECT ( entree_date_init_etat ),
		       "button_press_event",
		       GTK_SIGNAL_FUNC ( clique_sur_entree_date_etat ),
		       NULL );
  gtk_signal_connect_object ( GTK_OBJECT ( entree_date_init_etat ),
			      "focus_out_event",
			      GTK_SIGNAL_FUNC ( modifie_date ),
			      GTK_OBJECT ( entree_date_init_etat ));
  gtk_box_pack_end ( GTK_BOX ( hbox ),
		     entree_date_init_etat,
		     FALSE,
		     FALSE,
		     0 );
  gtk_widget_show ( entree_date_init_etat );

  /* on met ensuite la date perso de fin */

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( vbox_utilisation_date ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  label = gtk_label_new ( _("Date finale : ") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );

  entree_date_finale_etat = gtk_entry_new_with_max_length ( 11 );
  gtk_widget_set_usize ( entree_date_finale_etat,
			 100,
			 FALSE );
  gtk_signal_connect ( GTK_OBJECT ( entree_date_finale_etat ),
		       "button_press_event",
		       GTK_SIGNAL_FUNC ( clique_sur_entree_date_etat ),
		       NULL );
  gtk_signal_connect_object ( GTK_OBJECT ( entree_date_finale_etat ),
			      "focus_out_event",
			      GTK_SIGNAL_FUNC ( modifie_date ),
			      GTK_OBJECT ( entree_date_finale_etat ));
  gtk_box_pack_end ( GTK_BOX ( hbox ),
		     entree_date_finale_etat,
		     FALSE,
		     FALSE,
		     0 );
  gtk_widget_show ( entree_date_finale_etat );

  
  /* on permet ensuite la s�paration des r�sultats */

  separateur = gtk_hseparator_new ();
  gtk_box_pack_start ( GTK_BOX ( vbox_utilisation_date ),
		       separateur,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( separateur );


  bouton_separe_plages_etat = gtk_check_button_new_with_label ( _("S�parer les r�sultats par p�riode") );
  gtk_signal_connect ( GTK_OBJECT ( bouton_separe_plages_etat ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( change_separation_result_periode ),
		       NULL );
  gtk_box_pack_start ( GTK_BOX ( vbox_utilisation_date ),
		       bouton_separe_plages_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_separe_plages_etat );

  /* mise en place de la ligne type - choix perso */

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( vbox_utilisation_date ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

 

  bouton_type_separe_plages_etat = gtk_option_menu_new ();

  menu = gtk_menu_new ();

  menu_item = gtk_menu_item_new_with_label ( _("Semaine") );
  gtk_object_set_data ( GTK_OBJECT ( menu_item ),
			"type",
			NULL );
  gtk_signal_connect_object ( GTK_OBJECT ( menu_item ),
			      "activate",
			      GTK_SIGNAL_FUNC ( modif_type_separation_dates ),
			      NULL );
  gtk_menu_append ( GTK_MENU ( menu ),
		    menu_item );
  gtk_widget_show ( menu_item );

  menu_item = gtk_menu_item_new_with_label ( _("Mois") );
  gtk_object_set_data ( GTK_OBJECT ( menu_item ),
			"type",
			GINT_TO_POINTER (1) );
  gtk_signal_connect_object ( GTK_OBJECT ( menu_item ),
			      "activate",
			      GTK_SIGNAL_FUNC ( modif_type_separation_dates ),
			      GINT_TO_POINTER (1) );
  gtk_menu_append ( GTK_MENU ( menu ),
		    menu_item );
  gtk_widget_show ( menu_item );

  menu_item = gtk_menu_item_new_with_label ( _("Ann�e") );
  gtk_object_set_data ( GTK_OBJECT ( menu_item ),
			"type",
			GINT_TO_POINTER (2));
  gtk_signal_connect_object ( GTK_OBJECT ( menu_item ),
			      "activate",
			      GTK_SIGNAL_FUNC ( modif_type_separation_dates ),
			      GINT_TO_POINTER (2));
  gtk_menu_append ( GTK_MENU ( menu ),
		    menu_item );
  gtk_widget_show ( menu_item );

  menu_item = gtk_menu_item_new_with_label ( _("Personnalis�") );
  gtk_object_set_data ( GTK_OBJECT ( menu_item ),
			"type",
			GINT_TO_POINTER (3));
  gtk_signal_connect_object ( GTK_OBJECT ( menu_item ),
			      "activate",
			      GTK_SIGNAL_FUNC ( modif_type_separation_dates ),
			      GINT_TO_POINTER (3));
  gtk_menu_append ( GTK_MENU ( menu ),
		    menu_item );
  gtk_widget_show ( menu_item );

  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( bouton_type_separe_plages_etat ),
			     menu );
  gtk_widget_show ( menu );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_type_separe_plages_etat,
		       FALSE, 
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_type_separe_plages_etat );

  entree_separe_perso_etat = gtk_entry_new ();
  gtk_widget_set_usize ( entree_separe_perso_etat,
			 50,
			 FALSE );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       entree_separe_perso_etat,
		       FALSE, 
		       FALSE,
		       0 );
  gtk_widget_show ( entree_separe_perso_etat );


  bouton_type_separe_perso_etat = gtk_option_menu_new ();

  menu = gtk_menu_new ();

  menu_item = gtk_menu_item_new_with_label ( _("Jours") );
  gtk_object_set_data ( GTK_OBJECT ( menu_item ),
			"type",
			GINT_TO_POINTER (0));
  gtk_menu_append ( GTK_MENU ( menu ),
		    menu_item );
  gtk_widget_show ( menu_item );

  menu_item = gtk_menu_item_new_with_label ( _("Mois") );
  gtk_object_set_data ( GTK_OBJECT ( menu_item ),
			"type",
			GINT_TO_POINTER (1));
  gtk_menu_append ( GTK_MENU ( menu ),
		    menu_item );
  gtk_widget_show ( menu_item );

  menu_item = gtk_menu_item_new_with_label ( _("Ans") );
  gtk_object_set_data ( GTK_OBJECT ( menu_item ),
			"type",
			GINT_TO_POINTER (2));
  gtk_menu_append ( GTK_MENU ( menu ),
		    menu_item );
  gtk_widget_show ( menu_item );

  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( bouton_type_separe_perso_etat ),
			     menu );
  gtk_widget_show ( menu );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_type_separe_perso_etat,
		       FALSE, 
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_type_separe_perso_etat );

  /* mise en place de la ligne de d�but de semaine */

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( vbox_utilisation_date ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  label = gtk_label_new ( _("La semaine commence le ") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       label,
		       FALSE, 
		       FALSE,
		       0 );
  gtk_widget_show ( label );


  bouton_debut_semaine = gtk_option_menu_new ();

  menu = gtk_menu_new ();

  i = 0;

  while ( jours_semaine[i] )
    {
      menu_item = gtk_menu_item_new_with_label ( jours_semaine[i] );
      gtk_object_set_data ( GTK_OBJECT ( menu_item ),
			    _("jour"),
			    GINT_TO_POINTER (i));
      gtk_menu_append ( GTK_MENU ( menu ),
			menu_item );
      gtk_widget_show ( menu_item );
      i++;
    }

  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( bouton_debut_semaine ),
			     menu );
  gtk_widget_show ( menu );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_debut_semaine,
		       FALSE, 
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_debut_semaine );



  /* on remplit maintenant en fonction des donn�es */

  if ( etat -> exo_date )
    {
      gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( radio_button_utilise_exo ),
				     TRUE );
      gtk_widget_set_sensitive ( vbox_utilisation_date,
				 FALSE );
    }
  else
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( radio_button_utilise_dates ),
				   TRUE );


  if ( etat -> utilise_detail_exo )
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_detaille_exo_etat ),
				   TRUE );
  else
    gtk_widget_set_sensitive ( vbox_generale_exo_etat,
			       FALSE );

  /* on s�lectionne les exercices */

  pointeur_liste = etat -> no_exercices;

  while ( pointeur_liste )
    {
      gtk_clist_select_row ( GTK_CLIST ( liste_exo_etat ),
			     g_slist_position ( liste_struct_exercices,
						g_slist_find_custom ( liste_struct_exercices,
								      pointeur_liste -> data,
								      (GCompareFunc) recherche_exercice_par_no )),
			     0 );
      pointeur_liste = pointeur_liste -> next;
    }



  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_separe_exo_etat ),
				 etat -> separation_par_exo );

  /* on s�lectionne la plage de date */

  gtk_clist_select_row ( GTK_CLIST ( liste_plages_dates_etat ),
			 etat -> no_plage_date,
			 0 );

  if ( etat -> no_plage_date != 1 )
    {
      gtk_widget_set_sensitive ( entree_date_init_etat,
				 FALSE );
      gtk_widget_set_sensitive ( entree_date_finale_etat,
				 FALSE );
    }
  else
    {
      gtk_widget_set_sensitive ( entree_date_init_etat,
				 TRUE );
      gtk_widget_set_sensitive ( entree_date_finale_etat,
				 TRUE );
    }

  /* on remplit les dates perso si elles existent */

  if ( etat -> date_perso_debut )
    gtk_entry_set_text ( GTK_ENTRY ( entree_date_init_etat ),
			 g_strdup_printf ( "%d/%d/%d",
					   g_date_day ( etat -> date_perso_debut ),
					   g_date_month ( etat -> date_perso_debut ),
					   g_date_year ( etat -> date_perso_debut )));

  if ( etat -> date_perso_fin )
    gtk_entry_set_text ( GTK_ENTRY ( entree_date_finale_etat ),
			 g_strdup_printf ( "%d/%d/%d",
					   g_date_day ( etat -> date_perso_fin ),
					   g_date_month ( etat -> date_perso_fin ),
					   g_date_year ( etat -> date_perso_fin )));

  /* on remplit les d�tails de la s�paration des dates */

  if ( etat -> separation_par_plage )
    {
      gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_separe_plages_etat ),
				     TRUE );
      gtk_widget_set_sensitive ( bouton_type_separe_plages_etat,
				 TRUE );
      modif_type_separation_dates ( GINT_TO_POINTER ( etat -> type_separation_plage ));
    }
  else
    {
      gtk_widget_set_sensitive ( bouton_type_separe_plages_etat,
				 FALSE );
      gtk_widget_set_sensitive ( bouton_debut_semaine,
				 FALSE );
      gtk_widget_set_sensitive ( bouton_type_separe_perso_etat,
				 FALSE );
      gtk_widget_set_sensitive ( entree_separe_perso_etat,
				 FALSE );
    }

  gtk_option_menu_set_history ( GTK_OPTION_MENU ( bouton_type_separe_plages_etat ),
				etat -> type_separation_plage );
  gtk_option_menu_set_history ( GTK_OPTION_MENU ( bouton_debut_semaine ),
				etat -> jour_debut_semaine );
  gtk_option_menu_set_history ( GTK_OPTION_MENU ( bouton_type_separe_perso_etat ),
				etat -> type_separation_perso );

  if ( etat -> delai_separation_perso )
    gtk_entry_set_text ( GTK_ENTRY ( entree_separe_perso_etat ),
			 itoa ( etat -> delai_separation_perso ));


  return ( widget_retour );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* v�rifie o� l'on clique et emp�che la d�s�lection de ligne */
/*****************************************************************************************************/

void click_liste_etat ( GtkCList *liste,
			GdkEventButton *evenement,
			gint origine )
{
  gint colonne, x, y;
  gint ligne;

  /*   origine = 0 si �a vient des dates, dans ce cas on sensitive les entr�es init et fin */
  /* origine = 1 si �a vient du choix de type de classement */


  gtk_signal_emit_stop_by_name ( GTK_OBJECT ( liste ),
				 "button_press_event");

  /* R�cup�ration des coordonn�es de la souris */

  gdk_window_get_pointer ( GTK_CLIST ( liste ) -> clist_window,
			   &x,
			   &y,
			   FALSE );

  gtk_clist_get_selection_info ( GTK_CLIST ( liste ),
				 x,
				 y,
				 &ligne,
				 &colonne);

  if ( GTK_CLIST ( liste ) -> selection
       &&
       GPOINTER_TO_INT ( GTK_CLIST ( liste ) -> selection -> data ) == ligne )
    return;

  gtk_clist_select_row ( GTK_CLIST ( liste ),
			 ligne,
			 0 );

  if ( !origine )
    {
      if ( ligne )
	{
	  gtk_widget_set_sensitive ( entree_date_init_etat,
				     FALSE );
	  gtk_widget_set_sensitive ( entree_date_finale_etat,
				     FALSE );
	}
      else
	{
	  gtk_widget_set_sensitive ( entree_date_init_etat,
				     TRUE );
	  gtk_widget_set_sensitive ( entree_date_finale_etat,
				     TRUE );
	}
    }

}
/*****************************************************************************************************/



/*****************************************************************************************************/
void clique_sur_entree_date_etat ( GtkWidget *entree,
				   GdkEventButton *ev )
{

  if ( ev->type == GDK_2BUTTON_PRESS )
    {
      GtkWidget *popup;
      GtkWidget *popup_boxv;
      GtkRequisition *taille_entree;
      gint x, y;
      GtkWidget *calendrier;
      int cal_jour, cal_mois, cal_annee;
      GtkWidget *bouton;
      GtkWidget *frame;

      /* cherche la position o� l'on va mettre la popup */

      taille_entree = malloc ( sizeof ( GtkRequisition ));

      gdk_window_get_origin ( GTK_WIDGET ( entree ) -> window,
			      &x,
			      &y );
      gtk_widget_size_request ( GTK_WIDGET ( entree ),
				taille_entree );
  
      y = y + taille_entree->height;


      /* cr�ation de la popup */

      popup = gtk_window_new ( GTK_WINDOW_POPUP );
      gtk_window_set_modal ( GTK_WINDOW (popup),
			     TRUE);
      gtk_widget_set_uposition ( GTK_WIDGET ( popup ),
				 x,
				 y );

      /* on associe l'entr�e qui sera remplie � la popup */

      gtk_object_set_data ( GTK_OBJECT ( popup ),
			    "entree",
			    entree );

      /* cr�ation de l'int�rieur de la popup */

      frame = gtk_frame_new ( NULL );
      gtk_container_add ( GTK_CONTAINER (popup),
			  frame);
      gtk_widget_show ( frame );

      popup_boxv = gtk_vbox_new ( FALSE,
				  5 );
      gtk_container_set_border_width ( GTK_CONTAINER ( popup_boxv ),
				       5 );

      gtk_container_add ( GTK_CONTAINER ( frame ),
			  popup_boxv);
      gtk_widget_show ( popup_boxv );

      if ( !( strlen ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( entree ))))
	      &&
	      sscanf ( gtk_entry_get_text ( GTK_ENTRY ( entree )),
		       "%d/%d/%d",
		       &cal_jour,
		       &cal_mois,
		       &cal_annee)))
	sscanf ( date_jour(),
		 "%d/%d/%d",
		 &cal_jour,
		 &cal_mois,
		 &cal_annee);
      
      calendrier = gtk_calendar_new();
      gtk_calendar_select_month ( GTK_CALENDAR ( calendrier ),
				  cal_mois-1,
				  cal_annee);
      gtk_calendar_select_day  ( GTK_CALENDAR ( calendrier ),
				 cal_jour);

      gtk_calendar_display_options ( GTK_CALENDAR ( calendrier ),
				     GTK_CALENDAR_SHOW_HEADING |
				     GTK_CALENDAR_SHOW_DAY_NAMES |
				     GTK_CALENDAR_WEEK_START_MONDAY );

      gtk_signal_connect ( GTK_OBJECT ( calendrier),
			   "day_selected_double_click",
			   GTK_SIGNAL_FUNC ( date_selectionnee_etat ),
			   popup );
      gtk_signal_connect ( GTK_OBJECT ( popup ),
			   "key_press_event",
			   GTK_SIGNAL_FUNC ( touche_calendrier ),
			   NULL );
      gtk_signal_connect_object ( GTK_OBJECT ( popup ),
				  "destroy",
				  GTK_SIGNAL_FUNC ( gdk_pointer_ungrab ),
				  GDK_CURRENT_TIME );
      gtk_box_pack_start ( GTK_BOX ( popup_boxv ),
			   calendrier,
			   TRUE,
			   TRUE,
			   0 );
      gtk_widget_show ( calendrier );


      /* ajoute le bouton annuler */

      bouton = gtk_button_new_with_label ( _("Annuler") );
      gtk_signal_connect_object ( GTK_OBJECT ( bouton ),
				  "clicked",
				  GTK_SIGNAL_FUNC ( gtk_widget_destroy ),
				  GTK_OBJECT ( popup ));
      gtk_box_pack_start ( GTK_BOX ( popup_boxv ),
			   bouton,
			   TRUE,
			   TRUE,
			   0 );
      gtk_widget_show ( bouton );

      gtk_widget_show (popup);
      
      gdk_pointer_grab ( popup -> window, 
			 TRUE,
			 GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
			 GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK |
			 GDK_POINTER_MOTION_MASK,
			 NULL, 
			 NULL, 
			 GDK_CURRENT_TIME );

      gtk_widget_grab_focus ( GTK_WIDGET ( popup ));
    }
}
/*****************************************************************************************************/



/***********************************************************************************************************/
/* Fonction date_selectionnee */
/* appel�e lorsqu'on a click� 2 fois sur une date du calendrier */
/***********************************************************************************************************/

void date_selectionnee_etat ( GtkCalendar *calendrier,
			      GtkWidget *popup )
{
  guint annee, mois, jour;

  gtk_calendar_get_date ( calendrier,
			  &annee,
			  &mois,
			  &jour);

  gtk_entry_set_text ( GTK_ENTRY ( gtk_object_get_data ( GTK_OBJECT ( popup ),
							 "entree" )),
		       g_strdup_printf ( "%02d/%02d/%d",
					 jour,
					 mois + 1,
					 annee));
  gtk_widget_destroy ( popup );
}
/***********************************************************************************************************/



/*****************************************************************************************************/
void change_separation_result_periode ( void )
{
  if ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton_separe_plages_etat )))
    {
      gtk_widget_set_sensitive ( bouton_type_separe_plages_etat,
				 TRUE );
      modif_type_separation_dates ( gtk_object_get_data ( GTK_OBJECT ( bouton_type_separe_plages_etat ),
							  "type" ));
    }
  else
    {
      gtk_widget_set_sensitive ( bouton_type_separe_plages_etat,
				 FALSE );
      gtk_widget_set_sensitive ( bouton_debut_semaine,
				 FALSE );
      gtk_widget_set_sensitive ( bouton_type_separe_perso_etat,
				 FALSE );
      gtk_widget_set_sensitive ( entree_separe_perso_etat,
				 FALSE );
    }
}
/*****************************************************************************************************/



/*****************************************************************************************************/
void modif_type_separation_dates ( gint *origine )
{

  switch ( GPOINTER_TO_INT ( origine ))
    {
    case 0:
      gtk_widget_set_sensitive ( bouton_debut_semaine,
				 TRUE );
      gtk_widget_set_sensitive ( bouton_type_separe_perso_etat,
				 FALSE );
      gtk_widget_set_sensitive ( entree_separe_perso_etat,
				 FALSE );
      break;

    case 1:
    case 2:
      gtk_widget_set_sensitive ( bouton_debut_semaine,
				 FALSE );
      gtk_widget_set_sensitive ( bouton_type_separe_perso_etat,
				 FALSE );
      gtk_widget_set_sensitive ( entree_separe_perso_etat,
				 FALSE );
      break;

    case 3:
      gtk_widget_set_sensitive ( bouton_debut_semaine,
				 FALSE );
      gtk_widget_set_sensitive ( bouton_type_separe_perso_etat,
				 TRUE );
      gtk_widget_set_sensitive ( entree_separe_perso_etat,
				 TRUE );
    }
}
/*****************************************************************************************************/



/*****************************************************************************************************/
GtkWidget *onglet_etat_comptes ( struct struct_etat *etat )
{
  GtkWidget *widget_retour;
  GtkWidget *scrolled_window;
  GtkWidget *bouton;
  GtkWidget *vbox;
  gint i;
  GtkWidget *label;
  GSList *pointeur_liste;
  GtkWidget *hbox;

  widget_retour = gtk_vbox_new ( FALSE,
				 5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( widget_retour ),
				   10 );
  gtk_widget_show ( widget_retour );


  /* on met dans la partie de gauche une liste contenant les comptes � */
  /* s�lectionner */

  bouton_detaille_comptes_etat = gtk_check_button_new_with_label ( _("D�tailler les comptes utilis�s") );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_detaille_comptes_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_detaille_comptes_etat );

  vbox_generale_comptes_etat = gtk_vbox_new ( FALSE,
					      5 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       vbox_generale_comptes_etat,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox_generale_comptes_etat );


  gtk_signal_connect ( GTK_OBJECT ( bouton_detaille_comptes_etat ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       vbox_generale_comptes_etat );

  label = gtk_label_new ( _("S�lectionner les comptes � inclure dans l'�tat :") );
  gtk_box_pack_start ( GTK_BOX ( vbox_generale_comptes_etat ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( vbox_generale_comptes_etat ),
		       hbox,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( hbox );

  scrolled_window = gtk_scrolled_window_new ( FALSE,
					      FALSE );
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_window ),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       scrolled_window,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( scrolled_window );

  liste_comptes_etat = gtk_clist_new ( 1 );
  gtk_clist_set_selection_mode ( GTK_CLIST ( liste_comptes_etat ),
				 GTK_SELECTION_MULTIPLE );
  gtk_clist_set_column_auto_resize ( GTK_CLIST ( liste_comptes_etat ),
				     0,
				     TRUE );
  gtk_container_add ( GTK_CONTAINER ( scrolled_window ),
		      liste_comptes_etat );
  gtk_widget_show ( liste_comptes_etat );

  /* on remplit la liste des comptes */

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

  for ( i=0 ; i<nb_comptes ; i++ )
    {
      gchar *nom[1];
      gint ligne;

      nom[0] = NOM_DU_COMPTE;

      ligne = gtk_clist_append ( GTK_CLIST ( liste_comptes_etat ),
				 nom );

      gtk_clist_set_row_data ( GTK_CLIST ( liste_comptes_etat ),
			       ligne,
			       GINT_TO_POINTER ( NO_COMPTE ));
      p_tab_nom_de_compte_variable++;
    }

      

  /*   sur la partie de droite, on met les boutons (d�)s�lectionner tout */

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       vbox,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox );

  bouton = gtk_button_new_with_label ( _("S�lectionner tout") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT  ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( gtk_clist_select_all ),
			      GTK_OBJECT  ( liste_comptes_etat ));
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );

  bouton = gtk_button_new_with_label ( _("D�s�lectionner tout") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT  ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( gtk_clist_unselect_all ),
			      GTK_OBJECT  ( liste_comptes_etat ));
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );


  bouton_regroupe_ope_compte_etat = gtk_check_button_new_with_label ( _("Regrouper les op�rations par compte") );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_regroupe_ope_compte_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_regroupe_ope_compte_etat );

  bouton_affiche_sous_total_compte = gtk_check_button_new_with_label ( _("Afficher un sous-total lors d'un changement de compte") );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_affiche_sous_total_compte,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_affiche_sous_total_compte );

  gtk_signal_connect ( GTK_OBJECT ( bouton_regroupe_ope_compte_etat ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       bouton_affiche_sous_total_compte );

  /* on va maintenant s�lectionner les comptes n�cessaires */

  if ( etat -> utilise_detail_comptes )
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_detaille_comptes_etat ),
				   TRUE );
  else
    gtk_widget_set_sensitive ( vbox_generale_comptes_etat,
			       FALSE );

  pointeur_liste = etat -> no_comptes;

  while ( pointeur_liste )
    {
      gtk_clist_select_row ( GTK_CLIST ( liste_comptes_etat ),
			     gtk_clist_find_row_from_data ( GTK_CLIST ( liste_comptes_etat ),
							    pointeur_liste -> data ),
			     0 );
      pointeur_liste = pointeur_liste -> next;
    }

  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_regroupe_ope_compte_etat ),
				 etat -> regroupe_ope_par_compte );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_affiche_sous_total_compte ),
				 etat -> affiche_sous_total_compte );

  sens_desensitive_pointeur ( bouton_regroupe_ope_compte_etat,
			      bouton_affiche_sous_total_compte );
  return ( widget_retour );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
GtkWidget *onglet_etat_categories ( struct struct_etat *etat )
{
  GtkWidget *widget_retour;
  GtkWidget *frame;
  GtkWidget *vbox;
  GtkWidget *label;
  GSList *pointeur_liste;
  GtkWidget *scrolled_window;
  GtkWidget *bouton;
  GtkWidget *separateur;
  GtkWidget *hbox;

  widget_retour = gtk_vbox_new ( FALSE,
				 5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( widget_retour ),
				   10 );
  gtk_widget_show ( widget_retour );

  bouton_utilise_categ_etat = gtk_check_button_new_with_label ( _("Utiliser les cat�gories dans l'�tat") );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_utilise_categ_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_utilise_categ_etat );

  vbox_generale_categ_etat = gtk_vbox_new ( FALSE,
					    5 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       vbox_generale_categ_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( vbox_generale_categ_etat );

  gtk_signal_connect ( GTK_OBJECT ( bouton_utilise_categ_etat ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       vbox_generale_categ_etat );

  /* mise en place de la frame qui contient la possibilit� de d�tailler les cat�g utilis�es */

  frame = gtk_frame_new ( NULL );
  gtk_box_pack_start ( GTK_BOX ( vbox_generale_categ_etat ),
		       frame,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( frame );

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( vbox ),
				   10 );
  gtk_container_add ( GTK_CONTAINER ( frame ),
		      vbox );
  gtk_widget_show ( vbox );

  bouton_detaille_categ_etat = gtk_check_button_new_with_label ( _("D�tailler les cat�gories utilis�es") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_detaille_categ_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_detaille_categ_etat );


  /* mise en place de la liste et des boutons de d�taillage */

  hbox_detaille_categ_etat = gtk_hbox_new ( FALSE,
					    5 );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       hbox_detaille_categ_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox_detaille_categ_etat );

  gtk_signal_connect ( GTK_OBJECT ( bouton_detaille_categ_etat ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       hbox_detaille_categ_etat );

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( hbox_detaille_categ_etat ),
		       vbox,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox );

  label = gtk_label_new ( _("S�lectionner les cat�gories � inclure dans l'�tat :") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );


  scrolled_window = gtk_scrolled_window_new ( FALSE,
					      FALSE );
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_window ),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       scrolled_window,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( scrolled_window );

  liste_categ_etat = gtk_clist_new ( 1 );
  gtk_clist_set_selection_mode ( GTK_CLIST ( liste_categ_etat ),
				 GTK_SELECTION_MULTIPLE );
  gtk_clist_set_column_auto_resize ( GTK_CLIST ( liste_categ_etat ),
				     0,
				     TRUE );
  gtk_clist_set_compare_func ( GTK_CLIST ( liste_categ_etat ),
			       (GtkCListCompareFunc) classement_alphabetique_tree );
  gtk_container_add ( GTK_CONTAINER ( scrolled_window ),
		      liste_categ_etat );
  gtk_widget_show ( liste_categ_etat );

  /* on va remplir la liste avec les cat�gories */

  pointeur_liste = liste_struct_categories;

  while ( pointeur_liste )
    {
      struct struct_categ *categ;
      gchar *nom[1];
      gint ligne;

      categ = pointeur_liste -> data;

      nom[0] = categ -> nom_categ;

      ligne = gtk_clist_append ( GTK_CLIST ( liste_categ_etat ),
				 nom );

      gtk_clist_set_row_data ( GTK_CLIST ( liste_categ_etat ),
			       ligne,
			       GINT_TO_POINTER ( categ -> no_categ ));

      pointeur_liste = pointeur_liste -> next;
    }

  gtk_clist_sort ( GTK_CLIST ( liste_categ_etat ));

  /*   sur la partie de droite, on met les boutons (d�)s�lectionner tout */

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( hbox_detaille_categ_etat ),
		       vbox,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox );

  bouton = gtk_button_new_with_label ( _("S�lectionner tout") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT  ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( gtk_clist_select_all ),
			      GTK_OBJECT  ( liste_categ_etat ));
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );

  bouton = gtk_button_new_with_label ( _("D�s�lectionner tout") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT  ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( gtk_clist_unselect_all ),
			      GTK_OBJECT  ( liste_categ_etat ));
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );

  bouton = gtk_button_new_with_label ( _("Cat�gories de revenus") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT  ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( click_type_categ_etat ),
			      NULL );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );

  bouton = gtk_button_new_with_label ( _("Cat�gories de d�penses") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT  ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( click_type_categ_etat ),
			      GINT_TO_POINTER (1));
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( vbox_generale_categ_etat ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       vbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( vbox );



  bouton_exclure_ope_sans_categ = gtk_check_button_new_with_label ( _("Exclure les op�rations sans cat�gorie") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_exclure_ope_sans_categ,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_exclure_ope_sans_categ );

  bouton_affiche_sous_total_categ = gtk_check_button_new_with_label ( _("Afficher un sous-total lors du changement de cat�gorie") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_affiche_sous_total_categ,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_affiche_sous_total_categ );

  /* mise en place du bouton pour afficher les sous categ */

  bouton_afficher_sous_categ = gtk_check_button_new_with_label ( _("Afficher les sous-cat�gories") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_afficher_sous_categ,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_sous_categ );

  bouton_affiche_sous_total_sous_categ = gtk_check_button_new_with_label ( _("Afficher un sous-total lors du changement de sous-cat�gorie") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_affiche_sous_total_sous_categ,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_affiche_sous_total_sous_categ );

  bouton_afficher_pas_de_sous_categ = gtk_check_button_new_with_label ( _("Afficher \"Pas de sous-cat�gorie\" si absente") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_afficher_pas_de_sous_categ,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_pas_de_sous_categ );

  gtk_signal_connect ( GTK_OBJECT ( bouton_afficher_sous_categ ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       bouton_affiche_sous_total_sous_categ );
  gtk_signal_connect ( GTK_OBJECT ( bouton_afficher_sous_categ ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       bouton_afficher_pas_de_sous_categ );


  separateur = gtk_vseparator_new ();
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       separateur,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( separateur );

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       vbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( vbox );

  label = gtk_label_new ( _("Devise utilis�e pour le calcul :") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );

  bouton_devise_categ_etat = gtk_option_menu_new ();
  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( bouton_devise_categ_etat ),
			     creation_option_menu_devises ( 0,
							    liste_struct_devises ));
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_devise_categ_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_devise_categ_etat );




  separateur = gtk_hseparator_new ();
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       separateur,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( separateur );

  /*   en dessous, on met les boutons d'inclusion ou non des virements */

  bouton_inclusion_virements_actifs_etat = gtk_radio_button_new_with_label ( NULL,
									     _("Inclure les virements de ou vers les comptes d'actif et de passif") );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_inclusion_virements_actifs_etat,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_inclusion_virements_actifs_etat );

  bouton_inclusion_virements_hors_etat = gtk_radio_button_new_with_label ( gtk_radio_button_group ( GTK_RADIO_BUTTON ( bouton_inclusion_virements_actifs_etat )),
									   _("Inclure les virements de ou vers les comptes ne figurant pas dans l'�tat") );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_inclusion_virements_hors_etat,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_inclusion_virements_hors_etat );

  bouton_non_inclusion_virements = gtk_radio_button_new_with_label ( gtk_radio_button_group ( GTK_RADIO_BUTTON ( bouton_inclusion_virements_actifs_etat )),
								     _("Ne pas inclure les virements") );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_non_inclusion_virements,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_non_inclusion_virements );


  /* on remplit les infos de l'�tat */

  if ( etat -> utilise_categ )
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_utilise_categ_etat ),
				   TRUE );
  else
    gtk_widget_set_sensitive ( vbox_generale_categ_etat,
			       FALSE );

  if ( etat -> utilise_detail_categ )
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_detaille_categ_etat ),
				   TRUE );
  else
    gtk_widget_set_sensitive ( hbox_detaille_categ_etat,
			       FALSE );

  /* on s�lectionne les cat�gories choisies */

  pointeur_liste = etat -> no_categ;

  while ( pointeur_liste )
    {
      gtk_clist_select_row ( GTK_CLIST ( liste_categ_etat ),
			     gtk_clist_find_row_from_data ( GTK_CLIST ( liste_categ_etat ),
							    pointeur_liste -> data ),
			     0 );
      pointeur_liste = pointeur_liste -> next;
    }

  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_exclure_ope_sans_categ ),
				 etat -> exclure_ope_sans_categ );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_affiche_sous_total_categ ),
				 etat -> affiche_sous_total_categ );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_sous_categ ),
				 etat -> afficher_sous_categ );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_affiche_sous_total_sous_categ ),
				 etat -> affiche_sous_total_sous_categ );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_pas_de_sous_categ ),
				 etat -> afficher_pas_de_sous_categ );
  sens_desensitive_pointeur ( bouton_afficher_sous_categ,
			      bouton_affiche_sous_total_sous_categ );
  sens_desensitive_pointeur ( bouton_afficher_sous_categ,
			      bouton_afficher_pas_de_sous_categ );

  if ( etat -> type_virement )
    {
      if ( etat -> type_virement == 1 )
	gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_inclusion_virements_actifs_etat ),
				       TRUE );
      else
	gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_inclusion_virements_hors_etat ),
				       TRUE );
    }
  else
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_non_inclusion_virements ),
				   TRUE );

  /* mise en forme de la devise */

  gtk_option_menu_set_history ( GTK_OPTION_MENU ( bouton_devise_categ_etat ),
				g_slist_position ( liste_struct_devises,
						   g_slist_find_custom ( liste_struct_devises,
									 GINT_TO_POINTER ( etat -> devise_de_calcul_categ ),
									 ( GCompareFunc ) recherche_devise_par_no )));


  return ( widget_retour );
}
/*****************************************************************************************************/




/*****************************************************************************************************/
void click_type_categ_etat ( gint type )
{
  /* type est 0 pour les revenus et 1 pour les d�penses */
  /* fait le tour des cat�g dans la liste et s�lectionne celles */
  /* qui correspondent au type recherch� */

  gint i;

  gtk_clist_unselect_all ( GTK_CLIST ( liste_categ_etat ));

  for ( i=0 ; i<GTK_CLIST ( liste_categ_etat ) -> rows ; i++ )
    {
      struct struct_categ *categ;

      categ = g_slist_find_custom ( liste_struct_categories,
				    gtk_clist_get_row_data ( GTK_CLIST ( liste_categ_etat ),
							     i ),
				    (GCompareFunc) recherche_categorie_par_no ) -> data;

      if ( categ -> type_categ == type )
	gtk_clist_select_row ( GTK_CLIST ( liste_categ_etat ),
			       i,
			       0 );
    }
}
/*****************************************************************************************************/



/*****************************************************************************************************/
GtkWidget *onglet_etat_ib ( struct struct_etat *etat )
{
  GtkWidget *widget_retour;
  GtkWidget *frame;
  GtkWidget *vbox;
  GtkWidget *label;
  GSList *pointeur_liste;
  GtkWidget *scrolled_window;
  GtkWidget *bouton;
  GtkWidget *hbox;
  GtkWidget *separateur;

  widget_retour = gtk_vbox_new ( FALSE,
				 5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( widget_retour ),
				   10 );
  gtk_widget_show ( widget_retour );

  bouton_utilise_ib_etat = gtk_check_button_new_with_label ( _("Utiliser les imputations budg�taires dans l'�tat") );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_utilise_ib_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_utilise_ib_etat );

  vbox_generale_ib_etat = gtk_vbox_new ( FALSE,
					 5 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       vbox_generale_ib_etat,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox_generale_ib_etat );

  gtk_signal_connect ( GTK_OBJECT ( bouton_utilise_ib_etat ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       vbox_generale_ib_etat );

  /* mise en place de la frame qui contient la possibilit� de d�tailler les ib utilis�es */

  frame = gtk_frame_new ( NULL );
  gtk_box_pack_start ( GTK_BOX ( vbox_generale_ib_etat ),
		       frame,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( frame );

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( vbox ),
				   10 );
  gtk_container_add ( GTK_CONTAINER ( frame ),
		      vbox );
  gtk_widget_show ( vbox );

  bouton_detaille_ib_etat = gtk_check_button_new_with_label ( _("D�tailler les imputations budg�taires utilis�es") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_detaille_ib_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_detaille_ib_etat );


  /* mise en place de la liste et des boutons de d�taillage */

  hbox_detaille_ib_etat = gtk_hbox_new ( FALSE,
					 5 );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       hbox_detaille_ib_etat,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( hbox_detaille_ib_etat );

  gtk_signal_connect ( GTK_OBJECT ( bouton_detaille_ib_etat ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       hbox_detaille_ib_etat );

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( hbox_detaille_ib_etat ),
		       vbox,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox );

  label = gtk_label_new ( _("S�lectionner les imputations budg�taires � inclure dans l'�tat :") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );


  scrolled_window = gtk_scrolled_window_new ( FALSE,
					      FALSE );
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_window ),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       scrolled_window,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( scrolled_window );

  liste_ib_etat = gtk_clist_new ( 1 );
  gtk_clist_set_selection_mode ( GTK_CLIST ( liste_ib_etat ),
				 GTK_SELECTION_MULTIPLE );
  gtk_clist_set_column_auto_resize ( GTK_CLIST ( liste_ib_etat ),
				     0,
				     TRUE );
  gtk_clist_set_compare_func ( GTK_CLIST ( liste_ib_etat ),
			       (GtkCListCompareFunc) classement_alphabetique_tree );
  gtk_container_add ( GTK_CONTAINER ( scrolled_window ),
		      liste_ib_etat );
  gtk_widget_show ( liste_ib_etat );

  /* on va remplir la liste avec les ib */

  pointeur_liste = liste_struct_imputation;

  while ( pointeur_liste )
    {
      struct struct_imputation *imputation;
      gchar *nom[1];
      gint ligne;

      imputation = pointeur_liste -> data;

      nom[0] = imputation -> nom_imputation;

      ligne = gtk_clist_append ( GTK_CLIST ( liste_ib_etat ),
				 nom );

      gtk_clist_set_row_data ( GTK_CLIST ( liste_ib_etat ),
			       ligne,
			       GINT_TO_POINTER ( imputation -> no_imputation ));

      pointeur_liste = pointeur_liste -> next;
    }

  gtk_clist_sort ( GTK_CLIST ( liste_ib_etat ));

  /*   sur la partie de droite, on met les boutons (d�)s�lectionner tout */

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( hbox_detaille_ib_etat ),
		       vbox,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox );

  bouton = gtk_button_new_with_label ( _("S�lectionner tout") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT  ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( gtk_clist_select_all ),
			      GTK_OBJECT  ( liste_ib_etat ));
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );

  bouton = gtk_button_new_with_label ( _("D�s�lectionner tout") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT  ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( gtk_clist_unselect_all ),
			      GTK_OBJECT  ( liste_ib_etat ));
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );

  bouton = gtk_button_new_with_label ( _("I.B. de revenus") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT  ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( click_type_ib_etat ),
			      NULL );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );

  bouton = gtk_button_new_with_label ( _("I.B. de d�penses") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT  ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( click_type_ib_etat ),
			      GINT_TO_POINTER (1));
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( vbox_generale_ib_etat ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       vbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( vbox );

  bouton_exclure_ope_sans_ib = gtk_check_button_new_with_label ( _("Exclure les op�rations sans imputation budg�taire") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_exclure_ope_sans_ib,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_exclure_ope_sans_ib );

  bouton_affiche_sous_total_ib = gtk_check_button_new_with_label ( _("Afficher un sous-total lors du changement d'imputation") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_affiche_sous_total_ib,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_affiche_sous_total_ib );

  /* mise en place du bouton pour afficher les sous categ */

  bouton_afficher_sous_ib = gtk_check_button_new_with_label ( _("Afficher les sous-imputations") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_afficher_sous_ib,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_sous_ib );

  bouton_affiche_sous_total_sous_ib = gtk_check_button_new_with_label ( _("Afficher un sous-total lors du changement de sous-imputation") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_affiche_sous_total_sous_ib,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_affiche_sous_total_sous_ib );

  bouton_afficher_pas_de_sous_ib = gtk_check_button_new_with_label ( _("Afficher \"Pas de sous-imputation\" si absente") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_afficher_pas_de_sous_ib,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_afficher_pas_de_sous_ib );

  gtk_signal_connect ( GTK_OBJECT ( bouton_afficher_sous_ib ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       bouton_affiche_sous_total_sous_ib );
  gtk_signal_connect ( GTK_OBJECT ( bouton_afficher_sous_ib ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       bouton_afficher_pas_de_sous_ib );

  separateur = gtk_vseparator_new ();
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       separateur,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( separateur );

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       vbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( vbox );

  label = gtk_label_new ( _("Devise utilis�e pour le calcul :") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );

  bouton_devise_ib_etat = gtk_option_menu_new ();
  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( bouton_devise_ib_etat ),
			     creation_option_menu_devises ( 0,
							    liste_struct_devises ));
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_devise_ib_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_devise_ib_etat );


  /* on remplit les infos de l'�tat */

  if ( etat -> utilise_ib )
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_utilise_ib_etat ),
				   TRUE );
  else
    gtk_widget_set_sensitive ( vbox_generale_ib_etat,
			       FALSE );

  if ( etat -> utilise_detail_ib )
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_detaille_ib_etat ),
				   TRUE );
  else
    gtk_widget_set_sensitive ( hbox_detaille_ib_etat,
			       FALSE );

  /* on s�lectionne les ib choisies */

  pointeur_liste = etat -> no_ib;

  while ( pointeur_liste )
    {
      gtk_clist_select_row ( GTK_CLIST ( liste_ib_etat ),
			     gtk_clist_find_row_from_data ( GTK_CLIST ( liste_ib_etat ),
							    pointeur_liste -> data ),
			     0 );
      pointeur_liste = pointeur_liste -> next;
    }

  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_exclure_ope_sans_ib ),
				 etat -> exclure_ope_sans_ib );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_affiche_sous_total_ib ),
				 etat -> affiche_sous_total_ib );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_sous_ib ),
				 etat -> afficher_sous_ib );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_affiche_sous_total_sous_ib ),
				 etat -> affiche_sous_total_sous_ib );
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_afficher_pas_de_sous_ib ),
				 etat -> afficher_pas_de_sous_ib );
  sens_desensitive_pointeur ( bouton_afficher_sous_ib,
			      bouton_affiche_sous_total_sous_ib );
  sens_desensitive_pointeur ( bouton_afficher_sous_ib,
			      bouton_afficher_pas_de_sous_ib );

  /* mise en forme de la devise */

  gtk_option_menu_set_history ( GTK_OPTION_MENU ( bouton_devise_ib_etat ),
				g_slist_position ( liste_struct_devises,
						   g_slist_find_custom ( liste_struct_devises,
									 GINT_TO_POINTER ( etat -> devise_de_calcul_ib ),
									 ( GCompareFunc ) recherche_devise_par_no )));

  return ( widget_retour );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
void click_type_ib_etat ( gint type )
{
  /* type est 0 pour les revenus et 1 pour les d�penses */
  /* fait le tour des cat�g dans la liste et s�lectionne celles */
  /* qui correspondent au type recherch� */

  gint i;

  gtk_clist_unselect_all ( GTK_CLIST ( liste_ib_etat ));

  for ( i=0 ; i<GTK_CLIST ( liste_ib_etat ) -> rows ; i++ )
    {
      struct struct_imputation *imputation;

      imputation = g_slist_find_custom ( liste_struct_imputation,
					 gtk_clist_get_row_data ( GTK_CLIST ( liste_ib_etat ),
								  i ),
					 (GCompareFunc) recherche_imputation_par_no ) -> data;

      if ( imputation -> type_imputation == type )
	gtk_clist_select_row ( GTK_CLIST ( liste_ib_etat ),
			       i,
			       0 );
    }
}
/*****************************************************************************************************/



/*****************************************************************************************************/
GtkWidget *onglet_etat_tiers ( struct struct_etat *etat )
{
  GtkWidget *widget_retour;

  GtkWidget *frame;
  GtkWidget *vbox;
  GtkWidget *vbox2;
  GtkWidget *label;
  GSList *pointeur_liste;
  GtkWidget *scrolled_window;
  GtkWidget *bouton;
  GtkWidget *hbox;

  widget_retour = gtk_vbox_new ( FALSE,
				 5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( widget_retour ),
				   10 );
  gtk_widget_show ( widget_retour );

  bouton_utilise_tiers_etat = gtk_check_button_new_with_label ( _("Utiliser les tiers dans l'�tat") );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_utilise_tiers_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_utilise_tiers_etat );

  vbox_generale_tiers_etat = gtk_vbox_new ( FALSE,
					    5 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       vbox_generale_tiers_etat,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox_generale_tiers_etat );

  gtk_signal_connect ( GTK_OBJECT ( bouton_utilise_tiers_etat ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       vbox_generale_tiers_etat );

  /* mise en place de la frame qui contient la possibilit� de d�tailler les tiers utilis�es */

  frame = gtk_frame_new ( NULL );
  gtk_box_pack_start ( GTK_BOX ( vbox_generale_tiers_etat ),
		       frame,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( frame );

  vbox = gtk_vbox_new ( FALSE,
			5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( vbox ),
				   10 );
  gtk_container_add ( GTK_CONTAINER ( frame ),
		      vbox );
  gtk_widget_show ( vbox );

  bouton_detaille_tiers_etat = gtk_check_button_new_with_label ( _("D�tailler les tiers utilis�s") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_detaille_tiers_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_detaille_tiers_etat );


  /* mise en place de la liste et des boutons de d�taillage */

  hbox_detaille_tiers_etat = gtk_hbox_new ( FALSE,
					    5 );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       hbox_detaille_tiers_etat,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( hbox_detaille_tiers_etat );

  gtk_signal_connect ( GTK_OBJECT ( bouton_detaille_tiers_etat ),
		       "toggled",
		       GTK_SIGNAL_FUNC ( sens_desensitive_pointeur ),
		       hbox_detaille_tiers_etat );

  vbox2 = gtk_vbox_new ( FALSE,
			 5 );
  gtk_box_pack_start ( GTK_BOX ( hbox_detaille_tiers_etat ),
		       vbox2,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox2 );

  label = gtk_label_new ( _("S�lectionner les tiers � inclure dans l'�tat :") );
  gtk_box_pack_start ( GTK_BOX ( vbox2 ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );


  scrolled_window = gtk_scrolled_window_new ( FALSE,
					      FALSE );
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_window ),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC );
  gtk_box_pack_start ( GTK_BOX ( vbox2 ),
		       scrolled_window,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( scrolled_window );

  liste_tiers_etat = gtk_clist_new ( 1 );
  gtk_clist_set_selection_mode ( GTK_CLIST ( liste_tiers_etat ),
				 GTK_SELECTION_MULTIPLE );
  gtk_clist_set_column_auto_resize ( GTK_CLIST ( liste_tiers_etat ),
				     0,
				     TRUE );
  gtk_clist_set_compare_func ( GTK_CLIST ( liste_tiers_etat ),
			       (GtkCListCompareFunc) classement_alphabetique_tree );
  gtk_container_add ( GTK_CONTAINER ( scrolled_window ),
		      liste_tiers_etat );
  gtk_widget_show ( liste_tiers_etat );

  /* on va remplir la liste avec les tiers */

  pointeur_liste = liste_struct_tiers;

  while ( pointeur_liste )
    {
      struct struct_tiers *tiers;
      gchar *nom[1];
      gint ligne;

      tiers = pointeur_liste -> data;

      nom[0] = tiers -> nom_tiers;

      ligne = gtk_clist_append ( GTK_CLIST ( liste_tiers_etat ),
				 nom );

      gtk_clist_set_row_data ( GTK_CLIST ( liste_tiers_etat ),
			       ligne,
			       GINT_TO_POINTER ( tiers -> no_tiers ));

      pointeur_liste = pointeur_liste -> next;
    }

  gtk_clist_sort ( GTK_CLIST ( liste_tiers_etat ));

  /*   sur la partie de droite, on met les boutons (d�)s�lectionner tout */

  vbox2 = gtk_vbox_new ( FALSE,
			 5 );
  gtk_box_pack_start ( GTK_BOX ( hbox_detaille_tiers_etat ),
		       vbox2,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( vbox2 );

  bouton = gtk_button_new_with_label ( _("S�lectionner tout") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT  ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( gtk_clist_select_all ),
			      GTK_OBJECT  ( liste_tiers_etat ));
  gtk_box_pack_start ( GTK_BOX ( vbox2 ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );

  bouton = gtk_button_new_with_label ( _("D�s�lectionner tout") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect_object ( GTK_OBJECT  ( bouton ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( gtk_clist_unselect_all ),
			      GTK_OBJECT  ( liste_tiers_etat ));
  gtk_box_pack_start ( GTK_BOX ( vbox2 ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );


  bouton_affiche_sous_total_tiers = gtk_check_button_new_with_label ( _("Afficher un sous-total lors du changement de tiers") );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       bouton_affiche_sous_total_tiers,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_affiche_sous_total_tiers );

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( vbox ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  label = gtk_label_new ( _("Devise utilis�e pour le calcul :") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );

  bouton_devise_tiers_etat = gtk_option_menu_new ();
  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( bouton_devise_tiers_etat ),
			     creation_option_menu_devises ( 0,
							    liste_struct_devises ));
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton_devise_tiers_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_devise_tiers_etat );



  /* on remplit les infos de l'�tat */

  if ( etat -> utilise_tiers )
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_utilise_tiers_etat ),
				   TRUE );
  else
    gtk_widget_set_sensitive ( vbox_generale_tiers_etat,
			       FALSE );

  if ( etat -> utilise_detail_tiers )
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_detaille_tiers_etat ),
				   TRUE );
  else
    gtk_widget_set_sensitive ( hbox_detaille_tiers_etat,
			       FALSE );
 
  /* on s�lectionne les tiers choisies */

  pointeur_liste = etat -> no_tiers;

  while ( pointeur_liste )
    {
      gtk_clist_select_row ( GTK_CLIST ( liste_tiers_etat ),
			     gtk_clist_find_row_from_data ( GTK_CLIST ( liste_tiers_etat ),
							    pointeur_liste -> data ),
			     0 );
      pointeur_liste = pointeur_liste -> next;
    }
    
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_affiche_sous_total_tiers ),
				 etat -> affiche_sous_total_tiers );

  /* mise en forme de la devise */

  gtk_option_menu_set_history ( GTK_OPTION_MENU ( bouton_devise_tiers_etat ),
				g_slist_position ( liste_struct_devises,
						   g_slist_find_custom ( liste_struct_devises,
									 GINT_TO_POINTER ( etat -> devise_de_calcul_tiers ),
									 ( GCompareFunc ) recherche_devise_par_no )));

  return ( widget_retour );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
GtkWidget *onglet_etat_texte ( struct struct_etat *etat )
{
  GtkWidget *widget_retour;
  GtkWidget *hbox;
  GtkWidget *label;

  widget_retour = gtk_vbox_new ( FALSE,
				 5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( widget_retour ),
				   10 );
  gtk_widget_show ( widget_retour );

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  label = gtk_label_new ( _("R�duire la recherche aux op�rations contenant ce texte (notes) :") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  entree_texte_etat = gtk_entry_new ();
  gtk_box_pack_end ( GTK_BOX ( hbox ),
		     entree_texte_etat,
		     FALSE,
		     FALSE,
		     0 );
  gtk_widget_show ( entree_texte_etat );

  /*   on remplit l'entr�e */

  if ( etat -> texte )
    gtk_entry_set_text ( GTK_ENTRY ( entree_texte_etat ),
			 g_strstrip ( etat -> texte ));

  return ( widget_retour );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
GtkWidget *onglet_etat_montant ( struct struct_etat *etat )
{
  GtkWidget *widget_retour;
  GtkWidget *hbox;
  GtkWidget *label;

  widget_retour = gtk_vbox_new ( FALSE,
				 5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( widget_retour ),
				   10 );
  gtk_widget_show ( widget_retour );

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  label = gtk_label_new ( _("R�duire la recherche aux op�rations contenant ce montant :") );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );

  hbox = gtk_hbox_new ( FALSE,
			5 );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  entree_montant_etat = gtk_entry_new ();
  gtk_box_pack_end ( GTK_BOX ( hbox ),
		     entree_montant_etat,
		     FALSE,
		     FALSE,
		     0 );
  gtk_widget_show ( entree_montant_etat );

  /* on remplit l'entr�e */

  if ( etat -> montant )
    gtk_entry_set_text ( GTK_ENTRY ( entree_montant_etat ),
			 g_strdup_printf ( "%4.2f",
					   etat -> montant ));

  return ( widget_retour );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
void affichage_etat ( struct struct_etat *etat, 
		      struct struct_etat_affichage * affichage )
{
  GSList *liste_opes_selectionnees;
  gint i;

  if ( !etat )
    {
      if ( etat_courant )
	etat = etat_courant;
      else
	return;
    }

  if ( !affichage )
    {
      affichage = &gtktable_affichage;
    }

  /*   selection des op�rations */
  /* on va mettre l'adresse des op�s s�lectionn�es dans une liste */

  liste_opes_selectionnees = NULL;

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

  for ( i=0 ; i<nb_comptes ; i++ )
    {
      /* on commence par v�rifier que le compte fait partie de l'�tat */

      if ( !etat -> utilise_detail_comptes
	   ||
	   g_slist_index ( etat -> no_comptes,
			   GINT_TO_POINTER ( i )) != -1 )
	{
	  /* 	  le compte est bon, passe � la suite de la s�lection */

	  /* on va faire le tour de toutes les op�s du compte */

	  GSList *pointeur_tmp;

	  pointeur_tmp = LISTE_OPERATIONS;

	  while ( pointeur_tmp )
	    {
	      struct structure_operation *operation;

	      operation = pointeur_tmp -> data;

	      /* si c'est une op� ventil�e, d�pend de la conf */

	      if ( operation -> operation_ventilee
		   &&
		   !etat -> pas_detailler_ventilation )
		goto operation_refusee;

	      if ( operation -> no_operation_ventilee_associee
		   &&
		   etat -> pas_detailler_ventilation )
		goto operation_refusee;



	      /* 	  on va v�rifier si un montant est demand�, c'est le test le plus rapide */
	      /* et le plus limitant */

	      if ( etat -> montant
		   &&
		   operation -> montant != etat -> montant )
		goto operation_refusee;

	      /* on v�rifie ensuite si un texte est recherch� */

	      if ( etat -> texte
		   &&
		   (
		    !operation -> notes
		    ||
		    !strcasestr ( operation -> notes,
				  etat -> texte )))
		goto operation_refusee;


	      /* 	      on v�rifie les virements */

	      if ( operation -> relation_no_operation )
		{
		  if ( !etat -> type_virement )
		    goto operation_refusee;

		  if ( etat -> type_virement == 1 )
		    {
		      /* on inclue l'op� que si le compte de virement */
		      /* est un compte de passif ou d'actif */

		      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

		      /* pour l'instant on n'a que le compte passif */

		      if ( TYPE_DE_COMPTE != 2 )
			goto operation_refusee;

		      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i; 
		    }
		  else
		    {
		      /* on inclut l'op� que si le compte de virement n'est */
		      /* pas pr�sent dans l'�tat */

		      if ( g_slist_index ( etat -> no_comptes,
					   GINT_TO_POINTER ( operation -> relation_no_compte )) != -1 )
			goto operation_refusee;
		    }
		}

	      if ( etat -> utilise_categ )
		{
		  /* on va maintenant v�rifier que les cat�g sont bonnes */
 
		  if ((( etat -> utilise_detail_categ
			 &&
			 g_slist_index ( etat -> no_categ,
					 GINT_TO_POINTER ( operation -> categorie )) == -1 )
		       ||
		       ( etat -> exclure_ope_sans_categ
			 &&
			 !operation -> categorie ))
		      &&
		      !operation -> operation_ventilee )
		    goto operation_refusee;
		}

	      /* v�rification de l'imputation budg�taire */

	      if (( etat -> utilise_ib
		    &&
		    etat -> utilise_detail_ib
		    &&
		    g_slist_index ( etat -> no_ib,
				    GINT_TO_POINTER ( operation -> imputation )) == -1 )
		  ||
		  ( etat -> exclure_ope_sans_ib
		    &&
		    !operation -> imputation ))
		goto operation_refusee;

	      /* v�rification du tiers */

	      if ( etat -> utilise_tiers
		   &&
		   etat -> utilise_detail_tiers
		   &&
		   g_slist_index ( etat -> no_tiers,
				   GINT_TO_POINTER ( operation -> tiers )) == -1 )
		goto operation_refusee;


	      /* v�rifie la plage de date */

	      if ( etat -> exo_date )
		{
		  /* on utilise l'exercice */

		  if ( etat -> utilise_detail_exo
		       &&
		       g_slist_index ( etat -> no_exercices,
				       GINT_TO_POINTER ( operation -> no_exercice )) == -1 ||
		       operation -> no_exercice == 0)
		    goto operation_refusee;
		}
	      else
		{
		  /* on utilise une plage de dates */

		  GDate *date_jour;
		  GDate *date_tmp;

		  date_jour = g_date_new ();
		  g_date_set_time ( date_jour,
				    time ( NULL ));


		  switch ( etat -> no_plage_date )
		    {
		    case 0:
		      /* toutes */

		      break;

		    case 1:
		      /* plage perso */

		      if ( !etat -> date_perso_debut
			   ||
			   !etat -> date_perso_fin
			   ||
			   g_date_compare ( etat -> date_perso_debut,
					    operation -> date ) > 0
			   ||
			   g_date_compare ( etat -> date_perso_fin,
					    operation -> date ) < 0 )
			goto operation_refusee;
		      break;

		    case 2:
		      /* cumul � ce jour */

		      if ( g_date_compare ( date_jour,
					    operation -> date ) > 0 )
			goto operation_refusee;
		      break;

		    case 3:
		      /* mois en cours */

		      if ( g_date_month ( date_jour ) != g_date_month ( operation -> date ))
			goto operation_refusee;
		      break;

		    case 4:
		      /* ann�e en cours */

		      if ( g_date_year ( date_jour ) != g_date_year ( operation -> date ))
			goto operation_refusee;
		      break;

		    case 5:
		      /* cumul mensuel */

		      if ( g_date_month ( date_jour ) != g_date_month ( operation -> date )
			   ||
			   g_date_compare ( date_jour,
					    operation -> date ) > 0 )
			goto operation_refusee;
		      break;

		    case 6:
		      /* cumul annuel */

		      if ( g_date_year ( date_jour ) != g_date_year ( operation -> date )
			   ||
			   g_date_compare ( date_jour,
					    operation -> date ) > 0 )
			goto operation_refusee;
		      break;

		    case 7:
		      /* mois pr�c�dent */

		      g_date_subtract_months ( date_jour,
					       1 );

		      if ( g_date_month ( date_jour ) != g_date_month ( operation -> date ))
			goto operation_refusee;
		      break;

		    case 8:
		      /* ann�e pr�c�dente */

		      g_date_subtract_years ( date_jour,
					      1 );

		      if ( g_date_year ( date_jour ) != g_date_year ( operation -> date ))
			goto operation_refusee;
		      break;

		    case 9:
		      /* 30 derniers jours */

		      date_tmp = g_date_new_dmy ( g_date_day ( date_jour ),
						  g_date_month ( date_jour ),
						  g_date_year ( date_jour ) );
		      g_date_subtract_days ( date_tmp,
					     30 );
		      if ( g_date_compare ( date_tmp,
					    operation -> date ) > 0
			   ||
			   g_date_compare ( date_jour,
					    operation -> date ) < 0 )
			goto operation_refusee;
		      break;

		    case 10:
		      /* 3 derniers mois */

		      date_tmp = g_date_new_dmy ( g_date_day ( date_jour ),
						  g_date_month ( date_jour ),
						  g_date_year ( date_jour ) );
		      g_date_subtract_months ( date_tmp,
					       3 );
		      g_date_subtract_months ( date_jour,
					       1 );
		      if ( g_date_compare ( date_tmp,
					    operation -> date ) > 0
			   ||
			   g_date_compare ( date_jour,
					    operation -> date ) < 0 )
			goto operation_refusee;
		      break;

		    case 11:
		      /* 6 derniers mois */

		      date_tmp = g_date_new_dmy ( g_date_day ( date_jour ),
						  g_date_month ( date_jour ),
						  g_date_year ( date_jour ) );
		      g_date_subtract_months ( date_tmp,
					       6 );
		      g_date_subtract_months ( date_jour,
					       1 );
		      if ( g_date_compare ( date_tmp,
					    operation -> date ) > 0
			   ||
			   g_date_compare ( date_jour,
					    operation -> date ) < 0 )
			goto operation_refusee;
		      break;

		    case 12:
		      /* 12 derniers mois */

		      date_tmp = g_date_new_dmy ( g_date_day ( date_jour ),
						  g_date_month ( date_jour ),
						  g_date_year ( date_jour ) );
		      g_date_subtract_months ( date_tmp,
					       12 );
		      g_date_subtract_months ( date_jour,
					       1 );
		      if ( g_date_compare ( date_tmp,
					    operation -> date ) > 0
			   ||
			   g_date_compare ( date_jour,
					    operation -> date ) < 0 )
			goto operation_refusee;
		      break;

		    case 13:
		      /* 30 prochains jours */

		      date_tmp = g_date_new_dmy ( g_date_day ( date_jour ),
						  g_date_month ( date_jour ),
						  g_date_year ( date_jour ) );
		      g_date_add_days ( date_tmp,
					30 );
		      if ( g_date_compare ( date_tmp,
					    operation -> date ) < 0
			   ||
			   g_date_compare ( date_jour,
					    operation -> date ) > 0 )
			goto operation_refusee;
		      break;

		    case 14:
		      /* 3 prochains mois */

		      date_tmp = g_date_new_dmy ( g_date_day ( date_jour ),
						  g_date_month ( date_jour ),
						  g_date_year ( date_jour ) );
		      g_date_add_months ( date_tmp,
					  3 );
		      g_date_add_months ( date_jour,
					  1 );
		      if ( g_date_compare ( date_tmp,
					    operation -> date ) < 0
			   ||
			   g_date_compare ( date_jour,
					    operation -> date ) > 0 )
			goto operation_refusee;
		      break;

		    case 15:
		      /* 6 prochains mois */

		      date_tmp = g_date_new_dmy ( g_date_day ( date_jour ),
						  g_date_month ( date_jour ),
						  g_date_year ( date_jour ) );
		      g_date_add_months ( date_tmp,
					  6 );
		      g_date_add_months ( date_jour,
					  1 );
		      if ( g_date_compare ( date_tmp,
					    operation -> date ) < 0
			   ||
			   g_date_compare ( date_jour,
					    operation -> date ) > 0 )
			goto operation_refusee;
		      break;

		    case 16:
		      /* 12 prochains mois */

		      date_tmp = g_date_new_dmy ( g_date_day ( date_jour ),
						  g_date_month ( date_jour ),
						  g_date_year ( date_jour ) );
		      g_date_add_months ( date_tmp,
					  12 );
		      g_date_add_months ( date_jour,
					  1 );
		      if ( g_date_compare ( date_tmp,
					    operation -> date ) < 0
			   ||
			   g_date_compare ( date_jour,
					    operation -> date ) > 0 )
			goto operation_refusee;
		    }
		}


	      liste_opes_selectionnees = g_slist_append ( liste_opes_selectionnees,
							  operation );

	    operation_refusee:
	      pointeur_tmp = pointeur_tmp -> next;
	    }
	}
      p_tab_nom_de_compte_variable++;
    }

  /*   � ce niveau, on a r�cup�r� toutes les op�s qui entreront dans */
  /* l'�tat ; reste plus qu'� les classer et les afficher */


  /* on classe la liste et l'affiche en fonction du choix du type de classement */

  etape_finale_affichage_etat ( liste_opes_selectionnees, affichage );

}
/*****************************************************************************************************/


/*****************************************************************************************************/
/* Fonction d'impression de l'�tat */
/*****************************************************************************************************/
void impression_etat ( struct struct_etat *etat )
{
  affichage_etat ( etat, &gnomeprint_affichage );
}


/*****************************************************************************************************/
/* Fonction de rafraichissement de l'�tat */
/*****************************************************************************************************/
void rafraichissement_etat ( struct struct_etat *etat )
{
  affichage_etat ( etat, &gtktable_affichage );
}

/*****************************************************************************************************/
/* Fonction de classement de la liste */
/* en fonction du choix du type de classement */
/*****************************************************************************************************/

gint classement_liste_opes_etat ( struct structure_operation *operation_1,
				  struct structure_operation *operation_2 )
{
  GList *pointeur;

  pointeur = etat_courant -> type_classement;

 classement_suivant:

  /*   si pointeur est nul, on a fait le tour du classement, les op�s sont identiques */
  /* on classe par date, et si pareil, par no d'op� */

  if ( !pointeur )
    {
      if ( g_date_compare ( operation_1 -> date,
			    operation_2 -> date ))
	return ( g_date_compare ( operation_1 -> date,
				  operation_2 -> date ));

      /*       les dates sont identiques, on classe par no d'op� */

      return ( operation_1 -> no_operation - operation_2 -> no_operation );
    }


  switch ( GPOINTER_TO_INT ( pointeur -> data ))
    {
      /* classement des cat�gories */

    case 1:

      if ( operation_1 -> categorie != operation_2 -> categorie )
	return ( operation_1 -> categorie - operation_2 -> categorie );

      /*     si  les cat�gories sont nulles, on doit d�partager entre virements, pas */
      /* de categ ou op� ventil�e */
      /* on met en 1er les op�s sans categ, ensuite les ventilations et enfin les virements */

      if ( !operation_1 -> categorie )
	{
	  if ( operation_1 -> operation_ventilee )
	    {
	      if ( operation_2 -> relation_no_operation )
		return ( -1 );
	      else
		if ( !operation_2 -> operation_ventilee )
		  return ( 1 );
	    }
	  else
	    {
	      if ( operation_1 -> relation_no_operation )
		{
		  if ( !operation_2 -> relation_no_operation )
		    return ( 1 );
		}
	      else
		if ( operation_2 -> relation_no_operation
		     ||
		     operation_2 -> operation_ventilee )
		  return ( -1 );
	    }
	}

      /*       les cat�gories sont identiques, passe au classement suivant */

      pointeur = pointeur -> next;
      goto classement_suivant;

      break;

      /* classement des sous cat�gories */

    case 2:

      if ( operation_1 -> sous_categorie != operation_2 -> sous_categorie )
	return ( operation_1 -> sous_categorie - operation_2 -> sous_categorie );

      /*       les ss-cat�gories sont identiques, passe au classement suivant */

      pointeur = pointeur -> next;
      goto classement_suivant;

      break;

      /* classement des ib */

    case 3:

      if ( operation_1 -> imputation != operation_2 -> imputation )
	return ( operation_1 -> imputation - operation_2 -> imputation );

      /*       les ib sont identiques, passe au classement suivant */

      pointeur = pointeur -> next;
      goto classement_suivant;

      break;

      /* classement des sous ib */

    case 4:

      if ( operation_1 -> sous_imputation != operation_2 -> sous_imputation )
	return ( operation_1 -> sous_imputation - operation_2 -> sous_imputation );

      /*       les ib sont identiques, passe au classement suivant */

      pointeur = pointeur -> next;
      goto classement_suivant;

      break;


      /* classement des comptes */

    case 5:

      if ( operation_1 -> no_compte != operation_2 -> no_compte )
	return ( operation_1 ->no_compte  - operation_2 -> no_compte );

      /*       les comptes sont identiques, passe au classement suivant */

      pointeur = pointeur -> next;
      goto classement_suivant;
      break;


      /* classement des tiers */

    case 6:

      if ( operation_1 -> tiers != operation_2 -> tiers )
	return ( operation_1 ->tiers  - operation_2 -> tiers );

      /*       les tiers sont identiques, passe au classement suivant */

      pointeur = pointeur -> next;
      goto classement_suivant;
      break;
    }
  return (0);
}
/*****************************************************************************************************/


/*****************************************************************************************************/
void etape_finale_affichage_etat ( GSList *ope_selectionnees,
				   struct struct_etat_affichage *affichage)
{
  GSList *liste_ope_revenus;
  GSList *liste_ope_depenses;
  GSList *pointeur_tmp;
  gint i;
  gint ligne;
  gdouble total_partie;
  gdouble total_general;
  gchar *decalage_base;
  gchar *decalage_categ;
  gchar *decalage_sous_categ;
  gchar *decalage_ib;
  gchar *decalage_sous_ib;
  gchar *decalage_compte;
  gchar *decalage_tiers;
  GList *pointeur_glist;


  /* on commence par s�parer la liste revenus et de d�penses */
  /*   si le classement racine est la cat�gorie, on s�pare par cat�gorie */
  /* de revenu ou de d�pense */
  /* si c'est un autre, on s�pare par montant positif ou n�gatif */

  pointeur_glist = etat_courant -> type_classement;

  liste_ope_revenus = NULL;
  liste_ope_depenses = NULL;
  pointeur_tmp = ope_selectionnees;

  while ( pointeur_tmp )
    {
      struct structure_operation *operation;

      operation = pointeur_tmp -> data;

      if ( GPOINTER_TO_INT ( pointeur_glist -> data ) == 1 )
	{
	  /* le classement racine est la cat�gorie */
	  /* s'il n'y a pas de cat�g, c'est un virement ou une ventilation */
	  /*       dans ce cas, on classe en fonction du montant */

	  if ( operation -> categorie )
	    {
	      struct struct_categ *categ;

	      categ = g_slist_find_custom ( liste_struct_categories,
					    GINT_TO_POINTER ( operation -> categorie ),
					    (GCompareFunc) recherche_categorie_par_no ) -> data;

	      if ( categ -> type_categ )
		liste_ope_depenses = g_slist_append ( liste_ope_depenses,
						      operation );
	      else
		liste_ope_revenus = g_slist_append ( liste_ope_revenus,
						     operation );
	    }
	  else
	    {
	      if ( operation -> montant < 0 )
		liste_ope_depenses = g_slist_append ( liste_ope_depenses,
						      operation );
	      else
		liste_ope_revenus = g_slist_append ( liste_ope_revenus,
						     operation );
	    }
	}
      else
	{
	  /* le classement racine n'est pas la cat�g, on s�pare en fonction du montant */

	  if ( operation -> montant < 0 )
	    liste_ope_depenses = g_slist_append ( liste_ope_depenses,
						  operation );
	  else
	    liste_ope_revenus = g_slist_append ( liste_ope_revenus,
						 operation );
	}
      pointeur_tmp = pointeur_tmp -> next;
    }

  /* on va maintenant classer ces 2 listes dans l'ordre ad�quat */

  liste_ope_depenses = g_slist_sort ( liste_ope_depenses,
				      (GCompareFunc) classement_liste_opes_etat );
  liste_ope_revenus = g_slist_sort ( liste_ope_revenus,
				     (GCompareFunc) classement_liste_opes_etat );

  
  /* calcul du d�calage pour chaque classement */
  /* c'est une chaine vide qu'on ajoute devant le nom du */
  /*   classement ( tiers, ib ...) */
 
  /* on met 2 espaces par d�calage */
  /*   normalement, pointeur_glist est d�j� positionn� */

  decalage_base = "";

  /*   pour �viter le warning lors de la compilation, on met */
  /* toutes les var char � "" */

  decalage_categ = "";
  decalage_sous_categ = "";
  decalage_ib = "";
  decalage_sous_ib = "";
  decalage_compte = "";
  decalage_tiers = "";

  while ( pointeur_glist )
    {
      switch ( GPOINTER_TO_INT ( pointeur_glist -> data ))
	{
	  /* d�calage de la cat�gorie */

	case 1:
	  if ( etat_courant -> utilise_categ )
	    decalage_categ = g_strconcat ( decalage_base,
					   "    ",
					   NULL );
	  else
	    {
	      decalage_categ = decalage_base;
	      goto pas_decalage;
	    }
	  break;

	  /* d�calage de la ss-cat�gorie */

	case 2:
	  if ( etat_courant -> utilise_categ
	       &&
	       etat_courant -> afficher_sous_categ )
	    decalage_sous_categ = g_strconcat ( decalage_base,
						"    ",
						NULL );
	  else
	    {
	      decalage_sous_categ = decalage_base;
	      goto pas_decalage;
	    }
	  break;

	  /* d�calage de l'ib */

	case 3:
	  if ( etat_courant -> utilise_ib )
	    decalage_ib = g_strconcat ( decalage_base,
					"    ",
					NULL );
	  else
	    {
	      decalage_ib = decalage_base;
	      goto pas_decalage;
	    }
	  break;

	  /* d�calage de la ss-ib */

	case 4:
	  if ( etat_courant -> utilise_ib
	       &&
	       etat_courant -> afficher_sous_ib )
	    decalage_sous_ib = g_strconcat ( decalage_base,
					     "    ",
					     NULL );
	  else
	    {
	      decalage_sous_ib = decalage_base;
	      goto pas_decalage;
	    }
	  break;

	  /* d�calage du compte */

	case 5:
	  if ( etat_courant -> regroupe_ope_par_compte )
	    decalage_compte = g_strconcat ( decalage_base,
					    "    ",
					    NULL );
	  else
	    {
	      decalage_compte = decalage_base;
	      goto pas_decalage;
	    }
	  break;

	  /* d�calage du tiers */

	case 6:
	  if ( etat_courant -> utilise_tiers )
	    decalage_tiers = g_strconcat ( decalage_base,
					   "    ",
					   NULL );
	  else
	    {
	      decalage_tiers = decalage_base;
	      goto pas_decalage;
	    }
	  break;
	}

      decalage_base = g_strconcat ( decalage_base,
				    "    ",
				    NULL );

    pas_decalage:
      pointeur_glist = pointeur_glist -> next;
    }



  /*   calcul du nb de colonnes : */
  /* 1�re pour les titres de structure */
  /* la derni�re pour les montants */
  /* et entre les 2 pour l'affichage des op�s -> variable */

  nb_colonnes = 0;

  if ( etat_courant -> afficher_opes )
    {
      nb_colonnes = nb_colonnes + etat_courant -> afficher_date_ope;
      nb_colonnes = nb_colonnes + etat_courant -> afficher_tiers_ope;
      nb_colonnes = nb_colonnes + etat_courant -> afficher_categ_ope;
/*       nb_colonnes = nb_colonnes + etat_courant -> afficher_sous_categ_ope; */
      nb_colonnes = nb_colonnes + etat_courant -> afficher_ib_ope;
/*       nb_colonnes = nb_colonnes + etat_courant -> afficher_sous_ib_ope; */
      nb_colonnes = nb_colonnes + etat_courant -> afficher_notes_ope;
      nb_colonnes = nb_colonnes + etat_courant -> afficher_pc_ope;
      nb_colonnes = nb_colonnes + etat_courant -> afficher_infobd_ope;
      nb_colonnes = nb_colonnes + etat_courant -> afficher_no_ope;
      nb_colonnes = nb_colonnes + etat_courant -> afficher_type_ope;
      nb_colonnes = nb_colonnes + etat_courant -> afficher_cheque_ope;
      nb_colonnes = nb_colonnes + etat_courant -> afficher_rappr_ope;
      nb_colonnes = nb_colonnes + etat_courant -> afficher_exo_ope;

      /* on ajoute les s�parations */

      nb_colonnes = 2 * nb_colonnes;
    }

  nb_colonnes = nb_colonnes + 2;
  ligne_debut_partie = -1;

  nom_categ_en_cours = NULL;
  nom_ss_categ_en_cours = NULL;
  nom_ib_en_cours = NULL;
  nom_ss_ib_en_cours = NULL;
  nom_compte_en_cours = NULL;
  nom_tiers_en_cours = NULL;

  if (! affichage -> init ())
    {
      return;
    }

  /* on commence � remplir le tableau */

  /* on met le titre */

  total_general = 0;
  ligne = affichage -> afficher_titre ( 0 );

  /* s�paration */
  ligne = affichage -> afficher_separateur (ligne);

  /*   si n�cessaire, on met les titres des colonnes */

  if ( etat_courant -> afficher_opes
       &&
       etat_courant -> afficher_titre_colonnes
       &&
       !etat_courant -> type_affichage_titres )
    ligne = affichage -> affiche_titres_colonnes ( ligne );

  for ( i=0 ; i<2 ; i++ )
    {
      ancienne_categ_etat = -1;
      ancienne_categ_speciale_etat = 0;
      ancienne_sous_categ_etat = -1;
      ancienne_ib_etat = -1;
      ancienne_sous_ib_etat = -1;
      ancien_compte_etat = -1;
      ancien_tiers_etat = -1;

      montant_categ_etat = 0;
      montant_sous_categ_etat = 0;
      montant_ib_etat = 0;
      montant_sous_ib_etat = 0;
      montant_compte_etat = 0;
      montant_tiers_etat = 0;
      total_partie = 0;

      changement_de_groupe_etat = 0;
      debut_affichage_etat = 1;
      devise_compte_en_cours_etat = NULL;

      /*       on met directement les adr des devises de categ, ib et tiers en global pour */
      /* gagner de la vitesse */

      devise_categ_etat = g_slist_find_custom ( liste_struct_devises,
						GINT_TO_POINTER ( etat_courant -> devise_de_calcul_categ ),
						( GCompareFunc ) recherche_devise_par_no) -> data;

      devise_ib_etat = g_slist_find_custom ( liste_struct_devises,
						GINT_TO_POINTER ( etat_courant -> devise_de_calcul_ib ),
						( GCompareFunc ) recherche_devise_par_no) -> data;

      devise_tiers_etat = g_slist_find_custom ( liste_struct_devises,
						GINT_TO_POINTER ( etat_courant -> devise_de_calcul_tiers ),
						( GCompareFunc ) recherche_devise_par_no) -> data;

      devise_generale_etat = g_slist_find_custom ( liste_struct_devises,
						   GINT_TO_POINTER ( etat_courant -> devise_de_calcul_general ),
						   ( GCompareFunc ) recherche_devise_par_no) -> data;

      /* on met ici le pointeur sur les revenus ou sur les d�penses */
      /* en v�rifiant qu'il y en a */

      if ( !i )
	{
	  /* on met le pointeur sur les revenus */

	  if ( liste_ope_revenus )
	    {
	      pointeur_tmp = liste_ope_revenus;

	      ligne = affichage -> affiche_titre_revenus_etat ( ligne );
	    }
	  else
	    {
	      /* il n'y a pas de revenus, on saute directement aux d�penses */

	      i++;
	      pointeur_tmp = liste_ope_depenses;

	      ligne = affichage -> affiche_titre_depenses_etat ( ligne );
	    }
	}
      else
	{
	  /* on met le pointeur sur les d�penses */

	  if ( liste_ope_depenses )
	    {
	      /* s�paration */
	      ligne = affichage -> afficher_separateur (ligne);
	      pointeur_tmp = liste_ope_depenses;

	      ligne = affichage -> affiche_titre_depenses_etat ( ligne );
	    }
	  else
	    goto fin_boucle_affichage_etat;
	}


      /* on commence la boucle qui fait le tour de chaque op� */

      while ( pointeur_tmp )
	{
	  struct structure_operation *operation;
	  struct struct_devise *devise_operation;
	  gdouble montant;

	  operation = pointeur_tmp -> data;

	  pointeur_glist = etat_courant -> type_classement;

	  while ( pointeur_glist )
	    {
	      switch ( GPOINTER_TO_INT ( pointeur_glist -> data ))
		{
		case 1:
		  ligne = affichage -> affiche_categ_etat ( operation,
					       					       decalage_categ,
					       ligne );
		  break;

		case 2:
		  ligne = affichage -> affiche_sous_categ_etat ( operation,
						    						    decalage_sous_categ,
						    ligne );

		  break;

		case 3:
		  ligne = affichage -> affiche_ib_etat ( operation,
					    					    decalage_ib,
					    ligne );

		  break;

		case 4:
		  ligne = affichage -> affiche_sous_ib_etat ( operation,
						 						 decalage_sous_ib,
						 ligne );

		  break;

		case 5:
		  ligne = affichage -> affiche_compte_etat ( operation,
												decalage_compte,
						ligne );

		  break;

		case 6:
		  ligne = affichage -> affiche_tiers_etat ( operation,
					       					       decalage_tiers,
					       ligne );
		}

	      pointeur_glist = pointeur_glist -> next;
	    }


	  ligne = affichage -> affichage_ligne_ope ( operation,
						     ligne );

	  /* on ajoute les montants que pour ceux affich�s */

	  /* calcule le montant de la categ */

	  if ( etat_courant -> utilise_categ )
	    {
	      if ( operation -> devise == devise_categ_etat -> no_devise )
		montant = operation -> montant;
	      else
		{
		  devise_operation = g_slist_find_custom ( liste_struct_devises,
							   GINT_TO_POINTER ( operation -> devise ),
							   ( GCompareFunc ) recherche_devise_par_no ) -> data;

		  if ( devise_categ_etat -> passage_euro
		       &&
		       !strcmp ( devise_operation -> nom_devise, _("Euro") ) )
		    montant = operation -> montant * devise_categ_etat -> change;
		  else
		    if ( devise_operation -> passage_euro
			 &&
			 !strcmp ( devise_categ_etat -> nom_devise, _("Euro") ))
		      montant = operation -> montant / devise_operation -> change;
		    else
		      if ( operation -> une_devise_compte_egale_x_devise_ope )
			montant = operation -> montant / operation -> taux_change - operation -> frais_change;
		      else
			montant = operation -> montant * operation -> taux_change - operation -> frais_change;

		  montant = ( rint (montant * 100 )) / 100;
		}
	      montant_categ_etat = montant_categ_etat + montant;
	      montant_sous_categ_etat = montant_sous_categ_etat + montant;
	    }

	  /* calcule le montant de l'ib */
	  
	  if ( etat_courant -> utilise_ib )
	    {
	      if ( operation -> devise == devise_ib_etat -> no_devise )
		montant = operation -> montant;
	      else
		{
		  devise_operation = g_slist_find_custom ( liste_struct_devises,
							   GINT_TO_POINTER ( operation -> devise ),
							   ( GCompareFunc ) recherche_devise_par_no ) -> data;

		  if ( devise_ib_etat -> passage_euro
		       &&
		       !strcmp ( devise_operation -> nom_devise, _("Euro") ) )
		    montant = operation -> montant * devise_ib_etat -> change;
		  else
		    if ( devise_operation -> passage_euro
			 &&
			 !strcmp ( devise_ib_etat -> nom_devise, _("Euro") ))
		      montant = operation -> montant / devise_operation -> change;
		    else
		      if ( operation -> une_devise_compte_egale_x_devise_ope )
			montant = operation -> montant / operation -> taux_change - operation -> frais_change;
		      else
			montant = operation -> montant * operation -> taux_change - operation -> frais_change;

		  montant = ( rint (montant * 100 )) / 100;
		}
	      montant_ib_etat = montant_ib_etat + montant;
	      montant_sous_ib_etat = montant_sous_ib_etat + montant;
	    }

	  /* calcule le montant du tiers */

	  if ( etat_courant -> utilise_tiers )
	    {
	      if ( operation -> devise == devise_tiers_etat -> no_devise )
		montant = operation -> montant;
	      else
		{
		  devise_operation = g_slist_find_custom ( liste_struct_devises,
							   GINT_TO_POINTER ( operation -> devise ),
							   ( GCompareFunc ) recherche_devise_par_no ) -> data;

		  if ( devise_tiers_etat -> passage_euro
		       &&
		       !strcmp ( devise_operation -> nom_devise, _("Euro") ) )
		    montant = operation -> montant * devise_tiers_etat -> change;
		  else
		    if ( devise_operation -> passage_euro
			 &&
			 !strcmp ( devise_tiers_etat -> nom_devise, _("Euro") ))
		      montant = operation -> montant / devise_operation -> change;
		    else
		      if ( operation -> une_devise_compte_egale_x_devise_ope )
			montant = operation -> montant / operation -> taux_change - operation -> frais_change;
		      else
			montant = operation -> montant * operation -> taux_change - operation -> frais_change;

		  montant = ( rint (montant * 100 )) / 100;
		}
	      montant_tiers_etat = montant_tiers_etat + montant;
	    }

	  /* calcule le montant du compte */

	  if ( etat_courant -> affiche_sous_total_compte )
	    {
	      /* on modifie le montant s'il n'est pas de la devise du compte en cours */

	      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

	      if ( !devise_compte_en_cours_etat
		   ||
		   DEVISE != devise_compte_en_cours_etat -> no_devise )
		devise_compte_en_cours_etat = g_slist_find_custom ( liste_struct_devises,
								    GINT_TO_POINTER ( DEVISE ),
								    ( GCompareFunc ) recherche_devise_par_no) -> data;

	      if ( operation -> devise == DEVISE )
		montant = operation -> montant;
	      else
		{
		  /* ce n'est pas la devise du compte, si le compte passe � l'euro et que la devise est l'euro, */
		  /* utilise la conversion du compte, */
		  /* si c'est une devise qui passe � l'euro et que la devise du compte est l'euro, utilise la conversion du compte */
		  /* sinon utilise la conversion stock�e dans l'op� */

		  devise_operation = g_slist_find_custom ( liste_struct_devises,
							   GINT_TO_POINTER ( operation -> devise ),
							   ( GCompareFunc ) recherche_devise_par_no ) -> data;

		  if ( devise_compte_en_cours_etat -> passage_euro
		       &&
		       !strcmp ( devise_operation -> nom_devise, _("Euro") ) )
		    montant = operation -> montant * devise_compte_en_cours_etat -> change;
		  else
		    if ( devise_operation -> passage_euro
			 &&
			 !strcmp ( devise_compte_en_cours_etat -> nom_devise, _("Euro") ))
		      montant = operation -> montant / devise_operation -> change;
		    else
		      if ( operation -> une_devise_compte_egale_x_devise_ope )
			montant = operation -> montant / operation -> taux_change - operation -> frais_change;
		      else
			montant = operation -> montant * operation -> taux_change - operation -> frais_change;

		  montant = ( rint (montant * 100 )) / 100;
		}
	      montant_compte_etat = montant_compte_etat + montant;
	    }

	  /* calcule les montants totaux */

	  if ( operation -> devise == devise_generale_etat -> no_devise )
	    montant = operation -> montant;
	  else
	    {
	      devise_operation = g_slist_find_custom ( liste_struct_devises,
						       GINT_TO_POINTER ( operation -> devise ),
						       ( GCompareFunc ) recherche_devise_par_no ) -> data;

	      if ( devise_generale_etat -> passage_euro
		   &&
		   !strcmp ( devise_operation -> nom_devise, _("Euro") ) )
		montant = operation -> montant * devise_generale_etat -> change;
	      else
		if ( devise_operation -> passage_euro
		     &&
		     !strcmp ( devise_generale_etat -> nom_devise, _("Euro") ))
		  montant = operation -> montant / devise_operation -> change;
		else
		  if ( operation -> une_devise_compte_egale_x_devise_ope )
		    montant = operation -> montant / operation -> taux_change - operation -> frais_change;
		  else
		    montant = operation -> montant * operation -> taux_change - operation -> frais_change;

	      montant = ( rint (montant * 100 )) / 100;
	    }


	  total_partie = total_partie + montant;
	  total_general = total_general + montant;

	  changement_de_groupe_etat = 0;

	  pointeur_tmp = pointeur_tmp -> next;
	}

      /*   � la fin, on affiche les totaux des derni�res lignes */

      ligne = affichage -> affiche_totaux_sous_jaccent ( GPOINTER_TO_INT ( etat_courant -> type_classement -> data ),
					    ligne );

					    
      /* on ajoute le total de la structure racine */


       switch ( GPOINTER_TO_INT ( etat_courant -> type_classement -> data ))
	{
	case 1:
	  ligne = affichage -> affiche_total_categories ( ligne );
	  break;

	case 2:
	  ligne = affichage -> affiche_total_sous_categ ( ligne );
	  break;

	case 3:
	  ligne = affichage -> affiche_total_ib ( ligne );
	  break;

	case 4:
	  ligne = affichage -> affiche_total_sous_ib ( ligne );
	  break;

	case 5:
	  ligne = affichage -> affiche_total_compte ( ligne );
	  break;

	case 6:
	  ligne = affichage -> affiche_total_tiers ( ligne );
	  break;
	}

      /* on affiche le total de la partie en cours */

      ligne = affichage -> affiche_total_partiel ( total_partie,
				      ligne,
				      i );

    fin_boucle_affichage_etat:
    }

  /* on affiche maintenant le total g�n�ral */

  ligne = affichage -> affiche_total_general ( total_general,
				  ligne );

  affichage -> finish ();
}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* met tous les anciens_x_etat sous jaccents � l'origine � -1 */
/*****************************************************************************************************/

void denote_struct_sous_jaccentes ( gint origine )
{
  GList *pointeur_glist;

  /* on peut partir du bout de la liste pour revenir vers la structure demand�e */
  /* gros vulgaire copier coller de la fonction pr�c�dente */

  pointeur_glist = g_list_last ( etat_courant -> type_classement );


  while ( GPOINTER_TO_INT ( pointeur_glist -> data ) != origine )
    {
      switch ( GPOINTER_TO_INT ( pointeur_glist -> data ))
	{
	case 1:
	  ancienne_categ_etat = -1;
	  ancienne_categ_speciale_etat = 0;
	  break;

	case 2:
	  ancienne_sous_categ_etat = -1;
	  break;

	case 3:
	  ancienne_ib_etat = -1;
	  break;

	case 4:
	  ancienne_sous_ib_etat = -1;
	  break;

	case 5:
	  ancien_compte_etat = -1;
	  break;

	case 6:
	  ancien_tiers_etat = -1;
	  break;
	}
      pointeur_glist = pointeur_glist -> prev;
    }
}
/*****************************************************************************************************/
