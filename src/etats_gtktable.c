/*  Fichier qui s'occupe d'afficher les �tats via une gtktable */
/*      etats_gtktable.c */

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

struct struct_etat_affichage gtktable_affichage = {
  gtktable_initialise,
  gtktable_affiche_titre,
  gtktable_affiche_separateur,
  gtktable_affiche_total_categories,
  gtktable_affiche_total_sous_categ,
  gtktable_affiche_total_ib,
  gtktable_affiche_total_sous_ib,
  gtktable_affiche_total_compte,
  gtktable_affiche_total_tiers,
  gtktable_affichage_ligne_ope,
  gtktable_affiche_total_partiel,
  gtktable_affiche_total_general,
  gtktable_affiche_categ_etat,
  gtktable_affiche_sous_categ_etat,
  gtktable_affiche_ib_etat,
  gtktable_affiche_sous_ib_etat,
  gtktable_affiche_compte_etat,
  gtktable_affiche_tiers_etat,
  gtktable_affiche_titre_revenus_etat,
  gtktable_affiche_titre_depenses_etat,
  gtktable_affiche_totaux_sous_jaccent,
  gtktable_affiche_titres_colonnes,
  gtktable_finish
};

/*****************************************************************************************************/
gint gtktable_initialise (GSList * opes_selectionnees)
{
  /* on peut maintenant cr�er la table */
  /* pas besoin d'indiquer la hauteur, elle grandit automatiquement */

  table_etat = gtk_table_new ( 0,
			       nb_colonnes,
			       FALSE );
  gtk_table_set_col_spacings ( GTK_TABLE ( table_etat ),
			       5 );
  if ( GTK_BIN ( scrolled_window_etat ) -> child )
    gtk_container_remove ( GTK_CONTAINER ( scrolled_window_etat ),
			   GTK_BIN ( scrolled_window_etat ) -> child );

  gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW ( scrolled_window_etat ),
					  table_etat );
  gtk_widget_show ( table_etat );
  
  return 1;
}
/*****************************************************************************************************/


/*****************************************************************************************************/
gint gtktable_affiche_titre ( gint ligne )
{
  GtkWidget *label;
  gchar *titre;
  GDate *date_jour;
  GSList *liste_tmp;

  date_jour = g_date_new ();
  g_date_set_time ( date_jour,
		    time ( NULL ));

  titre = etat_courant -> nom_etat;


  if ( etat_courant -> exo_date )
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


      switch ( etat_courant -> utilise_detail_exo )
	{
	case 0:
	  /* tous les exos */

	  titre = g_strconcat ( titre,
				", tous les exercices",
				NULL );
	  break;

	case 1:
	  /* exercice courant */

	  if ( exo_courant )
	    titre = g_strconcat ( titre,
				  ", exercice courant (",
				  exo_courant -> nom_exercice,
				  ")",
				  NULL );
	  else
	    titre = g_strconcat ( titre,
				  ", exercice courant",
				  NULL );
	  break;

	case 2:
	  /* exo pr�c�dent */

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

	  /* 	  � ce niveau, exo_precedent contient l'exercice pr�c�dent ou NULL */

	  if ( exo_precedent )
	    titre = g_strconcat ( titre,
				  ", exercice pr�c�dent (",
				  exo_precedent -> nom_exercice,
				  ")",
				  NULL );
	  else
	    titre = g_strconcat ( titre,
				  ", exercice pr�c�dent",
				  NULL );
	  break;
	
	case 3:
	  /* exos perso */
	  /* 	  un ou plusieurs exos ont �t� s�lectionn�s, on r�cup�re le nom de chacuns */

	  liste_tmp = etat_courant -> no_exercices;

	  if ( g_slist_length ( liste_tmp ) > 1 )
	    titre = g_strconcat ( titre,
				  ", exercices ",
				  NULL );
	  else
	    titre = g_strconcat ( titre,
				  ", exercice ",
				  NULL );

	  while ( liste_tmp )
	    {
	      exo = g_slist_find_custom ( liste_struct_exercices,
					  liste_tmp -> data,
					  (GCompareFunc) recherche_exercice_par_no ) -> data;

	      if ( liste_tmp == g_slist_last ( etat_courant -> no_exercices ))
		titre = g_strconcat ( titre,
				      exo -> nom_exercice,
				      NULL );
	      else
		titre = g_strconcat ( titre,
				      exo -> nom_exercice,
				      ", ",
				      NULL );
	      liste_tmp = liste_tmp -> next;
	    }



	  break;
	}
    }
  else
    {
      /* c'est une plage de dates qui a �t� entr�e */

      gchar buffer_date[15];
      gchar buffer_date_2[15];
      GDate *date_tmp;

      switch ( etat_courant -> no_plage_date )
	{
	case 0:
	  /* toutes */

	  titre = g_strconcat ( titre,
				", toutes les dates",
				NULL );
	  break;

	case 1:
	  /* plage perso */

	  if ( etat_courant -> date_perso_debut
	       &&
	       etat_courant -> date_perso_fin )
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( ", du %d/%d/%d au %d/%d/%d",
						    g_date_day ( etat_courant -> date_perso_debut ),
						    g_date_month ( etat_courant -> date_perso_debut ),
						    g_date_year ( etat_courant -> date_perso_debut ),
						    g_date_day ( etat_courant -> date_perso_fin ),
						    g_date_month ( etat_courant -> date_perso_fin ),
						    g_date_year ( etat_courant -> date_perso_fin )),
				  NULL );
	  else
	    titre = g_strconcat ( titre,
				  ", plages perso non remplies",
				  NULL );
	  break;

	case 2:
	  /* cumul � ce jour */

	  titre = g_strconcat ( titre,
				g_strdup_printf ( ", cumul au %d/%d/%d",
						  g_date_day ( date_jour ),
						  g_date_month ( date_jour ),
						  g_date_year ( date_jour )),
				NULL );
	  break;

	case 3:
	  /* mois en cours */

	  g_date_strftime ( buffer_date,
			    14,
			    "%B",
			    date_jour );

	  if ( buffer_date[0] == 'a'
	       ||
	       buffer_date[0] == 'o' )
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( ", mois d'%s %d",
						    buffer_date,
						    g_date_year ( date_jour )),
				  NULL );
	  else
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( ", mois de %s %d",
						    buffer_date,
						    g_date_year ( date_jour )),
				  NULL );
	  break;

	case 4:
	  /* ann�e en cours */

	  titre = g_strconcat ( titre,
				g_strdup_printf ( ", ann�e %d",
						  g_date_year ( date_jour )),
				NULL );
	  break;

	case 5:
	  /* cumul mensuel */

	  titre = g_strconcat ( titre,
				g_strdup_printf ( ", cumul mensuel au %d/%d/%d",
						  g_date_day ( date_jour ),
						  g_date_month ( date_jour ),
						  g_date_year ( date_jour )),
				NULL );
	  break;

	case 6:
	  /* cumul annuel */

	  titre = g_strconcat ( titre,
				g_strdup_printf ( ", cumul annuel au %d/%d/%d",
						  g_date_day ( date_jour ),
						  g_date_month ( date_jour ),
						  g_date_year ( date_jour )),
				NULL );
	  break;

	case 7:
	  /* mois pr�c�dent */

	  g_date_subtract_months ( date_jour,
				    1 );
	  g_date_strftime ( buffer_date,
			    14,
			    "%B",
			    date_jour );

	  if ( buffer_date[0] == 'a'
	       ||
	       buffer_date[0] == 'o' )
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( ", mois d'%s %d",
						    buffer_date,
						    g_date_year ( date_jour )),
				  NULL );
	  else
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( ", mois de %s %d",
						    buffer_date,
						    g_date_year ( date_jour )),
				  NULL );
	  break;

	case 8:
	  /* ann�e pr�c�dente */

	  titre = g_strconcat ( titre,
				g_strdup_printf ( ", ann�e %d",
						  g_date_year ( date_jour ) - 1),
				NULL );
	  break;

	case 9:
	  /* 30 derniers jours */

	  date_tmp = g_date_new ();
	  g_date_set_time ( date_tmp,
			    time ( NULL ));

	  g_date_subtract_days ( date_tmp,
				 30 );

	  titre = g_strconcat ( titre,
				g_strdup_printf ( ", du %d/%d/%d au %d/%d/%d",
						  g_date_day ( date_tmp ),
						  g_date_month ( date_tmp ),
						  g_date_year (date_tmp  ),
						  g_date_day ( date_jour ),
						  g_date_month ( date_jour ),
						  g_date_year ( date_jour )),
				NULL );
	  break;

	case 10:
	  /* 3 derniers mois */

	  date_tmp = g_date_new ();
	  g_date_set_time ( date_tmp,
			    time ( NULL ));
	  g_date_subtract_months ( date_tmp,
				   3 );
	  g_date_strftime ( buffer_date_2,
			    14,
			    "%B",
			    date_tmp );
	  g_date_strftime ( buffer_date,
			    14,
			    "%B",
			    date_jour );


	  if ( buffer_date_2[0] == 'a'
	       ||
	       buffer_date_2[0] == 'o' )
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( ", du mois d'%s %d ",
						    buffer_date_2,
						    g_date_year ( date_tmp )),
				  NULL );
	  else
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( ", du mois de %s %d ",
						    buffer_date_2,
						    g_date_year ( date_tmp )),
				  NULL );

	  if ( buffer_date[0] == 'a'
	       ||
	       buffer_date[0] == 'o' )
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( "au mois d'%s %d",
						    buffer_date,
						    g_date_year ( date_jour )),
				  NULL );
	  else
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( "au mois de %s %d",
						    buffer_date,
						    g_date_year ( date_jour )),
				  NULL );
	  break;

	case 11:
	  /* 6 derniers mois */

	  date_tmp = g_date_new ();
	  g_date_set_time ( date_tmp,
			    time ( NULL ));
	  g_date_subtract_months ( date_tmp,
				   6 );
	  g_date_strftime ( buffer_date_2,
			    14,
			    "%B",
			    date_tmp );
	  g_date_strftime ( buffer_date,
			    14,
			    "%B",
			    date_jour );


	  if ( buffer_date_2[0] == 'a'
	       ||
	       buffer_date_2[0] == 'o' )
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( ", du mois d'%s %d ",
						    buffer_date_2,
						    g_date_year ( date_tmp )),
				  NULL );
	  else
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( ", du mois de %s %d ",
						    buffer_date_2,
						    g_date_year ( date_tmp )),
				  NULL );

	  if ( buffer_date[0] == 'a'
	       ||
	       buffer_date[0] == 'o' )
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( "au mois d'%s %d",
						    buffer_date,
						    g_date_year ( date_jour )),
				  NULL );
	  else
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( "au mois de %s %d",
						    buffer_date,
						    g_date_year ( date_jour )),
				  NULL );
	  break;

	case 12:
	  /* 12 derniers mois */

	  date_tmp = g_date_new ();
	  g_date_set_time ( date_tmp,
			    time ( NULL ));
	  g_date_subtract_months ( date_tmp,
				   12 );
	  g_date_strftime ( buffer_date_2,
			    14,
			    "%B",
			    date_tmp );
	  g_date_strftime ( buffer_date,
			    14,
			    "%B",
			    date_jour );


	  if ( buffer_date_2[0] == 'a'
	       ||
	       buffer_date_2[0] == 'o' )
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( ", du mois d'%s %d ",
						    buffer_date_2,
						    g_date_year ( date_tmp )),
				  NULL );
	  else
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( ", du mois de %s %d ",
						    buffer_date_2,
						    g_date_year ( date_tmp )),
				  NULL );

	  if ( buffer_date[0] == 'a'
	       ||
	       buffer_date[0] == 'o' )
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( "au mois d'%s %d",
						    buffer_date,
						    g_date_year ( date_jour )),
				  NULL );
	  else
	    titre = g_strconcat ( titre,
				  g_strdup_printf ( "au mois de %s %d",
						    buffer_date,
						    g_date_year ( date_jour )),
				  NULL );
	  break;
	}
    }


  label = gtk_label_new ( titre );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     0, GTK_TABLE ( table_etat ) -> ncols,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  return 1;
}
/*****************************************************************************************************/


/*****************************************************************************************************/
gint gtktable_affiche_separateur ( gint ligne )
{
  GtkWidget * separateur;
  separateur = gtk_hseparator_new ();
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     separateur,
		     0, GTK_TABLE ( table_etat ) -> ncols,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( separateur );

  return ligne + 1;
}
/*****************************************************************************************************/


/*****************************************************************************************************/
/* affiche le total � l'endroit donn� de la table */
/* si les cat�gories sont affich�es */
/* retourne le ligne suivante de la table */
/*****************************************************************************************************/
gint gtktable_affiche_total_categories ( gint ligne )
{
  GtkWidget *separateur;
  GtkWidget *label;

  if ( etat_courant -> utilise_categ
       &&
       etat_courant -> affiche_sous_total_categ )
    {
      /* si rien n'est affich� en dessous de la cat�g, on */
      /* met le r�sultat sur la ligne de la cat�g */
      /* sinon on fait une barre et on met le r�sultat */

      if ( etat_courant -> afficher_sous_categ
	   ||
	   etat_courant -> utilise_ib
	   ||
	   etat_courant -> regroupe_ope_par_compte
	   ||
	   etat_courant -> utilise_tiers
	   ||
	   etat_courant -> afficher_opes )
	{
	  /* 	  si on affiche les op�s, on met les traits entre eux */

	  if ( etat_courant -> afficher_opes
	       &&
	       ligne_debut_partie != -1 )
	    {
	      gint i;
	      gint colonne;

	      colonne = 2;

	      for ( i=0 ; i<((nb_colonnes-2)/2) ; i++ )
		{
		  separateur = gtk_vseparator_new ();
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     separateur,
				     colonne, colonne + 1,
				     ligne_debut_partie, ligne,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( separateur );

		  colonne = colonne + 2;
		}
	      ligne_debut_partie = -1;
	    }


	  label = gtk_label_new ( "" );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols -1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;

	  separateur = gtk_hseparator_new ();
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     separateur,
			     1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( separateur );

	  ligne++;

	  if ( nom_categ_en_cours )
	    {
	      if ( etat_courant -> afficher_nb_opes )
		label = gtk_label_new ( g_strdup_printf ( _("Total %s ( %d op�rations )"),
							  nom_categ_en_cours,
							  nb_ope_categ_etat ));
	      else
		label = gtk_label_new ( g_strconcat ( POSTSPACIFY(_("Total")),
						      nom_categ_en_cours,
						      NULL ));
	    }
	  else
	    {
	      if ( etat_courant -> afficher_nb_opes )
		label = gtk_label_new ( g_strdup_printf ( COLON(_("Total Cat�gorie ( %d op�rations )")),
							    nb_ope_categ_etat));
	      else
		label = gtk_label_new ( COLON(_("Total Cat�gorie")) );
	    }
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
						    montant_categ_etat,
						    devise_categ_etat -> code_devise ));
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   1,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     GTK_TABLE ( table_etat ) -> ncols -1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;

	  label = gtk_label_new ( "" );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols -1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}
      else
	{
	  ligne--;

	  if ( etat_courant -> afficher_nb_opes )
	    label = gtk_label_new ( g_strdup_printf ( "%4.2f %s ( %d op�rations )",
						      montant_categ_etat,
						      devise_categ_etat -> code_devise,
						      nb_ope_categ_etat ));
	  else
	    label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
						      montant_categ_etat,
						      devise_categ_etat -> code_devise ));
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     GTK_TABLE ( table_etat ) -> ncols -1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}

    }

  montant_categ_etat = 0;
  nom_categ_en_cours = NULL;
  titres_affiches = 0;
  nb_ope_categ_etat = 0;

  return (ligne );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* affiche le total � l'endroit donn� de la table */
/* si les sous_categ sont affich�es */
/* retourne le ligne suivante de la table */
/*****************************************************************************************************/
gint gtktable_affiche_total_sous_categ ( gint ligne )
{
  GtkWidget *separateur;
  GtkWidget *label;

  if ( etat_courant -> utilise_categ
       &&
       etat_courant -> afficher_sous_categ
       &&
       etat_courant -> affiche_sous_total_sous_categ )
    {
      /* si rien n'est affich� en dessous de la sous_categ, on */
      /* met le r�sultat sur la ligne de la ss categ */
      /* sinon on fait une barre et on met le r�sultat */

      if ( etat_courant -> utilise_ib
	   ||
	   etat_courant -> regroupe_ope_par_compte
	   ||
	   etat_courant -> utilise_tiers
	   ||
	   etat_courant -> afficher_opes )
	{
	  /* 	  si on affiche les op�s, on met les traits entre eux */

	  if ( etat_courant -> afficher_opes
	       &&
	       ligne_debut_partie != -1 )
	    {
	      gint i;
	      gint colonne;

	      colonne = 2;

	      for ( i=0 ; i<((nb_colonnes-2)/2) ; i++ )
		{
		  separateur = gtk_vseparator_new ();
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     separateur,
				     colonne, colonne + 1,
				     ligne_debut_partie, ligne,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( separateur );

		  colonne = colonne + 2;
		}
	      ligne_debut_partie = -1;
	    }


	  label = gtk_label_new ( "" );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;

	  separateur = gtk_hseparator_new ();
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     separateur,
			     1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( separateur );

	  ligne++;

	  if ( nom_categ_en_cours
	       &&
	       nom_ss_categ_en_cours )
	    {
	      if ( etat_courant -> afficher_nb_opes )
		label = gtk_label_new (  g_strdup_printf ( COLON(_("Total %s : %s ( %d op�rations )")),
							   nom_categ_en_cours,
							   nom_ss_categ_en_cours,
							   nb_ope_sous_categ_etat ));
	      else
		label = gtk_label_new ( g_strdup_printf ( _("Total %s : %s"),
							  nom_categ_en_cours,
							  nom_ss_categ_en_cours ));
	    }
	  else
	    {
	      if ( etat_courant -> afficher_nb_opes )
		label = gtk_label_new ( g_strdup_printf ( COLON(_("Total sous-cat�gories ( %d op�rations )")),
							  nb_ope_sous_categ_etat ));
	      else
		label = gtk_label_new ( COLON(_("Total Sous-cat�gories")) );
	    }
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
						    montant_sous_categ_etat,
						    devise_categ_etat -> code_devise ));
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   1,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     GTK_TABLE ( table_etat ) -> ncols -1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  label = gtk_label_new ( "" );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}
      else
	{
	  ligne--;

	  if ( etat_courant -> afficher_nb_opes )
	    label = gtk_label_new ( g_strdup_printf ( "%4.2f %s ( %d op�rations )",
						      montant_sous_categ_etat,
						      devise_categ_etat -> code_devise,
						      nb_ope_sous_categ_etat ));
	  else
	    label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
						      montant_sous_categ_etat,
						      devise_categ_etat -> code_devise ));
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     GTK_TABLE ( table_etat ) -> ncols -1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}
    }

  montant_sous_categ_etat = 0;
  nom_ss_categ_en_cours = NULL;
  titres_affiches = 0;
  nb_ope_sous_categ_etat = 0;

  return (ligne );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* affiche le total � l'endroit donn� de la table */
/* si les ib sont affich�es */
/* retourne le ligne suivante de la table */
/*****************************************************************************************************/
gint gtktable_affiche_total_ib ( gint ligne )
{
  GtkWidget *separateur;
  GtkWidget *label;

  if ( etat_courant -> utilise_ib
       &&
       etat_courant -> affiche_sous_total_ib )
    {
      /* si rien n'est affich� en dessous de la ib, on */
      /* met le r�sultat sur la ligne de l'ib */
      /* sinon on fait une barre et on met le r�sultat */

      if ( etat_courant -> afficher_sous_ib
	   ||
	   etat_courant -> regroupe_ope_par_compte
	   ||
	   etat_courant -> utilise_tiers
	   ||
	   etat_courant -> afficher_opes )
	{
	  /* 	  si on affiche les op�s, on met les traits entre eux */

	  if ( etat_courant -> afficher_opes
	       &&
	       ligne_debut_partie != -1 )
	    {
	      gint i;
	      gint colonne;

	      colonne = 2;

	      for ( i=0 ; i<((nb_colonnes-2)/2) ; i++ )
		{
		  separateur = gtk_vseparator_new ();
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     separateur,
				     colonne, colonne + 1,
				     ligne_debut_partie, ligne,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( separateur );

		  colonne = colonne + 2;
		}
	      ligne_debut_partie = -1;
	    }


	  label = gtk_label_new ( "" );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;

	  separateur = gtk_hseparator_new ();
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     separateur,
			     1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( separateur );

	  ligne++;

	  if ( nom_ib_en_cours )
	    {
	      if ( etat_courant -> afficher_nb_opes )
		label = gtk_label_new ( g_strdup_printf ( _("Total %s ( %d op�rations )"),
							  nom_ib_en_cours,
							  nb_ope_ib_etat ));
	      else
		label = gtk_label_new ( g_strconcat ( POSTSPACIFY(_("Total")),
						      nom_ib_en_cours,
						      NULL ));
		}
	  else
	    {
	      if ( etat_courant -> afficher_nb_opes )
		label = gtk_label_new ( g_strdup_printf ( COLON(_("Total Imputations budg�taires : ( %d op�rations )")),
							  nb_ope_ib_etat ));
	      else
		label = gtk_label_new ( COLON(_("Total Imputations budg�taires")) );
	    }
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
						    montant_ib_etat,
						    devise_ib_etat -> code_devise ));
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   1,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     GTK_TABLE ( table_etat ) -> ncols -1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  label = gtk_label_new ( "" );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}
      else
	{
	  ligne--;

	  if ( etat_courant -> afficher_nb_opes )
	    label = gtk_label_new ( g_strdup_printf ( "%4.2f %s ( %d op�rations )",
						      montant_ib_etat,
						      devise_ib_etat -> code_devise,
						      nb_ope_ib_etat ));
	  else
	    label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
						      montant_ib_etat,
						      devise_ib_etat -> code_devise ));
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     GTK_TABLE ( table_etat ) -> ncols -1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}
    }

  montant_ib_etat = 0;
  nom_ib_en_cours = NULL;
  titres_affiches = 0;
  nb_ope_ib_etat = 0;

  return (ligne );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* affiche le total � l'endroit donn� de la table */
/* si les sous_ib sont affich�es */
/* retourne le ligne suivante de la table */
/*****************************************************************************************************/
gint gtktable_affiche_total_sous_ib ( gint ligne )
{
  GtkWidget *separateur;
  GtkWidget *label;

  if ( etat_courant -> utilise_ib
       &&
       etat_courant -> afficher_sous_ib
       &&
       etat_courant -> affiche_sous_total_sous_ib )
    {
      /* si rien n'est affich� en dessous de la sous ib, on */
      /* met le r�sultat sur la ligne de la sous ib */
      /* sinon on fait une barre et on met le r�sultat */

      if ( etat_courant -> regroupe_ope_par_compte
	   ||
	   etat_courant -> utilise_tiers
	   ||
	   etat_courant -> afficher_opes )
	{
	  /* 	  si on affiche les op�s, on met les traits entre eux */

	  if ( etat_courant -> afficher_opes
	       &&
	       ligne_debut_partie != -1 )
	    {
	      gint i;
	      gint colonne;

	      colonne = 2;

	      for ( i=0 ; i<((nb_colonnes-2)/2) ; i++ )
		{
		  separateur = gtk_vseparator_new ();
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     separateur,
				     colonne, colonne + 1,
				     ligne_debut_partie, ligne,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( separateur );

		  colonne = colonne + 2;
		}
	      ligne_debut_partie = -1;
	    }


	  label = gtk_label_new ( "" );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;

	  separateur = gtk_hseparator_new ();
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     separateur,
			     1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( separateur );

	  ligne++;

	  if ( nom_ib_en_cours
	       &&
	       nom_ss_ib_en_cours )
	    {
	      if ( etat_courant -> afficher_nb_opes )
		label = gtk_label_new ( g_strdup_printf ( COLON(_("Total %s : %s ( %d op�rations )")),
						      nom_ib_en_cours,
						      nom_ss_ib_en_cours,
						      nb_ope_sous_ib_etat ));
	      else
		label = gtk_label_new ( g_strdup_printf ( _("Total %s : %s"),
						      nom_ib_en_cours, nom_ss_ib_en_cours ));
	    }
	  else
	    {
	      if ( etat_courant -> afficher_nb_opes )
		label = gtk_label_new ( g_strdup_printf ( COLON(_("Total Sous-imputations budg�taires : ( %d op�rations )")),
							  nb_ope_sous_ib_etat ));
	      else
		label = gtk_label_new ( COLON(_("Total Sous-imputations budg�taires")) );
	    }
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
						    montant_sous_ib_etat,
						    devise_ib_etat -> code_devise ));
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   1,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     GTK_TABLE ( table_etat ) -> ncols -1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;

	  label = gtk_label_new ( "" );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}
      else
	{
	  ligne--;

	  if ( etat_courant -> afficher_nb_opes )
	    label = gtk_label_new ( g_strdup_printf ( "%4.2f %s ( %d op�rations )",
						      montant_sous_ib_etat,
						      devise_ib_etat -> code_devise,
						      nb_ope_sous_ib_etat ));
	  else
	    label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
						      montant_sous_ib_etat,
						      devise_ib_etat -> code_devise ));
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     GTK_TABLE ( table_etat ) -> ncols -1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}
    }

  montant_sous_ib_etat = 0;
  nom_ss_ib_en_cours = NULL;
  titres_affiches = 0;
  nb_ope_sous_ib_etat = 0;

  return (ligne );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* affiche le total � l'endroit donn� de la table */
/* si les compte sont affich�es */
/* retourne le ligne suivante de la table */
/*****************************************************************************************************/
gint gtktable_affiche_total_compte ( gint ligne )
{
  GtkWidget *separateur;
  GtkWidget *label;

  if ( etat_courant -> regroupe_ope_par_compte
       &&
       etat_courant -> affiche_sous_total_compte )
    {
      /* si rien n'est affich� en dessous du compte, on */
      /* met le r�sultat sur la ligne du compte */
      /* sinon on fait une barre et on met le r�sultat */

      if ( etat_courant -> utilise_tiers
	   ||
	   etat_courant -> afficher_opes )
	{
	  /* 	  si on affiche les op�s, on met les traits entre eux */

	  if ( etat_courant -> afficher_opes
	       &&
	       ligne_debut_partie != -1 )
	    {
	      gint i;
	      gint colonne;

	      colonne = 2;

	      for ( i=0 ; i<((nb_colonnes-2)/2) ; i++ )
		{
		  separateur = gtk_vseparator_new ();
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     separateur,
				     colonne, colonne + 1,
				     ligne_debut_partie, ligne,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( separateur );

		  colonne = colonne + 2;
		}
	      ligne_debut_partie = -1;
	    }


	  label = gtk_label_new ( "" );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;

	  separateur = gtk_hseparator_new ();
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     separateur,
			     1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( separateur );

	  ligne++;

	  if ( nom_compte_en_cours )
	    {
	      if ( etat_courant -> afficher_nb_opes )
		label = gtk_label_new ( g_strdup_printf ( _("Total %s ( %d op�rations )"),
						      nom_compte_en_cours,
						      nb_ope_compte_etat ));
	      else
		label = gtk_label_new ( g_strconcat ( POSTSPACIFY(_("Total")),
						      nom_compte_en_cours,
						      NULL ));
	    }
	  else
	    {
	      if ( etat_courant -> afficher_nb_opes )
		label = gtk_label_new ( g_strdup_printf ( COLON(_("Total Compte : ( %d op�rations )")),
							    nb_ope_compte_etat ) );
	      else
		label = gtk_label_new ( COLON(_("Total Compte")) );
	    }
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
						    montant_compte_etat,
						    devise_compte_en_cours_etat -> code_devise ));
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   1,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     GTK_TABLE ( table_etat ) -> ncols -1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;

	  label = gtk_label_new ( "" );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}
      else
	{
	  ligne--;

	  if ( etat_courant -> afficher_nb_opes )
	    label = gtk_label_new ( g_strdup_printf ( "%4.2f %s ( %d op�rations )",
						      montant_compte_etat,
						      devise_compte_en_cours_etat -> code_devise,
						      nb_ope_compte_etat ));
	  else
	    label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
						      montant_compte_etat,
						      devise_compte_en_cours_etat -> code_devise ));
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     GTK_TABLE ( table_etat ) -> ncols -1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}
    }

  montant_compte_etat = 0;
  nom_compte_en_cours = NULL;
  titres_affiches = 0;
  nb_ope_compte_etat = 0;

  return (ligne );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
/* affiche le total � l'endroit donn� de la table */
/* si les tiers sont affich�es */
/* retourne le ligne suivante de la table */
/*****************************************************************************************************/
gint gtktable_affiche_total_tiers ( gint ligne )
{
  GtkWidget *separateur;
  GtkWidget *label;

  if ( etat_courant -> utilise_tiers
       &&
       etat_courant -> affiche_sous_total_tiers )
    {
      /* si rien n'est affich� en dessous du tiers, on */
      /* met le r�sultat sur la ligne du tiers */
      /* sinon on fait une barre et on met le r�sultat */

      if ( etat_courant -> afficher_opes )
	{
	  /* 	  si on affiche les op�s, on met les traits entre eux */

	  if ( etat_courant -> afficher_opes
	       &&
	       ligne_debut_partie != -1 )
	    {
	      gint i;
	      gint colonne;

	      colonne = 2;

	      for ( i=0 ; i<((nb_colonnes-2)/2) ; i++ )
		{
		  separateur = gtk_vseparator_new ();
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     separateur,
				     colonne, colonne + 1,
				     ligne_debut_partie, ligne,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( separateur );

		  colonne = colonne + 2;
		}
	      ligne_debut_partie = -1;
	    }


	  label = gtk_label_new ( "" );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;

	  separateur = gtk_hseparator_new ();
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     separateur,
			     1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( separateur );

	  ligne++;

	  if ( nom_tiers_en_cours )
	    {
	      if ( etat_courant -> afficher_nb_opes )
		label = gtk_label_new ( g_strdup_printf ( _("Total %s ( %d op�rations )"),
							  nom_tiers_en_cours,
							  nb_ope_tiers_etat ));
	      else
		label = gtk_label_new ( g_strdup_printf ( _("Total %s"),
							  nom_tiers_en_cours ));
	    }
	  else
	    {
	      if ( etat_courant -> afficher_nb_opes )
		label = gtk_label_new ( g_strdup_printf ( COLON(_("Total Tiers : ( %d op�rations )")),
							  nb_ope_tiers_etat ));
	      else
		label = gtk_label_new ( COLON(_("Total Tiers")) );
	    }
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
						    montant_tiers_etat,
						    devise_tiers_etat -> code_devise ));
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   1,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     GTK_TABLE ( table_etat ) -> ncols -1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;

	  label = gtk_label_new ( "" );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}
      else
	{
	  ligne--;

	  if ( etat_courant -> afficher_nb_opes )
	    label = gtk_label_new ( g_strdup_printf ( "%4.2f %s ( %d op�rations )",
						      montant_tiers_etat,
						      devise_tiers_etat -> code_devise,
						      nb_ope_tiers_etat ));
	  else
	    label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
						      montant_tiers_etat,
						      devise_tiers_etat -> code_devise ));
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     GTK_TABLE ( table_etat ) -> ncols -1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}
    }

  montant_tiers_etat = 0;
  nom_tiers_en_cours = NULL;
  titres_affiches = 0;
  nb_ope_tiers_etat = 0;

  return (ligne );
}
/*****************************************************************************************************/






/*****************************************************************************************************/
/* affiche le total de la p�riode � l'endroit donn� de la table */
/* retourne le ligne suivante de la table */
/* si force = 0, v�rifie les dates et affiche si n�cessaire */
/*   si force = 1, affiche le total (chgt de categ, ib ...) */
/*****************************************************************************************************/
gint gtktable_affiche_total_periode ( struct structure_operation *operation,
				      gint ligne,
				      gint force )
{
  GtkWidget *label;
  GtkWidget *separateur;

  if ( etat_courant -> separation_par_plage )
    {
      gchar *message;

      message = NULL;

      /* si la date de d�but de p�riode est nulle, on la met au d�but de la p�riode la date de l'op�ration */

      if ( !date_debut_periode )
	{
	  if ( operation )
	    {
	      /*  il y a une op�ration, on va rechercher le d�but de la p�riode qui la contient */
	      /* �a peut �tre le d�but de la semaine, du mois ou de l'ann�e de l'op� */

	      switch ( etat_courant -> type_separation_plage )
		{
		case 0:
		  /* s�paration par semaine : on recherche le d�but de la semaine qui contient l'op� */

		  date_debut_periode = g_date_new_dmy ( g_date_day ( operation -> date ),
							g_date_month ( operation -> date ),
							g_date_year ( operation -> date ));

		  if ( g_date_weekday ( date_debut_periode )  != (etat_courant -> jour_debut_semaine + 1 ))
		    {
		      if ( g_date_weekday ( date_debut_periode ) < (etat_courant -> jour_debut_semaine + 1 ))
			g_date_subtract_days ( date_debut_periode,
					       g_date_weekday ( date_debut_periode ) + etat_courant -> jour_debut_semaine - 2 );
		      else
			g_date_subtract_days ( date_debut_periode,
					       g_date_weekday ( date_debut_periode ) - etat_courant -> jour_debut_semaine - 1 );
		    }
		  break;

		case 1:
		  /* s�paration par mois */

		  date_debut_periode = g_date_new_dmy ( 1,
							g_date_month ( operation -> date ),
							g_date_year ( operation -> date ));
		  break;

		case 2:
		  /* s�paration par an */

		  date_debut_periode = g_date_new_dmy ( 1,
							1,
							g_date_year ( operation -> date ));
		  break;
		}
	    }
	  else
	    date_debut_periode = NULL;
	  return ( ligne );
	}

      /* on v�rifie maintenant s'il faut afficher un total ou pas */

      switch ( etat_courant -> type_separation_plage )
	{
	  gchar buffer[30];
	  GDate *date_tmp;

	case 0:
	  /* s�paration par semaine */

	  /* 	  si c'est le m�me jour que l'op� pr�c�dente, on fait rien */

	  if ( !force
	       &&
	       !g_date_compare ( operation -> date,
				date_debut_periode ))
	    return ( ligne );

	  /* 	  si on est en d�but de semaine, on met un sous total */

	  date_tmp = g_date_new_dmy ( g_date_day ( date_debut_periode ),
				      g_date_month ( date_debut_periode ),
				      g_date_year ( date_debut_periode ));
	  g_date_add_days ( date_tmp,
			    7 );

	  if ( !force
	       &&
	       ( g_date_weekday ( operation -> date )  != (etat_courant -> jour_debut_semaine + 1 )
		 &&
		 g_date_compare ( operation -> date,
				  date_tmp ) < 0 ))
	    return ( ligne );

	  /* on doit retrouver la date du d�but de semaine et y ajouter 6j pour afficher la p�riode */

	  if ( g_date_weekday ( date_debut_periode )  != (etat_courant -> jour_debut_semaine + 1 ))
	    {
	      if ( g_date_weekday ( date_debut_periode ) < (etat_courant -> jour_debut_semaine + 1 ))
		g_date_subtract_days ( date_debut_periode,
				       g_date_weekday ( date_debut_periode ) + etat_courant -> jour_debut_semaine - 2 );
	      else
		g_date_subtract_days ( date_debut_periode,
				       g_date_weekday ( date_debut_periode ) - etat_courant -> jour_debut_semaine - 1 );
	    }


	  g_date_free ( date_tmp );

	  date_tmp = g_date_new_dmy ( g_date_day ( date_debut_periode ),
				      g_date_month ( date_debut_periode ),
				      g_date_year ( date_debut_periode ));
	  g_date_add_days ( date_tmp,
			    6 );
	  if ( etat_courant -> afficher_nb_opes )
	    message = g_strdup_printf ( _( "R�sultat du %d/%d/%d au %d/%d/%d (%d op�rations) :" ),
					g_date_day ( date_debut_periode ),
					g_date_month ( date_debut_periode ),
					g_date_year ( date_debut_periode ),
					g_date_day ( date_tmp ),
					g_date_month ( date_tmp ),
					g_date_year ( date_tmp ),
					nb_ope_periode_etat );
	  else
	    message = g_strdup_printf ( _( "R�sultat du %d/%d/%d au %d/%d/%d :" ),
					g_date_day ( date_debut_periode ),
					g_date_month ( date_debut_periode ),
					g_date_year ( date_debut_periode ),
					g_date_day ( date_tmp ),
					g_date_month ( date_tmp ),
					g_date_year ( date_tmp ));

	  break;

	case 1:
	  /* s�paration par mois */

	  if ( !force
	       &&
	       operation -> mois == g_date_month ( date_debut_periode ) )
	    return ( ligne );

	  g_date_strftime ( buffer,
			    29,
			    "%B %Y",
			    date_debut_periode );
			    
	  if ( etat_courant -> afficher_nb_opes )
	    message = g_strdup_printf ( COLON(_("R�sultat de %s (%d op�rations )")),
					buffer, nb_ope_periode_etat );
	  else
	    message = g_strconcat ( COLON(_("R�sultat de %s")),
				    buffer );
				      
	  break;

	case 2:
	  /* s�paration par an */

	  if ( !force
	       &&
	       operation -> annee == g_date_year ( date_debut_periode ) )
	    return ( ligne );

	  g_date_strftime ( buffer,
			    29,
			    "%Y",
			    date_debut_periode );
			    
	  if ( etat_courant -> afficher_nb_opes )
	    message = g_strdup_printf ( COLON(_("R�sultat de l'ann�e%s ( %d op�rations )")),
				    buffer, nb_ope_periode_etat );
	  else
	    message = g_strdup_printf ( COLON(_("R�sultat de l'ann�e%s")),
					buffer );
	  break;
	}

      /*       si on arrive ici, c'est qu'il y a un chgt de p�riode ou que c'est forc� */
      /* 	  si on affiche les op�s, on met les traits entre eux */

      if ( etat_courant -> afficher_opes
	   &&
	   ligne_debut_partie != -1 )
	{
	  gint i;
	  gint colonne;

	  colonne = 2;

	  for ( i=0 ; i<((nb_colonnes-2)/2) ; i++ )
	    {
	      separateur = gtk_vseparator_new ();
	      gtk_table_attach ( GTK_TABLE ( table_etat ),
				 separateur,
				 colonne, colonne + 1,
				 ligne_debut_partie, ligne,
				 GTK_SHRINK | GTK_FILL,
				 GTK_SHRINK | GTK_FILL,
				 0, 0 );
	      gtk_widget_show ( separateur );

	      colonne = colonne + 2;
	    }
	  ligne_debut_partie = -1;
	}


      label = gtk_label_new ( "" );
      gtk_misc_set_alignment ( GTK_MISC ( label ),
			       0,
			       0.5 );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 1, GTK_TABLE ( table_etat ) -> ncols - 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      ligne++;

      separateur = gtk_hseparator_new ();
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 separateur,
			 1, GTK_TABLE ( table_etat ) -> ncols,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( separateur );

      ligne++;

      label = gtk_label_new ( message );
      gtk_misc_set_alignment ( GTK_MISC ( label ),
			       0,
			       0.5 );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 1, GTK_TABLE ( table_etat ) -> ncols - 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
						montant_periode_etat,
						devise_generale_etat -> code_devise ));
      gtk_misc_set_alignment ( GTK_MISC ( label ),
			       1,
			       0.5 );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 GTK_TABLE ( table_etat ) -> ncols - 1, GTK_TABLE ( table_etat ) -> ncols,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      ligne++;

	  label = gtk_label_new ( "" );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     1, GTK_TABLE ( table_etat ) -> ncols - 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;


      montant_periode_etat = 0;
      nb_ope_periode_etat = 0;

      /* comme il y a un changement de p�riode, on remet la date_debut_periode � celle du d�but de la p�riode */
      /* de l'op�ration  en cours */


	  if ( operation )
	    {
	      /*  il y a une op�ration, on va rechercher le d�but de la p�riode qui la contient */
	      /* �a peut �tre le d�but de la semaine, du mois ou de l'ann�e de l'op� */

	      switch ( etat_courant -> type_separation_plage )
		{
		case 0:
		  /* s�paration par semaine : on recherche le d�but de la semaine qui contient l'op� */

		  date_debut_periode = g_date_new_dmy ( g_date_day ( operation -> date ),
							g_date_month ( operation -> date ),
							g_date_year ( operation -> date ));

		  if ( g_date_weekday ( date_debut_periode )  != (etat_courant -> jour_debut_semaine + 1 ))
		    {
		      if ( g_date_weekday ( date_debut_periode ) < (etat_courant -> jour_debut_semaine + 1 ))
			g_date_subtract_days ( date_debut_periode,
					       g_date_weekday ( date_debut_periode ) + etat_courant -> jour_debut_semaine - 2 );
		      else
			g_date_subtract_days ( date_debut_periode,
					       g_date_weekday ( date_debut_periode ) - etat_courant -> jour_debut_semaine - 1 );
		    }
		  break;

		case 1:
		  /* s�paration par mois */

		  date_debut_periode = g_date_new_dmy ( 1,
							g_date_month ( operation -> date ),
							g_date_year ( operation -> date ));
		  break;

		case 2:
		  /* s�paration par an */

		  date_debut_periode = g_date_new_dmy ( 1,
							1,
							g_date_year ( operation -> date ));
		  break;
		}
	    }
	  else
	    date_debut_periode = NULL;
    }

  return (ligne );

}
/*****************************************************************************************************/

/*****************************************************************************************************/
gint gtktable_affichage_ligne_ope ( struct structure_operation *operation,
				    gint ligne )
{
  gint colonne;
  GtkWidget *label;

  /* on met tous les labels dans un event_box pour aller directement sur l'op� si elle est click�e */


  if ( etat_courant -> afficher_opes )
    {
      /* on affiche ce qui est demand� pour les op�s */


      /* si les titres ne sont pas affich�s et qu'il faut le faire, c'est ici */

      if ( !titres_affiches
	   &&
	   etat_courant -> afficher_titre_colonnes
	   &&
	   etat_courant -> type_affichage_titres )
	ligne = gtktable_affichage . affiche_titres_colonnes ( ligne );

      colonne = 1;

      /*       pour chaque info, on v�rifie si on l'op� doit �tre clickable */
      /* si c'est le cas, on place le label dans un event_box */

      if ( etat_courant -> afficher_no_ope )
	{
	  label = gtk_label_new ( itoa ( operation -> no_operation ));
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );

	  if ( etat_courant -> ope_clickables )
	    {
	      GtkWidget *event_box;

	      event_box = gtk_event_box_new ();
	      gtk_signal_connect ( GTK_OBJECT ( event_box ),
				   "enter_notify_event",
				   GTK_SIGNAL_FUNC ( met_en_prelight ),
				   NULL );
	      gtk_signal_connect ( GTK_OBJECT ( event_box ),
				   "leave_notify_event",
				   GTK_SIGNAL_FUNC ( met_en_normal ),
				   NULL );
	      gtk_signal_connect_object ( GTK_OBJECT ( event_box ),
					  "button_press_event",
					  GTK_SIGNAL_FUNC ( gtktable_click_sur_ope_etat ),
					  (GtkObject *) operation );
	      gtk_table_attach ( GTK_TABLE ( table_etat ),
				 event_box,
				 colonne, colonne + 1,
				 ligne, ligne + 1,
				 GTK_SHRINK | GTK_FILL,
				 GTK_SHRINK | GTK_FILL,
				 0, 0 );
	      gtk_widget_show ( event_box );

	      gtk_widget_set_style ( label,
				     style_label_nom_compte );
	      gtk_container_add ( GTK_CONTAINER ( event_box ),
				  label );
	    }
	  else
	    gtk_table_attach ( GTK_TABLE ( table_etat ),
			       label,
			       colonne, colonne + 1,
			       ligne, ligne + 1,
			       GTK_SHRINK | GTK_FILL,
			       GTK_SHRINK | GTK_FILL,
			       0, 0 );

	  gtk_widget_show ( label );

	  colonne = colonne + 2;
	}

      if ( etat_courant -> afficher_date_ope )
	{
	  label = gtk_label_new ( g_strdup_printf  ( "%.2d/%.2d/%d",
						     operation -> jour,
						     operation -> mois,
						     operation -> annee ));
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );

	  if ( etat_courant -> ope_clickables )
	    {
	      GtkWidget *event_box;

	      event_box = gtk_event_box_new ();
	      gtk_signal_connect ( GTK_OBJECT ( event_box ),
				   "enter_notify_event",
				   GTK_SIGNAL_FUNC ( met_en_prelight ),
				   NULL );
	      gtk_signal_connect ( GTK_OBJECT ( event_box ),
				   "leave_notify_event",
				   GTK_SIGNAL_FUNC ( met_en_normal ),
				   NULL );
	      gtk_signal_connect_object ( GTK_OBJECT ( event_box ),
					  "button_press_event",
					  GTK_SIGNAL_FUNC ( gtktable_click_sur_ope_etat ),
					  (GtkObject *) operation );
	      gtk_table_attach ( GTK_TABLE ( table_etat ),
				 event_box,
				 colonne, colonne + 1,
				 ligne, ligne + 1,
				 GTK_SHRINK | GTK_FILL,
				 GTK_SHRINK | GTK_FILL,
				 0, 0 );
	      gtk_widget_show ( event_box );

	      gtk_widget_set_style ( label,
				     style_label_nom_compte );
	      gtk_container_add ( GTK_CONTAINER ( event_box ),
				  label );
	    }
	  else
	    gtk_table_attach ( GTK_TABLE ( table_etat ),
			       label,
			       colonne, colonne + 1,
			       ligne, ligne + 1,
			       GTK_SHRINK | GTK_FILL,
			       GTK_SHRINK | GTK_FILL,
			       0, 0 );


	  gtk_widget_show ( label );

	  colonne = colonne + 2;
	}

      if ( etat_courant -> afficher_exo_ope )
	{
	  if ( operation -> no_exercice )
	    {
	      label = gtk_label_new ( ((struct struct_exercice *)(g_slist_find_custom ( liste_struct_exercices,
											GINT_TO_POINTER ( operation -> no_exercice ),
											(GCompareFunc) recherche_exercice_par_no )->data)) -> nom_exercice );
	      gtk_misc_set_alignment ( GTK_MISC ( label ),
				       0,
				       0.5 );

	      if ( etat_courant -> ope_clickables )
		{
		  GtkWidget *event_box;

		  event_box = gtk_event_box_new ();
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "enter_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_prelight ),
				       NULL );
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "leave_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_normal ),
				       NULL );
		  gtk_signal_connect_object ( GTK_OBJECT ( event_box ),
					      "button_press_event",
					      GTK_SIGNAL_FUNC ( gtktable_click_sur_ope_etat ),
					      (GtkObject *) operation );
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     event_box,
				     colonne, colonne + 1,
				     ligne, ligne + 1,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( event_box );

		  gtk_widget_set_style ( label,
					 style_label_nom_compte );
		  gtk_container_add ( GTK_CONTAINER ( event_box ),
				      label );
		}
	      else
		gtk_table_attach ( GTK_TABLE ( table_etat ),
				   label,
				   colonne, colonne + 1,
				   ligne, ligne + 1,
				   GTK_SHRINK | GTK_FILL,
				   GTK_SHRINK | GTK_FILL,
				   0, 0 );

	      gtk_widget_show ( label );
	    }
	  colonne = colonne + 2;
	}


      if ( etat_courant -> afficher_tiers_ope )
	{
	  if ( operation -> tiers )
	    {
	      label = gtk_label_new ( ((struct struct_tiers *)(g_slist_find_custom ( liste_struct_tiers,
										     GINT_TO_POINTER ( operation -> tiers ),
										     (GCompareFunc) recherche_tiers_par_no )->data)) -> nom_tiers );
		      
	      gtk_misc_set_alignment ( GTK_MISC ( label ),
				       0,
				       0.5 );

	      if ( etat_courant -> ope_clickables )
		{
		  GtkWidget *event_box;

		  event_box = gtk_event_box_new ();
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "enter_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_prelight ),
				       NULL );
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "leave_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_normal ),
				       NULL );
		  gtk_signal_connect_object ( GTK_OBJECT ( event_box ),
					      "button_press_event",
					      GTK_SIGNAL_FUNC ( gtktable_click_sur_ope_etat ),
					      (GtkObject *) operation );
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     event_box,
				     colonne, colonne + 1,
				     ligne, ligne + 1,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( event_box );

		  gtk_widget_set_style ( label,
					 style_label_nom_compte );
		  gtk_container_add ( GTK_CONTAINER ( event_box ),
				      label );
		}
	      else
		gtk_table_attach ( GTK_TABLE ( table_etat ),
				   label,
				   colonne, colonne + 1,
				   ligne, ligne + 1,
				   GTK_SHRINK | GTK_FILL,
				   GTK_SHRINK | GTK_FILL,
				   0, 0 );

	      gtk_widget_show ( label );
	    }

	  colonne = colonne + 2;
	}

      if ( etat_courant -> afficher_categ_ope )
	{
	  gchar *pointeur;

	  pointeur = NULL;

	  if ( operation -> categorie )
	    {
	      struct struct_categ *categ;

	      categ = g_slist_find_custom ( liste_struct_categories,
					    GINT_TO_POINTER ( operation -> categorie ),
					    (GCompareFunc) recherche_categorie_par_no ) -> data;
	      pointeur = categ -> nom_categ;

	      if ( operation -> sous_categorie
		   &&
		   etat_courant -> afficher_sous_categ_ope )
		pointeur = g_strconcat ( pointeur,
					 " : ",
					 ((struct struct_sous_categ *)(g_slist_find_custom ( categ -> liste_sous_categ,
											     GINT_TO_POINTER ( operation -> sous_categorie ),
											     (GCompareFunc) recherche_sous_categorie_par_no ) -> data )) -> nom_sous_categ,
					 NULL );
	    }
	  else
	    {
	      /* si c'est un virement, on le marque, sinon c'est qu'il n'y a pas de categ */
	      /* ou que c'est une op� ventil�e, et on marque rien */

	      if ( operation -> relation_no_operation )
		{
		  /* c'est un virement */

		  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

		  if ( operation -> montant < 0 )
		    pointeur = g_strdup_printf ( _("Virement vers %s"),
						 NOM_DU_COMPTE );
		  else
		    pointeur = g_strdup_printf ( _("Virement de %s"),
						 NOM_DU_COMPTE );
		}
	    }

	  if ( pointeur )
	    {
	      label = gtk_label_new ( pointeur );
	      gtk_misc_set_alignment ( GTK_MISC ( label ),
				       0,
				       0.5 );

	      if ( etat_courant -> ope_clickables )
		{
		  GtkWidget *event_box;

		  event_box = gtk_event_box_new ();
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "enter_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_prelight ),
				       NULL );
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "leave_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_normal ),
				       NULL );
		  gtk_signal_connect_object ( GTK_OBJECT ( event_box ),
					      "button_press_event",
					      GTK_SIGNAL_FUNC ( gtktable_click_sur_ope_etat ),
					      (GtkObject *) operation );
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     event_box,
				     colonne, colonne + 1,
				     ligne, ligne + 1,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( event_box );

		  gtk_widget_set_style ( label,
					 style_label_nom_compte );
		  gtk_container_add ( GTK_CONTAINER ( event_box ),
				      label );
		}
	      else
		gtk_table_attach ( GTK_TABLE ( table_etat ),
				   label,
				   colonne, colonne + 1,
				   ligne, ligne + 1,
				   GTK_SHRINK | GTK_FILL,
				   GTK_SHRINK | GTK_FILL,
				   0, 0 );

	      gtk_widget_show ( label );
	    }
	  colonne = colonne + 2;
	}
		


      if ( etat_courant -> afficher_ib_ope )
	{
	  if ( operation -> imputation )
	    {
	      struct struct_imputation *ib;
	      gchar *pointeur;

	      ib = g_slist_find_custom ( liste_struct_imputation,
					 GINT_TO_POINTER ( operation -> imputation ),
					 (GCompareFunc) recherche_imputation_par_no ) -> data;
	      pointeur = ib -> nom_imputation;

	      if ( operation -> sous_imputation
		   &&
		   etat_courant -> afficher_sous_ib_ope )
		pointeur = g_strconcat ( pointeur,
					 " : ",
					 ((struct struct_sous_imputation *)(g_slist_find_custom ( ib -> liste_sous_imputation,
												  GINT_TO_POINTER ( operation -> sous_imputation ),
												  (GCompareFunc) recherche_sous_imputation_par_no ) -> data )) -> nom_sous_imputation,
					 NULL );

	      label = gtk_label_new ( pointeur );
	      gtk_misc_set_alignment ( GTK_MISC ( label ),
				       0,
				       0.5 );

	      if ( etat_courant -> ope_clickables )
		{
		  GtkWidget *event_box;

		  event_box = gtk_event_box_new ();
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "enter_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_prelight ),
				       NULL );
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "leave_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_normal ),
				       NULL );
		  gtk_signal_connect_object ( GTK_OBJECT ( event_box ),
					      "button_press_event",
					      GTK_SIGNAL_FUNC ( gtktable_click_sur_ope_etat ),
					      (GtkObject *) operation );
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     event_box,
				     colonne, colonne + 1,
				     ligne, ligne + 1,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( event_box );

		  gtk_widget_set_style ( label,
					 style_label_nom_compte );
		  gtk_container_add ( GTK_CONTAINER ( event_box ),
				      label );
		}
	      else
		gtk_table_attach ( GTK_TABLE ( table_etat ),
				   label,
				   colonne, colonne + 1,
				   ligne, ligne + 1,
				   GTK_SHRINK | GTK_FILL,
				   GTK_SHRINK | GTK_FILL,
				   0, 0 );

	      gtk_widget_show ( label );
	    }
	  colonne = colonne + 2;
	}


      if ( etat_courant -> afficher_notes_ope )
	{
	  if ( operation -> notes )
	    {
	      label = gtk_label_new ( operation -> notes );
	      gtk_misc_set_alignment ( GTK_MISC ( label ),
				       0,
				       0.5 );

	      if ( etat_courant -> ope_clickables )
		{
		  GtkWidget *event_box;

		  event_box = gtk_event_box_new ();
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "enter_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_prelight ),
				       NULL );
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "leave_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_normal ),
				       NULL );
		  gtk_signal_connect_object ( GTK_OBJECT ( event_box ),
					      "button_press_event",
					      GTK_SIGNAL_FUNC ( gtktable_click_sur_ope_etat ),
					      (GtkObject *) operation );
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     event_box,
				     colonne, colonne + 1,
				     ligne, ligne + 1,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( event_box );

		  gtk_widget_set_style ( label,
					 style_label_nom_compte );
		  gtk_container_add ( GTK_CONTAINER ( event_box ),
				      label );
		}
	      else
		gtk_table_attach ( GTK_TABLE ( table_etat ),
				   label,
				   colonne, colonne + 1,
				   ligne, ligne + 1,
				   GTK_SHRINK | GTK_FILL,
				   GTK_SHRINK | GTK_FILL,
				   0, 0 );

	      gtk_widget_show ( label );
	    }
	  colonne = colonne + 2;
	}

      if ( etat_courant -> afficher_type_ope )
	{
	  GSList *pointeur;

	  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

	  pointeur = g_slist_find_custom ( TYPES_OPES,
					   GINT_TO_POINTER ( operation -> type_ope ),
					   (GCompareFunc) recherche_type_ope_par_no );

	  if ( pointeur )
	    {
	      struct struct_type_ope *type;

	      type = pointeur -> data;

	      label = gtk_label_new ( type -> nom_type );
	      gtk_misc_set_alignment ( GTK_MISC ( label ),
				       0,
				       0.5 );

	      if ( etat_courant -> ope_clickables )
		{
		  GtkWidget *event_box;

		  event_box = gtk_event_box_new ();
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "enter_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_prelight ),
				       NULL );
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "leave_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_normal ),
				       NULL );
		  gtk_signal_connect_object ( GTK_OBJECT ( event_box ),
					      "button_press_event",
					      GTK_SIGNAL_FUNC ( gtktable_click_sur_ope_etat ),
					      (GtkObject *) operation );
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     event_box,
				     colonne, colonne + 1,
				     ligne, ligne + 1,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( event_box );

		  gtk_widget_set_style ( label,
					 style_label_nom_compte );
		  gtk_container_add ( GTK_CONTAINER ( event_box ),
				      label );
		}
	      else
		gtk_table_attach ( GTK_TABLE ( table_etat ),
				   label,
				   colonne, colonne + 1,
				   ligne, ligne + 1,
				   GTK_SHRINK | GTK_FILL,
				   GTK_SHRINK | GTK_FILL,
				   0, 0 );

	      gtk_widget_show ( label );
	    }
	  colonne = colonne + 2;
	}


      if ( etat_courant -> afficher_cheque_ope )
	{
	  if ( operation -> contenu_type )
	    {
	      label = gtk_label_new ( operation -> contenu_type );
	      gtk_misc_set_alignment ( GTK_MISC ( label ),
				       0,
				       0.5 );

	      if ( etat_courant -> ope_clickables )
		{
		  GtkWidget *event_box;

		  event_box = gtk_event_box_new ();
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "enter_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_prelight ),
				       NULL );
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "leave_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_normal ),
				       NULL );
		  gtk_signal_connect_object ( GTK_OBJECT ( event_box ),
					      "button_press_event",
					      GTK_SIGNAL_FUNC ( gtktable_click_sur_ope_etat ),
					      (GtkObject *) operation );
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     event_box,
				     colonne, colonne + 1,
				     ligne, ligne + 1,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( event_box );

		  gtk_widget_set_style ( label,
					 style_label_nom_compte );
		  gtk_container_add ( GTK_CONTAINER ( event_box ),
				      label );
		}
	      else
		gtk_table_attach ( GTK_TABLE ( table_etat ),
				   label,
				   colonne, colonne + 1,
				   ligne, ligne + 1,
				   GTK_SHRINK | GTK_FILL,
				   GTK_SHRINK | GTK_FILL,
				   0, 0 );

	      gtk_widget_show ( label );
	    }
	  colonne = colonne + 2;
	}


      if ( etat_courant -> afficher_pc_ope )
	{
	  if ( operation -> no_piece_comptable )
	    {
	      label = gtk_label_new ( operation -> no_piece_comptable );
	      gtk_misc_set_alignment ( GTK_MISC ( label ),
				       0,
				       0.5 );

	      if ( etat_courant -> ope_clickables )
		{
		  GtkWidget *event_box;

		  event_box = gtk_event_box_new ();
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "enter_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_prelight ),
				       NULL );
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "leave_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_normal ),
				       NULL );
		  gtk_signal_connect_object ( GTK_OBJECT ( event_box ),
					      "button_press_event",
					      GTK_SIGNAL_FUNC ( gtktable_click_sur_ope_etat ),
					      (GtkObject *) operation );
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     event_box,
				     colonne, colonne + 1,
				     ligne, ligne + 1,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( event_box );

		  gtk_widget_set_style ( label,
					 style_label_nom_compte );
		  gtk_container_add ( GTK_CONTAINER ( event_box ),
				      label );
		}
	      else
		gtk_table_attach ( GTK_TABLE ( table_etat ),
				   label,
				   colonne, colonne + 1,
				   ligne, ligne + 1,
				   GTK_SHRINK | GTK_FILL,
				   GTK_SHRINK | GTK_FILL,
				   0, 0 );

	      gtk_widget_show ( label );
	    }
	  colonne = colonne + 2;
	}



      if ( etat_courant -> afficher_infobd_ope )
	{
	  if ( operation -> info_banque_guichet )
	    {
	      label = gtk_label_new ( operation -> info_banque_guichet );
	      gtk_misc_set_alignment ( GTK_MISC ( label ),
				       0,
				       0.5 );

	      if ( etat_courant -> ope_clickables )
		{
		  GtkWidget *event_box;

		  event_box = gtk_event_box_new ();
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "enter_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_prelight ),
				       NULL );
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "leave_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_normal ),
				       NULL );
		  gtk_signal_connect_object ( GTK_OBJECT ( event_box ),
					      "button_press_event",
					      GTK_SIGNAL_FUNC ( gtktable_click_sur_ope_etat ),
					      (GtkObject *) operation );
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     event_box,
				     colonne, colonne + 1,
				     ligne, ligne + 1,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( event_box );

		  gtk_widget_set_style ( label,
					 style_label_nom_compte );
		  gtk_container_add ( GTK_CONTAINER ( event_box ),
				      label );
		}
	      else
		gtk_table_attach ( GTK_TABLE ( table_etat ),
				   label,
				   colonne, colonne + 1,
				   ligne, ligne + 1,
				   GTK_SHRINK | GTK_FILL,
				   GTK_SHRINK | GTK_FILL,
				   0, 0 );

	      gtk_widget_show ( label );
	    }
	  colonne = colonne + 2;
	}

      if ( etat_courant -> afficher_rappr_ope )
	{
	  GSList *pointeur;

	  pointeur = g_slist_find_custom ( liste_no_rapprochements,
					   GINT_TO_POINTER ( operation -> no_rapprochement ),
					   (GCompareFunc) recherche_no_rapprochement_par_no );

	  if ( pointeur )
	    {
	      struct struct_no_rapprochement *rapprochement;

	      rapprochement = pointeur -> data;

	      label = gtk_label_new ( rapprochement -> nom_rapprochement );
	      gtk_misc_set_alignment ( GTK_MISC ( label ),
				       0,
				       0.5 );

	      if ( etat_courant -> ope_clickables )
		{
		  GtkWidget *event_box;

		  event_box = gtk_event_box_new ();
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "enter_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_prelight ),
				       NULL );
		  gtk_signal_connect ( GTK_OBJECT ( event_box ),
				       "leave_notify_event",
				       GTK_SIGNAL_FUNC ( met_en_normal ),
				       NULL );
		  gtk_signal_connect_object ( GTK_OBJECT ( event_box ),
					      "button_press_event",
					      GTK_SIGNAL_FUNC ( gtktable_click_sur_ope_etat ),
					      (GtkObject *) operation );
		  gtk_table_attach ( GTK_TABLE ( table_etat ),
				     event_box,
				     colonne, colonne + 1,
				     ligne, ligne + 1,
				     GTK_SHRINK | GTK_FILL,
				     GTK_SHRINK | GTK_FILL,
				     0, 0 );
		  gtk_widget_show ( event_box );

		  gtk_widget_set_style ( label,
					 style_label_nom_compte );
		  gtk_container_add ( GTK_CONTAINER ( event_box ),
				      label );
		}
	      else
		gtk_table_attach ( GTK_TABLE ( table_etat ),
				   label,
				   colonne, colonne + 1,
				   ligne, ligne + 1,
				   GTK_SHRINK | GTK_FILL,
				   GTK_SHRINK | GTK_FILL,
				   0, 0 );

	      gtk_widget_show ( label );
	    }
	  colonne = colonne + 2;
	}



      /* on affiche le montant au bout de la ligne */

      if ( devise_compte_en_cours_etat
	   &&
	   operation -> devise == devise_compte_en_cours_etat -> no_devise )
	label = gtk_label_new ( g_strdup_printf  ("%4.2f %s",
						  operation -> montant,
						  devise_compte_en_cours_etat -> code_devise ));
      else
	{
	  struct struct_devise *devise_operation;

	  devise_operation = g_slist_find_custom ( liste_struct_devises,
						   GINT_TO_POINTER ( operation -> devise ),
						   ( GCompareFunc ) recherche_devise_par_no ) -> data;
	  label = gtk_label_new ( g_strdup_printf  ("%4.2f %s",
						    operation -> montant,
						    devise_operation -> code_devise ));
	}

      gtk_misc_set_alignment ( GTK_MISC ( label ),
			       1,
			       0.5 );

      if ( etat_courant -> ope_clickables )
	{
	  GtkWidget *event_box;

	  event_box = gtk_event_box_new ();
	  gtk_signal_connect ( GTK_OBJECT ( event_box ),
			       "enter_notify_event",
			       GTK_SIGNAL_FUNC ( met_en_prelight ),
			       NULL );
	  gtk_signal_connect ( GTK_OBJECT ( event_box ),
			       "leave_notify_event",
			       GTK_SIGNAL_FUNC ( met_en_normal ),
			       NULL );
	  gtk_signal_connect_object ( GTK_OBJECT ( event_box ),
				      "button_press_event",
				      GTK_SIGNAL_FUNC ( gtktable_click_sur_ope_etat ),
				      (GtkObject *) operation );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     event_box,
			     GTK_TABLE ( table_etat ) -> ncols - 1, GTK_TABLE ( table_etat ) -> ncols,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( event_box );

	  gtk_widget_set_style ( label,
				 style_label_nom_compte );
	  gtk_container_add ( GTK_CONTAINER ( event_box ),
			      label );
	}
      else
	gtk_table_attach ( GTK_TABLE ( table_etat ),
			   label,
			   GTK_TABLE ( table_etat ) -> ncols - 1, GTK_TABLE ( table_etat ) -> ncols,
			   ligne, ligne + 1,
			   GTK_SHRINK | GTK_FILL,
			   GTK_SHRINK | GTK_FILL,
			   0, 0 );

      gtk_widget_show ( label );

      if ( ligne_debut_partie == -1 )
	ligne_debut_partie = ligne;

      ligne++;
    }
  return ( ligne );
}
/*****************************************************************************************************/


/*****************************************************************************************************/
gint gtktable_affiche_total_partiel ( gdouble total_partie,
				      gint ligne,
				      gint type )
{
  GtkWidget *label;
  GtkWidget *separateur;

  /* 	  si on affiche les op�s, on met les traits entre eux */

  if ( etat_courant -> afficher_opes
       &&
       ligne_debut_partie != -1 )
    {
      gint i;
      gint colonne;

      colonne = 2;

      for ( i=0 ; i<((nb_colonnes-2)/2) ; i++ )
	{
	  separateur = gtk_vseparator_new ();
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     separateur,
			     colonne, colonne + 1,
			     ligne_debut_partie, ligne,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( separateur );

	  colonne = colonne + 2;
	}
      ligne_debut_partie = -1;
    }


  label = gtk_label_new ( "" );
  gtk_misc_set_alignment ( GTK_MISC ( label ),
			   0,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     1, GTK_TABLE ( table_etat ) -> ncols - 1,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  ligne++;

  separateur = gtk_hseparator_new ();
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     separateur,
		     0, GTK_TABLE ( table_etat ) -> ncols,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( separateur );

  ligne++;

  if ( type )
    {
      if ( etat_courant -> afficher_nb_opes )
	label = gtk_label_new ( g_strdup_printf ( COLON(_("Total d�penses (%d op�rations)")),
						  nb_ope_partie_etat ));
      else
	label = gtk_label_new ( COLON(_("Total d�penses")) );
    }
  else
    {
      if ( etat_courant -> afficher_nb_opes )
	label = gtk_label_new ( g_strdup_printf ( COLON(_("Total revenus (%d op�rations)")),
						  nb_ope_partie_etat ));
      else
	label = gtk_label_new ( COLON(_("Total revenus")) );
    }
  gtk_misc_set_alignment ( GTK_MISC ( label ),
			   0,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     1, GTK_TABLE ( table_etat ) -> ncols,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
					    total_partie,
					    devise_generale_etat -> code_devise ));
  gtk_misc_set_alignment ( GTK_MISC ( label ),
			   1,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     GTK_TABLE ( table_etat ) -> ncols - 1, GTK_TABLE ( table_etat ) -> ncols,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  ligne++;

  separateur = gtk_hseparator_new ();
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     separateur,
		     0, GTK_TABLE ( table_etat ) -> ncols,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( separateur );

  ligne++;

  label = gtk_label_new ( "" );
  gtk_misc_set_alignment ( GTK_MISC ( label ),
			   0,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     1, GTK_TABLE ( table_etat ) -> ncols,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  ligne++;

  nom_categ_en_cours = NULL;
  nom_ss_categ_en_cours = NULL;
  nom_ib_en_cours = NULL;
  nom_ss_ib_en_cours = NULL;
  nom_compte_en_cours = NULL;
  nom_tiers_en_cours = NULL;


  return ( ligne );
}
/*****************************************************************************************************/

/*****************************************************************************************************/
gint gtktable_affiche_total_general ( gdouble total_general,
				      gint ligne )
{
  GtkWidget *label;
  GtkWidget *separateur;

  /* 	  si on affiche les op�s, on met les traits entre eux */

  if ( etat_courant -> afficher_opes
       &&
       ligne_debut_partie != -1 )
    {
      gint i;
      gint colonne;

      colonne = 2;

      for ( i=0 ; i<((nb_colonnes-2)/2) ; i++ )
	{
	  separateur = gtk_vseparator_new ();
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     separateur,
			     colonne, colonne + 1,
			     ligne_debut_partie, ligne,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( separateur );

	  colonne = colonne + 2;
	}
      ligne_debut_partie = -1;
    }

  label = gtk_label_new ( "" );
  gtk_misc_set_alignment ( GTK_MISC ( label ),
			   0,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     1, GTK_TABLE ( table_etat ) -> ncols,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  ligne++;

  separateur = gtk_hseparator_new ();
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     separateur,
		     0, GTK_TABLE ( table_etat ) -> ncols,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( separateur );

  ligne++;

  if ( etat_courant -> afficher_nb_opes )
    label = gtk_label_new ( g_strdup_printf ( COLON(_("Total g�n�ral (%d op�rations)")),
					      nb_ope_general_etat ));
  else
    label = gtk_label_new ( COLON(_("Total g�n�ral")) );
  gtk_misc_set_alignment ( GTK_MISC ( label ),
			   0,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     1, GTK_TABLE ( table_etat ) -> ncols -1,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  label = gtk_label_new ( g_strdup_printf ( "%4.2f %s",
					    total_general,
					    devise_generale_etat -> code_devise ));
  gtk_misc_set_alignment ( GTK_MISC ( label ),
			   1,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     GTK_TABLE ( table_etat ) -> ncols -1, GTK_TABLE ( table_etat ) -> ncols,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  ligne++;

  separateur = gtk_hseparator_new ();
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     separateur,
		     0, GTK_TABLE ( table_etat ) -> ncols,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( separateur );

  ligne++;

  label = gtk_label_new ( "" );
  gtk_misc_set_alignment ( GTK_MISC ( label ),
			   0,
			   0.5 );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     1, GTK_TABLE ( table_etat ) -> ncols,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  ligne++;

  return ( ligne );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
gint gtktable_affiche_categ_etat ( struct structure_operation *operation,
				   gchar *decalage_categ,
				   gint ligne )
{
  gchar *pointeur_char;
  GtkWidget *label;

  /* v�rifie qu'il y a un changement de cat�gorie */
  /* �a peut �tre aussi chgt pour virement, ventilation ou pas de categ */

  if ( etat_courant -> utilise_categ
       &&
       ( operation -> categorie != ancienne_categ_etat
	 ||
	 ( ancienne_categ_speciale_etat == 1
	   &&
	   !operation -> relation_no_operation )
	 ||
	 ( ancienne_categ_speciale_etat == 2
	   &&
	   !operation -> operation_ventilee )
	 ||
	 ( ancienne_categ_speciale_etat == 3
	   &&
	   ( operation -> operation_ventilee
	     ||
	     operation -> relation_no_operation ))))
    {

      /* lorsqu'on est au d�but de l'affichage de l'�tat, on n'affiche pas de totaux */

      if ( !debut_affichage_etat
	   &&
	   !changement_de_groupe_etat )
	{
	  /* on affiche le total de la p�riode en le for�ant */

	  ligne = gtktable_affiche_total_periode ( operation,
						   ligne,
						   1 );

	  /* on ajoute les totaux de tout ce qu'il y a derri�re la cat�gorie */

	  ligne = gtktable_affichage . affiche_totaux_sous_jaccent ( 1,
								     ligne );

	  /* on ajoute le total de la categ */

	  ligne = gtktable_affichage . affiche_total_categories ( ligne );
	}

      /*       si on a demand� de ne pas afficher les noms des cat�g, on saute la partie suivante */

      if ( etat_courant -> afficher_nom_categ )
	{
	  if ( operation -> categorie )
	    {
	      nom_categ_en_cours = ((struct struct_categ *)(g_slist_find_custom ( liste_struct_categories,
										  GINT_TO_POINTER ( operation -> categorie ),
										  (GCompareFunc) recherche_categorie_par_no ) -> data )) -> nom_categ;
	      pointeur_char = g_strconcat ( decalage_categ,
					    nom_categ_en_cours,
					    NULL );
	      ancienne_categ_speciale_etat = 0;
	    }
	  else
	    {
	      if ( operation -> relation_no_operation )
		{
		  pointeur_char = g_strconcat ( decalage_categ,
						_("Virements"),
						NULL );
		  ancienne_categ_speciale_etat = 1;
		}
	      else
		{
		  if ( operation -> operation_ventilee )
		    {
		      pointeur_char = g_strconcat ( decalage_categ,
						    _("Op�ration ventil�e"),
						    NULL );
		      ancienne_categ_speciale_etat = 2;
		    }
		  else
		    {
		      pointeur_char = g_strconcat ( decalage_categ,
						    _("Pas de cat�gorie"),
						    NULL );
		      ancienne_categ_speciale_etat = 3;
		    }
		}
	    }

	  label = gtk_label_new ( pointeur_char );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     0, 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}

      ligne_debut_partie = ligne;
      denote_struct_sous_jaccentes ( 1 );

      ancienne_categ_etat = operation -> categorie;

      debut_affichage_etat = 0;
      changement_de_groupe_etat = 1;
    }

  return ( ligne );
}
/*****************************************************************************************************/



/*****************************************************************************************************/
gint gtktable_affiche_sous_categ_etat ( struct structure_operation *operation,
			       			       gchar *decalage_sous_categ,
					gint ligne )
{
  gchar *pointeur_char;
  GtkWidget *label;

  if ( etat_courant -> utilise_categ
       &&
       etat_courant -> afficher_sous_categ
       &&
       operation -> categorie
       &&
       operation -> sous_categorie != ancienne_sous_categ_etat )
    {
      struct struct_categ *categ;

     /* lorsqu'on est au d�but de l'affichage de l'�tat, on n'affiche pas de totaux */

      if ( !debut_affichage_etat
	   &&
	   !changement_de_groupe_etat )
	{
	  /* on affiche le total de la p�riode en le for�ant */

	  ligne = gtktable_affiche_total_periode ( operation,
						   ligne,
						   1 );

	  /* on ajoute les totaux de tout ce qu'il y a derri�re la sous cat�gorie */

	  ligne = gtktable_affichage . affiche_totaux_sous_jaccent ( 2,
	ligne );

	  /* on ajoute le total de la sous categ */

	  ligne = gtktable_affichage . affiche_total_sous_categ ( ligne );
	}

      /*       si on a demand� de ne pas afficher les noms des ss-cat�g, on saute la partie suivante */

      if ( etat_courant -> afficher_nom_categ )
	{
	  categ = g_slist_find_custom ( liste_struct_categories,
					GINT_TO_POINTER ( operation -> categorie ),
					(GCompareFunc) recherche_categorie_par_no ) -> data;

	  if ( operation -> sous_categorie )
	    {
	      nom_ss_categ_en_cours = ((struct struct_sous_categ *)(g_slist_find_custom ( categ->liste_sous_categ,
											  GINT_TO_POINTER ( operation -> sous_categorie ),
											  (GCompareFunc) recherche_sous_categorie_par_no ) -> data )) -> nom_sous_categ;
	      pointeur_char = g_strconcat ( decalage_sous_categ,
					    nom_ss_categ_en_cours,
					    NULL );
	    }
	  else
	    {
	      if ( etat_courant -> afficher_pas_de_sous_categ )
		pointeur_char = g_strconcat ( decalage_sous_categ,
					      _("Pas de sous-cat�gorie"),
					      NULL );
	      else
		pointeur_char = "";
	    }

	  label = gtk_label_new ( pointeur_char );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     0,1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}
 
     ligne_debut_partie = ligne;
     denote_struct_sous_jaccentes ( 2 );

     ancienne_sous_categ_etat = operation -> sous_categorie;

     debut_affichage_etat = 0;
     changement_de_groupe_etat = 1;
    }

  return ( ligne );
}
/*****************************************************************************************************/




/*****************************************************************************************************/
gint gtktable_affiche_ib_etat ( struct structure_operation *operation,
		       		       gchar *decalage_ib,
		       gint ligne )
{
  gchar *pointeur_char;
  GtkWidget *label;

  /* mise en place de l'ib */


  if ( etat_courant -> utilise_ib
       &&
       operation -> imputation != ancienne_ib_etat )
    {
      /* lorsqu'on est au d�but de l'affichage de l'�tat, on n'affiche pas de totaux */

      if ( !debut_affichage_etat
	   &&
	   !changement_de_groupe_etat )
	{
	  /* on affiche le total de la p�riode en le for�ant */

	  ligne = gtktable_affiche_total_periode ( operation,
						   ligne,
						   1 );

	  /* on ajoute les totaux de tout ce qu'il y a derri�re l'ib */

	  ligne = gtktable_affichage . affiche_totaux_sous_jaccent ( 3,
	ligne );

	  /* on ajoute le total de l'ib */

	  ligne = gtktable_affichage . affiche_total_ib ( ligne );
	}
 
      /*       si on a demand� de ne pas afficher les noms des ib, on saute la partie suivante */

      if ( etat_courant -> afficher_nom_ib )
	{
	  if ( operation -> imputation )
	    {
	      nom_ib_en_cours = ((struct struct_imputation *)(g_slist_find_custom ( liste_struct_imputation,
										    GINT_TO_POINTER ( operation -> imputation ),
										    (GCompareFunc) recherche_imputation_par_no ) -> data )) -> nom_imputation;
	      pointeur_char = g_strconcat ( decalage_ib,
					    nom_ib_en_cours,
					    NULL );
	    }
	  else
	    pointeur_char = g_strconcat ( decalage_ib,
					  _("Pas d'imputation budg�taire"),
					  NULL );

	  label = gtk_label_new ( pointeur_char );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     0, 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}

      ligne_debut_partie = ligne;
      denote_struct_sous_jaccentes ( 3 );

      ancienne_ib_etat = operation -> imputation;

      debut_affichage_etat = 0;
      changement_de_groupe_etat = 1;
    }

  return ( ligne );
}
/*****************************************************************************************************/




/*****************************************************************************************************/
gint gtktable_affiche_sous_ib_etat ( struct structure_operation *operation,
			    			    gchar *decalage_sous_ib,
			    gint ligne )
{
  gchar *pointeur_char;
  GtkWidget *label;


  /* mise en place de la sous_ib */


  if ( etat_courant -> utilise_ib
       &&
       etat_courant -> afficher_sous_ib
       &&
       operation -> imputation
       &&
       operation -> sous_imputation != ancienne_sous_ib_etat )
    {
      struct struct_imputation *imputation;

      /* lorsqu'on est au d�but de l'affichage de l'�tat, on n'affiche pas de totaux */

      if ( !debut_affichage_etat
	   &&
	   !changement_de_groupe_etat )
	{
	  /* on affiche le total de la p�riode en le for�ant */

	  ligne = gtktable_affiche_total_periode ( operation,
						   ligne,
						   1 );

	  /* on ajoute les totaux de tout ce qu'il y a derri�re la sous ib */

	  ligne = gtktable_affichage . affiche_totaux_sous_jaccent ( 4,
	ligne );

	  /* on ajoute le total de la sous ib */

	  ligne = gtktable_affichage . affiche_total_sous_ib ( ligne );
	}
 
      /*       si on a demand� de ne pas afficher les noms des ss-ib, on saute la partie suivante */

      if ( etat_courant -> afficher_nom_ib )
	{
	  imputation = g_slist_find_custom ( liste_struct_imputation,
					     GINT_TO_POINTER ( operation -> imputation ),
					     (GCompareFunc) recherche_imputation_par_no ) -> data;

	  if ( operation -> sous_imputation )
	    {
	      nom_ss_ib_en_cours = ((struct struct_sous_imputation *)(g_slist_find_custom ( imputation->liste_sous_imputation,
											    GINT_TO_POINTER ( operation -> sous_imputation ),
											    (GCompareFunc) recherche_sous_imputation_par_no ) -> data )) -> nom_sous_imputation;
	      pointeur_char = g_strconcat ( decalage_sous_ib,
					    nom_ss_ib_en_cours,
					    NULL );
	    }
	  else
	    {
	      if ( etat_courant -> afficher_pas_de_sous_ib )
		pointeur_char = g_strconcat ( decalage_sous_ib,
					      _("Pas de sous-imputation"),
					      NULL );
	      else
		pointeur_char = "";
	    }

	  label = gtk_label_new ( pointeur_char );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     0, 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}

      ligne_debut_partie = ligne;
      denote_struct_sous_jaccentes ( 4 );

      ancienne_sous_ib_etat = operation -> sous_imputation;

      debut_affichage_etat = 0;
      changement_de_groupe_etat = 1;
    }

  return ( ligne );
}
/*****************************************************************************************************/




/*****************************************************************************************************/
gint gtktable_affiche_compte_etat ( struct structure_operation *operation,
			   			   gchar *decalage_compte,
			   gint ligne )
{
  gchar *pointeur_char;
  GtkWidget *label;

  /* mise en place du compte */

  if ( etat_courant -> regroupe_ope_par_compte
       &&
       operation -> no_compte != ancien_compte_etat )
    {
     /* lorsqu'on est au d�but de l'affichage de l'�tat, on n'affiche pas de totaux */

      if ( !debut_affichage_etat
	   &&
	   !changement_de_groupe_etat )
	{
 	  /* on affiche le total de la p�riode en le for�ant */

	  ligne = gtktable_affiche_total_periode ( operation,
						   ligne,
						   1 );

	  /* on ajoute les totaux de tout ce qu'il y a derri�re le compte */

	  ligne = gtktable_affichage . affiche_totaux_sous_jaccent ( 5,
	ligne );

	  /* on ajoute le total du compte */

	  ligne = gtktable_affichage . affiche_total_compte ( ligne );
	}
 
      /*       si on a demand� de ne pas afficher les noms des comptes, on saute la partie suivante */

      if ( etat_courant -> afficher_nom_compte )
	{
	  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

	  pointeur_char = g_strconcat ( decalage_compte,
					NOM_DU_COMPTE,
					NULL );
	  nom_compte_en_cours = NOM_DU_COMPTE;

	  label = gtk_label_new ( pointeur_char );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     0, 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );

	  ligne++;
	}

      ligne_debut_partie = ligne;
      denote_struct_sous_jaccentes ( 5 );

      ancien_compte_etat = operation -> no_compte;

      debut_affichage_etat = 0;
      changement_de_groupe_etat = 1;
    }

  return ( ligne );
}
/*****************************************************************************************************/




/*****************************************************************************************************/
gint gtktable_affiche_tiers_etat ( struct structure_operation *operation,
			  			  gchar *decalage_tiers,
			  gint ligne )
{
  gchar *pointeur_char;
  GtkWidget *label;

  /* affiche le tiers */

  if ( etat_courant -> utilise_tiers
       &&
       operation -> tiers != ancien_tiers_etat )
    {
      /* lorsqu'on est au d�but de l'affichage de l'�tat, on n'affiche pas de totaux */

      if ( !debut_affichage_etat
	   &&
	   !changement_de_groupe_etat )
	{
	  /* on affiche le total de la p�riode en le for�ant */

	  ligne = gtktable_affiche_total_periode ( operation,
						   ligne,
						   1 );

	  /* on ajoute les totaux de tout ce qu'il y a derri�re le tiers */

	  ligne = gtktable_affichage . affiche_totaux_sous_jaccent ( 6,
	ligne );

	  /* on ajoute le total du tiers */

	  ligne = gtktable_affichage . affiche_total_tiers ( ligne );
	}

      /*       si on a demand� de ne pas afficher les noms des tiers, on saute la partie suivante */

      if ( etat_courant -> afficher_nom_tiers )
	{
	  if ( operation -> tiers )
	    {
	      nom_tiers_en_cours = ((struct struct_tiers *)(g_slist_find_custom ( liste_struct_tiers,
										  GINT_TO_POINTER ( operation -> tiers ),
										  (GCompareFunc) recherche_tiers_par_no ) -> data )) -> nom_tiers;
	      pointeur_char = g_strconcat ( decalage_tiers,
					    nom_tiers_en_cours,
					    NULL );
	    }
	  else
	    pointeur_char = g_strconcat ( decalage_tiers,
					  _("Pas de tiers"),
					  NULL );

	  label = gtk_label_new ( pointeur_char );
	  gtk_misc_set_alignment ( GTK_MISC ( label ),
				   0,
				   0.5 );
	  gtk_table_attach ( GTK_TABLE ( table_etat ),
			     label,
			     0, 1,
			     ligne, ligne + 1,
			     GTK_SHRINK | GTK_FILL,
			     GTK_SHRINK | GTK_FILL,
			     0, 0 );
	  gtk_widget_show ( label );
	

	  ligne++;
	}

      ligne_debut_partie = ligne;
      denote_struct_sous_jaccentes ( 6 );

      ancien_tiers_etat = operation -> tiers;

      debut_affichage_etat = 0;
      changement_de_groupe_etat = 1;
    }
  return ( ligne );
}
/*****************************************************************************************************/


/*****************************************************************************************************/
gint gtktable_affiche_titre_revenus_etat ( gint ligne )
{
  GtkWidget *label;

  label = gtk_label_new ( "" );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     0, 1,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  ligne++;
  label = gtk_label_new ( _("Revenus") );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     0, 1,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  ligne++;

  label = gtk_label_new ( "" );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     0, 1,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  ligne++;

  return ( ligne );
}
/*****************************************************************************************************/


/*****************************************************************************************************/
gint gtktable_affiche_titre_depenses_etat ( gint ligne )
{
  GtkWidget *label;

  label = gtk_label_new ( "" );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     0, 1,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  ligne++;

  label = gtk_label_new ( _("D�penses") );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     0, 1,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  ligne++;

  label = gtk_label_new ( "" );
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     label,
		     0, 1,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( label );

  ligne++;


  return ( ligne );
}
/*****************************************************************************************************/


/*****************************************************************************************************/
/* appel�e lors de l'affichage d'une structure ( cat�g, ib ... ) */
/* affiche le total de toutes les structures sous jaccentes */
/*****************************************************************************************************/

gint gtktable_affiche_totaux_sous_jaccent ( gint origine,
					    gint ligne )
{
  GList *pointeur_glist;


  /* on doit partir du bout de la liste pour revenir vers la structure demand�e */

  pointeur_glist = g_list_last ( etat_courant -> type_classement );


  while ( GPOINTER_TO_INT ( pointeur_glist -> data ) != origine )
    {
      switch ( GPOINTER_TO_INT ( pointeur_glist -> data ))
	{
	case 1:
	  ligne = gtktable_affichage . affiche_total_categories ( ligne );
	  break;

	case 2:
	  ligne = gtktable_affichage . affiche_total_sous_categ ( ligne );
	  break;

	case 3:
	  ligne = gtktable_affichage . affiche_total_ib ( ligne );
	  break;

	case 4:
	  ligne = gtktable_affichage . affiche_total_sous_ib ( ligne );
	  break;

	case 5:
	  ligne = gtktable_affichage . affiche_total_compte ( ligne );
	  break;

	case 6:
	  ligne = gtktable_affichage . affiche_total_tiers ( ligne );
	  break;
	}
      pointeur_glist = pointeur_glist -> prev;
    }

  return ( ligne );

}
/*****************************************************************************************************/



/*****************************************************************************************************/
gint gtktable_affiche_titres_colonnes ( gint ligne )
{
  gint colonne;
  GtkWidget *label;
  GtkWidget *separateur;

  colonne = 1;

  if ( etat_courant -> afficher_no_ope )
    {
      label = gtk_label_new ( _("N�") );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 colonne, colonne + 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      colonne = colonne + 2;
    }

  if ( etat_courant -> afficher_date_ope )
    {
      label = gtk_label_new ( _("Date") );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 colonne, colonne + 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      colonne = colonne + 2;
    }

  if ( etat_courant -> afficher_exo_ope )
    {
      label = gtk_label_new ( _("Exercice") );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 colonne, colonne + 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      colonne = colonne + 2;
    }

  if ( etat_courant -> afficher_tiers_ope )
    {
      label = gtk_label_new ( _("Tiers") );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 colonne, colonne + 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      colonne = colonne + 2;
    }

  if ( etat_courant -> afficher_categ_ope )
    {
      label = gtk_label_new ( _("Cat�gorie") );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 colonne, colonne + 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      colonne = colonne + 2;
    }

  if ( etat_courant -> afficher_ib_ope )
    {
      label = gtk_label_new ( _("Imputation budg�taire") );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 colonne, colonne + 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      colonne = colonne + 2;
    }

  if ( etat_courant -> afficher_notes_ope )
    {
      label = gtk_label_new ( _("Notes") );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 colonne, colonne + 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      colonne = colonne + 2;
    }

  if ( etat_courant -> afficher_type_ope )
    {
      label = gtk_label_new ( _("Mode de r�glement") );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 colonne, colonne + 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      colonne = colonne + 2;
    }

  if ( etat_courant -> afficher_cheque_ope )
    {
      label = gtk_label_new ( _("Ch�que") );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 colonne, colonne + 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      colonne = colonne + 2;
    }

  if ( etat_courant -> afficher_pc_ope )
    {
      label = gtk_label_new ( _("Pi�ce comptable") );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 colonne, colonne + 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      colonne = colonne + 2;
    }

  if ( etat_courant -> afficher_infobd_ope )
    {
      label = gtk_label_new ( _("Info banque/guichet") );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 colonne, colonne + 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      colonne = colonne + 2;
    }

  if ( etat_courant -> afficher_rappr_ope )
    {
      label = gtk_label_new ( _("Relev�") );
      gtk_table_attach ( GTK_TABLE ( table_etat ),
			 label,
			 colonne, colonne + 1,
			 ligne, ligne + 1,
			 GTK_SHRINK | GTK_FILL,
			 GTK_SHRINK | GTK_FILL,
			 0, 0 );
      gtk_widget_show ( label );

      colonne = colonne + 2;
    }


  ligne++;

  separateur = gtk_hseparator_new ();
  gtk_table_attach ( GTK_TABLE ( table_etat ),
		     separateur,
		     1, GTK_TABLE ( table_etat ) -> ncols - 1,
		     ligne, ligne + 1,
		     GTK_SHRINK | GTK_FILL,
		     GTK_SHRINK | GTK_FILL,
		     0, 0 );
  gtk_widget_show ( separateur );

  ligne++;

  titres_affiches = 1;

  return ( ligne );
}
/*****************************************************************************************************/


/*****************************************************************************************************/
gint gtktable_finish ()
{
  /* Nothing to do in GTK */
  return 1;
}
/*****************************************************************************************************/


/*****************************************************************************************************/
/* cette fonction est appel�e si on click sur une op� dans un �tat */
/* elle affiche la liste des op�s sur cette op� */
/*****************************************************************************************************/

void gtktable_click_sur_ope_etat ( struct structure_operation *operation )
{
  /* si c'est une op� de ventilation, on affiche l'op�ration m�re */

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

  if ( operation -> no_operation_ventilee_associee )
    operation = g_slist_find_custom ( LISTE_OPERATIONS,
				      GINT_TO_POINTER ( operation -> no_operation_ventilee_associee ),
				      (GCompareFunc) recherche_operation_par_no ) -> data;

  /* passage sur le compte concern� */

  changement_compte ( GINT_TO_POINTER ( operation -> no_compte ));

  /* r�cup�ration de la ligne de l'op� dans la liste ; affichage de toutes les op� si n�cessaire */

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

  if ( operation -> pointe == 2 && !AFFICHAGE_R )
    change_aspect_liste ( NULL,
			  2 );

  OPERATION_SELECTIONNEE = operation;

  selectionne_ligne ( compte_courant );
}
/*****************************************************************************************************/
