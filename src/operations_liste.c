/* ************************************************************************** */
/*  Fichier qui g�re la liste des op�rations                                  */
/* 			liste_operations.c                                    */
/*                                                                            */
/*     Copyright (C)	2000-2003 C�dric Auger (cedric@grisbi.org)	      */
/*			2004      Benjamin Drieu (bdrieu@april.org) 	      */
/*			2003-2004 Alain Portal (dionysos@grisbi.org) 	      */
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
#include "structures.h"
#include "variables-extern.c"
#include "constants.h"
#include "mouse.h"

/* cette structure est utilis�e pour retrouver la position de la cellule solde sur une op�ration */

struct structure_position_solde
{
    gint ligne;
    gint colonne;
};



#include "accueil.h"
#include "barre_outils.h"
#include "categories_onglet.h"
#include "comptes_traitements.h"
#include "constants.h"
#include "devises.h"
#include "dialog.h"
#include "echeancier_formulaire.h"
#include "echeancier_liste.h"
#include "equilibrage.h"
#include "exercice.h"
#include "imputation_budgetaire.h"
#include "operations_classement.h"
#include "operations_formulaire.h"
#include "operations_liste.h"
#include "operations_onglet.h"
#include "search_glist.h"
#include "tiers_onglet.h"
#include "traitement_variables.h"
#include "type_operations.h"
#include "utils.h"
#include "ventilation.h"
#include "main.h"

GtkJustification col_justs[] = { GTK_JUSTIFY_CENTER,
    GTK_JUSTIFY_CENTER,
    GTK_JUSTIFY_LEFT,
    GTK_JUSTIFY_CENTER,
    GTK_JUSTIFY_RIGHT,
    GTK_JUSTIFY_RIGHT,
    GTK_JUSTIFY_RIGHT };

/* tree_view qui affiche la liste des op�s ventil�es */

GtkWidget *tree_view_liste_ventilations;
GtkWidget *scrolled_window_liste_ventilations;

/* les colonnes de la liste des ventils */

GtkTreeViewColumn *colonnes_liste_ventils[3];

/* hauteur d'une ligne de la liste des op�s */

gint hauteur_ligne_liste_opes;

/* on va essayer de cr�er un object tooltip g�n�ral pour grisbi */
/* donc associer tous les autres tooltips � ce tooltip (FIXME) */

GtkTooltips *tooltips_general_grisbi;

/* le GdkGC correspondant aux lignes, cr��s au d�but une fois pour toute */

GdkGC *gc_separateur_operation;

/* utilis� pour �viter que �a s'emballe lors du r�glage de la largeur des colonnes */

gint allocation_precedente;

/* r�servation d'espace pour un iter, utilis� pour pouvoir passer son adr d'une fonction */
/* � l'autre, sans faire de copies en m�moire � chaque fois */

GtkTreeIter iter_liste_operations;


GtkTreeViewColumn *colonne_classement_tmp;


extern struct operation_echeance *echeance_selectionnnee;
extern gint no_derniere_echeance;
extern GSList *liste_struct_echeances; 
extern gint nb_echeances;
extern GdkColor couleur_fond[2];
extern GdkColor couleur_selection;
extern PangoFontDescription *pango_desc_fonte_liste;
extern GSList *liste_labels_titres_colonnes_liste_ope;
extern GtkWidget *widget_formulaire_ventilation[TRANSACTION_BREAKDOWN_FORM_TOTAL_WIDGET];
extern gint ligne_selectionnee_ventilation;
extern GtkWidget *label_equilibrage_pointe;
extern GtkWidget *label_equilibrage_ecart;
extern gdouble operations_pointees;
extern gdouble solde_initial;
extern gdouble solde_final;
extern GtkWidget *bouton_ok_equilibrage;
extern gint mise_a_jour_liste_comptes_accueil;
extern gint mise_a_jour_soldes_minimaux;
extern gint mise_a_jour_liste_echeances_auto_accueil;
extern gint mise_a_jour_combofix_tiers_necessaire;
extern gint mise_a_jour_combofix_categ_necessaire;
extern gint mise_a_jour_combofix_imputation_necessaire;
extern GtkWidget *bouton_affiche_r;
extern GtkWidget *bouton_enleve_r;
extern gint id_fonction_idle;
extern GtkWidget *bouton_ope_lignes[4];
extern GtkWidget *bouton_grille;



/******************************************************************************/
/*  Routine qui cr�e la fen�tre de la liste des op�  */
/******************************************************************************/
GtkWidget *creation_fenetre_operations ( void )
{
    GtkWidget *win_operations;
    GtkWidget *solde_box;
    GtkWidget *frame;

    /*   la fenetre des op� est une vbox : la liste en haut, le solde et  */
    /*     des boutons de conf au milieu, le formulaire en bas */

    allocation_precedente = 0;

    win_operations = gtk_vbox_new ( FALSE,
				    5 );
    /* cr�ation de la barre d'outils */

    barre_outils = creation_barre_outils ();
    gtk_box_pack_start ( GTK_BOX ( win_operations ),
			 barre_outils,
			 FALSE,
			 FALSE,
			 0);
    gtk_widget_show ( barre_outils );

    /* cr�ation du notebook des op� */

    notebook_listes_operations = creation_tree_view_operations ();
    gtk_box_pack_start ( GTK_BOX ( win_operations ),
			 notebook_listes_operations,
			 TRUE,
			 TRUE,
			 0);
    gtk_widget_show ( notebook_listes_operations );

    /*   cr�ation de la ligne contenant le solde ( sous la liste des op�rations ) et les boutons */

    solde_box = gtk_table_new ( 1,
				3,
				TRUE);

    gtk_box_pack_start ( GTK_BOX ( win_operations ),
			 solde_box,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( solde_box );


    /* mise en forme du solde point� */

    frame = gtk_frame_new ( NULL );
    gtk_frame_set_shadow_type ( GTK_FRAME ( frame ),
				GTK_SHADOW_ETCHED_OUT );
    gtk_table_attach ( GTK_TABLE ( solde_box ),
		       frame,
		       0, 1,
		       0, 1,
		       GTK_EXPAND | GTK_SHRINK,
		       FALSE,
		       0, 0);
    gtk_widget_show ( frame );


    solde_label_pointe = gtk_label_new ( SPACIFY(COLON(_("Reconciled balance"))) );
    gtk_label_set_justify ( GTK_LABEL ( solde_label_pointe ),
			    GTK_JUSTIFY_LEFT);
    gtk_container_add ( GTK_CONTAINER ( frame ),
			solde_label_pointe );
    gtk_widget_show ( solde_label_pointe );


    /* mise en forme du solde courant */

    frame = gtk_frame_new ( NULL );
    gtk_frame_set_shadow_type ( GTK_FRAME ( frame ),
				GTK_SHADOW_ETCHED_OUT );
    gtk_table_attach ( GTK_TABLE ( solde_box ),
		       frame,
		       2, 3,
		       0, 1,
		       GTK_EXPAND | GTK_SHRINK,
		       FALSE,
		       0, 0);
    gtk_widget_show ( frame );


    solde_label = gtk_label_new ( SPACIFY(COLON(_("Curent balance"))) );
    gtk_label_set_justify ( GTK_LABEL ( solde_label ),
			    GTK_JUSTIFY_RIGHT);
    gtk_container_add ( GTK_CONTAINER ( frame ),
			solde_label );
    gtk_widget_show ( solde_label );

    return ( win_operations );
}
/******************************************************************************/

/******************************************************************************/
/* Cr�ation des treeview des op�rations					      */
/* y ajoute les colonnes de la liste des op�s */
/* retour le treeviw */
/******************************************************************************/
GtkWidget *creation_tree_view_operations ( void )
{
    GtkWidget *hbox;
    gint i;
    
    if ( DEBUG )
	printf ( "creation_tree_view_operations\n" );

    hbox = gtk_hbox_new ( FALSE,
			  0 );

/*     1 tree view avec plusieurs list_store est trop lent */
/* 	on cr�e donc une hbox contenant tous les tree_view (1 par compte) */
/* 	les tree_view seront remplis petit Ã����� petit */

    for ( i=0 ; i<nb_comptes ; i++ )
    {

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;
	
	SCROLLED_WINDOW_LISTE_OPERATIONS = gtk_scrolled_window_new ( NULL,
								     NULL );
	gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( SCROLLED_WINDOW_LISTE_OPERATIONS ),
					 GTK_POLICY_AUTOMATIC,
					 GTK_POLICY_AUTOMATIC );
	gtk_box_pack_start ( GTK_BOX ( hbox ),
			     SCROLLED_WINDOW_LISTE_OPERATIONS,
			     TRUE,
			     TRUE,
			     0 );

	/*     cr�ation proprement dite du treeview */

	TREE_VIEW_LISTE_OPERATIONS = gtk_tree_view_new ();
	gtk_container_add ( GTK_CONTAINER ( SCROLLED_WINDOW_LISTE_OPERATIONS ),
			    TREE_VIEW_LISTE_OPERATIONS );
	gtk_widget_show ( TREE_VIEW_LISTE_OPERATIONS );

	/*     on peut s�lectionner plusieurs lignes */

	gtk_tree_selection_set_mode ( GTK_TREE_SELECTION ( gtk_tree_view_get_selection ( GTK_TREE_VIEW( TREE_VIEW_LISTE_OPERATIONS ))),
				      GTK_SELECTION_MULTIPLE );

	/* 	met en place la grille */

	if ( etat.affichage_grille )
	    g_signal_connect_after ( G_OBJECT ( TREE_VIEW_LISTE_OPERATIONS ),
				     "expose-event",
				     G_CALLBACK ( affichage_traits_liste_operation ),
				     NULL );

	/* v�rifie le simple ou double click */

	g_signal_connect ( G_OBJECT ( TREE_VIEW_LISTE_OPERATIONS ),
			   "button_press_event",
			   G_CALLBACK ( selectionne_ligne_souris ),
			   NULL );

	/* v�rifie la touche entr�e, haut et bas */

	g_signal_connect ( G_OBJECT ( TREE_VIEW_LISTE_OPERATIONS ),
			   "key_press_event",
			   G_CALLBACK ( traitement_clavier_liste ),
			   NULL );

	/*     ajuste les colonnes si modification de la taille */

	g_signal_connect ( G_OBJECT ( TREE_VIEW_LISTE_OPERATIONS ),
			   "size-allocate",
			   G_CALLBACK ( changement_taille_liste_ope ),
			   NULL );

	/*     normalement les colonnes sont d�j� cr��s */
	/* mais bon, on teste, sait on jamais... */

	if ( COLONNE_LISTE_OPERATIONS(TRANSACTION_COL_NB_CHECK) )
	{
	    gint j;

	    for ( j = 0 ; j < TRANSACTION_LIST_COL_NB ; j++ )
	    {
		gtk_tree_view_append_column ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ),
					      GTK_TREE_VIEW_COLUMN ( COLONNE_LISTE_OPERATIONS(j) ));
		gtk_tree_view_column_set_clickable ( GTK_TREE_VIEW_COLUMN ( COLONNE_LISTE_OPERATIONS(j) ),
						     TRUE );
		g_signal_connect ( G_OBJECT ( COLONNE_LISTE_OPERATIONS(j) ),
				   "clicked",
				   G_CALLBACK ( click_sur_titre_colonne_operations ),
				   GINT_TO_POINTER (j));
	    }
	    COLONNE_CLASSEMENT = COLONNE_LISTE_OPERATIONS(GPOINTER_TO_INT (COLONNE_CLASSEMENT));
	}
	else
	    printf ( "bizarre, les colonnes d'op�rations n'ont pas encore �t� cr��s (operation_liste.c)...\n" );

	/*     on cr�e le list_store maintenant et on l'associe vide au tree_view */

	/* 	structure de la liste : */
	/* 	    col 0 � 6 -> les donn�es */
	/* 	    col 7 -> la couleur du background */
	/* 	    col 8 -> la couleur du solde */
	/*	    col 9 -> adr de l'op�ration */
	/*	    col 10 -> sauvegarde background quand ligne s�lectionn�e */
	/*	    col 11 -> contient NULL ou l'adr de la pangofontdescription utilis�e */
	/* 	    col 12 -> contient le no de ligne affich�e (par le choix de l'ordre d'affichage des lignes) */

	STORE_LISTE_OPERATIONS = gtk_list_store_new ( 13,
						      G_TYPE_STRING,
						      G_TYPE_STRING,
						      G_TYPE_STRING,
						      G_TYPE_STRING,
						      G_TYPE_STRING,
						      G_TYPE_STRING,
						      G_TYPE_STRING,
						      GDK_TYPE_COLOR,
						      G_TYPE_STRING,
						      G_TYPE_POINTER,
						      GDK_TYPE_COLOR,
						      PANGO_TYPE_FONT_DESCRIPTION,
						      G_TYPE_INT );
	gtk_tree_view_set_model ( GTK_TREE_VIEW (TREE_VIEW_LISTE_OPERATIONS),
				  GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ));
	SLIST_DERNIERE_OPE_AJOUTEE = NULL;
    }

    /*     on cr�e maintenant la liste de ventilation */

    scrolled_window_liste_ventilations = gtk_scrolled_window_new ( NULL,
								   NULL );
    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_window_liste_ventilations ),
				     GTK_POLICY_AUTOMATIC,
				     GTK_POLICY_AUTOMATIC );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 scrolled_window_liste_ventilations,
			 TRUE,
			 TRUE,
			 0 );


    tree_view_liste_ventilations = gtk_tree_view_new ();
    if ( etat.affichage_grille )
	g_signal_connect_after ( G_OBJECT ( tree_view_liste_ventilations ),
				 "expose-event",
				 G_CALLBACK ( affichage_traits_liste_ventilation ),
				 NULL );
    gtk_container_add ( GTK_CONTAINER ( scrolled_window_liste_ventilations ),
			tree_view_liste_ventilations );
    gtk_widget_show ( tree_view_liste_ventilations );

    /*     on peut s�lectionner plusieurs lignes */

    gtk_tree_selection_set_mode ( GTK_TREE_SELECTION ( gtk_tree_view_get_selection ( GTK_TREE_VIEW( tree_view_liste_ventilations ))),
				  GTK_SELECTION_MULTIPLE );

    /* v�rifie le simple ou double click */

    g_signal_connect ( G_OBJECT ( tree_view_liste_ventilations ),
		       "button_press_event",
		       G_CALLBACK ( selectionne_ligne_souris_ventilation ),
		       NULL );

    /* v�rifie la touche entr�e, haut et bas */

    g_signal_connect ( G_OBJECT ( tree_view_liste_ventilations ),
		       "key_press_event",
		       G_CALLBACK ( traitement_clavier_liste_ventilation ),
		       NULL );

    /*     ajuste les colonnes si modification de la taille */

    g_signal_connect ( G_OBJECT ( tree_view_liste_ventilations ),
		       "size-allocate",
		       G_CALLBACK ( changement_taille_liste_ope_ventilation ),
		       NULL );

    /* 	on ajoute tout de suite les colonnes de la ventil, mais elles sont invisibles */

    if ( colonnes_liste_ventils[0] )
	for ( i=0 ; i<3 ; i++ )
	    gtk_tree_view_append_column ( GTK_TREE_VIEW ( tree_view_liste_ventilations ),
					  colonnes_liste_ventils[i] );
    else
	printf ( "bizarre, les colonnes de ventilation n'ont pas encore �t� cr��s (operation_liste.c)...\n" );


    /*     la hauteur est mise � 0 pour l'instant, elle sera remplie d�s que n�cessaire */

    hauteur_ligne_liste_opes = 0;
    gc_separateur_operation = NULL;

    return ( hbox );
}
/******************************************************************************/


/******************************************************************************/
/* cette fonction est appel�e pour cr�er les tree_view_column des listes d'op� et */
/* de ventil */
/******************************************************************************/
void creation_titres_tree_view ( void )
{
    gint i, j;
    gfloat alignement[] = {
	COLUMN_CENTER,
	COLUMN_CENTER,
	COLUMN_LEFT,
	COLUMN_CENTER,
	COLUMN_RIGHT,
	COLUMN_RIGHT,
	COLUMN_RIGHT
    };
    gchar *titres_liste_ventil[] = { 
	_("Category"),
	_("Notes"),
	_("Amount")
    };
    gfloat alignement_ventil[] = {
	COLUMN_LEFT,
	COLUMN_LEFT,
	COLUMN_RIGHT
    };
    struct structure_position_solde *position_solde;


    if ( !titres_colonnes_liste_operations )
	return;

    if ( !tooltips_general_grisbi )
	tooltips_general_grisbi = gtk_tooltips_new ();

    /*     on r�cup�re la position de cellule du solde pour pouvoir le mettre en rouge */

    position_solde = recherche_position_solde ();

    /*     on commence par s'occuper des listes d'op�rations */

    for ( j=0 ; j<nb_comptes ; j++ )
    {
	
	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + j;

	for ( i = 0 ; i < TRANSACTION_LIST_COL_NB ; i++ )
	{
	    COLONNE_LISTE_OPERATIONS(i) = gtk_tree_view_column_new_with_attributes ( titres_colonnes_liste_operations[i],
										     gtk_cell_renderer_text_new (),
										     "text", i,
										     "background-gdk", 7,
										     "font-desc", 11,
										     NULL );
	    gtk_tree_view_column_set_alignment ( GTK_TREE_VIEW_COLUMN ( COLONNE_LISTE_OPERATIONS(i) ),
						 alignement[i] );
	    gtk_tree_view_column_set_sizing ( GTK_TREE_VIEW_COLUMN ( COLONNE_LISTE_OPERATIONS(i) ),
					      GTK_TREE_VIEW_COLUMN_FIXED );

	    if ( etat.largeur_auto_colonnes )
		gtk_tree_view_column_set_resizable ( COLONNE_LISTE_OPERATIONS(i),
						     FALSE );
	    else
		gtk_tree_view_column_set_resizable ( COLONNE_LISTE_OPERATIONS(i),
						     TRUE );

	}

	/*     pour la colonne du solde, on rajoute le foreground */

	if ( position_solde -> colonne != -1 )
	    gtk_tree_view_column_add_attribute ( COLONNE_LISTE_OPERATIONS(position_solde -> colonne),
						 gtk_tree_view_column_get_cell_renderers ( COLONNE_LISTE_OPERATIONS(position_solde -> colonne) ) -> data,
						 "foreground", 8 );
    }

    /*    on cr�e les titres de la liste de ventilation  */

    for ( i=0 ; i<3 ; i++ )
    {
	colonnes_liste_ventils[i] = gtk_tree_view_column_new_with_attributes ( titres_liste_ventil[i],
									       gtk_cell_renderer_text_new (),
									       "text", i,
									       "background-gdk", 4,
									       "font-desc", 6,
									       NULL );
	gtk_tree_view_column_set_alignment ( GTK_TREE_VIEW_COLUMN ( colonnes_liste_ventils[i] ),
					     alignement_ventil[i] );
	gtk_tree_view_column_set_sizing ( GTK_TREE_VIEW_COLUMN ( colonnes_liste_ventils[i] ),
					  GTK_TREE_VIEW_COLUMN_FIXED );
    }
}
/******************************************************************************/


/******************************************************************************/
/* cette fonction est appel�e une fois que les titres et tips pour les col */
/* de la liste d'op� ont �t� cr��s. donc soit on les cr�e, soit on les update */
/******************************************************************************/
void update_titres_tree_view ( void )
{
    gint i;
    gint j;

    if ( !titres_colonnes_liste_operations )
	return;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

    if ( !COLONNE_LISTE_OPERATIONS(TRANSACTION_COL_NB_CHECK) )
	creation_titres_tree_view ();


    /*     on s'occupe des listes d'op�rations */

    for ( j=0 ; j<nb_comptes ; j++ )
    {
	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + j;

	for ( i = 0 ; i < TRANSACTION_LIST_COL_NB ; i++ )
	{
	    gtk_tree_view_column_set_title ( GTK_TREE_VIEW_COLUMN ( COLONNE_LISTE_OPERATIONS(i) ),
					     titres_colonnes_liste_operations[i] );

	    if ( GTK_TREE_VIEW_COLUMN ( COLONNE_LISTE_OPERATIONS(i) )->button )
	    {
		gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi ),
				       GTK_TREE_VIEW_COLUMN ( COLONNE_LISTE_OPERATIONS(i) )->button,
				       tips_col_liste_operations[i],
				       tips_col_liste_operations[i] ); 
	    }
	}
    }
}
/******************************************************************************/



/******************************************************************************/
/* cette fonction v�rifie que la liste d'op� du compte demand�e est coml�tement */
/* termin�e. elle est normalement appel�e en cas de changement de comptef */
/* si la liste n'est pas finie, on la termine ici */
/******************************************************************************/
void verification_list_store_termine ( gint no_compte )
{
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;

    if ( SLIST_DERNIERE_OPE_AJOUTEE != GINT_TO_POINTER (-1))
	ajoute_operations_compte_dans_list_store ( no_compte,
						   0 );

    if ( !COULEUR_BACKGROUND_FINI )
	update_couleurs_background ( no_compte,
				     NULL  );

    if ( !AFFICHAGE_SOLDE_FINI )
	update_soldes_list_store ( no_compte,
				   NULL );

    if ( !SELECTION_OPERATION_FINI )
	selectionne_ligne ( OPERATION_SELECTIONNEE );

    return;
}
/******************************************************************************/





/******************************************************************************/
/* remplissage du list_store du compte donn� en argument */
/* cette fonction efface l'ancienne liste et la remplit avec les donn�es actuelles */
/* elle remplit la liste d'un seul coup */
/* elle met � jour les soldes courant et point�s */
/******************************************************************************/
void remplissage_liste_operations ( gint compte )
{
    if ( DEBUG )
	printf ( "remplissage_liste_operations compte %d\n", compte );

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte;

    /*     on efface la liste */
	
    gtk_list_store_clear ( STORE_LISTE_OPERATIONS );
    
    /*     plus rien n'est affich� */

    SLIST_DERNIERE_OPE_AJOUTEE = NULL;
    COULEUR_BACKGROUND_FINI = 0;
/*     AFFICHAGE_SOLDE_FINI = 0; */
    SELECTION_OPERATION_FINI = 0;

    /*     on remplit le list_store */

    ajoute_operations_compte_dans_list_store ( compte,
					       0 );

    /*     on met les couleurs du fond */

    update_couleurs_background ( compte,
				 NULL  );

    /*     on met les soldes */
	
    update_soldes_list_store ( compte,
			       NULL );

    /*     on s�lectionne la ligne blanche */

    selectionne_ligne ( GINT_TO_POINTER(-1) );
}
/******************************************************************************/


/******************************************************************************/
/* cette fonction ajoute les op�s du compte donn� en argument dans son list_store */
/* si OPE_EN_COURS != 0, on commence � cette op� */
/* si par_partie = 1, on renvoie la main apr�s n op�rations */
/******************************************************************************/
void ajoute_operations_compte_dans_list_store ( gint compte,
						gint par_partie )
{
    GSList *liste_operations_tmp;
    gint nb_max_ope_a_ajouter;
    gint nb_ope_ajoutees;
    gint i, j;

    /*     on d�cide pour l'instant arbitrairement du nb d'op� � ajouter si on */
    /* 	le fait par partie ; peut �tre dans les pr�f�rences un jour */

    if ( par_partie )
	nb_max_ope_a_ajouter = 20;
    else
	nb_max_ope_a_ajouter = -1;

    nb_ope_ajoutees = 0;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte;

    /*     si SLIST_DERNIERE_OPE_AJOUTEE n'est pas nul, on se place sur l'op� suivante */

    if ( SLIST_DERNIERE_OPE_AJOUTEE )
    {
	liste_operations_tmp = SLIST_DERNIERE_OPE_AJOUTEE;
	liste_operations_tmp = liste_operations_tmp -> next;
    }
    else
	liste_operations_tmp = LISTE_OPERATIONS;

    /*     on commence la boucle  */

    while ( liste_operations_tmp
	    &&
	    ( nb_ope_ajoutees < nb_max_ope_a_ajouter
	      ||
	      nb_max_ope_a_ajouter == -1))
    {
	struct structure_operation *operation;

	operation = liste_operations_tmp -> data;

	/* 	    on met tout de suite l'op� en cours pour permettre de reprendre � la suivante lors */
	/* 		du remplissage de la liste */

	SLIST_DERNIERE_OPE_AJOUTEE = liste_operations_tmp;
		    
	/* si c'est une op� de ventilation, ou si elle est relev�e mais que l'on ne veut pas les afficher */
	/* 	on saute l'op� */

	if ( !(operation -> no_operation_ventilee_associee
	       ||
	       ( !AFFICHAGE_R
		 &&
		 operation -> pointe == 3 )))
	{
	    /* 	    on a d�cid� d'afficher cette op� */

	    nb_ope_ajoutees++;

	    /* 	    on ajoute l'op� � la fin du list_store */

	    remplit_ligne_operation ( operation,
				      GINT_TO_POINTER (-1));
	}
	liste_operations_tmp = liste_operations_tmp -> next;
    }

    /*     si liste_operations_tmp n'est pas null, c'est qu'on n'est pas all� jusqu'au bout */
    /* 	des op�s du comptes, donc on s'en va maintenant */

    if ( liste_operations_tmp )
	return;

    /* affiche la ligne blanche du bas */

    for ( j = 0 ; j < NB_LIGNES_OPE ; j++ )
    {
	/* on met � NULL tout les pointeurs */

	gtk_list_store_append ( STORE_LISTE_OPERATIONS,
				&iter_liste_operations );

	for ( i=0 ; i<TRANSACTION_LIST_COL_NB ; i++ )
	    gtk_list_store_set ( STORE_LISTE_OPERATIONS,
				 &iter_liste_operations,
				 i, NULL,
				 -1 );

	/* on met le no d'op�ration de cette ligne � -1 */

	gtk_list_store_set ( STORE_LISTE_OPERATIONS,
			     &iter_liste_operations,
			     9, GINT_TO_POINTER (-1),
			     -1 );
    }

    /*     on est all� jusqu'au bout du compte */

    SLIST_DERNIERE_OPE_AJOUTEE = GINT_TO_POINTER ( -1 );
}
/******************************************************************************/





/******************************************************************************/
/* cette fonction affiche l'op�ration donn�e en argument � la position iter */
/* dans le store du compte de l'op�ration */
/* elle ajoute de 1 � 4 lignes mais ne se pr�ocupe pas du solde en cours*/
/* si iter = -1, on en cr�e un avec un append (dans le cas de remplissage complet de la liste) */
/* si iter = NULL, on remplace l'affichage de l'op� donn�e (et si l'op� n'est pas d�j� affich�e, on ne fait rien ) */
/* si iter=adr d'iter, on insÃ�re l'op�ration devant l'iter donn� */
/******************************************************************************/
void remplit_ligne_operation ( struct structure_operation *operation,
			       GtkTreeIter *iter )
{
    gint i;
    GtkTreeIter iter_liste;
    
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

    /* on fait le tour de tab_affichage_ope pour remplir les lignes du tableau */

    for ( i = 0 ; i < TRANSACTION_LIST_ROWS_NB ; i++ )
    {
	/* on ne passe que si la ligne doit �tre affich�e */

	if ( !i 
	     ||
	     NB_LIGNES_OPE == 4
	     ||
	     ( ( i == 1 || i == 2 )
	       &&
	       NB_LIGNES_OPE == 3 )
	     ||
	     ( i == 1 && NB_LIGNES_OPE == 2 ))
	{
	    gint ligne_affichee;


	    /* 	    en fonction de la demande, soit on ajoute la ligne � la fin du list_store, */
	    /* 	    soit on l'ajoute devant l'iter de l'op� donn� en arg */
	    /* 	    soit on remplace l'op� donn�e en argument */

	    if ( iter == GINT_TO_POINTER (-1))
		gtk_list_store_append ( STORE_LISTE_OPERATIONS,
					&iter_liste );
	    else
	    {
		if ( iter )
		    gtk_list_store_insert_before ( STORE_LISTE_OPERATIONS,
						   &iter_liste,
						   iter );
		else
		{
		    /*    on va remplacer l'op� � l'iter courant */
		    /*donc si on est sur la premi�re ligne, on change pas l'iter */
		    /* sinon c'est un iter next */

		    if ( i )
			gtk_tree_model_iter_next ( GTK_TREE_MODEL (STORE_LISTE_OPERATIONS),
						   &iter_liste );
		    else
		    {
			GtkTreeIter *iter_ope;

			iter_ope = cherche_iter_operation (operation);

			if ( iter_ope )
			    memcpy  ( &iter_liste,
				      iter_ope,
				      sizeof ( GtkTreeIter ));
			else
			    /*   l'op�ration n'est pas affich�e sur cette liste... on fait rien */
			    return;
		    }
		}
	    }

	    /*  en fonction de i (la ligne en cours) et NB_LIGNES_OPE, on retrouve la ligne qu'il faut */
	    /* afficher selon les configurations */

	    switch ( NB_LIGNES_OPE )
	    {
		case 1:
		    ligne_affichee = ligne_affichage_une_ligne;
		    break;

		case 2:
		    if ( i )
			ligne_affichee = GPOINTER_TO_INT ( lignes_affichage_deux_lignes -> next -> data );
		    else
			ligne_affichee = GPOINTER_TO_INT ( lignes_affichage_deux_lignes -> data );
		    break;

		case 3:
		    switch ( i )
		    {
			case 0:
			    ligne_affichee = GPOINTER_TO_INT ( lignes_affichage_trois_lignes -> data );
			    break;

			case 1:
			    ligne_affichee = GPOINTER_TO_INT ( lignes_affichage_trois_lignes -> next -> data );
			    break;

			case 2:
			    ligne_affichee = GPOINTER_TO_INT ( lignes_affichage_trois_lignes -> next -> next -> data );
			    break;

			default:
			    ligne_affichee = 0;
		    }
		    break;

		case 4:
		    ligne_affichee = i;
		    break;

		default:
		    ligne_affichee = 0;
	    }

	    /* 	    on a positionn� l'iter o� on voulait mettre l'op� ainsi que la ligne � afficher */
	    /* 		on peut afficher les infos */

	    affiche_ligne_ope ( operation,
				&iter_liste,
				ligne_affichee );
	}
    }
}
/******************************************************************************/







/******************************************************************************/
/* cette fonction r�cup�re les donn�es � afficher pour la ligne donn�e en argument */
/* de l'op�ration, et les �crit sur l'iter */
/******************************************************************************/
void affiche_ligne_ope ( struct structure_operation *operation,
			 GtkTreeIter *iter,
			 gint ligne )
{
    gint i;
    gchar *ligne_list[TRANSACTION_LIST_ROWS_NB][TRANSACTION_LIST_COL_NB];


    /*  on remplit la ligne de str avec les donn�es ou NULL */
	
    for ( i = 0 ; i < TRANSACTION_LIST_COL_NB ; i++ )
	ligne_list[ligne][i] = recherche_contenu_cellule ( operation,
							   tab_affichage_ope[ligne][i]  );

    /* on affiche la ligne compos�e de TRANSACTION_LIST_COL_NB colonnes */
    /* 	   utilisation d'un for plut�t que de tout mettre d'un coup pour permettre */
    /* 	une �volution : nb de colonnes variables */
    /* 	    si c'est la cellule du solde (-1), on ne fait rien, en on n'efface pas non plus */

    for ( i=0 ; i<TRANSACTION_LIST_COL_NB ; i++ )
	if ( ligne_list[ligne][i] != GINT_TO_POINTER (-1))
	    gtk_list_store_set ( STORE_LISTE_OPERATIONS,
				 iter,
				 i, ligne_list[ligne][i],
				 -1 );

    /* on associe � cette ligne l'adr de la struct de l'op� */

    gtk_list_store_set ( STORE_LISTE_OPERATIONS,
			 iter,
			 9, operation,
			 -1 );

    /* 		    si on utilise une fonte perso, c'est ici */

    if ( etat.utilise_fonte_listes )
	gtk_list_store_set ( STORE_LISTE_OPERATIONS,
			     iter,
			     11, pango_desc_fonte_liste,
			     -1 );

    /*     on y ajoute le no de ligne */

    gtk_list_store_set ( STORE_LISTE_OPERATIONS,
			 iter,
			 12, ligne,
			 -1 );
}
/******************************************************************************/



/******************************************************************************/
/* Fonction recherche_contenu_cellule */
/* prend en argument l'op�ration concern�e */
/* et le num�ro de l'argument qu'on veut afficher (tab_affichage_ope) */
/* renvoie la chaine � afficher ou NULL */
/******************************************************************************/
gchar *recherche_contenu_cellule ( struct structure_operation *operation,
				   gint no_affichage )
{
    gchar *temp;
    gdouble montant;

    switch ( no_affichage )
    {
	case 0:
	    return ( NULL );
	    break;

	    /* mise en forme de la date */

	case 1:
	    /* FIXME: should use locale + strftime for that */
	    return ( g_strdup_printf ("%02d/%02d/%04d", 
				      operation -> jour,
				      operation -> mois,
				      operation -> annee ));
	    break;

	    /* mise en forme de la date de valeur */

	case 2:
	    if ( operation -> jour_bancaire )
		/* FIXME: should use locale + strftime for that */
		return ( g_strdup_printf ("%02d/%02d/%04d",
					  operation -> jour_bancaire,
					  operation -> mois_bancaire,
					  operation -> annee_bancaire ));
	    else
		return ( NULL );
	    break;

	    /* mise en forme du tiers */

	case 3:
	    return ( tiers_name_by_no ( operation -> tiers, TRUE ));
	    break;

	    /* mise en forme de l'ib */

	case 4:

	    temp = nom_imputation_par_no ( operation -> imputation,
					   operation -> sous_imputation );
	    break;


	    /* mise en forme du d�bit */
	case 5:
	    if ( operation -> montant < 0 )
	    {
		temp = g_strdup_printf ( "%4.2f", -operation -> montant );

		/* si la devise en cours est diff�rente de celle de l'op�, on la retrouve */

		if ( !devise_operation
		     ||
		     devise_operation -> no_devise != operation -> devise )
		    devise_operation = devise_par_no ( operation -> devise );

		if ( devise_operation -> no_devise != DEVISE )
		    temp = g_strconcat ( temp,
					 "(",
					 devise_code ( devise_operation ),
					 ")",
					 NULL );

		return ( temp );
	    }
	    else
		return ( NULL );

	    break;

	    /* mise en forme du cr�dit */
	case 6:
	    if ( operation -> montant >= 0 )
	    {
		temp = g_strdup_printf ( "%4.2f", operation -> montant );

		/* si la devise en cours est diff�rente de celle de l'op�, on la retrouve */

		if ( !devise_operation
		     ||
		     devise_operation -> no_devise != operation -> devise )
		    devise_operation = devise_par_no ( operation -> devise );

		if ( devise_operation -> no_devise != DEVISE )
		    temp = g_strconcat ( temp,
					 "(",
					 devise_code ( devise_operation ),
					 ")",
					 NULL );

		return ( temp );
	    }
	    else
		return ( NULL );

	    break;

	case 7:
	    /* mise en forme du solde */
	    /* 	     on renvoie -1 car le solde sera affich� plus tard */
	    /* 	    et le -1 permet de ne pas effacer la cellule qui contient le */
	    /* 		solde (utile en cas de modif d'op�) */
	    /* 	    de plus, on met AFFICHAGE_SOLDE_FINI � 0 comme �a il sera affich� plus tard */
	    
	    AFFICHAGE_SOLDE_FINI = 0;
	    return GINT_TO_POINTER (-1);
	    break;

	    /* mise en forme du montant dans la devise du compte */

	case 8:
	    if ( operation -> devise != DEVISE )
	    {
		/* on doit calculer et afficher le montant de l'opÃ������ */

		montant = calcule_montant_devise_renvoi ( operation -> montant,
							  DEVISE,
							  operation -> devise,
							  operation -> une_devise_compte_egale_x_devise_ope,
							  operation -> taux_change,
							  operation -> frais_change );

		return ( g_strdup_printf ( "(%4.2f %s)",
					   montant,
					   devise_code ( devise_compte ) ));
	    }
	    else
		return ( NULL );

	    break;

	    /* mise en forme du moyen de paiement */

	case 9:
	    return ( type_ope_name_by_no ( operation -> type_ope,
					   operation -> no_compte ));
	    break;

	    /* mise en forme du no de rapprochement */

	case 10:
	    return ( rapprochement_name_by_no ( operation -> no_rapprochement ));
	    break;

	    /* mise en place de l'exo */

	case 11:
	    return ( exercice_name_by_no (  operation -> no_exercice ));
	    break;

	    /* mise en place des cat�gories */

	case 12:
	    temp = NULL;

	    if ( operation -> categorie )
		/* c'est une categ : ss categ */
		temp = nom_categ_par_no ( operation -> categorie,
					  operation -> sous_categorie );
	    else
	    {
		if ( operation -> relation_no_operation )
		{
		    /* c'est un virement */

		    if ( operation -> relation_no_compte == -1 )
		    {
			if ( operation -> montant < 0 )
			    temp = _("Transfer to a deleted account");
			else
			    temp = _("Transfer from a deleted account");
		    }
		    else
		    {
			gpointer **save_ptab;

			save_ptab = p_tab_nom_de_compte_variable;
			p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

			if ( operation -> montant < 0 )
			    temp = g_strdup_printf ( _("Transfer to %s"),
						     NOM_DU_COMPTE );
			else
			    temp = g_strdup_printf ( _("Transfer from %s"),
						     NOM_DU_COMPTE );

			p_tab_nom_de_compte_variable = save_ptab;
		    }
		}
		else
		{
		    /* v�rification si l'op�ration est ventil�e */

		    if ( operation -> operation_ventilee )
			temp = _("Breakdown of transaction");
		}
	    }
	    return ( temp );

	    break;

	    /* mise en forme R/P */

	case 13:
	    if ( operation -> pointe == 1 )
		return ( _("P") );
	    else
	    {
		if ( operation -> pointe == 2 )
		    return ( _("T") );
		else
		{
		    if ( operation -> pointe == 3 )
			return ( _("R"));
		    else
			return ( NULL );
		}
	    }
	    break;


	    /* mise en place de la pi�ce comptable */

	case 14:
	    return ( operation -> no_piece_comptable );
	    break;

	    /* mise en forme des notes */

	case 15:
	    return ( operation -> notes );
	    break;

	    /* mise en place de l'info banque/guichet */

	case 16:
	    return ( operation -> info_banque_guichet );
	    break;

	    /* mise en place du no d'op� */

	case 17:
	    return ( itoa ( operation -> no_operation ));
	    break;

	    /* mise en place du no de ch�que/virement */

	case 18:

	    if ( operation -> contenu_type )
		return ( g_strconcat ( "(",
				       operation -> contenu_type,
				       ")",
				       NULL ));
	    else
		return ( NULL );
	    break;
    }
    return ( NULL );
}
/******************************************************************************/


/******************************************************************************/
/* cette fonction met � jour les couleurs du background sur une */
/* list_store d�j� remplie */
/* si iter n'est pas nul, on commence la mise � jour � partir de cet iter */
/******************************************************************************/

void update_couleurs_background ( gint compte,
				  GtkTreeIter *iter_debut )
{
    GtkTreeIter *iter;
    gboolean result_iter;
    gint couleur_en_cours;


    if ( DEBUG )
	printf ( "update_couleurs_background compte %d\n", compte );

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte;
    
    if ( iter_debut )
    {
	result_iter = 1;
	iter = iter_debut;
	couleur_en_cours =( my_atoi ( gtk_tree_model_get_string_from_iter ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
									   iter )) / NB_LIGNES_OPE ) % 2;
    }
    else
    {
	result_iter = gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
						      &iter_liste_operations );
	iter = &iter_liste_operations;
	couleur_en_cours = 0;
    }


    while ( result_iter )
    {
	gint i;

	for ( i=0 ; i<NB_LIGNES_OPE ; i++ )
	{
	    if ( result_iter )
	    {
		gtk_list_store_set ( STORE_LISTE_OPERATIONS,
				     iter,
				     7, &couleur_fond[couleur_en_cours],
				     -1 );
		result_iter = gtk_tree_model_iter_next ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
							 iter );
	    }
	}
	couleur_en_cours = 1 - couleur_en_cours;
   }

    COULEUR_BACKGROUND_FINI = 1;

    /*     on a vir� la s�lection en remettant la couleur de fond */

    SELECTION_OPERATION_FINI = 0;
}
/******************************************************************************/



/******************************************************************************/
/* cette fonction affiche dans le list_store le solde courant � chaque op� */
/* et mes ce solde en rouge s'il est n�gatif */
/* si iter n'est pas nul, on commence la mise � jour � partir de cet iter */
/******************************************************************************/

void update_soldes_list_store ( gint compte,
				GtkTreeIter *iter_debut )
{
    GtkTreeIter *iter;
    gboolean result_iter;
    gdouble solde_courant;
    struct structure_position_solde *position_solde;

    if ( DEBUG )
	printf ( "update_soldes_list_store compte %d\n", compte );



    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte;

    /*     on commence par retrouver la colonne et la ligne pour afficher le solde de l'op� */
    
    position_solde = recherche_position_solde ();
    
    if ( position_solde -> ligne == -1 
	 ||
	 position_solde -> ligne >= NB_LIGNES_OPE )
    {
	/* 	le solde n'est pas affich�, on peut repartir */
	AFFICHAGE_SOLDE_FINI = 1;
	return;
    }

    /*     on a maintenant la position du solde, et on est s�r qu'il sera affich� */

    /*     on peut commencer � faire le tour du list_store */


    if ( iter_debut )
    {
	gint ligne_solde_precedent;
	GtkTreeIter iter_ligne_solde_precedent;
	gchar *str_tmp = NULL;

	result_iter = 1;
	iter = iter_debut;
	
	/* 	on reprend le solde au niveau de l'op� pr�c�dente */

	ligne_solde_precedent = my_atoi ( gtk_tree_model_get_string_from_iter ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
										iter )) - NB_LIGNES_OPE + position_solde -> ligne;

	/* 	si on est sur la premi�re ligne, on reprend le solde du d�but */

	if ( ligne_solde_precedent < 0 )
	    solde_courant = solde_debut_affichage ( compte );
	else
	{
	    if ( gtk_tree_model_get_iter_from_string ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
						       &iter_ligne_solde_precedent,
						       itoa ( ligne_solde_precedent )))
		gtk_tree_model_get ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
				     &iter_ligne_solde_precedent,
				     position_solde -> colonne, &str_tmp,
				     -1 );

	    solde_courant = my_strtod ( str_tmp,
					NULL );
	}
    }
    else
    {
	result_iter = gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
						      &iter_liste_operations );
	iter = &iter_liste_operations;

	/*     on calcule le solde de d�marrage */

	solde_courant = solde_debut_affichage ( compte );
    }

    while ( result_iter )
    {
	gint i;
	struct structure_operation *operation;

	/* 	on se place sur la bonne ligne */
	
	for ( i=0 ; i<position_solde -> ligne ; i++ )
	    result_iter = gtk_tree_model_iter_next ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
						     iter );


	gtk_tree_model_get ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
			     iter,
			     9, &operation,
			     -1 );

	if ( operation != GINT_TO_POINTER (-1))
	{
	    solde_courant = solde_courant + calcule_montant_devise_renvoi ( operation -> montant,
									    DEVISE,
									    operation -> devise,
									    operation -> une_devise_compte_egale_x_devise_ope,
									    operation -> taux_change,
									    operation -> frais_change );

	    gtk_list_store_set ( STORE_LISTE_OPERATIONS,
				 iter,
				 position_solde -> colonne, g_strdup_printf ( "%4.2f",
							    solde_courant ),
				 -1 );

	    /* 	on met la couleur du solde */

	    if ( solde_courant > -0.01 )
		gtk_list_store_set ( STORE_LISTE_OPERATIONS,
				     iter,
				     8, NULL,
				     -1 );

	    else
		gtk_list_store_set ( STORE_LISTE_OPERATIONS,
				     iter,
				     8, "red",
				     -1 );
	}


	/* 	on se place sur la prochaine op� */

	for ( i=0 ; i<(NB_LIGNES_OPE - position_solde -> ligne ) ; i++ )
	    result_iter = gtk_tree_model_iter_next ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
						     iter );
   }
    AFFICHAGE_SOLDE_FINI = 1;
}
/******************************************************************************/


/******************************************************************************/
/* cette fonction recherche la place du solde sur une ligne d'op� selon les pr�f�rences */
/* de l'utilisateur. elle renvoie une structure contenant la ligne et la colonne. */
/* ces valeurs sont mises � -1 si non trouv�es */
/******************************************************************************/
struct structure_position_solde *recherche_position_solde ( void )
{
    gint ligne;
    gint colonne;
    gint trouve = 0;
    struct structure_position_solde *position_solde;

    position_solde = malloc ( sizeof ( struct structure_position_solde ));


    ligne = -1;
    do
    {
	ligne++;
	colonne = -1;
	do
	{
	    colonne++;
	    if ( tab_affichage_ope[ligne][colonne] == 7 )
		trouve = 1;
	}
	while ( colonne < 7
		&&
		!trouve );
    }
    while ( ligne < 4
	    &&
	    !trouve );

    if ( trouve )
    {
	position_solde -> ligne = ligne;
	position_solde -> colonne = colonne;
    }
    else
    {
	position_solde -> ligne = -1;
	position_solde -> colonne = -1;
    }

    return position_solde;
}
/******************************************************************************/




/******************************************************************************/
/* cette fonction calcule le solde de d�part pour l'affichage de la premi�re op� */
/* du compte */
/* c'est soit le solde initial du compte si on affiche les R */
/* soit le solde initial - les op�s R si elles ne sont pas affich�es */
/******************************************************************************/
gdouble solde_debut_affichage ( gint no_compte )
{
    gdouble solde;
    GSList *liste_tmp;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;

    solde = SOLDE_INIT;

    if ( AFFICHAGE_R )
	return (solde);

    /*     les R ne sont pas affich�s, on les d�duit du solde initial */

    liste_tmp = LISTE_OPERATIONS;

    while ( liste_tmp )
    {
	struct structure_operation *operation;

	operation = liste_tmp -> data;

	/* 	si l'op� est ventil�e ou non relev�e, on saute */

	if ( !(operation -> no_operation_ventilee_associee
	       ||
	       operation -> pointe != 3 ))
	solde = solde + calcule_montant_devise_renvoi ( operation -> montant,
							DEVISE,
							operation -> devise,
							operation -> une_devise_compte_egale_x_devise_ope,
							operation -> taux_change,
							operation -> frais_change );
	liste_tmp = liste_tmp -> next;
    }

    return ( solde );
}
/******************************************************************************/


/******************************************************************************/
/* cette fonction calcule et renvoie le solde du compte donn� en argument */
/******************************************************************************/
gdouble calcule_solde_compte ( gint no_compte )
{
    gdouble solde;
    GSList *liste_tmp;

    if ( DEBUG )
	printf ( "calcule_solde_compte %d\n", no_compte );

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;

    solde = SOLDE_INIT;

    liste_tmp = LISTE_OPERATIONS;

    while ( liste_tmp )
    {
	struct structure_operation *operation;

	operation = liste_tmp -> data;

	/* 	si l'op� est ventil�e, on saute */

	if ( !operation -> no_operation_ventilee_associee )
	solde = solde + calcule_montant_devise_renvoi ( operation -> montant,
							DEVISE,
							operation -> devise,
							operation -> une_devise_compte_egale_x_devise_ope,
							operation -> taux_change,
							operation -> frais_change );
	liste_tmp = liste_tmp -> next;
    }

    return ( solde );
}
/******************************************************************************/



/******************************************************************************/
/* cette fonction calcule et renvoie le solde point� du compte donn� en argument */
/******************************************************************************/
gdouble calcule_solde_pointe_compte ( gint no_compte )
{
    gdouble solde;
    GSList *liste_tmp;

    if ( DEBUG )
	printf ( "calcule_solde_pointe_compte %d\n", no_compte );

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;

    solde = SOLDE_INIT;

    liste_tmp = LISTE_OPERATIONS;

    while ( liste_tmp )
    {
	struct structure_operation *operation;

	operation = liste_tmp -> data;

	/* 	si l'op� est ventilée ou non point�e, on saute */

	if ( operation -> pointe
	     &&
	     !operation -> no_operation_ventilee_associee )
	solde = solde + calcule_montant_devise_renvoi ( operation -> montant,
							DEVISE,
							operation -> devise,
							operation -> une_devise_compte_egale_x_devise_ope,
							operation -> taux_change,
							operation -> frais_change );
	liste_tmp = liste_tmp -> next;
    }

    return ( solde );
}
/******************************************************************************/



/******************************************************************************/
/* Fonction selectionne_ligne_souris */
/* place la s�lection sur l'op� click�e */
/******************************************************************************/
gboolean selectionne_ligne_souris ( GtkWidget *tree_view,
				    GdkEventButton *evenement )
{
    gint x, y;
    gint ligne, colonne;
    GtkTreePath *path;
    GtkTreeViewColumn *tree_colonne;

    /* si le click se situe dans les menus, c'est qu'on redimensionne, on fait rien */

    if ( evenement -> window != gtk_tree_view_get_bin_window ( GTK_TREE_VIEW ( tree_view )) )
	return(FALSE);

    /* R�cup�ration des coordonn�es de la souris */


    gdk_window_get_pointer ( gtk_tree_view_get_bin_window ( GTK_TREE_VIEW ( tree_view )),
			     &x,
			     &y,
			     FALSE );

    /*     on r�cup�re le path aux coordonn�es */
    /* 	si ce n'est pas une ligne de la liste, on se barre */

    if ( !gtk_tree_view_get_path_at_pos ( GTK_TREE_VIEW ( tree_view ),
					  x,
					  y,
					  &path,
					  &tree_colonne,
					  NULL,
					  NULL ))
    {
	/* 	�ventuellement, si c'est un clic du bouton droit, on affiche la popup partielle */
	if ( evenement -> button == RIGHT_BUTTON )
	    popup_transaction_context_menu ( FALSE );

	return (TRUE);
    }

    /* R�cup�ration de la 1�re ligne de l'op�ration cliqu�e */

    ligne = my_atoi ( gtk_tree_path_to_string ( path ));
    ligne = ligne / NB_LIGNES_OPE * NB_LIGNES_OPE;

    selectionne_ligne( cherche_operation_from_ligne ( ligne,
						      compte_courant ));

    /*     si on est sur la ligne blanche et qu'on a fait un clic droit, on met le menu contextuel adapt� */

    if ( evenement -> button == RIGHT_BUTTON )
    {
	struct structure_operation *operation = NULL;
	GtkTreeIter iter;

	gtk_tree_model_get_iter ( GTK_TREE_MODEL ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( tree_view ))),
				  &iter,
				  path );

	gtk_tree_model_get ( GTK_TREE_MODEL ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( tree_view ))),
			     &iter,
			     9, &operation,
			     -1 );

	if ( operation == GINT_TO_POINTER (-1))
	{
	    popup_transaction_context_menu ( FALSE );
	    return(TRUE);
	}
    }

    /*     on r�cup�re la colonne cliqu�e au cas o� on clique sur les P */

    colonne = g_list_index ( gtk_tree_view_get_columns ( GTK_TREE_VIEW ( tree_view )),
			     tree_colonne );

    /*     si on est en train d'�quilibrer, on fait le boulot */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

    if ( etat.equilibrage 
	 &&
	 colonne == TRANSACTION_COL_NB_PR 
	 &&
	 !(ligne % NB_LIGNES_OPE) )
	pointe_equilibrage ( ligne );

    /* si on a cliqu� sur la colonne P/R alors que la touche CTRL
       est enfonc�e, alors on (d�)pointe l'op�ration */

    if ( ( ( evenement -> state & GDK_CONTROL_MASK ) == GDK_CONTROL_MASK )
	 &&
	 colonne == TRANSACTION_COL_NB_PR )
	p_press ();

    /*  si on a double-cliqu� ou bouton droit sur une op�ration, c'est ici */

    if ( evenement -> type == GDK_2BUTTON_PRESS )
	edition_operation ();
    else
	if ( evenement -> button == RIGHT_BUTTON )
	    popup_transaction_context_menu ( TRUE );

    return ( TRUE );
}
/******************************************************************************/

/******************************************************************************/
/* Fonction traitement_clavier_liste */
/* g�re le clavier sur la liste des op�s */
/******************************************************************************/
gboolean traitement_clavier_liste ( GtkWidget *widget_variable,
				    GdkEventKey *evenement )
{
    gint ligne_selectionnee;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

    switch ( evenement -> keyval )
    {
	case GDK_Return :		/* entr�e */
	case GDK_KP_Enter :

	    edition_operation ();
	    break;

	case GDK_Up :		/* touches fl�che haut */
	case GDK_KP_Up :

	    ligne_selectionnee = cherche_ligne_operation ( OPERATION_SELECTIONNEE );

	    if ( ligne_selectionnee )
		selectionne_ligne ( cherche_operation_from_ligne ( ligne_selectionnee - NB_LIGNES_OPE,
								   compte_courant ));
	    return TRUE;

	case GDK_Down :		/* touches fl�che bas */
	case GDK_KP_Down :

	    ligne_selectionnee = cherche_ligne_operation ( OPERATION_SELECTIONNEE );

	    if ( ligne_selectionnee
		 !=
		 (GTK_LIST_STORE ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS )))->length - NB_LIGNES_OPE))
		selectionne_ligne ( cherche_operation_from_ligne ( ligne_selectionnee + NB_LIGNES_OPE,
								   compte_courant ));
	    return TRUE;

	case GDK_Delete:		/*  del  */
	    supprime_operation ( OPERATION_SELECTIONNEE );
	    return TRUE;

	case GDK_P:			/* touche P */
	case GDK_p:			/* touche p */

	    if ( ( evenement -> state & GDK_CONTROL_MASK ) == GDK_CONTROL_MASK )
		p_press ();
	    break;

	case GDK_r:			/* touche r */
	case GDK_R:			/* touche R */

	    if ( ( evenement -> state & GDK_CONTROL_MASK ) == GDK_CONTROL_MASK )
		r_press ();
	    break;

	default: 
	    return TRUE;
    }

    return TRUE;
}
/******************************************************************************/

/******************************************************************************/
/* Routine qui s�lectionne l'op�ration de la ligne envoy�e en argument */
/******************************************************************************/
void selectionne_ligne ( struct structure_operation *nouvelle_operation_selectionnee )
{
    GtkTreeIter *iter;
    gint i;
    GdkColor *couleur;

    if ( DEBUG )
    {
	if ( nouvelle_operation_selectionnee == GINT_TO_POINTER (-1))
	    printf ( "selectionne_ligne blanche\n");
	else
	    printf ( "selectionne_ligne %d\n", nouvelle_operation_selectionnee->no_operation );
    }


    /*     on ne place p_tab_nom_de_compte_variable que si la nouvelle op� est != -1 */
    /* 	sinon, normalement p_tab a �t� plac� avant */
    /* 	donc si plante ici, voir la fonction qui a appel� selectionne_ligne */

    if ( nouvelle_operation_selectionnee != GINT_TO_POINTER (-1))
	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + nouvelle_operation_selectionnee -> no_compte;
    else
	verification_p_tab ("selectionne_ligne");


    /*     si SELECTION_OPERATION_FINI=0, c'est qu'il n'y a encore aucune s�lection sur la liste */
    /*     donc nouvelle_operation_selectionnee = OPERATION_SELECTIONNEE = -1, mais on ne se barre pas */
    /*     sinon si on est d�j� dessus, on se barre */

    if ( nouvelle_operation_selectionnee == OPERATION_SELECTIONNEE
	 &&
	 SELECTION_OPERATION_FINI )
	return;

    /*   vire l'ancienne s�lection : consiste � remettre la couleur d'origine du background */

    if ( SELECTION_OPERATION_FINI )
    {
	iter = cherche_iter_operation ( OPERATION_SELECTIONNEE );

	/* 	iter est maintenant positionn� sur la 1�re ligne de l'op� � d�s�lectionner */

	for ( i=0 ; i<NB_LIGNES_OPE ; i++ )
	{
	    gtk_tree_model_get ( GTK_TREE_MODEL ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
				 iter,
				 10, &couleur,
				 -1 );
	    gtk_list_store_set ( GTK_LIST_STORE ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
				 iter,
				 7,couleur,
				 -1 );
	    gtk_list_store_set ( GTK_LIST_STORE ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
				 iter,
				 10, NULL,
				 -1 );

	    gtk_tree_model_iter_next ( GTK_TREE_MODEL ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
				       iter );
	}
    }

    OPERATION_SELECTIONNEE = nouvelle_operation_selectionnee;

    iter = cherche_iter_operation ( OPERATION_SELECTIONNEE );

    /* 	iter est maintenant positionné sur la 1�re ligne de l'opÃ� � s�lectionner */

    for ( i=0 ; i<NB_LIGNES_OPE ; i++ )
    {
	gtk_tree_model_get ( GTK_TREE_MODEL ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
			     iter,
			     7, &couleur,
			     -1 );
	gtk_list_store_set ( GTK_LIST_STORE ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
			     iter,
			     7, &couleur_selection,
			     -1 );
	gtk_list_store_set ( GTK_LIST_STORE ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
			     iter,
			     10, couleur,
			     -1 );

	gtk_tree_model_iter_next ( GTK_TREE_MODEL ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
				   iter );
    }

    /*     on d�place le scrolling de la liste si n�cessaire pour afficher la s�lection */
    /*     on prend le no compte � partir de p_tab_nom_de_compte_variable au cas o� l'op� */
    /* 	s�lectionn�e est -1 */

    ajuste_scrolling_liste_operations_a_selection ( p_tab_nom_de_compte_variable - p_tab_nom_de_compte );

    SELECTION_OPERATION_FINI = 1;
}
/******************************************************************************/


/******************************************************************************/
void ajuste_scrolling_liste_operations_a_selection ( gint compte )
{
    GtkAdjustment *v_adjustment;
    gint y_ligne;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte;
    
    /*     si on n'a pas encore r�cup�r� la hauteur des lignes, on va le faire ici */

    if ( !hauteur_ligne_liste_opes )
	hauteur_ligne_liste_opes = recupere_hauteur_ligne_tree_view ( TREE_VIEW_LISTE_OPERATIONS );

    v_adjustment = gtk_tree_view_get_vadjustment ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ));

    y_ligne = cherche_ligne_operation ( OPERATION_SELECTIONNEE ) * hauteur_ligne_liste_opes;

    /*     si l'op� est trop haute, on la rentre et la met en haut */

    if ( y_ligne < v_adjustment -> value )
	gtk_adjustment_set_value ( GTK_ADJUSTMENT ( v_adjustment ),
				   y_ligne );
    else
	if ( (y_ligne + hauteur_ligne_liste_opes*NB_LIGNES_OPE ) > ( v_adjustment -> value + v_adjustment -> page_size ))
	    gtk_adjustment_set_value ( GTK_ADJUSTMENT ( v_adjustment ),
				       y_ligne + hauteur_ligne_liste_opes*NB_LIGNES_OPE - v_adjustment -> page_size );

}
/******************************************************************************/



/******************************************************************************/
gint recupere_hauteur_ligne_tree_view ( GtkWidget *tree_view )
{
    GdkRectangle rectangle;

    gtk_tree_view_get_background_area ( GTK_TREE_VIEW ( tree_view ),
					gtk_tree_path_new_from_string ( "1" ),
					NULL,
					&rectangle );
    return ( rectangle.height );
}
/******************************************************************************/




/******************************************************************************/
/* renvoie l'adr de l'op�ration correspondant  � la ligne envoy�es */
/* en argument */
/******************************************************************************/
struct structure_operation *cherche_operation_from_ligne ( gint ligne,
							   gint no_compte )
{
    GtkTreeIter iter;
    struct structure_operation *operation;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;

    if ( !gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
					       &iter,
					       itoa (ligne)))
	return NULL;

    gtk_tree_model_get ( GTK_TREE_MODEL ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
			 &iter,
			 9, &operation,
			 -1 );

    return ( operation );

}
/******************************************************************************/


/******************************************************************************/
/* cette fonction renvoie l'adr de l'op�ration correspondant � l'iter donn� en argument */
/******************************************************************************/
struct structure_operation *operation_from_iter ( GtkTreeIter *iter,
						  gint no_compte )
{
    struct structure_operation *operation = NULL;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;

    if ( gtk_list_store_iter_is_valid ( STORE_LISTE_OPERATIONS,
					iter ))
	gtk_tree_model_get ( GTK_TREE_MODEL (STORE_LISTE_OPERATIONS),
			     iter,
			     9, &operation,
			     -1 );

    return ( operation );
}
/******************************************************************************/


/******************************************************************************/
/* fonction cherche_iter_operation */
/* retrouve l'iter correspondant � l'op�ration donn�e en argument dans la tree_view des op�rations */
/* renvoie null si pas trouv� */
/******************************************************************************/
GtkTreeIter *cherche_iter_operation ( struct structure_operation *operation )
{
    struct structure_operation *operation_tmp;
    GtkTreeIter iter;

    if ( !operation )
	return NULL;

    /*     si l'est la ligne blanche qui est demand�e (ope=-1), p_tab doit �tre fix� avant... */

    if ( operation != GINT_TO_POINTER (-1))
	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;
    else
	verification_p_tab ("cherche_iter_operation");


    /*     on va faire le tour de la liste, et d�s qu'une op� = operation */
    /* 	on retourne son iter */

    gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
				    &iter );
    operation_tmp = NULL;

    do
    {
	gtk_tree_model_get ( GTK_TREE_MODEL ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
			     &iter,
			     9, &operation_tmp,
			     -1 );
    }
    while ( operation_tmp != operation
	    &&
	    gtk_tree_model_iter_next ( GTK_TREE_MODEL ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
				       &iter ));

    if ( operation_tmp == operation )
	return ( gtk_tree_iter_copy ( &iter ));
    else
	return NULL;

}
/******************************************************************************/




/******************************************************************************/
/* cette fonction renvoie le no de ligne de l'op�ration en argument */
/******************************************************************************/
gint cherche_ligne_operation ( struct structure_operation *operation )
{
    GtkTreeIter *iter;

    /*     si c'est la ligne blanche qui est demand�e, p_tab a d� �tre plac� sur le compte avant */

    if ( operation != GINT_TO_POINTER (-1))
	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;
    else
	verification_p_tab ("cherche_ligne_operation");

    iter = cherche_iter_operation ( operation );

    return ( my_atoi ( gtk_tree_model_get_string_from_iter (  GTK_TREE_MODEL ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
							      iter )));
}
/******************************************************************************/




/******************************************************************************/
/* Fonction edition_operation */
/* appel�e lors d'un double click sur une ligne ou entree */
/* place l'op�ration s�lectionn�e dans le formulaire */
/******************************************************************************/
void edition_operation ( void )
{
    struct structure_operation *operation;
    gchar *date ;
    gchar *date_bancaire ;
    struct struct_devise *devise;
    gchar *char_temp;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

    operation = OPERATION_SELECTIONNEE;
    formulaire_a_zero ();
    
    /* on affiche le formulaire sans modifier l'�tat => si il n'est pas affich� normalement,
       il sera efface lors du prochain formulaire_a_zero */

    if ( !etat.formulaire_toujours_affiche )
	gtk_widget_show ( frame_droite_bas );

    gtk_widget_set_sensitive ( bouton_affiche_cache_formulaire, FALSE );

    degrise_formulaire_operations ();

    /* si l'op� est -1, c'est que c'est une nouvelle op� */

    if ( operation == GINT_TO_POINTER ( -1 ) )
    {
	if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) == style_entree_formulaire[ENGRIS] )
	{
	    entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_DATE] );

	    if ( gtk_widget_get_style ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ) == style_entree_formulaire[ENGRIS] )
	    {
		gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ),
				     gsb_today() );
	    }
	}
	gtk_entry_select_region ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ), 0, -1);
	gtk_widget_grab_focus ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ) );
	return;
    }

    /*   l'op� n'est pas -1, c'est une modif, on remplit les champs */


    gtk_object_set_data ( GTK_OBJECT ( formulaire ),
			  "adr_struct_ope",
			  operation );


    /* on met le no de l'op� */

    gtk_label_set_text ( GTK_LABEL ( widget_formulaire_operations[TRANSACTION_FORM_OP_NB] ),
			 itoa ( operation -> no_operation ));

    /* mise en forme de la date */

    date = g_strdup_printf ( "%02d/%02d/%04d",
			     g_date_day ( operation -> date ),
			     g_date_month ( operation -> date ),
			     g_date_year ( operation -> date ) );

    entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_DATE] );
    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ),
			 date );

    /* mise en forme du tiers */

    if ( operation -> tiers )
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] );
	gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ),
				tiers_name_by_no ( operation -> tiers, TRUE ));
    }

    /* mise en forme du d�bit / cr�dit */

    if ( operation -> montant < 0 )
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] );
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ),
			     g_strdup_printf ( "%4.2f", -operation -> montant ));
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
		gtk_widget_hide ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] );
	}
    }
    else
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] );
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ),
			     g_strdup_printf ( "%4.2f", operation -> montant ));
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

    /* si l'op�ration est relev�e, on d�sensitive les entr�es de cr�dit et d�bit */

    if ( operation -> pointe == 3 )
    {
	gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT],
				   FALSE );
	gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT],
				   FALSE );
    }

    /* mise en forme de la devise */

    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_DEVISE] ),
				  g_slist_index ( liste_struct_devises,
						  devise_par_no ( operation -> devise )));

    /*   si la devise n'est pas celle du compte ni l'euro si le compte va y passer, affiche le bouton change */

    if ( !devise_compte
	 ||
	 devise_compte -> no_devise != DEVISE )
	devise_compte = devise_par_no ( DEVISE );

    devise = devise_par_no ( operation -> devise );

    if ( !( devise -> no_devise == DEVISE
	    ||
	    ( devise_compte -> passage_euro && !strcmp ( devise -> nom_devise, _("Euro") ))
	    ||
	    ( !strcmp ( devise_compte -> nom_devise, _("Euro") ) && devise -> passage_euro )))
	gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_CHANGE] );

    /* mise en forme des cat�gories */

    char_temp = nom_categ_par_no ( operation -> categorie,
				   operation -> sous_categorie );

    if ( char_temp )
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] );
	gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ),
				char_temp );
    }

    /* mise en forme de la date r�elle */

    if ( operation -> date_bancaire )
    {
	date_bancaire = g_strdup_printf ( "%02d/%02d/%04d",
					  g_date_day ( operation -> date_bancaire ),
					  g_date_month ( operation -> date_bancaire ),
					  g_date_year ( operation -> date_bancaire ) );

	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] );

	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ),
			     date_bancaire );
    }

    /* si l'op�ration est li�e, marque le virement */
    /* et si la contre op�ration est relev�e, on d�sensitive la categ et le montant */

    if ( operation -> relation_no_operation )
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] );

	if ( operation -> relation_no_compte == -1 )
	    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ),
				    _("Transfer: deleted account") );
	else
	{
	    GtkWidget *menu;
	    struct structure_operation *operation_2;

	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

	    /* on met le nom du compte du virement */

	    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ),
				    g_strconcat ( COLON(_("Transfer")),
						  NOM_DU_COMPTE,
						  NULL ));

	    /* si l'op�ration est relev�e, on emp�che le changement de virement */

	    if ( operation -> pointe == 3 )
		gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY],
					   FALSE );

	    /* r�cup�ration de la contre op�ration */

	    operation_2 = operation_par_no ( operation -> relation_no_operation,
					     operation -> relation_no_compte );

	    /* 	  si la contre op�ration est relev�e, on d�sensitive les categ et les montants */
	    if ( operation_2
		 &&
		 operation_2 -> pointe == 3 )
	    {
		gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT],
					   FALSE );
		gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT],
					   FALSE );
		gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY],
					   FALSE );
	    }

	    /* comme c'est un virement, on affiche s'il existe l'option menu du type de l'autre op� */

	    if ( operation -> montant >= 0 )
		menu = creation_menu_types ( 1, operation -> relation_no_compte, 2  );
	    else
		menu = creation_menu_types ( 2, operation -> relation_no_compte, 2  );

	    /*  on ne continue que si un menu a �t� cr�� */
	    /*    dans ce cas, on va chercher l'autre op� et retrouve le type */

	    if ( menu )
	    {
		gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ),
					   menu );
		gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] ),
					      cherche_no_menu_type_associe ( operation_2 -> type_ope,
									     0 ));
		gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_CONTRA] );
	    }
	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;
	}
    }

    /* mise en forme si l'op�ration est ventil�e */

    if ( operation -> operation_ventilee )
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] );
	gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ),
				_("Breakdown of transaction") );
	gtk_widget_show ( widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN] );
	gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_EXERCICE],
				   FALSE );
	gtk_widget_set_sensitive ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET],
				   FALSE );

	/* met la liste des op�s de ventilation dans liste_adr_ventilation */

	gtk_object_set_data ( GTK_OBJECT ( formulaire ),
			      "liste_adr_ventilation",
			      creation_liste_ope_de_ventil ( operation ));

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;
    }

    /* met l'option menu du type d'op� */

    if ( GTK_WIDGET_VISIBLE ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ))
    {
	struct struct_type_ope *type;
	
	gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
				      cherche_no_menu_type ( operation -> type_ope ));

	type = type_ope_par_no ( operation -> type_ope,
				 compte_courant );

	if ( type
	   &&
	   type -> affiche_entree
	   &&
	   operation -> contenu_type )
	    {
		entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] );
		gtk_entry_set_text ( GTK_ENTRY (widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ),
				     operation -> contenu_type );
	    }
    }

    /* met en place l'exercice */

    gtk_option_menu_set_history (  GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_EXERCICE] ),
				   cherche_no_menu_exercice ( operation -> no_exercice,
							      widget_formulaire_operations[TRANSACTION_FORM_EXERCICE] ));

    /* met en place l'imputation budg�taire */
    /* si c'est une op� ventil�e, on met rien */

    if ( !operation -> operation_ventilee )
    {
	gchar *char_tmp;

	char_tmp = nom_imputation_par_no ( operation -> imputation,
					   operation -> sous_imputation );
	if ( char_tmp )
	{
	    entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET]);
	    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] ),
				    char_tmp );
	}
    }

    /* mise en place de la pi�ce comptable */

    if ( operation -> no_piece_comptable )
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] );
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] ),
			     operation -> no_piece_comptable );
    }

    /* remplit les notes */

    if ( operation -> notes )
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_NOTES] );
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_NOTES] ),
			     operation -> notes );
    }

    /* remplit les infos guichet / banque */

    if ( operation -> info_banque_guichet )
    {
	entree_prend_focus ( widget_formulaire_operations[TRANSACTION_FORM_BANK] );
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_BANK] ),
			     operation -> info_banque_guichet );
    }

    /* mise en forme de auto / man */

    if ( operation -> auto_man )
	gtk_label_set_text ( GTK_LABEL ( widget_formulaire_operations[TRANSACTION_FORM_MODE]),
			     _("Auto"));
    else
	gtk_label_set_text ( GTK_LABEL ( widget_formulaire_operations[TRANSACTION_FORM_MODE]),
			     _("Manual"));

    /*   on a fini de remplir le formulaire, on donne le focus � la date */

    gtk_entry_select_region ( GTK_ENTRY ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ),
			      0,
			      -1);
    gtk_widget_grab_focus ( widget_formulaire_operations[TRANSACTION_FORM_DATE] );
}
/******************************************************************************/

/******************************************************************************/
/* Fonction p_press */
/* appel�e lorsque la touche p est press�e sur la liste */
/* pointe ou d�pointe l'op�ration courante */
/******************************************************************************/
void p_press (void)
{
    gdouble montant;
    struct structure_operation *operation;
    GtkTreeIter *iter;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

    operation = OPERATION_SELECTIONNEE;

    /* si on est sur l'op�ration vide -> on se barre */

    if (operation  == GINT_TO_POINTER ( -1 )
	||
	operation -> pointe == 3 )
	return;
    
    iter = cherche_iter_operation ( OPERATION_SELECTIONNEE );

    if ( operation -> pointe )
    {
	montant = calcule_montant_devise_renvoi ( operation -> montant,
						  DEVISE,
						  operation -> devise,
						  operation -> une_devise_compte_egale_x_devise_ope,
						  operation -> taux_change,
						  operation -> frais_change );

	if ( etat.equilibrage )
	    operations_pointees = operations_pointees - montant;

	SOLDE_POINTE = SOLDE_POINTE - montant;
	operation -> pointe = 0;

	gtk_list_store_set ( GTK_LIST_STORE ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
			     iter,
			     3, NULL,
			     -1 );
    }
    else
    {
	montant = calcule_montant_devise_renvoi ( operation -> montant,
						  DEVISE,
						  operation -> devise,
						  operation-> une_devise_compte_egale_x_devise_ope,
						  operation-> taux_change,
						  operation-> frais_change );

	if ( etat.equilibrage )
	    operations_pointees = operations_pointees + montant;

	SOLDE_POINTE = SOLDE_POINTE + montant;
	operation -> pointe = 1;

	gtk_list_store_set ( GTK_LIST_STORE ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
			     iter,
			     3, _("P"),
			     -1 );
    }

    /* si c'est une op� ventil�e, on recherche les op� filles pour leur mettre le m�me pointage que la m�re */

    if ( operation -> operation_ventilee )
    {
	/* p_tab est d�j� point� sur le compte courant */

	GSList *liste_tmp;

	liste_tmp = LISTE_OPERATIONS;

	while ( liste_tmp )
	{
	    struct structure_operation *ope_fille;

	    ope_fille = liste_tmp -> data;

	    if ( ope_fille -> no_operation_ventilee_associee == operation -> no_operation )
		ope_fille -> pointe = operation -> pointe;

	    liste_tmp = liste_tmp -> next;
	}
    }

    if ( etat.equilibrage )
    {
	gtk_label_set_text ( GTK_LABEL ( label_equilibrage_pointe ),
			     g_strdup_printf ("%4.2f",
					      operations_pointees ));

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


/*     met � jour les labels des soldes  */

    mise_a_jour_labels_soldes ();

    modification_fichier( TRUE );
/* ALAIN-FIXME : solution batarde me semble-t'il pour actualiser le solde point�
       sur la fen�tre d'accueil apr�s que l'on ait point� l'op�ration */

    mise_a_jour_liste_comptes_accueil = 1;
}
/******************************************************************************/

/******************************************************************************/
/* Fonction r_press */
/* appel�e lorsque la touche r est press�e sur la liste */
/* rel�ve ou d�rel�ve l'op�ration courante */
/******************************************************************************/
void r_press (void)
{
    struct structure_operation *operation;
    GtkTreeIter *iter;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

    operation = OPERATION_SELECTIONNEE;


    /* si on est sur l'op�ration vide -> on se barre */

    if ( operation == GINT_TO_POINTER ( -1 ))
	return;

    iter = cherche_iter_operation ( OPERATION_SELECTIONNEE );

    if ( !operation -> pointe )
    {
	/* on rel�ve l'op�ration */

	operation -> pointe = 3;

	/* on met soit le R, soit on change la s�lection vers l'op� suivante */

	if ( AFFICHAGE_R )
	    gtk_list_store_set ( GTK_LIST_STORE ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
				 iter,
				 3, _("R"),
				 -1 );
	else
	{
	    /*  l'op�ration va disparaitre, on met donc la s�lection sur l'op� suivante */

	    OPERATION_SELECTIONNEE = cherche_operation_from_ligne ( cherche_ligne_operation ( OPERATION_SELECTIONNEE ) + NB_LIGNES_OPE,
								compte_courant );
 	    remplissage_liste_operations ( compte_courant );
 	}

	modification_fichier( TRUE );
    }
    else
	if ( operation -> pointe == 3 )
	{
	    /* d�-rel�ve l'op�ration */

	    gtk_list_store_set ( GTK_LIST_STORE ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))),
				 iter,
				 3, NULL,
				 -1 );

	    modification_fichier( TRUE );
	}

    /* si c'est une ventilation, on fait le tour des op�rations du compte pour */
    /* rechercher les op�rations de ventilation associ�es � cette ventilation */

    if ( operation -> operation_ventilee )
    {
	GSList *liste_tmp;

	liste_tmp = LISTE_OPERATIONS;

	while ( liste_tmp )
	{
	    struct structure_operation *operation;

	    operation = liste_tmp -> data;

	    if ( operation -> no_operation_ventilee_associee == operation -> no_operation )
		operation -> pointe = operation -> pointe;

	    liste_tmp = liste_tmp -> next;
	}
    }
}
/******************************************************************************/

/******************************************************************************/
/*  Routine qui supprime l'op�ration donn�e en argument */
/******************************************************************************/
void supprime_operation ( struct structure_operation *operation )
{
    GSList *pointeur_tmp;
    struct structure_operation *ope_tmp;
    GtkTreeIter *iter;
    gdouble montant;
    gint i;

    /* v�rifications de bases */

    if ( !operation
	 ||
	 operation == GINT_TO_POINTER ( -1 ) )
	return;


    /* l'op�ration ne doit pas �tre relev�e */

    if ( operation -> pointe == 3 )
    {
	dialogue_error ( _("Imposible to delete a reconciled transaction.") );
	return;
    }

    /* si l'op�ration est li�e, on recherche l'autre op�ration, */
    /* on vire ses liaisons et on l'efface */
    /* sauf si elle est relev�e, dans ce cas on annule tout */

    if ( operation -> relation_no_operation && operation -> relation_no_compte != -1 )
    {
	ope_tmp = operation_par_no ( operation -> relation_no_operation,
				     operation -> relation_no_compte );

	if ( ope_tmp )
	{
	    if ( ope_tmp -> pointe == 3 )
	    {
		dialogue_error ( _("The contra-transaction of this transfer is reconciled, deletion impossible.") );
		return;
	    }

	    ope_tmp -> relation_no_operation = 0;
	    ope_tmp -> relation_no_compte = 0;

	    supprime_operation ( ope_tmp );
	}
    }

    /* si c'est une ventilation, on fait le tour de ses op�rations */
    /* de ventilation pour v�rifier qu'il n'y en a pas une qui est un virement */
    /* vers une op�ration relev�e */

    if ( operation -> operation_ventilee )
    {
	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

	pointeur_tmp = LISTE_OPERATIONS;

	while ( pointeur_tmp )
	{
	    ope_tmp = pointeur_tmp -> data;

	    if ( ope_tmp -> no_operation_ventilee_associee == operation -> no_operation )
	    {
		/* ope_tmp est une op�ration de ventilation de l'op�ration */
		/* � supprimer, on recherche si c'est un virement */

		if ( ope_tmp -> relation_no_operation )
		{
		    /* c'est un virement, on va voir la contre op�ration */

		    struct structure_operation *contre_operation;

		    contre_operation = operation_par_no ( ope_tmp -> relation_no_operation,
							  ope_tmp -> relation_no_compte );

		    if ( contre_operation
			 &&
			 contre_operation -> pointe == 3 )
		    {
			dialogue_error ( _("One of the breakdown lines is a transfer whose contra-transaction is reconciled.  Deletion canceled."));
			return;
		    }
		}
	    }
	    pointeur_tmp = pointeur_tmp -> next;
	}
    }

    /* les tests sont pass�s, si c'est une ventilation, */
    /* on vire toutes les op�rations associ�es */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

    if ( operation -> operation_ventilee )
    {
	pointeur_tmp = LISTE_OPERATIONS;

	while ( pointeur_tmp )
	{
	    ope_tmp = pointeur_tmp -> data;

	    if ( ope_tmp -> no_operation_ventilee_associee == operation -> no_operation )
	    {
		/* on se place tout de suite sur l'op� suivante */

		pointeur_tmp = pointeur_tmp -> next;

		supprime_operation ( ope_tmp );
	    }
	    else
		pointeur_tmp = pointeur_tmp -> next;
	}
    }


    /*     on se met sur l'iter de l'op� et on retir l'op� de la liste */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;
    iter = cherche_iter_operation ( operation );

    if ( iter )
    {
	/* si la s�lection est sur l'op�ration qu'on supprime, */
	/* on met la s�lection sur celle du dessous */

	if ( OPERATION_SELECTIONNEE == operation )
	    OPERATION_SELECTIONNEE = cherche_operation_from_ligne ( cherche_ligne_operation ( OPERATION_SELECTIONNEE ) + NB_LIGNES_OPE,
								    operation -> no_compte );

	for ( i=0 ; i<NB_LIGNES_OPE ; i++ )
	    gtk_list_store_remove ( GTK_LIST_STORE ( STORE_LISTE_OPERATIONS ),
				    iter );

	/*     on met � jour les couleurs et les soldes */

	update_couleurs_background ( operation -> no_compte,
				     gtk_tree_iter_copy (iter));
	update_soldes_list_store ( operation -> no_compte,
				   iter );
	selectionne_ligne ( OPERATION_SELECTIONNEE );
    }

    /*     calcul des nouveaux soldes */

    montant = calcule_montant_devise_renvoi ( operation -> montant,
					      DEVISE,
					      operation -> devise,
					      operation -> une_devise_compte_egale_x_devise_ope,
					      operation -> taux_change,
					      operation -> frais_change );

    SOLDE_COURANT = SOLDE_COURANT - montant;

    if ( operation -> pointe )
	SOLDE_POINTE = SOLDE_POINTE - montant;

    /*     on met � jour les labels de solde */

    mise_a_jour_labels_soldes ();

    /* si on est en train d'�quilibrer => recalcule le total point� */

    if ( etat.equilibrage )
	calcule_total_pointe_compte ( operation -> no_compte );

    /* supprime l'op�ration dans la liste des op�s */

    LISTE_OPERATIONS = g_slist_remove ( LISTE_OPERATIONS,
					operation );

    /* on r�affiche la liste de l'�tat des comptes de l'accueil */

    mise_a_jour_liste_comptes_accueil = 1;
    mise_a_jour_soldes_minimaux = 1;
    affiche_dialogue_soldes_minimaux ();


    /* FIXME : on devrait r�afficher les listes de tiers, categ, ib... */

    modification_fichier( TRUE );
}
/******************************************************************************/


/******************************************************************************/
/* Fonction changement_taille_liste_ope					      */
/* appel�e d�s que la taille de la clist a chang�			      */
/* pour mettre la taille des diff�rentes colonnes			      */
/******************************************************************************/
gboolean changement_taille_liste_ope ( GtkWidget *tree_view,
				       GtkAllocation *allocation )
{
    gint i, j;
    gint col0, col1, col2, col3, col4, col5, col6;

    /*     pour �viter que le syst�me ne s'emballe... */

    if ( allocation -> width
	 ==
	 allocation_precedente )
	return FALSE;

    allocation_precedente = allocation -> width;

    /* si la largeur est automatique, on change la largeur des colonnes */
    /* sinon, on y met les valeurs fixes */

    for ( j=0 ; j<nb_comptes ; j++ )
    {
	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + j;

	if ( etat.largeur_auto_colonnes )
	    for ( i = 0 ; i < TRANSACTION_LIST_COL_NB ; i++ )
		gtk_tree_view_column_set_fixed_width ( COLONNE_LISTE_OPERATIONS(i),
						       rapport_largeur_colonnes[i] * allocation_precedente / 100 );
	else
	    for ( i = 0 ; i < TRANSACTION_LIST_COL_NB ; i++ )
		if ( taille_largeur_colonnes[i] )
		    gtk_tree_view_column_set_fixed_width ( COLONNE_LISTE_OPERATIONS(i),
							   taille_largeur_colonnes[i]  );
    }

    /* met les entr�es du formulaire selon une taille proportionnelle */

    col0 = allocation_precedente * 5 / 100;
    col1 = allocation_precedente * 12 / 100;
    col2 = allocation_precedente * 30 / 100;
    col3 = allocation_precedente * 12 / 100;
    col4 = allocation_precedente * 12 / 100;
    col5 = allocation_precedente * 12 / 100;
    col6 = allocation_precedente * 12 / 100;

    /* 1�re ligne */

    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_OP_NB] ),
			   col0,
			   FALSE );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_DATE] ),
			   col1,
			   FALSE );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_PARTY] ),
			   col2,
			   FALSE  );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_DEBIT] ),
			   col3,
			   FALSE  );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_CREDIT] ),
			   col4,
			   FALSE  );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_DEVISE] ),
			   col5,
			   FALSE  );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_CHANGE] ),
			   col6,
			   FALSE  );

    /* 2�me ligne */

    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_VALUE_DATE] ),
			   col0+col1,
			   FALSE );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_CATEGORY] ),
			   col2,
			   FALSE );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_TYPE] ),
			   col3+col4,
			   FALSE  );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_CHEQUE] ),
			   col5,
			   FALSE  );

    /* 3�me ligne */

    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_EXERCICE] ),
			   col0+col1,
			   FALSE );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_BUDGET] ),
			   col2,
			   FALSE );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_VOUCHER] ),
			   col5,
			   FALSE  );

    /* 4�me ligne */

    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_BREAKDOWN] ),
			   col0+col1,
			   FALSE );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_NOTES] ),
			   col2,
			   FALSE );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_BANK] ),
			   col3+col4+col5,
			   FALSE  );
    gtk_widget_set_usize ( GTK_WIDGET ( widget_formulaire_operations[TRANSACTION_FORM_MODE] ),
			   col6,
			   FALSE  );

    while ( g_main_iteration ( FALSE ));

    return ( FALSE );
}
/******************************************************************************/


/******************************************************************************/
/* Fonction  demande_mise_a_jour_tous_comptes */
/* met la variable MISE_A_JOUR de tous les comptes � 1 */
/* ce qui fait que lorsqu'ils seront affich�s, ils seront mis � jour avant */
/* appelle aussi verification_mise_a_jour_liste pour mettre � jour la liste courante */
/******************************************************************************/
void demande_mise_a_jour_tous_comptes ( void )
{
    gint i;
    gpointer **save_p_tab;

/* FIXME : v�rifier les appels � cette fonction et la faire passer en idle */


    save_p_tab = p_tab_nom_de_compte_variable;
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

    for ( i = 0 ; i < nb_comptes ; i++ )
    {
	MISE_A_JOUR = 1;
	p_tab_nom_de_compte_variable++;
    }


    p_tab_nom_de_compte_variable = save_p_tab;
}
/******************************************************************************/


/**
 * Pop up a menu with several actions to apply to current transaction.
 */
void popup_transaction_context_menu ( gboolean full )
{
    GtkWidget *menu, *menu_item;

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

    if ( OPERATION_SELECTIONNEE == GINT_TO_POINTER(-1) )
	full = FALSE;

    menu = gtk_menu_new ();

    /* Edit transaction */
    menu_item = gtk_image_menu_item_new_with_label ( _("Edit transaction") );
    gtk_image_menu_item_set_image ( GTK_IMAGE_MENU_ITEM(menu_item),
				    gtk_image_new_from_stock ( GTK_STOCK_PROPERTIES,
							       GTK_ICON_SIZE_MENU ));
    g_signal_connect ( G_OBJECT(menu_item), "activate", edition_operation, NULL );
    if ( !full )
	gtk_widget_set_sensitive ( menu_item, FALSE );
    gtk_menu_append ( menu, menu_item );

    /* Separator */
    gtk_menu_append ( menu, gtk_separator_menu_item_new() );

    /* New transaction */
    menu_item = gtk_image_menu_item_new_with_label ( _("New transaction") );
    gtk_image_menu_item_set_image ( GTK_IMAGE_MENU_ITEM(menu_item),
				    gtk_image_new_from_stock ( GTK_STOCK_NEW,
							       GTK_ICON_SIZE_MENU ));
    g_signal_connect ( G_OBJECT(menu_item), "activate", new_transaction, NULL );
    gtk_menu_append ( menu, menu_item );

    /* Delete transaction */
    menu_item = gtk_image_menu_item_new_with_label ( _("Delete transaction") );
    gtk_image_menu_item_set_image ( GTK_IMAGE_MENU_ITEM(menu_item),
				    gtk_image_new_from_stock ( GTK_STOCK_DELETE,
							       GTK_ICON_SIZE_MENU ));
    g_signal_connect ( G_OBJECT(menu_item), "activate", remove_transaction, NULL );
    if ( !full )
	gtk_widget_set_sensitive ( menu_item, FALSE );
    gtk_menu_append ( menu, menu_item );

    /* Clone transaction */
    menu_item = gtk_image_menu_item_new_with_label ( _("Clone transaction") );
    gtk_image_menu_item_set_image ( GTK_IMAGE_MENU_ITEM(menu_item),
				    gtk_image_new_from_stock ( GTK_STOCK_COPY,
							       GTK_ICON_SIZE_MENU ));
    g_signal_connect ( G_OBJECT(menu_item), "activate", clone_selected_transaction, NULL );
    if ( !full )
	gtk_widget_set_sensitive ( menu_item, FALSE );
    gtk_menu_append ( menu, menu_item );

    /* Separator */
    gtk_menu_append ( menu, gtk_separator_menu_item_new() );

    /* Convert to scheduled transaction */
    menu_item = gtk_image_menu_item_new_with_label ( _("Convert transaction to scheduled transaction") );
    gtk_image_menu_item_set_image ( GTK_IMAGE_MENU_ITEM(menu_item),
				    gtk_image_new_from_stock ( GTK_STOCK_CONVERT,
							       GTK_ICON_SIZE_MENU ));
    g_signal_connect ( G_OBJECT(menu_item), "activate", schedule_selected_transaction, NULL );
    if ( !full )
	gtk_widget_set_sensitive ( menu_item, FALSE );
    gtk_menu_append ( menu, menu_item );

    /* Move to another account */
    menu_item = gtk_image_menu_item_new_with_label ( _("Move transaction to another account") );
    gtk_image_menu_item_set_image ( GTK_IMAGE_MENU_ITEM(menu_item),
				    gtk_image_new_from_stock ( GTK_STOCK_JUMP_TO,
							       GTK_ICON_SIZE_MENU ));
    if ( !full )
	gtk_widget_set_sensitive ( menu_item, FALSE );
    gtk_menu_append ( menu, menu_item );

    /* Add accounts submenu */
    gtk_menu_item_set_submenu ( GTK_MENU_ITEM(menu_item), 
				GTK_WIDGET(creation_option_menu_comptes(GTK_SIGNAL_FUNC(move_selected_operation_to_account), FALSE, FALSE)) );

    gtk_widget_show_all (menu);
    gtk_menu_popup ( GTK_MENU(menu), NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time());
}


/**
 *  Check that a transaction is selected and sets the
 *  p_tab_nom_de_compte_variable pointer accordingly.
 *
 * \return TRUE on success, FALSE otherwise.
 */
gboolean assert_selected_transaction ()
{
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

    if ( OPERATION_SELECTIONNEE == GINT_TO_POINTER(-1) )
	return FALSE;

    return TRUE;
}


/**
 *  Empty transaction form and select transactions tab.
 */
void new_transaction () 
{
    /* We do not test it since we don't care about no selected
       transaction */
    assert_selected_transaction();
    echap_formulaire();
    OPERATION_SELECTIONNEE = GINT_TO_POINTER (-1);
    edition_operation ();

    gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_general ), 1 );
}


/**
 * Remove selected transaction if any.
 */
void remove_transaction ()
{
    if (! assert_selected_transaction()) return;

    supprime_operation ( OPERATION_SELECTIONNEE );
    gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_general ), 1 );
}


/**
 * Clone selected transaction if any.  Update user interface as well.
 */
void clone_selected_transaction ()
{
    if (! assert_selected_transaction()) return;

    clone_transaction ( OPERATION_SELECTIONNEE );

    gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_general ), 1 );

    if ( mise_a_jour_combofix_tiers_necessaire )
	mise_a_jour_combofix_tiers ();
    if ( mise_a_jour_combofix_categ_necessaire )
	mise_a_jour_combofix_categ ();
    if ( mise_a_jour_combofix_imputation_necessaire )
	mise_a_jour_combofix_imputation ();

    modification_fichier ( TRUE );
}


/**
 * Clone transaction.  If it is a breakdown or a transfer, perform all
 * needed operations, like cloning associated transactions as well.
 *
 * \param operation Initial transaction to clone
 *
 * \return A newly created operation.
 */
struct structure_operation *  clone_transaction ( struct structure_operation * operation )
{
    struct structure_operation * new_transaction, * ope_ventilee;

    new_transaction = (struct structure_operation *) malloc ( sizeof(struct structure_operation) );
    if ( !new_transaction )
    {
	dialogue ( _("Cannot allocate memory, bad things will happen soon") );
	return(FALSE);
    }

#ifndef _WIN32
    bcopy ( operation, new_transaction, sizeof(struct structure_operation) );
#else
    memcpy(new_transaction, operation, sizeof(struct structure_operation) );
#endif
    new_transaction -> no_operation = 0;
    ajout_operation ( new_transaction );

    if ( operation -> relation_no_operation )
    {
	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;
	validation_virement_operation ( operation, 0, NOM_DU_COMPTE );
    }

    if ( operation -> operation_ventilee )
    {
	GSList *liste_tmp;

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

	liste_tmp = LISTE_OPERATIONS;

	while ( liste_tmp )
	{
	    struct structure_operation *operation_2;

	    operation_2 = liste_tmp -> data;

	    if ( operation_2 -> no_operation_ventilee_associee == operation -> no_operation )
	    {
		ope_ventilee = clone_transaction ( operation_2 );
		ope_ventilee -> no_operation_ventilee_associee = new_transaction -> no_operation;
	    }

	    liste_tmp = liste_tmp -> next;
	}
    }

    return new_transaction;
}



/**
 * Move selected transaction to another account.  Normally called as a
 * handler.
 *
 * \param menu_item The GtkMenuItem that triggered this handler.
 */
void move_selected_operation_to_account ( GtkMenuItem * menu_item )
{
    gint target_account, source_account;
    gpointer ** tmp = p_tab_nom_de_compte_variable;

    if (! assert_selected_transaction()) return;

    source_account = NO_COMPTE;
    target_account = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT(menu_item), 
							     "no_compte" ) );  

    if ( move_operation_to_account ( OPERATION_SELECTIONNEE, target_account ))
    {
	gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_general ), 1 );

	if ( mise_a_jour_combofix_tiers_necessaire )
	    mise_a_jour_combofix_tiers ();
	if ( mise_a_jour_combofix_categ_necessaire )
	    mise_a_jour_combofix_categ ();
	if ( mise_a_jour_combofix_imputation_necessaire )
	    mise_a_jour_combofix_imputation ();

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + source_account;

	SOLDE_COURANT = calcule_solde_compte ( source_account );
	SOLDE_POINTE = calcule_solde_pointe_compte ( source_account );

	mise_a_jour_labels_soldes ();

	p_tab_nom_de_compte_variable = tmp;
	modification_fichier ( TRUE );
    }
}



/**
 * Move transaction to another account
 *
 * \param transaction Transaction to move to other account
 * \param account Account to move the transaction to
 * return TRUE if ok
 */
gboolean move_operation_to_account ( struct structure_operation * transaction,
				     gint account )
{
    GtkTreeIter *iter;
    gpointer ** tmp = p_tab_nom_de_compte_variable;

    if ( transaction -> relation_no_operation )
    {
	struct structure_operation * contra_transaction;

	/* 	l'op�ration est un virement, si on veut la d�placer vers le compte */
	/* 	    vir�, on refuse */

	if ( transaction -> relation_no_compte == account )
	{
	    dialogue_error ( _("Cannot move a transfer on his contra-account"));
	    return FALSE;
	}

	contra_transaction = operation_par_no (  transaction -> relation_no_operation,
						 transaction -> relation_no_compte );
	if ( contra_transaction )
	{
	    contra_transaction -> relation_no_compte = account;

	    /* 	    p_tab est plac� par la fonction suivante, et on remet AFFICHAGE_SOLDE_FINI � 1 */
	    /* 		car est remis � 0 mais on ne change pas le solde */
	    
	    remplit_ligne_operation ( contra_transaction,
				      NULL );
	    AFFICHAGE_SOLDE_FINI = 1;
	}
    }

    if ( transaction -> operation_ventilee )
    {
	GSList *liste_tmp;

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + transaction -> no_compte;
	liste_tmp = g_slist_copy ( LISTE_OPERATIONS );

	while ( liste_tmp )
	{
	    struct structure_operation *transaction_2;

	    transaction_2 = liste_tmp -> data;

	    if ( transaction_2 -> no_operation_ventilee_associee == 
		 transaction -> no_operation )
	    {
		move_operation_to_account ( transaction_2, account );
		transaction_2 -> relation_no_compte = account;
	    }

	    liste_tmp = liste_tmp -> next;
	}

	g_slist_free ( liste_tmp );
    }

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + transaction -> no_compte;

    LISTE_OPERATIONS = g_slist_remove ( LISTE_OPERATIONS, transaction );

    /*     si l'op�ration �tait affich�e, on la retire du list_store */

    iter = cherche_iter_operation ( transaction );

    if ( iter )
    {
	gint i;

	/* si la s�lection est sur l'op�ration qu'on supprime, */
	/* on met la s�lection sur celle du dessous */

	if ( OPERATION_SELECTIONNEE == transaction )
	    OPERATION_SELECTIONNEE = cherche_operation_from_ligne ( cherche_ligne_operation ( OPERATION_SELECTIONNEE ) + NB_LIGNES_OPE,
								    transaction -> no_compte );

	for ( i=0 ; i<NB_LIGNES_OPE ; i++ )
	    gtk_list_store_remove ( GTK_LIST_STORE ( STORE_LISTE_OPERATIONS ),
				    iter );

	/*     on met � jour les couleurs et les soldes */

	update_couleurs_background ( transaction -> no_compte,
				     gtk_tree_iter_copy (iter));
	update_soldes_list_store ( transaction -> no_compte,
				   iter );
	selectionne_ligne ( OPERATION_SELECTIONNEE );
    }


    
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + account;
    transaction -> no_compte = account;

    /*     comme l'op� contient d�j� un no d'op�ration, on doit d'abord l'ajouter manuellement */
    /* 	� la liste avant d'appeler ajout_operation */

    LISTE_OPERATIONS = g_slist_insert_sorted ( LISTE_OPERATIONS,
					       transaction,
					       (GCompareFunc) CLASSEMENT_COURANT );
    ajout_operation ( transaction );
    p_tab_nom_de_compte_variable = tmp;
    return TRUE;
}



/**
 * Convert selected transaction to a template of scheduled transaction
 * via schedule_transaction().
 */
void schedule_selected_transaction ()
{
    struct operation_echeance * echeance;

    if (! assert_selected_transaction()) return;

    echeance = schedule_transaction ( OPERATION_SELECTIONNEE);

    mise_a_jour_liste_echeances_auto_accueil = 1;
    remplissage_liste_echeance ();

    echeance_selectionnnee = echeance;
    formulaire_echeancier_a_zero();
    degrise_formulaire_echeancier();
    selectionne_echeance ();
    edition_echeance ();

    gtk_notebook_set_current_page ( GTK_NOTEBOOK(notebook_general), 2 );

    modification_fichier ( TRUE );
}



/**
 *  Convert transaction to a template of scheduled transaction.
 *
 * \param transaction Transaction to use as a template.
 */
    struct operation_echeance *
schedule_transaction ( struct structure_operation * transaction )
{
    struct operation_echeance *echeance;

    echeance = (struct operation_echeance *) calloc ( 1,
						      sizeof(struct operation_echeance) );
    if ( !echeance )
    {
	dialogue ( _("Cannot allocate memory, bad things will happen soon") );
	return(FALSE);
    }

    echeance -> compte = transaction -> no_compte;
    echeance -> jour = transaction -> jour;
    echeance -> mois = transaction -> mois;
    echeance -> annee = transaction -> annee;
    echeance -> date = g_date_new_dmy ( transaction -> jour,
					transaction -> mois,
					transaction -> annee );

    echeance -> montant = transaction -> montant;
    echeance -> devise = transaction -> devise;

    echeance -> tiers = transaction -> tiers;
    echeance -> categorie = transaction -> categorie;
    echeance -> sous_categorie = transaction -> sous_categorie;

/*     pour 1 virement, categ et sous categ sont � 0, et compte_virement contient le no de compte */
/* 	mais si categ et sous categ sont � 0 et que ce n'est pas un virement ni une ventil, compte_virement = -1 */
/*     on va changer �a la prochaine version, d�s que c'est pas un virement -> -1 */

    if ( transaction -> relation_no_operation )
    {
	/* 	c'est un virement, on met la relation et on recherche le type de la contre op�ration */
	
	gpointer **save_ptab;
	struct structure_operation *contre_operation;
	
	echeance -> compte_virement = transaction -> relation_no_compte;

	save_ptab = p_tab_nom_de_compte_variable;

	contre_operation = operation_par_no ( transaction -> relation_no_operation,
					      echeance -> compte_virement );
	if ( contre_operation )
	    echeance -> type_contre_ope = contre_operation -> type_ope;
	p_tab_nom_de_compte_variable = save_ptab;
    }
    else
	if ( !echeance -> categorie
	     &&
	     !transaction -> operation_ventilee )
	    echeance -> compte_virement = -1;

    echeance -> notes = g_strdup ( transaction -> notes );
    echeance -> type_ope = transaction -> type_ope;
    echeance -> contenu_type = g_strdup ( transaction -> contenu_type );


    echeance -> no_exercice = transaction -> no_exercice;
    echeance -> imputation = transaction -> imputation;
    echeance -> sous_imputation = transaction -> sous_imputation;

    echeance -> operation_ventilee = transaction -> operation_ventilee;

/*     par d�faut, on met en manuel, pour �viter si l'utilisateur se gourre dans la date, */
/*     (c'est le cas, � 0 avec calloc) */
/*     que l'op� soit enregistr�e imm�diatement ; de m�me on le met en mensuel par d�faut */
/* 	pour la m�me raison */

    echeance -> periodicite = 2;
    
    echeance -> no_operation = ++no_derniere_echeance;
    nb_echeances++;
    liste_struct_echeances = g_slist_insert_sorted ( liste_struct_echeances,
						     echeance,
						     (GCompareFunc) comparaison_date_echeance );

/*     on r�cup�re les op�s de ventil si c'�tait une op� ventil�e */

    if ( echeance -> operation_ventilee )
    {
	GSList *liste_tmp;

	liste_tmp = LISTE_OPERATIONS;

	while ( liste_tmp )
	{
	    struct structure_operation *transaction_de_ventil;

	    transaction_de_ventil = liste_tmp -> data;

	    if ( transaction_de_ventil -> no_operation_ventilee_associee == transaction -> no_operation )
	    {
		struct operation_echeance *echeance_de_ventil;

		echeance_de_ventil = calloc ( 1,
					      sizeof ( struct operation_echeance));

		if ( !echeance_de_ventil )
		{
		    dialogue ( _("Cannot allocate memory, bad things will happen soon") );
		    return(FALSE);
		}

		echeance_de_ventil -> compte = transaction_de_ventil -> no_compte;
		echeance_de_ventil -> jour = transaction_de_ventil -> jour;
		echeance_de_ventil -> mois = transaction_de_ventil -> mois;
		echeance_de_ventil -> annee = transaction_de_ventil -> annee;
		echeance_de_ventil -> date = g_date_new_dmy ( transaction_de_ventil -> jour,
						    transaction_de_ventil -> mois,
						    transaction_de_ventil -> annee );

		echeance_de_ventil -> montant = transaction_de_ventil -> montant;
		echeance_de_ventil -> devise = transaction_de_ventil -> devise;

		echeance_de_ventil -> tiers = transaction_de_ventil -> tiers;
		echeance_de_ventil -> categorie = transaction_de_ventil -> categorie;
		echeance_de_ventil -> sous_categorie = transaction_de_ventil -> sous_categorie;

		/*     pour 1 virement, categ et sous categ sont � 0, et compte_virement contient le no de compte */
		/* 	mais si categ et sous categ sont � 0 et que ce n'est pas un virement, compte_virement = -1 */
		/*     on va changer �a la prochaine version, d�s que c'est pas un virement -> -1 */

		if ( transaction_de_ventil -> relation_no_operation )
		{
		    /* 	c'est un virement, on met la relation et on recherche le type de la contre op�ration */

		    gpointer **save_ptab;
		    struct structure_operation *contre_operation;

		    echeance_de_ventil -> compte_virement = transaction_de_ventil -> relation_no_compte;

		    save_ptab = p_tab_nom_de_compte_variable;

		    contre_operation = operation_par_no ( transaction_de_ventil -> relation_no_operation,
							  echeance_de_ventil -> compte_virement );

		    if ( contre_operation )
			echeance_de_ventil -> type_contre_ope = contre_operation -> type_ope;
		    p_tab_nom_de_compte_variable = save_ptab;
		}
		else
		    if ( !echeance_de_ventil -> categorie )
			echeance_de_ventil -> compte_virement = -1;

		echeance_de_ventil -> notes = g_strdup ( transaction_de_ventil -> notes );
		echeance_de_ventil -> type_ope = transaction_de_ventil -> type_ope;
		echeance_de_ventil -> contenu_type = g_strdup ( transaction_de_ventil -> contenu_type );


		echeance_de_ventil -> no_exercice = transaction_de_ventil -> no_exercice;
		echeance_de_ventil -> imputation = transaction_de_ventil -> imputation;
		echeance_de_ventil -> sous_imputation = transaction_de_ventil -> sous_imputation;

		echeance_de_ventil-> no_operation_ventilee_associee = echeance -> no_operation;

		/*     par d�faut, on met en manuel, pour �viter si l'utilisateur se gourre dans la date, */
		/*     (c'est le cas, � 0 avec calloc) */
		/*     que l'op� soit enregistr�e imm�diatement ; de m�me on le met en mensuel par d�faut */
		/* 	pour la m�me raison */

		echeance_de_ventil -> periodicite = 2;

		echeance_de_ventil -> no_operation = ++no_derniere_echeance;
		nb_echeances++;
		liste_struct_echeances = g_slist_insert_sorted ( liste_struct_echeances,
								 echeance_de_ventil,
								 (GCompareFunc) comparaison_date_echeance );
	    }
	    liste_tmp = liste_tmp -> next;
	}
    }
    return echeance;
}


/******************************************************************************/
/* cette fonction affiche les traits verticaux et horizontaux sur la liste des op�s */
/******************************************************************************/
gboolean affichage_traits_liste_operation ( void )
{

    GdkWindow *fenetre;
    gint i;
    gint largeur, hauteur;
    gint x, y;
    GtkAdjustment *adjustment;
    gint derniere_ligne;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

    /*  FIXME   sachant qu'on appelle �a � chaque expose-event, cad tr�s souvent ( d�s que la souris passe dessus ), */
    /*     �a peut ralentir bcp... � v�rifier  */

    fenetre = gtk_tree_view_get_bin_window ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ));

    gdk_drawable_get_size ( GDK_DRAWABLE ( fenetre ),
			    &largeur,
			    &hauteur );

    if ( !gc_separateur_operation )
	gc_separateur_operation = gdk_gc_new ( GDK_DRAWABLE ( fenetre ));

    /*     si la hauteur des lignes n'est pas encore calcul�e, on le fait ici */

/*     if ( !hauteur_ligne_liste_opes ) */
	hauteur_ligne_liste_opes = recupere_hauteur_ligne_tree_view ( TREE_VIEW_LISTE_OPERATIONS );

    /*     on commence par calculer la derni�re ligne en pixel correspondant � la derni�re op� de la liste */
    /* 	pour �viter de dessiner les traits en dessous */

    derniere_ligne = hauteur_ligne_liste_opes * GTK_LIST_STORE ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ))) -> length;
    hauteur = MIN ( derniere_ligne,
		    hauteur );

    /*     le plus facile en premier... les lignes verticales */
    /*     d�pend de si on est en train de ventiler ou non */
    /*     on en profite pour ajuster nb_ligne_ope_tree_view */

    x=0;

    for ( i=0 ; i<6 ; i++ )
    {
	x = x + gtk_tree_view_column_get_width ( GTK_TREE_VIEW_COLUMN ( COLONNE_LISTE_OPERATIONS(i) ));
	gdk_draw_line ( GDK_DRAWABLE ( fenetre ),
			gc_separateur_operation,
			x, 0,
			x, hauteur );
    }

    /*     les lignes horizontales : il faut calculer la position y de chaque changement d'op� � l'�cran */
    /*     on calcule la position y de la 1�re ligne � afficher */

    adjustment = gtk_tree_view_get_vadjustment ( GTK_TREE_VIEW ( TREE_VIEW_LISTE_OPERATIONS ));

    y = ( hauteur_ligne_liste_opes * NB_LIGNES_OPE ) * ( ceil ( adjustment->value / (hauteur_ligne_liste_opes* NB_LIGNES_OPE) )) - adjustment -> value;

    do
    {
	gdk_draw_line ( GDK_DRAWABLE ( fenetre ),
			gc_separateur_operation,
			0, y, 
			largeur, y );
	y = y + hauteur_ligne_liste_opes*NB_LIGNES_OPE;
    }
    while ( y < ( adjustment -> page_size )
	    &&
	    y <= derniere_ligne );

    return FALSE;
}
/******************************************************************************/






/******************************************************************************/
gboolean click_sur_titre_colonne_operations ( GtkTreeViewColumn *colonne,
					      gint *no_colonne )
{
    GtkWidget *menu, *menu_item;
    gchar **tab_char;
    gint i;


/*     on r�cup�re les popups du titre de la colonne */
/* 	s'il n'y a rien dans la colonne, on se barre */
/* 	cel� veut dire aussi que pour trier en fonction d'un param�tre, */
/*     ce param�tre doit �tre affich� dans la liste */

    tab_char = g_strsplit ( tips_col_liste_operations[GPOINTER_TO_INT(no_colonne)],
			    ", ",
			    0 );
    if ( !tab_char[0] )
	return FALSE;

    /*     on met la de colonne en tmp */

    colonne_classement_tmp = colonne;

	/*     s'il n'y a qu'un choix possible, on n'affiche pas la popup, on trie */
    /* 	directement */

    if ( !tab_char[1] )
    {
	if ( strcmp ( tab_char[0],
		      N_("Balance")))
	{
	    changement_choix_tri_liste_operations ( tab_char[0] );
	    g_strfreev ( tab_char );
	    return FALSE;
	}
	else
	    return FALSE;
    }

     menu = gtk_menu_new ();

    /*  ligne trier par */

    menu_item = gtk_menu_item_new_with_label ( _("Sort list by :") );

/*     les 2 signaux sont bloqu�s pour �viter que la ligne s'affiche comme un bouton */
/* pas r�ussi � faire autrement... */

    g_signal_connect ( G_OBJECT ( menu_item),
		       "enter-notify-event",
		       G_CALLBACK ( gtk_true ),
		       NULL );
    g_signal_connect ( G_OBJECT ( menu_item),
		       "motion-notify-event",
		       G_CALLBACK ( gtk_true ),
		       NULL );
  
    gtk_menu_append ( menu,
		      menu_item );
    gtk_widget_show_all ( menu_item );

    menu_item = gtk_separator_menu_item_new ();
    gtk_menu_append ( menu,
		      menu_item );
    gtk_widget_show ( menu_item );


    i = 0;

    while ( tab_char[i] )
    {
	if ( strcmp ( tab_char[i],
		      N_("Balance")))
	{
	    menu_item = gtk_menu_item_new_with_label ( tab_char[i] );
	    g_signal_connect_swapped ( G_OBJECT(menu_item),
				       "activate",
				       G_CALLBACK ( changement_choix_tri_liste_operations ),
				       tab_char[i] );
/* 	    g_signal_connect_data ( G_OBJECT(menu_item), */
/* 				    "activate", */
/* 				    G_CALLBACK ( g_strfreev ), */
/* 				    tab_char, */
/* 				    NULL, */
/* 				    G_CONNECT_AFTER | G_CONNECT_SWAPPED); */
	    gtk_menu_append ( menu,
			      menu_item );
	    gtk_widget_show ( menu_item );
	}
	i++;
    }


    gtk_menu_popup ( GTK_MENU(menu),
		     NULL,
		     NULL,
		     NULL,
		     NULL,
		     3,
		     gtk_get_current_event_time());
    gtk_widget_show (menu);
    
    return FALSE;
}					      
/******************************************************************************/


/*****************************************************************************/
gboolean changement_choix_tri_liste_operations ( gchar *nom_tri )
{
/*  cette fonction est appel�e quand on a cliqu� sur un titre de colonne pour changer */
/*      le tri de la liste d'opé */

    gint no_tri;
    GSList *liste_tmp_avant_tri;
    GSList *liste_tmp_apres_tri;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

    no_tri = g_slist_position ( liste_labels_titres_colonnes_liste_ope,
				g_slist_find_custom ( liste_labels_titres_colonnes_liste_ope,
						      nom_tri,
						      (GCompareFunc) cherche_string_equivalente_dans_slist ));

    CLASSEMENT_COURANT = recupere_classement_par_no ( no_tri );
    
    gtk_tree_view_column_set_sort_indicator ( COLONNE_CLASSEMENT,
					      FALSE );

    if ( COLONNE_CLASSEMENT == colonne_classement_tmp )
    {
	/* la liste �tait d�j� tri� par cette colonne, on change le sens de tri */

	CLASSEMENT_CROISSANT = !CLASSEMENT_CROISSANT;
    }
    else
    {
	COLONNE_CLASSEMENT = colonne_classement_tmp;
	CLASSEMENT_CROISSANT = 1;
    }

    gtk_tree_view_column_set_sort_indicator ( COLONNE_CLASSEMENT,
					      TRUE );

    if ( CLASSEMENT_CROISSANT )
	gtk_tree_view_column_set_sort_order ( COLONNE_CLASSEMENT,
					      GTK_SORT_ASCENDING );
    else
	gtk_tree_view_column_set_sort_order ( COLONNE_CLASSEMENT,
					      GTK_SORT_DESCENDING );

    /*     on va cr�er des slists des op�s affich�es avant et apr�s le tri pour pouvoir */
    /* 	mettre � jour la list_store des op�s */

    liste_tmp_avant_tri = cree_slist_affichee ( compte_courant );

    classe_liste_operations ( compte_courant );

    liste_tmp_apres_tri = cree_slist_affichee ( compte_courant );

    my_list_store_sort ( compte_courant,
			 liste_tmp_avant_tri,
			 liste_tmp_apres_tri );

    modification_fichier ( TRUE );
    return FALSE;
}
/******************************************************************************/

/******************************************************************************/
gpointer recupere_classement_par_no ( gint no_tri )
{

    switch ( no_tri )
    {
	case 0:
	    etat.classement_par_date = 1;
	    return ( classement_sliste_par_date);
	    break;
	case 1:
	    etat.classement_par_date = 0;
	    return ( classement_sliste_par_date);
	    break;
	case 2:
	    return ( classement_sliste_par_tiers);
	    break;
	case 3:
	    return ( classement_sliste_par_imputation);
	    break;
	case 4:
	    return ( classement_sliste_par_credit);
	    break;
	case 5:
	    return ( classement_sliste_par_debit);
	    break;
	case 6:
	    /* 	    balance, normalement ne devrait pas venir ici, dans le doute renvoie par date */
	    return ( classement_sliste_par_date );
	    break;
	case 7:
	    return ( classement_sliste_par_montant);
	    break;
	case 8:
	    return ( classement_sliste_par_type_ope);
	    break;
	case 9:
	    return ( classement_sliste_par_no_rapprochement);
	    break;
	case 10:
	    return ( classement_sliste_par_exercice);
	    break;
	case 11:
	    return ( classement_sliste_par_categories);
	    break;
	case 12:
	    return ( classement_sliste_par_pointage);
	    break;
	case 13:
	    return ( classement_sliste_par_pc);
	    break;
	case 14:
	    return ( classement_sliste_par_notes);
	    break;
	case 15:
	    return ( classement_sliste_par_ibg);
	    break;
	case 16:
	    return ( classement_sliste_par_no);
	    break;
	case 17:
	    return ( classement_sliste_par_no_rapprochement);
	    break;
	default :
	    return ( classement_sliste_par_date);
    }
}
/******************************************************************************/



/******************************************************************************/
/* cette fonction met juste les labels sous la liste des op�s � jour */
/* les soldes ont d� �tre calcul� avant */
/******************************************************************************/
void mise_a_jour_labels_soldes ( void )
{
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

    /*     met le solde */

    gtk_label_set_text ( GTK_LABEL ( solde_label ),
			 g_strdup_printf ( PRESPACIFY(_("Current balance: %4.2f %s")),
					   SOLDE_COURANT,
					   devise_code_by_no ( DEVISE )));


    /* met le label du solde point� */

    gtk_label_set_text ( GTK_LABEL ( solde_label_pointe ),
			 g_strdup_printf ( _("Checked balance: %4.2f %s"),
					   SOLDE_POINTE,
					   devise_code_by_no ( DEVISE )));
}
/******************************************************************************/


/******************************************************************************************************************/
/* la partie suivante est un copier/coller de gtk 2.2 car il y a un bug */
/* qui est r�par� dans la version 2.4 */
/* FIXME : cette partie sera � virer plus tard, quand tout le monde aura gtk 2.4 ... */
/******************************************************************************************************************/

/* Sorting and reordering */
typedef struct _SortTuple
{
  gint offset;
  GSList *el;
} SortTuple;

/* Reordering */
static gint
gtk_list_store_reorder_func (gconstpointer a,
			     gconstpointer b,
			     gpointer      user_data)
{
  SortTuple *a_reorder;
  SortTuple *b_reorder;

  a_reorder = (SortTuple *)a;
  b_reorder = (SortTuple *)b;

  if (a_reorder->offset < b_reorder->offset)
    return -1;
  if (a_reorder->offset > b_reorder->offset)
    return 1;

  return 0;
}

/**
 * gtk_list_store_reorder:
 * @store: A #GtkTreeStore.
 * @new_order: An integer array indicating the new order for the list.
 *
 * Reorders @store to follow the order indicated by @new_order. Note that
 * this function only works with unsorted stores.
 *
 * Since: 2.2
 **/
void
my_list_store_reorder (GtkListStore *store,
			gint         *new_order)
{
  gint i;
  GSList *current_list;
  GtkTreePath *path;
  SortTuple *sort_array;

  g_return_if_fail (GTK_IS_LIST_STORE (store));
  g_return_if_fail (new_order != NULL);

  sort_array = g_new (SortTuple, store->length);

  current_list = store->root;

  for (i = 0; i < store->length; i++)
    {
/* 	la correction du bug est ici... */
      sort_array[new_order[i]].offset = i;
      sort_array[i].el = current_list;

      current_list = current_list->next;
    }

  g_qsort_with_data (sort_array,
		     store->length,
		     sizeof (SortTuple),
		     gtk_list_store_reorder_func,
		     NULL);


  for (i = 0; i < store->length - 1; i++)
    (GSList *) (sort_array[i].el)->next = (GSList *) (sort_array[i+1].el);

  store->root = (GSList *) (sort_array[0].el);
  store->tail = (GSList *) (sort_array[store->length-1].el);
  ((GSList *) (store->tail))->next = NULL;

  /* emit signal */
  path = gtk_tree_path_new ();
  gtk_tree_model_rows_reordered (GTK_TREE_MODEL (store),
				 path, NULL, new_order);
  gtk_tree_path_free (path);
  g_free (sort_array);
}
/******************************************************************************************************************/
/* FIN DU COPIER COLLER GTK 2.2 */
/******************************************************************************************************************/





/******************************************************************************/
/* cette fonction classe la list_store du compte donn� en argument */
/* par le classement courant de ce compte */
/******************************************************************************/
void my_list_store_sort ( gint no_compte,
			  GSList *liste_avant_tri,
			  GSList *liste_apres_tri )
{
    gint longueur;
    gint *pos;
    gint i;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;

    longueur = STORE_LISTE_OPERATIONS -> length;

    pos = malloc ( longueur * sizeof ( gint ));

    /*     on r�alise le lien pos[nouvelle_ligne] = ancienne_ligne */

    for ( i=0 ; i<(longueur-NB_LIGNES_OPE) ; i++ )
	pos[i]= g_slist_index ( liste_avant_tri,
				g_slist_nth_data ( liste_apres_tri,
						   i/NB_LIGNES_OPE ))
	    * NB_LIGNES_OPE + (i%NB_LIGNES_OPE);


    /*     ajout de la ligne blanche � la fin */

    for ( i= longueur-NB_LIGNES_OPE ; i<longueur ; i++ )
	pos[i] = i;

    /* FIXME : si gtk2.4 appeler gtk_list_store_reorder */
    /* 	    sinon my_list_store_reorder  */

    my_list_store_reorder ( STORE_LISTE_OPERATIONS,
			    pos );

    g_slist_free ( liste_avant_tri );
    g_slist_free ( liste_apres_tri );
    free ( pos );

    /*     on remet les soldes et la couleur du fond */

    update_couleurs_background( no_compte,
				NULL );
    update_soldes_list_store ( no_compte,
			       NULL );
    selectionne_ligne ( OPERATION_SELECTIONNEE );
}
/******************************************************************************/




/******************************************************************************/
/* cette fonction renvoie une slist contenant uniquement les adr des op�s affich�es, */
/*     c'est donc la LISTE_OPERATIONS sans les op�s de ventils et plus ou moins les R */
/*     selon la conf */
/*     l'ordre de cette liste est l'ordre courant de la liste des op�s */
/******************************************************************************/

GSList *cree_slist_affichee ( gint no_compte )
{
    GSList *liste_tmp;
    GSList *nouvelle_liste;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;

    nouvelle_liste = NULL;

    liste_tmp = LISTE_OPERATIONS;

    while ( liste_tmp )
    {
	struct structure_operation *operation;

	operation = liste_tmp -> data;

	if ( !(operation -> no_operation_ventilee_associee
	       ||
	       ( operation -> pointe == 3
		 &&
		 !AFFICHAGE_R )))
	    nouvelle_liste = g_slist_append ( nouvelle_liste,
					      operation );
		     
	liste_tmp = liste_tmp -> next;
    }
    return nouvelle_liste;
}
/******************************************************************************/





/******************************************************************************/
/* cette fonction est appel�e lorsqu'on veut changer l'�tat de l'affichage avec/sans R */
/* argument : le nouvel affichage */
/* en fonction de l'argument, elle retire les lignes R de la liste ou elle les ajoute */
/******************************************************************************/
void mise_a_jour_affichage_r ( gint affichage_r )
{
    gint i;
    GtkTreeIter *iter;
    struct structure_operation *operation;


    if ( DEBUG )
	printf ( "mise_a_jour_affichage_r afficher : %d\n", affichage_r );
    
    /*     si affichage_r = AFFICHAGE_R on ne change rien */
    /* 	on v�rifie sur le compte courant car soit c'est sur ce compte */
    /* 	qu'on travaille, soit tous les autres comptes ont la m�me valeur */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

    if ( affichage_r == AFFICHAGE_R )
	return;


    AFFICHAGE_R = affichage_r;
    iter = &iter_liste_operations;

    if ( AFFICHAGE_R )
    {
	/* 	on a demand� d'afficher les R */
	for ( i=0 ; i<nb_comptes ; i++ )
	{
	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

	    /* 	    on ne touche � la liste que s'il le faut en fonction de retient_affichage_par_compte */
	    /* 		et du compte en cours */

	    if ( !( etat.retient_affichage_par_compte
		    &&
		    i != compte_courant
		    &&
		    g_slist_length ( LISTE_OPERATIONS )))
	    {
		/* 	on va rep�rer la ligne dans la liste des op�s de la 1�re op� affich�e */
		/* 		ensuite,  */
		/* 		soit l'op� que l'on veut afficher se situe avant, donc insertion devant la 1�re ligne */
		/* 		    soit elle se situe apr�s, donc insertion devant la 1�re ligne non R suivante */

		gint ligne_ope_pour_insertion;
		GSList *liste_tmp;

		/* 		si le store du compte n'est pas fini, il faut le finir avant de faire joujou */

		if ( SLIST_DERNIERE_OPE_AJOUTEE != GINT_TO_POINTER (-1))
		    ajoute_operations_compte_dans_list_store ( i,
							       0 );

		ligne_ope_pour_insertion = g_slist_index ( LISTE_OPERATIONS,
							   cherche_operation_from_ligne ( 0,
											  i ));
		gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
						iter );

		liste_tmp = LISTE_OPERATIONS;

		while ( liste_tmp )
		{
		    operation = liste_tmp -> data;

		    if ( operation -> pointe == 3
			 &&
			 !operation -> no_operation_ventilee_associee )
		    {
			/* 	c'est une op� relev�e, on l'ajoute � la liste */

			gint place_ope_r;

			place_ope_r = g_slist_position ( LISTE_OPERATIONS,
							 liste_tmp );

			    /* si cette op� r est avant la ligne d'insertion en cours,
			     * l'iter d'insertion est celui d�j� en m�moire 
			     * donc on fait rien ; sinon, on trouve le prochain iter
			     * apr�s cette op� */

			if ( place_ope_r > ligne_ope_pour_insertion )
			{
			    /* on doit trouver la prochaine op� non R apr�s l'op� R en cours */

			    ligne_ope_pour_insertion = place_ope_r + 1;
			    struct structure_operation *operation_recherche;
			    GSList *liste_recherche;

			    liste_recherche = g_slist_nth ( LISTE_OPERATIONS,
							    ligne_ope_pour_insertion );

			    do
			    {
				operation_recherche = liste_recherche -> data;
				liste_recherche = liste_recherche -> next;
			    }
			    while ( operation_recherche -> pointe == 3
				    &&
				    liste_recherche );

			    /*   � ce niveau, soit operation_recherche pointe sur la 1ere op� non R */
			    /*apr�s l'op� R en cours */
			    /*	soit liste_recherche = NULL, et donc on a atteind la fin de la liste */
				
			    if ( liste_recherche )
			    {
				ligne_ope_pour_insertion = g_slist_position ( LISTE_OPERATIONS,
									      liste_recherche );
				iter = cherche_iter_operation ( operation_recherche );
			    }
			    else
				iter = cherche_iter_operation ( GINT_TO_POINTER (-1));
			}

			/* iter pointe maintenant sur la 1�re op� non R apr�s l'op� R en cours */
			
			remplit_ligne_operation ( operation,
						  iter );
		    }
		    liste_tmp = liste_tmp -> next;
		}
		COULEUR_BACKGROUND_FINI = 0;
/* 		AFFICHAGE_SOLDE_FINI = 0; */
		SELECTION_OPERATION_FINI = 0;

		/* 	    il faut afficher le compte courant tout de suite sinon il ne sera pas freez� */
		/* 		lors du remplissage... */

		if ( i == compte_courant )
		{
		    verification_list_store_termine ( i );
		    while ( g_main_iteration (FALSE));
		    ajuste_scrolling_liste_operations_a_selection ( i );
		}
	    }
	}
/* 	gtk_button_clicked ( GTK_BUTTON ( bouton_affiche_r )); */
    }
    else
    {
	/* 	on a demand� d'effacer les R */

	for ( i=0 ; i<nb_comptes ; i++ )
	{
	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

	    /* 	    on ne touche � la liste que s'il le faut en fonction de retient_affichage_par_compte */
	    /* 		et du compte en cours */

	    if ( !( etat.retient_affichage_par_compte
		    &&
		    i != compte_courant
		    &&
		    g_slist_length ( LISTE_OPERATIONS )))
	    {
		/* 	on fait le tour du list_store, et toute op� relev�e est supprim�e */

		gint iter_valide;

		/* 		si le store du compte n'est pas fini, il faut le finir avant de faire joujou */

		if ( SLIST_DERNIERE_OPE_AJOUTEE != GINT_TO_POINTER (-1))
		    ajoute_operations_compte_dans_list_store ( i,
							       0 );

		iter_valide = gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
							      iter );

		while ( iter_valide )
		{
		    gint j;

		    operation = operation_from_iter ( iter,
						      i );

		    if ( operation != GINT_TO_POINTER (-1)
			 &&
			 operation -> pointe == 3 )
			for ( j=0 ; j<NB_LIGNES_OPE ; j++ )
			    gtk_list_store_remove ( GTK_LIST_STORE ( STORE_LISTE_OPERATIONS ),
						    iter );
		    else
			for ( j=0 ; j<NB_LIGNES_OPE ; j++ )
			    iter_valide = gtk_tree_model_iter_next ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
								     iter );
		}
		COULEUR_BACKGROUND_FINI = 0;
/* 		AFFICHAGE_SOLDE_FINI = 0; */
		SELECTION_OPERATION_FINI = 0;

		/* 	    il faut afficher le compte courant tout de suite sinon il ne sera pas freez� */
		/* 		lors du remplissage... */

		if ( i == compte_courant )
		{
		    verification_list_store_termine ( i );
		    while ( g_main_iteration (FALSE));
		    ajuste_scrolling_liste_operations_a_selection ( i );
		}
	    }
	}
/* 	gtk_button_clicked ( GTK_BUTTON ( bouton_enleve_r )); */
    }

    /*     ici, les listes ont �t� remplies, on met en marche l'idle s'il ne l'est pas */
    /* 	pour mettre � jour les background et soldes en tache de fond */

    if ( !id_fonction_idle )
	id_fonction_idle = g_idle_add ( (GSourceFunc) utilisation_temps_idle,
					NULL );

    modification_fichier ( TRUE );
}
/******************************************************************************/


/******************************************************************************/
/* cette fonction est appel�e lorsqu'on demande un autre nb de lignes */
/* � afficher : elle va ajouter ou supprimer des lignes en fonction de */
/* l'affichage pr�c�dent */
/******************************************************************************/
void mise_a_jour_affichage_lignes ( gint nb_lignes )
{
    gint i;
    GSList *liste_lignes_affichees;
    GSList *liste_lignes_a_afficher;
    GSList *liste_lignes_en_trop;
    GSList *liste_tmp;
    GtkTreeIter *iter;
    GSList *liste_lignes_a_afficher_pour_classement;
    GSList *ordre_lignes_nouvelle_liste_pour_classement;


    /*     si nb_lignes = NB_LIGNES_OPE on ne change rien */
    /* 	on v�rifie sur le compte courant car soit c'est sur ce compte */
    /* 	qu'on travaille, soit tous les autres comptes ont la m�me valeur */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_courant;

    if ( nb_lignes == NB_LIGNES_OPE )
	return;

    if ( DEBUG )
	printf ( "mise_a_jour_affichage_lignes %d lignes\n", nb_lignes );


    /*     on va cr�er de suite 2 liste par d�faut : la liste des lignes affich�es et la liste */
    /* 	des lignes � afficher */

    switch ( NB_LIGNES_OPE )
    {
	case 1:
	    liste_lignes_affichees = g_slist_append ( NULL,
						      GINT_TO_POINTER ( ligne_affichage_une_ligne ));
	    break;

	case 2:
	    liste_lignes_affichees = g_slist_copy ( lignes_affichage_deux_lignes );
	    break;

	case 3:
	    liste_lignes_affichees = g_slist_copy ( lignes_affichage_trois_lignes );
	    break;

	case 4:
	    liste_lignes_affichees = g_slist_append ( NULL,
						      NULL);
	    liste_lignes_affichees = g_slist_append ( liste_lignes_affichees,
						      GINT_TO_POINTER (1));
	    liste_lignes_affichees = g_slist_append ( liste_lignes_affichees,
						      GINT_TO_POINTER (2));
	    liste_lignes_affichees = g_slist_append ( liste_lignes_affichees,
						      GINT_TO_POINTER (3));
	    break;

	default:
	    /* 	 normalement, pas de d�faut possible */
	    return; 
    }


    switch ( nb_lignes )
    {
	case 1:
	    liste_lignes_a_afficher = g_slist_append ( NULL,
							     GINT_TO_POINTER ( ligne_affichage_une_ligne ));
	    break;

	case 2:
	    liste_lignes_a_afficher = g_slist_copy ( lignes_affichage_deux_lignes );
	    break;

	case 3:
	    liste_lignes_a_afficher = g_slist_copy ( lignes_affichage_trois_lignes );
	    break;

	case 4:
	    liste_lignes_a_afficher = g_slist_append ( NULL,
							     NULL);
	    liste_lignes_a_afficher = g_slist_append ( liste_lignes_a_afficher,
							     GINT_TO_POINTER (1));
	    liste_lignes_a_afficher = g_slist_append ( liste_lignes_a_afficher,
							     GINT_TO_POINTER (2));
	    liste_lignes_a_afficher = g_slist_append ( liste_lignes_a_afficher,
							     GINT_TO_POINTER (3));
	    break;

	default:
	    /* 	 normalement, pas de d�faut possible */
	    return; 
    }

    /*     on utilisera liste_lignes_a_afficher pour classer les lignes de chaque op�s, mais */
    /* 	cette liste sera modifi�e, on en fait une copie */

    liste_lignes_a_afficher_pour_classement = g_slist_copy ( liste_lignes_a_afficher );

    /* ordre_lignes_nouvelle_liste_pour_classement contiendra les no de lignes apr�s modification */
    /*     dans l'ordre o� elles apparaitront avant classement */

    ordre_lignes_nouvelle_liste_pour_classement = NULL;


    /*  on commence par faire le tour des lignes affich�es */
    /* 	si la ligne affich�e fait partie des lignes � afficher, on retire la ligne � afficher de sa liste */
    /* 	si elle n'en fait pas partie, on ajoute son no aux lignes en trop */

    liste_tmp = liste_lignes_affichees;
    liste_lignes_en_trop = NULL;

    while ( liste_tmp )
    {
	gint position;

	position = g_slist_index ( liste_lignes_a_afficher,
				   liste_tmp -> data );

	if ( position == -1 )
	{
	    /*  la ligne affich�e n'est plus � afficher ensuite ; on l'ajoute � */
	    /* 	liste_lignes_en_trop */

	    liste_lignes_en_trop = g_slist_append ( liste_lignes_en_trop,
						    liste_tmp -> data );
	}
	else
	{
	    /* 	  la ligne affich�e sera gard�e dans le prochain classement, on retire donc ce no */
	    /* 	de ligne de liste_lignes_a_afficher */

	    liste_lignes_a_afficher = g_slist_remove ( liste_lignes_a_afficher,
						       liste_tmp -> data );

	    /* 	    on ajoute cette ligne dans l'ordre affich� apr�s modifs */

	    ordre_lignes_nouvelle_liste_pour_classement = g_slist_append ( ordre_lignes_nouvelle_liste_pour_classement,
									   liste_tmp -> data );
	}
	liste_tmp = liste_tmp -> next;
    }

    /*     s'il reste des lignes � afficher dans la liste liste_lignes_a_afficher, elles seront ajout�es dans */
    /* 	le m�me ordre avant classement, donc on fait de m�me pour ordre_lignes_nouvelle_liste_pour_classement */

    liste_tmp = liste_lignes_a_afficher;

    while ( liste_tmp )
    {
	ordre_lignes_nouvelle_liste_pour_classement = g_slist_append ( ordre_lignes_nouvelle_liste_pour_classement,
								       liste_tmp -> data );
	liste_tmp = liste_tmp -> next;
    }



    /* on a donc maintenant : */
    /*    liste_lignes_en_trop contient les lignes � retirer du store actuel (pour chaque op�ration)*/
    /*	  liste_lignes_a_afficher contient les lignes � rajouter au store actuel (pour chaque op�ration)*/

    /*     on peut commencer � faire le tour des op�s */

    /*     iter pointe sur une place r�serv�e en global */

    iter = &iter_liste_operations;

    /* 	on fait le tour des comptes, mais on ne fait des changements que s'il le faut, */
    /* 	en fonction de retient_affichage_par_compte et cu compte_courant */

    for ( i=0 ; i<nb_comptes ; i++ )
    {
	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

	if ( !( etat.retient_affichage_par_compte
		&&
		i != compte_courant
		&&
		g_slist_length ( LISTE_OPERATIONS )))
	{

	    /* on va faire le tour du list_store et traiter chaque op� */
	    /*     pour chaque op� : */
	    /*     on met/retire les lignes � mettre/en trop */
	    /*     en utilisant les iter d�j� cr��s si possible */
	    /*     on classe les lignes de l'op� pour mettre dans l'ordre voulu */

	    gint iter_valide;
	    gint nb_reste_lignes_blanches;
	    gint j;
	    gint ligne_en_cours;
	    gint *pos;
	    gint ligne_ope_en_cours;


	    /* 		si le store du compte n'est pas fini, il faut le finir avant de faire joujou */

	    if ( SLIST_DERNIERE_OPE_AJOUTEE != GINT_TO_POINTER (-1))
		ajoute_operations_compte_dans_list_store ( i,
							   0 );

	    nb_reste_lignes_blanches = nb_lignes;
	    ligne_en_cours = 0;
	    pos = malloc ( (GTK_LIST_STORE ( STORE_LISTE_OPERATIONS ) -> length) / NB_LIGNES_OPE * nb_lignes * sizeof (gint));

	    iter_valide = gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
							  iter );

	    while ( iter_valide )
	    {
		struct structure_operation *operation;

		operation = operation_from_iter ( iter,
						  i );

		if ( operation != GINT_TO_POINTER (-1))
		{
		    GSList *liste_lignes_a_afficher_tmp;

		    liste_lignes_a_afficher_tmp = g_slist_copy ( liste_lignes_a_afficher );

		    /* 	on a donc : */
		    /* 	liste_lignes_en_trop contient les lignes � retirer du store actuel (pour chaque op�) */
		    /* 	liste_lignes_a_afficher_tmp contient les lignes � rajouter au store actuel (pour chaque op�) */

		    /* pour chaque ligne en trop (� virer), on va r�cup�rer son iter et l'�craser par une ligne */
		    /* restant � afficher */

		    /*   on va faire le tour de chaque iter de l'op� affich�e en cours et */
		    /* v�rifier si la ligne qu'il contient est � virer/�craser ou */
		    /* � garder */

		    for ( j=0 ; j<NB_LIGNES_OPE ; j++ )
		    {
			gint no_ligne_affichee;

			gtk_tree_model_get ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
					     iter,
					     12, &no_ligne_affichee,
					     -1 );

			if ( g_slist_index ( liste_lignes_en_trop,
					     GINT_TO_POINTER ( no_ligne_affichee )) != -1 )
			{
			    /* 	ce no de ligne a �té retrouv� dans liste_lignes_en_trop, c'est qu'il faut le remplacer */
			    /* 	ou le virer */

			    if ( liste_lignes_a_afficher_tmp )
			    {
				/* il reste des lignes � afficher, on affiche en utilisant l'iter de la ligne en cours */

				affiche_ligne_ope ( operation,
						    iter,
						    GPOINTER_TO_INT ( liste_lignes_a_afficher_tmp -> data ));
				liste_lignes_a_afficher_tmp = g_slist_delete_link ( liste_lignes_a_afficher_tmp,
										    liste_lignes_a_afficher_tmp );
				iter_valide = gtk_tree_model_iter_next ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
									 iter );
			    }
			    else
			    {
				/*il n'y a plus de lignes � afficher, on supprime la ligne correspondant � l'iter */
				/* cette fonction incrémente en m�me temps iter */

				iter_valide = gtk_list_store_remove ( GTK_LIST_STORE ( STORE_LISTE_OPERATIONS ),
								      iter );
			    }
			}
			else
			    iter_valide = gtk_tree_model_iter_next ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
								     iter );
		    }

		    /* � ce niveau on a fait le tour des lignes de l'ancienne op� affich�e */
		    /*     dans tous les cas, iter pointe sur l'op� suivante */
		    /*     s'il reste des lignes � afficher, on les ins�re ici devant l'iter en cours */

		    liste_tmp = liste_lignes_a_afficher_tmp;

		    while ( liste_tmp )
		    {
			GtkTreeIter new_iter;

			gtk_list_store_insert_before ( GTK_LIST_STORE ( STORE_LISTE_OPERATIONS ),
						       &new_iter,
						       iter );
			affiche_ligne_ope ( operation,
					    &new_iter,
					    GPOINTER_TO_INT ( liste_tmp -> data ));
			liste_tmp = liste_tmp -> next;
		    }

		    /*    maintenant, les lignes correctes sont affich�es sur les lignes de l'op� */
		    /*en cours, il ne reste plus qu'� les classer */

		    /* on va utiliser le gtk_list_store_reorder ensuite */

		    ligne_ope_en_cours = ligne_en_cours;

		    for ( j=0 ; j<nb_lignes ; j++ )
		    {
			pos[ligne_en_cours] = ligne_ope_en_cours + g_slist_index ( ordre_lignes_nouvelle_liste_pour_classement,
										   g_slist_nth_data ( liste_lignes_a_afficher_pour_classement,
												      j ));
			ligne_en_cours++;
		    }
		}
		else
		{
		    /*  on est sur la ligne blanche */
		    /* 		    soit on la laisse, soit on la vire */

		    if ( nb_reste_lignes_blanches )
		    {
			iter_valide = gtk_tree_model_iter_next ( GTK_TREE_MODEL ( STORE_LISTE_OPERATIONS ),
								 iter );
			nb_reste_lignes_blanches--;
			pos[ligne_en_cours] = ligne_en_cours;
			ligne_en_cours++;
		    }
		    else
			iter_valide = gtk_list_store_remove ( GTK_LIST_STORE ( STORE_LISTE_OPERATIONS ),
							      iter );
		}
	    }
	    
	    /* 	    on est arriv� au bout de la list_store  */
	    /* 		s'il reste des lignes blanches � ajouter, on le fait ici */

	    for ( j=0 ; j<nb_reste_lignes_blanches ; j++)
	    {
		gint k;

		/* on met � NULL tout les pointeurs */

		gtk_list_store_append ( STORE_LISTE_OPERATIONS,
					iter );

		for ( k=0 ; k<TRANSACTION_LIST_COL_NB ; k++ )
		    gtk_list_store_set ( STORE_LISTE_OPERATIONS,
					 iter,
					 k, NULL,
					 -1 );

		/* on met le no d'op�ration de cette ligne � -1 */

		gtk_list_store_set ( STORE_LISTE_OPERATIONS,
				     iter,
				     9, GINT_TO_POINTER (-1),
				     -1 );

		pos[ligne_en_cours] = ligne_en_cours;
		ligne_en_cours++;
	    }

	    /* 	    il reste � classer les lignes dans leurs op�s */

	    /* FIXME : si gtk2.4 appeler gtk_list_store_reorder */
	    /* 	    sinon my_list_store_reorder  */

	    my_list_store_reorder ( STORE_LISTE_OPERATIONS,
				    pos );

	    free (pos);

	    /* 	    on a fini ce compte, on peut y mettre NB_LIGNES_OPE */
	    /* 		et remettre les couleurs du fond */
	    /* 	    pour les soldes, si on a voulu afficher la cellule de solde,  */
	    /* 	    AFFICHAGE_SOLDE_FINI s'est d�j� mis � 0 tout seul */

		NB_LIGNES_OPE = nb_lignes;
		COULEUR_BACKGROUND_FINI = 0;

		if ( i == compte_courant )
		{
		    verification_list_store_termine ( i );
		}
	}
    }

    while ( g_main_iteration (FALSE));
    ajuste_scrolling_liste_operations_a_selection ( compte_courant );

    /*     ici, les listes ont �t� remplies, on met en marche l'idle s'il ne l'est pas */
    /* 	pour mettre � jour les background et soldes en tache de fond */

    if ( !id_fonction_idle )
	id_fonction_idle = g_idle_add ( (GSourceFunc) utilisation_temps_idle,
					NULL );

    modification_fichier ( TRUE );
}
/******************************************************************************/



/******************************************************************************/
/* cette fonction classe la sliste des op�s du compte donn� en argument */
/* en fonction du classement courant de ce compte */
/******************************************************************************/
void classe_liste_operations ( gint no_compte )
{
    if ( DEBUG )
	printf ( "classe_liste_operations compte n�%d\n", no_compte );

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;

    LISTE_OPERATIONS = g_slist_sort ( LISTE_OPERATIONS,
				      (GCompareFunc) CLASSEMENT_COURANT );
}
/******************************************************************************/

