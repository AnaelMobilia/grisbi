/* ce fichier de la gestion de l'import de fichiers (qif, ofx...) */


/*     Copyright (C) 2000-2003  Cédric Auger */
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
#include "import.h"
#include "constants.h"



#include "accueil.h"
#include "categories_onglet.h"
#include "comptes_gestion.h"
#include "comptes_onglet.h"
#include "comptes_traitements.h"
#include "devises.h"
#include "dialog.h"
#include "fenetre_principale.h"
#include "fichiers_gestion.h"
#include "html.h"
#include "gnucash.h"
#include "ofx.h"
#include "operations_classement.h"
#include "operations_comptes.h"
#include "operations_liste.h"
#include "patienter.h"
#include "qif.h"
#include "search_glist.h"
#include "tiers_onglet.h"
#include "traitement_variables.h"
#include "type_operations.h"
#include "utils.h"
#include "utils_files.h"
#include "utils_file_selection.h"




gint derniere_operation_enregistrement_ope_import;
gint valeur_echelle_recherche_date_import;
extern GtkWidget *window_vbox_principale;

extern GtkWidget *widget_formulaire_echeancier[SCHEDULER_FORM_TOTAL_WIDGET];


/* *******************************************************************************/
/* fonction importer_fichier */
/* appelée par le menu importer */
/* *******************************************************************************/

void importer_fichier ( void )
{
    liste_comptes_importes = NULL;
    dialog_recapitulatif = NULL;
    virements_a_chercher = 0;

    selection_fichiers_import ();
}
/* *******************************************************************************/



/* *******************************************************************************/
/* Affiche la fenètre de sélection de fichier pour l'import */
/* *******************************************************************************/

void selection_fichiers_import ( void )
{
    GtkWidget *fenetre;


    fenetre = gtk_file_selection_new ( _("Select files to import") );
    gtk_window_set_transient_for ( GTK_WINDOW ( fenetre ),
				   GTK_WINDOW ( window ));
    gtk_window_set_modal ( GTK_WINDOW ( fenetre ),
			   TRUE );
    gtk_file_selection_set_select_multiple ( GTK_FILE_SELECTION ( fenetre ), TRUE );
    file_selection_set_filename ( GTK_FILE_SELECTION ( fenetre ),
				      dernier_chemin_de_travail );

    gtk_signal_connect_object (GTK_OBJECT ( GTK_FILE_SELECTION( fenetre ) -> ok_button ),
			       "clicked",
			       GTK_SIGNAL_FUNC ( fichier_choisi_importation ),
			       GTK_OBJECT ( fenetre ));
    gtk_signal_connect_object (GTK_OBJECT ( GTK_FILE_SELECTION( fenetre ) -> cancel_button ),
			       "clicked",
			       GTK_SIGNAL_FUNC (gtk_widget_destroy),
			       GTK_OBJECT ( fenetre ));
    gtk_signal_connect ( GTK_OBJECT ( fenetre ),
			 "destroy",
			 GTK_SIGNAL_FUNC ( affichage_recapitulatif_importation ),
			 NULL );
    gtk_widget_show ( fenetre );


}
/* *******************************************************************************/



/* *******************************************************************************/
gboolean fichier_choisi_importation ( GtkWidget *fenetre )
{
    /* un ou plusieurs fichiers ont été sélectionnés dans la gtk_file_selection en argument */
    /* on va récupérer ces fichiers et les trier par qif/ofx/web */

    gchar **liste_selection;
    gint i;	
    gboolean result = TRUE;

    /* on sauve le répertoire courant  */

    dernier_chemin_de_travail = file_selection_get_last_directory(GTK_FILE_SELECTION ( fenetre ) ,TRUE);

    /* on va récupérer tous les fichiers sélectionnés puis proposer d'en importer d'autres */


    liste_selection = file_selection_get_selections ( GTK_FILE_SELECTION ( fenetre ));
    i=0;

    while ((result == TRUE)&&( liste_selection[i]))
    {
	FILE *fichier;
	gchar *pointeur_char = NULL;


	/* on ouvre maintenant le fichier pour tester sa structure */

	if ( !( fichier = utf8_fopen ( liste_selection[i],
				  "r" )))
	{
	    /* on n'a pas réussi à ouvrir le fichier, on affiche l'erreur et on retourne sur la sélection des fichiers */

	    dialogue ( latin2utf8 ( strerror ( errno ) ) );
            return FALSE;
	}


	/*       le fichier est ouvert, on va trier entre qif/ofx/html */
	/* 	le plus simple est ofx, on a <OFX> au départ */
	/* 	bon, en fait, non... on peut avoir ofx ou OFX n'importe o... */
	/*       pour le qif, on recherche !Type, !Account ou !Option */
	/* si ce n'est aucun des 3, on regarde s'il y a une ouverture de balise < dans ce cas */
	/* on fait comme si c'était du html */
	/*       chacune des fonctions récupèrent les infos du compte et les opés, et ajoutent la struct_compte_importation */
	/* 	à liste_comptes_importes */

	/* 	certains fichiers contiennent une ligne blanche au début... */

	do
	    get_line_from_file ( fichier,
				 &pointeur_char );
	while ( strlen ( pointeur_char ) == 1 );

/* #ifdef _WIN32 */
/*         { */
	    /* fscanf %a format has a very strange behaviour under WIN32 ...so we simulate it ... */
/*             gchar c = 0; */
/*             gint j  = 0; */
/*             pointeur_char = (gchar*)realloc(pointeur_char,256*sizeof(gchar)); */
/*             while ( (j<254) && ( c != '\n' ) && (c != '\r')) */
/*             { */
/*                 c =(gchar)fgetc(fichier); */
/*                 if (feof(fichier)) break; */
/*                 pointeur_char[j++] = c; */
/*             } */
/*             pointeur_char[j] = 0; */
/*         } */
/* #else */
/* 	fscanf ( fichier, */
/* 		 "%a[^\n]\n", */
/* 		 &pointeur_char ); */
/* #endif */
	if ( g_strrstr ( pointeur_char,
			 "ofx" )
	     ||
	     g_strrstr ( pointeur_char,
			 "OFX" ))
        {
	    result = recuperation_donnees_ofx ( liste_selection[i]);
        }
	else if ( !my_strncasecmp ( pointeur_char, "!Type", 5 ) ||
		  !my_strncasecmp ( pointeur_char, "!Account", 8 ) || 
		  !my_strncasecmp ( pointeur_char, "!Option", 7 ))
        {
		result = recuperation_donnees_qif ( fichier, FALSE );
        }
	else if ( !strncmp ( pointeur_char, "<?xml", 5 ))
	{
	  get_line_from_file ( fichier, &pointeur_char );
	  if ( !strncmp ( pointeur_char, "<gnc-v2", 7 ))
	  {
	    result = recuperation_donnees_gnucash ( liste_selection[i] );
	  }	  
	  else
	    {
	      dialogue_error_hint ( _("Grisbi is unable to determine type of this file.  If it is a QIF, an OFX file or a gnucash file, please contact the grisbi team to resolve this problem."),
				    g_strdup_printf ( _("File \"%s\" cannot be imported."), liste_selection[i]) );
	      result = FALSE;
	      break;
	    }
	}
	else
	{
	  dialogue_error_hint ( _("Grisbi is unable to determine type of this file.  If it is a QIF, an OFX file or a gnucash file, please contact the grisbi team to resolve this problem."),
				g_strdup_printf ( _("File \"%s\" cannot be imported."), liste_selection[i]) );
	  result = FALSE;
	}
        /* clean up */
	fclose ( fichier );
        if (pointeur_char) free ( pointeur_char );
        pointeur_char = NULL;
        
        /* In case of error, return to file selection dialog, else import next selected file */
        if (!result) return result;
	
        i++;
    }

    /* le fait de détruire la fenetre va envoyer directement sur l'affichage des infos */

    gtk_widget_destroy ( fenetre );
    return ( result );
}
/* *******************************************************************************/



/* *******************************************************************************/
gboolean affichage_recapitulatif_importation ( void )
{
    /* on affiche un tableau récapitulatif des comptes importés */
    /* propose l'action à faire pour chaque compte et propose */
    /* d'en importer d'autres */

    gint retour;
    GtkWidget *label;
    GSList *liste_tmp;
    GtkWidget *scrolled_window;
    GtkWidget *bouton;
    GtkWidget *hbox;

    if ( !liste_comptes_importes )
	return (FALSE);


    if ( dialog_recapitulatif )
    {
	/*  la boite a déjà été crée, on ajoute les nouveaux comptes à la suite */

	/* on vérifie déjà s'il y a plus d'éléments dans la liste que de lignes sur le tableau */

	if ( g_slist_length ( liste_comptes_importes ) > ( GTK_TABLE ( table_recapitulatif ) -> nrows - 1 ))
	{
	    /* on démarre au nouveaux comptes */

	    liste_tmp = g_slist_nth ( liste_comptes_importes,
				      GTK_TABLE ( table_recapitulatif ) -> nrows - 1 );

	    while ( liste_tmp )
	    {
		cree_ligne_recapitulatif ( liste_tmp -> data,
					   g_slist_position ( liste_comptes_importes,
							      liste_tmp ) + 1);
		liste_tmp = liste_tmp -> next;
	    }
	}

	gtk_widget_show ( dialog_recapitulatif );

    }
    else
    {
	/* la boite n'a pas encore été crée, on le fait */

	dialog_recapitulatif = gtk_dialog_new_with_buttons ( _("Actions on imported accounts:" ),
							     GTK_WINDOW ( window ),
							     GTK_DIALOG_MODAL,
							     _("Add more accounts"), 1,
							     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							     GTK_STOCK_OK, GTK_RESPONSE_OK,
							     NULL );
	gtk_window_set_policy ( GTK_WINDOW ( dialog_recapitulatif ),
				TRUE,
				TRUE,
				FALSE );

	if ( nb_comptes )
	    gtk_widget_set_usize ( dialog_recapitulatif,
				   900,
				   400 );
	else
	    gtk_widget_set_usize ( dialog_recapitulatif,
				   650,
				   400 );


	label = gtk_label_new ( _("List of the selection of accounts :"));
	gtk_box_pack_start ( GTK_BOX ( GTK_DIALOG ( dialog_recapitulatif ) -> vbox ),
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
	gtk_box_pack_start ( GTK_BOX ( GTK_DIALOG ( dialog_recapitulatif ) -> vbox ),
			     scrolled_window,
			     TRUE,
			     TRUE,
			     0 );
	gtk_widget_show ( scrolled_window );


	table_recapitulatif = gtk_table_new ( g_slist_length ( liste_comptes_importes ),
					      7,
					      FALSE );
	gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW ( scrolled_window ),
						table_recapitulatif );
	gtk_container_set_border_width ( GTK_CONTAINER ( table_recapitulatif ), 10 );
	gtk_table_set_col_spacings ( GTK_TABLE(table_recapitulatif), 6 );
	gtk_table_set_row_spacings ( GTK_TABLE(table_recapitulatif), 6 );
	gtk_widget_show ( table_recapitulatif );

	/* on met les titres des colonnes */

	label = gtk_label_new ( _( "Date" ));
	gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
			   label,
			   0, 1,
			   0 ,1,
			   GTK_SHRINK,
			   GTK_SHRINK,
			   0, 0 );
	gtk_widget_show ( label );

	label = gtk_label_new ( _( "Name" ));
	gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
			   label,
			   1, 2,
			   0 ,1,
			   GTK_SHRINK,
			   GTK_SHRINK,
			   0, 0 );
	gtk_widget_show ( label );

	label = gtk_label_new ( _( "Currency" ));
	gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
			   label,
			   2, 3,
			   0 ,1,
			   GTK_SHRINK,
			   GTK_SHRINK,
			   0, 0 );
	gtk_widget_show ( label );

	label = gtk_label_new ( _( "Action" ));
	gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
			   label,
			   3, 4,
			   0 ,1,
			   GTK_SHRINK,
			   GTK_SHRINK,
			   0, 0 );
	gtk_widget_show ( label );

	if ( nb_comptes )
	{
	    label = gtk_label_new ( _( "Account" ));
	    gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
			       label,
			       4, 5,
			       0 ,1,
			       GTK_SHRINK,
			       GTK_SHRINK,
			       0, 0 );
	    gtk_widget_show ( label );
	}

	label = gtk_label_new ( _( "Type of account" ));
	gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
			   label,
			   5, 6,
			   0 ,1,
			   GTK_SHRINK,
			   GTK_SHRINK,
			   0, 0 );
	gtk_widget_show ( label );

	label = gtk_label_new ( _( "Origine" ));
	gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
			   label,
			   6, 7,
			   0 ,1,
			   GTK_SHRINK,
			   GTK_SHRINK,
			   0, 0 );
	gtk_widget_show ( label );



	/* si aucun compte n'est ouvert, on crée les devises de base */

	if ( !nb_comptes )
	{
	    init_variables ( FALSE );
	    creation_devises_de_base ();
	}

	liste_tmp = liste_comptes_importes;

	while ( liste_tmp )
	{
	    cree_ligne_recapitulatif ( liste_tmp -> data,
				       g_slist_position ( liste_comptes_importes,
							  liste_tmp ) + 1);
	    liste_tmp = liste_tmp -> next;
	}

	/* on rajoute ensuite le bouton pour ajouter des devises */

	hbox = gtk_hbox_new ( FALSE,
			      0 );
	gtk_box_pack_start ( GTK_BOX ( GTK_DIALOG ( dialog_recapitulatif ) -> vbox ),
			     hbox,
			     FALSE,
			     FALSE,
			     0 );
	gtk_widget_show ( hbox );

	bouton = gtk_button_new_with_label ( _("Add a currency") );
	gtk_button_set_relief ( GTK_BUTTON ( bouton ),
				GTK_RELIEF_NONE );
	gtk_signal_connect ( GTK_OBJECT ( bouton ),
			     "clicked",
			     GTK_SIGNAL_FUNC ( ajout_devise_dans_liste_import ),
			     NULL );
	gtk_box_pack_start ( GTK_BOX ( hbox ),
			     bouton,
			     TRUE,
			     FALSE,
			     0 );
	gtk_widget_show ( bouton );
    }

    retour = gtk_dialog_run ( GTK_DIALOG ( dialog_recapitulatif ));

    switch ( retour )
    {
	case GTK_RESPONSE_OK:

	    /*  on a appuyé sur ok, il ne reste plus qu'à traiter les infos */

	    traitement_operations_importees ();
	    gtk_widget_destroy ( dialog_recapitulatif );
	    break;

	case 1:
	    /*  on a appuyé sur ajouter, on réaffiche la boite de sélection de fichier */

	    gtk_widget_hide ( dialog_recapitulatif );
	    selection_fichiers_import ();

	    break;

	default:
	    /* on annule */
	    gtk_widget_destroy ( dialog_recapitulatif );

    }
    return ( FALSE );
}
/* *******************************************************************************/

/* *******************************************************************************/
void ajout_devise_dans_liste_import ( void )
{
    /* permet d'ajouter une devise au moment de l'importation d'opérations */

    GSList *liste_tmp;


    ajout_devise (NULL);


    /*     on met maintenant à jour les options menu des devise dans la liste des comptes importés */

    liste_tmp = liste_comptes_importes;

    while ( liste_tmp )
    {
	struct struct_compte_importation *compte;

	compte = liste_tmp -> data;
	gtk_option_menu_set_menu ( GTK_OPTION_MENU ( compte -> bouton_devise ),
				   creation_option_menu_devises ( 0,
								  liste_struct_devises ));
	gtk_widget_show_all ( compte -> bouton_devise );
	liste_tmp = liste_tmp -> next;
    }
}
/* *******************************************************************************/


/* *******************************************************************************/
void cree_ligne_recapitulatif ( struct struct_compte_importation *compte,
				gint position )
{
    /* crée la ligne du compte en argument dans le récapitulatif à la position donnée */

    GtkWidget *label;
    GtkWidget *menu;
    GtkWidget *menu_item;
    GSList *liste_tmp;
    gint no_compte_trouve;
    GSList *ordre_comptes_variable;


    /* mise en place de la date si elle existe */

    if ( compte -> date_fin )
    {
	label = gtk_label_new ( g_strdup_printf ( "%02d/%02d/%d",
						  g_date_day ( compte -> date_fin ),
						  g_date_month ( compte -> date_fin ),
						  g_date_year ( compte -> date_fin )));
	gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
			   label,
			   0, 1,
			   position, position+1,
			   GTK_SHRINK,
			   GTK_SHRINK,
			   0, 0 );
	gtk_widget_show ( label );
    }
    else
    {
	label = gtk_label_new ( _("none"));
	gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
			   label,
			   0, 1,
			   position, position+1,
			   GTK_SHRINK,
			   GTK_SHRINK,
			   0, 0 );
	gtk_widget_show ( label );
    }


    /* mise en place du nom du compte s'il existe */

    if ( compte -> nom_de_compte )
    {
	label = gtk_label_new ( compte -> nom_de_compte );
	gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
			   label,
			   1, 2,
			   position, position+1,
			   GTK_SHRINK,
			   GTK_SHRINK,
			   0, 0 );
	gtk_widget_show ( label );
    }


    /* on crée le bouton de choix de devise */

    compte -> bouton_devise = gtk_option_menu_new ();
    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( compte -> bouton_devise ),
			       creation_option_menu_devises ( 0,
							      liste_struct_devises ));
    gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
		       compte -> bouton_devise,
		       2, 3,
		       position, position+1,
		       GTK_SHRINK,
		       GTK_SHRINK,
		       0, 0 );
    gtk_widget_show ( compte -> bouton_devise );


    if ( compte -> devise )
    {
	liste_tmp = g_slist_find_custom ( liste_struct_devises,
					  compte->devise,
					  (GCompareFunc) recherche_devise_par_code_iso );

	if ( liste_tmp )
	    gtk_option_menu_set_history ( GTK_OPTION_MENU ( compte -> bouton_devise ),
					  g_slist_position ( liste_struct_devises,
							     liste_tmp ));
	else
	{
	    /* 	    la devise avait un nom mais n'a pas été retrouvée (n'existe que pour ofx); 2 possibilités : */
	    /* 		soit elle n'est pas crée (l'utilisateur la créera une fois la fenetre affichée) */
	    /* 		soit elle est crée mais pas avec le bon code */

	    dialogue_warning_hint ( g_strdup_printf ( _( "Currency of imported account '%s' is %s.  Either this currency doesn't exist so you have to create it in dialog window, or this currency already exists but the ISO code is wrong.\nTo avoid this message, please set its ISO code in configuration."),
						      compte -> nom_de_compte,
						      compte -> devise ),
				    g_strdup_printf ( _("Can't associate ISO 4217 code for currency '%s'."),  compte -> devise ));

	}
    }

    /* on crée les boutons de comptes et de type de compte tout de suite */
    /*   pour les (dé-sensitiver lors de changement de l'action */

    compte -> bouton_compte = gtk_option_menu_new ();
    compte -> bouton_type_compte = gtk_option_menu_new ();


    /* on crée le bouton de l'action demandée */
    /* si aucun fichier n'est ouvert, on ne propose que créer un compte */

    compte -> bouton_action = gtk_option_menu_new ();

    menu = gtk_menu_new ();

    menu_item = gtk_menu_item_new_with_label ( _("Create a new account"));
    gtk_object_set_data ( GTK_OBJECT ( menu_item ),
			  "no_action",
			  GINT_TO_POINTER ( 0 ) );
    if ( nb_comptes )
	gtk_signal_connect_object ( GTK_OBJECT ( menu_item ),
				    "activate",
				    GTK_SIGNAL_FUNC ( desensitive_widget ),
				    GTK_OBJECT ( compte -> bouton_compte ));
    gtk_signal_connect_object ( GTK_OBJECT ( menu_item ),
				"activate",
				GTK_SIGNAL_FUNC ( sensitive_widget ),
				GTK_OBJECT ( compte -> bouton_type_compte ));
    gtk_menu_append ( GTK_MENU ( menu ),
		      menu_item );
    gtk_widget_show ( menu_item );

    if ( nb_comptes )
    {
	menu_item = gtk_menu_item_new_with_label ( _("Add the transactions"));
	gtk_object_set_data ( GTK_OBJECT ( menu_item ),
			      "no_action",
			      GINT_TO_POINTER ( 1 ));
	gtk_signal_connect_object ( GTK_OBJECT ( menu_item ),
				    "activate",
				    GTK_SIGNAL_FUNC ( sensitive_widget ),
				    GTK_OBJECT ( compte -> bouton_compte ));
	gtk_signal_connect_object ( GTK_OBJECT ( menu_item ),
				    "activate",
				    GTK_SIGNAL_FUNC ( desensitive_widget ),
				    GTK_OBJECT ( compte -> bouton_type_compte ));
	gtk_menu_append ( GTK_MENU ( menu ),
			  menu_item );
	gtk_widget_show ( menu_item );

	menu_item = gtk_menu_item_new_with_label ( _("Mark transactions"));
	gtk_object_set_data ( GTK_OBJECT ( menu_item ),
			      "no_action",
			      GINT_TO_POINTER ( 2 ) );
	gtk_signal_connect_object ( GTK_OBJECT ( menu_item ),
				    "activate",
				    GTK_SIGNAL_FUNC ( sensitive_widget ),
				    GTK_OBJECT ( compte -> bouton_compte ));
	gtk_signal_connect_object ( GTK_OBJECT ( menu_item ),
				    "activate",
				    GTK_SIGNAL_FUNC ( desensitive_widget ),
				    GTK_OBJECT ( compte -> bouton_type_compte ));
	gtk_menu_append ( GTK_MENU ( menu ),
			  menu_item );
	gtk_widget_show ( menu_item );
    }

    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( compte -> bouton_action ),
			       menu );
    gtk_widget_show ( menu );
    gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
		       compte -> bouton_action,
		       3, 4,
		       position, position+1,
		       GTK_SHRINK,
		       GTK_SHRINK,
		       0, 0 );
    gtk_widget_show ( compte -> bouton_action );


    /* on crée le bouton du compte sélectionné */
    /* si aucun fichier n'est ouvert, on ne crée pas ce bouton */

    no_compte_trouve = -1;

    if ( nb_comptes )
    {
	menu = gtk_menu_new ();

	ordre_comptes_variable = ordre_comptes;

	do
	{
	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + GPOINTER_TO_INT ( ordre_comptes_variable -> data );
	
	    if ( !COMPTE_CLOTURE )
	    {
		menu_item = gtk_menu_item_new_with_label ( NOM_DU_COMPTE );
		gtk_object_set_data ( GTK_OBJECT ( menu_item ),
				      "no_compte",
				      GINT_TO_POINTER ( p_tab_nom_de_compte_variable - p_tab_nom_de_compte ));

	    /* on recherche quel compte était noté dans le fichier  */
	    /* s'il y a une id, on la prend en priorité sur le nom */

		if ( compte -> id_compte
		     &&
		     ID_COMPTE
		     &&
		     !g_strcasecmp ( compte -> id_compte,
				     ID_COMPTE ))
		    no_compte_trouve = p_tab_nom_de_compte_variable - p_tab_nom_de_compte;

		/* on ne passe par cette étape que si le compte n'a pas déjà été trouvé avec l'id */

		if ( no_compte_trouve == -1
		     &&
		     compte -> nom_de_compte
		     &&
		     !g_strcasecmp ( compte -> nom_de_compte,
				     NOM_DU_COMPTE ))
		    no_compte_trouve = p_tab_nom_de_compte_variable - p_tab_nom_de_compte;


		gtk_menu_append ( GTK_MENU ( menu ),
				  menu_item );
		gtk_widget_show ( menu_item );
	    }
	}
	while ( (  ordre_comptes_variable = ordre_comptes_variable -> next ) );

	gtk_option_menu_set_menu ( GTK_OPTION_MENU ( compte -> bouton_compte ),
				   menu );
	gtk_widget_show ( menu );

	gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
			   compte -> bouton_compte,
			   4, 5,
			   position, position+1,
			   GTK_SHRINK,
			   GTK_SHRINK,
			   0, 0 );
	gtk_widget_show ( compte -> bouton_compte );

    }

    /* on crée le bouton du type de compte  */


    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( compte -> bouton_type_compte ),
			       creation_menu_type_compte() );
    gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
		       compte -> bouton_type_compte,
		       5, 6,
		       position, position+1,
		       GTK_SHRINK,
		       GTK_SHRINK,
		       0, 0 );
    gtk_widget_show ( compte -> bouton_type_compte );

    switch ( compte -> type_de_compte )
    {
	case 3:
	    /* actif ou passif */

	    gtk_option_menu_set_history ( GTK_OPTION_MENU ( compte -> bouton_type_compte ),
					  2 );
	    break;

	case 7:
	    /* cash */

	    gtk_option_menu_set_history ( GTK_OPTION_MENU ( compte -> bouton_type_compte ),
					  1 );
	    break;
    }

    /* mise en place de l'origine */

    switch ( compte -> origine )
    {
	case 0 :
	    label = gtk_label_new ( _( "QIF file"));
	    break;

	case 1:
	    label = gtk_label_new ( _( "OFX file"));
	    break;

	case 2:
	    label = gtk_label_new ( _( "HTML file"));
	    break;

	default: 
	    label = gtk_label_new ( _("Unknown"));
    }

    gtk_table_attach ( GTK_TABLE ( table_recapitulatif ),
		       label,
		       6, 7,
		       position, position+1,
		       GTK_SHRINK,
		       GTK_SHRINK,
		       0, 0 );
    gtk_widget_show ( label );

    /* 	si on a trouvé un compte qui correspond, on l'affiche, et on passe le 1er option menu à ajouter les opérations */

    if ( no_compte_trouve != -1 )
    {
	gtk_option_menu_set_history ( GTK_OPTION_MENU ( compte -> bouton_compte ),
				      no_compte_trouve );
	gtk_option_menu_set_history ( GTK_OPTION_MENU ( compte -> bouton_action ),
				      1 );
	gtk_widget_set_sensitive ( compte -> bouton_type_compte,
				   FALSE );
    }
    else
	gtk_widget_set_sensitive ( compte -> bouton_compte,
				   FALSE );

}
/* *******************************************************************************/



/* *******************************************************************************/
void traitement_operations_importees ( void )
{
    /* cette fonction va faire le tour de liste_comptes_importes */
    /* et faire l'action demandée pour chaque compte importé */

    GSList *liste_tmp;
    gint nouveau_fichier;

    /* fait le nécessaire si aucun compte n'est ouvert */

    if ( nb_comptes )
	nouveau_fichier = 0;
    else
    {
	init_variables ( TRUE );
	creation_liste_categories ();

	nouveau_fichier = 1;
	affiche_titre_fenetre();

	/*   la taille des colonnes est automatique au départ, on y met les rapports de base */

	etat.largeur_auto_colonnes = 1;
	rapport_largeur_colonnes[0] = 11;
	rapport_largeur_colonnes[1] = 13;
	rapport_largeur_colonnes[2] = 30;
	rapport_largeur_colonnes[3] = 3;
	rapport_largeur_colonnes[4] = 11;
	rapport_largeur_colonnes[5] = 11;
	rapport_largeur_colonnes[6] = 11;
    }


    liste_tmp = liste_comptes_importes;

    while ( liste_tmp )
    {
	struct struct_compte_importation *compte;

	compte = liste_tmp -> data;

	switch ( GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( compte -> bouton_action ) -> menu_item ),
							 "no_action" )))
	{
	    case 0:
		/* créer */

		creation_compte_importe ( compte,
					  nouveau_fichier );
		break;

	    case 1:
		/* ajouter */

		ajout_opes_importees ( compte );

		break;

	    case 2:
		/* pointer */

		pointe_opes_importees ( compte );

		break;
	}
	liste_tmp = liste_tmp -> next;
    }


    /* les différentes liste d'opérations ont été créées, on va faire le tour des opés */
    /* pour retrouver celles qui ont relation_no_compte à -2 */
    /* c'est que c'est un virement, il reste à retrouver le compte et l'opération correspondants */

    /* virements_a_chercher est à 1 si on doit chercher des relations entre opés importées */

    if ( virements_a_chercher )
	cree_liens_virements_ope_import ();


    /* création des listes d'opés */

    mise_en_route_attente ( _("Please wait") );

    if ( nouveau_fichier )
    {
	creation_fenetre_principale();
	creation_listes_operations ();
	changement_compte ( GINT_TO_POINTER ( compte_courant ) );
	gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_general ),
				0 );

	gtk_box_pack_start ( GTK_BOX ( window_vbox_principale),
			     notebook_general,
			     TRUE,
			     TRUE,
			     0 );
	gtk_widget_show ( notebook_general );

    }
    else
    {
	/* on fait le tour des comptes ajoutés pour leur créer une liste d'opés */

	gint i;

	for ( i=0 ; i<nb_comptes ; i++ )
	{
	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

	    if ( !CLIST_OPERATIONS )
	    {
		LISTE_OPERATIONS = g_slist_sort ( LISTE_OPERATIONS,
						  ( GCompareFunc ) classement_sliste );

		ajoute_nouvelle_liste_operation( i );
	    }
	}
	/* on met à jour tous les comptes */

	demande_mise_a_jour_tous_comptes ();

	/* on recrée les combofix des tiers et des catégories */

	mise_a_jour_tiers ();
	mise_a_jour_categ();

	/* on met à jour l'option menu du formulaire des échéances */

	update_liste_comptes_accueil ();
	update_options_menus_comptes ();
	mise_a_jour_soldes_minimaux ();

	affiche_titre_fenetre ();
	reaffiche_liste_comptes();
	reaffiche_liste_comptes_onglet ();

    }

    annulation_attente();

    modification_fichier ( TRUE );
}
/* *******************************************************************************/

/* *******************************************************************************/
/* cette fontion recherche des opés qui sont des virements non encore reliés après un import
   dans ce cas ces opés sont marquées à -2 en relation_no_compte et info_banque_guichet contient
   le nom du compte de virement. la fonction crée donc les liens entre virements */
/* *******************************************************************************/

void cree_liens_virements_ope_import ( void )
{
    /* on fait le tour de toutes les opés des comptes */
    /* si une opé à une relation_no_compte à -2 , */
    /* on recherche le compte associé dont le nom est dans info_banque_guichet */
    /*   et une opé ayant une relation_no_compte à -2, le nom du compte dans info_banque_guichet */
    /* le même montant, le même jour avec le même tiers */

    gint i;

    for ( i=0 ; i<nb_comptes ; i++ )
    {
	gchar *nom_compte_courant;
	GSList *liste_tmp;
	gint currency;

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

	nom_compte_courant = NOM_DU_COMPTE;
	liste_tmp = LISTE_OPERATIONS;
	currency = DEVISE;

	while ( liste_tmp )
	{
	    struct structure_operation *operation;

	    operation = liste_tmp -> data;

	    /* on fait la sélection sur relation_no_compte */

	    if ( operation -> relation_no_compte == -2 )
	    {
		/* recherche du compte associé */

		gint j;
		gint compte_trouve;

		compte_trouve = -1;

		for ( j=0 ; j<nb_comptes ; j++ )
		{
		    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + j;

		    if ( ( !g_strcasecmp ( g_strconcat ( "[", NOM_DU_COMPTE, "]", NULL ),
					   g_strstrip ( g_strdup ( operation -> info_banque_guichet )))) ||
			 ( !g_strcasecmp ( NOM_DU_COMPTE ,
					   g_strstrip ( g_strdup ( operation -> info_banque_guichet )))))
			 
			compte_trouve = j;
		}

		/* 		  si on n'a pas trouvé de relation avec l'autre compte, on vire les liaisons */
		/* et ça devient une opé normale sans catégorie */

		if ( compte_trouve == -1 )
		{
		    operation -> relation_no_compte = 0;
		    operation -> relation_no_operation = 0;
		    operation -> info_banque_guichet = NULL;
		}
		else
		{
		    /*  on a trouvé le compte opposé ; on cherche maintenant l'opération */

		    GSList *pointeur_tmp;
		    gboolean same_currency = FALSE;

		    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_trouve;
		    if ( currency == DEVISE )
		      same_currency = TRUE;

		    pointeur_tmp = LISTE_OPERATIONS;

		    while ( pointeur_tmp )
		    {
			struct structure_operation *operation_2;

			operation_2 = pointeur_tmp -> data;

			if ( operation_2 -> relation_no_compte == -2
			     &&
			     operation_2 -> info_banque_guichet
			     &&
			     (!g_strcasecmp ( g_strconcat ("[", nom_compte_courant, "]", NULL),
					      g_strstrip ( g_strdup ( operation_2 -> info_banque_guichet ))) ||
			      !g_strcasecmp ( nom_compte_courant,
					      g_strstrip ( g_strdup ( operation_2 -> info_banque_guichet ))))
			     &&
			     ( !same_currency || fabs ( operation -> montant ) == fabs ( operation_2 -> montant ))
			     &&
			     ( operation -> tiers == operation_2 -> tiers )
			     &&
			     !g_date_compare ( operation -> date, operation_2 -> date ))
			{
			    /* la 2ème opération correspond en tout point à la 1ère, on met les relations */

			    operation -> relation_no_operation = operation_2 -> no_operation;
			    operation -> relation_no_compte = operation_2 -> no_compte;

			    operation_2 -> relation_no_operation = operation -> no_operation;
			    operation_2 -> relation_no_compte = operation -> no_compte;

			    operation_2 -> pointe = operation -> pointe;

			    operation -> info_banque_guichet = NULL;
			    operation_2 -> info_banque_guichet = NULL;
			}
			pointeur_tmp = pointeur_tmp -> next;
		    }

		    /*   on a fait le tour de l'autre compte, si aucune contre opération n'a été trouvée, on vire les */
		    /* relations et ça devient une opé normale */

		    if ( operation -> relation_no_compte == -2 )
		    {
			operation -> relation_no_compte = 0;
			operation -> relation_no_operation = 0;
			operation -> info_banque_guichet = NULL;
		    }
		}
	    }
	    liste_tmp = liste_tmp -> next;
	}
    }
}
/* *******************************************************************************/



/* *******************************************************************************/
void creation_compte_importe ( struct struct_compte_importation *compte_import,
			       gint nouveau_fichier )
{
    /* crée un nouveau compte contenant les données de la structure importée */
    /* ajoute ce compte aux anciens et crée la liste des opérations */

    struct donnees_compte *compte;
    gdouble solde_courant;
    struct structure_operation *operation;
    gint derniere_operation;
    GSList *liste_tmp;

    /* on commence par enregistrer le compte */

    compte = calloc ( 1,
		      sizeof ( struct donnees_compte ));

    /* enregistre le compte */

    p_tab_nom_de_compte = realloc ( p_tab_nom_de_compte,
				    ( nb_comptes + 1 )* sizeof ( gpointer ) );
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + nb_comptes;
    nb_comptes++;
    *p_tab_nom_de_compte_variable = (gpointer) compte;

    p_tab_nom_de_compte_courant = p_tab_nom_de_compte + compte_courant; 


    /*     met l'id du compte s'il existe (import ofx) */

    if ( compte_import -> id_compte )
    {
	gchar **tab_str;

	compte -> id_compte = g_strdup ( compte_import -> id_compte );

	/* 	en théorie cet id est "no_banque no_guichet no_comptecle" */
	/* on va essayer d'importer ces données ici */
	/* si on rencontre un null, on s'arrête */

	tab_str = g_strsplit ( compte -> id_compte,
			       " ",
			       3 );
	if ( tab_str[1] )
	{
	    compte -> no_guichet = g_strdup ( tab_str[1] );
	    if ( tab_str[2] )
	    {
		gchar *temp;

		compte -> cle_compte = g_strdup ( tab_str[2] + strlen ( tab_str[2] ) - 1 );

		temp = g_strdup ( tab_str[2] );

		temp[strlen (temp) - 1 ] = 0;
		compte -> no_compte_banque = temp;
	    }
	}
	g_strfreev ( tab_str );
    }

    /* met le nom du compte */

    if ( compte_import -> nom_de_compte )
	compte -> nom_de_compte = g_strstrip ( compte_import -> nom_de_compte );
    else
	compte -> nom_de_compte = g_strdup ( _("Imported account"));

    /* choix de la devise du compte */

    compte -> devise = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( compte_import -> bouton_devise ) -> menu_item ),
							       "no_devise" ));

    /* met le type de compte */

    switch ( compte_import -> type_de_compte )
    {
	case 3:
	    compte -> type_de_compte = 2;
	    break;

	case 7:
	    compte -> type_de_compte = 1;
	    break;

	default:
	    compte -> type_de_compte = 0;
	    break;
    }

    /* met le solde init */

    compte -> solde_initial = compte_import -> solde;

    /* met le no de compte */

    compte -> no_compte = nb_comptes - 1;

    /* on l'ajoute à l'ordre des comptes */

    ordre_comptes = g_slist_append ( ordre_comptes,
				     GINT_TO_POINTER ( nb_comptes - 1 ));

    /* on crée les types d'opé par défaut */

    creation_types_par_defaut ( nb_comptes - 1,
				0);

    /* on fait maintenant le tour des opés de ce compte */

    /* la variable derniere_operation est utilisée pour garder le numéro de l'opé */
    /* précédente pour les ventilations */

    derniere_operation = 0;

    liste_tmp = compte_import -> operations_importees;
    solde_courant = compte -> solde_initial;

    while ( liste_tmp )
    {
	struct struct_ope_importation *operation_import;
	gchar **tab_str;
	GSList *pointeur_tmp;

	operation_import = liste_tmp -> data;

	operation = calloc ( 1,
			     sizeof ( struct structure_operation ));

	/* récupération du no de l'opé */

	operation -> no_operation = ++no_derniere_operation;

	/* 	récupération de l'id si elle existe */

	if ( operation_import -> id_operation )
	    operation -> id_operation = g_strdup ( operation_import -> id_operation );

	/* récupération de la date */

	operation -> jour = g_date_day ( operation_import -> date );
	operation -> mois = g_date_month ( operation_import -> date );
	operation -> annee = g_date_year ( operation_import -> date );

	operation -> date = g_date_new_dmy ( operation -> jour,
					     operation -> mois,
					     operation -> annee );

	/* récupération de la date de valeur */

	if ( operation_import -> date_de_valeur )
	{
	    operation -> jour_bancaire = g_date_day ( operation_import -> date_de_valeur );
	    operation -> mois_bancaire = g_date_month ( operation_import -> date_de_valeur );
	    operation -> annee_bancaire = g_date_year ( operation_import -> date_de_valeur );

	    operation -> date_bancaire = g_date_new_dmy ( operation -> jour_bancaire,
							  operation -> mois_bancaire,
							  operation -> annee_bancaire );
	}

	/* récupération du no de compte */

	operation -> no_compte = compte -> no_compte;


	/* récupération du montant */

	operation -> montant = operation_import -> montant;
	solde_courant = solde_courant + operation_import -> montant;

	/* 	  récupération de la devise, sur la popup affichée */

	operation -> devise = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( compte_import -> bouton_devise ) -> menu_item ),
								      "no_devise" ));

	/* récupération du tiers */

	if ( operation_import -> tiers
	     &&
	     strlen ( g_strstrip ( operation_import -> tiers )))
	{
	    pointeur_tmp = g_slist_find_custom ( liste_struct_tiers,
						 g_strstrip ( operation_import -> tiers ),
						 (GCompareFunc) recherche_tiers_par_nom );

	    /* si le tiers n'existait pas, on le crée */

	    if ( pointeur_tmp )
		operation -> tiers = ((struct struct_tiers *)(pointeur_tmp -> data)) -> no_tiers;
	    else
		operation -> tiers = (( struct struct_tiers * )( ajoute_nouveau_tiers ( operation_import -> tiers ))) -> no_tiers;

	}


	/* vérification si c'est ventilée sinon récupération des catégories */


	if ( operation_import -> operation_ventilee )
	{
	    /* l'opération est ventilée */

	    operation -> operation_ventilee = 1;
	}
	else
	{
	    /* vérification que ce n'est pas un virement */

	    if ( operation_import -> categ
		 &&
		 strlen ( g_strstrip (operation_import -> categ)) )
	    {
		if ( operation_import -> categ[0] == '[' )
		{
		    /* 		      c'est un virement, or le compte n'a peut être pas encore été créé, */
		    /* on va mettre le nom du compte dans info_banque_guichet qui n'est jamais utilisé */
		    /* lors d'import, et relation_no_compte sera mis à -2 (-1 est déjà utilisé pour les comptes supprimés */

		    operation -> info_banque_guichet = operation_import -> categ;
		    operation -> relation_no_compte = -2;
		    operation -> relation_no_operation = -1;
		    virements_a_chercher = 1;
		}
		else
		{
		    struct struct_categ *categ;
		    struct struct_sous_categ *sous_categ ;


		    tab_str = g_strsplit ( operation_import -> categ,
					   ":",
					   2 );


		    /* récupération ou création de la catégorie */

		    if ( !g_slist_find_custom ( liste_struct_categories,
						g_strstrip ( tab_str[0] ),
						(GCompareFunc) recherche_categorie_par_nom ))
		    {
			categ = calloc ( 1,
					 sizeof ( struct struct_categ ));

			categ -> no_categ = ++no_derniere_categorie;
			categ -> nom_categ = g_strdup ( g_strstrip ( tab_str[0] ) );

			if ( operation_import -> montant < 0 )
			    categ -> type_categ = 1;
			else
			    categ -> type_categ = 0;

			nb_enregistrements_categories++;

			liste_struct_categories = g_slist_append ( liste_struct_categories,
								   categ );
		    }
		    else
			categ = g_slist_find_custom ( liste_struct_categories,
						      g_strstrip ( tab_str[0] ),
						      (GCompareFunc) recherche_categorie_par_nom ) -> data;


		    operation -> categorie = categ -> no_categ;


		    /* récupération ou création de la sous-catégorie */

		    if ( tab_str[1] )
		    {
			if ( !g_slist_find_custom ( categ -> liste_sous_categ,
						    g_strstrip ( tab_str[1] ),
						    (GCompareFunc) recherche_sous_categorie_par_nom ))
			{
			    sous_categ = calloc ( 1,
						  sizeof ( struct struct_sous_categ ));

			    sous_categ -> no_sous_categ = ++( categ -> no_derniere_sous_categ );
			    sous_categ -> nom_sous_categ = g_strdup ( g_strstrip ( tab_str[1] ));

			    categ -> liste_sous_categ = g_slist_append ( categ -> liste_sous_categ,
									 sous_categ );
			}
			else
			    sous_categ = g_slist_find_custom ( categ -> liste_sous_categ,
							       g_strstrip ( tab_str[1] ),
							       (GCompareFunc) recherche_sous_categorie_par_nom ) -> data;

			operation -> sous_categorie = sous_categ -> no_sous_categ;
		    }
		    g_strfreev ( tab_str );
		}
	    }
	}

	/* récupération des notes */

	operation -> notes = g_strdup ( operation_import -> notes );


	p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

	/* récupération du chèque et mise en forme du type d'opération */

	if ( operation_import -> cheque )
	{
	    /* c'est un chèque, on va rechercher un type à incrémentation automatique et mettre l'opé sous ce type */
	    /* si un tel type n'existe pas, on met quand même le no dans contenu_type et on met le type par défaut */

	    struct struct_type_ope *type_choisi;
	    GSList *liste_tmp;

	    if ( operation -> montant < 0 )
		operation -> type_ope = TYPE_DEFAUT_DEBIT;
	    else
		operation -> type_ope = TYPE_DEFAUT_CREDIT;

	    operation -> contenu_type = utils_itoa ( operation_import -> cheque );

	    liste_tmp = TYPES_OPES;
	    type_choisi = NULL;

	    while ( liste_tmp )
	    {
		struct struct_type_ope *type;

		type = liste_tmp -> data;

		/* si l'opé on trouve un type à incrémentation automatique et que le signe du type est bon, on l'enregistre */
		/*   et on arrête la recherche, sinon, on l'enregistre mais on continue la recherche dans l'espoir de trouver */
		/* mieux */

		if ( type -> numerotation_auto )
		{
		    if ( !type -> signe_type
			 ||
			 ( type -> signe_type == 1 && operation -> montant < 0 )
			 ||
			 ( type -> signe_type == 2 && operation -> montant > 0 ))
		    {
			operation -> type_ope = type -> no_type;
			type_choisi = type;
			liste_tmp = NULL;
		    }
		    else
		    {
			operation -> type_ope = type -> no_type;
			type_choisi = type;
			liste_tmp = liste_tmp -> next;
		    }
		}
		else
		    liste_tmp = liste_tmp -> next;
	    }

	    /* type_choisi contient l'adr du type qui a été utilisée on peut y mettre le dernier no de chèque */

	    if ( type_choisi )
		type_choisi -> no_en_cours = MAX ( operation_import -> cheque,
						   type_choisi -> no_en_cours );
	}
	else
	{
	    /* comme ce n'est pas un chèque, on met sur le type par défaut */

	    if ( operation -> montant < 0 )
		operation -> type_ope = TYPE_DEFAUT_DEBIT;
	    else
		operation -> type_ope = TYPE_DEFAUT_CREDIT;

	}


	/* récupération du pointé */

	operation -> pointe = operation_import -> p_r;

	if ( operation -> pointe )
	    compte -> solde_pointe = compte -> solde_pointe + operation -> montant;


	/* si c'est une ope de ventilation, lui ajoute le no de l'opération précédente */

	if ( operation_import -> ope_de_ventilation )
	    operation -> no_operation_ventilee_associee = derniere_operation;
	else
	    derniere_operation = operation -> no_operation;


	/* ajoute l'opération dans la liste des opés du compte */

	compte -> gsliste_operations = g_slist_append ( compte -> gsliste_operations,
							operation );
	compte -> nb_operations++;


	liste_tmp = liste_tmp -> next;
    }


    /* on classe la liste */

    compte -> gsliste_operations = g_slist_sort ( compte -> gsliste_operations,
						  (GCompareFunc) classement_sliste );

    compte -> nb_lignes_ope = 3;
    compte -> solde_courant = solde_courant;
    compte -> date_releve = NULL;
    compte -> operation_selectionnee = GINT_TO_POINTER ( -1 );


}
/* *******************************************************************************/





/* *******************************************************************************/
void ajout_opes_importees ( struct struct_compte_importation *compte_import )
{
    GSList *liste_tmp;
    GDate *derniere_date;
    gint demande_confirmation;

    /* on se place sur le compte dans lequel on va importer les opés */

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( compte_import -> bouton_compte )->menu_item ),
												 "no_compte" ));

    /* si le compte importé a une id, on la vérifie ici */
    /*     si elle est absente, on met celle importée */
    /*     si elle est différente, on demande si on la remplace */

    if ( compte_import -> id_compte )
    {
	if ( ID_COMPTE )
	{
	    if ( g_strcasecmp ( ID_COMPTE,
				compte_import -> id_compte ))
	    {
		/* 		l'id du compte choisi et l'id du compte importé sont différents */
		/* 		    on propose encore d'arrêter... */


		if ( question_yes_no_hint ( _("The id of the imported and chosen accounts are different"),
					    _("Perhaps you choose a wrong account ?  If you choose to continue, the id of the account will be changed.  Do you want to continue ?")))
		    ID_COMPTE = g_strdup ( compte_import -> id_compte );
		else
		    return;
	    }
	}
	else
	    ID_COMPTE = g_strdup ( compte_import -> id_compte );

    }


    /* on fait un premier tour de la liste des opés pour repérer celles qui sont déjà entrées */
    /*   si on n'importe que du ofx, c'est facile, chaque opé est repérée par une id */
    /*     donc si l'opé importée a une id, il suffit de rechercher l'id dans le compte, si elle */
    /*     n'y est pas l'opé est à enregistrer */
    /*     si on importe du qif ou du html, il n'y a pas d'id. donc soit on retrouve une opé semblable */
    /*     (cad même montant et même date, on ne fait pas joujou avec le tiers car l'utilisateur */
    /* a pu le changer), et on demande à l'utilisateur quoi faire, sinon on enregistre l'opé */


    /*   pour gagner en rapidité, on va récupérer la dernière date du compte, toutes les opés importées */
    /* qui ont une date supèrieure sont automatiquement acceptées */


    liste_tmp = LISTE_OPERATIONS;
    derniere_date = NULL;

    while ( liste_tmp )
    {
	struct structure_operation *operation;

	operation = liste_tmp -> data;

	if ( !derniere_date
	     ||
	     g_date_compare ( operation -> date,
			      derniere_date ) > 0 )
	    derniere_date = operation -> date;

	liste_tmp = liste_tmp -> next;
    }


    liste_tmp = compte_import -> operations_importees;
    demande_confirmation = 0;

    while ( liste_tmp )
    {
	struct struct_ope_importation *operation_import;
	GSList *liste_ope;
	operation_import = liste_tmp -> data;

	/* on ne fait le tour de la liste des opés que si la date de l'opé importée est inférieure à la dernière date */
	/* de la liste */

	if ( g_date_compare ( derniere_date,
			      operation_import -> date ) >= 0 )
	{
	    /* 	    si l'opé d'import a une id, on recherche ça en priorité */

	    if ( operation_import -> id_operation )
	    {
		GSList *liste;

		liste = g_slist_find_custom ( LISTE_OPERATIONS,
					      operation_import -> id_operation,
					      (GCompareFunc) recherche_operation_par_id );

		if (liste)
		{
		    /* 			comme on est sr que cette opé a déjà été enregistree, on met l'action à 2, cad on demande l'avis de personne pour */
		    /* 			    pas l'enregistrer */

		    operation_import -> action = 2;
		}

	    }

	    /* 	    si l'opé d'import a un no de chq, on le recherche */

	    if ( operation_import -> action != 2
		 &&
		 operation_import -> cheque )
	    {
		GSList *liste;

		liste = g_slist_find_custom ( LISTE_OPERATIONS,
					      GINT_TO_POINTER ( operation_import -> cheque ),
					      (GCompareFunc) recherche_operation_par_cheque );

		if (liste)
		{
		    /* 	comme on est sr que cette opé a déjà été enregistree, on met l'action à 2, cad on demande l'avis de personne pour */
		    /*  pas l'enregistrer */

		    operation_import -> action = 2;
		}
	    }

	    /* on fait donc le tour de la liste des opés pour retrouver une opé comparable */
	    /* si elle n'a pas déjà été retrouvée par id... */

	    if ( operation_import -> action != 2 )
	    {
		GDate *date_debut_comparaison;
		GDate *date_fin_comparaison;

		date_debut_comparaison = g_date_new_dmy ( g_date_get_day ( operation_import -> date ),
							  g_date_get_month ( operation_import -> date ),
							  g_date_get_year ( operation_import -> date ));
		g_date_subtract_days ( date_debut_comparaison,
				       valeur_echelle_recherche_date_import );

		date_fin_comparaison = g_date_new_dmy ( g_date_get_day ( operation_import -> date ),
							g_date_get_month ( operation_import -> date ),
							g_date_get_year ( operation_import -> date ));
		g_date_add_days ( date_fin_comparaison,
				  valeur_echelle_recherche_date_import );

		liste_ope = LISTE_OPERATIONS;

		while ( liste_ope )
		{
		    struct structure_operation *operation;

		    operation = liste_ope -> data;

		    if ( fabs ( operation -> montant - operation_import -> montant ) < 0.01
			 &&
			 ( g_date_compare ( operation -> date,
					    date_debut_comparaison ) >= 0 )
			 &&
			 ( g_date_compare ( operation -> date,
					    date_fin_comparaison ) <= 0 )

			 &&
			 !operation_import -> ope_de_ventilation )
		    {
			/* l'opé a la même date et le même montant, on la marque pour demander quoi faire à l'utilisateur */
			operation_import -> action = 1; 
			operation_import -> ope_correspondante = operation;
			demande_confirmation = 1;
		    }
		    liste_ope = liste_ope -> next;
		}
	    }
	}
	liste_tmp = liste_tmp -> next;
    }

    /*   à ce niveau, toutes les opés douteuses ont été marquées, on appelle la fonction qui */
    /* se charge de demander à l'utilisateur que faire */

    if ( demande_confirmation )
	confirmation_enregistrement_ope_import ( compte_import );


    /* on fait le tour des opés de ce compte et enregistre les opés */

    /* la variable derniere_operation est utilisée pour garder le numéro de l'opé */
    /* précédente pour les ventilations */

    derniere_operation_enregistrement_ope_import = 0;

    liste_tmp = compte_import -> operations_importees;

    while ( liste_tmp )
    {
	struct struct_ope_importation *operation_import;

	operation_import = liste_tmp -> data;

	/* vérifie qu'on doit bien l'enregistrer */

	if ( !operation_import -> action )
	{
	    /* on récupère à ce niveau la devise choisie dans la liste */

	    operation_import -> devise = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( compte_import -> bouton_devise ) -> menu_item ),
										 "no_devise" ));
	    enregistre_ope_importee ( operation_import,
				      NO_COMPTE );
	} 
	liste_tmp = liste_tmp -> next;
    }


    mise_a_jour_solde ( NO_COMPTE );

    /* on classe la liste */

    LISTE_OPERATIONS = g_slist_sort ( LISTE_OPERATIONS,
				      (GCompareFunc) classement_sliste );
}
/* *******************************************************************************/


/* *******************************************************************************/
void confirmation_enregistrement_ope_import ( struct struct_compte_importation *compte_import )
{
    /*   cette fonction fait le tour des opérations importées, et demande que faire pour celles */
    /* qui sont douteuses lors d'un ajout des opés à un compte existant */

    GSList *liste_tmp;
    GtkWidget *dialog;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *scrolled_window;
    GtkWidget *label;
    gint action_derniere_ventilation;


    dialog = gtk_dialog_new_with_buttons ( _("Confirmation of importation of transactions"),
					   GTK_WINDOW ( window),
					   GTK_DIALOG_MODAL,
					   GTK_STOCK_OK,
					   GTK_RESPONSE_OK,
					   NULL );
    gtk_widget_set_usize ( dialog,
			   FALSE,
			   300 );

    label = gtk_label_new ( _("Some imported transactions seem to be already saved. Please select the transactions to import." ));
    gtk_box_pack_start ( GTK_BOX ( GTK_DIALOG ( dialog )-> vbox ),
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
    gtk_box_pack_start ( GTK_BOX ( GTK_DIALOG ( dialog )-> vbox ),
			 scrolled_window,
			 TRUE,
			 TRUE,
			 0 );
    gtk_widget_show ( scrolled_window );

    vbox = gtk_vbox_new ( FALSE,
			  5 );
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW ( scrolled_window ),
					    vbox );
    gtk_container_set_border_width ( GTK_CONTAINER ( vbox ),
				     10 );
    gtk_widget_show ( vbox );

    /*   on fait maintenant le tour des opés importées et affichent celles à problème */

    liste_tmp = compte_import -> operations_importees;

    while ( liste_tmp )
    {
	struct struct_ope_importation *ope_import;

	ope_import = liste_tmp -> data;

	/* on n'affiche pas si c'est des opés de ventil, si la même est cochée, les filles seront alors cochées */
	/* on ne teste pas ici car ça a été testé avant */

	if ( ope_import -> action == 1 
	     &&
	     !ope_import -> ope_de_ventilation )
	{
	    struct structure_operation *operation;
	    gchar *tiers;

	    operation = ope_import -> ope_correspondante;

	    hbox = gtk_hbox_new ( FALSE,
				  5 );
	    gtk_box_pack_start ( GTK_BOX ( vbox ),
				 hbox,
				 FALSE,
				 FALSE,
				 0 );
	    gtk_widget_show ( hbox );


	    ope_import -> bouton = gtk_check_button_new ();
	    gtk_box_pack_start ( GTK_BOX ( hbox ),
				 ope_import -> bouton,
				 FALSE,
				 FALSE,
				 0 );
	    gtk_widget_show ( ope_import -> bouton );

	    label = gtk_label_new ( g_strdup_printf ( _("Transactions to import : %02d/%02d/%04d ; %s ; %4.2f"),
						      g_date_day ( ope_import -> date ),
						      g_date_month ( ope_import -> date ),
						      g_date_year ( ope_import -> date ),
						      ope_import -> tiers,
						      ope_import -> montant ));
	    gtk_box_pack_start ( GTK_BOX ( hbox ),
				 label,
				 FALSE,
				 FALSE,
				 0 );
	    gtk_widget_show ( label );


	    hbox = gtk_hbox_new ( FALSE,
				  5 );
	    gtk_box_pack_start ( GTK_BOX ( vbox ),
				 hbox,
				 FALSE,
				 FALSE,
				 0 );
	    gtk_widget_show ( hbox );


	    label = gtk_label_new ( "       " );
	    gtk_box_pack_start ( GTK_BOX ( hbox ),
				 label,
				 FALSE,
				 FALSE,
				 0 );
	    gtk_widget_show ( label );

	    if ( operation -> tiers )
		tiers = ((struct struct_tiers *)(g_slist_find_custom ( liste_struct_tiers,
								       GINT_TO_POINTER ( operation -> tiers ),
								       (GCompareFunc) recherche_tiers_par_no )->data))->nom_tiers;
	    else
		tiers = _("no third party");

	    if ( operation -> notes )
		label = gtk_label_new ( g_strdup_printf ( _("Transaction found : %02d/%02d/%04d ; %s ; %4.2f ; %s"),
							  g_date_day ( operation -> date ),
							  g_date_month ( operation -> date ),
							  g_date_year ( operation -> date ),
							  tiers,
							  operation -> montant,
							  operation -> notes ));
	    else
		label = gtk_label_new ( g_strdup_printf ( _("Transaction found : %02d/%02d/%04d ; %s ; %4.2f"),
							  g_date_day ( operation -> date ),
							  g_date_month ( operation -> date ),
							  g_date_year ( operation -> date ),
							  tiers,
							  operation -> montant ));

	    gtk_box_pack_start ( GTK_BOX ( hbox ),
				 label,
				 FALSE,
				 FALSE,
				 0 );
	    gtk_widget_show ( label );
	}
	liste_tmp = liste_tmp -> next;
    }

    gtk_dialog_run ( GTK_DIALOG ( dialog ));

    /* on fait maintenant le tour des check buttons pour voir ce qu'on importe */

    liste_tmp = compte_import -> operations_importees;
    action_derniere_ventilation = 1;

    while ( liste_tmp )
    {
	struct struct_ope_importation *ope_import;

	ope_import = liste_tmp -> data;

	/* si c'est une opé de ventil, elle n'était pas affichée, dans ce cas si l'action de la
	   dernière ventil était 0, on fait de même pour les filles */

	if ( ope_import -> ope_de_ventilation )
	{
	    if ( ope_import -> action )
		ope_import -> action = action_derniere_ventilation;
	}
	else
	    action_derniere_ventilation = 1;



	if ( ope_import -> bouton
	     &&
	     gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( ope_import -> bouton )))
	{
	    ope_import -> action = 0;

	    /* si c'était une ventil on met l'action de la dernière ventil à 0 */

	    if ( ope_import -> operation_ventilee )
		action_derniere_ventilation = 0;	
	}

	liste_tmp = liste_tmp -> next;
    }

    gtk_widget_destroy ( dialog );
}
/* *******************************************************************************/


/* *******************************************************************************/
struct structure_operation *enregistre_ope_importee ( struct struct_ope_importation *operation_import,
						      gint no_compte )
{
    struct structure_operation *operation;
    gchar **tab_str;
    GSList *pointeur_tmp;


    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;

    operation = calloc ( 1,
			 sizeof ( struct structure_operation ));

    /* récupération du no de l'opé */

    operation -> no_operation = ++no_derniere_operation;

    /* récupération de l'id de l'opé s'il existe */

    if ( operation_import -> id_operation )
	operation -> id_operation = g_strdup ( operation_import -> id_operation );

    /* récuération de la date */

    operation -> jour = g_date_day ( operation_import -> date );
    operation -> mois = g_date_month ( operation_import -> date );
    operation -> annee = g_date_year ( operation_import -> date );

    operation -> date = g_date_new_dmy ( operation -> jour,
					 operation -> mois,
					 operation -> annee );

    /* récupération de la date de valeur */

    if ( operation_import -> date_de_valeur )
    {
	operation -> jour_bancaire = g_date_day ( operation_import -> date_de_valeur );
	operation -> mois_bancaire = g_date_month ( operation_import -> date_de_valeur );
	operation -> annee_bancaire = g_date_year ( operation_import -> date_de_valeur );

	operation -> date_bancaire = g_date_new_dmy ( operation -> jour,
						      operation -> mois,
						      operation -> annee );
    }

    /* récupération du no de compte */

    operation -> no_compte = NO_COMPTE;


    /* récupération du montant */

    operation -> montant = operation_import -> montant;

    /* 	  récupération de la devise, sur la popup affichée */

    operation -> devise = operation_import -> devise;

    /* récupération du tiers */

    if ( operation_import -> tiers )
    {
	pointeur_tmp = g_slist_find_custom ( liste_struct_tiers,
					     g_strstrip ( operation_import -> tiers ),
					     (GCompareFunc) recherche_tiers_par_nom );

	/* si le tiers n'existait pas, on le crée */

	if ( pointeur_tmp )
	    operation -> tiers = ((struct struct_tiers *)(pointeur_tmp -> data)) -> no_tiers;
	else
	    operation -> tiers = (( struct struct_tiers * )( ajoute_nouveau_tiers ( operation_import -> tiers ))) -> no_tiers;

    }


    /* vérification si c'est ventilé, sinon récupération des catégories */


    if ( operation_import -> operation_ventilee )
    {
	/* l'opération est ventilée */

	operation -> operation_ventilee = 1;
    }
    else
    {
	/* vérification que ce n'est pas un virement */

	if ( operation_import -> categ
	     &&
	     strlen ( g_strstrip (operation_import -> categ)) )
	{
	    if ( operation_import -> categ[0] == '[' )
	    {
		/* 		      c'est un virement, or le compte n'a peut être pas encore été créé, */
		/* on va mettre le nom du compte dans info_banque_guichet qui n'est jamais utilisé */
		/* lors d'import, et relation_no_compte sera mis à -2 (-1 est déjà utilisé pour les comptes supprimés */

		operation -> info_banque_guichet = operation_import -> categ;
		operation -> relation_no_compte = -2;
		operation -> relation_no_operation = -1;
		virements_a_chercher = 1;
	    }
	    else
	    {
		struct struct_categ *categ;
		struct struct_sous_categ *sous_categ ;


		tab_str = g_strsplit ( operation_import -> categ,
				       ":",
				       2 );


		/* récupération ou création de la catégorie */

		if ( !g_slist_find_custom ( liste_struct_categories,
					    g_strstrip ( tab_str[0] ),
					    (GCompareFunc) recherche_categorie_par_nom ))
		{
		    categ = calloc ( 1,
				     sizeof ( struct struct_categ ));

		    categ -> no_categ = ++no_derniere_categorie;
		    categ -> nom_categ = g_strdup ( g_strstrip ( tab_str[0] ) );

		    if ( operation_import -> montant < 0 )
			categ -> type_categ = 1;
		    else
			categ -> type_categ = 0;

		    nb_enregistrements_categories++;

		    liste_struct_categories = g_slist_append ( liste_struct_categories,
							       categ );
		}
		else
		    categ = g_slist_find_custom ( liste_struct_categories,
						  g_strstrip ( tab_str[0] ),
						  (GCompareFunc) recherche_categorie_par_nom ) -> data;


		operation -> categorie = categ -> no_categ;


		/* récupération ou création de la sous-catégorie */

		if ( tab_str[1] )
		{
		    if ( !g_slist_find_custom ( categ -> liste_sous_categ,
						g_strstrip ( tab_str[1] ),
						(GCompareFunc) recherche_sous_categorie_par_nom ))
		    {
			sous_categ = calloc ( 1,
					      sizeof ( struct struct_sous_categ ));

			sous_categ -> no_sous_categ = ++( categ -> no_derniere_sous_categ );
			sous_categ -> nom_sous_categ = g_strdup ( g_strstrip ( tab_str[1] ));

			categ -> liste_sous_categ = g_slist_append ( categ -> liste_sous_categ,
								     sous_categ );
		    }
		    else
			sous_categ = g_slist_find_custom ( categ -> liste_sous_categ,
							   g_strstrip ( tab_str[1] ),
							   (GCompareFunc) recherche_sous_categorie_par_nom ) -> data;

		    operation -> sous_categorie = sous_categ -> no_sous_categ;
		}
		g_strfreev ( tab_str );
	    }
	}
    }

    /* récupération des notes */

    operation -> notes = operation_import -> notes;


    /* récupération du chèque et mise en forme du type d'opération */

    if ( operation_import -> cheque )
    {
	/* c'est un chèque, on va rechercher un type à incrémentation automatique et mettre l'opé sous ce type */
	/* si un tel type n'existe pas, on met quand même le no dans contenu_type et on met le type par défaut */

	struct struct_type_ope *type_choisi;
	GSList *liste_tmp;

	if ( operation -> montant < 0 )
	    operation -> type_ope = TYPE_DEFAUT_DEBIT;
	else
	    operation -> type_ope = TYPE_DEFAUT_CREDIT;

	operation -> contenu_type = utils_itoa ( operation_import -> cheque );

	liste_tmp = TYPES_OPES;
	type_choisi = NULL;

	while ( liste_tmp )
	{
	    struct struct_type_ope *type;

	    type = liste_tmp -> data;

	    /* si l'opé on trouve un type à incrémentation automatique et que le signe du type est bon, on l'enregistre */
	    /*   et on arrête la recherche, sinon, on l'enregistre mais on continue la recherche dans l'espoir de trouver */
	    /* mieux */

	    if ( type -> numerotation_auto )
	    {
		if ( !type -> signe_type
		     ||
		     ( type -> signe_type == 1 && operation -> montant < 0 )
		     ||
		     ( type -> signe_type == 2 && operation -> montant > 0 ))
		{
		    operation -> type_ope = type -> no_type;
		    type_choisi = type;
		    liste_tmp = NULL;
		}
		else
		{
		    operation -> type_ope = type -> no_type;
		    type_choisi = type;
		    liste_tmp = liste_tmp -> next;
		}
	    }
	    else
		liste_tmp = liste_tmp -> next;
	}

	/* type_choisi contient l'adr du type qui a été utilisé, on peut y mettre le dernier no de chèque */

	if ( type_choisi )
	    type_choisi -> no_en_cours = MAX ( operation_import -> cheque,
					       type_choisi -> no_en_cours );
    }
    else
    {
	/* comme ce n'est pas un chèque, on met sur le type par défaut */

	if ( operation -> montant < 0 )
	    operation -> type_ope = TYPE_DEFAUT_DEBIT;
	else
	    operation -> type_ope = TYPE_DEFAUT_CREDIT;

    }


    /* récupération du pointé */

    operation -> pointe = operation_import -> p_r;

    /* si c'est une ope de ventilation, lui ajoute le no de l'opération précédente */

    if ( operation_import -> ope_de_ventilation )
	operation -> no_operation_ventilee_associee = derniere_operation_enregistrement_ope_import ;
    else
	derniere_operation_enregistrement_ope_import  = operation -> no_operation;


    /* ajoute l'opération dans la liste des opés du compte */

    LISTE_OPERATIONS = g_slist_append ( LISTE_OPERATIONS,
					operation );
    NB_OPE_COMPTE++;

    return ( operation );
}
/* *******************************************************************************/



/* *******************************************************************************/
void pointe_opes_importees ( struct struct_compte_importation *compte_import )
{
    GSList *liste_tmp;
    GSList *liste_opes_import_celibataires;
    gint no_compte;


    /* on se place sur le compte dans lequel on va pointer les opés */

    no_compte = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( compte_import -> bouton_compte )->menu_item ),
							"no_compte" ));
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;


    /* si le compte importé a une id, on la vérifie ici */
    /*     si elle est absente, on met celle importée */
    /*     si elle est différente, on demande si on la remplace */

    if ( compte_import -> id_compte )
    {
	if ( ID_COMPTE )
	{
	    if ( g_strcasecmp ( ID_COMPTE,
				compte_import -> id_compte ))
	    {
		/* 		l'id du compte choisi et l'id du compte importé sont différents */
		/* 		    on propose encore d'arrêter... */


		if ( question_yes_no_hint ( _("The id of the imported and chosen accounts are different"),
					    _("Perhaps you choose a wrong account ?  If you choose to continue, the id of the account will be changed.  Do you want to continue ?")))
		    ID_COMPTE = g_strdup ( compte_import -> id_compte );
		else
		    return;
	    }
	}
	else
	    ID_COMPTE = g_strdup ( compte_import -> id_compte );

    }

    /* on fait le tour des opés importées et recherche dans la liste d'opé s'il y a la correspondance */


    liste_tmp = compte_import -> operations_importees;
    liste_opes_import_celibataires = NULL;

    while ( liste_tmp )
    {
	GSList *liste_ope;
	GSList *ope_trouvees;
	struct struct_ope_importation *ope_import;
	struct structure_operation *operation;
	gint i;
	struct struct_ope_importation *autre_ope_import;

	ope_import = liste_tmp -> data;
	ope_trouvees = NULL;
	operation = NULL;

	/* si l'opé d'import a une id, on recherche dans la liste d'opé pour trouver
	   une id comparable */

	if ( ope_import -> id_operation )
	{
	    GSList *liste;

	    liste = g_slist_find_custom ( LISTE_OPERATIONS,
					  ope_import -> id_operation,
					  (GCompareFunc) recherche_operation_par_id );
	    if ( liste )
		ope_trouvees = g_slist_append ( ope_trouvees,
						liste -> data );
	}

	/* si on n'a rien trouvé par id, */
	/* on fait le tour de la liste d'opés pour trouver des opés comparable */
	/* cad même date avec + ou - une échelle et même montant et pas une opé de ventil */


	if ( !ope_trouvees )
	{
	    GDate *date_debut_comparaison;
	    GDate *date_fin_comparaison;

	    date_debut_comparaison = g_date_new_dmy ( g_date_get_day ( ope_import -> date ),
						      g_date_get_month ( ope_import -> date ),
						      g_date_get_year ( ope_import -> date ));
	    g_date_subtract_days ( date_debut_comparaison,
				   valeur_echelle_recherche_date_import );

	    date_fin_comparaison = g_date_new_dmy ( g_date_get_day ( ope_import -> date ),
						    g_date_get_month ( ope_import -> date ),
						    g_date_get_year ( ope_import -> date ));
	    g_date_add_days ( date_fin_comparaison,
			      valeur_echelle_recherche_date_import );



	    liste_ope = LISTE_OPERATIONS;

	    while ( liste_ope )
	    {
		operation = liste_ope -> data;

		if ( fabs ( operation -> montant - ope_import -> montant ) < 0.01
		     &&
		     ( g_date_compare ( operation -> date,
					date_debut_comparaison ) >= 0 )
		     &&
		     ( g_date_compare ( operation -> date,
					date_fin_comparaison ) <= 0 )

		     &&
		     !operation -> no_operation_ventilee_associee )
		    /* on a retouvé une opé de même date et même montant, on l'ajoute à la liste des opés trouvées */
		    ope_trouvees = g_slist_append ( ope_trouvees,
						    operation );

		liste_ope = liste_ope -> next;
	    }
	}
	/*       à ce stade, ope_trouvees contient la ou les opés qui sont comparables à l'opé importée */
	/* soit il n'y en n'a qu'une, et on la pointe, soit il y en a plusieurs, et on recherche dans */
	/* 	les opés importées s'il y en a d'autre comparables, et on pointe les opés en fonction */
	/* du nb de celles importées */

	switch ( g_slist_length ( ope_trouvees ))
	{
	    case 0:
		/* aucune opé comparable n'a été retrouvée */
		/* on marque donc cette opé comme seule */
		/* sauf si c'est une opé de ventil  */

		if ( !ope_import -> ope_de_ventilation ) 
		{
		    /* on met le no de compte et la devise de l'opération si plus tard on l'enregistre */

		    ope_import -> no_compte = no_compte;
		    ope_import -> devise = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( compte_import -> bouton_devise ) -> menu_item ),
										   "no_devise" ));
		    liste_opes_import_celibataires = g_slist_append ( liste_opes_import_celibataires,
								      ope_import );
		}

		break;

	    case 1:
		/*  il n'y a qu'une opé retrouvée, on la pointe */
		/* si elle est déjà pointée ou relevée, on ne fait rien */
		/* si l'opé d'import a une id et pas l'opé, on marque l'id dans l'opé */

		operation = ope_trouvees -> data;

		if ( !operation -> id_operation
		     &&
		     ope_import -> id_operation )
		    operation -> id_operation = ope_import -> id_operation;

		if ( !operation -> pointe )
		{
		    operation -> pointe = TELECHECKED_TRANSACTION;

		    /* si c'est une opé ventilée , on recherche les opé filles pour leur mettre le même pointage que la même */

		    if ( operation -> operation_ventilee )
		    {

			liste_ope = LISTE_OPERATIONS;

			while ( liste_ope )
			{
			    struct structure_operation *ope_fille;

			    ope_fille = liste_ope -> data;

			    if ( ope_fille -> no_operation_ventilee_associee == operation -> no_operation )
				ope_fille -> pointe = TELECHECKED_TRANSACTION;

			    liste_ope = liste_ope -> next;
			}
		    }
		}
		break;

	    default: 	   
		/* il y a plusieurs opé trouvées correspondant à l'opé importée */

		/* on va voir s'il y a d'autres opés importées ayant la même date et le même montant
		   si on retrouve autant d'opé importées que d'opé trouvées, on peut marquer cette
		   opé sans s'en préoccuper */


		i=0;
		liste_ope = compte_import -> operations_importees;

		while ( liste_ope )
		{
		    GDate *date_debut_comparaison;
		    GDate *date_fin_comparaison;

		    autre_ope_import = liste_ope -> data;
		    date_debut_comparaison = g_date_new_dmy ( g_date_get_day ( autre_ope_import -> date ),
							      g_date_get_month ( autre_ope_import -> date ),
							      g_date_get_year ( autre_ope_import -> date ));
		    g_date_subtract_days ( date_debut_comparaison,
					   valeur_echelle_recherche_date_import );

		    date_fin_comparaison = g_date_new_dmy ( g_date_get_day ( autre_ope_import -> date ),
							    g_date_get_month ( autre_ope_import -> date ),
							    g_date_get_year ( autre_ope_import -> date ));
		    g_date_add_days ( date_fin_comparaison,
				      valeur_echelle_recherche_date_import );


		    if ( fabs ( autre_ope_import -> montant - ope_import -> montant ) < 0.01
			 &&
			 ( g_date_compare ( operation -> date,
					    date_debut_comparaison ) >= 0 )
			 &&
			 ( g_date_compare ( operation -> date,
					    date_fin_comparaison ) <= 0 )

			 &&
			 !autre_ope_import -> ope_de_ventilation )
			/* on a retouvé une opé d'import de même date et même montant, on incrémente le nb d'opé d'import semblables trouvees */
			i++;

		    liste_ope = liste_ope -> next;
		}

		if ( i ==  g_slist_length ( ope_trouvees ))
		{
		    /* on a trouvé autant d'opé d'import semblables
		     * que d'opés semblables dans la liste d'opés */
		    /* donc on peut marquer les opés trouvées */
		    /* pour celles qui sont déjà pointées, on ne fait rien */
		    /* si l'opé importée à une id, on met cette id dans l'opération si elle n'en a pas */

		    GSList *liste_tmp_2;

		    liste_tmp_2 = ope_trouvees;

		    while ( liste_tmp_2 )
		    {
			operation = liste_tmp_2 -> data;

			if ( !operation -> id_operation
			     &&
			     ope_import -> id_operation )
			    operation -> id_operation = ope_import -> id_operation;

			if ( !operation -> pointe )
			{
			    operation -> pointe = TELECHECKED_TRANSACTION;

			    /* si c'est une opé ventilée, on recherche les opé filles pour leur mettre le même pointage que la même */

			    if ( operation -> operation_ventilee )
			    {
				liste_ope = LISTE_OPERATIONS;

				while ( liste_ope )
				{
				    struct structure_operation *ope_fille;

				    ope_fille = liste_ope -> data;

				    if ( ope_fille -> no_operation_ventilee_associee == operation -> no_operation )
					ope_fille -> pointe = TELECHECKED_TRANSACTION;

				    liste_ope = liste_ope -> next;
				}
			    }
			}
			liste_tmp_2 = liste_tmp_2 -> next;
		    }
		}
		else
		{
		    /* on a trouvé un nombre différent d'opés d'import et d'opés semblables dans la liste d'opés
		       on marque donc cette opé d'import comme seule */

		    ope_import -> no_compte = no_compte;
		    ope_import -> devise = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( compte_import -> bouton_devise ) -> menu_item ),
										   "no_devise" ));
		    liste_opes_import_celibataires = g_slist_append ( liste_opes_import_celibataires,
								      ope_import );

		}

	}


	liste_tmp = liste_tmp -> next;
    }

    /* a ce niveau, liste_opes_import_celibataires contient les opés d'import dont on n'a pas retrouvé
           l'opé correspondante
       on les affiche dans une liste en proposant de les ajouter à la liste */

    if ( liste_opes_import_celibataires )
    {
	GtkWidget *liste_ope_celibataires, *dialog, *label, *scrolled_window;
	GtkListStore *store;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;



	dialog = gtk_dialog_new_with_buttons ( _("Orphaned transactions"),
					       GTK_WINDOW ( window ),
					       GTK_DIALOG_DESTROY_WITH_PARENT,
					       GTK_STOCK_APPLY, 1,
					       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					       GTK_STOCK_OK, GTK_RESPONSE_OK,
					       NULL );

	label = gtk_label_new ( _("Mark transactions you want to add to the list and click the add button"));
	gtk_box_pack_start ( GTK_BOX ( GTK_DIALOG ( dialog ) -> vbox ),
			     label,
			     FALSE,
			     FALSE,
			     0 );
	gtk_widget_show ( label );

	store = gtk_list_store_new ( 4,
				     G_TYPE_BOOLEAN,
				     G_TYPE_STRING,
				     G_TYPE_STRING,
				     G_TYPE_DOUBLE );

	/* on remplit la liste */

	liste_tmp = liste_opes_import_celibataires;

	while ( liste_tmp ) 
	{
	    struct struct_ope_importation *ope_import;
	    GtkTreeIter iter;

	    ope_import = liste_tmp -> data;

	    gtk_list_store_append ( store,
				    &iter );

	    gtk_list_store_set ( store,
				 &iter,
				 0, FALSE,
				 1, g_strdup_printf ( "%02d/%02d/%04d",
						      g_date_get_day ( ope_import -> date ),
						      g_date_get_month ( ope_import -> date ),
						      g_date_get_year ( ope_import -> date )),
				 2, ope_import -> tiers,
				 3, ope_import -> montant,
				 -1 );

	    liste_tmp = liste_tmp -> next;
	}

	/* on crée la liste des opés célibataires
	   et on y associe la gslist */

	liste_ope_celibataires = gtk_tree_view_new_with_model ( GTK_TREE_MODEL (store));
	g_object_set_data ( G_OBJECT ( liste_ope_celibataires ),
			    "liste_ope",
			    liste_opes_import_celibataires );
	scrolled_window = gtk_scrolled_window_new ( FALSE, FALSE );
	gtk_widget_set_usize ( scrolled_window, FALSE, 300 );
	gtk_box_pack_start ( GTK_BOX ( GTK_DIALOG ( dialog ) -> vbox ),
			     scrolled_window,
			     TRUE,
			     TRUE,
			     0 );
	gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_window ),
					 GTK_POLICY_AUTOMATIC,
					 GTK_POLICY_AUTOMATIC );
	gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW ( scrolled_window ),
						liste_ope_celibataires );
	gtk_widget_show_all ( scrolled_window );

	/* on affiche les colonnes */

	renderer = gtk_cell_renderer_toggle_new ();
	g_signal_connect ( renderer,
			   "toggled",
			   G_CALLBACK (click_sur_liste_opes_orphelines ),
			   store );
	column = gtk_tree_view_column_new_with_attributes ( _("Mark"),
							    renderer,
							    "active", 0,

							    NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (liste_ope_celibataires), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ( _("Date"),
							    renderer,
							    "text", 1,
							    NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (liste_ope_celibataires), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ( _("Third party"),
							    renderer,
							    "text", 2,
							    NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (liste_ope_celibataires), column);


	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ( _("Amount"),
							    renderer,
							    "text", 3,
							    NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (liste_ope_celibataires), column);

	g_signal_connect ( G_OBJECT ( dialog ),
			   "response",
			   G_CALLBACK ( click_dialog_ope_orphelines ),
			   liste_ope_celibataires );

	gtk_widget_show ( dialog );
    }
}
/* *******************************************************************************/


/* *******************************************************************************/
gboolean click_dialog_ope_orphelines ( GtkWidget *dialog,
				       gint result,
				       GtkWidget *liste_ope_celibataires )
{
    GSList *liste_opes_import_celibataires;
    GSList *liste_tmp;
    GtkTreeIter iter;
    GtkTreeModel *model;

    switch ( result )
    {
	case 1:
	case GTK_RESPONSE_OK:
	    /* on ajoute la ou les opés marquées à la liste d'opés en les pointant d'un T
	       puis on les retire de la liste des orphelines
	       s'il ne reste plus d'opés orphelines, on ferme la boite de dialogue */

	    liste_opes_import_celibataires = g_object_get_data ( G_OBJECT ( liste_ope_celibataires ),
								 "liste_ope" );
	    model = gtk_tree_view_get_model ( GTK_TREE_VIEW ( liste_ope_celibataires ));
	    gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( model ),
					    &iter );

	    liste_tmp = liste_opes_import_celibataires;

	    /* normalement, pas besoin de mettre ça à 0 car normalement pas de ventilations à ce stade... */

	    derniere_operation_enregistrement_ope_import = 0;

	    while ( liste_tmp )
	    {
		gboolean enregistre;
		GSList *last_item;

		gtk_tree_model_get ( GTK_TREE_MODEL ( model ),
				     &iter,
				     0, &enregistre,
				     -1 );

		if ( enregistre )
		{
		    /* à ce niveau, l'opé a été cochée donc on l'enregistre en la marquant T	 */

		    struct struct_ope_importation *ope_import;
		    struct structure_operation *operation;

		    ope_import = liste_tmp -> data;

		    operation = enregistre_ope_importee ( ope_import,
							  ope_import -> no_compte	);
		    operation -> pointe = TELECHECKED_TRANSACTION;

		    /* on a enregistré l'opé, on la retire maintenant de la liste et de la sliste */

		    last_item = liste_tmp;
		    liste_tmp = liste_tmp -> next;
		    liste_opes_import_celibataires = g_slist_remove_link ( liste_opes_import_celibataires,
									   last_item );

		    /* on retire la ligne qu'on vient d'enregistrer, celà met l'iter directement sur la suite */

		    gtk_list_store_remove ( GTK_LIST_STORE ( model),
					    &iter );
		}
		else
		{
		    gtk_tree_model_iter_next ( GTK_TREE_MODEL ( model ),
					       &iter );
		    liste_tmp = liste_tmp -> next;
		}
	    }

	    /* on enregistre la nouvelle liste d'opé pour la retrouver plus tard */

	    g_object_set_data ( G_OBJECT ( liste_ope_celibataires ),
				"liste_ope",
				liste_opes_import_celibataires );

	    /* il est possible que les opés importées soient des virements, il faut faire les
	       relations ici */
	    if ( virements_a_chercher )
		cree_liens_virements_ope_import ();

	    /* on met à jour tous les comptes */

	    demande_mise_a_jour_tous_comptes ();

	    /* on recrée les combofix des tiers et des catégories */

	    mise_a_jour_tiers ();
	    mise_a_jour_categ();

	    /* mise à jour de l'accueil */

	    update_liste_comptes_accueil ();
	    mise_a_jour_soldes_minimaux ();

	    reaffiche_liste_comptes_onglet ();

	    modification_fichier ( TRUE );


	    if ( result != GTK_RESPONSE_OK
		 &&
		 g_slist_length ( liste_opes_import_celibataires ))
		break;

	default:
	    gtk_widget_destroy ( dialog );
	    break;
    }

    return ( FALSE );
}
/* *******************************************************************************/


/* *******************************************************************************/
gboolean click_sur_liste_opes_orphelines ( GtkCellRendererToggle *renderer, 
					   gchar *ligne,
					   GtkTreeModel *store )
{
    GtkTreeIter iter;

    if ( gtk_tree_model_get_iter_from_string ( GTK_TREE_MODEL ( store ),
					       &iter,
					       ligne ))
    {
	gboolean valeur;

	gtk_tree_model_get ( GTK_TREE_MODEL ( store ),
			     &iter,
			     0, &valeur,
			     -1 );
	gtk_list_store_set ( GTK_LIST_STORE ( store ),
			     &iter,
			     0, 1 - valeur,
			     -1 );
    }
    return ( FALSE );
}		
/* *******************************************************************************/



/* *******************************************************************************/
/* page de configuration pour l'importation */
/* *******************************************************************************/
GtkWidget *onglet_importation (void)
{
    GtkWidget *vbox_pref;
    GtkWidget *hbox;
    GtkWidget *label;
    GtkWidget *bouton;
    gchar *sTmp;

    vbox_pref = new_vbox_with_title_and_icon ( _("Import"),
					       "files.png" ); 
    hbox = gtk_hbox_new ( FALSE,
			  0 );
    gtk_box_pack_start ( GTK_BOX ( vbox_pref ),
			 hbox,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( hbox );

    sTmp = POSTSPACIFY(_("Search the transaction"));
    label = gtk_label_new ( sTmp );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 label,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( label );
    g_free(sTmp);
    sTmp = NULL;

    bouton = gtk_spin_button_new_with_range ( 0.0,
					      100.0,
					      1.0);
    gtk_spin_button_set_value ( GTK_SPIN_BUTTON ( bouton ),
				valeur_echelle_recherche_date_import );
    g_signal_connect ( G_OBJECT ( bouton ),
		       "value-changed",
		       G_CALLBACK ( changement_valeur_echelle_recherche_date_import ),
		       NULL );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( bouton );

    sTmp = PRESPACIFY(_("days around the date in the imported transaction."));
    label = gtk_label_new ( sTmp );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 label,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( label );
    g_free(sTmp);
    sTmp = NULL;

    if ( !nb_comptes )
    {
	gtk_widget_set_sensitive ( vbox_pref, FALSE );
    }

    return ( vbox_pref );
}
/* *******************************************************************************/


/* *******************************************************************************/
gboolean changement_valeur_echelle_recherche_date_import ( GtkWidget *spin_button )
{
    valeur_echelle_recherche_date_import = gtk_spin_button_get_value_as_int ( GTK_SPIN_BUTTON ( spin_button ));
    modification_fichier ( TRUE );
    return ( FALSE );
}
/* *******************************************************************************/

