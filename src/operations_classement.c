/* ************************************************************************** */
/* Contient toutes les fonctions utilis�es pour classer la liste des op�      */
/* 			classement_liste.c                                    */
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

#include "include.h"


/*START_INCLUDE*/
#include "operations_classement.h"
#include "devises.h"
#include "exercice.h"
#include "utils.h"
#include "categories_onglet.h"
#include "imputation_budgetaire.h"
#include "equilibrage.h"
#include "tiers_onglet.h"
#include "type_operations.h"
/*END_INCLUDE*/

/*START_STATIC*/
static gint classement_sliste_par_auto_man ( struct structure_operation *operation_1,
				      struct structure_operation *operation_2 );
static gint classement_sliste_par_devise ( struct structure_operation *operation_1,
				     struct structure_operation *operation_2 );
/*END_STATIC*/



/*START_EXTERN*/
extern gpointer **p_tab_nom_de_compte;
extern gpointer **p_tab_nom_de_compte_variable;
/*END_EXTERN*/



/* ************************************************************************** */
/* classement par no d'op� (donc d'entr�e)                                    */
/* FIXME : encore utilis� par les �ch�ances */
/* ************************************************************************** */
gint classement_liste_par_no_ope_ventil ( GtkWidget *liste,
					  GtkCListRow *ligne_1,
					  GtkCListRow *ligne_2 )
{
    struct operation_echeance *operation_1;
    struct operation_echeance *operation_2;

    operation_1 = ligne_1 -> data;
    operation_2 = ligne_2 -> data;

    if ( operation_1 == GINT_TO_POINTER ( -1 ) )
	return ( 1 );

    if ( operation_2 == GINT_TO_POINTER ( -1 ) )
	return ( -1 );

    if ( operation_1 == NULL )
	return ( 1 );

    if ( operation_2 == NULL )
	return ( -1 );

    return ( operation_1 -> no_operation - operation_2 -> no_operation );
}
/* ************************************************************************** */


/* ************************************************************************** */
/* Fonction par d�faut : par ordre de date                                    */
/* appel�e aussi en fin de classement pour tout classement */
/* classe par date ou date de valeur suivant la conf, puis par no d'op� */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* et classe en fonction de r/p si demand� (FIXME : virer dans l'instable) */
/* ************************************************************************** */
gint classement_sliste_par_date ( struct structure_operation *operation_1,
				  struct structure_operation *operation_2 )
{
    gint retour;

    if ( etat.classement_par_date )
	/* on classe par dates normales */
	retour = g_date_compare ( operation_1 -> date, operation_2 -> date );
    else
    {
	/* on classe par date de valeur, si elle existe */

	if ( operation_1 -> date_bancaire )
	{
	    if ( operation_2 -> date_bancaire )
		retour = g_date_compare ( operation_1 -> date_bancaire, operation_2 -> date_bancaire );
	    else
		retour = g_date_compare ( operation_1 -> date_bancaire, operation_2 -> date );
	}
	else
	{
	    if ( operation_2 -> date_bancaire )
		retour = g_date_compare ( operation_1 -> date, operation_2 -> date_bancaire );
	    else
		retour = g_date_compare ( operation_1 -> date, operation_2 -> date );
	}

	/* on a class� par date de valeur, si c'est la m�me date,
	   alors on classe par date d'op�ration */

	if ( !retour )
	    retour = g_date_compare ( operation_1 -> date, operation_2 -> date );
    }

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;

    if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_no ( operation_1,
					    operation_2 ));
}
/* ************************************************************************** */

/* ************************************************************************** */
/* classement op�rations par no */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_no ( struct structure_operation *operation_1,
				struct structure_operation *operation_2 )
{
    gint retour;

    retour = operation_1 -> no_operation - operation_2 -> no_operation;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;

    return ( retour );
}
/* ************************************************************************** */



/* ************************************************************************** */
/* classement op�rations R -> T -> P */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_pointage ( struct structure_operation *operation_1,
				     struct structure_operation *operation_2 )
{
    gint retour;

    retour = operation_2 -> pointe - operation_1 -> pointe;

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;

    if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
	    
}
/* ************************************************************************** */


/* ************************************************************************** */
/* classement op�rations par debit */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_debit ( struct structure_operation *operation_1,
				   struct structure_operation *operation_2 )
{
    gint retour;

/*     on place les d�bits en premier, et par ordre croissant */
/*     si la devise est identique, easy c'est direct, sinon on doit transformer les montant */
/*     dans la devise du compte */

    if ( operation_1 -> devise == operation_2 -> devise )
	retour = operation_1 -> montant - operation_2 -> montant;
    else
    {
	gdouble montant_1, montant_2;

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1 -> no_compte;

	montant_1 = calcule_montant_devise_renvoi ( operation_1 -> montant,
						    DEVISE,
						    operation_1 -> devise,
						    operation_1 -> une_devise_compte_egale_x_devise_ope,
						    operation_1 -> taux_change,
						    operation_1 -> frais_change );
	montant_2 = calcule_montant_devise_renvoi ( operation_2 -> montant,
						    DEVISE,
						    operation_2 -> devise,
						    operation_2 -> une_devise_compte_egale_x_devise_ope,
						    operation_2 -> taux_change,
						    operation_2 -> frais_change );
	retour = montant_1 - montant_2;
    }

     p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;

   if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
	    
}
/* ************************************************************************** */



/* ************************************************************************** */
/* classement op�rations par credit */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_credit ( struct structure_operation *operation_1,
				    struct structure_operation *operation_2 )
{
    gint retour;

/*     on place les cr�dits en premier et par ordre croissant */
/*     si la devise est identique, easy c'est direct, sinon on doit transformer les montant */
/*     dans la devise du compte */

    if ( operation_2 -> devise == operation_1 -> devise )
	retour = operation_1 -> montant - operation_2 -> montant;
    else
    {
	gdouble montant_1, montant_2;

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1 -> no_compte;

	montant_1 = calcule_montant_devise_renvoi ( operation_1 -> montant,
						    DEVISE,
						    operation_1 -> devise,
						    operation_1 -> une_devise_compte_egale_x_devise_ope,
						    operation_1 -> taux_change,
						    operation_1 -> frais_change );
	montant_2 = calcule_montant_devise_renvoi ( operation_2 -> montant,
						    DEVISE,
						    operation_2 -> devise,
						    operation_2 -> une_devise_compte_egale_x_devise_ope,
						    operation_2 -> taux_change,
						    operation_2 -> frais_change );
	retour = montant_1 - montant_2;
    }

     p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;

   if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
	    
}
/* ************************************************************************** */





/* ************************************************************************** */
/* classement op�rations par montant */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_montant ( struct structure_operation *operation_1,
				     struct structure_operation *operation_2 )
{
    gint retour;

/*     on classe cette fois par valeur absolue, du plus petit au plus grand */
/*     si la devise est identique, easy c'est direct, sinon on doit transformer les montant */
/*     dans la devise du compte */

    if ( operation_1 -> devise == operation_2 -> devise )
	retour = fabs(operation_1 -> montant) - fabs(operation_2 -> montant);
    else
    {
	gdouble montant_1, montant_2;

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1 -> no_compte;

	montant_1 = calcule_montant_devise_renvoi ( operation_1 -> montant,
						    DEVISE,
						    operation_1 -> devise,
						    operation_1 -> une_devise_compte_egale_x_devise_ope,
						    operation_1 -> taux_change,
						    operation_1 -> frais_change );
	montant_2 = calcule_montant_devise_renvoi ( operation_2 -> montant,
						    DEVISE,
						    operation_2 -> devise,
						    operation_2 -> une_devise_compte_egale_x_devise_ope,
						    operation_2 -> taux_change,
						    operation_2 -> frais_change );
	retour = fabs (montant_1) - fabs(montant_2);
    }

     p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;

   if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
	    
}
/* ************************************************************************** */



/* ************************************************************************** */
/* classement op�rations par devise */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_devise ( struct structure_operation *operation_1,
				     struct structure_operation *operation_2 )
{
    gint retour;
    gchar *devise_1, *devise_2;

    devise_1 = devise_code_by_no ( operation_1 -> devise );
    devise_2 = devise_code_by_no ( operation_2 -> devise );

    if ( devise_1 )
    {
	if ( devise_2 )
	    retour = my_strcmp ( devise_1,
			      devise_2 );
	else
	    retour = -1;
    }
    else
    {
	if ( devise_2 )
	    retour = 1;
	else
	    retour = 0;
    }

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;

    if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
	    
}
/* ************************************************************************** */



/* ************************************************************************** */
/* classement op�rations par tiers */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_tiers ( struct structure_operation *operation_1,
				     struct structure_operation *operation_2 )
{
    gint retour;
    gchar *tiers_1, *tiers_2;

    tiers_1 = tiers_name_by_no ( operation_1 -> tiers, TRUE );
    tiers_2 = tiers_name_by_no ( operation_2 -> tiers, TRUE );

    if ( tiers_1 )
    {
	if ( tiers_2 )
	    retour = my_strcmp ( tiers_1,
				 tiers_2 );
	else
	    retour = -1;
    }
    else
    {
	if ( tiers_2 )
	    retour = 1;
	else
	    retour = 0;
    }

     p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;

   if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
	    
}
/* ************************************************************************** */



/* ************************************************************************** */
/* classement op�rations par cat�gories  */
/* categ -> ventil -> virements -> pas de categ */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_categories ( struct structure_operation *operation_1,
					struct structure_operation *operation_2 )
{
    gint retour;
    gchar *categ_1, *categ_2;

    categ_1 = nom_categ_par_no ( operation_1 -> categorie,
				 operation_1 -> sous_categorie );
    categ_2 = nom_categ_par_no ( operation_2 -> categorie,
				 operation_2 -> sous_categorie );

    if ( categ_1 )
    {
	if ( categ_2 )
	    /* 	    cas le plus simple : ce sont 2 categ normales */
	    retour = my_strcmp ( categ_1,
			      categ_2 );
	else
	    retour = -1;
    }
    else
    {
	if ( categ_2 )
	    retour = 1;
	else
	{
	    /* 	    cas le plus compliq� : les op�s sont soit virement, soit ventil */
	    /* 	    on va placer les ventils avant les virements */
	    if ( operation_1 -> operation_ventilee )
	    {
		if ( operation_2 -> operation_ventilee )
		    /* 		    les 2 op�s sont des ventils */
		    retour = 0;
		else
		    retour = -1;
	    }
	    else
	    {
		if ( operation_2 -> operation_ventilee )
		    retour = 1;
		else
		{
		    /* 		    aucune des 2 ne sont des ventils */

		    if ( operation_1 -> relation_no_operation )
		    {
			if ( operation_2 -> relation_no_operation )
			    /* 			    toutes les 2 sont des virements */
			    retour = 0;
			else
			    retour = -1;
		    }
		    else
			if ( operation_2 -> relation_no_operation )
			    retour = 1;
			else
			    /* 			    ce sont 2 op�s sans categ */
			    retour = 0;
		}
	    }
	}
    }
	    
     p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;

   if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
	    
}
/* ************************************************************************** */



/* ************************************************************************** */
/* classement op�rations par imputation  */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_imputation ( struct structure_operation *operation_1,
					struct structure_operation *operation_2 )
{
    gint retour;
    gchar *ib_1, *ib_2;

    ib_1 = nom_imputation_par_no ( operation_1 -> imputation,
				   operation_1 -> sous_imputation );
    ib_2 = nom_imputation_par_no ( operation_2 -> imputation,
				   operation_2 -> sous_imputation );

    if ( ib_1 )
    {
	if ( ib_2 )
	    retour = my_strcmp ( ib_1,
			      ib_2 );
	else
	    retour = -1;
    }
    else
    {
	if ( ib_2 )
	    retour = 1;
	else
	    retour = 0;
    }

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;


    if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
	    
}
/* ************************************************************************** */



/* ************************************************************************** */
/* classement op�rations par notes */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_notes ( struct structure_operation *operation_1,
				   struct structure_operation *operation_2 )
{
    gint retour;

    if ( operation_1 -> notes )
    {
	if ( operation_2 -> notes )
	    retour = my_strcasecmp ( operation_1 -> notes,
				    operation_2 -> notes );
	else
	    retour = -1;
    }
    else
    {
	if ( operation_2 -> notes )
	    retour = 1;
	else
	    retour = 0;
    }
    
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;


    if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
	    
}
/* ************************************************************************** */



/* ************************************************************************** */
/* classement op�rations par auto/man */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* on met les op�rations automatiques en 1er */
/* ************************************************************************** */
gint classement_sliste_par_auto_man ( struct structure_operation *operation_1,
				      struct structure_operation *operation_2 )
{
    gint retour;

    retour = operation_2 ->  auto_man - operation_1 -> auto_man;

     p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;

   if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
	    
}
/* ************************************************************************** */



/* ************************************************************************** */
/* classement op�rations par no de rapprochement */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_no_rapprochement ( struct structure_operation *operation_1,
					      struct structure_operation *operation_2 )
{
    gint retour;
    gchar *rapr_1, *rapr_2;

    rapr_1 = rapprochement_name_by_no ( operation_1 -> no_rapprochement );
    rapr_2 = rapprochement_name_by_no ( operation_2 -> no_rapprochement );

    if ( rapr_1 )
    {
	if ( rapr_2 )
	    retour = my_strcmp ( rapr_1, 
			      rapr_2 );
	else
	    retour = -1;
    }
    else
    {
	if ( rapr_2 )
	    retour = 1;
	else
	    retour = 0;
    }

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;


    if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
	    
}
/* ************************************************************************** */



/* ************************************************************************** */
/* classement op�rations par exercice */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_exercice ( struct structure_operation *operation_1,
				      struct structure_operation *operation_2 )
{
    gint retour;

    if ( operation_1 -> no_exercice == operation_2 -> no_exercice )
	retour = 0;
    else
    {
	if ( operation_1 -> no_exercice )
	{
	    if ( operation_2 -> no_exercice )
	    {
		struct struct_exercice *exo_1, *exo_2;

		exo_1 = exercice_par_no ( operation_1 -> no_exercice );
		exo_2 = exercice_par_no ( operation_2 -> no_exercice );

		if ( exo_1 )
		{
		    if ( exo_2 )
			retour = g_date_compare ( exo_1 -> date_debut,
						  exo_2 -> date_debut );
		    else
			retour = -1;
		}
		else
		{
		    if ( exo_2 )
			retour = 1;
		    else
			retour = 0;
		}
	    }
	    else
		retour = -1;
	}
	else
	{
	    if ( operation_2 -> no_exercice )
		retour = 1;
	    else
		retour = 0;
	}
    }

    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;


    if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
	    
}
/* ************************************************************************** */



/* ************************************************************************** */
/* classement op�rations par piece comptable  */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_pc ( struct structure_operation *operation_1,
				struct structure_operation *operation_2 )
{
    gint retour;

    if ( operation_1 -> no_piece_comptable )
    {
	if ( operation_2 -> no_piece_comptable )
	    retour = my_strcasecmp ( operation_1 -> no_piece_comptable,
				    operation_2 -> no_piece_comptable );
	else
	    retour = -1;
    }
    else
    {
	if ( operation_2 -> no_piece_comptable )
	    retour = 1;
	else
	    retour = 0;
    }
    
   p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;


    if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
    
}
/* ************************************************************************** */



/* ************************************************************************** */
/* classement op�rations par info banque_guichet */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_ibg ( struct structure_operation *operation_1,
				 struct structure_operation *operation_2 )
{
    gint retour;

    if ( operation_1 -> info_banque_guichet )
    {
	if ( operation_2 -> info_banque_guichet )
	    retour = my_strcasecmp ( operation_1 -> info_banque_guichet,
				    operation_2 -> info_banque_guichet );
	else
	    retour = -1;
    }
    else
    {
	if ( operation_2 -> info_banque_guichet )
	    retour = 1;
	else
	    retour = 0;
    }
    

   p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;

    if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
      
}
/* ************************************************************************** */



/* ************************************************************************** */
/* classement op�rations par type d'op� */
/* retour = -1 si operation_1 doit �tre plac�e en 1er */
/* ************************************************************************** */
gint classement_sliste_par_type_ope ( struct structure_operation *operation_1,
				      struct structure_operation *operation_2 )
{
    gint retour;
    gchar *type_1, *type_2;

    if ( operation_1 -> type_ope )
    {
	if ( operation_2 -> type_ope )
	{
	    type_1 = type_ope_name_by_no ( operation_1 -> type_ope,
					   operation_1 -> no_compte );
	    type_2 = type_ope_name_by_no ( operation_2 -> type_ope,
					   operation_2 -> no_compte );

	    retour = my_strcmp ( type_1,
			      type_2 );

	    if ( !retour )
	    {
		if ( operation_1 -> contenu_type )
		{
		    if ( operation_2 -> contenu_type )
			retour = my_strcmp ( operation_1 -> contenu_type,
					  operation_2 -> contenu_type );
		    else
			retour = -1;
		}
		else
		    if ( operation_2 -> contenu_type )
			retour = 1;
	    }
	}
	else
	    retour = -1;
    }
    else
    {
	if ( operation_2 -> type_ope )
	    retour = 1;
	else
	    retour = 0;
    }

   p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1->no_compte;
    if ( CLASSEMENT_CROISSANT )
	retour = retour;
    else
	retour = -retour;

    if ( retour )
	return ( retour );
    else
	return ( classement_sliste_par_date ( operation_1, operation_2 ));
	    
}
/* ************************************************************************** */




/* ************************************************************************** */
/* classement d'une liste compos�e de chaines par ordre alphab�tique          */
/* en prenant en compte les accents                                           */
/* ************************************************************************** */
gint classe_liste_alphabetique ( gchar *string_1,
				 gchar *string_2 )
{
    /* comme la fonction g_strcasecmp met les accents derri�re,
       on magouille pour les prendre en compte */

    string_1 = g_strdup ( string_1 );
    string_1 = g_strdelimit ( string_1, "������", 'e' );
    string_1 = g_strdelimit ( string_1, "��", 'c' );
    string_1 = g_strdelimit ( string_1, "��", 'a' );
    string_1 = g_strdelimit ( string_1, "����", 'u' );
    string_1 = g_strdelimit ( string_1, "��", 'o' );
    string_1 = g_strdelimit ( string_1, "��", 'i' );

    string_2 = g_strdup ( string_2 );
    string_2 = g_strdelimit ( string_2, "������", 'e' );
    string_2 = g_strdelimit ( string_2, "��", 'c' );
    string_2 = g_strdelimit ( string_2, "��", 'a' );
    string_2 = g_strdelimit ( string_2, "����", 'u' );
    string_2 = g_strdelimit ( string_2, "��", 'o' );
    string_2 = g_strdelimit ( string_2, "��", 'i' );

    return ( g_strcasecmp ( string_1, string_2 ));
}
/* ************************************************************************** */

