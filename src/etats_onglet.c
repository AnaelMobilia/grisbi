/*  Fichier qui s'occupe de l'onglet �tats */
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

  /* on met le bouton imprimer */

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

  /* on met le bouton exporter */

  bouton_exporter_etat = gtk_button_new_with_label ( _("Exporter ...") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton_exporter_etat ),
			  GTK_RELIEF_NONE );
  gtk_widget_set_sensitive ( bouton_exporter_etat,
			     FALSE );
  gtk_signal_connect ( GTK_OBJECT ( bouton_exporter_etat ),
		       "clicked",
		       GTK_SIGNAL_FUNC ( exporter_etat ),
		       NULL );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_exporter_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_exporter_etat );

  /* on met le bouton importer */

  bouton_importer_etat = gtk_button_new_with_label ( _("Importer ...") );
  gtk_button_set_relief ( GTK_BUTTON ( bouton_importer_etat ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect ( GTK_OBJECT ( bouton_importer_etat ),
		       "clicked",
		       GTK_SIGNAL_FUNC ( importer_etat ),
		       NULL );
  gtk_box_pack_start ( GTK_BOX ( widget_retour ),
		       bouton_importer_etat,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton_importer_etat );

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
	  gtk_widget_set_sensitive ( bouton_exporter_etat,
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
  gtk_widget_set_sensitive ( bouton_exporter_etat,
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
void exporter_etat ( void )
{
  GtkWidget *dialog;
  GtkWidget *label;
  GtkWidget *fenetre_nom;
  gint resultat;
  struct stat test_fichier;
  gchar *nom_etat;

  dialog = gnome_dialog_new ( _("Exporter un �tat"),
			      GNOME_STOCK_BUTTON_OK,
			      GNOME_STOCK_BUTTON_CANCEL,
			      NULL );
  gtk_window_set_transient_for ( GTK_WINDOW ( dialog ),
				 GTK_WINDOW ( window ));
  gnome_dialog_set_default ( GNOME_DIALOG ( dialog ),
			     0 );
  gtk_signal_connect ( GTK_OBJECT ( dialog ),
		       "destroy",
		       GTK_SIGNAL_FUNC ( gtk_signal_emit_stop_by_name ),
		       "destroy" );

  label = gtk_label_new ( _("Entrer un nom pour l'export :") );
  gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );

  fenetre_nom = gnome_file_entry_new ( "nom_fichier",
				       "nom_fichier" );
  gnome_file_entry_set_default_path ( GNOME_FILE_ENTRY ( fenetre_nom ),
				      dernier_chemin_de_travail );
  gtk_entry_set_text ( GTK_ENTRY ( gnome_file_entry_gtk_entry ( GNOME_FILE_ENTRY ( fenetre_nom ))),
		       g_strconcat ( dernier_chemin_de_travail,
				     etat_courant -> nom_etat,
				     ".egsb",
				     NULL ));
  gtk_entry_set_position ( GTK_ENTRY ( gnome_file_entry_gtk_entry ( GNOME_FILE_ENTRY ( fenetre_nom ))),
			   strlen (dernier_chemin_de_travail ));
  gnome_dialog_editable_enters ( GNOME_DIALOG ( dialog ),
				 GTK_EDITABLE ( gnome_file_entry_gtk_entry ( GNOME_FILE_ENTRY ( fenetre_nom ))));
  gtk_window_set_focus ( GTK_WINDOW ( dialog ),
			 gnome_file_entry_gtk_entry ( GNOME_FILE_ENTRY ( fenetre_nom )));
  gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
		       fenetre_nom,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( fenetre_nom );

  resultat = gnome_dialog_run ( GNOME_DIALOG ( dialog ));

  switch ( resultat )
    {
    case 0 :
      nom_etat = g_strdup ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( gnome_file_entry_gtk_entry ( GNOME_FILE_ENTRY ( fenetre_nom ))))));

      gnome_dialog_close ( GNOME_DIALOG ( dialog ));

      /* v�rification que c'est possible */

      if ( !strlen ( nom_etat ))
	return;

      if ( stat ( nom_etat,
		  &test_fichier ) != -1 )
	{
	  if ( S_ISREG ( test_fichier.st_mode ) )
	    {
	      GtkWidget *etes_vous_sur;
	      GtkWidget *label;

	      etes_vous_sur = gnome_dialog_new ( _("Enregistrer le fichier"),
						 GNOME_STOCK_BUTTON_YES,
						 GNOME_STOCK_BUTTON_NO,
						 NULL );
	      label = gtk_label_new ( _("Le fichier existe. Voulez-vous l'�craser ?") );
	      gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( etes_vous_sur ) -> vbox ),
				   label,
				   TRUE,
				   TRUE,
				   5 );
	      gtk_widget_show ( label );

	      gnome_dialog_set_default ( GNOME_DIALOG ( etes_vous_sur ),
					 1 );
	      gnome_dialog_set_parent ( GNOME_DIALOG ( etes_vous_sur ),
					GTK_WINDOW ( window ) );
	      gtk_window_set_modal ( GTK_WINDOW ( etes_vous_sur ),
				     TRUE );
	      if ( gnome_dialog_run_and_close ( GNOME_DIALOG ( etes_vous_sur ) ) )
		return;
	    }
	  else
	    {
	      dialogue ( g_strconcat ( _("Nom de fichier \""),
				       nom_etat,
				       _("\" invalide !"),
				       NULL ));
	      return;
	    }
	}

      if ( !enregistre_etat ( nom_etat ))
	{
	  dialogue ( "L'enregistrement a �chou�." );
	  return;
	}

      break;

    default :
      gnome_dialog_close ( GNOME_DIALOG ( dialog ));
      return;
    }
}
/*****************************************************************************************************/


/*****************************************************************************************************/
void importer_etat ( void )
{
  GtkWidget *dialog;
  GtkWidget *label;
  GtkWidget *fenetre_nom;
  gint resultat;
  gchar *nom_etat;

  dialog = gnome_dialog_new ( _("Importer un �tat"),
			      GNOME_STOCK_BUTTON_OK,
			      GNOME_STOCK_BUTTON_CANCEL,
			      NULL );
  gtk_window_set_transient_for ( GTK_WINDOW ( dialog ),
				 GTK_WINDOW ( window ));
  gnome_dialog_set_default ( GNOME_DIALOG ( dialog ),
			     0 );
  gtk_signal_connect ( GTK_OBJECT ( dialog ),
		       "destroy",
		       GTK_SIGNAL_FUNC ( gtk_signal_emit_stop_by_name ),
		       "destroy" );

  label = gtk_label_new ( _("Entrer le nom du fichier :") );
  gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );

  fenetre_nom = gnome_file_entry_new ( "nom_fichier",
				       "nom_fichier" );
  gnome_file_entry_set_default_path ( GNOME_FILE_ENTRY ( fenetre_nom ),
				      dernier_chemin_de_travail );
  gtk_entry_set_text ( GTK_ENTRY ( gnome_file_entry_gtk_entry ( GNOME_FILE_ENTRY ( fenetre_nom ))),
		       g_strconcat ( dernier_chemin_de_travail,
				     ".egsb",
				     NULL ));
  gtk_entry_set_position ( GTK_ENTRY ( gnome_file_entry_gtk_entry ( GNOME_FILE_ENTRY ( fenetre_nom ))),
			   strlen (dernier_chemin_de_travail ));
  gnome_dialog_editable_enters ( GNOME_DIALOG ( dialog ),
				 GTK_EDITABLE ( gnome_file_entry_gtk_entry ( GNOME_FILE_ENTRY ( fenetre_nom ))));
  gtk_window_set_focus ( GTK_WINDOW ( dialog ),
			 gnome_file_entry_gtk_entry ( GNOME_FILE_ENTRY ( fenetre_nom )));
  gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
		       fenetre_nom,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( fenetre_nom );

  resultat = gnome_dialog_run ( GNOME_DIALOG ( dialog ));

  switch ( resultat )
    {
    case 0 :
      nom_etat = g_strdup ( g_strstrip ( gtk_entry_get_text ( GTK_ENTRY ( gnome_file_entry_gtk_entry ( GNOME_FILE_ENTRY ( fenetre_nom ))))));

      gnome_dialog_close ( GNOME_DIALOG ( dialog ));

      /* v�rification que c'est possible */

      if ( !strlen ( nom_etat ))
	return;


      if ( !charge_etat ( nom_etat ))
	{
	  dialogue ( "L'importation a �chou�." );
	  return;
	}

      break;

    default :
      gnome_dialog_close ( GNOME_DIALOG ( dialog ));
      return;
    }
}
/*****************************************************************************************************/
