/*  Fichier qui s'occupe du calcul des �tats */
/*      etats.c */

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
#include "en_tete.h"



#include "etats_gtktable.h"
#include "etats_gnomeprint.h"


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


  /*   selection des op�rations */
  /* on va mettre l'adresse des op�s s�lectionn�es dans une liste */

  liste_opes_selectionnees = recupere_opes_etat ( etat );


  /*   � ce niveau, on a r�cup�r� toutes les op�s qui entreront dans */
  /* l'�tat ; reste plus qu'� les classer et les afficher */
  /* on classe la liste et l'affiche en fonction du choix du type de classement */

  etape_finale_affichage_etat ( liste_opes_selectionnees,
				affichage );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* cette fontion prend en argument l'adr d'un �tat, fait le tour de tous les comptes et */
/* s�lectionne les op�rations qui appartiennent � cet �tat. elle renvoie une liste des */
/* adr de ces op�rations */
/* elle est appel�e pour l'affichage d'un �tat ou pour la r�cup�ration des tiers d'un �tat */
/*****************************************************************************************************/

GSList *recupere_opes_etat ( struct struct_etat *etat )
{
  GSList *liste_operations_etat;
  gint i;
  gint no_exercice_recherche;
  GSList *liste_tmp;

  liste_operations_etat = NULL;

  /* si on utilise l'exercice courant ou pr�c�dent, on cherche ici */
  /* le num�ro de l'exercice correspondant */

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


      /* si on veut l'exo pr�c�dent, c'est ici */

      switch ( etat -> utilise_detail_exo )
	{
	case 1:
	  /* on recherche l'exo courant */

	  if ( exo_courant )
	    no_exercice_recherche = exo_courant -> no_exercice;
	  break;

	case 2:
	  /* on recherche l'exo pr�c�dent */

	  liste_tmp = liste_struct_exercices;

	  while ( liste_tmp )
	    {
	      struct struct_exercice *exo;

	      exo = liste_tmp -> data;

	      if ( exo_courant )
		{
		  /* si l'exo est avant exo_courant et apr�s exo_precedent, c'est le nouvel exo_precedent */

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
		  /* 		      il n'y a pas d'exo courant, donc si l'exo est en date inf�rieur � la date du jour, */
		  /* et apr�s l'exo_precedent, c'est le nouvel exo pr�c�dent */

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


  /*   si on a utilis� "le plus grand" dans la recherche de texte, c'est ici qu'on recherche */
  /*     les plus grands no de chq, de rappr et de pc dans les comptes choisis */

  /* commence par rechercher si on a utilis� "le plus grand" */

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
	      /* on commence par v�rifier que le compte fait partie de l'�tat */

	      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

	      if ( !etat -> utilise_detail_comptes
		   ||
		   g_slist_index ( etat -> no_comptes,
				   GINT_TO_POINTER ( i )) != -1 )
		{
		  GSList *pointeur_tmp;

		  /* on fait le tour de la liste des op�s en recherchant le plus grand ds les 3 variables */

		  pointeur_tmp = LISTE_OPERATIONS;

		  while ( pointeur_tmp )
		    {
		      struct structure_operation *operation;

		      operation = pointeur_tmp -> data;

		      /* commence par le cheque, il faut que le type op� soit � incr�mentation auto */
		      /* et le no le plus grand */
		      /* on ne recherche le type d'op� que si l'op� a un contenu du type et que celui ci */
		      /* est > que dernier_chq */

		      if ( operation -> contenu_type 
			   &&
			   atoi ( operation -> contenu_type ) > dernier_chq
			   &&
			   operation -> type_ope )
			{
			  struct struct_type_ope *type_ope;

			  type_ope = g_slist_find_custom ( TYPES_OPES,
							   GINT_TO_POINTER ( operation -> type_ope ),
							   (GCompareFunc) recherche_type_ope_par_no ) -> data;

			  if ( type_ope -> affiche_entree
			       &&
			       type_ope -> numerotation_auto )
			    dernier_chq = atoi ( operation -> contenu_type );
			}


		      /* on r�cup�re maintenant la plus grande pc */

		      if ( operation -> no_piece_comptable
			   &&
			   atoi ( operation -> no_piece_comptable ) > dernier_pc )
			dernier_pc = atoi ( operation -> no_piece_comptable );


		      /* on r�cup�re maintenant le dernier relev� */

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
      /* on commence par v�rifier que le compte fait partie de l'�tat */

      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

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


	      /* on v�rifie ensuite si un texte est recherch� */

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


		      /* on commence par r�cup�rer le texte du champs recherch� */

		      texte = recupere_texte_test_etat ( operation,
							 comp_textes -> champ );

		      /* � ce niveau, texte est soit null, soit contient le texte dans lequel on effectue la recherche */
		      /* on v�rifie maintenant en fontion de l'op�rateur */
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

		      /* � ce niveau, ope_dans_test=1 si l'op� a pass� ce test */
		      /* il faut qu'on fasse le lien avec la ligne pr�c�dente */
			   
		      switch ( comp_textes -> lien_struct_precedente )
			{
			case -1:
			  /* 1�re ligne  */

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

		  /* on ne garde l'op�ration que si garde_ope = 1 */

		  if ( !garde_ope )
		    goto operation_refusee;
		}


	      /* on v�rifie les R */

	      if ( etat -> afficher_r )
		{
		  if ( ( etat -> afficher_r == 1
			 &&
			 operation -> pointe == 2 )
		       ||
		       ( etat -> afficher_r == 2
			 &&
			 operation -> pointe != 2 ))
		    goto operation_refusee;
		}


	      /*    v�rification du montant nul */

	      if ( etat -> exclure_montants_nuls
		   &&
		   fabs ( operation -> montant ) < 0.01 )
		goto operation_refusee;


	      /* v�rification des montants */

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

		      /* on v�rifie maintenant en fonction de la ligne de test si on garde cette op� */

		      ope_dans_premier_test = compare_montants_etat ( montant,
								      comp_montants -> montant_1,
								      comp_montants -> comparateur_1 );

		      if ( comp_montants -> lien_1_2 != 3 )
			   ope_dans_second_test = compare_montants_etat ( montant,
									  comp_montants -> montant_2,
									  comp_montants -> comparateur_2 );
		      else
			/* pour �viter les warning lors de la compil */
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

		      /* � ce niveau, ope_dans_test=1 si l'op� a pass� ce test */
		      /* il faut qu'on fasse le lien avec la ligne pr�c�dente */
			   

		      switch ( comp_montants -> lien_struct_precedente )
			{
			case -1:
			  /* 1�re ligne  */

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

		  /* on ne garde l'op�ration que si garde_ope = 1 */

		  if ( !garde_ope )
		    goto operation_refusee;

		}



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
		      if ( etat -> type_virement == 2 )
			{
			  /* on inclut l'op� que si le compte de virement n'est */
			  /* pas pr�sent dans l'�tat */
			  
			  if ( g_slist_index ( etat -> no_comptes,
					       GINT_TO_POINTER ( operation -> relation_no_compte )) != -1 )
			    goto operation_refusee;
			}
		      else
			{
			  /* on inclut l'op� que si le compte de virement est dans la liste */

			  if ( g_slist_index ( etat -> no_comptes_virements,
					       GINT_TO_POINTER ( operation -> relation_no_compte )) == -1 )
			    goto operation_refusee;

			}
		    }
		}
	      else
		{
		  /* 		  l'op� n'est pas un virement, si on doit exclure les non virement, c'est ici */

		  if ( etat -> type_virement
		       &&
		       etat -> exclure_ope_non_virement )
		    goto operation_refusee;
		}


	      /* on va maintenant v�rifier que les cat�g sont bonnes */
	      /* si on exclut les op�s sans categ, on v�rifie que c'est pas un virement ni une ventilation */

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


	      /* v�rification de l'imputation budg�taire */

	      if ( etat -> exclure_ope_sans_ib && !operation -> imputation )
		goto operation_refusee;

	      if ((etat -> utilise_detail_ib &&
		   g_slist_index(etat-> no_ib,
				 GINT_TO_POINTER(operation->imputation)) == -1) &&
		  operation -> imputation)
		goto operation_refusee;
		

	      
	      /* v�rification du tiers */

	      if ( etat -> utilise_detail_tiers
		   &&
		   g_slist_index ( etat -> no_tiers,
				   GINT_TO_POINTER ( operation -> tiers )) == -1 )
		goto operation_refusee;


	      /* v�rification du type d'op� */

	      if ( etat -> utilise_mode_paiement &&
		   operation -> type_ope)
		{
		  struct struct_type_ope *type_ope;
		  GSList *liste_tmp;

		  /* normalement p_tab... est sur le compte en cours */

		  liste_tmp = g_slist_find_custom ( TYPES_OPES,
						    GINT_TO_POINTER ( operation -> type_ope ),
						    (GCompareFunc) recherche_type_ope_par_no );

		  if ( liste_tmp )
		    type_ope = liste_tmp -> data;
		  else
		    goto operation_refusee;

		  if ( !g_slist_find_custom ( etat -> noms_modes_paiement,
					      type_ope -> nom_type,
					      (GCompareFunc) recherche_nom_dans_liste ))
		    goto operation_refusee;
		}

	      /* v�rifie la plage de date */

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
		      /* cumul � ce jour */

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
		      /* ann�e en cours */

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
		      /* mois pr�c�dent */

		      g_date_subtract_months ( date_jour,
					       1 );

		      if ( g_date_month ( date_jour ) != g_date_month ( operation -> date )
			   ||
			   g_date_year ( date_jour ) != g_date_year ( operation -> date ))
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
/* r�cup�re le texte pour faire le test sur les textes */
/*****************************************************************************************************/

gchar *recupere_texte_test_etat ( struct structure_operation *operation,
				  gint champ )
{
  gchar *texte;

  switch ( champ )
    {
    case 0:
      /* tiers  */

      if ( operation -> tiers )
	{
	  struct struct_tiers *tiers;

	  tiers = g_slist_find_custom ( liste_struct_tiers,
					GINT_TO_POINTER ( operation -> tiers ),
					(GCompareFunc) recherche_tiers_par_no ) -> data;

	  texte = tiers -> nom_tiers;
	}
      else
	texte = NULL;
      break;

    case 1:
      /* info du tiers */

      if ( operation -> tiers )
	{
	  struct struct_tiers *tiers;

	  tiers = g_slist_find_custom ( liste_struct_tiers,
					GINT_TO_POINTER ( operation -> tiers ),
					(GCompareFunc) recherche_tiers_par_no ) -> data;

	  texte = tiers -> texte;
	}
      else
	texte = NULL;
      break;

    case 2:
      /* categ */

      if ( operation -> categorie )
	{
	  struct struct_categ *categ;

	  categ = g_slist_find_custom ( liste_struct_categories,
					GINT_TO_POINTER ( operation -> categorie ),
					(GCompareFunc) recherche_categorie_par_no ) -> data;

	  texte = categ -> nom_categ;
	}
      else
	texte = NULL;
      break;

    case 3:
      /* ss-categ */

      if ( operation -> categorie
	   &&
	   operation -> sous_categorie )
	{
	  struct struct_categ *categ;
	  struct struct_sous_categ *ss_categ;

	  categ = g_slist_find_custom ( liste_struct_categories,
					GINT_TO_POINTER ( operation -> categorie ),
					(GCompareFunc) recherche_categorie_par_no ) -> data;


	  ss_categ = g_slist_find_custom ( categ -> liste_sous_categ,
					   GINT_TO_POINTER ( operation -> sous_categorie ),
					   (GCompareFunc) recherche_sous_categorie_par_no ) -> data;

	  texte = ss_categ -> nom_sous_categ;
	}
      else
	texte = NULL;
      break;

    case 4:
      /* ib */

      if ( operation -> imputation )
	{
	  struct struct_imputation *ib;

	  ib = g_slist_find_custom ( liste_struct_imputation,
				     GINT_TO_POINTER ( operation -> imputation ),
				     (GCompareFunc) recherche_imputation_par_no ) -> data;

	  texte = ib -> nom_imputation;
	}
      else
	texte = NULL;
      break;

    case 5:
      /* ss-ib */

      if ( operation -> imputation
	   &&
	   operation -> sous_imputation )
	{
	  struct struct_imputation *ib;
	  struct struct_sous_imputation *ss_ib;

	  ib = g_slist_find_custom ( liste_struct_imputation,
				     GINT_TO_POINTER ( operation -> imputation ),
				     (GCompareFunc) recherche_imputation_par_no ) -> data;

	  ss_ib = g_slist_find_custom ( ib -> liste_sous_imputation,
					GINT_TO_POINTER ( operation -> sous_imputation ),
					(GCompareFunc) recherche_sous_imputation_par_no ) -> data;

	  texte = ss_ib -> nom_sous_imputation;
	}
      else
	texte = NULL;
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

      if ( operation -> no_rapprochement )
	{
	  struct struct_no_rapprochement *rappr;

	  rappr = g_slist_find_custom ( liste_no_rapprochements,
					GINT_TO_POINTER ( operation -> no_rapprochement ),
					(GCompareFunc) recherche_no_rapprochement_par_no ) -> data;

	  texte = rappr -> nom_rapprochement;
	}
      else
	texte = NULL;
      break;

    default:
      texte = NULL;
    }

  return ( texte );
}
/*****************************************************************************************************/


/*****************************************************************************************************/
/* v�rifie si l'op� passe le test du texte */
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
/* v�rifie si l'op� passe le test du chq */
/*****************************************************************************************************/

gint verifie_chq_test_etat ( struct struct_comparaison_textes_etat *comp_textes,
			     gchar *no_chq )
{
  gint ope_dans_test;
  gint ope_dans_premier_test;
  gint ope_dans_second_test;

  /* pour �viter les warnings lors de la compil */

  ope_dans_premier_test = 0;
  ope_dans_second_test = 0;

  /*   si on cherche le plus grand, on met la valeur recherch�e � la place de montant_1 */

  if ( comp_textes -> comparateur_1 != 6 )
    ope_dans_premier_test = compare_cheques_etat ( atoi ( no_chq ),
						   comp_textes -> montant_1,
						   comp_textes -> comparateur_1 );
  else
    {
      struct struct_no_rapprochement *rapprochement;

      switch ( comp_textes -> champ )
	{
	case 8:
	  /* pc */
	  
	  ope_dans_premier_test = compare_cheques_etat ( atoi ( no_chq ),
							 dernier_pc,
							 comp_textes -> comparateur_1 );
	  break;

	case 9:
	  /* chq */

	  ope_dans_premier_test = compare_cheques_etat ( atoi ( no_chq ),
							 dernier_chq,
							 comp_textes -> comparateur_1 );
	  break;

	case 10:
	  /* rappr */
	  /* no_chq contient le nom du rapprochement de l'op�, or pour le plus grand, on cherche */
	  /* le no du rapprochement, on le cherche ici */

	  rapprochement = g_slist_find_custom ( liste_no_rapprochements,
						no_chq,
						(GCompareFunc) recherche_no_rapprochement_par_nom ) -> data;

	  ope_dans_premier_test = compare_cheques_etat ( rapprochement -> no_rapprochement,
							 dernier_no_rappr,
							 comp_textes -> comparateur_1 );
	  break;
	}
    }



  if ( comp_textes -> lien_1_2 != 3 )
    {
      if ( comp_textes -> comparateur_2 != 6 )
	ope_dans_second_test = compare_cheques_etat ( atoi ( no_chq ),
						      comp_textes -> montant_2,
						      comp_textes -> comparateur_2 );
      else
	{
	  switch ( comp_textes -> champ )
	    {
	    case 8:
	      /* pc */
	  
	      ope_dans_second_test = compare_cheques_etat ( atoi ( no_chq ),
							     dernier_pc,
							     comp_textes -> comparateur_2 );
	      break;

	    case 9:
	      /* chq */

	      ope_dans_second_test = compare_cheques_etat ( atoi ( no_chq ),
							     dernier_chq,
							     comp_textes -> comparateur_2 );
	      break;

	    case 10:
	      /* rappr */

	      ope_dans_second_test = compare_cheques_etat ( atoi ( no_chq ),
							     dernier_no_rappr,
							     comp_textes -> comparateur_2 );
	      break;
	    }
	}
    }
  else
    /* pour �viter les warning lors de la compil */
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
/* compare les 2 no de chq en fonction du comparateur donn� en argument */
/* renvoie 1 si l'op� passe le test, 0 sinon */
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
/* compare les 2 montants en fonction du comparateur donn� en argument */
/* renvoie 1 si l'op� passe le test, 0 sinon */
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
      /* n�gatif  */

      if ( montant_ope_int < 0 )
	retour = 1;
      break;

    }

  return ( retour );
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
  /* si elles sont affich�es, on classe classement demand� puis par no d'op� si identiques */
  /* sinon on repart en mettant -1 */

  if ( !pointeur )
    {
      if ( etat_courant -> afficher_opes )
	return ( classement_ope_perso_etat ( operation_1,
					     operation_2 ));
      else
	return (-1);
    }

  switch ( GPOINTER_TO_INT ( pointeur -> data ))
    {
      /* classement des cat�gories */

    case 1:

      if ( etat_courant -> utilise_categ )
	{
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
	}

      /*       les cat�gories sont identiques, passe au classement suivant */

      pointeur = pointeur -> next;
      goto classement_suivant;

      break;

      /* classement des sous cat�gories */

    case 2:

      if ( etat_courant -> utilise_categ
	   &&
	   etat_courant -> afficher_sous_categ )
	{
	  if ( operation_1 -> sous_categorie != operation_2 -> sous_categorie )
	    return ( operation_1 -> sous_categorie - operation_2 -> sous_categorie );
	}

      /*       les ss-cat�gories sont identiques, passe au classement suivant */

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
/* cette fonction est appel�e quand l'op� a �t� class� dans sa categ, ib, compte ou tiers */
/* et qu'elle doit �tre affich�e ; on classe en fonction de la demande de la conf ( date, no, tiers ...) */
/* si les 2 op�s sont �quivalentes � ce niveau, on classe par no d'op� */
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
      /* no op�  */

      retour = operation_1 -> no_operation - operation_2 -> no_operation;
      break;

    case 2:
      /* tiers  */

      if ( !operation_1 -> tiers
	   ||
	   !operation_2 -> tiers )
	retour = operation_2 -> tiers - operation_1 -> tiers;
      else
	{
	  struct struct_tiers *tiers_1;
	  struct struct_tiers *tiers_2;

	  tiers_1 = g_slist_find_custom ( liste_struct_tiers,
					  GINT_TO_POINTER ( operation_1 -> tiers ),
					  (GCompareFunc) recherche_tiers_par_no ) -> data;
	  tiers_2 = g_slist_find_custom ( liste_struct_tiers,
					  GINT_TO_POINTER ( operation_2 -> tiers ),
					  (GCompareFunc) recherche_tiers_par_no ) -> data;

	  retour = g_strcasecmp ( tiers_1 -> nom_tiers,
				  tiers_2 -> nom_tiers );
	}
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
	    {
	      struct struct_categ *categ_1;
	      struct struct_categ *categ_2;

	      categ_1 = g_slist_find_custom ( liste_struct_categories,
					      GINT_TO_POINTER ( operation_1 -> categorie ),
					      (GCompareFunc) recherche_categorie_par_no ) -> data;
	      categ_2 = g_slist_find_custom ( liste_struct_categories,
					      GINT_TO_POINTER ( operation_2 -> categorie ),
					      (GCompareFunc) recherche_categorie_par_no ) -> data;

	      if ( operation_1 -> categorie == operation_2 -> categorie )
		{
		  struct struct_sous_categ *ss_categ_1;
		  struct struct_sous_categ *ss_categ_2;

		  ss_categ_1 = g_slist_find_custom ( categ_1 -> liste_sous_categ,
						     GINT_TO_POINTER ( operation_1 -> sous_categorie ),
						     (GCompareFunc) recherche_sous_categorie_par_no ) -> data;
		  ss_categ_2 = g_slist_find_custom ( categ_2 -> liste_sous_categ,
						     GINT_TO_POINTER ( operation_2 -> sous_categorie ),
						     (GCompareFunc) recherche_sous_categorie_par_no ) -> data;

		  retour = g_strcasecmp ( ss_categ_1 -> nom_sous_categ,
					  ss_categ_2 -> nom_sous_categ );
		}
	      else
		retour = g_strcasecmp ( categ_1 -> nom_categ,
					categ_2 -> nom_categ );
	    }
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
	    {
	      struct struct_imputation *imputation_1;
	      struct struct_imputation *imputation_2;

	      imputation_1 = g_slist_find_custom ( liste_struct_imputation,
						   GINT_TO_POINTER ( operation_1 -> imputation ),
						   (GCompareFunc) recherche_imputation_par_no ) -> data;
	      imputation_2 = g_slist_find_custom ( liste_struct_imputation,
						   GINT_TO_POINTER ( operation_2 -> imputation ),
						   (GCompareFunc) recherche_imputation_par_no ) -> data;

	      if ( operation_1 -> imputation == operation_2 -> imputation )
		{
		  struct struct_sous_imputation *ss_imputation_1;
		  struct struct_sous_imputation *ss_imputation_2;

		  ss_imputation_1 = g_slist_find_custom ( imputation_1 -> liste_sous_imputation,
							  GINT_TO_POINTER ( operation_1 -> sous_imputation ),
							  (GCompareFunc) recherche_sous_imputation_par_no ) -> data;
		  ss_imputation_2 = g_slist_find_custom ( imputation_2 -> liste_sous_imputation,
							  GINT_TO_POINTER ( operation_2 -> sous_imputation ),
							  (GCompareFunc) recherche_sous_imputation_par_no ) -> data;

		  retour = g_strcasecmp ( ss_imputation_1 -> nom_sous_imputation,
					  ss_imputation_2 -> nom_sous_imputation );
		}
	      else
		retour = g_strcasecmp ( imputation_1 -> nom_imputation,
					imputation_2 -> nom_imputation );
	    }
	}
      break;

    case 5:
      /* note si une des 2 op�s n'a pas de notes, elle va en 2�me */

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
	  /* les op�s peuvent provenir de 2 comptes diff�rents, il faut donc trouver les 2 types dans les */
	  /* listes diff�rentes */

	  struct struct_type_ope *type_1;
	  struct struct_type_ope *type_2;

	  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_1 -> no_compte;

	  type_1 = g_slist_find_custom ( TYPES_OPES,
					 GINT_TO_POINTER ( operation_1 -> type_ope ),
					 (GCompareFunc) recherche_type_ope_par_no ) -> data;

	  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_2 -> no_compte;

	  type_2 = g_slist_find_custom ( TYPES_OPES,
					 GINT_TO_POINTER ( operation_2 -> type_ope ),
					 (GCompareFunc) recherche_type_ope_par_no ) -> data;

	  retour = g_strcasecmp ( type_1 -> nom_type,
				  type_2 -> nom_type );
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
	{
	  struct struct_no_rapprochement *rappr_1;
	  struct struct_no_rapprochement *rappr_2;

	  rappr_1 = g_slist_find_custom ( liste_no_rapprochements,
					GINT_TO_POINTER ( operation_1 -> no_rapprochement ),
					(GCompareFunc) recherche_no_rapprochement_par_no ) -> data;
	  rappr_2 = g_slist_find_custom ( liste_no_rapprochements,
					GINT_TO_POINTER ( operation_2 -> no_rapprochement ),
					(GCompareFunc) recherche_no_rapprochement_par_no ) -> data;

	retour = g_strcasecmp ( rappr_1 -> nom_rapprochement,
				rappr_2 -> nom_rapprochement );
	}
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


  /*   soit on s�pare en revenus et d�penses, soit non */

  liste_ope_revenus = NULL;
  liste_ope_depenses = NULL;
  pointeur_tmp = ope_selectionnees;
  pointeur_glist = etat_courant -> type_classement;

  if ( etat_courant -> separer_revenus_depenses )
    {
      /* on commence par s�parer la liste revenus et de d�penses */
      /*   si le classement racine est la cat�gorie, on s�pare par cat�gorie */
      /* de revenu ou de d�pense */
      /* si c'est un autre, on s�pare par montant positif ou n�gatif */

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
    }
  else
    /*     on ne veut pas s�parer en revenus et d�penses, on garde juste la liste d'op� telle quelle */

    liste_ope_revenus = ope_selectionnees;


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
      nb_colonnes = nb_colonnes + etat_courant -> afficher_ib_ope;
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

  nb_colonnes = nb_colonnes + 3;
  ligne_debut_partie = -1;

  nom_categ_en_cours = NULL;
  nom_ss_categ_en_cours = NULL;
  nom_ib_en_cours = NULL;
  nom_ss_ib_en_cours = NULL;
  nom_compte_en_cours = NULL;
  nom_tiers_en_cours = NULL;

  if (! affichage -> init (ope_selectionnees))
    return;


  /* on commence � remplir le tableau */

  /* on met le titre */

  total_general = 0;
  nb_ope_general_etat = 0;
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
      total_partie = 0;
      date_debut_periode = NULL;

      nb_ope_categ_etat = 0;
      nb_ope_sous_categ_etat = 0;
      nb_ope_ib_etat = 0;
      nb_ope_sous_ib_etat = 0;
      nb_ope_compte_etat = 0;
      nb_ope_tiers_etat = 0;
      nb_ope_partie_etat = 0;
      nb_ope_periode_etat = 0;

      changement_de_groupe_etat = 0;
      debut_affichage_etat = 1;
      devise_compte_en_cours_etat = NULL;

      /* on met ici le pointeur sur les revenus ou sur les d�penses */
      /* en v�rifiant qu'il y en a */

      if ( i )
	{
	  /* c'est le second passage */
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
      else
	{
	  /* c'est le premier passage */
	  /* on met le pointeur sur les revenus */
	  /* si on n'a pas demand� de s�parer les d�bits et cr�dits, on ne met pas de titre */

	  if ( etat_courant -> separer_revenus_depenses )
	    {
	      /* on s�pare les revenus des d�bits */

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

		  /* 	      s'il n'y a pas de d�penses non plus, on sort de la boucle */
 
		  if ( !liste_ope_depenses )
		    goto fin_boucle_affichage_etat;

		  ligne = affichage -> affiche_titre_depenses_etat ( ligne );
		}
	    }
	  else
	    {
	      /* 	      on ne s�pare pas les d�bits et les cr�dits, donc pas de titre, juste */
	      /* une v�rification qu'il y a des op�rations */

	      if ( liste_ope_revenus )
		pointeur_tmp = liste_ope_revenus;
	      else
		goto fin_boucle_affichage_etat;
	    }
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


	  /* on affiche si n�cessaire le total de la p�riode */

	  ligne = gtktable_affiche_total_periode ( operation,
						   ligne,
						   0 );


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
	      nb_ope_compte_etat++;
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

	  total_general = total_general + montant;
	  nb_ope_general_etat++;

	  /* calcule le montant de la periode */

	  if ( etat_courant -> separation_par_plage )
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
	      montant_periode_etat = montant_periode_etat + montant;
	      nb_ope_periode_etat++;
	    }

	  total_partie = total_partie + montant;
	  nb_ope_partie_etat++;

	  changement_de_groupe_etat = 0;

	  pointeur_tmp = pointeur_tmp -> next;
	}


      /*   � la fin, on affiche les totaux des derni�res lignes */

      /* on affiche le total de la p�riode en le for�ant */

      ligne = gtktable_affiche_total_periode ( NULL,
					       ligne,
					       1 );

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
       /* si les revenus et d�penses ne sont pas m�lang�s */

       if ( etat_courant -> separer_revenus_depenses )
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

