/* ************************************************************************** */
/* Ce fichier s'occupe de la gestion du formulaire de saisie des op�rations   */
/* 			formulaire.c                                          */
/*                                                                            */
/*     Copyright (C)	2000-2003 C�dric Auger (cedric@grisbi.org)	      */
/*			2003-2004 Benjamin Drieu (bdrieu@april.org)	      */
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


#include "include.h"
#include "variables-extern.c"
#include "structures.h"
#include "operations_formulaire.h"



#include "accueil.h"
#include "calendar.h"
#include "categories_onglet.h"
#include "constants.h"
#include "devises.h"
#include "dialog.h"
#include "equilibrage.h"
#include "etats_calculs.h"
#include "exercice.h"
#include "imputation_budgetaire.h"
#include "operations_classement.h"
#include "operations_liste.h"
#include "search_glist.h"
#include "tiers_onglet.h"
#include "traitement_variables.h"
#include "type_operations.h"
#include "utils.h"
#include "ventilation.h"


extern GtkWidget *tree_view_listes_operations;
extern gint hauteur_ligne_liste_opes;
extern GSList *list_store_comptes;
extern gint enregistre_ope_au_retour;
extern gint ligne_selectionnee_ventilation;


/******************************************************************************/
/*  Routine qui cr�e le formulaire et le renvoie                              */
/******************************************************************************/
GtkWidget *creation_formulaire ( void )
{
    GtkWidget *table;
    GtkWidget *menu;
    GtkWidget *bouton;
    GdkColor couleur_normale;
    GdkColor couleur_grise;
    GtkTooltips *tips;

    /* On cr�e tout de suite les styles qui seront appliqu�s aux entr�es du formulaire : */
    /*     style_entree_formulaire[ENCLAIR] sera la couleur noire, normale */
    /*     style_entree_formulaire[ENGRIS] sera une couleur att�nu�e quand le formulaire est vide */

    couleur_normale.red = COULEUR_NOIRE_RED;
    couleur_normale.green = COULEUR_NOIRE_GREEN;
    couleur_normale.blue = COULEUR_NOIRE_BLUE;

    couleur_grise.red = COULEUR_GRISE_RED;
    couleur_grise.green = COULEUR_GRISE_GREEN;
    couleur_grise.blue = COULEUR_GRISE_BLUE;

    /* on cr�e le tooltips */

    tips = gtk_tooltips_new ();

    /* le formulaire est une vbox avec en haut un tableau de 6 colonnes, et */
    /* en bas si demand� les boutons valider et annuler */

    formulaire = gtk_vbox_new ( FALSE, 5 );

    style_entree_formulaire[ENCLAIR] = gtk_style_new();
    style_entree_formulaire[ENCLAIR] -> text[GTK_STATE_NORMAL] = couleur_normale;

    style_entree_formulaire[ENGRIS] = gtk_style_new();
    style_entree_formulaire[ENGRIS] -> text[GTK_STATE_NORMAL] = couleur_grise;

    g_object_ref ( style_entree_formulaire[ENCLAIR] );
    g_object_ref ( style_entree_formulaire[ENGRIS] );

    /* le formulaire est une table de 4 lignes et 7 colonnes */

    table = gtk_table_new ( 4, 7, FALSE );

    gtk_table_set_col_spacings ( GTK_TABLE ( table ),
				 10 );
    gtk_box_pack_start ( GTK_BOX ( formulaire ),
			 table,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( table );

    /* met la taille du formulaire � 1 au d�but pour �viter un agrandissement
       automatique de la fen�tre d� aux tailles par d�faut des entr�es.
       Cette taille sera modifi�e automatiquement � l'affichage. */

    gtk_widget_set_usize ( GTK_WIDGET ( table ),
			   1,
			   FALSE );

    /* no d'op� */

    widget_formulaire_operations[TRANSACTION_FORM_OP_NB] = gtk_label_new ( "" );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_OP_NB],
		       0, 1, 0, 1,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    if ( etat.affiche_no_operation )
	gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_OP_NB] );

    /* entr�e de la date */

    widget_formulaire_operations[TRANSACTION_FORM_DATE] = gtk_entry_new();
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_DATE],
		       1, 2, 0, 1,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_DATE ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_DATE ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ),
			 "focus-in-event",
			 GTK_SIGNAL_FUNC ( entree_prend_focus ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ),
			 "focus-out-event",
			 GTK_SIGNAL_FUNC ( entree_perd_focus ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_DATE ) );
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_DATE] );

    /*  entr�e du tiers : c'est une combofix */

    creation_liste_tiers_combofix();
    widget_formulaire_operations[TRANSACTION_FORM_PARTY] = gtk_combofix_new_complex ( liste_tiers_combofix,
										      FALSE,
										      TRUE,
										      TRUE,
										      0 );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_PARTY],
		       2, 3, 0, 1,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ) -> entry ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_PARTY ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ) -> arrow ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_PARTY ) );
    gtk_signal_connect_object ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ) -> entry ),
				"focus-in-event",
				GTK_SIGNAL_FUNC ( entree_prend_focus ),
				GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ));
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ) -> entry ),
			 "focus-out-event",
			 GTK_SIGNAL_FUNC ( entree_perd_focus ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_PARTY ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ) -> entry ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_PARTY ) );
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] );

    /*  Affiche le d�bit */

    widget_formulaire_operations[TRANSACTION_FORM_DEBIT] = gtk_entry_new ();
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_DEBIT],
		       3, 4, 0, 1,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_DEBIT ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_DEBIT ) );
    gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ),
			       "focus-in-event",
			       GTK_SIGNAL_FUNC ( entree_prend_focus ),
			       NULL );
    gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ),
			       "focus-out-event",
			       GTK_SIGNAL_FUNC ( entree_perd_focus ),
			       GINT_TO_POINTER ( TRANSACTION_FORM_DEBIT ) );
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] );

    /*  Affiche le cr�dit */

    widget_formulaire_operations[TRANSACTION_FORM_CREDIT] = gtk_entry_new ();
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_CREDIT],
		       4, 5, 0, 1,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_CREDIT ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_CREDIT ) );
    gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ),
			       "focus-in-event",
			       GTK_SIGNAL_FUNC ( entree_prend_focus ),
			       NULL );
    gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ),
			       "focus-out-event",
			       GTK_SIGNAL_FUNC ( entree_perd_focus ),
			       GINT_TO_POINTER ( TRANSACTION_FORM_CREDIT ) );
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] );

    /* met l'option menu des devises */

    widget_formulaire_operations[TRANSACTION_FORM_DEVISE] = gtk_option_menu_new ();
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			   widget_formulaire_operations[TRANSACTION_FORM_DEVISE],
			   _("Choose currency"),
			   _("Choose currency") );
    menu = creation_option_menu_devises ( -1,
					  liste_struct_devises );
    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_DEVISE] ),
			       menu );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_DEVISE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_DEVISE ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_DEVISE],
		       5, 6, 0, 1,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_DEVISE] );

    /* mise en forme du bouton change */

    widget_formulaire_operations[TRANSACTION_FORM_CHANGE] = gtk_button_new_with_label ( _("Change") );
    gtk_button_set_relief ( GTK_BUTTON ( widget_formulaire_operations[TRANSACTION_FORM_CHANGE] ),
			    GTK_RELIEF_NONE );
    gtk_signal_connect ( GTK_OBJECT (  widget_formulaire_operations[TRANSACTION_FORM_CHANGE] ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( click_sur_bouton_voir_change ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CHANGE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_CHANGE ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_CHANGE],
		       6, 7, 0, 1,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_CHANGE] );

    /* mise en place de la date de valeur */

    widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] = gtk_entry_new();
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE],
		       1, 2, 1, 2,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_VALUE_DATE )  );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_VALUE_DATE )  );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ),
			 "focus-in-event",
			 GTK_SIGNAL_FUNC ( entree_prend_focus ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ),
			 "focus-out-event",
			 GTK_SIGNAL_FUNC ( entree_perd_focus ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_VALUE_DATE ) );
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] );

    gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE],
			       etat.affiche_date_bancaire );

    /*  Affiche les cat�gories / sous-cat�gories */

    widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] = gtk_combofix_new_complex ( liste_categories_combofix,
											 FALSE,
											 TRUE,
											 TRUE,
											 0 );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_CATEGORY],
		       2, 3, 1, 2,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ) -> entry ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_CATEGORY ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ) -> arrow ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_CATEGORY ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ) -> entry ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_CATEGORY ) );
    gtk_signal_connect_object ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ) -> entry ),
				"focus-in-event",
				GTK_SIGNAL_FUNC ( entree_prend_focus ),
				GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ));
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ) -> entry ),
			 "focus-out-event",
			 GTK_SIGNAL_FUNC ( entree_perd_focus ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_CATEGORY ) );
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] );

    /* cr�ation de l'entr�e du ch�que, non affich�e pour le moment */
    /* � cr�er avant l'option menu du type d'op� */

    widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] = gtk_entry_new();
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_CHEQUE ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_CHEQUE ) );
    gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ),
			       "focus-in-event",
			       GTK_SIGNAL_FUNC ( entree_prend_focus ),
			       NULL );
    gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ),
			       "focus-out-event",
			       GTK_SIGNAL_FUNC ( entree_perd_focus ),
			       GINT_TO_POINTER ( TRANSACTION_FORM_CHEQUE ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_CHEQUE],
		       5, 7, 1, 2,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);

    /* Affiche l'option menu des types */

    widget_formulaire_operations[TRANSACTION_FORM_TYPE] = gtk_option_menu_new ();
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			   widget_formulaire_operations[TRANSACTION_FORM_TYPE],
			   _("Choose the method of payment"),
			   _("Choose the method of payment") );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_TYPE ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_TYPE],
		       3, 5, 1, 2,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);

    /* le menu par d�faut est celui des d�bits */

    if ( ( menu = creation_menu_types ( 1, compte_courant, 0 ) ) )
    {
	p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

	gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
				   menu );
	gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
				      cherche_no_menu_type ( TYPE_DEFAUT_DEBIT ) );
	gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] );
    }

    /* met l'option menu de l'exercice */

    widget_formulaire_operations[TRANSACTION_FORM_EXERCICE] = gtk_option_menu_new ();
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			   widget_formulaire_operations[TRANSACTION_FORM_EXERCICE],
			   _("Choose the financial year"),
			   _("Choose the financial year") );
    menu = gtk_menu_new ();
    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_EXERCICE] ),
			       creation_menu_exercices (0) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_EXERCICE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_EXERCICE ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_EXERCICE],
		       0, 2, 2, 3,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_EXERCICE] );

    gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_EXERCICE],
			       etat.utilise_exercice );

    /* Affiche l'imputation budg�taire */

    widget_formulaire_operations[TRANSACTION_FORM_BUDGET] = gtk_combofix_new_complex ( liste_imputations_combofix,
										       FALSE,
										       TRUE,
										       TRUE,
										       0 );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_BUDGET],
		       2, 3, 2, 3,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] ) -> entry ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_BUDGET ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] ) -> arrow ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_BUDGET ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] ) -> entry ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_BUDGET ) );
    gtk_signal_connect_object ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] ) -> entry ),
				"focus-in-event",
				GTK_SIGNAL_FUNC ( entree_prend_focus ),
				GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] ) -> entry ),
			 "focus-out-event",
			 GTK_SIGNAL_FUNC ( entree_perd_focus ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_BUDGET ) );

    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] );

    gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET],
			       etat.utilise_imputation_budgetaire );

    /* mise en place du type associ� lors d'un virement */
    /* non affich� au d�part et pas de menu au d�part */

    widget_formulaire_operations[TRANSACTION_FORM_CONTRA] = gtk_option_menu_new ();
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			   widget_formulaire_operations[TRANSACTION_FORM_CONTRA],
			   _("Contra-transaction method of payment"),
			   _("Contra-transaction method of payment") );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_CONTRA ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_CONTRA],
		       3, 5, 2, 3,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);

    /* cr�ation de l'entr�e du no de pi�ce comptable */

    widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] = gtk_entry_new();
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_VOUCHER],
		       5, 7, 2, 3,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_VOUCHER ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_VOUCHER ) );
    gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] ),
			       "focus-in-event",
			       GTK_SIGNAL_FUNC ( entree_prend_focus ),
			       NULL );
    gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] ),
			       "focus-out-event",
			       GTK_SIGNAL_FUNC ( entree_perd_focus ),
			       GINT_TO_POINTER ( TRANSACTION_FORM_VOUCHER ) );
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] );

    gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER],
			       etat.utilise_piece_comptable );

    /* mise en forme du bouton ventilation */

    widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN] = gtk_button_new_with_label ( _("Breakdown") );
    gtk_button_set_relief ( GTK_BUTTON ( widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN] ),
			    GTK_RELIEF_NONE );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN] ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( basculer_vers_ventilation ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_BREAKDOWN ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN],
		       0, 2, 3, 4,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN] );

    /* Affiche les notes */

    widget_formulaire_operations[TRANSACTION_FORM_NOTES] = gtk_entry_new ();
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_NOTES],
		       2, 3, 3, 4,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_NOTES] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_NOTES ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_NOTES] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_NOTES ) );
    gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_NOTES] ),
			       "focus-in-event",
			       GTK_SIGNAL_FUNC ( entree_prend_focus ),
			       NULL );
    gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_NOTES] ),
			       "focus-out-event",
			       GTK_SIGNAL_FUNC ( entree_perd_focus ),
			       GINT_TO_POINTER ( TRANSACTION_FORM_NOTES ) );
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_NOTES] );

    /* Affiche les infos banque/guichet */

    widget_formulaire_operations[TRANSACTION_FORM_BANK] = gtk_entry_new ();
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_BANK],
		       3, 6, 3, 4,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_BANK] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_BANK ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_BANK] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( touches_champ_formulaire ),
			 GINT_TO_POINTER ( TRANSACTION_FORM_BANK ) );
    gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_BANK] ),
			       "focus-in-event",
			       GTK_SIGNAL_FUNC ( entree_prend_focus ),
			       NULL );
    gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_BANK] ),
			       "focus-out-event",
			       GTK_SIGNAL_FUNC ( entree_perd_focus ),
			       GINT_TO_POINTER ( TRANSACTION_FORM_BANK ) );
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_BANK] );

    gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_BANK],
			       etat.utilise_info_banque_guichet );

    /*  Affiche le mode automatique / manuel  */

    widget_formulaire_operations[TRANSACTION_FORM_MODE] = gtk_label_new ( "" );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_operations[TRANSACTION_FORM_MODE],
		       6, 7, 3, 4,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_MODE] );

    /* s�paration d'avec les boutons */

    separateur_formulaire_operations = gtk_hseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( formulaire ),
			 separateur_formulaire_operations,
			 FALSE,
			 FALSE,
			 0 );
    if ( etat.affiche_boutons_valider_annuler )
	gtk_widget_show ( separateur_formulaire_operations );

    /* mise en place des boutons */

    hbox_valider_annuler_ope = gtk_hbox_new ( FALSE,
					      5 );
    gtk_box_pack_start ( GTK_BOX ( formulaire ),
			 hbox_valider_annuler_ope,
			 FALSE,
			 FALSE,
			 0 );
    if ( etat.affiche_boutons_valider_annuler )
	gtk_widget_show ( hbox_valider_annuler_ope );

    bouton = gtk_button_new_from_stock ( GTK_STOCK_OK );
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( fin_edition ),
			 NULL );
    gtk_box_pack_end ( GTK_BOX ( hbox_valider_annuler_ope ),
		       bouton,
		       FALSE,
		       FALSE,
		       0 );
    gtk_widget_show ( bouton );

    bouton = gtk_button_new_from_stock ( GTK_STOCK_CANCEL );
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( echap_formulaire ),
			 NULL );
    gtk_box_pack_end ( GTK_BOX ( hbox_valider_annuler_ope ),
		       bouton,
		       FALSE,
		       FALSE,
		       0 );
    gtk_widget_show ( bouton );

    /* aucune op�ration associ�e au formulaire */

    gtk_object_set_data ( GTK_OBJECT ( table ),
			  "adr_struct_ope",
			  NULL );
    gtk_object_set_data ( GTK_OBJECT ( table ),
			  "liste_adr_ventilation",
			  NULL );

    return ( formulaire );
}
/******************************************************************************/

/******************************************************************************/
void echap_formulaire ( void )
{
    GSList *liste_tmp;

    /* si c'est une nouvelle op� ventil�e et qu'on a utilis� la compl�tion */
    /* il faut effacer les op�s de ventilation automatiquement cr��es */
    /* celles ci sont dans la liste dans "liste_adr_ventilation" */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

    liste_tmp = gtk_object_get_data ( GTK_OBJECT ( formulaire ),
				      "liste_adr_ventilation" );

    if ( liste_tmp
	 &&
	 !gtk_object_get_data ( GTK_OBJECT ( formulaire ),
				"adr_struct_ope" )
	 &&
	 liste_tmp != GINT_TO_POINTER ( -1 ))
    {
	while ( liste_tmp )
	{
	    ligne_selectionnee_ventilation = cherche_ligne_from_operation_ventilee (liste_tmp -> data);
	    supprime_operation_ventilation ();
	    liste_tmp = liste_tmp -> next;
	}
    }

    formulaire_a_zero();

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

    if ( !etat.formulaire_toujours_affiche )
	gtk_widget_hide ( frame_droite_bas );

    gtk_widget_grab_focus ( tree_view_listes_operations );

}
/******************************************************************************/

/******************************************************************************/
/* Fonction appel�e quand une entry prend le focus */
/* si elle contient encore des �l�ments gris�s, on les enl�ve */
/******************************************************************************/
gboolean entree_prend_focus ( GtkWidget *entree )
{
    /* si le style est le gris, on efface le contenu de l'entr�e, sinon on fait rien */

    if ( GTK_IS_COMBOFIX ( entree ) )
    {
	if ( gtk_widget_get_style ( GTK_COMBOFIX ( entree ) -> entry ) == style_entree_formulaire[ENGRIS] )
	{
	    gtk_combofix_set_text ( GTK_COMBOFIX ( entree ), "" );
	    gtk_widget_set_style ( GTK_COMBOFIX ( entree ) -> entry, style_entree_formulaire[ENCLAIR] );
	}
    }
    else
    {
	if ( gtk_widget_get_style ( entree ) == style_entree_formulaire[ENGRIS] )
	{
	    gtk_entry_set_text ( GTK_ENTRY ( entree ), "" );
	    gtk_widget_set_style ( entree, style_entree_formulaire[ENCLAIR] );
	}
    }
    return FALSE;
}
/******************************************************************************/

/******************************************************************************/
/* Fonction appel�e quand une entry perd le focus */
/* si elle ne contient rien, on remet la fonction en gris */
/******************************************************************************/
gboolean entree_perd_focus ( GtkWidget *entree,
			     GdkEventFocus *ev,
			     gint *no_origine )
{
    gchar *texte;

    texte = NULL;
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

    switch ( GPOINTER_TO_INT ( no_origine ) )
    {
	/* on sort de la date, soit c'est vide, soit on la v�rifie,
	   la compl�te si n�cessaire et met � jour l'exercice */
	case TRANSACTION_FORM_DATE :

	    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
	    {
		modifie_date ( entree );

		/* si c'est une modif d'op�, on ne change pas l'exercice */

		if ( !gtk_object_get_data ( GTK_OBJECT ( formulaire ),
					    "adr_struct_ope" ))
		    affiche_exercice_par_date( widget_formulaire_operations[TRANSACTION_FORM_DATE],
					       widget_formulaire_operations[TRANSACTION_FORM_EXERCICE] );
	    }
	    else
		texte = _("Date");
	    break;

	    /* on sort du tiers : soit vide soit compl�te le reste de l'op� */

	case TRANSACTION_FORM_PARTY :

	    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		completion_operation_par_tiers ();
	    else
		texte = _("Third party");
	    break;

	    /* on sort du d�bit : soit vide, soit change le menu des types s'il ne correspond pas */

	case TRANSACTION_FORM_DEBIT :

	    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
	    {
		/* on commence par virer ce qu'il y avait dans les cr�dits */

		if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ) == style_entree_formulaire[ENCLAIR] )
		{
		    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ),
					 "" );
		    entree_perd_focus ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT],
					NULL,
					GINT_TO_POINTER ( TRANSACTION_FORM_CREDIT ) );
		}

		/* si c'est un menu de cr�dit, on y met le menu de d�bit, sauf si tous les types sont affich�s */

		p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

		if ( ( !etat.affiche_tous_les_types
		       &&
		       GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] )
		       &&
		       GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ) -> menu ),
							       "signe_menu" ))
		       ==
		       2 )
		     ||
		     !GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ))
		{
		    /* on cr�e le nouveau menu et on met le d�faut */

		    GtkWidget *menu;

		    if ( ( menu = creation_menu_types ( 1, compte_courant, 0  )))
		    {
			gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
						   menu );
			gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
						      cherche_no_menu_type ( TYPE_DEFAUT_DEBIT ) );
			gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] );
		    }
		    else
		    {
			gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] );
			gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] );
		    }

		    /* comme il y a eu un changement de signe, on change aussi le type de l'op� associ�e */
		    /* s'il est affich� */

		    if ( GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] )
			 &&
			 GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ) -> menu ),
								 "signe_menu" ))
			 ==
			 1 )
		    {
			GtkWidget *menu;

			menu = creation_menu_types ( 2,
						     GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ),
											     "compte_virement" )),
						     2  );

			if ( menu )
			    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ),
						       menu );
			else
			    gtk_option_menu_remove_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ));
		    }
		}
		else
		{
		    /* on n'a pas recr�� de menu, donc soit c'est d�j� un menu de d�bit, soit tous les types */
		    /* sont affich�s, soit le widget n'est pas visible */
		    /* on met donc le d�faut, sauf si il y a qque chose dans les categ ou que le widget n'est pas visible */


		    if ( GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ) &&
			 gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ) == style_entree_formulaire[ENGRIS] )
			gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
						      cherche_no_menu_type ( TYPE_DEFAUT_DEBIT ) );
		}
	    }
	    else
		texte = _("Debit");
	    break;

	    /* on sort du cr�dit : soit vide, soit change le menu des types
	       s'il n'y a aucun tiers ( <=> nouveau tiers ) */

	case TRANSACTION_FORM_CREDIT :

	    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
	    {
		/* on commence par virer ce qu'il y avait dans les d�bits */

		if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ) == style_entree_formulaire[ENCLAIR] )
		{
		    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ),
					 "" );
		    entree_perd_focus ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT],
					NULL,
					GINT_TO_POINTER ( TRANSACTION_FORM_DEBIT ));
		}

		/* si c'est un menu de cr�dit, on y met le menu de d�bit,
		   sauf si tous les types sont affich�s */

		p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

		if ( ( !etat.affiche_tous_les_types &&
		       GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ) &&
		       GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ) -> menu ),
							       "signe_menu" )) == 1 ) ||
		     !GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ))
		{
		    /* on cr�e le nouveau menu et on met le d�faut */

		    GtkWidget *menu;

		    if ( ( menu = creation_menu_types ( 2, compte_courant, 0  ) ) )
		    {
			gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
						   menu );
			gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
						      cherche_no_menu_type ( TYPE_DEFAUT_CREDIT ) );
			gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] );
		    }
		    else
		    {
			gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] );
			gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] );
		    }

		    /* comme il y a eu un changement de signe, on change aussi le type de l'op� associ�e */
		    /* s'il est affich� */

		    if ( GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ) &&
			 GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ) -> menu ),
								 "signe_menu" )) == 2 )
		    {
			GtkWidget *menu;

			menu = creation_menu_types ( 1,
						     GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ),
											     "compte_virement" )),
						     2  );

			if ( menu )
			    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ),
						       menu );
			else
			    gtk_option_menu_remove_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ));
		    }
		}
		else
		{
		    /* on n'a pas recr�� de menu, donc soit c'est d�j� un menu de d�bit, soit tous les types */
		    /* sont affich�s, soit le widget n'est pas visible */
		    /* on met donc le d�faut, sauf si il y a qque chose dans les categ ou que le widget n'est pas visible */


		    if ( GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ) &&
			 gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ) == style_entree_formulaire[ENGRIS] )
			gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
						      cherche_no_menu_type ( TYPE_DEFAUT_CREDIT ) );
		}
	    }
	    else
		texte = _("Credit");
	    break;

	case TRANSACTION_FORM_VALUE_DATE :
	    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		modifie_date ( entree );
	    else
		texte = _("Value date");
	    break;

	    /* sort des catégories : si c'est une op� ventil�e, affiche le bouton de ventilation */
	    /* si c'est un virement affiche le bouton des types de l'autre compte */

	case TRANSACTION_FORM_CATEGORY :

	    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
	    {
		if ( strcmp ( g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ))),
			      _("Breakdown of transaction") ))
		{
		    gchar **tableau_char;

		    gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN] );
		    gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_EXERCICE],
					       TRUE );
		    gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET],
					       TRUE );

		    /* v�rification que ce n'est pas un virement */

		    tableau_char = g_strsplit ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] )),
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
			     && strlen ( tableau_char[1] ) )
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

				    if ( !GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] )
					 ||
					 ( GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ),
										   "no_compte" ))
					   !=
					   compte_virement ))
				    {
					/* v�rifie quel est le montant entr�, affiche les types oppos�s de l'autre compte */

					GtkWidget *menu;

					if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ) == style_entree_formulaire[ENCLAIR] )
					    /* il y a un montant dans le cr�dit */
					    menu = creation_menu_types ( 1, compte_virement, 2  );
					else
					    /* il y a un montant dans le d�bit ou d�faut */
					    menu = creation_menu_types ( 2, compte_virement, 2  );

					/* si un menu � �t� cr��, on l'affiche */

					if ( menu )
					{
					    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ),
								       menu );
					    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] );
					}

					/* on associe le no de compte de virement au formulaire pour le retrouver */
					/* rapidement s'il y a un chgt d�bit/cr�dit */

					gtk_object_set_data ( GTK_OBJECT ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ),
							      "compte_virement",
							      GINT_TO_POINTER ( compte_virement ));
				    }
				}
				else
				    gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] );
			    }
			    else
				gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] );
			}
			else
			    gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] );
		    }
		    else
			gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] );

		    g_strfreev ( tableau_char );
		}
		else
		{
		    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN] );
		    gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_EXERCICE],
					       FALSE );
		    gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET],
					       FALSE );
		}
	    }
	    else
		texte = _("Categories : Sub-categories");

	    break;

	case TRANSACTION_FORM_CHEQUE :

	    if ( !strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		texte = _("Cheque/Transfer number");
	    break;

	case TRANSACTION_FORM_BUDGET :

	    if ( !strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		texte = _("Budgetary line");
	    break;

	case TRANSACTION_FORM_VOUCHER :

	    if ( !strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		texte = _("Voucher");

	    break;

	case TRANSACTION_FORM_NOTES :

	    if ( !strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		texte = _("Notes");
	    break;

	case TRANSACTION_FORM_BANK :

	    if ( !strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		texte = _("Bank references");
	    break;

	default :
	    break;
    }

    /* l'entr�e �tait vide, on remet le d�faut */
    /* si l'origine �tait un combofix, il faut remettre le texte */
    /* avec le gtk_combofix (sinon risque de compl�tion), donc utiliser l'origine */

    if ( texte )
    {
	switch ( GPOINTER_TO_INT ( no_origine ))
	{
	    case TRANSACTION_FORM_PARTY :
	    case TRANSACTION_FORM_CATEGORY :
	    case TRANSACTION_FORM_BUDGET :
		gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[GPOINTER_TO_INT ( no_origine )] ),
					texte );
		break;

	    default:

		gtk_entry_set_text ( GTK_ENTRY ( entree ), texte );
		break;
	}
	gtk_widget_set_style ( entree, style_entree_formulaire[ENGRIS] );
    }
    return FALSE;
}
/******************************************************************************/

/******************************************************************************/
gboolean clique_champ_formulaire ( GtkWidget *entree,
				   GdkEventButton *ev,
				   gint *no_origine )
{
    GtkWidget *popup_cal;

    /* on rend sensitif tout ce qui ne l'�tait pas sur le formulaire */

    degrise_formulaire_operations ();

    /* si l'entr�e de la date est grise, on met la date courante seulement
       si la date r�elle est grise aussi. Dans le cas contraire,
       c'est elle qui prend le focus */

    if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) == style_entree_formulaire[ENGRIS] )
    {
	if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ) == style_entree_formulaire[ENGRIS] )
	{
	    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ),
				 gsb_today() );
	    gtk_widget_set_style ( widget_formulaire_operations[TRANSACTION_FORM_DATE],
				   style_entree_formulaire[ENCLAIR] );
	}
    }

    /* si ev est null ( cad que �a ne vient pas d'un click mais appel� par ex
       � la fin de fin_edition ), on se barre */

    if ( !ev )
	return FALSE;

    /* �num�ration suivant l'entr�e o� on clique */

    switch ( GPOINTER_TO_INT ( no_origine ) )
    {
	case TRANSACTION_FORM_DATE :
	case TRANSACTION_FORM_VALUE_DATE :

	    /* si double click, on popup le calendrier */
	    if ( ev -> type == GDK_2BUTTON_PRESS )
	    {
		gtk_signal_emit_stop_by_name ( GTK_OBJECT ( entree ),
					       "button-press-event" );
		popup_cal = gsb_calendar_new ( entree );
		gtk_signal_connect_object ( GTK_OBJECT ( popup_cal ),
					    "destroy",
					    GTK_SIGNAL_FUNC ( ferme_calendrier ),
					    GTK_OBJECT ( entree ) );
		gtk_widget_grab_focus ( GTK_WIDGET ( popup_cal ) );
	    }
	    break;

	default :

	    break;
    }
    return FALSE;
}
/******************************************************************************/

/******************************************************************************/
gboolean touches_champ_formulaire ( GtkWidget *widget,
				    GdkEventKey *ev,
				    gint *no_origine )
{
    gint origine;
    GtkWidget *popup_cal;

    origine = GPOINTER_TO_INT ( no_origine );

    /* si etat.entree = 1, la touche entr�e finit l'op�ration ( fonction par d�faut ) */
    /* sinon elle fait comme tab */

    if ( !etat.entree && ( ev -> keyval == GDK_Return || ev -> keyval == GDK_KP_Enter ))
	ev->keyval = GDK_Tab ;

    switch ( ev -> keyval )
    {
	case GDK_Escape :		/* �chap */

	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
	    echap_formulaire();
	    break;

	case GDK_Tab :		/* tabulation */

	    /* une tabulation passe au widget affich� suivant */
	    /* et retourne � la date ou enregistre l'op� s'il est � la fin */

	    gtk_signal_emit_stop_by_name ( GTK_OBJECT ( widget ),
					   "key-press-event");

	    /* on efface la s�lection en cours si c'est une entr�e ou un combofix */

	    if ( GTK_IS_ENTRY ( widget ))
		gtk_entry_select_region ( GTK_ENTRY ( widget ), 0, 0);
	    else
		if ( GTK_IS_COMBOFIX ( widget ))
		    gtk_entry_select_region ( GTK_ENTRY ( GTK_COMBOFIX ( widget ) -> entry ), 0, 0);

	    /* on fait perdre le focus au widget courant pour faire les changements automatiques si n�cessaire */

	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

	    /* on donne le focus au widget suivant */

	    origine = ( origine + 1 ) % ( TRANSACTION_FORM_WIDGET_NB -1 );

	    while ( !( GTK_WIDGET_VISIBLE ( widget_formulaire_operations[origine] ) &&
		       GTK_WIDGET_SENSITIVE ( widget_formulaire_operations[origine] ) &&
		       ( GTK_IS_COMBOFIX ( widget_formulaire_operations[origine] ) ||
			 GTK_IS_ENTRY ( widget_formulaire_operations[origine] ) ||
			 GTK_IS_BUTTON ( widget_formulaire_operations[origine] ) )))
		origine = ( origine + 1 ) % ( TRANSACTION_FORM_WIDGET_NB -1 );

	    /* si on se retrouve sur la date et que etat.entree = 0, on enregistre l'op�ration */

	    if ( origine == TRANSACTION_FORM_DATE && !etat.entree )
	    {
		fin_edition();
		return TRUE;
	    }

	    /* si le prochain est le d�bit, on v�rifie s'il n'y a rien dans cette
	       entr�e et s'il y a quelque chose dans l'entr�e du cr�dit */

	    if ( origine == TRANSACTION_FORM_DEBIT )
	    {
		/* si le d�bit est gris et le cr�dit est noir, on met sur le cr�dit */

		if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ) == style_entree_formulaire[ENGRIS] &&
		     gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ) == style_entree_formulaire[ENCLAIR] )
		    origine = TRANSACTION_FORM_CREDIT;
	    }

	    /* si le prochain est le cr�dit et que le d�bit a �t� rempli,
	       on se met sur la devise et on efface le cr�dit */

	    if ( origine == TRANSACTION_FORM_CREDIT )
		if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ) == style_entree_formulaire[ENCLAIR] &&
		     strlen(gtk_entry_get_text(GTK_ENTRY( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ))))
		{
		    origine = TRANSACTION_FORM_DEVISE;
		    gtk_widget_set_style ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT],
					   style_entree_formulaire[ENGRIS] );
		    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ),
					 _("Credit") );
		}

	    /* on s�lectionne le contenu de la nouvelle entr�e */

	    if ( GTK_IS_COMBOFIX ( widget_formulaire_operations[origine] ) )
	    {
		gtk_widget_grab_focus ( GTK_COMBOFIX ( widget_formulaire_operations[origine] ) -> entry );
		gtk_entry_select_region ( GTK_ENTRY ( GTK_COMBOFIX ( widget_formulaire_operations[origine] ) -> entry ), 0, -1 );
	    }
	    else
	    {
		if ( GTK_IS_ENTRY ( widget_formulaire_operations[origine] ) )
		    gtk_entry_select_region ( GTK_ENTRY ( widget_formulaire_operations[origine] ), 0, -1 );

		gtk_widget_grab_focus ( widget_formulaire_operations[origine]  );
	    }
	    break;

	case GDK_KP_Enter :		/* touches entr�e */
	case GDK_Return :

	    /*       gtk_signal_emit_stop_by_name ( GTK_OBJECT ( widget ),
		     "key-press-event");*/

	    /* si la touche CTRL est elle aussi active, alors c'est que l'on est
	       probablement sur un champ de date et que l'on souhaite ouvrir
	       un calendrier */
	    if ( ( ev -> state & GDK_CONTROL_MASK ) == GDK_CONTROL_MASK )
	    {
		switch ( origine )
		{
		    case TRANSACTION_FORM_DATE :
		    case TRANSACTION_FORM_VALUE_DATE :

			popup_cal = gsb_calendar_new ( GTK_WIDGET ( GTK_ENTRY ( widget_formulaire_operations[origine] ) ) );
			gtk_widget_grab_focus ( GTK_WIDGET ( popup_cal ) );
			break ;

		    default :
			break ;
		}
	    }
	    /* si la touche CTRL n'est pas active, alors on valide simplement
	       la saisie de l'op�ration */
	    else
	    {
		p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
		fin_edition();
	    }
	    break;

	case GDK_KP_Add:		/* touches + */
	case GDK_plus:

	    /* si on est dans une entree de date, on augmente d'un jour
	       (d'une semaine) la date */

	    /* GDC : prise en compte de la date r�elle (18) FinGDC */
	    if ( origine == TRANSACTION_FORM_DATE ||
		 origine == TRANSACTION_FORM_VALUE_DATE )
	    {
		/*	  gtk_signal_emit_stop_by_name ( GTK_OBJECT ( widget ),
			  "key-press-event");*/

		/* si c'est la date d'op�ration et que le champ est vide, alors on
		   r�cup�re la date du jour comme date d'op�ration */
		if ( origine == TRANSACTION_FORM_DATE &&
		     strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[origine] ) ) ) ) == 0 )
		    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ), gsb_today() );

		/* si c'est la date de valeur et que le champ est vide, alors on
		   r�cup�re la date d'op�ration comme date de valeur */
		if ( origine == TRANSACTION_FORM_VALUE_DATE &&
		     strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[origine] ) ) ) ) == 0 )
		{
		    /* si la date d'op�ration est vide elle aussi, alors on r�cup�re
		       la date du jour comme date de valeur */
		    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) ) ) ) == 0 ||
			 g_strcasecmp ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) ) ),  _("Date") ) == 0 )
			gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ), gsb_today() );
		    else
			gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ), g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) ) ) );
		}

		if ( ( ev -> state & GDK_SHIFT_MASK ) != GDK_SHIFT_MASK )
		    inc_dec_date ( widget_formulaire_operations[origine], ONE_DAY );
		else
		    inc_dec_date ( widget_formulaire_operations[origine], ONE_WEEK );
		return TRUE;
	    }
	    else if ( origine == TRANSACTION_FORM_CHEQUE )	/* Voucher number */
	    {
		increment_decrement_champ ( widget_formulaire_operations[origine], 1 );
		return TRUE;
	    }
	    else 
	    {
		return FALSE;
	    }
	    break;

	case GDK_KP_Subtract:		/* touches - */
	case GDK_minus:

	    /* si on est dans une entree de date, on diminue d'un jour (d'une semaine) la date */

	    /* GDC : prise en compte de la date r�elle (18) FinGDC */
	    if ( origine == TRANSACTION_FORM_DATE ||
		 origine == TRANSACTION_FORM_VALUE_DATE )
	    {
		/*	  gtk_signal_emit_stop_by_name ( GTK_OBJECT ( widget ),
			  "key-press-event");*/

		/* si c'est la date d'op�ration et que le champ est vide, alors on
		   r�cup�re la date du jour comme date d'op�ration */
		if ( origine == TRANSACTION_FORM_DATE &&
		     strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[origine] ) ) ) ) == 0 )
		    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ), gsb_today() );

		/* si c'est la date de valeur et que le champ est vide, alors on
		   r�cup�re la date d'op�ration comme date de valeur */
		if ( origine == TRANSACTION_FORM_VALUE_DATE &&
		     strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[origine] ) ) ) ) == 0 )
		{
		    /* si la date d'op�ration est vide elle aussi, alors on r�cup�re
		       la date du jour comme date de valeur */
		    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) ) ) ) == 0 ||
			 g_strcasecmp ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) ) ),  _("Date") ) == 0 )
			gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ), gsb_today() );
		    else
			gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ), g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) ) ) );
		}

		if ( ( ev -> state & GDK_SHIFT_MASK ) != GDK_SHIFT_MASK )
		    inc_dec_date ( widget_formulaire_operations[origine], - ONE_DAY );
		else
		    inc_dec_date ( widget_formulaire_operations[origine], - ONE_WEEK );
		return TRUE;
	    }
	    else if ( origine == TRANSACTION_FORM_CHEQUE )	/* Voucher number */
	    {
		increment_decrement_champ ( widget_formulaire_operations[origine], -1 );
		return TRUE;
	    }
	    else 
	    {
		return FALSE;
	    }
	    break;

	case GDK_Page_Up :		/* touche PgUp */
	case GDK_KP_Page_Up :

	    /* si on est dans une entree de date, on augmente d'un mois (d'un an) la date */
	    /* GDC : prise en compte de la date r�elle (18) FinGDC */
	    if ( origine == TRANSACTION_FORM_DATE ||
		 origine == TRANSACTION_FORM_VALUE_DATE )
	    {
		gtk_signal_emit_stop_by_name ( GTK_OBJECT ( widget ),
					       "key-press-event");

		/* si c'est la date d'op�ration et que le champ est vide, alors on
		   r�cup�re la date du jour comme date d'op�ration */
		if ( origine == TRANSACTION_FORM_DATE &&
		     strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[origine] ) ) ) ) == 0 )
		    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ), gsb_today() );

		/* si c'est la date de valeur et que le champ est vide, alors on
		   r�cup�re la date d'op�ration comme date de valeur */
		if ( origine == TRANSACTION_FORM_VALUE_DATE &&
		     strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[origine] ) ) ) ) == 0 )
		{
		    /* si la date d'op�ration est vide elle aussi, alors on r�cup�re
		       la date du jour comme date de valeur */
		    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) ) ) ) == 0 ||
			 g_strcasecmp ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) ) ),  _("Date") ) == 0 )
			gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ), gsb_today() );
		    else
			gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ), g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) ) ) );
		}

		if ( ( ev -> state & GDK_SHIFT_MASK ) != GDK_SHIFT_MASK )
		    inc_dec_date ( widget_formulaire_operations[origine], ONE_MONTH );
		else
		    inc_dec_date ( widget_formulaire_operations[origine], ONE_YEAR );
		return TRUE;
	    }
	    break;

	case GDK_Page_Down :		/* touche PgDown */
	case GDK_KP_Page_Down :

	    /* si on est dans une entree de date, on augmente d'un mois (d'un an) la date */
	    /* GDC : prise en compte de la date r�elle (18) FinGDC */
	    if ( origine == TRANSACTION_FORM_DATE ||
		 origine == TRANSACTION_FORM_VALUE_DATE )
	    {
		gtk_signal_emit_stop_by_name ( GTK_OBJECT ( widget ),
					       "key-press-event");

		/* si c'est la date d'op�ration et que le champ est vide, alors on
		   r�cup�re la date du jour comme date d'op�ration */
		if ( origine == TRANSACTION_FORM_DATE &&
		     strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[origine] ) ) ) ) == 0 )
		    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ), gsb_today() );

		/* si c'est la date de valeur et que le champ est vide, alors on
		   r�cup�re la date d'op�ration comme date de valeur */
		if ( origine == TRANSACTION_FORM_VALUE_DATE &&
		     strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[origine] ) ) ) ) == 0 )
		{
		    /* si la date d'op�ration est vide elle aussi, alors on r�cup�re
		       la date du jour comme date de valeur */
		    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) ) ) ) == 0 ||
			 g_strcasecmp ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) ) ),  _("Date") ) == 0 )
			gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ), gsb_today() );
		    else
			gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ), g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) ) ) );
		}

		if ( ( ev -> state & GDK_SHIFT_MASK ) != GDK_SHIFT_MASK )
		    inc_dec_date ( widget_formulaire_operations[origine], - ONE_MONTH );
		else
		    inc_dec_date ( widget_formulaire_operations[origine], - ONE_YEAR );
		return TRUE;
	    }
	    break;

	default:
	    /* Reverting to default handler */
	    return FALSE;
	    break;
    }

    return TRUE;
}
/******************************************************************************/



/******************************************************************************/
/* Fonction completion_operation_par_tiers                                    */
/* appel�e lorsque le tiers perd le focus                                     */
/* r�cup�re le tiers, et recherche la derni�re op� associ�e � ce tiers        */
/* remplit le reste de l'op�ration avec les derni�res donn�es                 */
/******************************************************************************/
void completion_operation_par_tiers ( void )
{
    GSList *liste_tmp;
    struct struct_tiers *tiers;
    struct structure_operation *operation;
    GSList *pointeur_ope;
    gpointer ** p_tab_nom_de_compte_ope_trouvee;
    gchar *char_tmp;

    /* s'il y a quelque chose dans les entr�es d�bit/cr�dit ou cat�gories, on se barre */

    if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ) == style_entree_formulaire[ENCLAIR]
	 ||
	 gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ) == style_entree_formulaire[ENCLAIR]
	 ||
	 gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] )->entry ) == style_entree_formulaire[ENCLAIR] )
	return;

    /* recherche le tiers demand� */

    liste_tmp = g_slist_find_custom ( liste_struct_tiers,
				      g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ))),
				      ( GCompareFunc ) recherche_tiers_par_nom );

    /*   si nouveau tiers,  on s'en va simplement */

    if ( !liste_tmp )
	return;

    tiers = liste_tmp -> data;

    /* on fait d'abord le tour du compte courant pour recherche une op� avec ce tiers */
    /* s'il n'y a aucune op� correspondante, on fait le tour de tous les comptes */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
    p_tab_nom_de_compte_ope_trouvee = p_tab_nom_de_compte_courant;

    operation = NULL;
    pointeur_ope = LISTE_OPERATIONS;

    while ( pointeur_ope )
    {
	struct structure_operation *ope_test;

	ope_test = pointeur_ope -> data;

	if ( ope_test -> tiers == tiers -> no_tiers
	     &&
	     !ope_test -> no_operation_ventilee_associee )
	{
	    if ( operation )
	    {
		if ( g_date_compare ( ope_test -> date,
				      operation -> date ) >= 0 )
		    operation = ope_test;
	    }
	    else
		operation = ope_test;
	}
	pointeur_ope = pointeur_ope -> next;
    }

    if ( !operation )
    {
	/* aucune op�ration correspondant � ce tiers n'a �t� trouv�e dans le compte courant */
	/* on recherche dans les autres comptes, la premi�re trouv�e fera l'affaire */

	gint i;

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

	for ( i = 0 ; i < nb_comptes ; i++ )
	{
	    if ( i != compte_courant )
	    {
		pointeur_ope = LISTE_OPERATIONS;

		while ( pointeur_ope )
		{
		    struct structure_operation *ope_test;

		    ope_test = pointeur_ope -> data;

		    if ( ope_test -> tiers == tiers -> no_tiers )
		    {
			operation = ope_test;
			pointeur_ope = NULL;
			i = nb_comptes;
		    }
		    else
			pointeur_ope = pointeur_ope -> next;
		}
	    }
	    p_tab_nom_de_compte_variable++;
	}
	p_tab_nom_de_compte_variable--;
	p_tab_nom_de_compte_ope_trouvee = p_tab_nom_de_compte_variable;
    }

    /* si on n'a trouv� aucune op�ration, on se tire */

    if ( !operation )
	return;

    /* remplit les diff�rentes entr�es du formulaire */

    //  p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_ope_trouvee;

    /* remplit les montant et place le menu correspondant dans l'option menu des types */

    if ( operation -> montant < 0 )
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] );
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ),
			     g_strdup_printf ( "%4.2f",
					       -operation -> montant ));

	/* met le menu des types d�bits */

	if ( !etat.affiche_tous_les_types )
	{
	    GtkWidget *menu;

	    if ( (menu = creation_menu_types ( 1, compte_courant, 0  )))
	    {
		gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
					   menu );
		gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] );
	    }
	    else
	    {
		gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] );
		gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] );
	    }
	}
    }
    else
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] );
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ),
			     g_strdup_printf ( "%4.2f",
					       operation -> montant ));

	/* met le menu des types cr�dits */

	if ( !etat.affiche_tous_les_types )
	{
	    GtkWidget *menu;

	    if ( (menu = creation_menu_types ( 2, compte_courant, 0 )))
	    {
		gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
					   menu );
		gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] );
	    }
	    else
	    {
		gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] );
		gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] );
	    }
	}
    }

    /* met le type correspondant � l'op� */

    if ( GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ))
    {
	gint place_type;
	struct struct_type_ope *type;

	/* recherche le type de l'op� */

	place_type = cherche_no_menu_type ( operation -> type_ope );

	/* si aucun type n'a �t� trouv�, on cherche le d�faut */

	if ( place_type == -1 )
	{
	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

	    if ( operation -> montant < 0 )
		place_type = cherche_no_menu_type ( TYPE_DEFAUT_DEBIT );
	    else
		place_type = cherche_no_menu_type ( TYPE_DEFAUT_CREDIT );

	    /* si le type par d�faut n'est pas trouv�, on met 0 */

	    if ( place_type == -1 )
		place_type = 0;
	}

	/*       � ce niveau, place type est mis */

	gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
				      place_type );

	/* r�cup�re l'adr du type pour mettre un n� auto si n�cessaire */

	type = gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ) -> menu_item ),
				     "adr_type" );

	if ( type -> affiche_entree )
	{
	    /* on r�affiche l'entr�e car si le menu avait �t� mis sur le 0, c'�tait pas affich� */

	    gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] );

	    if ( type -> numerotation_auto )
	    {
		entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] );
		gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ),
				     itoa ( type -> no_en_cours + 1 ));
	    }
	    else
		if ( operation -> contenu_type )
		{
		    entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] );
		    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ),
					 operation -> contenu_type );
		}
	}
    }

    /* met la devise */

    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_DEVISE] ),
				  g_slist_position ( liste_struct_devises,
						     g_slist_find_custom ( liste_struct_devises,
									   GINT_TO_POINTER ( operation -> devise ),
									   ( GCompareFunc ) recherche_devise_par_no )));
    /* mise en forme des cat�gories */

    if ( operation -> operation_ventilee )
    {
	GtkWidget *dialog;
	GtkWidget *label;
	gint result;

	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] );

	/* c'est une op� ventil�e, la cat�gorie est Op�ration ventil�e */
	/* on demande si on veut r�cup�rer les anciennes op�s de ventilation */

	gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ),
				_("Breakdown of transaction") );
	gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN] );

	/* affiche la question de r�cup�ration */

	dialog = gtk_dialog_new_with_buttons ( _("Recover breakdown?"),
					       GTK_WINDOW (window),
					       GTK_DIALOG_MODAL,
					       GTK_STOCK_YES,0,
					       GTK_STOCK_NO,1,
					       NULL );

	label = gtk_label_new ( _("Do you want to recover all the lines of the last breakdown?") );
	gtk_box_pack_start ( GTK_BOX ( GTK_DIALOG ( dialog ) -> vbox ),
			     label,
			     FALSE,
			     FALSE,
			     5 );
	gtk_widget_show ( label );

	result = gtk_dialog_run ( GTK_DIALOG ( dialog ));
	gtk_widget_destroy ( dialog );

	if ( !result )
	{
	    /* r�cup�ration des anciennes op�s de la ventilation */

	    GSList *liste_des_opes_de_ventilation;

	    liste_des_opes_de_ventilation = NULL;
	    pointeur_ope = LISTE_OPERATIONS;

	    while ( pointeur_ope )
	    {
		struct structure_operation *ope_test;

		ope_test = pointeur_ope -> data;

		if ( ope_test -> no_operation_ventilee_associee == operation -> no_operation )
		{
		    struct struct_ope_ventil *nouvelle_operation;

		    nouvelle_operation = calloc ( 1,
						  sizeof ( struct struct_ope_ventil ));

		    nouvelle_operation -> montant = ope_test -> montant;
		    nouvelle_operation -> categorie = ope_test -> categorie;
		    nouvelle_operation -> sous_categorie = ope_test -> sous_categorie;

		    if ( ope_test -> notes )
			nouvelle_operation -> notes = g_strdup ( ope_test -> notes );

		    nouvelle_operation -> imputation = ope_test -> imputation;
		    nouvelle_operation -> sous_imputation = ope_test -> sous_imputation;

		    if ( ope_test -> no_piece_comptable )
			nouvelle_operation -> no_piece_comptable = g_strdup ( ope_test -> no_piece_comptable );

		    /* si c'est un virement, on met le compte de virement et le type choisi */

		    if ( ope_test -> relation_no_operation )
		    {
			struct structure_operation *contre_operation;

			nouvelle_operation -> relation_no_operation = -1;
			nouvelle_operation -> relation_no_compte = ope_test -> relation_no_compte;

			/*  on va chercher le type de l'op� associ�e */

			p_tab_nom_de_compte_variable = p_tab_nom_de_compte + ope_test -> relation_no_compte;

			contre_operation = g_slist_find_custom ( LISTE_OPERATIONS,
								 GINT_TO_POINTER ( ope_test -> relation_no_operation ),
								 (GCompareFunc) recherche_operation_par_no ) -> data;

			nouvelle_operation -> no_type_associe = contre_operation -> type_ope;

			//		      p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
			p_tab_nom_de_compte_variable = p_tab_nom_de_compte_ope_trouvee;
		    }

		    liste_des_opes_de_ventilation = g_slist_append ( liste_des_opes_de_ventilation,
								     nouvelle_operation );
		}
		pointeur_ope = pointeur_ope -> next;
	    }

	    if ( liste_des_opes_de_ventilation )
		gtk_object_set_data ( GTK_OBJECT ( formulaire ),
				      "liste_adr_ventilation",
				      liste_des_opes_de_ventilation );
	    else
		gtk_object_set_data ( GTK_OBJECT ( formulaire ),
				      "liste_adr_ventilation",
				      GINT_TO_POINTER ( -1 ) );
	}
    }
    else
	if ( operation -> relation_no_operation )
	{
	    /* c'est un virement */

	    GtkWidget *menu;

	    entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] );

	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

	    if ( operation -> relation_no_operation != -1 )
		gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ),
					g_strconcat ( COLON(_("Transfer")),
						      NOM_DU_COMPTE,
						      NULL ));
	    else
		gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ),
					_("Transfer") );

	    /* comme c'est un virement, on va afficher le type de la contre op� s'il existe */

	    if ( operation -> montant >= 0 )
		menu = creation_menu_types ( 1, operation -> relation_no_compte, 2  );
	    else
		menu = creation_menu_types ( 2, operation -> relation_no_compte, 2  );

	    /*  on ne continue que si un menu a �t� cr�� */
	    /*    dans ce cas, on va chercher l'autre op� et retrouve le type */

	    if ( menu )
	    {
		struct structure_operation *operation_2;

		gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ),
					   menu );
		gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] );

		operation_2 = g_slist_find_custom ( LISTE_OPERATIONS,
						    GINT_TO_POINTER ( operation -> relation_no_operation ),
						    (GCompareFunc) recherche_operation_par_no ) -> data;
		gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ),
					      cherche_no_menu_type_associe ( operation_2 -> type_ope,
									     0 ));
	    }
	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
	}
	else
	{
	    /* c'est des cat�gories normales */

	    char_tmp = categorie_name_by_no ( operation -> categorie,
					      operation -> sous_categorie );
	    if ( char_tmp )
	    {
		entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] );
		gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ),
					char_tmp );
	    }
	}

    /* met en place l'imputation budg�taire */

    char_tmp = ib_name_by_no ( operation -> imputation,
			       operation -> sous_imputation );
    if ( char_tmp )
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] );
	gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] ),
				char_tmp );
    }

    /* mise en place de la pi�ce comptable */

    if ( operation -> no_piece_comptable )
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] );
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] ),
			     operation -> no_piece_comptable );
    }

    /*   remplit les notes */

    if ( operation -> notes )
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_NOTES] );
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_NOTES] ),
			     operation -> notes );
    }

    /*   remplit les infos guichet / banque */

    if ( operation -> info_banque_guichet )
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_BANK] );
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_BANK] ),
			     operation -> info_banque_guichet );
    }
}
/******************************************************************************/

/******************************************************************************/
/* proc�dure appel�e lors de la fin de l'�dition d'op�ration */
/* cr�e une nouvelle op�ration � partir du formulaire */
/* puis appelle ajout_operation pour la sauver */
/******************************************************************************/
void fin_edition ( void )
{
    struct structure_operation *operation;
    gint modification;
    GSList *liste_no_tiers;
    GSList *liste_tmp;

    /* r�cup�ration de l'op�ration : soit l'adr de la struct, soit NULL si nouvelle */

    operation = gtk_object_get_data ( GTK_OBJECT ( formulaire ),
				      "adr_struct_ope" );

    if ( operation )
	modification = 1;
    else
	modification = 0;

    /* on commence par v�rifier que les donn�es entr�es sont correctes */
    /* si la fonction renvoie false, c'est qu'on doit arr�ter l� */

    if ( !verification_validation_operation ( operation ))
	return;

    /* si le tiers est un �tat, on va faire autant d'op�rations qu'il y a de tiers
       dans l'�tat concern�. On va cr�er une liste avec les nos de tiers
       ( ou -1, le tiers n'est pas un �tat), puis on fera une boucle sur cette liste
       pour ajouter autant d'op�rations que de tiers */

    liste_no_tiers = NULL;

    if ( strncmp ( g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ))),
		   COLON(_("Report")),
		   7 ))
	/*     ce n'est pas un �tat, on met -1 comme no de tiers */
	liste_no_tiers = g_slist_append (liste_no_tiers,
					 GINT_TO_POINTER ( -1 ));
    else
    {
	/* c'est bien un �tat */
	/* on commence par retrouver le nom de l'�tat */
	/* toutes les v�rifications ont �t� faites pr�c�demment */

	gchar **tableau_char;
	struct struct_etat *etat;
	GSList *liste_opes_selectionnees;

	tableau_char = g_strsplit ( g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ))),
				    ":",
				    2 );

	tableau_char[1] = g_strstrip ( tableau_char[1] );
	liste_tmp = liste_struct_etats;
	etat = NULL;

	while ( liste_tmp )
	{
	    etat = liste_tmp -> data;

	    if ( !strcmp ( etat -> nom_etat,
			   tableau_char[1] ))
		liste_tmp = NULL;
	    else
		liste_tmp = liste_tmp -> next;
	}

	g_strfreev ( tableau_char );

	/* � ce niveau, etat contient l'adr le la struct de l'�tat choisi */

	liste_opes_selectionnees = recupere_opes_etat ( etat );

	liste_tmp = liste_opes_selectionnees;

	while ( liste_tmp )
	{
	    struct structure_operation *operation;

	    operation = liste_tmp -> data;

	    if ( !g_slist_find ( liste_no_tiers,
				 GINT_TO_POINTER ( operation -> tiers )))
		liste_no_tiers = g_slist_append ( liste_no_tiers,
						  GINT_TO_POINTER ( operation -> tiers ));

	    liste_tmp = liste_tmp -> next;
	}

	g_slist_free ( liste_opes_selectionnees );
    }

    /* � ce niveau, liste_no_tiers contient la liste des no de tiers pour chacun desquels on */
    /* fera une op�, ou -1 si on utilise que le tiers dans l'entr�e du formulaire */
    /* on fait donc le tour de cette liste en ajoutant l'op� � chaque fois */

    liste_tmp = liste_no_tiers;

    while ( liste_tmp )
    {
	/* soit on va chercher le tiers dans la liste des no de tiers et on le met dans le formulaire, */
	/* soit on laisse tel quel et on met liste_tmp � NULL */

	if ( liste_tmp -> data == GINT_TO_POINTER ( -1 ) )
	    liste_tmp = NULL;
	else if ( liste_tmp -> data == NULL )
	{
	    dialogue_error ( _("No third party selected for this report."));
	    return;
	}
	else
	{
	    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ),
				    tiers_name_by_no ( GPOINTER_TO_INT (liste_tmp -> data), TRUE ));

	    /* si le moyen de paiement est � incr�mentation automatique, � partir de la 2�me op�, */
	    /* on incr�mente le contenu (no de ch�que en g�n�ral) */

	    /* comme c'est une liste de tiers, c'est forcemment une nouvelle op� */
	    /* donc si operation n'est pas nul, c'est qu'on n'est pas sur la 1�re */
	    /* donc on peut incr�menter si n�cessaire le contenu */
	    /* et on peut r�cup�rer le no_type de l'ancienne op� */

	    if ( operation )
	    {
		GSList *pointeur_tmp;

		p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

		pointeur_tmp = g_slist_find_custom ( TYPES_OPES,
						     GINT_TO_POINTER ( operation -> type_ope ),
						     (GCompareFunc) recherche_type_ope_par_no );

		if ( pointeur_tmp )
		{
		    struct struct_type_ope *type;

		    type = pointeur_tmp -> data;

		    if ( type -> affiche_entree &&
			 type -> numerotation_auto )
			gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ),
					     itoa ( type -> no_en_cours + 1 ));
		}
	    }
	    liste_tmp = liste_tmp -> next;
	}

	/* si c'est une nouvelle op�, on la cr�e en mettant tout � 0 sauf le no de compte */

	if ( !modification )
	{
	    operation = calloc ( 1,
				 sizeof ( struct structure_operation ) );
	    if ( !operation )
	    {
		dialogue_error ( _("Cannot allocate memory, bad things will happen soon") );
		return;
	    }
	    operation -> no_compte = compte_courant;
	}

	/* on r�cup�re les donn�es du formulaire sauf la categ qui est trait�e plus tard */

	recuperation_donnees_generales_formulaire ( operation );

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

	/* il faut ajouter l'op�ration � la liste � ce niveau pour lui attribuer un num�ro */

	/* celui ci sera utilis� si c'est un virement ou si c'est une ventil qui contient des */
	/* virements */

	ajout_operation ( operation );

	/*   r�cup�ration des cat�gories / sous-cat�g, s'ils n'existent pas, on les cr�e */
	/* � mettre en dernier car si c'est une op� ventil�e, chaque op� de ventil va r�cup�rer les donn�es du dessus */

	recuperation_categorie_formulaire ( operation,
					    modification );
    }

    /* on lib�re la liste des no tiers */

    g_slist_free ( liste_no_tiers );

    /* si on est en train d'�quilibrer => recalcule le total point� */

    if ( etat.equilibrage )
    {
	calcule_total_pointe_compte ( compte_courant );

	if ( fabs ( solde_final - solde_initial - operations_pointees ) < 0.01 )
	{
	    gtk_label_set_text ( GTK_LABEL ( label_equilibrage_ecart ),
				 g_strdup_printf ( "%4.2f",
						   0.0 ));
	    gtk_widget_set_sensitive ( GTK_WIDGET ( bouton_ok_equilibrage ),
				       TRUE );
	}
	else
	{
	    gtk_label_set_text ( GTK_LABEL ( label_equilibrage_ecart ),
				 g_strdup_printf ( "%4.2f",
						   solde_final - solde_initial - operations_pointees ));
	    gtk_widget_set_sensitive ( GTK_WIDGET ( bouton_ok_equilibrage ),
				       FALSE );
	}
    }

    /* si c'�tait une nouvelle op�ration, on efface le formulaire,
       on remet la date pour la suivante,
       si c'�tait une modification, on redonne le focus � la liste */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

    if ( gtk_object_get_data ( GTK_OBJECT ( formulaire ),
			       "adr_struct_ope" ) )
    {
	formulaire_a_zero ();

	if ( !etat.formulaire_toujours_affiche )
	    gtk_widget_hide ( frame_droite_bas );
    }
    else
    {
	formulaire_a_zero ();

	clique_champ_formulaire ( widget_formulaire_operations[TRANSACTION_FORM_DATE],
				  NULL,
				  GINT_TO_POINTER ( TRANSACTION_FORM_DATE ) );
	gtk_entry_select_region ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ),
				  0,
				  -1);
	gtk_widget_grab_focus ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) );
    }

    /* on r�affiche la liste courante pour afficher les categ qui ont �t� ajout�e apr�s */
    /* l'ajout d'op�ration */

    MISE_A_JOUR = 1;
    verification_mise_a_jour_liste ();

    /* met � jour les listes ( nouvelle op�ration associ�e au tiers et � la cat�gorie ) */

    mise_a_jour_tiers ();
    mise_a_jour_categ ();
    mise_a_jour_imputation ();

    modification_fichier ( TRUE );
}
/******************************************************************************/

/******************************************************************************/
/* v�rifie que les donn�es dans le formulaire sont valides pour enregistrer   */
/* l'op�ration appel�e lors de la fin_edition d'une op�                       */
/******************************************************************************/
gint verification_validation_operation ( struct structure_operation *operation )
{
    gchar **tableau_char;
    GSList *liste_tmp;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

    /* on v�rifie qu'il y a bien une date */

    if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) != style_entree_formulaire[ENCLAIR] )
    {
	dialogue_error ( _("You must enter a date.") );
	return (FALSE);
    }

    /* v�rifie que la date est correcte */

    if ( !modifie_date ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ))
    {
	dialogue_error ( _("Invalid date") );
	gtk_widget_grab_focus ( widget_formulaire_operations[TRANSACTION_FORM_DATE] );
	gtk_entry_select_region ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ),
				  0,
				  -1);
	return (FALSE);
    }

    /* la date est correcte, il faut l'enregistrer dans date_courante */

    strncpy ( date_courante,
	      (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] )),
	      10 );

    /* v�rifie que la date de valeur est correcte */

    if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ) == style_entree_formulaire[ENCLAIR]
	 &&
	 !modifie_date ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ) )
    {
	dialogue_error ( _("Invalid date.") );
	gtk_widget_grab_focus ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] );
	gtk_entry_select_region ( GTK_ENTRY (  widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ),
				  0,
				  -1);
	return (FALSE);
    }

    /* v�rification que ce n'est pas un virement sur lui-m�me */
    /* et que le compte de virement existe */

    if ( gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ) -> entry ) == style_entree_formulaire[ENCLAIR] )
    {
	tableau_char = g_strsplit ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] )),
				    ":",
				    2 );

	tableau_char[0] = g_strstrip ( tableau_char[0] );

	if ( tableau_char[1] )
	    tableau_char[1] = g_strstrip ( tableau_char[1] );


	    /* Si c'est un virement, on fait les v�rifications */
	    if ( !strcmp ( tableau_char[0], _("Transfer") ) )
	    {
		/* S'il n'y a rien apr�s "Transfer", alors : */
		if ( !tableau_char[1] ||
		     !strlen ( tableau_char[1] ) )
		{
		    dialogue_error ( _("There is no associated account for this transfer.") );
		    return (FALSE);
		}
		/* si c'est un virement vers un compte supprim�, laisse passer */

		if ( strcmp ( tableau_char[1],
			      _("Deleted account") ) )
		{
		    /* recherche le no de compte du virement */

		    gint compte_virement;
		    gint i;

		    p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

		    compte_virement = -1;

		    for ( i = 0 ; i < nb_comptes ; i++ )
		    {
			if ( !g_strcasecmp ( NOM_DU_COMPTE,
					     tableau_char[1] ) )
			    compte_virement = i;
			p_tab_nom_de_compte_variable++;
		    }


		    if ( compte_virement == -1 )
		    {
			dialogue_warning ( _("Associated account of this transfer is invalid.") );
			return (FALSE);
		    }

		    if ( compte_virement == compte_courant )
		    {
			dialogue_error ( _("Can't issue a transfer its own account.") );
			return (FALSE);
		    }

/* 		    v�rifie si le compte n'est pas clos */

		    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_virement;

		    if ( COMPTE_CLOTURE )
		    {
			dialogue_error ( _("Can't issue a transfer on a closed account." ));
			return ( FALSE );
		    }

		    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

		}
	}
	g_strfreev ( tableau_char );
    }

    /* pour les types qui sont � incr�mentation automatique ( surtout les chèques ) */
    /* on fait le tour des operations pour voir si le no n'a pas d�j� �t� utilis� */
    /* si operation n'est pas nul, c'est une modif donc on ne fait pas ce test */

    if ( GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ))
    {
	struct struct_type_ope *type;

	type = gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ) -> menu_item ),
				     "adr_type" );

	if ( type -> numerotation_auto )
	{
	    gchar *no_cheque;

	    /* v�rifie s'il y a quelque chose */

	    if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ) == style_entree_formulaire[ENGRIS] )
	    {
		if ( question ( _("Selected method of payment has an automatic incremental number\nbut doesn't contain any number.\nContinue anyway?") ) )
		    goto sort_test_cheques;
		else
		    return (FALSE);
	    }

	    /* v�rifie si le no de ch�que n'est pas d�j� utilis� */

	    if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ) == style_entree_formulaire[ENCLAIR] )
	    {
		liste_tmp = LISTE_OPERATIONS;
		no_cheque = g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] )));

		while ( liste_tmp )
		{
		    struct structure_operation *operation_tmp;

		    operation_tmp = liste_tmp -> data;

		    if ( operation_tmp -> type_ope == type -> no_type &&
			 operation_tmp -> contenu_type &&
			 !strcmp ( operation_tmp -> contenu_type,
				   no_cheque ))
		    {
			/* on a trouv� le m�me no de ch�que, si c'est une nouvelle op�, c'est pas normal, */
			/* si c'est une modif d'op�, c'est normal que si c'est cette op� qu'on a trouv� */

			if ( !operation ||
			     operation_tmp -> no_operation != operation -> no_operation )
			{
			    if ( question ( _("Warning: this cheque number is already used.\nContinue anyway?") ))
				goto sort_test_cheques;
			    else
				return (FALSE);
			}
		    }
		    liste_tmp = liste_tmp -> next;
		}
	    }
	}
    }

sort_test_cheques :

    /* v�rifie tout de suite si c'est une op�ration ventil�e, */
    /* si c'est le cas, si la liste des ventilation existe (soit adr de liste, soit -1), on va l'enregistrer plus tard */
    /* sinon on va ventiler tout de suite */

    if ( !strcmp ( g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ))),
		   _("Breakdown of transaction") )
	 &&
	 !gtk_object_get_data ( GTK_OBJECT ( formulaire ),
				"liste_adr_ventilation" ))
    {
	enregistre_ope_au_retour = 1;

	if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ) == style_entree_formulaire[ENCLAIR] )
	    ventiler_operation ( -my_strtod ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ))),
					      NULL ));
	else
	    ventiler_operation ( my_strtod ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ))),
					     NULL ));

	return (FALSE);
    }

    /* on v�rifie si le tiers est un �tat, que c'est une nouvelle op�ration */

    if ( !strncmp ( g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ))),
		    COLON(_("Report")),
		    7 ))
    {
	gint trouve;

	/* on v�rifie d'abord si c'est une modif d'op� */

	if ( operation )
	{
	    dialogue_error ( _("A transaction with a multiple third party must be a new one.") );
	    return (FALSE);
	}

	/* on v�rifie maintenant si l'�tat existe */

	tableau_char = g_strsplit ( g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ))),
				    ":",
				    2 );

	tableau_char[1] = g_strstrip ( tableau_char[1] );
	liste_tmp = liste_struct_etats;
	trouve = 0;

	while ( liste_tmp )
	{
	    struct struct_etat *etat;

	    etat = liste_tmp -> data;

	    if ( !strcmp ( etat -> nom_etat,
			   tableau_char[1] ))
	    {
		trouve = 1;
		liste_tmp = NULL;
	    }
	    else
		liste_tmp = liste_tmp -> next;
	}

	g_strfreev ( tableau_char );

	if ( !trouve )
	{
	    dialogue_error ( _("Invalid multiple third party.") );
	    return (FALSE);
	}
    }
    return ( TRUE );
}
/******************************************************************************/

/******************************************************************************/
/* cette fonction r�cup�re les donn�es du formulaire et les met dans          */
/* l'op�ration en argument sauf la cat�g                                      */
/******************************************************************************/
void recuperation_donnees_generales_formulaire ( struct structure_operation *operation )
{
    gchar *pointeur_char;
    GSList *pointeur_liste;
    gchar **tableau_char;
    struct struct_devise *devise;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

    /* traitement de la date */

    pointeur_char = g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] )));

    tableau_char = g_strsplit ( pointeur_char,
				"/",
				3 );

    operation -> jour = my_strtod ( tableau_char[0], NULL );
    operation -> mois = my_strtod ( tableau_char[1], NULL );
    operation -> annee = my_strtod ( tableau_char[2], NULL );

    operation -> date = g_date_new_dmy ( operation -> jour,
					 operation -> mois,
					 operation -> annee );

    /* traitement de la date de valeur */

    if ( etat.affiche_date_bancaire &&
	 gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ) == style_entree_formulaire[ENCLAIR] )
    {
	pointeur_char = g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] )));

	tableau_char = g_strsplit ( pointeur_char,
				    "/",
				    3 );

	operation -> jour_bancaire = my_strtod ( tableau_char[0], NULL );
	operation -> mois_bancaire = my_strtod ( tableau_char[1], NULL );
	operation -> annee_bancaire = my_strtod ( tableau_char[2], NULL );

	operation -> date_bancaire = g_date_new_dmy ( operation -> jour_bancaire,
						      operation -> mois_bancaire,
						      operation -> annee_bancaire );
    }
    else
    {
	operation -> jour_bancaire = 0;
	operation -> mois_bancaire = 0;
	operation -> annee_bancaire = 0;

	operation->date_bancaire = NULL;
    }

    /* r�cup�ration du tiers, s'il n'existe pas, on le cr�e */

    if ( gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ) -> entry ) == style_entree_formulaire[ENCLAIR] )
    {
	pointeur_char = g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] )));

	if ( ( pointeur_liste = g_slist_find_custom ( liste_struct_tiers,
						      pointeur_char,
						      ( GCompareFunc ) recherche_tiers_par_nom )) )
	    operation -> tiers = (( struct struct_tiers * )( pointeur_liste -> data )) -> no_tiers;
	else
	    operation -> tiers = (( struct struct_tiers * )( ajoute_nouveau_tiers ( pointeur_char ))) -> no_tiers;
    }
    else
	operation -> tiers = 0;

    /* r�cup�ration du montant */

    if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ) == style_entree_formulaire[ENCLAIR] )
	operation -> montant = -my_strtod ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ))),
					    NULL );
    else
	operation -> montant = my_strtod ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ))),
					   NULL );

    /* r�cup�ration de la devise */

    devise = gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_DEVISE] ) -> menu_item ),
				   "adr_devise" );


    /* r�cup�ration de la devise */

    devise = gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_DEVISE] ) -> menu_item ),
				   "adr_devise" );

    /* si c'est la devise du compte ou */
    /* si c'est un compte qui doit passer � l'euro ( la transfo se fait au niveau de l'affichage de la liste ) */
    /* ou si c'est un compte en euro et l'op� est dans une devise qui doit passer � l'euro -> pas de change � demander */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

    if ( !devise_compte ||
	 devise_compte -> no_devise != DEVISE )
	devise_compte = g_slist_find_custom ( liste_struct_devises,
					      GINT_TO_POINTER ( DEVISE ),
					      ( GCompareFunc ) recherche_devise_par_no ) -> data;

    operation -> devise = devise -> no_devise;

    if ( !( operation -> no_operation ||
	    devise -> no_devise == DEVISE ||
	    ( devise_compte -> passage_euro && !strcmp ( devise -> nom_devise, _("Euro") )) ||
	    ( !strcmp ( devise_compte -> nom_devise, _("Euro") ) && devise -> passage_euro )))
    {
	/* c'est une devise �trang�re, on demande le taux de change et les frais de change */

	demande_taux_de_change ( devise_compte, devise, 1,
				 ( gdouble ) 0, ( gdouble ) 0, FALSE );

	operation -> taux_change = taux_de_change[0];
	operation -> frais_change = taux_de_change[1];

	if ( operation -> taux_change < 0 )
	{
	    operation -> taux_change = -operation -> taux_change;
	    operation -> une_devise_compte_egale_x_devise_ope = 1;
	}
    }

    /* r�cup�ration du type d'op�ration */

    if ( GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ))
    {
	operation -> type_ope = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ) -> menu_item ),
									"no_type" ));
	if ( GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] )
	     &&
	     gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ) == style_entree_formulaire[ENCLAIR] )
	{
	    struct struct_type_ope *type;

	    type = gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ) -> menu_item ),
					 "adr_type" );

	    operation -> contenu_type = g_strdup ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ))));

	    if ( type -> numerotation_auto )
		type -> no_en_cours = ( my_atoi ( operation -> contenu_type ));
	}
	else
	    operation -> contenu_type = NULL;
    }
    else
    {
	operation -> type_ope = 0;
	operation -> contenu_type = NULL;
    }

    /* r�cup�ration du no d'exercice */
    /* si l'exo est � -1, c'est que c'est sur non affich� */
    /* soit c'est une modif d'op� et on touche pas � l'exo */
    /* soit c'est une nouvelle op� et on met l'exo � 0 */

    if ( GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_EXERCICE] ) -> menu_item ),
						 "no_exercice" )) == -1 )
    {
	if ( !operation -> no_operation )
	    operation -> no_exercice = 0;
    }
    else
	operation -> no_exercice = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_EXERCICE] ) -> menu_item ),
									   "no_exercice" ));

    /* r�cup�ration de l'imputation budg�taire */
    /* si c'est une op� ventil�e, on ne r�cup�re pas l'ib */

    if ( gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] ) -> entry ) == style_entree_formulaire[ENCLAIR]
	 &&
	 !GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN] ))
    {
	struct struct_imputation *imputation;

	pointeur_char = gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] ));

	tableau_char = g_strsplit ( pointeur_char,
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

	if ( tableau_char[1] && strlen ( tableau_char[1] ) )
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
    else
    {
	operation -> imputation = 0;
	operation -> sous_imputation = 0;
    }

    /* r�cup�ration du no de pi�ce comptable */

    if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] ) == style_entree_formulaire[ENCLAIR] )
	operation -> no_piece_comptable = g_strdup ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] ))));
    else
	operation -> no_piece_comptable = NULL;

    /* r�cup�ration des notes */

    if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_NOTES] ) == style_entree_formulaire[ENCLAIR] )
	operation -> notes = g_strdup ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_NOTES] ))));
    else
	operation -> notes = NULL;

    /* r�cup�ration de l'info banque/guichet */

    if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_BANK] ) == style_entree_formulaire[ENCLAIR] )
	operation -> info_banque_guichet = g_strdup ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_BANK] ))));
    else
	operation -> info_banque_guichet = NULL;
}
/******************************************************************************/

/******************************************************************************/
/* r�cup�ration des categ du formulaire, cr�e la contre op�ration si          */
/* n�cessaire ou remplit les op�s de ventil                                   */
/******************************************************************************/
void recuperation_categorie_formulaire ( struct structure_operation *operation,
					 gint modification )
{
    gchar *pointeur_char;
    gchar **tableau_char;
    struct structure_operation *operation_2;

    if ( gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ) -> entry ) == style_entree_formulaire[ENCLAIR] )
    {
	struct struct_categ *categ;

	pointeur_char = gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ));

	/* r�cup�ration de la ventilation si n�cessaire */

	if ( !strcmp ( g_strstrip ( pointeur_char ),
		       _("Breakdown of transaction") ))
	{
	    /* c'est une op� ventil�e, on va appeler la fonction validation_ope_de_ventilation */
	    /* qui va cr�er les nouvelles op�, les contre-op�rations et faire toutes les */
	    /* suppressions n�cessaires */

	    /*  auparavant, si c'est une modif d'op� et que l'ancienne op� �tait un virement, on  */
	    /* vire l'ancienne op� associ�e */

	    if ( modification &&
		 operation -> relation_no_operation )
	    {
		/* c'�tait un virement, et ce ne l'est plus, donc on efface l'op� en relation */

		p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

		operation_2 = g_slist_find_custom ( LISTE_OPERATIONS,
						    GINT_TO_POINTER ( operation -> relation_no_operation ),
						    ( GCompareFunc ) recherche_operation_par_no ) -> data;

		operation_2 -> relation_no_operation = 0;
		MISE_A_JOUR = 1;

		supprime_operation ( operation_2 );

		p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

		operation -> relation_no_operation = 0;
		operation -> relation_no_compte = 0;
	    }

	    validation_ope_de_ventilation ( operation );
	    operation -> operation_ventilee = 1;
	    operation -> categorie = 0;
	    operation -> sous_categorie = 0;
	}
	else
	{
	    /* ce n'est pas une op� ventil�e, si c'est une modif d'op� et que */
	    /* c'en �tait une, on supprime les op�s de ventil asssoci�es */

	    if ( modification &&
		 operation -> operation_ventilee )
	    {
		GSList *liste_tmp;

		liste_tmp = LISTE_OPERATIONS;

		while ( liste_tmp )
		{
		    struct structure_operation *ope_tmp;

		    ope_tmp = liste_tmp -> data;

		    if ( ope_tmp -> no_operation_ventilee_associee == operation -> no_operation )
		    {
			liste_tmp = liste_tmp -> next;
			supprime_operation ( ope_tmp );
		    }
		    else
			liste_tmp = liste_tmp -> next;
		}
		operation -> operation_ventilee = 0;
	    }

	    /* on va maintenant s�parer entre virement et cat�g normale */

	    tableau_char = g_strsplit ( pointeur_char,
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
		     && strlen ( tableau_char[1] ) )
		{
		    /* c'est un virement, il n'y a donc aucune cat�gorie */

		    operation -> categorie = 0;
		    operation -> sous_categorie = 0;

		    /* s�pare entre virement vers un compte et virement vers un compte supprim� */

		    if ( strcmp ( tableau_char[1],
				  _("Deleted account") ) )
		    {
			/* c'est un virement normal, on appelle la fonction qui va traiter �a */

			validation_virement_operation ( operation,
							modification,
							tableau_char[1] );
		    }
		    else
		    {
			/* c'est un virement vers un compte supprim� */

			operation -> relation_no_compte = -1;
			operation -> relation_no_operation = 1;
		    }
		}
		else
		{
		    /* c'est une cat�g normale, si c'est une modif d'op�, v�rifier si ce n'�tait pas un virement */

		    if ( modification &&
			 operation -> relation_no_operation )
		    {
			/* c'�tait un virement, et ce ne l'est plus, donc on efface l'op� en relation */

			p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

			operation_2 = g_slist_find_custom ( LISTE_OPERATIONS,
							    GINT_TO_POINTER ( operation -> relation_no_operation ),
							    ( GCompareFunc ) recherche_operation_par_no ) -> data;

			operation_2 -> relation_no_operation = 0;
			MISE_A_JOUR = 1;

			supprime_operation ( operation_2 );

			p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

			operation -> relation_no_operation = 0;
			operation -> relation_no_compte = 0;
		    }

		    categ = categ_par_nom ( tableau_char[0],
					    1,
					    operation -> montant < 0,
					    0 );

		    if ( !categ )
		    {
			struct struct_sous_categ *sous_categ;

			operation -> categorie = categ -> no_categ;

			sous_categ = sous_categ_par_nom ( categ,
							  tableau_char[1],
							  1 );

			if ( !sous_categ )
			    operation -> sous_categorie = sous_categ -> no_sous_categ;
		    }
		}
	    }
	    g_strfreev ( tableau_char );
	}
    }
}
/******************************************************************************/

/******************************************************************************/
/* cette fonction cr�e la contre-op�ration, l'enregistre et met en place      */
/* les liens si c'est une modif de virement, elle supprime aussi l'ancienne   */
/* contre op�ration                                                           */
/******************************************************************************/
void validation_virement_operation ( struct structure_operation *operation,
				     gint modification,
				     gchar *nom_compte_vire )
{
    struct struct_devise *devise, *devise_compte_2;
    struct structure_operation *contre_operation;
    gpointer **save_ptab;
    gint compte_virement, i;
    GSList *tmp;

    save_ptab = p_tab_nom_de_compte_variable;

    /* on n'a plus besoin de faire de tests, ceux ci ont �t� fait dans verification_validation_operation */

    /* r�cup�re le no du compte de la contre op�ration */

    compte_virement = -1;

    for ( i = 0 ; i < nb_comptes ; i++ )
    {
	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

	if ( !g_strcasecmp ( NOM_DU_COMPTE,
			     nom_compte_vire ))
	{
	    compte_virement = i;
	    i = nb_comptes;
	}
    }

    /*   2 possibilit�s, soit c'est un nouveau virement ou une modif qui n'�tait pas un virement, */
    /* soit c'est une modif de virement */
    /*     dans le 2nd cas, si on a modifi� le compte de la contre op�ration */
    /*     on supprime l'ancienne contre op�ration et c'est un nouveau virement */

    if ( modification )
    {
	/*       c'est une modif d'op� */
	/* 	soit c'�tait un virement vers un autre compte et dans ce cas on vire la contre-op�ration pour la recr�er plus tard */
	/* soit c'est un virement vers le m�me compte et dans ce cas on fait rien, la contre op� sera modifi�e automatiquement */
	/* soit ce n'�tait pas un virement et dans ce cas on consid�re l'op� comme une nouvelle op� */

	if ( operation -> relation_no_operation )
	{
	    /* c'�tait d�j� un virement, on ne vire la contre op� que si le compte cible a chang� */

	    if ( operation -> relation_no_compte != compte_virement )
	    {
		/* il faut retirer l'ancienne contre op�ration */

		p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

		contre_operation = g_slist_find_custom ( LISTE_OPERATIONS,
							 GINT_TO_POINTER ( operation -> relation_no_operation ),
							 ( GCompareFunc ) recherche_operation_par_no ) -> data;
		contre_operation -> relation_no_operation = 0;

		supprime_operation ( contre_operation );
		modification = 0;
	    }
	}
	else
	{
	    /* ce n'�tait pas un virement, on consid�re que c'est une nouvelle op� pour cr�er la contre op�ration */

	    modification = 0;
	}
    }

    /*   on en est maintenant � soit nouveau virement, soit modif de virement sans chgt de compte */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_virement;

    if ( modification
	 &&
	 operation -> relation_no_operation )
	contre_operation = g_slist_find_custom ( LISTE_OPERATIONS,
						 GINT_TO_POINTER ( operation -> relation_no_operation ),
						 ( GCompareFunc ) recherche_operation_par_no ) -> data;
    else
    {
	contre_operation = calloc ( 1,
				    sizeof ( struct structure_operation ) );
	contre_operation -> no_compte = compte_virement;
	contre_operation -> taux_change = 0;
	contre_operation -> frais_change = 0;
    }

    /* remplit la nouvelle op� */

    contre_operation -> jour = operation -> jour;
    contre_operation -> mois = operation -> mois;
    contre_operation -> annee = operation -> annee;
    contre_operation -> date = g_date_new_dmy ( contre_operation->jour,
						contre_operation->mois,
						contre_operation->annee);
    contre_operation -> montant = -operation -> montant;

    /* si c'est la devise du compte ou si c'est un compte qui doit passer � l'euro ( la transfo se fait au niveau */
    /* de l'affichage de la liste ) ou si c'est un compte en euro et l'op� est dans une devise qui doit passer � l'euro -> ok */

    devise_compte_2 = g_slist_find_custom ( liste_struct_devises,
					    GINT_TO_POINTER ( DEVISE ),
					    ( GCompareFunc ) recherche_devise_par_no ) -> data;
    tmp = g_slist_find_custom ( liste_struct_devises,
				GINT_TO_POINTER ( operation -> devise ),
				( GCompareFunc ) recherche_devise_par_no);
    if ( tmp )
	devise = (struct struct_devise *) tmp -> data;
    else
	devise = NULL;

    contre_operation -> devise = operation -> devise;

    if ( !( contre_operation-> no_operation
	    ||
	    devise -> no_devise == DEVISE
	    ||
	    ( devise_compte_2 -> passage_euro && is_euro(devise))
	    ||
	    ( is_euro(devise_compte_2) && devise -> passage_euro )))
    {
	/* c'est une devise �trang�re, on demande le taux de change et les frais de change */

	demande_taux_de_change ( devise_compte_2, devise, 1,
				 (gdouble ) 0, (gdouble ) 0, FALSE );

	contre_operation -> taux_change = taux_de_change[0];
	contre_operation -> frais_change = taux_de_change[1];

	if ( contre_operation -> taux_change < 0 )
	{
	    contre_operation -> taux_change = -contre_operation -> taux_change;
	    contre_operation -> une_devise_compte_egale_x_devise_ope = 1;
	}
    }

    contre_operation -> tiers = operation -> tiers;
    contre_operation -> categorie = operation -> categorie;
    contre_operation -> sous_categorie = operation -> sous_categorie;

    if ( operation -> notes )
	contre_operation -> notes = g_strdup ( operation -> notes);

    contre_operation -> auto_man = operation -> auto_man;

    /* r�cup�ration du type de l'autre op� */

    if ( GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ))
	contre_operation -> type_ope = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ) -> menu_item ),
									       "no_type" ));
    else
    {
	contre_operation -> type_ope = 0;
	operation -> contenu_type = NULL;
    }

    if ( operation -> contenu_type )
	contre_operation -> contenu_type = g_strdup ( operation -> contenu_type );

    contre_operation -> no_exercice = operation -> no_exercice;
    contre_operation -> imputation = operation -> imputation;
    contre_operation -> sous_imputation = operation -> sous_imputation;

    if ( operation -> no_piece_comptable )
	contre_operation -> no_piece_comptable = g_strdup ( operation -> no_piece_comptable );

    if ( operation -> info_banque_guichet )
	contre_operation -> info_banque_guichet = g_strdup ( operation -> info_banque_guichet );

    /*   on a fini de remplir l'op�, on peut l'ajouter � la liste */

    ajout_operation ( contre_operation );

    /* on met maintenant les relations entre les diff�rentes op� */

    contre_operation -> pointe = operation -> pointe;
    contre_operation -> no_rapprochement = operation -> no_rapprochement;

    operation -> relation_no_operation = contre_operation -> no_operation;
    operation -> relation_no_compte = contre_operation -> no_compte;
    contre_operation -> relation_no_operation = operation -> no_operation;
    contre_operation -> relation_no_compte = operation -> no_compte;

    p_tab_nom_de_compte_variable = save_ptab;
}
/******************************************************************************/

/******************************************************************************/
/* Fonction ajout_operation                                                   */
/* ajoute l'op�ration donn�e en argument � la liste des op�s, trie la liste   */
/* et r�affiche la clist                                                      */
/* remplit si n�cessaire le no d'op�ration                                    */
/******************************************************************************/
void ajout_operation ( struct structure_operation *operation )
{
    gpointer **save_ptab;

    save_ptab = p_tab_nom_de_compte_variable;

    /* si c'est une nouvelle op�ration, on lui met son no d'op�ration */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

    if ( !operation -> no_operation )
    {
	operation -> no_operation = ++no_derniere_operation;

	LISTE_OPERATIONS = g_slist_insert_sorted ( LISTE_OPERATIONS,
						   operation,
						   (GCompareFunc) CLASSEMENT_COURANT );
	NB_OPE_COMPTE++;
    }

    /* on r�affiche(ra) la liste des op�s */

    MISE_A_JOUR = 1;

    /* on classe la liste */

/*     LISTE_OPERATIONS = g_slist_sort ( LISTE_OPERATIONS, */
/* 				      (GCompareFunc) CLASSEMENT_COURANT ); */

    if ( operation -> no_compte == compte_courant )
	verification_mise_a_jour_liste ();

    mise_a_jour_solde ( operation -> no_compte );

    /* on r�affiche les comptes de l'accueil */

    update_liste_comptes_accueil();
    mise_a_jour_soldes_minimaux();
    mise_a_jour_fin_comptes_passifs ();

    p_tab_nom_de_compte_variable = save_ptab;
}
/******************************************************************************/

/******************************************************************************/
/* efface le contenu du formulaire                                            */
/******************************************************************************/
void formulaire_a_zero (void)
{
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

    /* on met les styles des entr�es au gris */

    gtk_widget_set_style ( widget_formulaire_operations[TRANSACTION_FORM_DATE],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ) -> entry,
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ) -> entry,
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] ) -> entry,
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_operations[TRANSACTION_FORM_NOTES],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_operations[TRANSACTION_FORM_BANK],
			   style_entree_formulaire[ENGRIS] );

    gtk_label_set_text ( GTK_LABEL ( widget_formulaire_operations[TRANSACTION_FORM_OP_NB] ),
			 "" );

    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ),
			 _("Date") );

    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ),
			    _("Third party") );
    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ),
			 _("Debit") );
    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ),
			 _("Credit") );

    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ),
			 _("Value date") );
    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ),
			    _("Categories : Sub-categories") );
    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ),
			 _("Cheque/Transfer number") );

    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] ),
			    _("Budgetary line") );
    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] ),
			 _("Voucher") );

    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_NOTES] ),
			 _("Notes") );
    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_BANK] ),
			 _("Bank references") );

    gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_CHANGE] );
    gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] );
    gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN] );

    gtk_label_set_text ( GTK_LABEL ( widget_formulaire_operations[TRANSACTION_FORM_MODE] ),
			 "" );

    gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT],
			       TRUE );
    gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT],
			       TRUE );
    gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY],
			       TRUE );

    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
				  cherche_no_menu_type ( TYPE_DEFAUT_DEBIT ) );
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_DEVISE] ),
				  g_slist_position ( liste_struct_devises,
						     g_slist_find_custom ( liste_struct_devises,
									   GINT_TO_POINTER ( DEVISE ),
									   ( GCompareFunc ) recherche_devise_par_no )));

    gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
			       FALSE );
    gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_DEVISE] ),
			       FALSE );
    gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_EXERCICE] ),
			       FALSE );
    gtk_widget_set_sensitive ( GTK_WIDGET ( hbox_valider_annuler_ope ),
			       FALSE );
    gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET],
			       etat.utilise_imputation_budgetaire );

    gtk_object_set_data ( GTK_OBJECT ( formulaire ),
			  "adr_struct_ope",
			  NULL );
    gtk_object_set_data ( GTK_OBJECT ( formulaire ),
			  "liste_adr_ventilation",
			  NULL );

    gtk_widget_set_sensitive ( bouton_affiche_cache_formulaire,
			       TRUE );
}
/******************************************************************************/

/******************************************************************************/
/* Fonction affiche_cache_le_formulaire                                       */
/* si le formulaire �tait affich�, le cache et vice-versa                     */
/******************************************************************************/
void affiche_cache_le_formulaire ( void )
{
    gpointer **save_ptab;

    save_ptab = p_tab_nom_de_compte_variable;
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

    if ( etat.formulaire_toujours_affiche )
    {
	gtk_widget_hide ( fleche_bas );
	gtk_widget_show ( fleche_haut );

	gtk_widget_hide ( frame_droite_bas );
	etat.formulaire_toujours_affiche = 0;
    }
    else
    {
	GtkAdjustment *ajustement;

	gtk_widget_hide ( fleche_haut );
	gtk_widget_show ( fleche_bas );

	gtk_widget_show ( frame_droite_bas );
	etat.formulaire_toujours_affiche = 1;

	/* 	si apr�s avoir remont� le formulaire la ligne s�lectionn�e est cach�e, */
	/* 	on la remonte pour la mettre juste au dessus du formulaire */

	while ( g_main_iteration ( FALSE ));
	ajustement = gtk_tree_view_get_vadjustment ( GTK_TREE_VIEW ( tree_view_listes_operations ));

	if ( (LIGNE_SELECTIONNEE+NB_LIGNES_OPE)*hauteur_ligne_liste_opes > (ajustement->value + ajustement->page_size))
	    gtk_adjustment_set_value ( ajustement,
				       (LIGNE_SELECTIONNEE+NB_LIGNES_OPE)*hauteur_ligne_liste_opes - ajustement->page_size );
    }

    p_tab_nom_de_compte_variable = save_ptab;
}
/******************************************************************************/



/******************************************************************************/
/* Fonction basculer_vers_ventilation                                         */
/* appel�e par l'appui du bouton Ventilation...                               */
/* permet de voir les op�s ventil�es d'une ventilation                        */
/******************************************************************************/
void basculer_vers_ventilation ( GtkWidget *bouton,
				 gpointer null )
{
    enregistre_ope_au_retour = 0;

    if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ) == style_entree_formulaire[ENCLAIR] )
	ventiler_operation ( -my_strtod ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ))),
					  NULL ));
    else
	ventiler_operation ( my_strtod ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ))),
					 NULL ));
}
/******************************************************************************/

/******************************************************************************/
/* Fonction click_sur_bouton_voir_change  */
/* permet de modifier un change �tabli pour une op�ration */
/******************************************************************************/
void click_sur_bouton_voir_change ( void )
{
    struct structure_operation *operation;
    struct struct_devise *devise;

    gtk_widget_grab_focus ( widget_formulaire_operations[TRANSACTION_FORM_DATE] );

    operation = gtk_object_get_data ( GTK_OBJECT ( formulaire ),
				      "adr_struct_ope" );

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

    if ( !devise_compte ||
	 devise_compte -> no_devise != DEVISE )
	devise_compte = g_slist_find_custom ( liste_struct_devises,
					      GINT_TO_POINTER ( DEVISE ),
					      ( GCompareFunc ) recherche_devise_par_no ) -> data;
    devise = g_slist_find_custom ( liste_struct_devises,
				   GINT_TO_POINTER ( operation -> devise ),
				   ( GCompareFunc ) recherche_devise_par_no ) -> data;

    demande_taux_de_change ( devise_compte, devise,
			     operation -> une_devise_compte_egale_x_devise_ope,
			     operation -> taux_change, operation -> frais_change, 
			     TRUE );

    if ( taux_de_change[0] ||  taux_de_change[1] )
    {
	operation -> taux_change = taux_de_change[0];
	operation -> frais_change = taux_de_change[1];

	if ( operation -> taux_change < 0 )
	{
	    operation -> taux_change = -operation -> taux_change;
	    operation -> une_devise_compte_egale_x_devise_ope = 1;
	}
	else
	    operation -> une_devise_compte_egale_x_devise_ope = 0;
    }
}
/******************************************************************************/

/******************************************************************************/
void degrise_formulaire_operations ( void )
{
    struct struct_type_ope *type;

    gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
			       TRUE );
    gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_DEVISE] ),
			       TRUE );
    gtk_widget_set_sensitive ( GTK_WIDGET ( hbox_valider_annuler_ope ),
			       TRUE );

    /* on ne d�grise l'exo que si le bouton ventiler n'est pas visible */

    if ( !GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN] ))
	gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_EXERCICE] ),
				   etat.utilise_exercice );

    /*   si le type par d�faut est un ch�que, on met le nouveau num�ro */

    if ( GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ))
    {
	type = gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ) -> menu_item ),
				     "adr_type" );

	if ( type -> numerotation_auto )
	{
	    entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] );

	    if ( !strlen ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ))))
		gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ),
				     itoa ( type -> no_en_cours  + 1));
	}
    }
}
/******************************************************************************/

/******************************************************************************/
/**
 *  Increment or decrement the value of a GtkEntry.
 *
 * \param entry Entry to change the value of.
 * \param increment Value to add or substract from the numerical value of entry.
 */
void increment_decrement_champ ( GtkWidget *entry, gint increment )
{
    double number;

    number = my_strtod ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entry ))), NULL );
    number += increment;

    gtk_entry_set_text ( GTK_ENTRY ( entry ), itoa ( number ) );
}
/******************************************************************************/

/******************************************************************************/
/* retrouve la place du type dans option_menu */
/* et la renvoie */
/******************************************************************************/
gint place_type_choix_type ( GtkWidget *option_menu,
			     gint type )
{
    GList *pointeur_liste;
    gint i;

    pointeur_liste = GTK_MENU_SHELL ( GTK_OPTION_MENU ( option_menu ) -> menu ) -> children;
    i=0;

    while ( pointeur_liste )
    {
	if ( GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( pointeur_liste -> data ),
						     "no_type" )) == type )
	    return ( i );

	i++;
	pointeur_liste = pointeur_liste -> next;
    }

    return ( 0 );
}
/******************************************************************************/
