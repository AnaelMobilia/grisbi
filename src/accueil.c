/* ************************************************************************** */
/* fichier qui s'occupe de la page d'accueil ( de d�marrage lors de           */
/* l'ouverture d'un fichier de comptes                                        */
/*                                                                            */
/*                                  accueil.c                                 */
/*                                                                            */
/*     Copyright (C) 2000-2003  C�dric Auger                                  */
/*                              cedric@grisbi.org                             */
/*                              http://www.grisbi.org                         */
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
#include "en_tete.h"

#define show_paddingbox(child) gtk_widget_show_all (gtk_widget_get_parent(gtk_widget_get_parent(gtk_widget_get_parent(GTK_WIDGET(child)))))
#define hide_paddingbox(child) gtk_widget_hide_all (gtk_widget_get_parent(gtk_widget_get_parent(gtk_widget_get_parent(GTK_WIDGET(child)))))

GtkWidget * label_jour;

/* ************************************************************************* */
GtkWidget *creation_onglet_accueil ( void )
{
  GtkWidget *fenetre_accueil, *paddingbox, *base, *base_scroll, *base_box_scroll;
  GtkWidget *hbox, *label, *separateur;
  gchar *nom_utilisateur, tampon_date [50];
  struct passwd *utilisateur;
  time_t date;

/*  la premi�re s�paration : une hbox : � gauche, le logo, � droite le reste */

  fenetre_accueil = gtk_hbox_new ( FALSE, 15 );

  gtk_widget_show ( fenetre_accueil );


  /* cr�ation du logo */

  if ( !chemin_logo
       ||
       !strlen ( chemin_logo ))
    chemin_logo = CHEMIN_LOGO;

  if ( chemin_logo )
    {
      logo_accueil =  gnome_pixmap_new_from_file ( chemin_logo );

      gtk_box_pack_start ( GTK_BOX ( fenetre_accueil ),
			   logo_accueil,
			   FALSE,
			   FALSE,
			   20 );
      gtk_widget_show ( logo_accueil );

      /* s�paration gauche-droite */
/*
      separateur = gtk_vseparator_new ();
      gtk_box_pack_start ( GTK_BOX ( fenetre_accueil ),
			   separateur,
			   FALSE,
			   FALSE,
			   0 );
      gtk_widget_show ( separateur );
*/
    }
  else
    logo_accueil = NULL;

/* cr�ation de la partie droite */

  base = gtk_vbox_new ( FALSE,
			15 );
  gtk_box_pack_end ( GTK_BOX ( fenetre_accueil ),
		       base,
		       TRUE,
		       TRUE,
		       0 );
  gtk_widget_show ( base );


  /*   la partie tout en haut affiche la date, le nom et l'heure */

  hbox = gtk_hbox_new ( FALSE,
				  5 );
  gtk_box_pack_start ( GTK_BOX ( base ),
		       hbox,
		       FALSE,
		       FALSE,
		       10 );
  gtk_widget_show ( hbox );


  /* r�cup�re le nom de l'utilisateur, si nul, met le login */

  utilisateur = getpwuid ( getuid () );

  if ( ! utilisateur )
    {
      utilisateur = getpwuid ( 65534 );
    }

  if ( utilisateur )
    {
      if ( !strlen ( nom_utilisateur = g_strdelimit ( utilisateur->pw_gecos, ",", 0 ) ) )
	nom_utilisateur = utilisateur->pw_name;
    }
  else
    {
      nom_utilisateur = _("No user");
    }

  label = gtk_label_new ( g_strconcat ( COLON(_("User")), nom_utilisateur, NULL) );

  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       label,
		       TRUE,
		       FALSE,
		       5 );
  gtk_widget_show ( label );


  label_jour = gtk_label_new ( "" );
  gtk_misc_set_alignment ( GTK_MISC (label_jour ), 1, 1);

  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       label_jour,
		       TRUE,
 		       TRUE,
		       5 );
  gtk_widget_show ( label_jour );

  label_temps = gtk_label_new ( "" );

  gtk_box_pack_start ( GTK_BOX ( hbox ),
		       label_temps,
		       TRUE,
		       FALSE,
		       5 );
  gtk_widget_show ( label_temps );

  change_temps(label_temps);

  id_temps = gtk_timeout_add ( 1000,
			       (GtkFunction) change_temps,
			       GTK_WIDGET ( label_temps ));


  /*   en dessous, on met le titre du fichier s'il existe */

  if ( titre_fichier )
    {
      label_titre_fichier = gtk_label_new ( titre_fichier );
      gtk_label_set_markup ( GTK_LABEL ( label_titre_fichier ), 
			     g_strconcat ("<span size=\"x-large\">",
					  titre_fichier, "</span>", NULL ) );
      
    }
  else
    {
      label_titre_fichier = gtk_label_new ( "" );
    }

  gtk_box_pack_start ( GTK_BOX ( base ),
		       label_titre_fichier,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( label_titre_fichier );

  /* s�paration haut-bas */
/*
  separateur = gtk_hseparator_new ();
  gtk_box_pack_start ( GTK_BOX ( base ),
		       separateur,
		       FALSE,
		       FALSE,
		       0 );
  gtk_widget_show ( separateur );
*/

  /* on cr�e � ce niveau base_scroll qui est aussi une vbox mais qui peut scroller verticalement */

  base_scroll = gtk_scrolled_window_new ( NULL,
					  NULL);
  gtk_container_set_border_width ( GTK_CONTAINER ( base_scroll ),
				   5 );
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( base_scroll ),
				   GTK_POLICY_NEVER,
				   GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start (GTK_BOX ( base ),
		      base_scroll,
		      TRUE,
		      TRUE,
		      0);
  gtk_widget_show (base_scroll);
  

  /* on met la nouvelle vbox dans le fenetre scrollable */

  base_box_scroll = gtk_vbox_new ( FALSE, 0 );
  gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW (base_scroll),
					  base_box_scroll);

  gtk_viewport_set_shadow_type ( GTK_VIEWPORT ( GTK_BIN (base_scroll)  -> child ),
				 GTK_SHADOW_NONE );
  gtk_container_set_border_width ( GTK_CONTAINER ( base_box_scroll ),
				   10 );
  gtk_widget_show (base_box_scroll);


  /* on cr�e la premi�re frame dans laquelle on met les �tats des comptes */
  paddingbox = new_paddingbox_with_title ( base_box_scroll, FALSE,
					   _("Account balances") );
/*   frame_etat_comptes_accueil = gtk_frame_new (""); */
  frame_etat_comptes_accueil = gtk_notebook_new ();
  gtk_notebook_set_show_tabs ( GTK_NOTEBOOK(frame_etat_comptes_accueil), FALSE );
  gtk_notebook_set_show_border ( GTK_NOTEBOOK(frame_etat_comptes_accueil), FALSE );
  gtk_container_set_border_width ( GTK_CONTAINER(frame_etat_comptes_accueil), 0 );
  gtk_box_set_spacing ( GTK_BOX(paddingbox), 6 );
  gtk_box_pack_start ( GTK_BOX(paddingbox), frame_etat_comptes_accueil,
		       FALSE, FALSE, 0 );
  show_paddingbox (frame_etat_comptes_accueil);
  /* on met la liste des comptes et leur �tat dans la frame */
  update_liste_comptes_accueil ();


  /* mise en place de la partie fin des comptes passif */
  paddingbox = new_paddingbox_with_title ( base_box_scroll, FALSE,
					   _("Closed liabilities accounts") );
  frame_etat_fin_compte_passif = gtk_notebook_new ();
  gtk_notebook_set_show_tabs ( GTK_NOTEBOOK(frame_etat_fin_compte_passif), FALSE );
  gtk_notebook_set_show_border ( GTK_NOTEBOOK(frame_etat_fin_compte_passif), FALSE );
  gtk_box_pack_start ( GTK_BOX(paddingbox), frame_etat_fin_compte_passif,
		       FALSE, FALSE, 0 );
  mise_a_jour_fin_comptes_passifs ();


  /* mise en place de la partie des �ch�ances manuelles ( non affich� ) */
  paddingbox = new_paddingbox_with_title ( base_box_scroll, FALSE,
					  _("Manual scheduled transactions at maturity date") );
  frame_etat_echeances_manuelles_accueil = gtk_notebook_new ();
  gtk_notebook_set_show_tabs ( GTK_NOTEBOOK(frame_etat_echeances_manuelles_accueil), FALSE );
  gtk_notebook_set_show_border ( GTK_NOTEBOOK(frame_etat_echeances_manuelles_accueil), FALSE );
  gtk_container_set_border_width ( GTK_CONTAINER(frame_etat_echeances_manuelles_accueil), 0 );
  gtk_box_set_spacing ( GTK_BOX(paddingbox), 6 );
  gtk_box_pack_start ( GTK_BOX(paddingbox), 
		       frame_etat_echeances_manuelles_accueil,
		       FALSE, FALSE, 6 );


  /* mise en place de la partie des �ch�ances auto  ( non affich� )*/
  paddingbox = new_paddingbox_with_title ( base_box_scroll, FALSE,
					  _("Automatic scheduled transactions entered") );
  frame_etat_echeances_auto_accueil = gtk_notebook_new ();
  gtk_notebook_set_show_tabs ( GTK_NOTEBOOK(frame_etat_echeances_auto_accueil), FALSE );
  gtk_notebook_set_show_border ( GTK_NOTEBOOK(frame_etat_echeances_auto_accueil), FALSE );
  gtk_container_set_border_width ( GTK_CONTAINER(frame_etat_echeances_auto_accueil), 0 );
  gtk_box_set_spacing ( GTK_BOX(paddingbox), 6 );
  gtk_box_pack_start ( GTK_BOX(paddingbox), 
		       frame_etat_echeances_auto_accueil,
		       FALSE, FALSE, 6 );

  
  /* partie des fin d'�ch�ances */
  paddingbox = new_paddingbox_with_title ( base_box_scroll, FALSE,
					   _("Closed scheduled transactions") );
  frame_etat_echeances_finies = gtk_notebook_new ();
  gtk_notebook_set_show_tabs ( GTK_NOTEBOOK(frame_etat_echeances_finies), FALSE );
  gtk_notebook_set_show_border ( GTK_NOTEBOOK(frame_etat_echeances_finies), FALSE );
  gtk_container_set_border_width ( GTK_CONTAINER(frame_etat_echeances_finies), 0 );
  gtk_box_set_spacing ( GTK_BOX(paddingbox), 6 );
  gtk_box_pack_start ( GTK_BOX(paddingbox), 
		       frame_etat_echeances_finies,
		       FALSE, FALSE, 6 );


  /* partie des soldes minimaux autoris�s */
  paddingbox = new_paddingbox_with_title ( base_box_scroll, FALSE,
					   _("Accounts under authorized balance") );
  frame_etat_soldes_minimaux_autorises = gtk_notebook_new ();
  gtk_notebook_set_show_tabs ( GTK_NOTEBOOK(frame_etat_soldes_minimaux_autorises), FALSE );
  gtk_notebook_set_show_border ( GTK_NOTEBOOK(frame_etat_soldes_minimaux_autorises), FALSE );
  gtk_container_set_border_width ( GTK_CONTAINER(frame_etat_soldes_minimaux_autorises), 0 );
  gtk_box_set_spacing ( GTK_BOX(paddingbox), 6 );
  gtk_box_pack_start ( GTK_BOX(paddingbox), 
		       frame_etat_soldes_minimaux_autorises,
		       FALSE, FALSE, 6 );


  /* partie des soldes minimaux voulus */
  paddingbox = new_paddingbox_with_title ( base_box_scroll, FALSE,
					   _("Accounts under wanted balance") );
  frame_etat_soldes_minimaux_voulus = gtk_notebook_new ();
  gtk_notebook_set_show_tabs ( GTK_NOTEBOOK(frame_etat_soldes_minimaux_voulus),
			       FALSE );
  gtk_notebook_set_show_border ( GTK_NOTEBOOK(frame_etat_soldes_minimaux_voulus),
				 FALSE );
  gtk_container_set_border_width ( GTK_CONTAINER(frame_etat_soldes_minimaux_voulus), 0 );
  gtk_box_set_spacing ( GTK_BOX(paddingbox), 6 );
  gtk_box_pack_start ( GTK_BOX(paddingbox), 
		       frame_etat_soldes_minimaux_voulus,
		       FALSE, FALSE, 6 );

  mise_a_jour_soldes_minimaux ();

  return ( fenetre_accueil );
}
/* ************************************************************************* */



/* ************************************************************************* */
void change_temps ( GtkWidget *label_temps )
{
  gchar tampon_date [50], * tampon;
  time_t date;
  GError *error = NULL;

  time ( &date );
  strftime ( tampon_date,
	     (size_t) 50,
	     "%X",
	     (const struct tm *) localtime ( &date ) );

  /* Convert to UTF-8 */
  tampon = g_convert (tampon_date, strlen(tampon_date), 
		      "UTF-8", "ISO-8859-1", 
		      NULL, NULL, &error);

  gtk_label_set_text ( GTK_LABEL (label_temps ),
		       tampon );

  strftime ( tampon_date,
	     (size_t) 50,
	     "%A %d %B %Y",
	     (const struct tm *) localtime ( &date ) );
  /* Convert to UTF-8 */
  tampon = g_convert (tampon_date, strlen(tampon_date), 
		      "UTF-8", "ISO-8859-1", 
		      NULL, NULL, &error);
  /* Capitalize */
  tampon[0] = toupper ( tampon[0] );

  gtk_label_set_text ( GTK_LABEL (label_jour ),
		       tampon );

}
/* ************************************************************************* */



/* ************************************************************************* */
/*        Fonction appel�e lorsqu'on clicke sur une �ch�ance � saisir        */
/* ************************************************************************* */

gboolean saisie_echeance_accueil ( GtkWidget *event_box,
				   GdkEventButton *event,
				   struct operation_echeance *echeance )
{
  GtkWidget *ancien_parent;
  struct operation_echeance *ancienne_selection_echeance;
  GtkWidget *dialog;
  gint resultat;

  /* on s�lectionne l'�ch�ance demand�e */

  ancienne_selection_echeance = echeance_selectionnnee;
  echeance_selectionnnee = echeance;

  ancien_parent = formulaire_echeancier -> parent;

  /* cr�e la boite de dialogue */

  dialog = gnome_dialog_new ( _("Enter a scheduled transaction"),
			      GNOME_STOCK_BUTTON_OK,
			      GNOME_STOCK_BUTTON_CANCEL,
			      NULL );
  gtk_widget_set_usize ( GTK_WIDGET ( dialog ),
			 600,
			 FALSE );
  gtk_signal_connect ( GTK_OBJECT ( dialog ),
		       "destroy",
		       GTK_SIGNAL_FUNC ( gtk_signal_emit_stop_by_name ),
		        "destroy" );
  gtk_window_set_transient_for ( GTK_WINDOW ( dialog ),
				 GTK_WINDOW ( window ));
  gnome_dialog_set_default ( GNOME_DIALOG ( dialog ),
			     0 );


  /* met le formulaire dans la boite de dialogue */

  gtk_widget_unrealize ( frame_formulaire_echeancier );
  gtk_widget_reparent ( formulaire_echeancier,
			GNOME_DIALOG ( dialog ) -> vbox );


/* applique aux entry l'entr�e � ok */

  gnome_dialog_editable_enters ( GNOME_DIALOG ( dialog ),
				 GTK_EDITABLE ( widget_formulaire_echeancier [0] ));
  gnome_dialog_editable_enters ( GNOME_DIALOG ( dialog ),
				 GTK_EDITABLE ( GTK_COMBOFIX ( widget_formulaire_echeancier [1] ) -> entry ));
  gnome_dialog_editable_enters ( GNOME_DIALOG ( dialog ),
				 GTK_EDITABLE ( widget_formulaire_echeancier [2] ));
  gnome_dialog_editable_enters ( GNOME_DIALOG ( dialog ),
				 GTK_EDITABLE ( widget_formulaire_echeancier [3] ));
  gnome_dialog_editable_enters ( GNOME_DIALOG ( dialog ),
				 GTK_EDITABLE ( GTK_COMBOFIX ( widget_formulaire_echeancier [6] ) -> entry ));
  gnome_dialog_editable_enters ( GNOME_DIALOG ( dialog ),
				 GTK_EDITABLE ( widget_formulaire_echeancier [8] ));
  gnome_dialog_editable_enters ( GNOME_DIALOG ( dialog ),
				 GTK_EDITABLE ( GTK_COMBOFIX ( widget_formulaire_echeancier [10] ) -> entry ));
  gnome_dialog_editable_enters ( GNOME_DIALOG ( dialog ),
				 GTK_EDITABLE ( widget_formulaire_echeancier [11] ));
  gnome_dialog_editable_enters ( GNOME_DIALOG ( dialog ),
				 GTK_EDITABLE ( widget_formulaire_echeancier [12] ));
  gnome_dialog_editable_enters ( GNOME_DIALOG ( dialog ),
				 GTK_EDITABLE ( widget_formulaire_echeancier [14] ));


  /* enl�ve les boutons valider/annuler si necessaire */

  if ( etat.affiche_boutons_valider_annuler )
    {
      gtk_widget_hide (hbox_valider_annuler_echeance);
    }

  /* remplit le formulaire */

  click_sur_saisir_echeance();


  etat.formulaire_echeance_dans_fenetre = 1;
  dialogue_echeance = dialog;

  resultat = gnome_dialog_run ( GNOME_DIALOG ( dialog ));

  if ( !resultat )
    fin_edition_echeance ();

  gtk_widget_reparent ( formulaire_echeancier,
			ancien_parent );

  gnome_dialog_close ( GNOME_DIALOG ( dialog ));

  etat.formulaire_echeance_dans_fenetre = 0;

  /* remet les boutons valider/annuler si necessaire */

  if ( etat.affiche_boutons_valider_annuler )
    {
      gtk_widget_show (separateur_formulaire_echeancier);
      gtk_widget_show (hbox_valider_annuler_echeance);
    }


  formulaire_echeancier_a_zero();

  echeance_selectionnnee = ancienne_selection_echeance;


  if ( !etat.formulaire_echeancier_toujours_affiche )
    gtk_widget_hide ( frame_formulaire_echeancier );

  return FALSE;
}
/* ************************************************************************* */

/* ************************************************************************* */
gboolean met_en_prelight ( GtkWidget *event_box,
			   GdkEventMotion *event,
			   gpointer pointeur )
{
  gtk_widget_set_state ( GTK_WIDGET ( GTK_BIN (event_box)->child ),
			 GTK_STATE_PRELIGHT );

  return FALSE;
}
/* ************************************************************************* */

/* ************************************************************************* */
gboolean met_en_normal ( GtkWidget *event_box,
			 GdkEventMotion *event,
			 gpointer pointeur )
{
  gtk_widget_set_state ( GTK_WIDGET ( GTK_BIN (event_box)->child ),
			 GTK_STATE_NORMAL );

  return FALSE;
}
/* ************************************************************************* */

/* ************************************************************************* */
/* Fonction update_liste_comptes_accueil                                     */
/* affiche la liste des comptes et leur solde courant dans la frame qui leur */
/* est r�serv�e dans l'accueil                                               */
/* ************************************************************************* */


void update_liste_comptes_accueil ( void )
{
  GtkWidget *pTable,
	    *pEventBox,
	    *pLabel;
  
  GdkColor CouleurSoldeAlarmeVerteNormal,
	   CouleurSoldeAlarmeVertePrelight,
	   CouleurSoldeAlarmeOrangeNormal,
	   CouleurSoldeAlarmeOrangePrelight,
	   CouleurSoldeAlarmeRougeNormal,
	   CouleurSoldeAlarmeRougePrelight,
	   CouleurNomCompteNormal,
	   CouleurNomComptePrelight;
  
  GtkStyle *pStyleLabelNomCompte,
	   *pStyleLabelSoldeCourant,
	   *pStyleLabelSoldePointe;
  
  GSList *ordre_comptes_variable,
	 *liste_operations_tmp;
  
  gdouble montant,
	  solde_global_courant,
	  solde_global_pointe;
  
  gint i;

  
  
  if ( !nb_comptes )
    return;

  /* Initialisation des couleurs des diff�rents labels */
  /* Pourra �tre int�gr� � la configuration g�n�rale */
  CouleurSoldeAlarmeVerteNormal.red =     0.00 * 65535 ;
  CouleurSoldeAlarmeVerteNormal.green =   0.50 * 65535 ;
  CouleurSoldeAlarmeVerteNormal.blue =    0.00 * 65535 ;
  CouleurSoldeAlarmeVertePrelight.red =   0.00 * 65535 ;
  CouleurSoldeAlarmeVertePrelight.green = 0.90 * 65535 ;
  CouleurSoldeAlarmeVertePrelight.blue =  0.00 * 65535 ;

  CouleurSoldeAlarmeOrangeNormal.red =     0.90 * 65535 ;
  CouleurSoldeAlarmeOrangeNormal.green =   0.60 * 65535 ;
  CouleurSoldeAlarmeOrangeNormal.blue =    0.00 * 65535 ;
  CouleurSoldeAlarmeOrangePrelight.red =   1.00 * 65535 ;
  CouleurSoldeAlarmeOrangePrelight.green = 0.80 * 65535 ;
  CouleurSoldeAlarmeOrangePrelight.blue =  0.00 * 65535 ;

  CouleurSoldeAlarmeRougeNormal.red =     0.60 * 65535 ;
  CouleurSoldeAlarmeRougeNormal.green =   0.00 * 65535 ;
  CouleurSoldeAlarmeRougeNormal.blue =    0.00 * 65535 ;
  CouleurSoldeAlarmeRougePrelight.red =   1.00 * 65535 ;
  CouleurSoldeAlarmeRougePrelight.green = 0.00 * 65535 ;
  CouleurSoldeAlarmeRougePrelight.blue =  0.00 * 65535 ;

  CouleurNomCompteNormal.red =     0.00 * 65535 ;
  CouleurNomCompteNormal.green =   0.00 * 65535 ;
  CouleurNomCompteNormal.blue =    0.00 * 65535 ;
  CouleurNomComptePrelight.red =   0.61 * 65535 ;
  CouleurNomComptePrelight.green = 0.61 * 65535 ;
  CouleurNomComptePrelight.blue =  0.61 * 65535 ;

  /* Cr�ation d'un label juste pour en r�cup�rer le style */
  pLabel = gtk_label_new ("");
  
  /* Initialisation du style � Nom du compte � */
  pStyleLabelNomCompte = gtk_style_copy ( gtk_widget_get_style (pLabel));
  pStyleLabelNomCompte->fg[GTK_STATE_NORMAL] = CouleurNomCompteNormal;
  pStyleLabelNomCompte->fg[GTK_STATE_PRELIGHT] = CouleurNomComptePrelight;

  /* Cr�ation du cadre principal */
  gtk_notebook_remove_page ( GTK_NOTEBOOK (frame_etat_comptes_accueil), 0 );

  /* Cr�ation du tableau dans lequel seront stock�s les comptes avec leur     */
  /* solde.                                                                   */
  /* ATTENTION : ce tableau est cr�� de fa�on � pouvoir afficher la somme des */
  /* soldes de tous les comptes.                                              */
  /* Il faut donc TOUS les comptes soient dans la M�ME DEVISE !!!!!!!         */
  /* Il serait donc utile d'effectuer un test sur les devises utilis�es,      */
  /* et si les comptes utilisaient des devises diff�rentes, il faudrait alors */
  /* soit renoncer � afficher le solde global ;                               */
  /* soit effectuer une conversion des soldes des tous les comptes vers une   */
  /* m�me devise qu'il faudrait �galement d�finir.                            */
  /* Bref, pas simple.                                                        */
  pTable = gtk_table_new ( nb_comptes + 4,
			   8,
			   FALSE );
  gtk_container_add ( GTK_CONTAINER ( frame_etat_comptes_accueil ),
		      pTable );
  gtk_widget_show ( pTable );

  /* Cr�ation et remplissage de la premi�re ligne du tableau */
  pLabel = gtk_label_new (_("Current balance"));
  gtk_misc_set_alignment ( GTK_MISC ( pLabel ), 0.5, 0.5);
  gtk_table_attach_defaults ( GTK_TABLE ( pTable ),
			      pLabel,
			      2, 4,
			      0, 1 );
  gtk_widget_show ( pLabel );
  pLabel = gtk_label_new (_("Reconciled balance"));
  gtk_misc_set_alignment ( GTK_MISC ( pLabel ), 0.5, 0.5);
  gtk_table_attach_defaults ( GTK_TABLE ( pTable ),
			      pLabel,
			      5, 7,
			      0, 1 );
  gtk_widget_show ( pLabel );

  /* Affichage des comptes et de leur solde */
  ordre_comptes_variable = ordre_comptes;
  i = 1;
  solde_global_courant = 0 ;
  solde_global_pointe = 0 ;

  /* Pour chaque compte non clotur� (pour chaque ligne), */
  /* cr�er toutes les colonnes et les remplir            */
  do
    {
      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + GPOINTER_TO_INT ( ordre_comptes_variable->data );

      if ( !COMPTE_CLOTURE )
	{
	  /* Premi�re colonne : vide */
	  pLabel = gtk_label_new ( g_strconcat ( (gchar *) NOM_DU_COMPTE, " : ", NULL ));
	  gtk_misc_set_alignment ( GTK_MISC ( pLabel ), 0, 0.5);
	  gtk_widget_set_style ( pLabel,
				 pStyleLabelNomCompte );

	  /* Est-il r�ellement n�cessaire de cr�er des labels vides pour */
	  /* remplir des colonnes vides d'un tableau ?                   */
	  pLabel = gtk_label_new ( "" );
	  gtk_table_attach_defaults ( GTK_TABLE ( pTable ),
				      pLabel,
				      0, 1,
				      i, i+1 );
	  gtk_widget_show ( pLabel );

	  /* Deuxi�me colonne : elle contient le nom du compte */
	  pLabel = gtk_label_new ( g_strconcat ( (gchar *) NOM_DU_COMPTE, " : ", NULL ));
	  gtk_misc_set_alignment ( GTK_MISC ( pLabel ), 0, 0.5);
	  gtk_widget_set_style ( pLabel,
				 pStyleLabelNomCompte );

	  /* Cr�ation d'une boite � �v�nement qui sera rattach�e au nom du compte */
	  pEventBox = gtk_event_box_new ();
	  gtk_signal_connect ( GTK_OBJECT ( pEventBox ),
			       "enter-notify-event",
			       GTK_SIGNAL_FUNC ( met_en_prelight ),
			       NULL );
	  gtk_signal_connect ( GTK_OBJECT ( pEventBox ),
			       "leave-notify-event",
			       GTK_SIGNAL_FUNC ( met_en_normal ),
			       NULL );
	  gtk_signal_connect_object ( GTK_OBJECT ( pEventBox ),
				      "button-press-event",
				      GTK_SIGNAL_FUNC ( changement_compte ),
				      GINT_TO_POINTER ( ordre_comptes_variable->data ) );
	  gtk_table_attach ( GTK_TABLE ( pTable ),
			     pEventBox,
			     1, 2,
			     i, i+1,
			     GTK_FILL |GTK_SHRINK,
			     GTK_FILL| GTK_SHRINK,
			     20, 0 );
	  gtk_widget_show ( pEventBox );

	  gtk_container_add ( GTK_CONTAINER ( pEventBox ),
			      pLabel );
	  gtk_widget_show ( pLabel );

	  /* Troisi�me colonne : elle contient le solde courant du compte */
	  pLabel = gtk_label_new ( g_strdup_printf ( "%4.2f",
						    SOLDE_COURANT ));
	  gtk_misc_set_alignment ( GTK_MISC ( pLabel ), 1, 0.5);
	  
	  /* Mise en place du style du label en fonction du solde courant */
	  pStyleLabelSoldeCourant = gtk_style_copy ( gtk_widget_get_style (pLabel));
	  if ( SOLDE_COURANT >= SOLDE_MINI_VOULU )
	    {
	     pStyleLabelSoldeCourant->fg[GTK_STATE_NORMAL] = CouleurSoldeAlarmeVerteNormal;
	     pStyleLabelSoldeCourant->fg[GTK_STATE_PRELIGHT] = CouleurSoldeAlarmeVertePrelight;
	    }
	  else
	    {
	     if ( SOLDE_COURANT >= SOLDE_MINI )
	       {
		pStyleLabelSoldeCourant->fg[GTK_STATE_NORMAL] = CouleurSoldeAlarmeOrangeNormal;
		pStyleLabelSoldeCourant->fg[GTK_STATE_PRELIGHT] = CouleurSoldeAlarmeOrangePrelight;
	       }
	     else
	       {
		pStyleLabelSoldeCourant->fg[GTK_STATE_NORMAL] = CouleurSoldeAlarmeRougeNormal;
		pStyleLabelSoldeCourant->fg[GTK_STATE_PRELIGHT] = CouleurSoldeAlarmeRougePrelight;
	       }
	    }
	  gtk_widget_set_style ( pLabel,
				 pStyleLabelSoldeCourant );

	  /* Cr�ation d'une boite � �v�nement qui sera rattach�e au solde courant du compte */
	  pEventBox = gtk_event_box_new ();
	  gtk_signal_connect ( GTK_OBJECT ( pEventBox ),
			       "enter-notify-event",
			       GTK_SIGNAL_FUNC ( met_en_prelight ),
			       NULL );
	  gtk_signal_connect ( GTK_OBJECT ( pEventBox ),
			       "leave-notify-event",
			       GTK_SIGNAL_FUNC ( met_en_normal ),
			       NULL );
	  gtk_signal_connect_object ( GTK_OBJECT ( pEventBox ),
				      "button-press-event",
				      GTK_SIGNAL_FUNC ( changement_compte ),
				      GINT_TO_POINTER ( ordre_comptes_variable->data ));
	  gtk_table_attach ( GTK_TABLE ( pTable ),
			     pEventBox,
			     2, 3,
			     i, i+1,
			     GTK_FILL| GTK_SHRINK,
			     GTK_FILL| GTK_SHRINK,
			     0, 0 );
	  gtk_widget_show ( pEventBox );

	  gtk_container_add ( GTK_CONTAINER ( pEventBox ),
			      pLabel );
	  gtk_widget_show ( pLabel );

	  /* Quatri�me colonne : elle contient le symbole de la devise du compte */
	  pLabel = gtk_label_new ( devise_name ( g_slist_find_custom ( liste_struct_devises,
								       GINT_TO_POINTER ( DEVISE ),
								       (GCompareFunc) recherche_devise_par_no ) -> data ) );
	  gtk_misc_set_alignment ( GTK_MISC ( pLabel ), 0, 0.5);
	  gtk_table_attach ( GTK_TABLE ( pTable ),
			     pLabel,
			     3, 4,
			     i, i+1,
			     GTK_FILL | GTK_SHRINK,
			     GTK_FILL | GTK_SHRINK,
			     5, 0 );
	  gtk_widget_show ( pLabel );

	  /* Cinqui�me colonne : vide */
	  pLabel = gtk_label_new ("");
	  gtk_table_attach_defaults ( GTK_TABLE ( pTable ),
				      pLabel,
				      4, 5,
				      i, i+1 );
	  gtk_widget_show ( pLabel );

	  /* Calcul du solde point� */
	  /* Je ne sais plus pourquoi on fait ce calcul, sans doute le solde point� */
	  /* n'est-il pas disponible en tant que variable globale */
	  solde_pointe_affichage_liste = SOLDE_INIT;

	  /* on commence la boucle : fait le tour de toutes les op�rations */
	  /* met � jour les solde_courant_affichage_liste et solde_pointe_affichage_liste */
	  /* affiche l'op�ration � l'�cran en fonction de l'affichage de R */
	  liste_operations_tmp = LISTE_OPERATIONS;

	  while ( liste_operations_tmp )
	    {
	      struct structure_operation *operation;

	      operation = liste_operations_tmp -> data;

	      /* si c'est une op� de ventilation, on la saute */
	      if ( !operation -> no_operation_ventilee_associee )
		{
		  /* quelle que soit l'op�ration (relev�e ou non), on calcule les soldes courant */
		  montant = calcule_montant_devise_renvoi ( operation -> montant,
							    DEVISE,
							    operation -> devise,
							    operation -> une_devise_compte_egale_x_devise_ope,
							    operation -> taux_change,
							    operation -> frais_change );

	  	  /* si l'op� est point�e ou relev�e, on ajoute ce montant au solde point� */
		  if ( operation -> pointe )
		    solde_pointe_affichage_liste = solde_pointe_affichage_liste + montant;
		}
	      liste_operations_tmp = liste_operations_tmp -> next;
	    }

	  /* on enregistre le solde final */
	  SOLDE_POINTE = solde_pointe_affichage_liste;

	  /* Sixi�me colonne : elle contient le solde point� du compte */
	  pLabel = gtk_label_new ( g_strdup_printf ( "%4.2f",
						    SOLDE_POINTE ));
	  gtk_misc_set_alignment ( GTK_MISC ( pLabel ), 1, 0.5);
	  
	  /* Mise en place du style du label en fonction du solde point� */
	  pStyleLabelSoldePointe = gtk_style_copy ( gtk_widget_get_style (pLabel));
	  if ( SOLDE_POINTE >= SOLDE_MINI_VOULU )
	    {
	     pStyleLabelSoldePointe->fg[GTK_STATE_NORMAL] = CouleurSoldeAlarmeVerteNormal;
	     pStyleLabelSoldePointe->fg[GTK_STATE_PRELIGHT] = CouleurSoldeAlarmeVertePrelight;
	    }
	  else
	    {
	     if ( SOLDE_POINTE >= SOLDE_MINI )
	       {
		pStyleLabelSoldePointe->fg[GTK_STATE_NORMAL] = CouleurSoldeAlarmeOrangeNormal;
		pStyleLabelSoldePointe->fg[GTK_STATE_PRELIGHT] = CouleurSoldeAlarmeOrangePrelight;
	       }
	     else
	       {
		pStyleLabelSoldePointe->fg[GTK_STATE_NORMAL] = CouleurSoldeAlarmeRougeNormal;
		pStyleLabelSoldePointe->fg[GTK_STATE_PRELIGHT] = CouleurSoldeAlarmeRougePrelight;
	       }
	    }
	  gtk_widget_set_style ( pLabel,
				 pStyleLabelSoldePointe );

	  /* Cr�ation d'une boite � �v�nement qui sera rattach�e au solde point� du compte */
	  pEventBox = gtk_event_box_new ();
	  gtk_signal_connect ( GTK_OBJECT ( pEventBox ),
			       "enter-notify-event",
			       GTK_SIGNAL_FUNC ( met_en_prelight ),
			       NULL );
	  gtk_signal_connect ( GTK_OBJECT ( pEventBox ),
			       "leave-notify-event",
			       GTK_SIGNAL_FUNC ( met_en_normal ),
			       NULL );
	  gtk_signal_connect_object ( GTK_OBJECT ( pEventBox ),
				      "button-press-event",
				      GTK_SIGNAL_FUNC ( changement_compte ),
				      GINT_TO_POINTER ( ordre_comptes_variable->data ));
	  gtk_table_attach ( GTK_TABLE ( pTable ),
			     pEventBox,
			     5, 6,
			     i, i+1,
			     GTK_FILL| GTK_SHRINK,
			     GTK_FILL| GTK_SHRINK,
			     0, 0 );
	  gtk_widget_show ( pEventBox );

	  gtk_container_add ( GTK_CONTAINER ( pEventBox ),
			      pLabel );
	  gtk_widget_show ( pLabel );

	  /* Septi�me colonne : elle contient le symbole de la devise du compte */
	  pLabel = gtk_label_new ( devise_name((struct struct_devise *)(g_slist_find_custom ( liste_struct_devises,
									  GINT_TO_POINTER ( DEVISE ),
									  (GCompareFunc) recherche_devise_par_no )-> data )));
	  gtk_misc_set_alignment ( GTK_MISC ( pLabel ), 0, 0.5);
	  gtk_table_attach ( GTK_TABLE ( pTable ),
			     pLabel,
			     6, 7,
			     i, i+1,
			     GTK_FILL | GTK_SHRINK,
			     GTK_FILL | GTK_SHRINK,
			     5, 0 );
	  gtk_widget_show ( pLabel );

	  /* Huiti�me colonne : vide */
	  pLabel = gtk_label_new ("");
	  gtk_table_attach_defaults ( GTK_TABLE ( pTable ),
				      pLabel,
				      7, 8,
				      i, i+1 );
	  gtk_widget_show ( pLabel );
	}

      i++;
      
      /* ATTENTION : les sommes effectu�es ici pr�supposent que */
      /* TOUS les comptes sont dans la M�ME DEVISE !!!!!        */
      solde_global_courant += SOLDE_COURANT ;
      solde_global_pointe += SOLDE_POINTE ;
    }
  while ( ( ordre_comptes_variable = ordre_comptes_variable->next ) );

  /* Cr�ation de la (nb_comptes + 2)i�me ligne du tableau : vide */
  pLabel = gtk_label_new ( "" );
  gtk_table_attach_defaults ( GTK_TABLE ( pTable ),
			      pLabel,
			      0, 1,
			      i, i+1 );
  gtk_widget_show ( pLabel );

  /* Cr�ation et remplissage de la (nb_comptes + 3)i�me ligne du tableau : */
  /* elle contient la somme des soldes de chaque compte */
  /* Deuxi�me colonne */
  pLabel = gtk_label_new (COLON(_("Global balances")));
  gtk_misc_set_alignment ( GTK_MISC ( pLabel ),
			   0,
			   0.5);
  gtk_table_attach ( GTK_TABLE ( pTable ),
			     pLabel,
			     1, 2,
			     i+1, i+2,
			     GTK_FILL |GTK_SHRINK,
			     GTK_FILL| GTK_SHRINK,
			     20, 0 );
  gtk_widget_show ( pLabel );

  /* Troisi�me colonne : elle contient le solde total courant des comptes */
  pLabel = gtk_label_new ( g_strdup_printf ( "%4.2f",
					    solde_global_courant ));
  gtk_misc_set_alignment ( GTK_MISC ( pLabel ), 1, 0.5);
  gtk_table_attach ( GTK_TABLE ( pTable ),
		     pLabel,
		     2, 3,
		     i+1, i+2,
		     GTK_FILL | GTK_SHRINK,
		     GTK_FILL | GTK_SHRINK,
		     0, 0 );
  gtk_widget_show ( pLabel );

  /* Quatri�me colonne : elle contient le symbole de la devise du compte */
  pLabel = gtk_label_new ( devise_name((struct struct_devise *)(g_slist_find_custom ( liste_struct_devises,
									  GINT_TO_POINTER ( DEVISE ),
									  (GCompareFunc) recherche_devise_par_no )-> data )) );
  gtk_misc_set_alignment ( GTK_MISC ( pLabel ), 0, 0.5);
  gtk_table_attach ( GTK_TABLE ( pTable ),
		     pLabel,
		     3, 4,
		     i+1, i+2,
		     GTK_FILL | GTK_SHRINK,
		     GTK_FILL | GTK_SHRINK,
		     5, 0 );
  gtk_widget_show ( pLabel );

  /* Sixi�me colonne : elle contient le solde total point� des comptes */
  pLabel = gtk_label_new ( g_strdup_printf ( "%4.2f",
					    solde_global_pointe ));
  gtk_misc_set_alignment ( GTK_MISC ( pLabel ), 1, 0.5);
  gtk_table_attach ( GTK_TABLE ( pTable ),
		     pLabel,
		     5, 6,
		     i+1, i+2,
		     GTK_FILL | GTK_SHRINK,
		     GTK_FILL | GTK_SHRINK,
		     0, 0 );
  gtk_widget_show ( pLabel );

  /* Septi�me colonne : elle contient le symbole de la devise du compte */
  pLabel = gtk_label_new ( devise_name ((struct struct_devise *)(g_slist_find_custom ( liste_struct_devises,
									  GINT_TO_POINTER ( DEVISE ),
									  (GCompareFunc) recherche_devise_par_no )-> data )) );
  gtk_misc_set_alignment ( GTK_MISC ( pLabel ), 0, 0.5);
  gtk_table_attach ( GTK_TABLE ( pTable ),
		     pLabel,
		     6, 7,
		     i+1, i+2,
		     GTK_FILL | GTK_SHRINK,
		     GTK_FILL | GTK_SHRINK,
		     5, 0 );
  gtk_widget_show ( pLabel );

  /* Cr�ation de la (nb_comptes + 4)i�me (et derni�re) ligne du tableau : vide */
/*   pLabel = gtk_label_new ( "" ); */
/*   gtk_table_attach_defaults ( GTK_TABLE ( pTable ), */
/* 			      pLabel, */
/* 			      0, 1, */
/* 			      i+2, i+3 ); */
/*   gtk_widget_show ( pLabel ); */
}
/* ************************************************************************* */

/* ************************************************************************* */
/* Classement de deux �ch�ances d'operations par date   */
/* ************************************************************************* */
gint classement_date_echeance (struct operation_echeance * a, 
			       struct operation_echeance * b)
{
  if (a->annee > b->annee)
    return 1;
  else if (a->annee < b->annee)
    return -1;
  else 
    {
      if (a->mois > b->mois)
	return 1;
      else if (a->mois < b->mois)
	return -1;
      else 
	{
	  if (a->jour > b->jour)
	    return 1;
	  else if (a->jour < b->jour)
	    return -1;
	  else
	    return 0;
	}
    }
      
}
/* ************************************************************************* */

/* ************************************************************************* */
void update_liste_echeances_manuelles_accueil ( void )
{

  verification_echeances_a_terme ();

  if ( echeances_a_saisir )
    {
      GtkWidget *vbox;
      GtkWidget *label;
      GSList *pointeur_liste;
      GtkWidget *event_box;
      GtkWidget *hbox;
      GdkColor couleur_bleue, couleur_jaune;

      /* s'il y avait d�j� un fils dans la frame, le d�truit */
      gtk_notebook_remove_page ( GTK_NOTEBOOK(frame_etat_echeances_manuelles_accueil), 0 );

      /* on affiche la seconde frame dans laquelle on place les
	 �ch�ances � saisir */
      show_paddingbox ( frame_etat_echeances_manuelles_accueil );
 
      /* on y place la liste des �ch�ances */
      vbox = gtk_vbox_new ( FALSE, 6 );
      gtk_container_add ( GTK_CONTAINER(frame_etat_echeances_manuelles_accueil),
			  vbox );
      gtk_widget_show ( vbox );

      /* on met une ligne vide pour faire joli */
      label = gtk_label_new ("");
/*       gtk_box_pack_start ( GTK_BOX (vbox ), */
/* 			   label, */
/* 			   FALSE, */
/* 			   FALSE, */
/* 			   0 ); */
/*       gtk_widget_show ( label ); */

      /* cr�ation du style normal -> bleu */
      /* pointeur dessus -> jaune-rouge */

      style_label = gtk_style_copy ( gtk_widget_get_style (label));

      couleur_bleue.red = 500;
      couleur_bleue.green = 500;
      couleur_bleue.blue = 65535;

      couleur_jaune.red =40000;
      couleur_jaune.green =40000;
      couleur_jaune.blue = 0;

      style_label->fg[GTK_STATE_PRELIGHT] = couleur_jaune;
      style_label->fg[GTK_STATE_NORMAL] = couleur_bleue;


      pointeur_liste = g_slist_sort(echeances_a_saisir,
				    (GCompareFunc) classement_date_echeance);

      while ( pointeur_liste )
	{
	  hbox = gtk_hbox_new ( TRUE,
				5 );
	  gtk_box_pack_start ( GTK_BOX ( vbox ),
			       hbox,
			       FALSE,
			       FALSE,
			       0 );
	  gtk_widget_show (  hbox );

	  /* 	  bouton � gauche */

	  event_box = gtk_event_box_new ();
	  gtk_signal_connect ( GTK_OBJECT ( event_box ),
			       "enter-notify-event",
			       GTK_SIGNAL_FUNC ( met_en_prelight ),
			       NULL );
	  gtk_signal_connect ( GTK_OBJECT ( event_box ),
			       "leave-notify-event",
			       GTK_SIGNAL_FUNC ( met_en_normal ),
			       NULL );
	  gtk_signal_connect ( GTK_OBJECT ( event_box ),
			       "button-press-event",
			       (GtkSignalFunc) saisie_echeance_accueil,
			       ECHEANCE_COURANTE );
	  gtk_box_pack_start ( GTK_BOX ( hbox ),
			       event_box,
			       TRUE,
			       TRUE,
			       5 );
	  gtk_widget_show ( event_box  );


	  if ( ECHEANCE_COURANTE->tiers )
	    label = gtk_label_new ( g_strdup_printf ( "%02d/%02d/%d : %s",
						      ECHEANCE_COURANTE->jour,
						      ECHEANCE_COURANTE->mois,
						      ECHEANCE_COURANTE->annee,
						      ((struct struct_tiers *)(g_slist_find_custom ( liste_struct_tiers,
												     GINT_TO_POINTER ( ECHEANCE_COURANTE->tiers ),
												     (GCompareFunc) recherche_tiers_par_no )->data)) -> nom_tiers ) );
	  else
	    label = gtk_label_new ( g_strdup_printf ( "%02d/%02d/%d : %s",
						      ECHEANCE_COURANTE->jour,
						      ECHEANCE_COURANTE->mois,
						      ECHEANCE_COURANTE->annee,
						      _("No third party defined") ));
	  
	  gtk_widget_set_style ( label,
				 style_label );
	  gtk_misc_set_alignment ( GTK_MISC ( label ), 0, 0.5);
	  gtk_container_add ( GTK_CONTAINER ( event_box ),
			      label );
	  gtk_widget_show ( label  );
	  
	  /* 	  label � droite */

	  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + ECHEANCE_COURANTE->compte;

	  if ( ECHEANCE_COURANTE -> montant >= 0 )
	    label = gtk_label_new ( g_strdup_printf (_("%4.2f %s credit on %s"),
						     ECHEANCE_COURANTE->montant,
						     devise_name((struct struct_devise *)(g_slist_find_custom ( liste_struct_devises,
												     GINT_TO_POINTER ( ECHEANCE_COURANTE -> devise ),
												     (GCompareFunc) recherche_devise_par_no )->data)),
						     NOM_DU_COMPTE ));
	  else
	    label = gtk_label_new ( g_strdup_printf (_("%4.2f %s debit on %s"),
						     -ECHEANCE_COURANTE->montant,
						     devise_name((struct struct_devise *)(g_slist_find_custom ( liste_struct_devises,
												     GINT_TO_POINTER ( ECHEANCE_COURANTE -> devise ),
												     (GCompareFunc) recherche_devise_par_no )->data)),
						     NOM_DU_COMPTE ));


	  gtk_misc_set_alignment ( GTK_MISC ( label ), 0, 0.5);
	  gtk_box_pack_start ( GTK_BOX ( hbox ),
			       label,
			       FALSE,
			       TRUE,
			       0 );
	  gtk_widget_show (  label );

	  pointeur_liste = pointeur_liste -> next;
	}

      /* on met une ligne vide pour faire joli */

/*       label = gtk_label_new (""); */
/*       gtk_box_pack_start ( GTK_BOX (vbox ), */
/* 			   label, */
/* 			   FALSE, */
/* 			   FALSE, */
/* 			   0 ); */
/*       gtk_widget_show ( label ); */

    }
  else
    {
      hide_paddingbox ( frame_etat_echeances_manuelles_accueil );

    }
}
/* ************************************************************************* */

/* ************************************************************************* */
void update_liste_echeances_auto_accueil ( void )
{

  if ( echeances_saisies )
    {
      GtkWidget *vbox;
      GtkWidget *label;
      GSList *pointeur_liste;
      GtkWidget *hbox;
      struct structure_operation *operation;

      /* s'il y avait d�j� un fils dans la frame, le d�truit */
      gtk_notebook_remove_page ( GTK_NOTEBOOK(frame_etat_echeances_auto_accueil), 0 );
      /*       on affiche la seconde frame dans laquelle on place les �ch�ances � saisir */
      show_paddingbox ( frame_etat_echeances_auto_accueil );
 

      /* on y place la liste des �ch�ances */

      vbox = gtk_vbox_new ( FALSE,
			    5 );
      gtk_container_add ( GTK_CONTAINER ( frame_etat_echeances_auto_accueil ),
			  vbox);
      gtk_widget_show ( vbox);

      /* on met une ligne vide pour faire joli */

      pointeur_liste = echeances_saisies;

      while ( pointeur_liste )
	{
	  operation = pointeur_liste -> data;

	  hbox = gtk_hbox_new ( TRUE,
						     5 );
	  gtk_box_pack_start ( GTK_BOX ( vbox ),
			       hbox,
			       FALSE,
			       FALSE,
			       0 );
	  gtk_widget_show (  hbox );

	  /* 	  label � gauche */
	  
	  if ( operation ->tiers )
	    label = gtk_label_new ( g_strdup_printf ( "%02d/%02d/%d : %s",
						      operation ->jour,
						      operation ->mois,
						      operation ->annee,
						      ((struct struct_tiers *)(g_slist_find_custom ( liste_struct_tiers,
												     GINT_TO_POINTER ( operation->tiers ),
												     (GCompareFunc) recherche_tiers_par_no )->data)) -> nom_tiers ) );
	  else
	    label = gtk_label_new ( g_strdup_printf ( "%02d/%02d/%d : %s",
						      operation ->jour,
						      operation ->mois,
						      operation ->annee,
						      _("No third party defined") ));
	  

	  gtk_misc_set_alignment ( GTK_MISC ( label ), 0, 0.5);
	  gtk_box_pack_start ( GTK_BOX ( hbox ),
			       label,
			       TRUE,
			       TRUE,
			       5 );
	  gtk_widget_show ( label  );

	  /* 	  label � droite */

	  p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation->no_compte;

	  if ( operation -> montant >= 0 )
	    label = gtk_label_new ( g_strdup_printf (_("%4.2f %s credit on %s"),
						     operation->montant,
						     devise_name((struct struct_devise *)(g_slist_find_custom ( liste_struct_devises,
												     GINT_TO_POINTER ( operation -> devise ),
												     (GCompareFunc) recherche_devise_par_no )->data)),
						     NOM_DU_COMPTE ));
	  else
	    label = gtk_label_new ( g_strdup_printf (_("%4.2f %s debit on %s"),
						     -operation->montant,
						     devise_name((struct struct_devise *)(g_slist_find_custom ( liste_struct_devises,
												     GINT_TO_POINTER (  operation -> devise ),
												     (GCompareFunc) recherche_devise_par_no )->data)),
						     NOM_DU_COMPTE ));

	  gtk_misc_set_alignment ( GTK_MISC ( label ), 0, 0.5);
	  gtk_box_pack_start ( GTK_BOX ( hbox ),
			       label,
			       FALSE,
			       TRUE,
			       0 );
	  gtk_widget_show (  label );

	  pointeur_liste = pointeur_liste -> next;
	}

      /* comme des op�s ont �t� saisies, on met � jour les listes */
      mise_a_jour_tiers ();
      mise_a_jour_categ ();
      mise_a_jour_imputation ();
    }
  else
    {
      hide_paddingbox ( frame_etat_echeances_auto_accueil );
    }
}
/* ************************************************************************* */

/* ************************************************************************* */
/* Fonction mise_a_jour_soldes_minimaux */
/* v�rifie les soldes de tous les comptes, affiche un message d'alerte si n�cessaire */
/* et ajoute dans l'accueil les comptes sous les soldes minimaux */
/* ************************************************************************* */

void mise_a_jour_soldes_minimaux ( void )
{
  GtkWidget *vbox_1;
  GtkWidget *vbox_2;
  GtkWidget *label;
  gint i;
  GtkWidget *hbox;

  /* s'il y avait d�j� un fils dans la frame, le d�truit */
    
  gtk_notebook_remove_page ( GTK_NOTEBOOK(frame_etat_soldes_minimaux_autorises), 0 );
  gtk_notebook_remove_page ( GTK_NOTEBOOK(frame_etat_soldes_minimaux_voulus), 0 );

  hide_paddingbox ( frame_etat_soldes_minimaux_autorises );
  hide_paddingbox ( frame_etat_soldes_minimaux_voulus );

  vbox_1 = NULL;
  vbox_2 = NULL;


  p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

  for ( i = 0 ; i < nb_comptes ; i++ )
    {
      gint solde_courant;
      gint solde_mini;
      gint solde_mini_voulu;

      /* le plus simple est de faire les comparaisons de soldes sur des integer */

      solde_courant = rint ( SOLDE_COURANT * 100 );
      solde_mini = rint ( SOLDE_MINI * 100 );
      solde_mini_voulu = rint ( SOLDE_MINI_VOULU * 100 );


      if ( solde_courant < solde_mini && TYPE_DE_COMPTE != 2 )
	{
	  if ( !vbox_1 )
	    {
	      vbox_1 = gtk_vbox_new ( TRUE, 5 );
	      gtk_container_add ( GTK_CONTAINER ( frame_etat_soldes_minimaux_autorises ),
				  vbox_1 );
	      gtk_widget_show ( vbox_1 );
	      show_paddingbox ( frame_etat_soldes_minimaux_autorises );

	    }
	  label = gtk_label_new ( NOM_DU_COMPTE );
	  gtk_box_pack_start ( GTK_BOX ( vbox_1 ), label,
			       FALSE, FALSE, 0 );
	  gtk_misc_set_alignment ( GTK_MISC ( label ), 0, 0);
	  gtk_widget_show ( label );

	  if ( !MESSAGE_SOUS_MINI )
	    {
	      if ( solde_courant < solde_mini_voulu )
		{
		  if ( !patience_en_cours )
		    dialogue ( g_strdup_printf (_("Warning, balance of account %s is under wanted and authorised minima!"), 
			       NOM_DU_COMPTE ));
		  MESSAGE_SOUS_MINI_VOULU = 1;
		}
	      else
		if ( !patience_en_cours )
		  dialogue ( g_strdup_printf (_("Warning, the balance of the account %s  is under the authorised minimum!"),
			     NOM_DU_COMPTE ));
	      MESSAGE_SOUS_MINI = 1;
	    }

	  show_paddingbox ( frame_etat_soldes_minimaux_autorises );
	}

      if ( solde_courant < solde_mini_voulu && TYPE_DE_COMPTE != 2 &&
	   solde_courant > solde_mini && TYPE_DE_COMPTE != 2)
	{
	  if ( !vbox_2 )
	    {
	      vbox_2 = gtk_vbox_new ( TRUE, 5 );
	      gtk_container_add ( GTK_CONTAINER ( frame_etat_soldes_minimaux_voulus ),
				  vbox_2 );
	      gtk_widget_show ( vbox_2 );
	      show_paddingbox ( frame_etat_soldes_minimaux_voulus );
	    }

	  label = gtk_label_new ( NOM_DU_COMPTE );
	  gtk_misc_set_alignment ( GTK_MISC ( label ), 0, 0);
	  gtk_box_pack_start ( GTK_BOX ( vbox_2 ), label,
			       FALSE, FALSE, 0 );
	  gtk_widget_show ( label );

	  if ( !MESSAGE_SOUS_MINI_VOULU )
	    {
	      if ( solde_courant < solde_mini )
		{
		  if ( !patience_en_cours )
		    dialogue ( g_strdup_printf ( _("Warning, balance of account %s is under wanted and authorised minima!"),
						 NOM_DU_COMPTE));
		  MESSAGE_SOUS_MINI = 1;
		}
	      else
		if ( !patience_en_cours )
		  dialogue ( g_strdup_printf ( _("Warning, the balance of the account %s is under the wanted minimum!"),
					       NOM_DU_COMPTE ));
	      MESSAGE_SOUS_MINI_VOULU = 1;
	    }

	  show_paddingbox ( frame_etat_soldes_minimaux_voulus );
	}

      if ( solde_courant > solde_mini )
	MESSAGE_SOUS_MINI = 0;
      if ( solde_courant > solde_mini_voulu )
	MESSAGE_SOUS_MINI_VOULU = 0;


      p_tab_nom_de_compte_variable++;
    }

}
/* ************************************************************************* */


/* ************************************************************************* */
void mise_a_jour_fin_comptes_passifs ( void )
{
  gint i;
  GtkWidget *vbox;
  GtkWidget *label;
  GSList *liste_tmp;
  GSList *pointeur;

  gtk_notebook_remove_page ( GTK_NOTEBOOK(frame_etat_fin_compte_passif), 0 );
  hide_paddingbox ( frame_etat_fin_compte_passif );

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte;
  liste_tmp = NULL;

  for ( i = 0 ; i < nb_comptes ; i++ )
    {
      if ( TYPE_DE_COMPTE == 2
	   &&
	   SOLDE_COURANT >= 0 )
	liste_tmp = g_slist_append ( liste_tmp,
				     NOM_DU_COMPTE );

      p_tab_nom_de_compte_variable++;
    }


  if ( g_slist_length ( liste_tmp ) )
    {
      vbox = gtk_vbox_new ( FALSE,
			    0 );
/*       gtk_container_set_border_width ( GTK_CONTAINER ( vbox ), */
/* 				       5 ); */
      gtk_container_add ( GTK_CONTAINER ( frame_etat_fin_compte_passif ),
			  vbox );
      gtk_widget_show ( vbox );

      if ( g_slist_length ( liste_tmp ) > 1 )
	label = gtk_label_new (COLON(_("The following liabilities accounts are closed")));
      else
	label = gtk_label_new (COLON(_("The following liabilities account is closed")));
      gtk_misc_set_alignment ( GTK_MISC ( label ), 0, 0.5 );
      gtk_box_pack_start ( GTK_BOX ( vbox ),
			   label,
			   FALSE,
			   FALSE,
			   0 );
      gtk_widget_show ( label );


      pointeur = liste_tmp;

      while ( pointeur )
	{
	  label = gtk_label_new (pointeur -> data );
	  gtk_box_pack_start ( GTK_BOX ( vbox ),
			       label,
			       FALSE,
			       FALSE,
			       0 );
	  gtk_widget_show ( label );

	  pointeur = pointeur -> next;
	}

      show_paddingbox ( frame_etat_fin_compte_passif );
    }

}
/* ************************************************************************* */
