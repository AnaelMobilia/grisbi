/* Fichier banque.c */
/* s'occupe de tout ce qui concerne les banques */


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
#include "categories_onglet.h"
#include "devises.h"
#include "erreur.h"
#include "exercice.h"
#include "fichiers_io.h"
#include "gtkcombofix.h"
#include "imputation_budgetaire.h"
#include "operations_formulaire.h"
#include "operations_liste.h"
#include "type_operations.h"
#include "ventilation.h"
#include "operations_classement.h"



/*******************************************************************************************/
/* Fonction creation_fenetre_ventilation */
/* cr�e la fenetre qui contient la liste des ventilation */
/*******************************************************************************************/

GtkWidget *creation_fenetre_ventilation ( void )
{
  GtkWidget *onglet;
  gchar *titres[] ={ _("Category"),
		     _("Notes"),
		     _("Amount") };

  /* cr�ation de la scrolled window  */

  onglet = gtk_scrolled_window_new ( NULL,
				     NULL);
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( onglet ),
				   GTK_POLICY_NEVER,
				   GTK_POLICY_AUTOMATIC);
  gtk_widget_show ( onglet );


  /* cr�ation de la liste */

  liste_operations_ventilees = gtk_clist_new_with_titles ( 3,
				       titres );
  gtk_widget_set_usize ( GTK_WIDGET ( liste_operations_ventilees ),
			 1,
			 FALSE );

  /*   par d�faut, le classement de la liste s'effectue par no des op�rations */

  gtk_clist_set_compare_func ( GTK_CLIST ( liste_operations_ventilees ),
			       (GtkCListCompareFunc) classement_liste_par_no_ope );

  gtk_signal_connect ( GTK_OBJECT ( liste_operations_ventilees ),
		       "size-allocate",
		       GTK_SIGNAL_FUNC ( changement_taille_liste_ventilation ),
		       NULL );
  gtk_container_add ( GTK_CONTAINER ( onglet ),
		      liste_operations_ventilees );
  gtk_widget_show ( liste_operations_ventilees );
  

  /* On annule la fonction bouton des titres */

  gtk_clist_column_titles_passive ( GTK_CLIST ( liste_operations_ventilees ));

  gtk_clist_set_column_resizeable ( GTK_CLIST ( liste_operations_ventilees ),
				    0,
				    FALSE );
  gtk_clist_set_column_resizeable ( GTK_CLIST ( liste_operations_ventilees ),
				    1,
				    FALSE );
  gtk_clist_set_column_resizeable ( GTK_CLIST ( liste_operations_ventilees ),
				    2,
				    FALSE );


  /* justification du contenu des cellules */


  gtk_clist_set_column_justification ( GTK_CLIST ( liste_operations_ventilees ),
				       0,
				       GTK_JUSTIFY_LEFT);
  gtk_clist_set_column_justification ( GTK_CLIST ( liste_operations_ventilees ),
				       1,
				       GTK_JUSTIFY_LEFT);
  gtk_clist_set_column_justification ( GTK_CLIST ( liste_operations_ventilees ),
				       2,
				       GTK_JUSTIFY_RIGHT);


  /* v�rifie le simple ou double click */

  gtk_signal_connect ( GTK_OBJECT ( liste_operations_ventilees ),
		       "button_press_event",
		       GTK_SIGNAL_FUNC ( selectionne_ligne_souris_ventilation ),
		       NULL );


  /*   v�rifie la touche entr�e, haut et bas */

  gtk_signal_connect ( GTK_OBJECT ( liste_operations_ventilees ),
		       "key_press_event",
		       GTK_SIGNAL_FUNC ( traitement_clavier_liste_ventilation ),
		       NULL );



  return ( onglet );
}
/*******************************************************************************************/



/*******************************************************************************************/
/* Fonction  creation_verification_ventilation*/
/* cr�e la fenetre � la place de la liste des comptes qui contient les boutons et l'�tat de la ventilation */
/*******************************************************************************************/

GtkWidget *creation_verification_ventilation ( void )
{
  GtkWidget *onglet;
  GtkWidget *label;
  GtkWidget *frame;
  GtkWidget *tableau;
  GtkWidget *separateur;
  GtkWidget *hbox;
  GtkWidget *bouton;


  /* cr�ation de la vbox */

  onglet = gtk_vbox_new ( FALSE,
			  10 );
  gtk_container_set_border_width ( GTK_CONTAINER ( onglet ),
				   10 );
  gtk_widget_show ( onglet );


/* cr�ation du titre "op�ration ventil�e" */

  frame = gtk_frame_new ( NULL );
  gtk_box_pack_start ( GTK_BOX ( onglet ),
		       frame,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( frame );

  label = gtk_label_new ( _("Breakdown of transaction") );
  gtk_container_add ( GTK_CONTAINER ( frame ),
		      label );
  gtk_widget_show ( label );


  /* cr�ation du tableau */

  tableau = gtk_table_new ( 4,
			    2,
			    FALSE );
  gtk_table_set_row_spacings ( GTK_TABLE ( tableau ),
			       10 );
  gtk_table_set_col_spacings ( GTK_TABLE ( tableau ),
			       10 );
  gtk_box_pack_start ( GTK_BOX ( onglet ),
		       tableau,
		       FALSE,
		       FALSE,
		       20 );
  gtk_widget_show ( tableau );


  label = gtk_label_new ( COLON(_("Break down amount")) );
  gtk_misc_set_alignment ( GTK_MISC ( label ),
			   0,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( tableau ),
		     label,
		     0, 1,
		     0, 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  label_somme_ventilee = gtk_label_new ( "" );
  gtk_misc_set_alignment ( GTK_MISC ( label_somme_ventilee ),
			   1,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( tableau ),
		     label_somme_ventilee,
		     1, 2,
		     0, 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label_somme_ventilee );


  label = gtk_label_new ( COLON(_("Not assigned")) );
  gtk_misc_set_alignment ( GTK_MISC ( label ),
			   0,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( tableau ),
		     label,
		     0, 1,
		     1, 2,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  label_non_affecte = gtk_label_new ( "" );
  gtk_misc_set_alignment ( GTK_MISC ( label_non_affecte ),
			   1,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( tableau ),
		     label_non_affecte,
		     1, 2,
		     1, 2,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label_non_affecte );


  separateur = gtk_hseparator_new ();
  gtk_table_attach ( GTK_TABLE ( tableau ),
		     separateur,
		     0, 2,
		     2, 3,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( separateur );



  label = gtk_label_new ( COLON(_("Amount")) );
  gtk_misc_set_alignment ( GTK_MISC ( label ),
			   0,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( tableau ),
		     label,
		     0, 1,
		     3, 4,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  label_montant_operation_ventilee = gtk_label_new ( "" );
  gtk_misc_set_alignment ( GTK_MISC ( label_montant_operation_ventilee ),
			   1,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( tableau ),
		     label_montant_operation_ventilee,
		     1, 2,
		     3, 4,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label_montant_operation_ventilee );



/* mise en place des boutons */

  hbox = gtk_hbox_new ( FALSE,
			10 );
  gtk_box_pack_end ( GTK_BOX ( onglet ),
		       hbox,
		       FALSE,
		       FALSE,
		       10 );
  gtk_widget_show ( hbox );


  /* FIXME */
/*   bouton = gnome_stock_button ( GNOME_STOCK_BUTTON_OK ); */
  bouton = gtk_button_new_from_stock (GTK_STOCK_OK);

  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect ( GTK_OBJECT ( bouton ),
		       "clicked",
		       GTK_SIGNAL_FUNC ( valider_ventilation ),
		       NULL );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );

  /* FIXME */
/*   bouton = gnome_stock_button ( GNOME_STOCK_BUTTON_CANCEL ); */
  bouton = gtk_button_new_from_stock    (GTK_STOCK_CANCEL);
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect ( GTK_OBJECT ( bouton ),
		       "clicked",
		       GTK_SIGNAL_FUNC ( annuler_ventilation ),
		       NULL );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       bouton,
		       TRUE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );


  separateur = gtk_hseparator_new ();
  gtk_box_pack_end ( GTK_BOX ( onglet ),
		       separateur,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( separateur );

  return ( onglet );
}
/*******************************************************************************************/


/*******************************************************************************************/
/* Fonction creation_formulaire_ventilation */
/* cr�e la fenetre qui contient e formulaire pour la ventilation */
/*******************************************************************************************/

GtkWidget *creation_formulaire_ventilation ( void )
{
  GtkWidget *onglet;
  GtkTooltips *tips;
  GtkWidget *table;
  GtkWidget *bouton;
  GtkWidget *menu;

  /* on cr�e le tooltips */

  tips = gtk_tooltips_new ();

  /* cr�ation du formulaire */

  onglet = gtk_vbox_new ( FALSE,
			  5 );
  gtk_container_set_border_width ( GTK_CONTAINER ( onglet ),
				   10);
  gtk_widget_show ( onglet );

  /* mise en place de la table */

  table = gtk_table_new ( 2,
			  5,
			  FALSE);
  gtk_table_set_col_spacings ( GTK_TABLE ( table ),
			       10 );
  gtk_box_pack_start ( GTK_BOX ( onglet ),
		       table,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( table );

  /* mise en place des cat�gories */

  widget_formulaire_ventilation[0] = gtk_combofix_new_complex ( liste_categories_echeances_combofix,
								FALSE,
								TRUE,
								TRUE,
								0 );
  gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_ventilation[0] ) -> entry ),
 		       "key_press_event",
		       GTK_SIGNAL_FUNC ( appui_touche_ventilation ),
		       NULL );
  gtk_signal_connect ( GTK_OBJECT (GTK_COMBOFIX (widget_formulaire_ventilation[0]) -> entry),
 		       "button_press_event",
		       GTK_SIGNAL_FUNC (clique_champ_formulaire_ventilation),
		       NULL );
  gtk_signal_connect ( GTK_OBJECT (GTK_COMBOFIX (widget_formulaire_ventilation[0]) -> arrow),
 		       "button_press_event",
		       GTK_SIGNAL_FUNC (clique_champ_formulaire_ventilation),
		       NULL );
  gtk_signal_connect_object ( GTK_OBJECT ( GTK_COMBOFIX (widget_formulaire_ventilation[0]) -> entry ),
			      "focus_in_event",
			      GTK_SIGNAL_FUNC (entree_prend_focus),
			      GTK_OBJECT ( widget_formulaire_ventilation[0] ) );
  gtk_signal_connect ( GTK_OBJECT (GTK_COMBOFIX (widget_formulaire_ventilation[0]) -> entry),
		       "focus_out_event",
		       GTK_SIGNAL_FUNC (entree_ventilation_perd_focus),
		       GINT_TO_POINTER (0) );
  gtk_table_attach ( GTK_TABLE (table),
		     widget_formulaire_ventilation[0],
		     0, 1, 0,1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0,0);
  gtk_widget_show ( widget_formulaire_ventilation[0] );

  /* mise en place des notes */

  widget_formulaire_ventilation[1] = gtk_entry_new ();
  gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_ventilation[1] ),
 		       "key_press_event",
		       GTK_SIGNAL_FUNC ( appui_touche_ventilation ),
		       GINT_TO_POINTER ( 1 ) );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[1]),
 		       "button_press_event",
		       GTK_SIGNAL_FUNC (clique_champ_formulaire_ventilation ),
		       NULL );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[1]),
		       "focus_in_event",
		       GTK_SIGNAL_FUNC (entree_prend_focus),
		       NULL );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[1]),
		       "focus_out_event",
		       GTK_SIGNAL_FUNC (entree_ventilation_perd_focus),
		       GINT_TO_POINTER (1) );
  gtk_table_attach ( GTK_TABLE (table),
		     widget_formulaire_ventilation[1],
		     1, 3, 0,1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0,0);
  gtk_widget_show ( widget_formulaire_ventilation[1] );



  /* mise en place du d�bit */

  widget_formulaire_ventilation[2] = gtk_entry_new ();
  gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_ventilation[2] ),
 		       "key_press_event",
		       GTK_SIGNAL_FUNC ( appui_touche_ventilation ),
		       GINT_TO_POINTER ( 2 ) );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[2]),
 		       "button_press_event",
		       GTK_SIGNAL_FUNC (clique_champ_formulaire_ventilation ),
		       NULL );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[2]),
		       "focus_in_event",
		       GTK_SIGNAL_FUNC (entree_prend_focus),
		       NULL );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[2]),
		       "focus_out_event",
		       GTK_SIGNAL_FUNC (entree_ventilation_perd_focus),
		       GINT_TO_POINTER (2) );
  gtk_table_attach ( GTK_TABLE (table),
		     widget_formulaire_ventilation[2],
		     3, 4, 0,1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0,0);
  gtk_widget_show ( widget_formulaire_ventilation[2] );


  /* mise en place du cr�dit */

  widget_formulaire_ventilation[3] = gtk_entry_new ();
  gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_ventilation[3] ),
 		       "key_press_event",
		       GTK_SIGNAL_FUNC ( appui_touche_ventilation ),
		       GINT_TO_POINTER ( 3 ) );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[3]),
 		       "button_press_event",
		       GTK_SIGNAL_FUNC (clique_champ_formulaire_ventilation ),
		       NULL );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[3]),
		       "focus_in_event",
		       GTK_SIGNAL_FUNC (entree_prend_focus),
		       NULL );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[3]),
		       "focus_out_event",
		       GTK_SIGNAL_FUNC (entree_ventilation_perd_focus),
		       GINT_TO_POINTER (3) );
  gtk_table_attach ( GTK_TABLE (table),
		     widget_formulaire_ventilation[3],
		     4, 5, 0,1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0,0);
  gtk_widget_show ( widget_formulaire_ventilation[3] );


  /*  Affiche l'imputation budg�taire */

  widget_formulaire_ventilation[4] = gtk_combofix_new_complex ( liste_imputations_combofix,
								FALSE,
								TRUE,
								TRUE,
								0 );
  gtk_table_attach ( GTK_TABLE (table),
		     widget_formulaire_ventilation[4],
		     0, 1, 1, 2,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0,0);
  gtk_signal_connect ( GTK_OBJECT (GTK_COMBOFIX (widget_formulaire_ventilation[4]) -> entry),
 		       "button_press_event",
		       GTK_SIGNAL_FUNC (clique_champ_formulaire_ventilation),
		       NULL );
  gtk_signal_connect ( GTK_OBJECT (GTK_COMBOFIX (widget_formulaire_ventilation[4]) -> arrow),
 		       "button_press_event",
		       GTK_SIGNAL_FUNC (clique_champ_formulaire_ventilation),
		       NULL );
  gtk_signal_connect ( GTK_OBJECT (GTK_COMBOFIX (widget_formulaire_ventilation[4]) -> entry),
 		       "key_press_event",
		       GTK_SIGNAL_FUNC (appui_touche_ventilation),
		       GINT_TO_POINTER(4) );
  gtk_signal_connect_object ( GTK_OBJECT ( GTK_COMBOFIX (widget_formulaire_ventilation[4]) -> entry ),
			      "focus_in_event",
			      GTK_SIGNAL_FUNC (entree_prend_focus),
			      GTK_OBJECT ( widget_formulaire_ventilation[4] ) );
  gtk_signal_connect ( GTK_OBJECT (GTK_COMBOFIX (widget_formulaire_ventilation[4]) -> entry),
		       "focus_out_event",
		       GTK_SIGNAL_FUNC (entree_ventilation_perd_focus),
		       GINT_TO_POINTER (4) );

  gtk_widget_show (widget_formulaire_ventilation[4]);

  gtk_widget_set_sensitive ( widget_formulaire_ventilation[4],
			     etat.utilise_imputation_budgetaire );


  /* mise en place du type de l'op� associ�e en cas de virement */
  /* non affich� au d�part */
  
  widget_formulaire_ventilation[5] = gtk_option_menu_new ();
  gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			 widget_formulaire_ventilation[5],
			 _("Associated method of payment"),
			 _("Associated method of payment") );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[5]),
 		       "key_press_event",
		       GTK_SIGNAL_FUNC ( appui_touche_ventilation ),
		       GINT_TO_POINTER(5) );
  gtk_table_attach ( GTK_TABLE ( table ),
		     widget_formulaire_ventilation[5],
		     1, 2, 1, 2,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0,0);

  /* cr�ation du bouton de l'exo */

  widget_formulaire_ventilation[6] = gtk_option_menu_new ();
  gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			 widget_formulaire_ventilation[6],
			 _("Choose the financial year"),
			 _("Choose the financial year") );
  menu = gtk_menu_new ();
  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_ventilation[6] ),
			     creation_menu_exercices (0) );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[6]),
 		       "key_press_event",
		       GTK_SIGNAL_FUNC (appui_touche_ventilation),
		       GINT_TO_POINTER(6) );
  gtk_table_attach ( GTK_TABLE ( table ),
		     widget_formulaire_ventilation[6],
		     2, 3, 1, 2,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0,0);
  gtk_widget_show ( widget_formulaire_ventilation[6] );

  gtk_widget_set_sensitive ( widget_formulaire_ventilation[6],
			     etat.utilise_exercice );

  /*   cr�ation de l'entr�e du no de pi�ce comptable */

  widget_formulaire_ventilation[7] = gtk_entry_new();
  gtk_table_attach ( GTK_TABLE (table),
		     widget_formulaire_ventilation[7],
		     3, 5, 1, 2,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0,0);
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[7]),
 		       "button_press_event",
		       GTK_SIGNAL_FUNC (clique_champ_formulaire_ventilation ),
		       NULL );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[7]),
 		       "key_press_event",
		       GTK_SIGNAL_FUNC (appui_touche_ventilation),
		       GINT_TO_POINTER(7) );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[7]),
		       "focus_in_event",
		       GTK_SIGNAL_FUNC (entree_prend_focus),
		       NULL );
  gtk_signal_connect ( GTK_OBJECT (widget_formulaire_ventilation[7]),
		       "focus_out_event",
		       GTK_SIGNAL_FUNC (entree_ventilation_perd_focus),
		       GINT_TO_POINTER (7) );
  gtk_widget_show ( widget_formulaire_ventilation[7] );

  gtk_widget_set_sensitive ( widget_formulaire_ventilation[7],
			     etat.utilise_piece_comptable );

  /* s�paration d'avec les boutons */

  separateur_formulaire_ventilations = gtk_hseparator_new ();
  gtk_box_pack_start ( GTK_BOX ( onglet ),
		       separateur_formulaire_ventilations,
		       FALSE,
		       FALSE,
		       0 );
  if ( etat.affiche_boutons_valider_annuler )
    gtk_widget_show ( separateur_formulaire_ventilations );

  /* mise en place des boutons */

  hbox_valider_annuler_ventil = gtk_hbox_new ( FALSE,
					       5 );
  gtk_box_pack_start ( GTK_BOX ( onglet ),
		       hbox_valider_annuler_ventil,
		       FALSE,
		       FALSE,
		       0 );
  if ( etat.affiche_boutons_valider_annuler )
    gtk_widget_show ( hbox_valider_annuler_ventil );

  /* FIXME */
/*   bouton = gnome_stock_button ( GNOME_STOCK_BUTTON_CANCEL ); */
  bouton = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect ( GTK_OBJECT ( bouton ),
		       "clicked",
		       GTK_SIGNAL_FUNC ( echap_formulaire_ventilation ),
		       NULL );
  gtk_box_pack_end ( GTK_BOX ( hbox_valider_annuler_ventil ),
		     bouton,
		     FALSE,
		     FALSE,
		     0 );
  gtk_widget_show ( bouton );

  /* FIXME */
/*   bouton = gnome_stock_button ( GNOME_STOCK_BUTTON_OK ); */
  bouton = gtk_button_new_from_stock (GTK_STOCK_OK);
  gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			  GTK_RELIEF_NONE );
  gtk_signal_connect ( GTK_OBJECT ( bouton ),
		       "clicked",
		       GTK_SIGNAL_FUNC ( fin_edition_ventilation ),
		       NULL );
  gtk_box_pack_end ( GTK_BOX ( hbox_valider_annuler_ventil ),
		     bouton,
		     FALSE,
		     FALSE,
		     0 );
  gtk_widget_show ( bouton );


  /*   met l'adr de l'op� dans le formulaire � -1 */

  gtk_object_set_data ( GTK_OBJECT ( widget_formulaire_ventilation[0] ),
			"adr_struct_ope",
			GINT_TO_POINTER ( -1 ) );

  return ( onglet );
}
/*******************************************************************************************/





/***********************************************************************************************************/
gboolean clique_champ_formulaire_ventilation ( void )
{

  /* on rend sensitif tout ce qui ne l'�tait pas sur le formulaire */

  gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_ventilation[5] ),
			     TRUE );
  gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_ventilation[6] ),
			     TRUE );
  gtk_widget_set_sensitive ( GTK_WIDGET ( hbox_valider_annuler_ventil ),
			     TRUE );

  return FALSE;
}
/***********************************************************************************************************/




/***********************************************************************************************************/
/* Fonction appel�e quand une entry perd le focus */
/* si elle ne contient rien, on remet la fonction en gris */
/***********************************************************************************************************/

gboolean entree_ventilation_perd_focus ( GtkWidget *entree, GdkEventFocus *ev,
					 gint *no_origine )
{
  gchar *texte;

  texte = NULL;

  switch ( GPOINTER_TO_INT ( no_origine ))
    {
      /* on sort des cat�gories */
    case 0:
      if ( strlen ( g_strstrip ( (char *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
	{
	  /* si c'est un virement, on met le menu des types de l'autre compte */
	  /* si ce menu n'est pas d�j� affich� */

	  gchar **tableau_char;

	  tableau_char = g_strsplit ( (char *) gtk_entry_get_text ( GTK_ENTRY ( entree )),
				      ":",
				      2 );

	  tableau_char[0] = g_strstrip ( tableau_char[0] );

	  if ( tableau_char[1] )
	    tableau_char[1] = g_strstrip ( tableau_char[1] );


	  if ( strlen ( tableau_char[0] ) )
	    {
	      if ( !strcmp ( tableau_char[0],
			     _("Transfer") )
		   && tableau_char[1]
		   && strlen ( tableau_char[1]) )
		{
		  /* c'est un virement : on recherche le compte associ� et on affiche les types de paiement */

		  gint i;

		  if ( strcmp ( tableau_char[1],
				_("Deleted account") ) )
		    {
		      /* recherche le no de compte du virement */

		      gint compte_virement;

		      p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

		      compte_virement = -1;

		      for ( i = 0 ; i < nb_comptes ; i++ )
			{
			  if ( !g_strcasecmp ( NOM_DU_COMPTE,
					       tableau_char[1] ) )
			    compte_virement = i;
			  p_tab_nom_de_compte_variable++;
			}

		      /* si on a touv� un compte de virement, que celui ci n'est pas le compte */
		      /* courant et que son menu des types n'est pas encore affich�, on cr�e le menu */

		      if ( compte_virement != -1
			   &&
			   compte_virement != compte_courant )
			{
			  /* si le menu affich� est d�j� celui du compte de virement, on n'y touche pas */

			  if ( !GTK_WIDGET_VISIBLE ( widget_formulaire_ventilation[5] )
			       ||
			       ( GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_ventilation[5] ) -> menu ),
									 "no_compte" ))
				 !=
				 compte_virement ))
			    {
			      /* v�rifie quel est le montant entr�, affiche les types oppos�s de l'autre compte */

			      GtkWidget *menu;

			      if ( gtk_widget_get_style ( widget_formulaire_ventilation[4] ) == style_entree_formulaire[0] )
				/* il y a un montant dans le cr�dit */
				menu = creation_menu_types ( 1, compte_virement, 2  );
			      else
				/* il y a un montant dans le d�bit ou d�faut */
				menu = creation_menu_types ( 2, compte_virement, 2  );

			      /* si un menu � �t� cr��, on l'affiche */

			      if ( menu )
				{
				  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_ventilation[5] ),
							     menu );
				  gtk_widget_show ( widget_formulaire_ventilation[5] );
				}
				  
			      /* on associe le no de compte de virement au formulaire pour le retrouver */
			      /* rapidement s'il y a un chgt d�bit/cr�dit */

			      gtk_object_set_data ( GTK_OBJECT ( widget_formulaire_ventilation[5] ),
						    "compte_virement",
						    GINT_TO_POINTER ( compte_virement ));
			    }
			}
		      else
			gtk_widget_hide ( widget_formulaire_ventilation[5] );
		    }
		  else
		    gtk_widget_hide ( widget_formulaire_ventilation[5] );
		}
	      else
		gtk_widget_hide ( widget_formulaire_ventilation[5] );
	    }
	  else
	    gtk_widget_hide ( widget_formulaire_ventilation[5] );

	  g_strfreev ( tableau_char );
	}
      else
	texte = _("Categories : Sub-categories");
      break;

      /* sort des notes */

    case 1:
      if ( !strlen ( g_strstrip ( (char *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
	texte = _("Notes");
      break;

      /* sort du d�bit */
     /*   soit vide, soit change le menu des types s'il ne correspond pas */

    case 2:

      if ( strlen ( g_strstrip ( (char *) gtk_entry_get_text ( GTK_ENTRY ( entree ))))
	   &&
	   gtk_widget_get_style ( widget_formulaire_ventilation[2] ) == style_entree_formulaire[0] )
	{
	  /* on  commence par virer ce qu'il y avait dans les cr�dits */

	  if ( gtk_widget_get_style ( widget_formulaire_ventilation[3] ) == style_entree_formulaire[0] )
	    {
	      gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_ventilation[3] ),
				   "" );
	      gtk_widget_set_style ( widget_formulaire_ventilation[3],
				     style_entree_formulaire[1] );
	      gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_ventilation[3]),
				   _("Credit") );
	    }

	  /* comme il y a eu un changement de signe, on change aussi le type de l'op� associ�e */
	  /* s'il est affich� */

	  if ( GTK_WIDGET_VISIBLE ( widget_formulaire_ventilation[5] )
	       &&
	       GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_ventilation[5] ) -> menu ),
						       "signe_menu" ))
	       ==
	       1 )
	    {
	      GtkWidget *menu;

	      menu = creation_menu_types ( 2,
					   GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( widget_formulaire_ventilation[5] ),
										   "compte_virement" )),
					   2  );

	      if ( menu )
		gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_ventilation[5] ),
					   menu );
	      else
		gtk_option_menu_remove_menu ( GTK_OPTION_MENU ( widget_formulaire_ventilation[5] ));
	    }
	}
      else
	texte = _("Debit");
      break;

      /* sort du cr�dit */

    case 3:
      if ( strlen ( g_strstrip ( (char *) gtk_entry_get_text ( GTK_ENTRY ( entree ))))
	   &&
	   gtk_widget_get_style ( widget_formulaire_ventilation[3] ) == style_entree_formulaire[0])
	{
	  /* on  commence par virer ce qu'il y avait dans les d�bits */

	  if ( gtk_widget_get_style ( widget_formulaire_ventilation[2] ) == style_entree_formulaire[0] )
	    {
	      gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_ventilation[2] ),
				   "" );
	      gtk_widget_set_style ( widget_formulaire_ventilation[2],
				     style_entree_formulaire[1] );
	      gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_ventilation[2]),
				   _("Debit") );
	    }

	  /* comme il y a eu un changement de signe, on change aussi le type de l'op� associ�e */
	  /* s'il est affich� */

	  if ( GTK_WIDGET_VISIBLE ( widget_formulaire_ventilation[5] )
	       &&
	       GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_ventilation[5] ) -> menu ),
						       "signe_menu" ))
	       ==
	       2 )
	    {
	      GtkWidget *menu;

	      menu = creation_menu_types ( 1,
					   GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( widget_formulaire_ventilation[5] ),
										   "compte_virement" )),
					   2  );

	      if ( menu )
		gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_ventilation[5] ),
					   menu );
	      else
		gtk_option_menu_remove_menu ( GTK_OPTION_MENU ( widget_formulaire_ventilation[5] ));
	    }
	}
      else
	texte = _("Credit");
      break;

      /* sort de l'ib */

    case 4:
      if ( !strlen ( g_strstrip ( (char *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
	texte = _("Budgetary line");
      break;

      /* sort de la pi�ce comptable */

    case 7:
      if ( !strlen ( g_strstrip ( (char *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
	texte = _("Voucher");
      break;

    }


  /* l'entr�e �tait vide, on remet le d�faut */

  if ( texte )
    {
      gtk_widget_set_style ( entree,
			     style_entree_formulaire[1] );
      gtk_entry_set_text ( GTK_ENTRY ( entree ),
			   texte );
    }

  return FALSE;
}
/*******************************************************************************************/




/*******************************************************************************************/
/* Fonction ventiler_operation */
/* appel�e lorsque la cat�gorie est Ventilation lors de l'enregistrement d'une op� */
/* ou lors d'une modif d'une op� ventil�e */
/* Arguments : montant de l'op� */
/*******************************************************************************************/

void ventiler_operation ( gdouble montant )
{
  /*   si liste_ope est NULL, c'est une nouvelle op�, les ventils ne peuvent �tre associ�es */
  /* � un no d'op�, on les met donc � -1 */
  /* si c'est != NULL, c'est que c'�tait d�j� une ventilation, et on a appuy� sur voir pour */
  /* arriver ici */

  /* on commence par mettre la taille au formulaire et � la liste */

  ancienne_largeur_ventilation = 0;
  changement_taille_liste_ventilation ( liste_operations_ventilees,
					&liste_operations_ventilees->allocation,
					NULL );

  montant_operation_ventilee = montant;

  ligne_selectionnee_ventilation = NULL;

  /* affiche la liste */

  affiche_liste_ventilation ();

  ligne_selectionnee_ventilation = GINT_TO_POINTER ( -1 );

  gtk_clist_select_row ( GTK_CLIST ( liste_operations_ventilees ),
			 gtk_clist_find_row_from_data ( GTK_CLIST ( liste_operations_ventilees ),
							ligne_selectionnee_ventilation ),
			 0 );

  /* met � jour les labels */

  gtk_label_set_text ( GTK_LABEL ( label_somme_ventilee ),
		       g_strdup_printf ( "%4.2f",
					 somme_ventilee ) );


/*   s'il n'y a pas de montant total, celui ci = la somme ventil�e */

  if ( montant_operation_ventilee )
    {
      gtk_label_set_text ( GTK_LABEL ( label_non_affecte ),
			   g_strdup_printf ( "%4.2f",
					     montant_operation_ventilee - somme_ventilee ));
      gtk_label_set_text ( GTK_LABEL ( label_montant_operation_ventilee ),
			   g_strdup_printf ( "%4.2f",
					     montant_operation_ventilee ));
    }
  else
    {
      gtk_label_set_text ( GTK_LABEL ( label_non_affecte ),
			   "" );
      gtk_label_set_text ( GTK_LABEL ( label_montant_operation_ventilee ),
			   g_strdup_printf ( "%4.2f",
					     somme_ventilee ));
    }


  gtk_widget_grab_focus ( liste_operations_ventilees );


  /* bloque les fonctions */
  /* qui modifient la position dans la liste des op�s */

  gtk_signal_handler_block_by_func ( GTK_OBJECT ( frame_droite_bas ),
				     GTK_SIGNAL_FUNC(allocation_taille_formulaire),
				     NULL );
  gtk_signal_handler_block_by_func ( GTK_OBJECT ( frame_droite_bas ),
				     GTK_SIGNAL_FUNC(efface_formulaire),
				     NULL );


  /* affiche les pages de ventilation */

  gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_listes_operations ),
			  0 );
  gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_comptes_equilibrage ),
			  1 );
  gtk_widget_hide ( formulaire );
  gtk_widget_hide ( frame_droite_bas );
  gtk_widget_show ( frame_droite_bas );
  gtk_widget_hide ( barre_outils );
  gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_formulaire ),
			  1 );

  /* on grise tout ce qu'il faut */

  echap_formulaire_ventilation ();

  /* on donne le focus directement aux categ */

  clique_champ_formulaire_ventilation ();
  gtk_window_set_focus ( GTK_WINDOW ( window ),
			 GTK_COMBOFIX ( widget_formulaire_ventilation[0] ) -> entry );

}
/*******************************************************************************************/





/* ***************************************************************************************************** */
/* Fonction changement_taille_liste_ventilation */
/* appel�e d�s que la taille de la clist a chang� */
/* pour mettre la taille des diff�rentes colonnes */
/* ***************************************************************************************************** */

void changement_taille_liste_ventilation ( GtkWidget *clist,
					   GtkAllocation *allocation,
					   gpointer null )
{
  gint categorie, description, montant;
  gint largeur;
  gint col0, col1, col2;

  /*   si la largeur de grisbi est < 700, on fait rien */

  if ( window -> allocation.width < 700 )
    return;

  largeur = allocation->width;

  if ( largeur == ancienne_largeur_ventilation )
    return;

  ancienne_largeur_ventilation = largeur;

  categorie = ( 40 * largeur) / 100;
  description = ( 35 * largeur) / 100;
  montant = ( 10 * largeur) / 100;

  gtk_clist_set_column_width ( GTK_CLIST ( clist ),
			       0,
			       categorie );
  gtk_clist_set_column_width ( GTK_CLIST ( clist ),
			       1,
			       description );
  gtk_clist_set_column_width ( GTK_CLIST ( clist ),
			       2,
			       montant );

/* met les entr�es du formulaire � la m�me taille */

  col0 = largeur * 32  / 100;
  col1 = largeur * 32  / 100;
  col2 = largeur * 15  / 100;

  /* 1�re ligne */

  gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_ventilation[0] ),
			 col0,
			 FALSE );
  gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_ventilation[1] ),
			 col1,
			 FALSE );
  gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_ventilation[2] ),
			 col2,
			 FALSE );
  gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_ventilation[3] ),
			 col2,
			 FALSE );

  /* 2�me ligne */

  gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_ventilation[4] ),
			 col0,
			 FALSE );
  gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_ventilation[5] ),
			 col1 / 2,
			 FALSE );
  gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_ventilation[6] ),
			 col1/2,
			 FALSE );
  gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_ventilation[7] ),
			 col2,
			 FALSE );


}
/* ***************************************************************************************************** */



/***************************************************************************************************/
/* Fonction traitement_clavier_liste */
/* g�re le clavier sur la clist */
/***************************************************************************************************/

gboolean traitement_clavier_liste_ventilation ( GtkCList *liste,
						GdkEventKey *evenement,
						gpointer null )
{
  gint ligne;


  p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

  gtk_signal_emit_stop_by_name ( GTK_OBJECT ( liste ),
				 "key_press_event");

  switch ( evenement->keyval )
    {
     /* entr�e */
    case GDK_KP_Enter:
    case GDK_Return:
      edition_operation_ventilation ();
      break;

     /* fl�che haut  */
    case GDK_Up:
      ligne = gtk_clist_find_row_from_data ( GTK_CLIST ( liste ),
					     ligne_selectionnee_ventilation );
      if ( ligne )
	{
	  ligne--;

	  ligne_selectionnee_ventilation = gtk_clist_get_row_data ( GTK_CLIST ( liste ),
								    ligne );
	  gtk_clist_unselect_all ( GTK_CLIST ( liste ) );
	  gtk_clist_select_row ( GTK_CLIST ( liste ), ligne, 0 );

	  if ( gtk_clist_row_is_visible ( GTK_CLIST ( liste ),
					  ligne) != GTK_VISIBILITY_FULL )
	    gtk_clist_moveto ( GTK_CLIST ( liste ),
			       ligne, 0, 0, 0 );
	}
      break;


      /* fl�che bas */
    case GDK_Down:
      if ( ligne_selectionnee_ventilation != GINT_TO_POINTER ( -1 ) )
	{
	  ligne = gtk_clist_find_row_from_data ( GTK_CLIST ( liste ),
						 ligne_selectionnee_ventilation );

	  ligne++;

	  ligne_selectionnee_ventilation = gtk_clist_get_row_data ( GTK_CLIST ( liste ),
							    ligne );
	  gtk_clist_unselect_all ( GTK_CLIST ( liste ) );
	  gtk_clist_select_row ( GTK_CLIST ( liste ),
				 ligne,
				 0 );


	  if ( gtk_clist_row_is_visible ( GTK_CLIST ( liste ),
					  ligne )
	       != GTK_VISIBILITY_FULL )
	    gtk_clist_moveto ( GTK_CLIST ( liste ),
			       ligne,
			       0,
			       1,
			       0 );
	}
      break;


    /*  del  */
    case GDK_Delete:
      supprime_operation_ventilation ();
      break;


    default : 
      return TRUE;
    }

  return FALSE;
}
/***************************************************************************************************/






/***************************************************************************************************/
/* Fonction selectionne_ligne_souris */
/* place la s�lection sur l'op� click�e */
/***************************************************************************************************/

void selectionne_ligne_souris_ventilation ( GtkCList *liste,
					    GdkEventButton *evenement,
					    gpointer null )
{
  gint colonne, x, y;
  gint ligne;

  gtk_signal_emit_stop_by_name ( GTK_OBJECT ( liste_operations_ventilees ),
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

  if ( !gtk_clist_get_row_data ( GTK_CLIST ( liste ),
				 ligne ) )
    return;

  /*   vire l'ancienne s�lection */

  gtk_clist_unselect_all ( GTK_CLIST ( liste ) );

  /* on met l'adr de la struct dans OPERATION_SELECTIONNEE */

  ligne_selectionnee_ventilation = gtk_clist_get_row_data ( GTK_CLIST ( liste ),
							    ligne );

  gtk_clist_select_row ( GTK_CLIST ( liste ),
			 ligne,
			 colonne );

  gtk_widget_grab_focus ( GTK_WIDGET (liste) );

  if ( evenement -> type == GDK_2BUTTON_PRESS )
    edition_operation_ventilation ();

}
/***************************************************************************************************/



/***********************************************************************************************************/
/* Fonction appui_touche_ventilation  */
/* g�re l'action du clavier sur les entr�es du formulaire de ventilation */
/***********************************************************************************************************/
gboolean appui_touche_ventilation ( GtkWidget *entree, GdkEventKey *evenement,
				    gint *no_origine )
{
  gint origine;

  origine = GPOINTER_TO_INT ( no_origine );

  /*   si etat.entree = 1, la touche entr�e finit l'op�ration (
       fonction par d�faut ) sinon elle fait comme tab */

  if ( !etat.entree
       &&
       ( evenement->keyval == 65293
	 ||
	 evenement->keyval == 65421 ))
    evenement -> keyval = 65289;


  switch (evenement->keyval)
    {

    case GDK_Up:
    case GDK_Down:
      gtk_signal_emit_stop_by_name ( GTK_OBJECT ( entree ),
				     "key_press_event");
      gtk_widget_grab_focus ( entree );
      break;


    case GDK_Tab:
      gtk_signal_emit_stop_by_name ( GTK_OBJECT ( entree ),
				     "key_press_event");

      /* on efface la s�lection en cours si c'est une entr�e ou un combofix */

      if ( GTK_IS_ENTRY ( entree ))
	gtk_entry_select_region ( GTK_ENTRY ( entree ), 0, 0);
      else
	if ( GTK_IS_COMBOFIX ( entree ))
	  gtk_entry_select_region ( GTK_ENTRY(GTK_COMBOFIX(entree) -> entry),
				    0, 0);

      /* on donne le focus au widget suivant */

      origine = (origine + 1 ) % 8;

      while ( !(GTK_WIDGET_VISIBLE ( widget_formulaire_ventilation[origine] )
		&&
		GTK_WIDGET_SENSITIVE ( widget_formulaire_ventilation[origine] )
		&&
		( GTK_IS_COMBOFIX (widget_formulaire_ventilation[origine] )
		  ||
		  GTK_IS_ENTRY ( widget_formulaire_ventilation[origine] )
		  ||
		  GTK_IS_BUTTON ( widget_formulaire_ventilation[origine] ) )))
	origine = (origine + 1 ) % 8;

     /*       si on se retrouve sur les cat�g et que etat.entree = 0, on enregistre l'op�rations */

      if ( !origine && !etat.entree )
	{
	  fin_edition_ventilation();
	  return;
	}

      /* si on se retrouve sur le cr�dit et qu'il y a qque chose dans le d�bit, on passe au suivant */
      /*       � ce niveau, il n'y a pas eu encore de focus out donc on peut tester par strlen */

      if ( origine == 3
	   &&
	   strlen ( (char *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_ventilation[2] ))))
	origine = (origine + 1 ) % 8;

      /* on s�lectionne le contenu de la nouvelle entr�e */

      if ( GTK_IS_COMBOFIX ( widget_formulaire_ventilation[origine] ) )
	{
	  gtk_widget_grab_focus ( GTK_COMBOFIX ( widget_formulaire_ventilation[origine] ) -> entry );  
	  gtk_entry_select_region ( GTK_ENTRY ( GTK_COMBOFIX ( widget_formulaire_ventilation[origine] ) -> entry ),
				    0,
				    -1 );
	}
      else
	{
	  if ( GTK_IS_ENTRY ( widget_formulaire_ventilation[origine] ) )
	    gtk_entry_select_region ( GTK_ENTRY ( widget_formulaire_ventilation[origine] ),
				      0,
				      -1 );

	  gtk_widget_grab_focus ( widget_formulaire_ventilation[origine]  );
	}
      break;


    case GDK_KP_Enter:
    case GDK_Return:
      gtk_signal_emit_stop_by_name ( GTK_OBJECT ( entree ),
				     "key_press_event");
      fin_edition_ventilation ();
      break;


    case GDK_Escape :
      echap_formulaire_ventilation ();
      break;

    default:
      return FALSE;
    }

  return TRUE;
}
/***********************************************************************************************************/





/***********************************************************************************************************/
void echap_formulaire_ventilation ( void )
{

  /* on met les styles des entr�es au gris */

  gtk_widget_set_style ( GTK_COMBOFIX ( widget_formulaire_ventilation[0] )->entry,
			 style_entree_formulaire[1] );
  gtk_widget_set_style ( widget_formulaire_ventilation[1],
			 style_entree_formulaire[1] );
  gtk_widget_set_style ( widget_formulaire_ventilation[2],
			 style_entree_formulaire[1] );
  gtk_widget_set_style ( widget_formulaire_ventilation[3],
			 style_entree_formulaire[1] );
  gtk_widget_set_style ( GTK_COMBOFIX ( widget_formulaire_ventilation[4] )->entry,
			 style_entree_formulaire[1] );
  gtk_widget_set_style ( widget_formulaire_ventilation[7],
			 style_entree_formulaire[1] );


  gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_ventilation[0] ),
			  _("Categories : Sub-categories") );
  gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_ventilation[1]),
		       _("Notes") );
  gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_ventilation[2]),
		       _("Debit") );
  gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_ventilation[3]),
		       _("Credit") );
  gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_ventilation[4] ),
			  _("Budgetary line") );
  gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_ventilation[7]),
		       _("Voucher") );

  gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_ventilation[5] ),
				0 );

  gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_ventilation[5] ),
			     FALSE );
  gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_ventilation[6] ),
			     FALSE );
  gtk_widget_set_sensitive ( GTK_WIDGET ( hbox_valider_annuler_ventil ),
			     FALSE );
  gtk_widget_set_sensitive ( widget_formulaire_ventilation[0],
			     TRUE );
  gtk_widget_set_sensitive ( widget_formulaire_ventilation[2],
			     TRUE );
  gtk_widget_set_sensitive ( widget_formulaire_ventilation[3],
			     TRUE );

  gtk_widget_hide ( widget_formulaire_ventilation[5] );

/*   met l'adr de l'op� dans le formulaire � -1 */

  gtk_object_set_data ( GTK_OBJECT ( widget_formulaire_ventilation[0] ),
			"adr_struct_ope",
			GINT_TO_POINTER ( -1 ) );

  gtk_widget_grab_focus ( liste_operations_ventilees );
}
/***********************************************************************************************************/





/***********************************************************************************************************/
void fin_edition_ventilation ( void )
{
  struct struct_ope_ventil *operation;
  gint modification;
  gchar **tableau_char;
  gint compte_vire;
  gint perte_ligne_selectionnee;

  /* pour �viter les warnings lors de la compil */

  compte_vire = 0;
  tableau_char = NULL;

  /* on met le focus sur la liste des op�s pour �ventuellement faire perdre le focus aux entr�es des */
  /* montants pour faire les modifs n�cessaires automatiquement */

  gtk_window_set_focus ( GTK_WINDOW ( window ),
			 liste_operations_ventilees );

  /* perte ligne s�lectionn�e sera � 1 s'il y a une magouille avec les virements et */
  /* qu'on recr�e une op� au lieu de la modifier. dans ce cas on remettra la ligne */
  /* s�lectionn� sur la nouvelle op� */

  perte_ligne_selectionnee = 0;

  /*   dans cette fonction, on r�cup�re les infos du formulaire qu'on met dans une structure */
  /* de ventilation, et on ajoute cette structure � celle en cours (ou modifie si elle existait */
  /* d�j� */

  /* on v�rifie si c'est un virement que le compte est valide et que ce n'est pas un virement sur lui-m�me */


  if ( gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_ventilation[0] ) -> entry ) == style_entree_formulaire[0] )
    {
      /*       on split d�j� les cat�g, sans lib�rer la variable, pour la r�cup�rer ensuite pour les categ */

      tableau_char = g_strsplit ( g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_ventilation[0] ))),
				  ":",
				  2 );
      if ( tableau_char[0]  )
	{
	  tableau_char[0] = g_strstrip ( tableau_char[0] );

	  if ( tableau_char[1] )
	    tableau_char[1] = g_strstrip ( tableau_char[1] );


	  if ( !strcmp ( tableau_char[0],
			 _("Transfer")))
	    {
	      gint i;

	      if ( tableau_char[1] )
		{
		  compte_vire = -1;

		  for ( i = 0 ; i < nb_comptes ; i++ )
		    {
		      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

		      if ( !strcmp ( NOM_DU_COMPTE,
				     tableau_char[1] ) )
			compte_vire = i;
		    }

		  if ( compte_vire == -1 )
		    {
		      dialogue_error ( _("The associated account for this transfer is invalid") );
		      return;
		    }

		  if ( compte_vire == compte_courant )
		    {
		      dialogue_error ( _("It's impossible to transfer an account to itself") );
		      return;
		    }
		}
	      else
		{
		  dialogue_error ( _("No account associated with the transfer") );
		  return;
		}
	    }
	}
    }


  /*   on r�cup�re l'adresse de l'op�ration, soit c'est une modif, soit c'est une nouvelle (-1) */

  operation = gtk_object_get_data ( GTK_OBJECT ( widget_formulaire_ventilation[0] ),
				    "adr_struct_ope" );


  if (operation == GINT_TO_POINTER ( -1 ))
    {
      operation = calloc ( 1,
			   sizeof ( struct struct_ope_ventil ));
      modification = 0;
    }
  else
    modification = 1;


  /*   r�cup�ration des cat�gories / sous-cat�g, s'ils n'existent pas, on les cr�e */
  /* la variable tableau_char est d�j� initialis�e lors des tests du virement */

  /*   il y a 3 possibilit�s en rapport avec les virements : */
  /* si l'ancienne op� �tait un virement, la nouvelle est : */
  /* soit virement vers le m�me compte */
  /* soit virement vers un autre compte */
  /* soit ce n'est plus un virement */
  /*     pour la 1�re, c'est une modif normale d'op� */
  /*     pour les 2nde et 3�me, on supprime cette op� et en recr�e une nouvelle */

  /* il faut donc mettre la r�cup des cat�g en premier car il peut y avoir un changement au niveau des */
  /* modif avec suppression de l'ancienne et cr�ation d'une nouvelle ope */

  if ( gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_ventilation[0] ) -> entry ) == style_entree_formulaire[0] )
    {
      struct struct_categ *categ;

      if ( strlen ( tableau_char[0] ) )
	{
	  /* on v�rifie ici si c'est un virement */

	  if ( strcmp ( tableau_char[0],
			_("Transfer") ) )
	    {
	      /* ce n'est pas un virement, recherche les cat�g */

	      GSList *pointeur_liste;

	      /* si c'est une modif d'op� et que l'ancienne op� �tait un virement */
	      /* on marque cette op� comme supprim�e et on en fait une nouvelle */

	      if ( modification
		   &&
		   operation -> relation_no_operation )
		{
		  operation -> supprime = 1;
		  operation = calloc ( 1,
				       sizeof ( struct struct_ope_ventil ));
		  modification = 0;
		  perte_ligne_selectionnee = 1;
		}

	      /* recherche des cat�gories */

	      pointeur_liste = g_slist_find_custom ( liste_struct_categories,
						     tableau_char[0],
						     ( GCompareFunc ) recherche_categorie_par_nom );

	      if ( pointeur_liste )
		categ = pointeur_liste -> data;
	      else
		{
		  categ = ajoute_nouvelle_categorie ( tableau_char[0] );
		  if ( operation -> montant < 0 )
		    categ -> type_categ = 1;
		  else
		    categ -> type_categ = 0;
		}
	  
	      operation -> categorie = categ -> no_categ;
	  
	      if ( tableau_char[1] && strlen (tableau_char[1]) )
		{
		  struct struct_sous_categ *sous_categ;
		  
		  pointeur_liste = g_slist_find_custom ( categ -> liste_sous_categ,
							 tableau_char[1],
							 ( GCompareFunc ) recherche_sous_categorie_par_nom );
	      
		  if ( pointeur_liste )
		    sous_categ = pointeur_liste -> data;
		  else
		    sous_categ = ajoute_nouvelle_sous_categorie ( tableau_char[1],
								  categ );
		  
		  operation -> sous_categorie = sous_categ -> no_sous_categ;
		}
	      else
		operation -> sous_categorie = 0;
	    }
	  else
	    {
	      /* c'est un virement */

	      /* si c'est une nouvelle op�, on est content et on prend juste le compte de virement */
	      /* si c'est une modif d'op� et que l'ancienne n'�tait pas un virement, idem */
	      /* si l'ancienne �tait un virement vers le m�me compte, idem */
	      /* si l'ancienne �tait un virement vers un autre compte, c'est qu'on cherche les bugs ... */
	      /* dans ce cas, on marque l'op� comme supprim�e et on en recr�e une nouvelle */

	      /* le no de compte du virement est d�j� dans compte_vire */

	      if ( modification
		   &&
		   operation -> relation_no_operation != -1
		   &&
		   operation -> relation_no_compte != compte_vire )
		{
		  /* on supprime donc l'op� et en cr�e une nouvelle */

		  operation -> supprime = 1;
		  operation = calloc ( 1,
				       sizeof ( struct struct_ope_ventil ));
		  modification = 0;
		  perte_ligne_selectionnee = 1;
		}

	      /* on met les no de categ � 0 */

	      operation -> categorie = 0;
	      operation -> sous_categorie = 0;

	      /* on met le compte en relation si c'est une nouvelle op�ration */

	      if ( !modification )
		operation -> relation_no_operation = -1;

	      operation -> relation_no_compte = compte_vire;
	    }
	}
      /*       on peut maintenant lib�rer la variable tableau_char, qui ne sera plus utilis�e */

      g_strfreev ( tableau_char );
    }
  else
    {
      /* il n'y a aucune cat�g, si c'est une modif d'op� et que cette op� �tait un virement, */
      /* on marque cette op� comme supprim�e et on en recr�e une nouvelle */

      if ( modification
	   &&
	   operation -> relation_no_operation )
	{
	  operation -> supprime = 1;
	  operation = calloc ( 1,
			       sizeof ( struct struct_ope_ventil ));
	  modification = 0;
	  perte_ligne_selectionnee = 1;
	}
    }

  /* r�cup�ration du type d'op� associ�e s'il est affich� */

  if ( GTK_WIDGET_VISIBLE ( widget_formulaire_ventilation[5] ))
    operation -> no_type_associe = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_ventilation[5] ) -> menu_item ),
									   "no_type" ));

  /* r�cup�ration des notes */

  if ( gtk_widget_get_style ( widget_formulaire_ventilation[1] ) == style_entree_formulaire[0] )
    operation -> notes = g_strdup ( g_strstrip ( (char *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_ventilation[1] ))));
  else
    operation -> notes = NULL;


  /* r�cup�ration du montant */

  if ( gtk_widget_get_style ( widget_formulaire_ventilation[2] ) == style_entree_formulaire[0] )
    /* c'est un d�bit */
    operation -> montant = -my_strtod ( g_strstrip ( (char *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_ventilation[2] ))),
				       NULL );
  else
    operation -> montant = my_strtod ( g_strstrip ( (char *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_ventilation[3] ))),
				      NULL );



  /* r�cup�ration de l'imputation budg�taire */

  if ( gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_ventilation[4] ) -> entry ) == style_entree_formulaire[0] )
    {
      struct struct_imputation *imputation;
      gchar **tableau_char;
      GSList *pointeur_liste;

      tableau_char = g_strsplit ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_ventilation[4] )),
				  ":",
				  2 );
      
      tableau_char[0] = g_strstrip ( tableau_char[0] );

      if ( tableau_char[1] )
	tableau_char[1] = g_strstrip ( tableau_char[1] );

      pointeur_liste = g_slist_find_custom ( liste_struct_imputation,
					     tableau_char[0],
					     ( GCompareFunc ) recherche_imputation_par_nom );

      if ( pointeur_liste )
	imputation = pointeur_liste -> data;
      else
	{
	  imputation = ajoute_nouvelle_imputation ( tableau_char[0] );

	  if ( operation -> montant < 0 )
	    imputation -> type_imputation = 1;
	  else
	    imputation -> type_imputation = 0;
	}

      operation -> imputation = imputation -> no_imputation;
	  
      if ( tableau_char[1] && strlen (tableau_char[1]) )
	{
	  struct struct_sous_imputation *sous_imputation;
		  
	  pointeur_liste = g_slist_find_custom ( imputation -> liste_sous_imputation,
						 tableau_char[1],
						 ( GCompareFunc ) recherche_sous_imputation_par_nom );
	      
	  if ( pointeur_liste )
	    sous_imputation = pointeur_liste -> data;
	  else
	    sous_imputation = ajoute_nouvelle_sous_imputation ( tableau_char[1],
								imputation );

	  operation -> sous_imputation = sous_imputation -> no_sous_imputation;
	}
      else
	operation -> sous_imputation = 0;

      g_strfreev ( tableau_char );
    }

  /* r�cup�ration de l'exercice */
  /* si l'exo est � -1, c'est que c'est sur non affich� */
  /* soit c'est une modif d'op� et on touche pas � l'exo */
  /* soit c'est une nouvelle op� et on met l'exo � 0 */

  if ( GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_ventilation[6] ) -> menu_item ),
					       "no_exercice" )) == -1 )
    {
      if ( !operation -> no_operation )
	operation -> no_exercice = 0;
    }
  else
    operation -> no_exercice = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_ventilation[6] ) -> menu_item ),
								       "no_exercice" ));


/* r�cup�ration du no de pi�ce comptable */

  if ( gtk_widget_get_style ( widget_formulaire_ventilation[7] ) == style_entree_formulaire[0] )
    operation -> no_piece_comptable = g_strdup ( g_strstrip ( (char *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_ventilation[7] ))));
  else
    operation -> no_piece_comptable = NULL;


 
  /* on a rempli l'op�ration, on l'ajoute � la liste */
  /* si c'est une modif */

  if ( !modification )
    {
      GSList *liste_struct_ventilations;

      /* r�cup�ration de la liste de ventilations */

      liste_struct_ventilations = gtk_object_get_data ( GTK_OBJECT ( formulaire ),
							"liste_adr_ventilation" );

      /*   si cette liste est � -1 (ce qui veut dire qu'elle est nulle en r�alit� mais */
      /* qu'elle a d�j� �t� �dit�e ), on la met � 0 */

      if ( liste_struct_ventilations == GINT_TO_POINTER ( -1 ))
	liste_struct_ventilations = NULL;

      /* on ajoute l'op� */

      liste_struct_ventilations = g_slist_append ( liste_struct_ventilations,
						   operation );

      gtk_object_set_data ( GTK_OBJECT ( formulaire ),
			    "liste_adr_ventilation",
			    liste_struct_ventilations );
    }


  /*   si perte_ligne_selectionnee = 1, c'est qu'au lieu de modifier une op� (virement), on l'a */
  /* effac� puis recr�� une nouvelle. comme �a se fait que lors d'une modif d'op�, on remet */
  /* la selection sur cette nouvelle op� */

  if ( perte_ligne_selectionnee == 1 )
    ligne_selectionnee_ventilation = operation;


  mise_a_jour_categ ();
  mise_a_jour_imputation ();

  /* on met � jour la liste des ventilations */

  affiche_liste_ventilation ();

  /* efface le formulaire et pr�pare l'op� suivante */

  echap_formulaire_ventilation ();

  if ( modification )
    gtk_widget_grab_focus ( liste_operations_ventilees );
  else
    {
      clique_champ_formulaire_ventilation ();
      gtk_widget_grab_focus ( GTK_COMBOFIX ( widget_formulaire_ventilation[0] ) -> entry );
    }
}
/***********************************************************************************************************/





/***********************************************************************************************************/
/* Fonction edition_operation_ventilation */
/* appel� lors d'un double click ou entr�e sur une op� de ventilation */
/***********************************************************************************************************/

void edition_operation_ventilation ( void )
{
  struct struct_ope_ventil *operation;
  GSList *liste_tmp;

  /* on r�cup�re la struc de l'op� de ventil, ou -1 si c'est une nouvelle */

  operation = ligne_selectionnee_ventilation;

  echap_formulaire_ventilation ();

  /* d�grise ce qui est n�cessaire */

  clique_champ_formulaire_ventilation ();

  gtk_object_set_data ( GTK_OBJECT ( widget_formulaire_ventilation[0] ),
			"adr_struct_ope",
			operation );

/* si l'op� est -1, c'est que c'est une nouvelle op� */

  if ( operation == GINT_TO_POINTER ( -1 ) )
    {
      gtk_widget_grab_focus ( GTK_COMBOFIX ( widget_formulaire_ventilation[0] ) -> entry );
      return;
    }


/*   l'op� n'est pas -1, c'est une modif, on remplit les champs */

  gtk_object_set_data ( GTK_OBJECT ( widget_formulaire_ventilation[0] ),
			"adr_struct_ope",
			operation );


  /* mise en forme du montant */


  if ( operation -> montant < 0 )
    {
      entree_prend_focus (widget_formulaire_ventilation[2] );
      gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_ventilation[2] ),
			   g_strdup_printf ( "%4.2f", -operation -> montant ));
    }
  else
    {
      entree_prend_focus (widget_formulaire_ventilation[3] );
      gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_ventilation[3] ),
			   g_strdup_printf ( "%4.2f", operation -> montant ));
    }

  /* si l'op�ration est relev�e, emp�che la modif du montant */

  if ( operation -> pointe == 2 )
    {
      gtk_widget_set_sensitive ( widget_formulaire_ventilation[2],
				 FALSE );
      gtk_widget_set_sensitive ( widget_formulaire_ventilation[3],
				 FALSE );
    }

  /* mise en forme des cat�gories */

  if ( operation -> relation_no_operation )
    {
      /* c'est un virement */

      GtkWidget *menu;

      entree_prend_focus (widget_formulaire_ventilation[0] );

      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

      gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_ventilation[0] ),
			      g_strconcat ( COLON(_("Transfer")),
					    NOM_DU_COMPTE,
					    NULL ));

      /*       si la contre op�ration est relev�e, on d�sensitive les montants et les categ */
      /* seulement valable si ce virement existe d�j� */

      if ( operation -> no_operation )
	{
	  struct structure_operation *contre_operation;

	  contre_operation = g_slist_find_custom ( LISTE_OPERATIONS,
						   GINT_TO_POINTER ( operation -> relation_no_operation ),
						   (GCompareFunc) recherche_operation_par_no ) -> data;

	  if ( contre_operation -> pointe == 2 )
	    {
	      gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_ventilation[0] ),
					 FALSE );
	      gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_ventilation[2] ),
					 FALSE );
	      gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_ventilation[3] ),
					 FALSE );
	    }
	}
      /* on met le type de l'op� associ�e */

      if ( operation -> montant < 0 )
	menu = creation_menu_types ( 2,
				     operation -> relation_no_compte,
				     2  );
      else
	menu = creation_menu_types ( 1,
				     operation -> relation_no_compte,
				     2  );

      if ( menu )
	{
	  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_ventilation[5] ),
				     menu );

	  gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_ventilation[5] ),
					cherche_no_menu_type_associe ( operation -> no_type_associe,
								       1 ));
	  gtk_widget_show ( widget_formulaire_ventilation[5] );

	}
    }
  else
    {
      liste_tmp = g_slist_find_custom ( liste_struct_categories,
					GINT_TO_POINTER ( operation -> categorie ),
					( GCompareFunc ) recherche_categorie_par_no );

      if ( liste_tmp )
	{
	  GSList *liste_tmp_2;
      
	  entree_prend_focus (widget_formulaire_ventilation[0] );

	  liste_tmp_2 = g_slist_find_custom ( (( struct struct_categ * )( liste_tmp -> data )) -> liste_sous_categ,
					      GINT_TO_POINTER ( operation -> sous_categorie ),
					      ( GCompareFunc ) recherche_sous_categorie_par_no );
	  if ( liste_tmp_2 )
	    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_ventilation[0] ),
				    g_strconcat ( (( struct struct_categ * )( liste_tmp -> data )) -> nom_categ,
						  " : ",
						  (( struct struct_sous_categ * )( liste_tmp_2 -> data )) -> nom_sous_categ,
						  NULL ));
	  else
	    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_ventilation[0] ),
				    (( struct struct_categ * )( liste_tmp -> data )) -> nom_categ );
      
	}
    }


  /* mise en forme des notes */

  if ( operation -> notes )
    {
      entree_prend_focus (widget_formulaire_ventilation[1] );
      gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_ventilation[1] ),
			   operation -> notes );
    }


  /* met en place l'imputation budg�taire */

  liste_tmp = g_slist_find_custom ( liste_struct_imputation,
				    GINT_TO_POINTER ( operation -> imputation ),
				    ( GCompareFunc ) recherche_imputation_par_no );

  if ( liste_tmp )
    {
      GSList *liste_tmp_2;

      entree_prend_focus ( widget_formulaire_ventilation[4] );

      liste_tmp_2 = g_slist_find_custom ( (( struct struct_imputation * )( liste_tmp -> data )) -> liste_sous_imputation,
					  GINT_TO_POINTER ( operation -> sous_imputation ),
					  ( GCompareFunc ) recherche_sous_imputation_par_no );
      if ( liste_tmp_2 )
	gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_ventilation[4] ),
				g_strconcat ( (( struct struct_imputation * )( liste_tmp -> data )) -> nom_imputation,
					      " : ",
					      (( struct struct_sous_imputation * )( liste_tmp_2 -> data )) -> nom_sous_imputation,
					      NULL ));
      else
	gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_ventilation[4] ),
				(( struct struct_imputation * )( liste_tmp -> data )) -> nom_imputation );
    }


  /* met en place l'exercice */

  gtk_option_menu_set_history (  GTK_OPTION_MENU ( widget_formulaire_ventilation[6] ),
				 cherche_no_menu_exercice ( operation -> no_exercice,
							    widget_formulaire_ventilation[6] ));

  /* mise en place de la pi�ce comptable */

  if ( operation -> no_piece_comptable )
    {
      entree_prend_focus ( widget_formulaire_ventilation[7] );
      gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_ventilation[7] ),
			   operation -> no_piece_comptable );
    }


/*   on a fini de remplir le formulaire, on donne le focus � la date */

  if ( GTK_WIDGET_SENSITIVE ( widget_formulaire_ventilation[0] ))
    {
      gtk_entry_select_region ( GTK_ENTRY ( GTK_COMBOFIX ( widget_formulaire_ventilation[0] ) -> entry ),
				0,
				-1);
      gtk_widget_grab_focus ( GTK_COMBOFIX ( widget_formulaire_ventilation[0] ) -> entry );
    }
  else
    {
      gtk_entry_select_region ( GTK_ENTRY ( widget_formulaire_ventilation[1] ),
				0,
				-1);
      gtk_widget_grab_focus ( widget_formulaire_ventilation[1] );
    }
  enregistre_ope_au_retour = 1 ;
}
/***********************************************************************************************************/





/***********************************************************************************************************/
void supprime_operation_ventilation ( void )
{
  struct struct_ope_ventil *operation;
  gint ligne;


  operation = ligne_selectionnee_ventilation;

  if ( operation == GINT_TO_POINTER ( -1 ) ||
       ! ligne_selectionnee_ventilation )
    return;

  /* si l'op�ration est relev�e ou si c'est un virement et que la contre op�ration est */
  /*   relev�e, on ne peut la supprimer */

  if ( operation -> pointe == 2 )
    {
      dialogue_error ( _("This transaction has a reconciled breakdown line, deletion canceled.") );
      return;
    }
  else
    {
      if ( operation -> relation_no_operation
	   &&
	   operation -> relation_no_operation != -1 )
	{
	  /* on va chercher la contre op�ration */

	  GSList *tmp;

	  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

	  tmp = g_slist_find_custom ( LISTE_OPERATIONS,
				      GINT_TO_POINTER ( operation -> relation_no_operation ),
				      (GCompareFunc) recherche_operation_par_no );

	  if ( tmp )
	    {
	      struct structure_operation *operation_associee;

	      operation_associee = tmp -> data;

	      if ( operation_associee -> pointe == 2 )
		{
		  dialogue_error ( _("This transfer has a reconciled contra-transaction, deletion canceled.") );
		  return;
		}
	    }
	}
    }


  /* on marque cette op�ration comme supprim�e si ce n'est pas une nouvelle */
  /* sinon on la supprime tout simplement de la liste */

  if ( operation -> no_operation )
    operation -> supprime = 1;
  else
    {
      GSList *liste_struct_ventilations;

      liste_struct_ventilations = gtk_object_get_data ( GTK_OBJECT ( formulaire ),
							"liste_adr_ventilation" );

      liste_struct_ventilations = g_slist_remove ( liste_struct_ventilations,
						   operation );
      gtk_object_set_data ( GTK_OBJECT ( formulaire ),
			    "liste_adr_ventilation",
			    liste_struct_ventilations );
    }

  /*   si la s�lection est sur l'op� qu'on supprime, on met la s�lection sur celle du dessous */

  ligne = gtk_clist_find_row_from_data ( GTK_CLIST ( liste_operations_ventilees ),
					 operation );
  ligne_selectionnee_ventilation = gtk_clist_get_row_data ( GTK_CLIST ( liste_operations_ventilees ),
							    ligne + 1 );

  /* supprime l'op�ration de la liste */

  gtk_clist_remove ( GTK_CLIST ( liste_operations_ventilees ),
		     ligne );

  calcule_montant_ventilation();
  mise_a_jour_couleurs_liste_ventilation();
  selectionne_ligne_ventilation ();
  enregistre_ope_au_retour = 1 ;
}
/***********************************************************************************************************/



/***********************************************************************************************************/
/* Fonction affiche_liste_ventilation */
/* r�cup�re la liste des struct d'op� de ventil sur le formulaire et affiche ces op�s */
/***********************************************************************************************************/

void affiche_liste_ventilation ( void )
{
  gchar *ligne[3];
  gint ligne_insertion;
  GSList *liste_tmp;


  somme_ventilee = 0;

  gtk_clist_freeze ( GTK_CLIST ( liste_operations_ventilees ) );

  gtk_clist_clear ( GTK_CLIST ( liste_operations_ventilees ) );

  /* r�cup�re la liste des struct_ope_ventil */

  liste_tmp = gtk_object_get_data ( GTK_OBJECT ( formulaire ),
				    "liste_adr_ventilation" );

  while ( liste_tmp && GPOINTER_TO_INT ( liste_tmp ) != -1 )
    {
      ajoute_ope_sur_liste_ventilation ( liste_tmp -> data );

      liste_tmp = liste_tmp -> next;
    }


  /* ajoute la ligne blanche associee � -1 */

  ligne[0] = NULL;
  ligne[1] = NULL;
  ligne[2] = NULL;

  ligne_insertion = gtk_clist_append ( GTK_CLIST ( liste_operations_ventilees ),
				       ligne );

  gtk_clist_set_row_data ( GTK_CLIST ( liste_operations_ventilees ),
			   ligne_insertion,
			   GINT_TO_POINTER ( -1 ));



  /* on met la couleur */

  mise_a_jour_couleurs_liste_ventilation ();


  /* on s�lectionne la ligne blanche */

  selectionne_ligne_ventilation ();

  gtk_clist_thaw ( GTK_CLIST ( liste_operations_ventilees ));

  /* on met � jour les labels d'�tat */

  calcule_montant_ventilation ();
}
/***********************************************************************************************************/


/***********************************************************************************************************/
/* prend en argument une op� de ventil dont l'adr de la struct est donn�e en argument */
/***********************************************************************************************************/

void ajoute_ope_sur_liste_ventilation ( struct struct_ope_ventil *operation )
{
  gchar *ligne[3];
  gint ligne_insertion;
  GSList *liste_tmp;

  /*   si cette op�ration a �t� supprim�e, on ne l'affiche pas */

  if ( operation -> supprime )
    return;


  /* mise en forme des cat�gories */

  if ( operation -> relation_no_operation )
    {
      /* c'est un virement */

      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

      ligne [0] = g_strconcat ( COLON(_("Transfer")),
				NOM_DU_COMPTE,
				NULL );
      p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
    }
  else
    {
      /* c'est des categ : sous categ */

      liste_tmp = g_slist_find_custom ( liste_struct_categories,
					GINT_TO_POINTER ( operation -> categorie ),
					( GCompareFunc ) recherche_categorie_par_no );


      if ( liste_tmp )
	{
	  GSList *liste_tmp_2;

	  liste_tmp_2 = g_slist_find_custom ( (( struct struct_categ * )( liste_tmp -> data )) -> liste_sous_categ,
					      GINT_TO_POINTER ( operation -> sous_categorie ),
					      ( GCompareFunc ) recherche_sous_categorie_par_no );
	  if ( liste_tmp_2 )
	    ligne [0] = g_strconcat ( (( struct struct_categ * )( liste_tmp -> data )) -> nom_categ,
				      " : ",
				      (( struct struct_sous_categ * )( liste_tmp_2 -> data )) -> nom_sous_categ,
				      NULL );
	  else
	    ligne [0] = (( struct struct_categ * )( liste_tmp -> data )) -> nom_categ;
      
	}
      else
	ligne[0] = NULL;
    }


  /* mise en forme des notes */

  ligne[1] = operation -> notes;

  /* mise en forme du montant */

  ligne[2] = g_strdup_printf ( "%4.2f",
			       operation -> montant );


  ligne_insertion = gtk_clist_append ( GTK_CLIST ( liste_operations_ventilees ),
				       ligne );

  /* on associe � cette ligne l'adr de la struct de l'op� */

  gtk_clist_set_row_data ( GTK_CLIST ( liste_operations_ventilees ),
			   ligne_insertion,
			   operation);

  calcule_montant_ventilation ();
}
/***********************************************************************************************************/


/***********************************************************************************************************/
void calcule_montant_ventilation ( void )
{
  gint ligne;
  struct struct_ope_ventil *operation;

  /* fait le tour de la liste pour retrouver les ventil affich�e pour calculer le montant */


  ligne = 0;
  somme_ventilee = 0;

  while ( ( operation = gtk_clist_get_row_data ( GTK_CLIST ( liste_operations_ventilees ),
						 ligne )) != GINT_TO_POINTER ( -1 )
	  &&
	  operation )
    {
      somme_ventilee = somme_ventilee + operation -> montant;
      ligne++;
    }

  mise_a_jour_labels_ventilation ();
}
/***********************************************************************************************************/




/***********************************************************************************************************/
void mise_a_jour_labels_ventilation ( void )
{
  gtk_label_set_text ( GTK_LABEL ( label_somme_ventilee ),
		       g_strdup_printf ( "%4.2f",
					 somme_ventilee ));

  if ( montant_operation_ventilee )
    {
      gtk_label_set_text ( GTK_LABEL ( label_montant_operation_ventilee ),
			   g_strdup_printf ( "%4.2f",
					     montant_operation_ventilee ));

      gtk_label_set_text ( GTK_LABEL ( label_non_affecte ),
			   g_strdup_printf ( "%4.2f",
					     montant_operation_ventilee - somme_ventilee ));
    }
  else
    {
      gtk_label_set_text ( GTK_LABEL ( label_non_affecte ),
			   "" );
      gtk_label_set_text ( GTK_LABEL ( label_montant_operation_ventilee ),
			   g_strdup_printf ( "%4.2f",
					     somme_ventilee ));
    }
}
/***********************************************************************************************************/




/***********************************************************************************************************/
/* Fait le tour le la liste de ventilation et met bien les couleurs */
/***********************************************************************************************************/

void mise_a_jour_couleurs_liste_ventilation ( void )
{
  gint i;

  i=-1;

  do
    {
      i++;

      gtk_clist_set_row_style ( GTK_CLIST ( liste_operations_ventilees ),
				i,
				style_couleur [ i % 2 ] );
    }
  while ( gtk_clist_get_row_data ( GTK_CLIST ( liste_operations_ventilees ),
				   i ) != GINT_TO_POINTER ( -1 ));
}
/***********************************************************************************************************/


/***********************************************************************************************************/
void selectionne_ligne_ventilation ( void )
{
  gint ligne_selectionnee;

  ligne_selectionnee = gtk_clist_find_row_from_data ( GTK_CLIST ( liste_operations_ventilees ),
						      ligne_selectionnee_ventilation );

  gtk_clist_select_row ( GTK_CLIST ( liste_operations_ventilees ),
			 ligne_selectionnee,
			 0 );

  if ( gtk_clist_row_is_visible ( GTK_CLIST ( liste_operations_ventilees ),
				 ligne_selectionnee )
       != GTK_VISIBILITY_FULL )
    gtk_clist_moveto ( GTK_CLIST ( liste_operations_ventilees ),
		       ligne_selectionnee,
		       0,
		       0.5,
		       0 );
}
/***********************************************************************************************************/

/* ************************************************************************** */
/* Fonction valider_ventilation                                               */
/* appel�e par appui du bouton valider                                        */
/* ************************************************************************** */
void valider_ventilation ( void )
{
  /* Cette fonction est toute simple car la liste des structures des
     ventilations a �t� mise � jour au fur et � mesure et toujours associ�e
     au formulaire des op�rations. Donc, il faut juste r�afficher ce qu'il faut
     et return. C'est la validation r�elle de l'op�ration qui cr�era/supprimera
     toutes les op�rations */

  /* Si par contre cette liste est null, on met -1 sur le formulaire pour
     montrer qu'on est pass� par l� et qu'on veut une liste nulle */

  /* On associe l'adresse de la nouvelle liste des ventilation au formulaire,
     met -1 si la liste est vide */

  if ( !gtk_object_get_data ( GTK_OBJECT ( formulaire ),
			      "liste_adr_ventilation" ))
    gtk_object_set_data ( GTK_OBJECT ( formulaire ),
			  "liste_adr_ventilation",
			  GINT_TO_POINTER ( -1 ) );
/*
  if ( gtk_object_get_data ( GTK_OBJECT ( formulaire ), "liste_adr_ventilation" ) == GINT_TO_POINTER ( -1 ) )
    dialogue("Liste nulle");
*/
  if ( fabs ( montant_operation_ventilee - somme_ventilee ) >= 0.000001 )
	{
	  GtkWidget *dialog;
	  GtkWidget *label;
	  gint resultat;

	  dialog = gnome_dialog_new ( _("Breakdown incomplete"),
					GNOME_STOCK_BUTTON_YES,
					GNOME_STOCK_BUTTON_NO,
					NULL );

	  label = gtk_label_new ( _("The transaction amount isn't fully broken down.\n\nProcess anyway?"));
	  gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
			       label,
			       FALSE,
			       FALSE,
			       0 );
	  gtk_widget_show ( label );
	  gtk_window_set_position ( GTK_WINDOW ( dialog ), GTK_WIN_POS_CENTER );

	  resultat = gnome_dialog_run_and_close ( GNOME_DIALOG ( dialog ));

	  /* si on n'appuye pas sur YES, alors on abandonne */
	  if ( resultat != 0 )
	    {
	     return;
	    }
	}

  quitter_ventilation ();

  if ( enregistre_ope_au_retour )
    fin_edition();
}
/* ************************************************************************** */

/* ************************************************************************** */
/* Fonction annuler_ventilation                                               */
/* appel�e par appui du bouton annuler                                        */
/* ************************************************************************** */
void annuler_ventilation ( void )
{
  /* Cette fonction remet la liste des structures de ventilation par d�faut
     en recherchant les op�rations de ventilation dans la liste des op�rations
     puis appelle valider ventilation */

  gtk_object_set_data ( GTK_OBJECT ( formulaire ),
			"liste_adr_ventilation",
			creation_liste_ope_de_ventil ( gtk_object_get_data ( GTK_OBJECT ( formulaire ),
									     "adr_struct_ope" )));

  quitter_ventilation ();
}
/* ************************************************************************** */

/* ************************************************************************** */
/* Fonction quitter_ventilation                                               */
/* appel�e valider_ventilation et quitter_ventilation                         */
/* ************************************************************************** */
void quitter_ventilation ( void )
{
  /* Cette fonction remet la liste des structures de ventilation par d�faut
     en recherchant les op�rations de ventilation dans la liste des op�rations
     puis appelle valider ventilation */

  gtk_widget_show ( barre_outils );
  gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_listes_operations ), compte_courant + 1 );
  gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_comptes_equilibrage ), 0 );
  gtk_widget_show ( formulaire );

  /* On a r�affich� le formulaire, on peut d�bloquer les fonctions
     qui modifient la position dans la liste des op�rations */

  gtk_signal_handler_unblock_by_func ( GTK_OBJECT ( frame_droite_bas ),
			       allocation_taille_formulaire,
			       NULL );
  gtk_signal_handler_unblock_by_func ( GTK_OBJECT ( frame_droite_bas ),
			       efface_formulaire,
			       NULL );

  gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_formulaire ), 0 );

  gtk_widget_grab_focus ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ) -> entry );

  if ( !montant_operation_ventilee )
    {
      if ( somme_ventilee < 0 )
	{
	  entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] );
	  gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ),
			       g_strdup_printf ( "%4.2f", fabs ( somme_ventilee ) ));
	}
      else
	{
	  entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] );
	  gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ),
			       g_strdup_printf ( "%4.2f", somme_ventilee ));
	}
    }
}
/* ************************************************************************** */

/***********************************************************************************************************/
/* Cette fonction prend une op� ventil�e en argument et cr�e la liste des op�s de ventil */
/* qui correspondent avec des struct struct_ope_ventil */
/* renvoie cette liste */
/***********************************************************************************************************/

GSList *creation_liste_ope_de_ventil ( struct structure_operation *operation )
{
  GSList *liste_ventil;
  GSList *liste_operations;
  GSList *liste_tmp;

  liste_ventil = NULL;
  liste_operations = NULL;

  /* si c'est une nouvelle op�, il n'y a aucun op� de ventil associ�e */

  if ( !operation )
    return ( NULL );

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

  liste_tmp = LISTE_OPERATIONS;

  while ( liste_tmp )
    {
      struct structure_operation *operation_2;

      operation_2 = liste_tmp -> data;

      /* si l'op�ration est une op� de ventil de l'op� demand�e, on lui fait une struct struct_ope_ventil */

      if ( operation_2 -> no_operation_ventilee_associee == operation -> no_operation )
	{
	  struct struct_ope_ventil *ope_ventil;

	  ope_ventil = calloc ( 1,
				sizeof ( struct struct_ope_ventil ));

	  ope_ventil -> no_operation = operation_2 -> no_operation;
	  ope_ventil -> montant = operation_2 -> montant;
	  ope_ventil -> categorie = operation_2 -> categorie;
	  ope_ventil -> sous_categorie = operation_2 -> sous_categorie;

	  if ( operation_2 -> notes )
	    ope_ventil -> notes = g_strdup ( operation_2 -> notes );

	  ope_ventil -> imputation = operation_2 -> imputation;
	  ope_ventil -> sous_imputation = operation_2 -> sous_imputation;

	  ope_ventil -> no_exercice = operation_2 -> no_exercice;

	  if ( ope_ventil -> no_piece_comptable )
	    ope_ventil -> no_piece_comptable = g_strdup ( operation_2 -> no_piece_comptable );

	  ope_ventil -> relation_no_operation = operation_2 -> relation_no_operation;
	  ope_ventil -> relation_no_compte = operation_2 -> relation_no_compte;
	  ope_ventil -> pointe = operation_2 -> pointe;

	  /* si c'est un virement, on va rechercher le type de l'autre op�ration */

	  if ( ope_ventil -> relation_no_operation )
	    {
	      GSList *tmp;

	      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + ope_ventil -> relation_no_compte;

	      tmp = g_slist_find_custom ( LISTE_OPERATIONS,
					  GINT_TO_POINTER ( ope_ventil -> relation_no_operation ),
					  (GCompareFunc) recherche_operation_par_no );

	      if ( tmp )
		{
		  struct structure_operation *operation_associee;

		  operation_associee = tmp -> data;
		  ope_ventil -> no_type_associe = operation_associee -> type_ope;
		}
	    }

	  liste_ventil = g_slist_append ( liste_ventil,
					  ope_ventil );
	}
      liste_tmp = liste_tmp -> next;
    }
  return ( liste_ventil );
}
/***********************************************************************************************************/



/***********************************************************************************************************/
/* cette fonction est appel�e lors de la validation d'une ventilation */
/* l'op�ration en argument a d�j� son num�ro d'op� */
/* ellse fait le tour des structures de ventil et cr�e/supprime/modifie */
/* les op�rations n�cessaires */
/***********************************************************************************************************/

void validation_ope_de_ventilation ( struct structure_operation *operation )
{
  GSList *liste_struct_ventilations;

  /* r�cup�ration de la liste de ventilations */

  liste_struct_ventilations = gtk_object_get_data ( GTK_OBJECT ( formulaire ),
						    "liste_adr_ventilation" );

  /*   si cette liste est � -1, c'est qu'elle est null, donc rien � faire */

  if ( liste_struct_ventilations == GINT_TO_POINTER ( -1 ))
    return;

  while ( liste_struct_ventilations )
    {
      struct struct_ope_ventil *ope_ventil;

      ope_ventil = liste_struct_ventilations -> data;

      /*       si cette op� est supprim�e, c'est ici */
      /* cela sous entend qu'elle existait d�j� */
      /* et si c'�tait un virement, l'autre va �tre automatiquement supprim�e */

      if ( ope_ventil -> supprime )
	{
	  /* petite protection quand m�me, normalement le texte ne devrait jamais apparaitre */

	  if ( !ope_ventil -> no_operation )
	    dialogue_warning ( _("A breakdown line is to be deleted though it is not yet registered."));
	  else
	    {
	      GSList *tmp;

	      p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
	      tmp = g_slist_find_custom ( LISTE_OPERATIONS,
					  GINT_TO_POINTER ( ope_ventil -> no_operation ),
					  (GCompareFunc) recherche_operation_par_no );

	      if ( tmp )
		supprime_operation  ( tmp -> data );
	    }
	}
      else
	{
	  /* l'op�ration ne doit pas �tre supprim�e, c'est qu'elle doit �tre cr��e ou modifi�e */
	  /* 	  on n'a pas � s'emb�ter avec des changements de virements ou autres trucs bizarres, dans */
	  /* ce cas il y aura eu une suppression puis une nouvelle op�ration */

	  if ( ope_ventil -> no_operation )
	    {
	      /* c'est une modif d'op�ration */

	      GSList *tmp;

	      p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
	      tmp = g_slist_find_custom ( LISTE_OPERATIONS,
					  GINT_TO_POINTER ( ope_ventil -> no_operation ),
					  (GCompareFunc) recherche_operation_par_no );

	      if ( tmp )
		{
		  struct structure_operation *ope_modifiee;

		  ope_modifiee = tmp -> data;

		  /* on r�cup�re d'abord les modifs de l'op� de ventil */

		  ope_modifiee -> montant = ope_ventil -> montant;
		  ope_modifiee -> categorie = ope_ventil -> categorie;
		  ope_modifiee -> sous_categorie = ope_ventil -> sous_categorie;

		  if ( ope_ventil -> notes )
		    ope_modifiee -> notes = g_strdup ( ope_ventil -> notes );

		  ope_modifiee -> no_exercice = ope_ventil -> no_exercice;

		  ope_modifiee -> imputation = ope_ventil -> imputation;
		  ope_modifiee -> sous_imputation = ope_ventil -> sous_imputation;

		  if ( ope_ventil -> no_piece_comptable )
		    ope_modifiee -> no_piece_comptable = g_strdup ( ope_ventil -> no_piece_comptable );

		  /* on r�cup�re ensuite les modifs de la ventilation */

		  ope_modifiee -> jour = operation -> jour;
		  ope_modifiee -> mois = operation -> mois;
		  ope_modifiee -> annee = operation -> annee;

		  ope_modifiee -> date = g_date_new_dmy ( operation -> jour,
							  operation -> mois,
							  operation -> annee );

		  ope_modifiee -> jour_bancaire = operation -> jour_bancaire;
		  ope_modifiee -> mois_bancaire = operation -> mois_bancaire;
		  ope_modifiee -> annee_bancaire = operation -> annee_bancaire;

		  if ( operation -> jour_bancaire )
		    ope_modifiee -> date_bancaire = g_date_new_dmy ( operation -> jour_bancaire,
								     operation -> mois_bancaire,
								     operation -> annee_bancaire );

		  ope_modifiee -> no_compte = operation -> no_compte;
		  ope_modifiee -> devise = operation -> devise;
		  ope_modifiee -> une_devise_compte_egale_x_devise_ope = operation -> une_devise_compte_egale_x_devise_ope;
		  ope_modifiee -> taux_change = operation -> taux_change;
		  ope_modifiee -> frais_change = operation -> frais_change;
		  ope_modifiee -> tiers = operation -> tiers;
		  ope_modifiee -> type_ope = operation -> type_ope;
		  ope_modifiee -> pointe = operation -> pointe;
		  ope_modifiee -> auto_man = operation -> auto_man;
		  ope_modifiee -> no_rapprochement = operation -> no_rapprochement;

		  /* th�oriquement, cette ligne n'est pas n�cessaire vu que c'est une modif d'op� de ventil */

		  ope_modifiee -> no_operation_ventilee_associee = operation -> no_operation;

		  /* si cette op� de ventil est un virement, on met � jour la contre op�ration */

		  if ( ope_ventil -> relation_no_operation )
		    {
		      /*  soit c'�tait un virement, et on modifie l'op� associ�e */
		      /* soit c'est un nouveau virement, et on cr�e l'op� associ�e */

		      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + ope_ventil -> relation_no_compte;

		      MISE_A_JOUR = 1;

		      if ( ope_ventil -> relation_no_operation != -1 )
			{
			  /* c'�tait d�j� un virement vers ce compte */

			  tmp = g_slist_find_custom ( LISTE_OPERATIONS,
						      GINT_TO_POINTER ( ope_ventil -> relation_no_operation ),
						      (GCompareFunc) recherche_operation_par_no );

			  if ( tmp )
			    {
			      struct structure_operation *ope_modifiee_2;

			      ope_modifiee_2 = tmp -> data;

			      /* on commence par mettre le type choisi */

			      ope_modifiee_2 -> type_ope = ope_ventil -> no_type_associe;

			      ope_modifiee_2 -> montant = -ope_modifiee -> montant;
			      ope_modifiee_2 -> categorie = ope_modifiee -> categorie;
			      ope_modifiee_2 -> sous_categorie = ope_modifiee -> sous_categorie;

			      if ( ope_modifiee -> notes )
				ope_modifiee_2 -> notes = g_strdup ( ope_modifiee -> notes );

			      ope_modifiee_2 -> imputation = ope_modifiee -> imputation;
			      ope_modifiee_2 -> sous_imputation = ope_modifiee -> sous_imputation;

			      if ( ope_modifiee -> no_piece_comptable )
				ope_modifiee_2 -> no_piece_comptable = g_strdup ( ope_modifiee -> no_piece_comptable );

			      ope_modifiee_2 -> jour = ope_modifiee -> jour;
			      ope_modifiee_2 -> mois = ope_modifiee -> mois;
			      ope_modifiee_2 -> annee = ope_modifiee -> annee;
			  
			      ope_modifiee_2 -> date = g_date_new_dmy ( operation -> jour,
									operation -> mois,
									operation -> annee );

			      ope_modifiee_2 -> jour_bancaire = ope_modifiee -> jour_bancaire;
			      ope_modifiee_2 -> mois_bancaire = ope_modifiee -> mois_bancaire;
			      ope_modifiee_2 -> annee_bancaire = ope_modifiee -> annee_bancaire;

			      if ( operation -> jour_bancaire )
				ope_modifiee_2 -> date_bancaire = g_date_new_dmy ( operation -> jour_bancaire,
										   operation -> mois_bancaire,
										   operation -> annee_bancaire );

			      if ( ope_modifiee_2 -> devise != ope_modifiee -> devise)
				{
				  struct struct_devise *devise_compte_1;
				  struct struct_devise *devise_compte_2;

				  ope_modifiee_2 -> devise = ope_modifiee -> devise;

				  ope_modifiee_2 -> une_devise_compte_egale_x_devise_ope = ope_modifiee -> une_devise_compte_egale_x_devise_ope;

				  devise_compte_1 = g_slist_find_custom ( liste_struct_devises,
									  GINT_TO_POINTER ( ope_modifiee -> devise ),
									  ( GCompareFunc ) recherche_devise_par_no ) -> data;
				  devise_compte_2 = g_slist_find_custom ( liste_struct_devises,
									  GINT_TO_POINTER ( DEVISE ),
									  ( GCompareFunc ) recherche_devise_par_no ) -> data;

				  if ( !( ope_modifiee -> devise == DEVISE
					  ||
					  ( devise_compte_2 -> passage_euro && !strcmp ( devise_compte_1 -> nom_devise, _("Euro") ))
					  ||
					  ( !strcmp ( devise_compte_2 -> nom_devise, _("Euro") ) && devise_compte_1 -> passage_euro )))
				    {
				      /* c'est une devise �trang�re, on demande le taux de change et les frais de change */
	  
				      demande_taux_de_change ( devise_compte_2,
							       devise_compte_1,
							       1,
							       (gdouble ) 0,
							       (gdouble ) 0,
							       FALSE );

				      ope_modifiee_2 -> taux_change = taux_de_change[0];
				      ope_modifiee_2 -> frais_change = taux_de_change[1];

				      if ( ope_modifiee_2 -> taux_change < 0 )
					{
					  ope_modifiee_2 -> taux_change = -ope_modifiee_2 -> taux_change;
					  ope_modifiee_2 -> une_devise_compte_egale_x_devise_ope = 1;
					}
				    }
				}
/* 			      ope_modifiee_2 -> une_devise_compte_egale_x_devise_ope = ope_modifiee -> une_devise_compte_egale_x_devise_ope; */
/* 			      ope_modifiee_2 -> taux_change = ope_modifiee -> taux_change; */
/* 			      ope_modifiee_2 -> frais_change = ope_modifiee -> frais_change; */
			      ope_modifiee_2 -> tiers = ope_modifiee -> tiers;
			      ope_modifiee_2 -> auto_man = ope_modifiee -> auto_man;
			      ope_modifiee_2 -> no_exercice = ope_modifiee -> no_exercice;
			    }
			}
		      else
			{
			  /* c'est un nouveau virement, on doit cr�er l'op� associ�e */

			  struct structure_operation *nouvelle_ope_2;

			  nouvelle_ope_2 = calloc ( 1,
						    sizeof ( struct structure_operation ));

			  /* on commence par mettre le type choisi */

			  nouvelle_ope_2 -> type_ope = ope_ventil -> no_type_associe;
			  nouvelle_ope_2 -> no_compte = ope_ventil -> relation_no_compte;

			  nouvelle_ope_2 -> montant = -ope_modifiee ->  montant;
			  nouvelle_ope_2 -> categorie = ope_modifiee ->  categorie;
			  nouvelle_ope_2 -> sous_categorie = ope_modifiee ->  sous_categorie;

			  if ( ope_modifiee -> notes )
			    nouvelle_ope_2 -> notes = g_strdup ( ope_modifiee ->  notes );

			  nouvelle_ope_2 -> imputation = ope_modifiee ->  imputation;
			  nouvelle_ope_2 -> sous_imputation = ope_modifiee ->  sous_imputation;

			  if ( ope_modifiee -> no_piece_comptable )
			    nouvelle_ope_2 -> no_piece_comptable = g_strdup ( ope_modifiee ->  no_piece_comptable );

			  nouvelle_ope_2 -> jour = ope_modifiee ->  jour;
			  nouvelle_ope_2 -> mois = ope_modifiee ->  mois;
			  nouvelle_ope_2 -> annee = ope_modifiee ->  annee;
			  
			  nouvelle_ope_2 -> date = g_date_new_dmy ( operation -> jour,
								    operation -> mois,
								    operation -> annee );

			  nouvelle_ope_2 -> jour_bancaire = ope_modifiee ->  jour_bancaire;
			  nouvelle_ope_2 -> mois_bancaire = ope_modifiee ->  mois_bancaire;
			  nouvelle_ope_2 -> annee_bancaire = ope_modifiee ->  annee_bancaire;

			  if ( operation -> jour_bancaire )
			    nouvelle_ope_2 -> date_bancaire = g_date_new_dmy ( operation -> jour_bancaire,
									       operation -> mois_bancaire,
									       operation -> annee_bancaire );

			  nouvelle_ope_2 -> devise = ope_modifiee ->  devise;
			  nouvelle_ope_2 -> une_devise_compte_egale_x_devise_ope = ope_modifiee ->  une_devise_compte_egale_x_devise_ope;
			  nouvelle_ope_2 -> taux_change = ope_modifiee ->  taux_change;
			  nouvelle_ope_2 -> frais_change = ope_modifiee ->  frais_change;
			  nouvelle_ope_2 -> tiers = ope_modifiee ->  tiers;
			  nouvelle_ope_2 -> auto_man = ope_modifiee ->  auto_man;
			  nouvelle_ope_2 -> no_exercice = ope_modifiee ->  no_exercice;

			  ajout_operation ( nouvelle_ope_2 );

			  /* on met les relations */

			  ope_modifiee -> relation_no_operation = nouvelle_ope_2 -> no_operation;
			  ope_modifiee -> relation_no_compte = nouvelle_ope_2 -> no_compte;
			  nouvelle_ope_2 -> relation_no_operation = ope_modifiee ->  no_operation;
			  nouvelle_ope_2 -> relation_no_compte = ope_modifiee ->  no_compte;
			}
		    }
		}
	    }
	  else
	    {
	      /* c'est une nouvelle op�ration */
	      /*  on la cr�e, l'ajoute et si c'est un virement on cr�e la contre op�ration */

	      struct structure_operation *nouvelle_ope;

	      nouvelle_ope = calloc ( 1,
				      sizeof ( struct structure_operation ));

	      /* on r�cup�re d'abord les modifs de l'op� de ventil */

	      nouvelle_ope -> montant = ope_ventil -> montant;
	      nouvelle_ope -> categorie = ope_ventil -> categorie;
	      nouvelle_ope -> sous_categorie = ope_ventil -> sous_categorie;

	      if ( ope_ventil -> notes )
		nouvelle_ope -> notes = g_strdup ( ope_ventil -> notes );

	      nouvelle_ope -> imputation = ope_ventil -> imputation;
	      nouvelle_ope -> sous_imputation = ope_ventil -> sous_imputation;

	      if ( ope_ventil -> no_piece_comptable )
		nouvelle_ope -> no_piece_comptable = g_strdup ( ope_ventil -> no_piece_comptable );

	      nouvelle_ope -> no_exercice = ope_ventil -> no_exercice;

	      /* on r�cup�re ensuite les modifs de la ventilation */

	      nouvelle_ope -> jour = operation -> jour;
	      nouvelle_ope -> mois = operation -> mois;
	      nouvelle_ope -> annee = operation -> annee;

	      nouvelle_ope -> date = g_date_new_dmy ( operation -> jour,
						      operation -> mois,
						      operation -> annee );

	      nouvelle_ope -> jour_bancaire = operation -> jour_bancaire;
	      nouvelle_ope -> mois_bancaire = operation -> mois_bancaire;
	      nouvelle_ope -> annee_bancaire = operation -> annee_bancaire;

	      if ( operation -> jour_bancaire )
		nouvelle_ope -> date_bancaire = g_date_new_dmy ( operation -> jour_bancaire,
								 operation -> mois_bancaire,
								 operation -> annee_bancaire );

	      nouvelle_ope -> no_compte = operation -> no_compte;
	      nouvelle_ope -> devise = operation -> devise;
	      nouvelle_ope -> une_devise_compte_egale_x_devise_ope = operation -> une_devise_compte_egale_x_devise_ope;
	      nouvelle_ope -> taux_change = operation -> taux_change;
	      nouvelle_ope -> frais_change = operation -> frais_change;
	      nouvelle_ope -> tiers = operation -> tiers;
	      nouvelle_ope -> type_ope = operation -> type_ope;
	      nouvelle_ope -> pointe = operation -> pointe;
	      nouvelle_ope -> auto_man = operation -> auto_man;
	      nouvelle_ope -> no_rapprochement = operation -> no_rapprochement;

	      nouvelle_ope -> no_operation_ventilee_associee = operation -> no_operation;

	      /* on ajoute cette op� � la liste */

	      ajout_operation ( nouvelle_ope );

	      /* si cette op� de ventil est un virement, on cr�e la contre op�ration */

	      if ( ope_ventil -> relation_no_operation )
		{
		  struct structure_operation *nouvelle_ope_2;
		  struct struct_devise *devise_compte_1;
		  struct struct_devise *devise_compte_2;

		  nouvelle_ope_2 = calloc ( 1,
					    sizeof ( struct structure_operation ));

		  /* on commence par mettre le type choisi */

		  nouvelle_ope_2 -> type_ope = ope_ventil -> no_type_associe;
		  nouvelle_ope_2 -> no_compte = ope_ventil -> relation_no_compte;

		  nouvelle_ope_2 -> montant = -nouvelle_ope -> montant;
		  nouvelle_ope_2 -> categorie = nouvelle_ope -> categorie;
		  nouvelle_ope_2 -> sous_categorie = nouvelle_ope -> sous_categorie;

		  if ( nouvelle_ope -> notes )
		    nouvelle_ope_2 -> notes = g_strdup ( nouvelle_ope -> notes );

		  nouvelle_ope_2 -> imputation = nouvelle_ope -> imputation;
		  nouvelle_ope_2 -> sous_imputation = nouvelle_ope -> sous_imputation;

		  if ( nouvelle_ope -> no_piece_comptable )
		    nouvelle_ope_2 -> no_piece_comptable = g_strdup ( nouvelle_ope -> no_piece_comptable );

		  nouvelle_ope_2 -> jour = nouvelle_ope -> jour;
		  nouvelle_ope_2 -> mois = nouvelle_ope -> mois;
		  nouvelle_ope_2 -> annee = nouvelle_ope -> annee;
			  
		  nouvelle_ope_2 -> date = g_date_new_dmy ( operation -> jour,
							    operation -> mois,
							    operation -> annee );

		  nouvelle_ope_2 -> jour_bancaire = nouvelle_ope -> jour_bancaire;
		  nouvelle_ope_2 -> mois_bancaire = nouvelle_ope -> mois_bancaire;
		  nouvelle_ope_2 -> annee_bancaire = nouvelle_ope -> annee_bancaire;
 		  nouvelle_ope_2 -> devise = nouvelle_ope -> devise;

		  if ( operation -> jour_bancaire )
		    nouvelle_ope_2 -> date_bancaire = g_date_new_dmy ( operation -> jour_bancaire,
								       operation -> mois_bancaire,
								       operation -> annee_bancaire );

		  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + nouvelle_ope_2 -> no_compte;
		  
		  nouvelle_ope_2 -> une_devise_compte_egale_x_devise_ope = nouvelle_ope -> une_devise_compte_egale_x_devise_ope;

		  devise_compte_1 = g_slist_find_custom ( liste_struct_devises,
							  GINT_TO_POINTER ( nouvelle_ope -> devise ),
							  ( GCompareFunc ) recherche_devise_par_no ) -> data;
		  devise_compte_2 = g_slist_find_custom ( liste_struct_devises,
							  GINT_TO_POINTER ( DEVISE ),
							  ( GCompareFunc ) recherche_devise_par_no ) -> data;

		  if ( !( nouvelle_ope -> devise == DEVISE
			  ||
			  ( devise_compte_2 -> passage_euro && !strcmp ( devise_compte_1 -> nom_devise, _("Euro") ))
			  ||
			  ( !strcmp ( devise_compte_2 -> nom_devise, _("Euro") ) && devise_compte_1 -> passage_euro )))
		    {
		      /* c'est une devise �trang�re, on demande le taux de change et les frais de change */
	  
		      demande_taux_de_change ( devise_compte_2, devise_compte_1, 1,
					       (gdouble ) 0, (gdouble ) 0, FALSE );

		      nouvelle_ope_2 -> taux_change = taux_de_change[0];
		      nouvelle_ope_2 -> frais_change = taux_de_change[1];

		      if ( nouvelle_ope_2 -> taux_change < 0 )
			{
			  nouvelle_ope_2 -> taux_change = -nouvelle_ope_2 -> taux_change;
			  nouvelle_ope_2 -> une_devise_compte_egale_x_devise_ope = 1;
			}
		    }

/* 		  nouvelle_ope_2 -> taux_change = nouvelle_ope -> taux_change; */
/* 		  nouvelle_ope_2 -> frais_change = nouvelle_ope -> frais_change; */

		  nouvelle_ope_2 -> tiers = nouvelle_ope -> tiers;
		  nouvelle_ope_2 -> auto_man = nouvelle_ope -> auto_man;
		  nouvelle_ope_2 -> no_exercice = nouvelle_ope -> no_exercice;

		  ajout_operation ( nouvelle_ope_2 );

		  /* on met les relations */

		  nouvelle_ope -> relation_no_operation = nouvelle_ope_2 -> no_operation;
		  nouvelle_ope -> relation_no_compte = nouvelle_ope_2 -> no_compte;
		  nouvelle_ope_2 -> relation_no_operation = nouvelle_ope -> no_operation;
		  nouvelle_ope_2 -> relation_no_compte = nouvelle_ope -> no_compte;

		}
	    }
	}
      liste_struct_ventilations = liste_struct_ventilations -> next;
    }
}
/***********************************************************************************************************/
