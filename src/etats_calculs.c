/*  Fichier qui s'occupe du calcul des états */
/*      etats.c */

/*     Copyright (C)	2000-2003 Cédric Auger (cedric@grisbi.org)	      */
/*			2004 Benjamin Drieu (bdrieu@april.org)		      */
/* 			http://www.grisbi.org				      */

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


/*START_INCLUDE*/
#include "etats_calculs.h"
#include "utils_devises.h"
#include "utils_categories.h"
#include "search_glist.h"
#include "etats_affiche.h"
#include "utils_str.h"
#include "utils_ib.h"
#include "utils_rapprochements.h"
#include "utils_tiers.h"
#include "utils_types.h"
/*END_INCLUDE*/

/*START_STATIC*/
static void affichage_etat ( struct struct_etat *etat, 
		      struct struct_etat_affichage * affichage );
static gint classement_liste_opes_etat ( struct structure_operation *operation_1,
				  struct structure_operation *operation_2 );
static gint classement_ope_perso_etat ( struct structure_operation *operation_1,
				 struct structure_operation *operation_2 );
static gint compare_cheques_etat ( gint chq_ope,
			    gint chq_test,
			    gint comparateur );
static gint compare_montants_etat ( gdouble montant_ope,
			     gdouble montant_test,
			     gint comparateur );
static void etape_finale_affichage_etat ( GSList *ope_selectionnees,
				   struct struct_etat_affichage *affichage);
static gchar *recupere_texte_test_etat ( struct structure_operation *operation,
				  gint champ );
static gint verifie_chq_test_etat ( struct struct_comparaison_textes_etat *comp_textes,
			     gchar *no_chq );
static gint verifie_texte_test_etat ( struct struct_comparaison_textes_etat *comp_textes,
			       gchar *texte_ope );
/*END_STATIC*/



gint dernier_chq;     /* quand on a choisi le plus grand, contient le dernier no de chq dans les comptes choisis */
gint dernier_pc;     /* quand on a choisi le plus grand, contient le dernier no de pc dans les comptes choisis */
gint dernier_no_rappr;     /* quand on a choisi le plus grand, contient le dernier no de rappr dans les comptes choisis */
struct struct_etat_affichage * etat_affichage_output;



gint ancien_tiers_etat;

/*START_EXTERN*/
extern gint ancien_compte_etat;
extern gint ancien_tiers_etat;
extern gint ancien_tiers_etat;
extern gint ancienne_categ_etat;
extern gint ancienne_categ_speciale_etat;
extern gint ancienne_ib_etat;
extern gint ancienne_sous_categ_etat;
extern gint ancienne_sous_ib_etat;
extern gint changement_de_groupe_etat;
extern GDate *date_debut_periode;
extern gint debut_affichage_etat;
extern struct struct_devise *devise_categ_etat;
extern struct struct_devise *devise_compte_en_cours_etat;
extern struct struct_devise *devise_generale_etat;
extern struct struct_devise *devise_ib_etat;
extern struct struct_devise *devise_operation;
extern struct struct_devise *devise_tiers_etat;
extern struct struct_etat *etat_courant;
extern gint exo_en_cours_etat;
extern struct struct_etat_affichage gtktable_affichage ;
extern struct struct_etat_affichage gtktable_affichage;
extern struct struct_etat_affichage latex_affichage ;
extern gint ligne_debut_partie;
extern GSList *liste_struct_exercices;
extern gdouble montant_categ_etat;
extern gdouble montant_compte_etat;
extern gdouble montant_exo_etat;
extern gdouble montant_ib_etat;
extern gdouble montant_periode_etat;
extern gdouble montant_sous_categ_etat;
extern gdouble montant_sous_ib_etat;
extern gdouble montant_tiers_etat;
extern gint nb_colonnes;
extern gint nb_comptes;
extern gint nb_ope_categ_etat;
extern gint nb_ope_compte_etat;
extern gint nb_ope_exo_etat;
extern gint nb_ope_general_etat;
extern gint nb_ope_ib_etat;
extern gint nb_ope_partie_etat;
extern gint nb_ope_periode_etat;
extern gint nb_ope_sous_categ_etat;
extern gint nb_ope_sous_ib_etat;
extern gint nb_ope_tiers_etat;
extern gchar *nom_categ_en_cours;
extern gchar *nom_compte_en_cours;
extern gchar *nom_ib_en_cours;
extern gchar *nom_ss_categ_en_cours;
extern gchar *nom_ss_ib_en_cours;
extern gchar *nom_tiers_en_cours;
extern GtkWidget *notebook_general;
extern gpointer **p_tab_nom_de_compte;
extern gpointer **p_tab_nom_de_compte_variable;
extern GtkTreeSelection * selection;
/*END_EXTERN*/


/*****************************************************************************************************/
void affichage_etat ( struct struct_etat *etat, 
		      struct struct_etat_affichage * affichage )
{
    GSList *liste_opes_selectionnees;

    if ( !etat )
    {
	if ( etat_courant )
	    etat = etat_courant;
	else
	    return;
    }

    if ( !affichage )
	affichage = &gtktable_affichage;


    /*   selection des opérations */
    /* on va mettre l'adresse des opés sélectionnées dans une liste */

    liste_opes_selectionnees = recupere_opes_etat ( etat );


    /*   à ce niveau, on a récupéré toutes les opés qui entreront dans */
    /* l'état ; reste plus qu'à les classer et les afficher */
    /* on classe la liste et l'affiche en fonction du choix du type de classement */

    etat_affichage_output = affichage;
    etape_finale_affichage_etat ( liste_opes_selectionnees, affichage );

}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* cette fontion prend en argument l'adr d'un état, fait le tour de tous les comptes et */
/* sélectionne les opérations qui appartiennent à cet état. elle renvoie une liste des */
/* adr de ces opérations */
/* elle est appelée pour l'affichage d'un état ou pour la récupération des tiers d'un état */
/*****************************************************************************************************/

GSList *recupere_opes_etat ( struct struct_etat *etat )
{
    GSList *liste_operations_etat;
    gint i;
    gint no_exercice_recherche;
    GSList *liste_tmp;

    liste_operations_etat = NULL;

    /* si on utilise l'exercice courant ou précédent, on cherche ici */
    /* le numéro de l'exercice correspondant */

    no_exercice_recherche = 0;

    if ( etat -> exo_date )
    {
	GDate *date_jour;
	struct struct_exercice *exo;
	struct struct_exercice *exo_courant;
	struct struct_exercice *exo_precedent;

	liste_tmp = liste_struct_exercices;
	date_jour = g_date_new ();
	g_date_set_time ( date_jour,
			  time ( NULL ));


	exo_courant = NULL;
	exo_precedent = NULL;

	/*  dans tous les cas, on recherche l'exo courant */


	while ( liste_tmp )
	{
	    exo = liste_tmp -> data;

	    if ( g_date_compare ( date_jour,
				  exo -> date_debut ) >= 0
		 &&
		 g_date_compare ( date_jour,
				  exo -> date_fin ) <= 0 )
	    {
		exo_courant = exo;
		liste_tmp = NULL;
	    }
	    else
		liste_tmp = liste_tmp -> next;
	}


	/* si on veut l'exo précédent, c'est ici */

	switch ( etat -> utilise_detail_exo )
	{
	    case 1:
		/* on recherche l'exo courant */

		if ( exo_courant )
		    no_exercice_recherche = exo_courant -> no_exercice;
		break;

	    case 2:
		/* on recherche l'exo précédent */

		liste_tmp = liste_struct_exercices;

		while ( liste_tmp )
		{
		    struct struct_exercice *exo;

		    exo = liste_tmp -> data;

		    if ( exo_courant )
		    {
			/* si l'exo est avant exo_courant et après exo_precedent, c'est le nouvel exo_precedent */

			if ( g_date_compare ( exo -> date_fin,
					      exo_courant -> date_debut ) <= 0 )
			{
			    if ( exo_precedent )
			    {
				if ( g_date_compare ( exo -> date_debut,
						      exo_precedent -> date_fin ) >= 0 )
				    exo_precedent = exo;
			    }
			    else
				exo_precedent = exo;
			}
		    }
		    else
		    {
			/* 		      il n'y a pas d'exo courant, donc si l'exo est en date inférieur à la date du jour, */
			/* et après l'exo_precedent, c'est le nouvel exo précédent */

			if ( g_date_compare ( exo -> date_fin,
					      date_jour ) <= 0 )
			{
			    if ( exo_precedent )
			    {
				if ( g_date_compare ( exo -> date_debut,
						      exo_precedent -> date_fin ) >= 0 )
				    exo_precedent = exo;
			    }
			    else
				exo_precedent = exo;
			}

		    }

		    liste_tmp = liste_tmp -> next;
		}

		if ( exo_precedent )
		    no_exercice_recherche = exo_precedent -> no_exercice;

		break;
	}
    }


    /*   si on a utilisé "le plus grand" dans la recherche de texte, c'est ici qu'on recherche */
    /*     les plus grands no de chq, de rappr et de pc dans les comptes choisis */

    /* commence par rechercher si on a utilisé "le plus grand" */

    liste_tmp = etat -> liste_struct_comparaison_textes;

    while ( liste_tmp )
    {
	struct struct_comparaison_textes_etat *comp_textes;

	comp_textes = liste_tmp -> data;

	if ( comp_textes -> comparateur_1 == 6
	     ||
	     comp_textes -> comparateur_2 == 6 )
	{
	    /* on utilise "le plus grand" qque part, donc on va remplir les 3 variables */

	    dernier_chq = 0;
	    dernier_pc = 0;
	    dernier_no_rappr = 0;

	    for ( i=0 ; i<nb_comptes ; i++ )
	    {
		/* on commence par vérifier que le compte fait partie de l'état */

		p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

		if ( !etat -> utilise_detail_comptes
		     ||
		     g_slist_index ( etat -> no_comptes,
				     GINT_TO_POINTER ( i )) != -1 )
		{
		    GSList *pointeur_tmp;

		    /* on fait le tour de la liste des opés en recherchant le plus grand ds les 3 variables */

		    pointeur_tmp = LISTE_OPERATIONS;

		    while ( pointeur_tmp )
		    {
			struct structure_operation *operation;

			operation = pointeur_tmp -> data;

			/* commence par le cheque, il faut que le type opé soit à incrémentation auto */
			/* et le no le plus grand */
			/* on ne recherche le type d'opé que si l'opé a un contenu du type et que celui ci */
			/* est > que dernier_chq */

			if ( operation -> contenu_type 
			     &&
			     my_atoi ( operation -> contenu_type ) > dernier_chq
			     &&
			     operation -> type_ope )
			{
			    struct struct_type_ope *type_ope;

			    type_ope = type_ope_par_no ( operation -> type_ope,
							 i );

			    if ( type_ope
				 &&
				 type_ope -> affiche_entree
				 &&
				 type_ope -> numerotation_auto )
				dernier_chq = my_atoi ( operation -> contenu_type );
			}


			/* on récupère maintenant la plus grande pc */

			if ( operation -> no_piece_comptable
			     &&
			     my_atoi ( operation -> no_piece_comptable ) > dernier_pc )
			    dernier_pc = my_atoi ( operation -> no_piece_comptable );


			/* on récupère maintenant le dernier relevé */

			if ( operation -> no_rapprochement > dernier_no_rappr )
			    dernier_no_rappr = operation -> no_rapprochement;

			pointeur_tmp = pointeur_tmp -> next;
		    }
		}
	    }
	    liste_tmp = NULL;
	}
	else
	    liste_tmp = liste_tmp -> next;
    }


    for ( i=0 ; i<nb_comptes ; i++ )
    {
	/* on commence par vérifier que le compte fait partie de l'état */

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

	if ( !etat -> utilise_detail_comptes
	     ||
	     g_slist_index ( etat -> no_comptes,
			     GINT_TO_POINTER ( i )) != -1 )
	{
	    /* 	  le compte est bon, passe à la suite de la sélection */


	    /* on va faire le tour de toutes les opés du compte */

	    GSList *pointeur_tmp;

	    pointeur_tmp = LISTE_OPERATIONS;

	    while ( pointeur_tmp )
	    {
		struct structure_operation *operation;

		operation = pointeur_tmp -> data;

		/* si c'est une opé ventilée, dépend de la conf */

		if ( operation -> operation_ventilee
		     &&
		     !etat -> pas_detailler_ventilation )
		    goto operation_refusee;

		if ( operation -> no_operation_ventilee_associee
		     &&
		     etat -> pas_detailler_ventilation )
		    goto operation_refusee;

		/* on vérifie ensuite si un texte est recherché */

		if ( etat -> utilise_texte )
		{
		    gint garde_ope;

		    liste_tmp = etat -> liste_struct_comparaison_textes;
		    garde_ope = 0;

		    while ( liste_tmp )
		    {
			gchar *texte;
			struct struct_comparaison_textes_etat *comp_textes;
			gint ope_dans_test;

			comp_textes = liste_tmp -> data;


			/* on commence par récupérer le texte du champs recherché */

			texte = recupere_texte_test_etat ( operation,
							   comp_textes -> champ );

			/* à ce niveau, texte est soit null, soit contient le texte dans lequel on effectue la recherche */
			/* on vérifie maintenant en fontion de l'opérateur */
			/* si c'est un chq ou une pc et que utilise_txt, on utilise leur no */

			if ( ( comp_textes -> champ == 8
			       ||
			       comp_textes -> champ == 9
			       ||
			       comp_textes -> champ == 10 )
			     &&
			     !comp_textes -> utilise_txt )
			{
			    if ( texte )
				ope_dans_test = verifie_chq_test_etat ( comp_textes,
									texte );
			    else
				ope_dans_test = 0;
			}
			else
			    ope_dans_test = verifie_texte_test_etat ( comp_textes,
								      texte );

			/* à ce niveau, ope_dans_test=1 si l'opé a passé ce test */
			/* il faut qu'on fasse le lien avec la ligne précédente */

			switch ( comp_textes -> lien_struct_precedente )
			{
			    case -1:
				/* 1ère ligne  */

				garde_ope = ope_dans_test;
				break;

			    case 0:
				/* et  */

				garde_ope = garde_ope && ope_dans_test;
				break;

			    case 1:
				/* ou  */

				garde_ope = garde_ope || ope_dans_test;
				break;

			    case 2:
				/* sauf  */

				garde_ope = garde_ope && (!ope_dans_test);
				break;
			}
			liste_tmp = liste_tmp -> next;
		    }

		    /* on ne garde l'opération que si garde_ope = 1 */

		    if ( !garde_ope )
			goto operation_refusee;
		}


		/* on vérifie les R */

		if ( etat -> afficher_r )
		{
		    if ( ( etat -> afficher_r == 1
			   &&
			   operation -> pointe == 3 )
			 ||
			 ( etat -> afficher_r == 2
			   &&
			   operation -> pointe != 3 ))
			goto operation_refusee;
		}


		/*    vérification du montant nul */

		if ( etat -> exclure_montants_nuls
		     &&
		     fabs ( operation -> montant ) < 0.01 )
		    goto operation_refusee;


		/* vérification des montants */

		if ( etat -> utilise_montant )
		{
		    gint garde_ope;

		    liste_tmp = etat -> liste_struct_comparaison_montants;
		    garde_ope = 0;

		    while ( liste_tmp )
		    {
			gdouble montant;
			gint ope_dans_premier_test;
			gint ope_dans_second_test;
			struct struct_comparaison_montants_etat *comp_montants;
			gint ope_dans_test;

			comp_montants = liste_tmp -> data;

			montant = calcule_montant_devise_renvoi ( operation -> montant,
								  etat -> choix_devise_montant,
								  operation -> devise,
								  operation -> une_devise_compte_egale_x_devise_ope,
								  operation -> taux_change,
								  operation -> frais_change );

			/* on vérifie maintenant en fonction de la ligne de test si on garde cette opé */

			ope_dans_premier_test = compare_montants_etat ( montant,
									comp_montants -> montant_1,
									comp_montants -> comparateur_1 );

			if ( comp_montants -> lien_1_2 != 3 )
			    ope_dans_second_test = compare_montants_etat ( montant,
									   comp_montants -> montant_2,
									   comp_montants -> comparateur_2 );
			else
			    /* pour éviter les warning lors de la compil */
			    ope_dans_second_test = 0;

			switch ( comp_montants -> lien_1_2 )
			{
			    case 0:
				/* et  */

				ope_dans_test = ope_dans_premier_test && ope_dans_second_test;
				break;

			    case 1:
				/*  ou */

				ope_dans_test = ope_dans_premier_test || ope_dans_second_test;
				break;

			    case 2:
				/* sauf  */

				ope_dans_test = ope_dans_premier_test && (!ope_dans_second_test);
				break;

			    case 3:
				/* aucun  */
				ope_dans_test = ope_dans_premier_test;
				break;

			    default:
				ope_dans_test = 0;
			}

			/* à ce niveau, ope_dans_test=1 si l'opé a passé ce test */
			/* il faut qu'on fasse le lien avec la ligne précédente */


			switch ( comp_montants -> lien_struct_precedente )
			{
			    case -1:
				/* 1ère ligne  */

				garde_ope = ope_dans_test;
				break;

			    case 0:
				/* et  */

				garde_ope = garde_ope && ope_dans_test;
				break;

			    case 1:
				/* ou  */

				garde_ope = garde_ope || ope_dans_test;
				break;

			    case 2:
				/* sauf  */

				garde_ope = garde_ope && (!ope_dans_test);
				break;
			}
			liste_tmp = liste_tmp -> next;
		    }

		    /* on ne garde l'opération que si garde_ope = 1 */

		    if ( !garde_ope )
			goto operation_refusee;

		}



		/* 	      on vérifie les virements */

		if ( operation -> relation_no_operation )
		{
		    if ( !etat -> type_virement )
			goto operation_refusee;

		    if ( etat -> type_virement == 1 )
		    {
			/* on inclue l'opé que si le compte de virement */
			/* est un compte de passif ou d'actif */

			p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

			if ( TYPE_DE_COMPTE != GSB_TYPE_PASSIF
			     &&
			     TYPE_DE_COMPTE != GSB_TYPE_ACTIF )
			    goto operation_refusee;

			p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i; 
		    }
		    else
		    {
			if ( etat -> type_virement == 2 )
			{
			    /* on inclut l'opé que si le compte de virement n'est */
			    /* pas présent dans l'état */

			    /*    si on ne détaille pas les comptes, on ne cherche pas, l'opé est refusée */
			    if ( etat -> utilise_detail_comptes )
			    {
				if ( g_slist_index ( etat -> no_comptes,
						     GINT_TO_POINTER ( operation -> relation_no_compte )) != -1 )
				    goto operation_refusee;
			    }
			    else
				goto operation_refusee;
			}
			else
			{
			    /* on inclut l'opé que si le compte de virement est dans la liste */

			    if ( g_slist_index ( etat -> no_comptes_virements,
						 GINT_TO_POINTER ( operation -> relation_no_compte )) == -1 )
				goto operation_refusee;

			}
		    }
		}
		else
		{
		    /* 		  l'opé n'est pas un virement, si on doit exclure les non virement, c'est ici */

		    if ( etat -> type_virement
			 &&
			 etat -> exclure_ope_non_virement )
			goto operation_refusee;
		}


		/* on va maintenant vérifier que les catég sont bonnes */
		/* si on exclut les opés sans categ, on vérifie que c'est pas un virement ni une ventilation */

		if ((( etat -> utilise_detail_categ
		       &&
		       g_slist_index ( etat -> no_categ,
				       GINT_TO_POINTER ( operation -> categorie )) == -1 )
		     ||
		     ( etat -> exclure_ope_sans_categ
		       &&
		       !operation -> categorie ))
		    &&
		    !operation -> operation_ventilee
		    &&
		    !operation -> relation_no_operation )
		    goto operation_refusee;


		/* vérification de l'imputation budgétaire */

		if ( etat -> exclure_ope_sans_ib && !operation -> imputation )
		    goto operation_refusee;

		if ((etat -> utilise_detail_ib &&
		     g_slist_index(etat-> no_ib,
				   GINT_TO_POINTER(operation->imputation)) == -1) &&
		    operation -> imputation)
		    goto operation_refusee;


		/* vérification du tiers */

		if ( etat -> utilise_detail_tiers
		     &&
		     g_slist_index ( etat -> no_tiers,
				     GINT_TO_POINTER ( operation -> tiers )) == -1 )
		    goto operation_refusee;

		/* vérification du type d'opération */

		if ( etat -> utilise_mode_paiement )
		{
		    struct struct_type_ope *type_ope;

		    if ( ! operation -> type_ope )
			goto operation_refusee;

		    /* normalement p_tab... est sur le compte en cours */

		    type_ope = type_ope_par_no ( operation -> type_ope,
						 i );

		    if ( !type_ope )
			goto operation_refusee;

		    if ( !g_slist_find_custom ( etat -> noms_modes_paiement,
						type_ope -> nom_type,
						(GCompareFunc) cherche_string_equivalente_dans_slist ))
			goto operation_refusee;
		}

		/* vérifie la plage de date */

		if ( etat -> exo_date )
		{
		    /* on utilise l'exercice */

		    if ( (( etat -> utilise_detail_exo == 1
			    ||
			    etat -> utilise_detail_exo == 2 )
			  &&
			  ( operation -> no_exercice != no_exercice_recherche
			    ||
			    !operation -> no_exercice ))
			 ||
			 ( etat -> utilise_detail_exo == 3
			   &&
			   ( g_slist_index ( etat -> no_exercices,
					     GINT_TO_POINTER ( operation -> no_exercice )) == -1
			     ||
			     !operation -> no_exercice )))
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
			    /* cumul à ce jour */

			    if ( g_date_compare ( date_jour,
						  operation -> date ) < 0 )
				goto operation_refusee;
			    break;

			case 3:
			    /* mois en cours */

			    if ( g_date_month ( date_jour ) != g_date_month ( operation -> date )
				 ||
				 g_date_year ( date_jour ) != g_date_year ( operation -> date ))
				goto operation_refusee;
			    break;

			case 4:
			    /* année en cours */

			    if ( g_date_year ( date_jour ) != g_date_year ( operation -> date ))
				goto operation_refusee;
			    break;

			case 5:
			    /* cumul mensuel */

			    if ( g_date_month ( date_jour ) != g_date_month ( operation -> date )
				 ||
				 g_date_year ( date_jour ) != g_date_year ( operation -> date )
				 ||
				 g_date_compare ( date_jour,
						  operation -> date ) < 0 )
				goto operation_refusee;
			    break;

			case 6:
			    /* cumul annuel */

			    if ( g_date_year ( date_jour ) != g_date_year ( operation -> date )
				 ||
				 g_date_compare ( date_jour,
						  operation -> date ) < 0 )
				goto operation_refusee;
			    break;

			case 7:
			    /* mois précédent */

			    g_date_subtract_months ( date_jour,
						     1 );

			    if ( g_date_month ( date_jour ) != g_date_month ( operation -> date )
				 ||
				 g_date_year ( date_jour ) != g_date_year ( operation -> date ))
				goto operation_refusee;
			    break;

			case 8:
			    /* année précédente */

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
			    if ( g_date_compare ( date_tmp,
						  operation -> date ) > 0
				 ||
				 g_date_compare ( date_jour,
						  operation -> date ) < 0 )
				goto operation_refusee;
			    break;
		    }
		}


		liste_operations_etat = g_slist_append ( liste_operations_etat,
							 operation );

operation_refusee:
		pointeur_tmp = pointeur_tmp -> next;
	    }
	}
    }


    return ( liste_operations_etat );
}
/*****************************************************************************************************/


/*****************************************************************************************************/
/* récupère le texte pour faire le test sur les textes */
/*****************************************************************************************************/

gchar *recupere_texte_test_etat ( struct structure_operation *operation,
				  gint champ )
{
    gchar *texte;
    struct struct_tiers *tiers;

    switch ( champ )
    {
	case 0:
	    /* tiers  */

	    texte = tiers_name_by_no ( operation -> tiers, TRUE );
	    break;

	case 1:
	    /* info du tiers */
	    
	    tiers = tiers_par_no ( operation -> tiers );

	    if ( tiers )
		texte = tiers -> texte;
	    else
		texte = NULL;
	    break;

	case 2:
	    /* categ */

	    texte = nom_categ_par_no ( operation -> categorie,
				       0 );
	    break;

	case 3:
	    /* ss-categ */

	    texte = nom_sous_categ_par_no ( operation -> categorie,
					    operation -> sous_categorie );
	    break;

	case 4:
	    /* ib */

	    texte = nom_imputation_par_no ( operation -> imputation,
					    0 );
	    break;

	case 5:
	    /* ss-ib */

	    texte = nom_imputation_par_no ( operation -> imputation,
					    operation -> sous_imputation );
	    break;

	case 6:
	    /* notes  */

	    texte = operation -> notes;
	    break;

	case 7:
	    /* ref bancaires  */

	    texte = operation -> info_banque_guichet;
	    break;

	case 8:
	    /* pc */

	    texte = operation -> no_piece_comptable;
	    break;


	case 9:
	    /* chq  */

	    texte = operation -> contenu_type;
	    break;

	case 10:
	    /* no rappr */

	    texte = rapprochement_name_by_no ( operation -> no_rapprochement );
	    break;

	default:
	    texte = NULL;
    }

    return ( texte );
}
/*****************************************************************************************************/


/*****************************************************************************************************/
/* vérifie si l'opé passe le test du texte */
/*****************************************************************************************************/

gint verifie_texte_test_etat ( struct struct_comparaison_textes_etat *comp_textes,
			       gchar *texte_ope )
{
    gint ope_dans_test;
    gchar *position;

    ope_dans_test = 0;

    switch ( comp_textes -> operateur )
    {
	case 0:
	    /* contient  */

	    if ( texte_ope
		 &&
		 comp_textes -> texte )
	    {
		position = strstr ( texte_ope,
				    comp_textes -> texte );

		if ( position )
		    ope_dans_test = 1;
	    }
	    break;

	case 1:
	    /* ne contient pas  */

	    if ( texte_ope
		 &&
		 comp_textes -> texte )
	    {
		position = strstr ( texte_ope,
				    comp_textes -> texte );

		if ( !position )
		    ope_dans_test = 1;
	    }
	    else
		ope_dans_test = 1;
	    break;

	case 2:
	    /* commence par  */

	    if ( texte_ope
		 &&
		 comp_textes -> texte )
	    {
		position = strstr ( texte_ope,
				    comp_textes -> texte );

		if ( position == texte_ope )
		    ope_dans_test = 1;
	    }
	    break;

	case 3:
	    /* se termine par  */

	    if ( texte_ope
		 &&
		 comp_textes -> texte )
	    {
		position = strstr ( texte_ope,
				    comp_textes -> texte );
		if ( position == ( texte_ope + strlen ( texte_ope ) - strlen ( comp_textes -> texte)))
		    ope_dans_test = 1;
	    }
	    break;

	case 4:
	    /* vide  */

	    if ( !texte_ope )
		ope_dans_test = 1;
	    break;

	case 5:
	    /* non vide  */

	    if ( texte_ope )
		ope_dans_test = 1;
	    break;
    }

    return ( ope_dans_test );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* vérifie si l'opé passe le test du chq */
/*****************************************************************************************************/

gint verifie_chq_test_etat ( struct struct_comparaison_textes_etat *comp_textes,
			     gchar *no_chq )
{
    gint ope_dans_test;
    gint ope_dans_premier_test;
    gint ope_dans_second_test;

    /* pour éviter les warnings lors de la compil */

    ope_dans_premier_test = 0;
    ope_dans_second_test = 0;

    /*   si on cherche le plus grand, on met la valeur recherchée à la place de montant_1 */

    if ( comp_textes -> comparateur_1 != 6 )
	ope_dans_premier_test = compare_cheques_etat ( my_atoi ( no_chq ),
						       comp_textes -> montant_1,
						       comp_textes -> comparateur_1 );
    else
    {
	struct struct_no_rapprochement *rapprochement;

	switch ( comp_textes -> champ )
	{
	    case 8:
		/* pc */

		ope_dans_premier_test = compare_cheques_etat ( my_atoi ( no_chq ),
							       dernier_pc,
							       comp_textes -> comparateur_1 );
		break;

	    case 9:
		/* chq */

		ope_dans_premier_test = compare_cheques_etat ( my_atoi ( no_chq ),
							       dernier_chq,
							       comp_textes -> comparateur_1 );
		break;

	    case 10:
		/* rappr */
		/* no_chq contient le nom du rapprochement de l'opé, or pour le plus grand, on cherche */
		/* le no du rapprochement, on le cherche ici */

		rapprochement = rapprochement_par_nom ( no_chq );

		if ( rapprochement )
		    ope_dans_premier_test = compare_cheques_etat ( rapprochement -> no_rapprochement,
								   dernier_no_rappr,
								   comp_textes -> comparateur_1 );
		break;
	}
    }



    if ( comp_textes -> lien_1_2 != 3 )
    {
	if ( comp_textes -> comparateur_2 != 6 )
	    ope_dans_second_test = compare_cheques_etat ( my_atoi ( no_chq ),
							  comp_textes -> montant_2,
							  comp_textes -> comparateur_2 );
	else
	{
	    switch ( comp_textes -> champ )
	    {
		case 8:
		    /* pc */

		    ope_dans_second_test = compare_cheques_etat ( my_atoi ( no_chq ),
								  dernier_pc,
								  comp_textes -> comparateur_2 );
		    break;

		case 9:
		    /* chq */

		    ope_dans_second_test = compare_cheques_etat ( my_atoi ( no_chq ),
								  dernier_chq,
								  comp_textes -> comparateur_2 );
		    break;

		case 10:
		    /* rappr */

		    ope_dans_second_test = compare_cheques_etat ( my_atoi ( no_chq ),
								  dernier_no_rappr,
								  comp_textes -> comparateur_2 );
		    break;
	    }
	}
    }
    else
	/* pour éviter les warning lors de la compil */
	ope_dans_second_test = 0;

    switch ( comp_textes -> lien_1_2 )
    {
	case 0:
	    /* et  */

	    ope_dans_test = ope_dans_premier_test && ope_dans_second_test;
	    break;

	case 1:
	    /*  ou */

	    ope_dans_test = ope_dans_premier_test || ope_dans_second_test;
	    break;

	case 2:
	    /* sauf  */

	    ope_dans_test = ope_dans_premier_test && (!ope_dans_second_test);
	    break;

	case 3:
	    /* aucun  */
	    ope_dans_test = ope_dans_premier_test;
	    break;

	default:
	    ope_dans_test = 0;
    }


    return ( ope_dans_test );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* compare les 2 no de chq en fonction du comparateur donné en argument */
/* renvoie 1 si l'opé passe le test, 0 sinon */
/*****************************************************************************************************/

gint compare_cheques_etat ( gint chq_ope,
			    gint chq_test,
			    gint comparateur )
{
    gint retour;


    retour = 0;

    switch ( comparateur )
    {
	case 0:
	    /* =  */

	    if ( chq_ope == chq_test )
		retour = 1;
	    break;

	case 1:
	    /* <  */

	    if ( chq_ope < chq_test )
		retour = 1;
	    break;

	case 2:
	    /* <=  */

	    if ( chq_ope <= chq_test )
		retour = 1;
	    break;

	case 3:
	    /* >  */

	    if ( chq_ope > chq_test )
		retour = 1;
	    break;

	case 4:
	    /* >=  */

	    if ( chq_ope >= chq_test )
		retour = 1;
	    break;

	case 5:
	    /* !=  */

	    if ( chq_ope != chq_test )
		retour = 1;
	    break;

	case 6:
	    /* le plus grand */

	    if ( chq_ope == chq_test )
		retour = 1;
	    break;
    }

    return ( retour );
}
/*****************************************************************************************************/




/*****************************************************************************************************/
/* compare les 2 montants en fonction du comparateur donné en argument */
/* renvoie 1 si l'opé passe le test, 0 sinon */
/*****************************************************************************************************/

gint compare_montants_etat ( gdouble montant_ope,
			     gdouble montant_test,
			     gint comparateur )
{
    gint retour;
    gint montant_ope_int;
    gint montant_test_int;
    retour = 0;

    /*   le plus simple est de comparer des integer sinon, le = n'a pas grande valeur en float */

    montant_ope_int = rint ( montant_ope * 100 );
    montant_test_int = rint ( montant_test * 100 );

    switch ( comparateur )
    {
	case 0:
	    /* =  */

	    if ( montant_ope_int == montant_test_int )
		retour = 1;
	    break;

	case 1:
	    /* <  */

	    if ( montant_ope_int < montant_test_int )
		retour = 1;
	    break;

	case 2:
	    /* <=  */

	    if ( montant_ope_int <= montant_test_int )
		retour = 1;
	    break;

	case 3:
	    /* >  */

	    if ( montant_ope_int > montant_test_int )
		retour = 1;
	    break;

	case 4:
	    /* >=  */

	    if ( montant_ope_int >= montant_test_int )
		retour = 1;
	    break;

	case 5:
	    /* !=  */

	    if ( montant_ope_int != montant_test_int )
		retour = 1;
	    break;

	case 6:
	    /* nul  */

	    if ( !montant_ope_int )
		retour = 1;
	    break;

	case 7:
	    /* non nul  */

	    if ( montant_ope_int )
		retour = 1;
	    break;

	case 8:
	    /* positif  */

	    if ( montant_ope_int > 0 )
		retour = 1;
	    break;

	case 9:
	    /* négatif  */

	    if ( montant_ope_int < 0 )
		retour = 1;
	    break;

    }

    return ( retour );
}
/*****************************************************************************************************/





/*****************************************************************************************************/
/* Fonction de rafraichissement de l'état */
/*****************************************************************************************************/
void rafraichissement_etat ( struct struct_etat *etat )
{
    affichage_etat ( etat, &gtktable_affichage );
}


/*****************************************************************************************************/
/* Fonction d'impression de l'état */
/*****************************************************************************************************/
void impression_etat ( struct struct_etat *etat )
{
    affichage_etat ( etat, &latex_affichage );
}


void impression_etat_courant ( )
{
    if ( gtk_notebook_get_current_page ( GTK_NOTEBOOK ( notebook_general)) != 7 )
	gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_general),
				7 );

    impression_etat ( NULL );
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

    /*   si pointeur est nul, on a fait le tour du classement, les opés sont identiques */
    /* si elles sont affichées, on classe classement demandé puis par no d'opé si identiques */
    /* sinon on repart en mettant -1 */

    if ( !pointeur )
    {
	return ( classement_ope_perso_etat ( operation_1, operation_2 ));

    }

    switch ( GPOINTER_TO_INT ( pointeur -> data ))
    {
	/* classement des catégories */

	case 1:

	    if ( etat_courant -> utilise_categ )
	    {
		if ( operation_1 -> categorie != operation_2 -> categorie )
		    return ( operation_1 -> categorie - operation_2 -> categorie );

		/*     si  les catégories sont nulles, on doit départager entre virements, pas */
		/* de categ ou opé ventilée */
		/* on met en 1er les opés sans categ, ensuite les ventilations et enfin les virements */

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
	    }

	    /*       les catégories sont identiques, passe au classement suivant */

	    pointeur = pointeur -> next;
	    goto classement_suivant;

	    break;

	    /* classement des sous catégories */

	case 2:

	    if ( etat_courant -> utilise_categ
		 &&
		 etat_courant -> afficher_sous_categ )
	    {
		if ( operation_1 -> sous_categorie != operation_2 -> sous_categorie )
		    return ( operation_1 -> sous_categorie - operation_2 -> sous_categorie );
	    }

	    /*       les ss-catégories sont identiques, passe au classement suivant */

	    pointeur = pointeur -> next;
	    goto classement_suivant;

	    break;

	    /* classement des ib */

	case 3:

	    if ( etat_courant -> utilise_ib )
	    {
		if ( operation_1 -> imputation != operation_2 -> imputation )
		    return ( operation_1 -> imputation - operation_2 -> imputation );
	    }

	    /*       les ib sont identiques, passe au classement suivant */

	    pointeur = pointeur -> next;
	    goto classement_suivant;

	    break;

	    /* classement des sous ib */

	case 4:

	    if ( etat_courant -> utilise_ib
		 &&
		 etat_courant -> afficher_sous_ib )
	    {
		if ( operation_1 -> sous_imputation != operation_2 -> sous_imputation )
		    return ( operation_1 -> sous_imputation - operation_2 -> sous_imputation );
	    }

	    /*       les ib sont identiques, passe au classement suivant */

	    pointeur = pointeur -> next;
	    goto classement_suivant;

	    break;


	    /* classement des comptes */

	case 5:

	    if ( etat_courant -> regroupe_ope_par_compte )
	    {
		if ( operation_1 -> no_compte != operation_2 -> no_compte )
		    return ( operation_1 ->no_compte  - operation_2 -> no_compte );
	    }

	    /*       les comptes sont identiques, passe au classement suivant */

	    pointeur = pointeur -> next;
	    goto classement_suivant;
	    break;


	    /* classement des tiers */

	case 6:

	    if ( etat_courant -> utilise_tiers )
	    {
		if ( operation_1 -> tiers != operation_2 -> tiers )
		    return ( operation_1 ->tiers  - operation_2 -> tiers );
	    }

	    /*       les tiers sont identiques, passe au classement suivant */

	    pointeur = pointeur -> next;
	    goto classement_suivant;
	    break;
    }
    return (0);
}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* cette fonction est appelée quand l'opé a été classé dans sa categ, ib, compte ou tiers */
/* et qu'elle doit être affichée ; on classe en fonction de la demande de la conf ( date, no, tiers ...) */
/* si les 2 opés sont équivalentes à ce niveau, on classe par no d'opé */
/*****************************************************************************************************/

gint classement_ope_perso_etat ( struct structure_operation *operation_1,
				 struct structure_operation *operation_2 )
{
    gint retour;

    switch ( etat_courant -> type_classement_ope )
    {
	case 0:
	    /* date  */

	    retour = g_date_compare ( operation_1 -> date,
				      operation_2 -> date );
	    break;

	case 1:
	    /* no opé  */

	    retour = operation_1 -> no_operation - operation_2 -> no_operation;
	    break;

	case 2:
	    /* tiers  */

	    if ( !operation_1 -> tiers
		 ||
		 !operation_2 -> tiers )
		retour = operation_2 -> tiers - operation_1 -> tiers;
	    else
		retour = g_strcasecmp ( tiers_name_by_no ( operation_1 -> tiers, TRUE ),
					tiers_name_by_no ( operation_2 -> tiers, TRUE ));
	    break;

	case 3:
	    /* categ  */

	    if ( !operation_1 -> categorie
		 ||
		 !operation_2 -> categorie )
		retour = operation_2 -> categorie - operation_1 -> categorie;
	    else
	    {
		if ( operation_1 -> categorie == operation_2 -> categorie
		     &&
		     ( !operation_1 -> sous_categorie
		       ||
		       !operation_2 -> sous_categorie ))
		    retour = operation_2 -> sous_categorie - operation_1 -> sous_categorie;
		else
		    retour = g_strcasecmp ( nom_categ_par_no ( operation_1 -> categorie, operation_1 -> sous_categorie ),
					    nom_categ_par_no ( operation_2 -> categorie, operation_2 -> sous_categorie ));
	    }
	    break;

	case 4:
	    /* ib  */

	    if ( !operation_1 -> imputation
		 ||
		 !operation_2 -> imputation )
		retour = operation_2 -> imputation - operation_1 -> imputation;
	    else
	    {
		if ( operation_1 -> imputation == operation_2 -> imputation
		     &&
		     ( !operation_1 -> sous_imputation
		       ||
		       !operation_2 -> sous_imputation ))
		    retour = operation_2 -> sous_imputation - operation_1 -> sous_imputation;
		else
		    retour = g_strcasecmp ( nom_imputation_par_no ( operation_1 -> imputation, operation_1 -> sous_imputation ),
					    nom_imputation_par_no ( operation_2 -> imputation, operation_2 -> sous_imputation ));
	    }
	    break;

	case 5:
	    /* note si une des 2 opés n'a pas de notes, elle va en 2ème */

	    if ( operation_1 -> notes
		 &&
		 operation_2 -> notes )
		retour = g_strcasecmp ( operation_1 -> notes,
					operation_2 -> notes );
	    else
		retour = GPOINTER_TO_INT ( operation_2 -> notes ) - GPOINTER_TO_INT ( operation_1 -> notes );
	    break;

	case 6:
	    /* type ope  */

	    if ( !operation_1 -> type_ope
		 ||
		 !operation_2 -> type_ope )
		retour = operation_2 -> type_ope - operation_1 -> type_ope;
	    else
	    {
		/* les opés peuvent provenir de 2 comptes différents, il faut donc trouver les 2 types dans les */
		/* listes différentes */

		retour = g_strcasecmp ( type_ope_name_by_no ( operation_1 -> type_ope,
							      operation_1 -> no_compte ),
					type_ope_name_by_no ( operation_2 -> type_ope,
							      operation_2 -> no_compte ));
	    }
	    break;

	case 7:
	    /* no chq  */

	    if ( operation_1 -> contenu_type
		 &&
		 operation_2 -> contenu_type )
		retour = g_strcasecmp ( operation_1 -> contenu_type,
					operation_2 -> contenu_type );
	    else
		retour = GPOINTER_TO_INT ( operation_2 -> contenu_type ) - GPOINTER_TO_INT ( operation_1 -> contenu_type );
	    break;

	case 8:
	    /* pc  */

	    if ( operation_1 -> no_piece_comptable
		 &&
		 operation_2 -> no_piece_comptable )
		retour = g_strcasecmp ( operation_1 -> no_piece_comptable,
					operation_2 -> no_piece_comptable );
	    else
		retour = GPOINTER_TO_INT ( operation_2 -> no_piece_comptable ) - GPOINTER_TO_INT ( operation_1 -> no_piece_comptable );
	    break;

	case 9:
	    /* ibg  */

	    if ( operation_1 -> info_banque_guichet
		 &&
		 operation_2 -> info_banque_guichet )
		retour = g_strcasecmp ( operation_1 -> info_banque_guichet,
					operation_2 -> info_banque_guichet );
	    else
		retour = GPOINTER_TO_INT ( operation_2 -> info_banque_guichet ) - GPOINTER_TO_INT ( operation_1 -> info_banque_guichet );
	    break;

	case 10:
	    /* no rappr  */

	    if ( !operation_1 -> no_rapprochement
		 ||
		 !operation_2 -> no_rapprochement )
		retour = operation_2 -> no_rapprochement - operation_1 -> no_rapprochement;
	    else
		retour = g_strcasecmp ( rapprochement_name_by_no( operation_1 -> no_rapprochement ),
					rapprochement_name_by_no ( operation_2 -> no_rapprochement ));
	    break;

	default :
	    retour = 0;
    }


    if ( !retour )
	retour = operation_1 -> no_operation - operation_2 -> no_operation;

    return ( retour );
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


    /*   soit on sépare en revenus et dépenses, soit non */

    liste_ope_revenus = NULL;
    liste_ope_depenses = NULL;
    pointeur_tmp = ope_selectionnees;
    pointeur_glist = etat_courant -> type_classement;

    if ( etat_courant -> separer_revenus_depenses )
    {
	/* on commence par séparer la liste revenus et de dépenses */
	/*   si le classement racine est la catégorie, on sépare par catégorie */
	/* de revenu ou de dépense */
	/* si c'est un autre, on sépare par montant positif ou négatif */

	while ( pointeur_tmp )
	{
	    struct structure_operation *operation;

	    operation = pointeur_tmp -> data;

	    if ( GPOINTER_TO_INT ( pointeur_glist -> data ) == 1 )
	    {
		/* le classement racine est la catégorie */
		/* s'il n'y a pas de catég, c'est un virement ou une ventilation */
		/*       dans ce cas, on classe en fonction du montant */

		if ( operation -> categorie )
		{
		    struct struct_categ *categ;

		    categ = categ_par_no ( operation -> categorie );

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
		/* le classement racine n'est pas la catég, on sépare en fonction du montant */

		if ( operation -> montant < 0 )
		    liste_ope_depenses = g_slist_append ( liste_ope_depenses,
							  operation );
		else
		    liste_ope_revenus = g_slist_append ( liste_ope_revenus,
							 operation );
	    }
	    pointeur_tmp = pointeur_tmp -> next;
	}
    }
    else
	/*     on ne veut pas séparer en revenus et dépenses, on garde juste la liste d'opé telle quelle */

	liste_ope_revenus = ope_selectionnees;


    /* on va maintenant classer ces 2 listes dans l'ordre adéquat */

    liste_ope_depenses = g_slist_sort ( liste_ope_depenses,
					(GCompareFunc) classement_liste_opes_etat );
    liste_ope_revenus = g_slist_sort ( liste_ope_revenus,
				       (GCompareFunc) classement_liste_opes_etat );


    /* calcul du décalage pour chaque classement */
    /* c'est une chaine vide qu'on ajoute devant le nom du */
    /*   classement ( tiers, ib ...) */

    /* on met 2 espaces par décalage */
    /*   normalement, pointeur_glist est déjà positionné */

    decalage_base = "";

    /*   pour éviter le warning lors de la compilation, on met */
    /* toutes les var char à "" */

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
	    /* décalage de la catégorie */

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

		/* décalage de la ss-catégorie */

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

		/* décalage de l'ib */

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

		/* décalage de la ss-ib */

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

		/* décalage du compte */

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

		/* décalage du tiers */

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
    /* 1ère pour les titres de structure */
    /* la dernière pour les montants */
    /* et entre les 2 pour l'affichage des opés -> variable */

    nb_colonnes = 0;

    if ( etat_courant -> afficher_opes )
    {
	nb_colonnes = nb_colonnes + etat_courant -> afficher_date_ope;
	nb_colonnes = nb_colonnes + etat_courant -> afficher_tiers_ope;
	nb_colonnes = nb_colonnes + etat_courant -> afficher_categ_ope;
	nb_colonnes = nb_colonnes + etat_courant -> afficher_ib_ope;
	nb_colonnes = nb_colonnes + etat_courant -> afficher_notes_ope;
	nb_colonnes = nb_colonnes + etat_courant -> afficher_pc_ope;
	nb_colonnes = nb_colonnes + etat_courant -> afficher_infobd_ope;
	nb_colonnes = nb_colonnes + etat_courant -> afficher_no_ope;
	nb_colonnes = nb_colonnes + etat_courant -> afficher_type_ope;
	nb_colonnes = nb_colonnes + etat_courant -> afficher_cheque_ope;
	nb_colonnes = nb_colonnes + etat_courant -> afficher_rappr_ope;
	nb_colonnes = nb_colonnes + etat_courant -> afficher_exo_ope;

	/* on ajoute les séparations */

	nb_colonnes = 2 * nb_colonnes - 1;
    }

    nb_colonnes = nb_colonnes + 3;
    ligne_debut_partie = -1;

    nom_categ_en_cours = NULL;
    nom_ss_categ_en_cours = NULL;
    nom_ib_en_cours = NULL;
    nom_ss_ib_en_cours = NULL;
    nom_compte_en_cours = NULL;
    nom_tiers_en_cours = NULL;

    if (! etat_affiche_initialise (ope_selectionnees))
	return;


    /* on commence à remplir le tableau */

    /* on met le titre */

    total_general = 0;
    nb_ope_general_etat = 0;
    ligne = etat_affiche_affiche_titre ( 0 );

    /* séparation */
    ligne = etat_affiche_affiche_separateur (ligne);

    /*   si nécessaire, on met les titres des colonnes */

    if ( etat_courant -> afficher_opes
	 &&
	 etat_courant -> afficher_titre_colonnes
	 &&
	 !etat_courant -> type_affichage_titres )
	ligne = etat_affiche_affiche_titres_colonnes ( ligne );

    /*       on met directement les adr des devises de categ, ib et tiers en global pour */
    /* gagner de la vitesse */

    devise_categ_etat = devise_par_no ( etat_courant -> devise_de_calcul_categ );
    devise_ib_etat = devise_par_no ( etat_courant -> devise_de_calcul_ib );
    devise_tiers_etat = devise_par_no ( etat_courant -> devise_de_calcul_tiers );
    devise_generale_etat = devise_par_no ( etat_courant -> devise_de_calcul_general );


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
	montant_periode_etat = 0;
	montant_exo_etat = 0;
	total_partie = 0;
	date_debut_periode = NULL;
	exo_en_cours_etat = -1;


	nb_ope_categ_etat = 0;
	nb_ope_sous_categ_etat = 0;
	nb_ope_ib_etat = 0;
	nb_ope_sous_ib_etat = 0;
	nb_ope_compte_etat = 0;
	nb_ope_tiers_etat = 0;
	nb_ope_partie_etat = 0;
	nb_ope_periode_etat = 0;
	nb_ope_exo_etat = 0;

	changement_de_groupe_etat = 0;
	debut_affichage_etat = 1;
	devise_compte_en_cours_etat = NULL;

	/* on met ici le pointeur sur les revenus ou sur les dépenses */
	/* en vérifiant qu'il y en a */

	if ( i )
	{
	    /* c'est le second passage */
	    /* on met le pointeur sur les dépenses */

	    if ( liste_ope_depenses )
	    {
		/* séparation */
		ligne = etat_affiche_affiche_separateur (ligne);
		pointeur_tmp = liste_ope_depenses;

		ligne = etat_affiche_affiche_titre_depenses_etat ( ligne );
	    }
	    else
		continue;
	}
	else
	{
	    /* c'est le premier passage */
	    /* on met le pointeur sur les revenus */
	    /* si on n'a pas demandé de séparer les débits et crédits, on ne met pas de titre */

	    if ( etat_courant -> separer_revenus_depenses )
	    {
		/* on sépare les revenus des débits */

		if ( liste_ope_revenus )
		{
		    pointeur_tmp = liste_ope_revenus;

		    ligne = etat_affiche_affiche_titre_revenus_etat ( ligne );
		}
		else
		{
		    /* il n'y a pas de revenus, on saute directement aux dépenses */

		    i++;
		    pointeur_tmp = liste_ope_depenses;

		    /* 	      s'il n'y a pas de dépenses non plus, on sort de la boucle */

		    if ( !liste_ope_depenses )
			continue;

		    ligne = etat_affiche_affiche_titre_depenses_etat ( ligne );
		}
	    }
	    else
	    {
		/* 	      on ne sépare pas les débits et les crédits, donc pas de titre, juste */
		/* une vérification qu'il y a des opérations */

		if ( liste_ope_revenus )
		    pointeur_tmp = liste_ope_revenus;
		else
		    continue;
	    }
	}


	/* on commence la boucle qui fait le tour de chaque opé */

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
			ligne = etat_affiche_affiche_categ_etat ( operation,
								  decalage_categ,
								  ligne );
			break;

		    case 2:
			ligne = etat_affiche_affiche_sous_categ_etat ( operation,
								       decalage_sous_categ,
								       ligne );

			break;

		    case 3:
			ligne = etat_affiche_affiche_ib_etat ( operation,
							       decalage_ib,
							       ligne );

			break;

		    case 4:
			ligne = etat_affiche_affiche_sous_ib_etat ( operation,
								    decalage_sous_ib,
								    ligne );

			break;

		    case 5:
			ligne = etat_affiche_affiche_compte_etat ( operation,
								   decalage_compte,
								   ligne );

			break;

		    case 6:
			ligne = etat_affiche_affiche_tiers_etat ( operation,
								  decalage_tiers,
								  ligne );
		}

		pointeur_glist = pointeur_glist -> next;
	    }


	    /* on affiche si nécessaire le total de la période */

	    ligne = etat_affiche_affiche_total_periode ( operation,
							 ligne,
							 0 );

	    /* on affiche si nécessaire le total de  l'exercice */

	    ligne = etat_affiche_affiche_total_exercice ( operation,
							 ligne,
							 0 );



	    ligne = etat_affiche_affichage_ligne_ope ( operation,
						       ligne );

	    /* on ajoute les montants que pour ceux affichés */

	    /* calcule le montant de la categ */

	    if ( etat_courant -> utilise_categ )
	    {
		if ( operation -> devise == devise_categ_etat -> no_devise )
		    montant = operation -> montant;
		else
		{
		    devise_operation = devise_par_no ( operation -> devise );

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
		nb_ope_categ_etat++;
		nb_ope_sous_categ_etat++;
	    }

	    /* calcule le montant de l'ib */

	    if ( etat_courant -> utilise_ib )
	    {
		if ( operation -> devise == devise_ib_etat -> no_devise )
		    montant = operation -> montant;
		else
		{
		    devise_operation = devise_par_no ( operation -> devise );

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
		nb_ope_ib_etat++;
		nb_ope_sous_ib_etat++;
	    }

	    /* calcule le montant du tiers */

	    if ( etat_courant -> utilise_tiers )
	    {
		if ( operation -> devise == devise_tiers_etat -> no_devise )
		    montant = operation -> montant;
		else
		{
		    devise_operation = devise_par_no ( operation -> devise );

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
		nb_ope_tiers_etat++;
	    }

	    /* calcule le montant du compte */

	    if ( etat_courant -> affiche_sous_total_compte )
	    {
		/* on modifie le montant s'il n'est pas de la devise du compte en cours */

		p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

		if ( !devise_compte_en_cours_etat
		     ||
		     DEVISE != devise_compte_en_cours_etat -> no_devise )
		    devise_compte_en_cours_etat = devise_par_no ( DEVISE );

		if ( operation -> devise == DEVISE )
		    montant = operation -> montant;
		else
		{
		    /* ce n'est pas la devise du compte, si le compte passe à l'euro et que la devise est l'euro, */
		    /* utilise la conversion du compte, */
		    /* si c'est une devise qui passe à l'euro et que la devise du compte est l'euro, utilise la conversion du compte */
		    /* sinon utilise la conversion stockée dans l'opé */

		    devise_operation = devise_par_no ( operation -> devise );

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
		nb_ope_compte_etat++;
	    }

	    /* calcule les montants totaux */

	    if ( operation -> devise == devise_generale_etat -> no_devise )
		montant = operation -> montant;
	    else
	    {
		devise_operation = devise_par_no ( operation -> devise );

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

	    total_general = total_general + montant;
	    nb_ope_general_etat++;

	    /* calcule le montant de la periode */

	    if ( etat_courant -> separation_par_plage )
	    {
		if ( operation -> devise == devise_categ_etat -> no_devise )
		    montant = operation -> montant;
		else
		{
		    devise_operation = devise_par_no ( operation -> devise );

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
		montant_periode_etat = montant_periode_etat + montant;
		nb_ope_periode_etat++;
	    }

	    /* calcule le montant de l'exo */

	    if ( etat_courant -> separation_par_exo )
	    {
		if ( operation -> devise == devise_categ_etat -> no_devise )
		    montant = operation -> montant;
		else
		{
		    devise_operation = devise_par_no ( operation -> devise );

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
		montant_exo_etat = montant_exo_etat + montant;
		nb_ope_exo_etat++;
	    }

	    total_partie = total_partie + montant;
	    nb_ope_partie_etat++;


	    changement_de_groupe_etat = 0;

	    pointeur_tmp = pointeur_tmp -> next;
	}


	/*   à la fin, on affiche les totaux des dernières lignes */

	/* on affiche le total de la période en le forçant */

	ligne = etat_affiche_affiche_total_periode ( NULL,
						     ligne,
						     1 );

	/* on affiche le total de l'exercice en le forçant */

	ligne = etat_affiche_affiche_total_exercice ( NULL,
						     ligne,
						     1 );

	ligne = etat_affiche_affiche_totaux_sous_jaccent ( GPOINTER_TO_INT ( etat_courant -> type_classement -> data ),
							   ligne );


	/* on ajoute le total de la structure racine */


	switch ( GPOINTER_TO_INT ( etat_courant -> type_classement -> data ))
	{
	    case 1:
		ligne = etat_affiche_affiche_total_categories ( ligne );
		break;

	    case 2:
		ligne = etat_affiche_affiche_total_sous_categ ( ligne );
		break;

	    case 3:
		ligne = etat_affiche_affiche_total_ib ( ligne );
		break;

	    case 4:
		ligne = etat_affiche_affiche_total_sous_ib ( ligne );
		break;

	    case 5:
		ligne = etat_affiche_affiche_total_compte ( ligne );
		break;

	    case 6:
		ligne = etat_affiche_affiche_total_tiers ( ligne );
		break;
	}

	/* on affiche le total de la partie en cours */
	/* si les revenus et dépenses ne sont pas mÃ©langés */

	if ( etat_courant -> separer_revenus_depenses )
	    ligne = etat_affiche_affiche_total_partiel ( total_partie,
							 ligne,
							 i );

    }

    /* on affiche maintenant le total génÃ©ral */

    ligne = etat_affiche_affiche_total_general ( total_general,
						 ligne );

    etat_affiche_finish ();
}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* met tous les anciens_x_etat sous jaccents à l'origine à -1 */
/*****************************************************************************************************/

void denote_struct_sous_jaccentes ( gint origine )
{
    GList *pointeur_glist;

    /* on peut partir du bout de la liste pour revenir vers la structure demandée */
    /* gros vulgaire copier coller de la fonction précédente */

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


/* Local Variables: */
/* c-basic-offset: 4 */
/* End: */
