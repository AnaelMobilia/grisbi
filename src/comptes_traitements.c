/* Ce fichier s'occupe des manipulations de comptes */
/* comptes_traitements.c */

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



/* *********************************************************************************************************** */
/* Routine appel�e lorsque l'on cr�e un nouveau compte */
/* *********************************************************************************************************** */

void  nouveau_compte ( void )
{
  GtkWidget *bouton;
  gint type_de_compte;
  gint no_compte;

  if ( !nb_comptes )
    {
      nouveau_fichier ();
      return;
    }

  type_de_compte = demande_type_nouveau_compte ();

  no_compte = initialisation_nouveau_compte ( type_de_compte );

  /* si la cr�ation s'est mal plac�e, on se barre */

  if ( no_compte == -1 )
    return;

  /* cr�e le nouveau bouton du compte et l'ajoute � la liste des comptes */

  bouton = comptes_appel( NO_COMPTE );
  gtk_box_pack_start (GTK_BOX (vbox_liste_comptes),
		      bouton,
		      FALSE,
		      FALSE,
		      0);
  gtk_widget_show (bouton);


  /* on cr�e la liste des op�s */

  ajoute_nouvelle_liste_operation( NO_COMPTE );

  /* on recr�e les combofix des cat�gories */

  mise_a_jour_categ();

/* on met � jour l'option menu des formulaires des �ch�ances et des op�s */

  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[5] ),
			     creation_option_menu_comptes () );


  /* mise � jour de l'accueil */

  update_liste_comptes_accueil ();

  remplissage_liste_comptes_etats ();
  selectionne_liste_comptes_etat_courant ();

  gtk_widget_set_sensitive ( bouton_supprimer_compte,
			     TRUE );


  /* on cr�e le nouveau compte dans les propri�t�s des comptes */

  bouton = comptes_appel_onglet ( nb_comptes - 1 );
  gtk_box_pack_start (GTK_BOX (vbox_liste_comptes_onglet),
		      bouton,
		      FALSE,
		      FALSE,
		      0);
  gtk_widget_show (bouton);

  /* on se place sur le nouveau compte pour faire les modifs */

  changement_compte_onglet ( ((GtkBoxChild *)(GTK_BOX ( bouton ) -> children -> data))->widget,
			     nb_comptes - 1 );

  /* on se met sur l'onglet de propri�t�s du compte */

  gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_general ),
			  3 );

  modification_fichier ( TRUE );
}
/* *********************************************************************************************************** */


/* *********************************************************************************************************** */
/* cette fonction cr�e un nouveau compte, l'initialise, l'ajoute aux comptes */
/* et renvoie le no du compte cr�� */
/* renvoie -1 s'il y a un pb */
/* *********************************************************************************************************** */
gint initialisation_nouveau_compte ( gint type_de_compte )
{

  if  (!(p_tab_nom_de_compte = realloc ( p_tab_nom_de_compte, ( nb_comptes + 1 )* sizeof ( gpointer ) )))
    {
      dialogue ( _("Erreur dans l'allocation de m�moire pour cr�er un nouveau compte !") );
      return (-1);
    };


  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + nb_comptes;

  if  (!(*p_tab_nom_de_compte_variable = calloc ( 1,
						  sizeof (struct donnees_compte) )) )
    {
      dialogue ( _("Erreur dans l'allocation de m�moire pour cr�er un nouveau compte !") );
      return (-1);
    };

  p_tab_nom_de_compte_courant = p_tab_nom_de_compte + compte_courant;

  /* ins�re ses param�tres ( comme c'est un appel � calloc, tout ce qui est � 0 est d�j� initialis� )*/

  NOM_DU_COMPTE = g_strdup ( _("Sans nom") );
  OPERATION_SELECTIONNEE = GINT_TO_POINTER ( -1 );
  DEVISE = 1;
  MISE_A_JOUR = 1;
  NO_COMPTE = nb_comptes;
  AFFICHAGE_R = 0;
  NB_LIGNES_OPE = 3;

  TYPE_DE_COMPTE = type_de_compte;

  nb_comptes++;


  /* on cr�e les types par d�faut */

  creation_types_par_defaut ( NO_COMPTE,
			      0);

  /* on met le compte � la fin dans le classement des comptes */

  ordre_comptes = g_slist_append ( ordre_comptes,
				   GINT_TO_POINTER ( NO_COMPTE ) );

  return (NO_COMPTE);
}
/* *********************************************************************************************************** */



/* *********************************************************************************************************** */
/* fonction affichant une boite de dialogue contenant une liste des comptes */
/* pour en supprimer un */
/* *********************************************************************************************************** */


void supprimer_compte ( void )
{
  GtkWidget *dialog;
  GtkWidget *label;
  GtkWidget *liste_comptes;
  gint resultat;
  short actualise = 0, i;
  GSList *pointeur_liste;
  gint compte_modifie;
  gchar *nom_compte_supprime;
  gint page_en_cours;


  dialog = gnome_dialog_new ( _("S�lection d'un compte"),
			      GNOME_STOCK_BUTTON_OK,
			      GNOME_STOCK_BUTTON_CANCEL,
			      NULL );
  gtk_window_set_transient_for ( GTK_WINDOW ( dialog ),
				 GTK_WINDOW ( window ));
  gtk_signal_connect ( GTK_OBJECT ( dialog),
		       "destroy_event",
		       GTK_SIGNAL_FUNC ( blocage_boites_dialogues ),
		       NULL );
  gtk_signal_connect ( GTK_OBJECT ( dialog ),
		       "destroy_event",
		       GTK_SIGNAL_FUNC ( blocage_boites_dialogues ),
		       GINT_TO_POINTER ( 1 ) );

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

  label = gtk_label_new ( _("Choisir le compte � supprimer :\n\n\n( Attention, il s'agit d'un suppression d�finitive !\n Pour cl�turer un compte, il faut\naller dans le d�tail du compte concern�. )") );
  gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
		       label,
		       FALSE,
		       FALSE,
		       5 );
  gtk_widget_show ( label );


  liste_comptes = gtk_option_menu_new ();
  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( liste_comptes ),
			     creation_option_menu_comptes () );
  gtk_option_menu_set_history ( GTK_OPTION_MENU ( liste_comptes ),
				compte_courant_onglet );
  gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
		       liste_comptes,
		       FALSE,
		       FALSE,
		       5 );
  gtk_widget_show ( liste_comptes );

  resultat = gnome_dialog_run ( GNOME_DIALOG ( dialog ));


  /* le compte � supprimer est maintenant choisi */

  if ( resultat )
    {
      gnome_dialog_close ( GNOME_DIALOG ( dialog ));
      return;
    }


  compte_modifie = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( liste_comptes ) -> menu_item ),
							   "no_compte" ) );

  gnome_dialog_close  ( GNOME_DIALOG ( dialog ));


/* on commence ici la suppression du compte */
 
/* si qu'un compte, on fermer le fichier */

  if ( nb_comptes == 1 )
    {
      etat.modification_fichier = 0;
      fermer_fichier ();
      return;
    }

  


  /* supprime l'onglet du compte */

  gtk_notebook_remove_page ( GTK_NOTEBOOK ( notebook_listes_operations ),
			     compte_modifie + 1 );

  /*       suppression des �ch�ances */

  while ( ( pointeur_liste = g_slist_find_custom ( gsliste_echeances,
						   GINT_TO_POINTER ( compte_modifie ),
						   (GCompareFunc) cherche_compte_dans_echeances ) ) )
    {
      if ( echeance_selectionnnee == ECHEANCE_COURANTE )
	ECHEANCE_COURANTE = -1;

      gsliste_echeances = g_slist_remove ( gsliste_echeances, 
					   ECHEANCE_COURANTE );
      nb_echeances--;
    }



  /* supprime le compte de la liste de l'ordre des comptes */

  ordre_comptes = g_slist_remove ( ordre_comptes,
				   GINT_TO_POINTER ( compte_modifie ));


  /* modifie les num�ros des comptes sup�rieurs au compte supprim� dans l'ordre des comptes */

  pointeur_liste = ordre_comptes;

  do
    {
      if ( GPOINTER_TO_INT ( pointeur_liste -> data ) > compte_modifie )
	pointeur_liste -> data--;
    }
  while ( ( pointeur_liste = pointeur_liste -> next ) );


  if ( compte_courant == compte_modifie )
    {
      actualise = 1;
      compte_courant = 0;
    }
  else
    if ( compte_courant > compte_modifie )
      {
	compte_courant--;
	actualise = 0;
      }


  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_modifie;
  p_tab_nom_de_compte_courant = p_tab_nom_de_compte + compte_courant;
  nb_comptes--;

  g_slist_free ( LISTE_OPERATIONS );
  nom_compte_supprime = g_strdup ( NOM_DU_COMPTE );

/* on d�cale en m�moire les comptes situ�s apr�s */

  for ( i = compte_modifie ; i < nb_comptes ; i++ )
    {
      NO_COMPTE = NO_COMPTE -1;
      *p_tab_nom_de_compte_variable = *(p_tab_nom_de_compte_variable + 1);
      p_tab_nom_de_compte_variable++;
    }


  /* recherche les �ch�ances pour les comptes pla��s apr�s le compe supprim� */
  /* pour leur diminuer leur num�ro de compte de 1 */

  pointeur_liste = gsliste_echeances;

  while ( pointeur_liste )
    {
      if ( ECHEANCE_COURANTE -> compte > compte_modifie )
	ECHEANCE_COURANTE -> compte--;
      if ( ECHEANCE_COURANTE -> compte_virement > compte_modifie )
	ECHEANCE_COURANTE -> compte_virement--;

      pointeur_liste = pointeur_liste -> next;
    }


  /*   fait le tour des op�s de tous les comptes, */
  /*     pour les op�s des comptes > � celui supprim�, on descend le no de compte */
  /*     pour les virements vers le compte supprim�, met relation_no_compte � -1 */

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

  for ( i=0 ; i < nb_comptes ; i++ )
    {
      GSList *pointeur_tmp;

      pointeur_tmp = LISTE_OPERATIONS;

      while ( pointeur_tmp )
	{
	  struct structure_operation *operation;

	  operation = pointeur_tmp -> data;

	  if ( operation -> no_compte > compte_modifie )
	    operation -> no_compte--;

	  if ( operation -> relation_no_operation )
	    {
	      if ( operation -> relation_no_compte == compte_modifie )
		{
		  operation -> relation_no_compte = -1;
		  MISE_A_JOUR = 1;
		}
	      else
		if ( operation -> relation_no_compte > compte_modifie )
		  operation -> relation_no_compte--;
	    }
	  pointeur_tmp = pointeur_tmp -> next;
	}
      p_tab_nom_de_compte_variable++;
    }

  /* retire le bouton du compte dans la liste des comptes */
  /*   pour cela, on efface vbox_liste_comptes et on le recr�e */

  reaffiche_liste_comptes();
  reaffiche_liste_comptes_onglet ();

  /* on recr�e les combofix des tiers et des cat�gories */

  mise_a_jour_tiers();
  mise_a_jour_categ();
  mise_a_jour_imputation();

/* on met � jour l'option menu du formulaire des �ch�ances */

  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[5] ),
			     creation_option_menu_comptes () );

  /* r�affiche la liste si necessaire */

  page_en_cours = gtk_notebook_get_current_page ( GTK_NOTEBOOK ( notebook_general ));

  changement_compte ( GINT_TO_POINTER ( compte_courant ));

  gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_general ),
			  page_en_cours );
      
  remplissage_liste_echeance ();
  update_liste_echeances_manuelles_accueil();
  update_liste_comptes_accueil ();
  mise_a_jour_soldes_minimaux ();
  mise_a_jour_fin_comptes_passifs();

  remplissage_liste_comptes_etats ();
  selectionne_liste_comptes_etat_courant ();

  modification_fichier( TRUE ); 

}
/* *********************************************************************************************************** */


/* *********************************************************************************************************** */
/* cette fonction est appel�e pour chercher dans les �ch�ances si certaines sont  */
/* associ�es au compte en train d'�tre supprim� */
/* *********************************************************************************************************** */

gint cherche_compte_dans_echeances ( struct operation_echeance *echeance,
				     gint no_compte )
{
  
  return ( echeance -> compte != no_compte );

}
/* *********************************************************************************************************** */






/***********************************************************************************************************/
/* Fonction creation_option_menu_comptes */
/* retourne un option menu contenant les comptes du fichier courant */
/***********************************************************************************************************/

GtkWidget *creation_option_menu_comptes ( void )
{
  GtkWidget *menu;
  GtkWidget *item;

  menu = gtk_menu_new ();

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

  do
    {
      item = gtk_menu_item_new_with_label ( NOM_DU_COMPTE );
      gtk_object_set_data ( GTK_OBJECT ( item ),
			    "no_compte",
			    GINT_TO_POINTER ( p_tab_nom_de_compte_variable - p_tab_nom_de_compte ));
      gtk_signal_connect ( GTK_OBJECT ( item ),
			   "activate",
			   GTK_SIGNAL_FUNC ( changement_choix_compte_echeancier ),
			   NULL );
      gtk_menu_append ( GTK_MENU ( menu ),
			item );
      gtk_widget_show ( item );
      p_tab_nom_de_compte_variable++;
    }
  while ( p_tab_nom_de_compte_variable < (p_tab_nom_de_compte + nb_comptes ) );

  return ( menu );
}
/***********************************************************************************************************/



/***********************************************************************************************************/
void changement_choix_compte_echeancier ( void )
{
  GtkWidget *menu;

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[5] )->menu_item),
											       "no_compte" ));

  if ( gtk_widget_get_style ( widget_formulaire_echeancier[3] ) == style_entree_formulaire[0] )
    {
      /*       il y a qque chose dans le cr�dit, on met le menu des types cr�dit */

      if ( (menu = creation_menu_types ( 2,
					 GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[5] )->menu_item),
										 "no_compte" )),
					 1 )))
	{
	  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[7] ),
				     menu );
	  gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[7] ),
					cherche_no_menu_type_echeancier ( TYPE_DEFAUT_CREDIT ) );
	  gtk_widget_show ( widget_formulaire_echeancier[7] );
	}
      else
	gtk_widget_hide ( widget_formulaire_echeancier[7] );
    }
  else
    {
      /*       il y a qque chose dans le d�bit ou c'est par d�faut, on met le menu des types d�bit */

      if ( (menu = creation_menu_types ( 1,
					 GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[5] )->menu_item),
										 "no_compte" )),
					 1 )))
	{
	  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[7] ),
				     menu );
	  gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[7] ),
					cherche_no_menu_type_echeancier ( TYPE_DEFAUT_DEBIT ) );
	  gtk_widget_show ( widget_formulaire_echeancier[7] );
	}
      else
	gtk_widget_hide ( widget_formulaire_echeancier[7] );
    }
}
/***********************************************************************************************************/



/***********************************************************************************************************/
void creation_types_par_defaut ( gint no_compte,
				 gulong dernier_cheque )
{
  gpointer **save_p_tab;

  save_p_tab = p_tab_nom_de_compte_variable;

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;


  /* si des types d'op� existaient d�j�, on les vire */

  if ( TYPES_OPES )
    g_slist_free ( TYPES_OPES );

  TYPES_OPES = NULL;
  TYPE_DEFAUT_DEBIT = 0;
  TYPE_DEFAUT_CREDIT = 0;

  if ( !TYPE_DE_COMPTE )
    {
      /* c'est un compte bancaire, on ajoute virement, pr�l�vement, ch�que et cb */
      /* 	  modification par rapport � avant, les nouveaux n�: */
      /* 	    1=virement, 2=d�pot, 3=cb, 4=pr�l�vement, 5=ch�que */
      /* les modifs pour chaque op�s se feront � leur chargement */

      struct struct_type_ope *type_ope;

      type_ope = malloc ( sizeof ( struct struct_type_ope ));
      type_ope -> no_type = 1;
      type_ope -> nom_type = g_strdup ( _("Virement") );
      type_ope -> signe_type = 0;
      type_ope -> affiche_entree = 1;
      type_ope -> numerotation_auto = 0;
      type_ope -> no_en_cours = 0;
      type_ope -> no_compte = no_compte;

      TYPES_OPES = g_slist_append ( TYPES_OPES,
				    type_ope );

      type_ope = malloc ( sizeof ( struct struct_type_ope ));
      type_ope -> no_type = 2;
      type_ope -> nom_type = g_strdup ( _("D�p�t") );
      type_ope -> signe_type = 2;
      type_ope -> affiche_entree = 0;
      type_ope -> numerotation_auto = 0;
      type_ope -> no_en_cours = 0;
      type_ope -> no_compte = no_compte;

      TYPES_OPES = g_slist_append ( TYPES_OPES,
				    type_ope );

      type_ope = malloc ( sizeof ( struct struct_type_ope ));
      type_ope -> no_type = 3;
      type_ope -> nom_type = g_strdup ( _("C.B.") );
      type_ope -> signe_type = 1;
      type_ope -> affiche_entree = 0;
      type_ope -> numerotation_auto = 0;
      type_ope -> no_en_cours = 0;
      type_ope -> no_compte = no_compte;

      TYPES_OPES = g_slist_append ( TYPES_OPES,
				    type_ope );

      type_ope = malloc ( sizeof ( struct struct_type_ope ));
      type_ope -> no_type = 4;
      type_ope -> nom_type = g_strdup ( _("Pr�l�vement") );
      type_ope -> signe_type = 1;
      type_ope -> affiche_entree = 0;
      type_ope -> numerotation_auto = 0;
      type_ope -> no_en_cours = 0;
      type_ope -> no_compte = no_compte;

      TYPES_OPES = g_slist_append ( TYPES_OPES,
				    type_ope );

      type_ope = malloc ( sizeof ( struct struct_type_ope ));
      type_ope -> no_type = 5;
      type_ope -> nom_type = g_strdup ( _("Ch�que") );
      type_ope -> signe_type = 1;
      type_ope -> affiche_entree = 1;
      type_ope -> numerotation_auto = 1;
      type_ope -> no_en_cours = dernier_cheque;
      type_ope -> no_compte = no_compte;

      TYPES_OPES = g_slist_append ( TYPES_OPES,
				    type_ope );


      TYPE_DEFAUT_DEBIT = 3;
      TYPE_DEFAUT_CREDIT = 2;

      /* on cr�e le tri pour compte bancaire qui sera 1 2 3 4 5 */

      LISTE_TRI = g_slist_append ( LISTE_TRI,
				   GINT_TO_POINTER ( 1 ));
      LISTE_TRI = g_slist_append ( LISTE_TRI,
				   GINT_TO_POINTER ( 2 ));
      LISTE_TRI = g_slist_append ( LISTE_TRI,
				   GINT_TO_POINTER ( 3 ));
      LISTE_TRI = g_slist_append ( LISTE_TRI,
				   GINT_TO_POINTER ( 4 ));
      LISTE_TRI = g_slist_append ( LISTE_TRI,
				   GINT_TO_POINTER ( 5 ));


    }
  else
    {
      if ( TYPE_DE_COMPTE == 2 )
	{
	  /* c'est un compte de passif, on ne met que le virement */

	  struct struct_type_ope *type_ope;

	  type_ope = malloc ( sizeof ( struct struct_type_ope ));
	  type_ope -> no_type = 1;
	  type_ope -> nom_type = g_strdup ( _("Virement") );
	  type_ope -> signe_type = 0;
	  type_ope -> affiche_entree = 1;
	  type_ope -> numerotation_auto = 0;
	  type_ope -> no_en_cours = 0;
	  type_ope -> no_compte = no_compte;

	  TYPES_OPES = g_slist_append ( TYPES_OPES,
					type_ope );

	  TYPE_DEFAUT_DEBIT = 1;
	  TYPE_DEFAUT_CREDIT = 1;

	  /* on cr�e le tri pour compte passif qui sera 1 */

	  LISTE_TRI = g_slist_append ( LISTE_TRI,
				       GINT_TO_POINTER ( 1 ));
	}
    }

  p_tab_nom_de_compte_variable = save_p_tab;
}
/***********************************************************************************************************/


/***********************************************************************************************************/
/* cette fonction est appel�e lors de la cr�ation d'un nouveau compte */
/* elle renvoie le type demand� pour pouvoir mettre ensuite les types par d�faut */
/***********************************************************************************************************/

gint demande_type_nouveau_compte ( void )
{
  GtkWidget *dialog;
  gint resultat;
  GtkWidget *label;
  GtkWidget *hbox;
  GtkWidget *hbox2;
  GtkWidget *bouton;
  GtkWidget *separateur;
  gint type_compte;

  dialog = gnome_dialog_new ( "Choix du type de compte",
			      GNOME_STOCK_BUTTON_OK,
			      NULL );

  label = gtk_label_new ( "Veuillez choisir le type du compte � cr�er\nCela permet de cr�er les moyens de paiement par d�faut.\nVous pourrez changer ult�rieurement le type de ce compte." );
  gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );

  separateur = gtk_hseparator_new ();
  gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
		       separateur,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( separateur );


/* cr�ation de la ligne du type de compte */

  hbox = gtk_hbox_new ( TRUE,
			0 );
  gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
		       hbox,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( hbox );

  hbox2 = gtk_hbox_new ( FALSE,
			 0 );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       hbox2,
		       FALSE,
		       TRUE,
		       0 );
  gtk_widget_show ( hbox2 );

  label = gtk_label_new ( COLON(_("Type du compte")) );
  gtk_box_pack_start ( GTK_BOX ( hbox2 ),
		       label,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label );


  hbox2 = gtk_hbox_new ( FALSE,
			 0 );
  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       hbox2,
		       FALSE,
		       TRUE,
		       0 );
  gtk_widget_show ( hbox2 );

  bouton = gtk_option_menu_new ();
  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( bouton ),
			     creation_menu_type_compte() );
  gtk_box_pack_start ( GTK_BOX ( hbox2 ),
		       bouton,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( bouton );

  resultat = gnome_dialog_run ( GNOME_DIALOG ( dialog ));

  if ( resultat )
    return ( 0 );

  type_compte = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( bouton ) -> menu_item ),
							"no_type_compte" ));

  gnome_dialog_close ( GNOME_DIALOG ( dialog ));
  return ( type_compte );
}
/***********************************************************************************************************/
