/* Ce fichier comprend toutes les op�rations concernant le traitement */
/* des fichiers */

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
#include "accueil.h"
#include "categories_onglet.h"
#include "comptes_traitements.h"
#include "devises.h"
#include "erreur.h"
#include "fichiers_gestion.h"
#include "fichiers_io.h"
#include "menu.h"
#include "operations_comptes.h"
#include "operations_liste.h"
#include "patienter.h"
#include "traitement_variables.h"
#include "fenetre_principale.h"


/* ************************************************************************************************************ */
void nouveau_fichier ( void )
{
  gint type_de_compte;
  gint no_compte;

/*   si la fermeture du fichier en cours se passe mal, on se barre */

  if ( !fermer_fichier () )
    return;


  init_variables ( FALSE );

  type_de_compte = demande_type_nouveau_compte ();
  if ( type_de_compte == -1 )
    return;

  no_compte = initialisation_nouveau_compte ( type_de_compte );

  /* si la cr�ation s'est mal pass�e, on se barre */

  if ( no_compte == -1 )
    return;

  p_tab_nom_de_compte_courant = p_tab_nom_de_compte;

  /* d�grise les menus n�cessaire */

  init_variables ( TRUE );

  /*   la taille des colonnes est automatique au d�part, on y met les rapports de base */

  etat.largeur_auto_colonnes = 1;
  rapport_largeur_colonnes[0] = 11;
  rapport_largeur_colonnes[1] = 13;
  rapport_largeur_colonnes[2] = 30;
  rapport_largeur_colonnes[3] = 3;
  rapport_largeur_colonnes[4] = 11;
  rapport_largeur_colonnes[5] = 11;
  rapport_largeur_colonnes[6] = 11;


  /* cr�ation des listes d'origine */

  creation_devises_de_base();
  creation_liste_categories ();
  creation_liste_categ_combofix ();

  /* on cr�e le notebook principal */

  gnome_app_set_contents ( GNOME_APP ( window ),
			   creation_fenetre_principale() );

  /* remplit les listes des op�s */

  creation_listes_operations ();

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

  changement_compte ( GINT_TO_POINTER ( compte_courant ) );


  /* affiche le nom du fichier de comptes dans le titre de la fenetre */

  affiche_titre_fenetre();

  gtk_widget_show ( notebook_general );

  /* on se met sur l'onglet de propri�t�s du compte */

  gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_general ),
			  3 );

  modification_fichier ( TRUE );
}
/* ************************************************************************************************************ */


/* ************************************************************************************************************ */
void ouvrir_fichier ( void )
{
  GtkWidget *selection_fichier;

  selection_fichier = gtk_file_selection_new ( _("Open an accounts file"));
  gtk_window_set_position ( GTK_WINDOW ( selection_fichier ),
			    GTK_WIN_POS_MOUSE);

  gtk_file_selection_set_filename ( GTK_FILE_SELECTION ( selection_fichier ),
				    dernier_chemin_de_travail );

  gtk_signal_connect_object ( GTK_OBJECT ( GTK_FILE_SELECTION ( selection_fichier ) -> cancel_button ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( gtk_widget_destroy ),
			      GTK_OBJECT ( selection_fichier ));
  gtk_signal_connect_object ( GTK_OBJECT ( GTK_FILE_SELECTION ( selection_fichier ) -> ok_button ),
			      "clicked",
			      GTK_SIGNAL_FUNC ( fichier_selectionne ),
			      GTK_OBJECT (selection_fichier) );

  gtk_widget_show ( selection_fichier );
  

}
/* ************************************************************************************************************ */

/* ************************************************************************************************************ */
void ouverture_fichier_par_menu ( GtkWidget *menu,
				  gchar *nom )
{

  /*   si la fermeture du fichier courant se passe mal, on se barre */

  if ( !fermer_fichier() )
    return;

  nom_fichier_comptes = nom;

  ouverture_confirmee ();
}
/* ************************************************************************************************************ */



/* ************************************************************************************************************ */
void fichier_selectionne ( GtkWidget *selection_fichier)
{

  /*   on cache la fenetre de s�lection et ferme le fichier en cours */
  /*     si lors de la fermeture, on a voulu enregistrer mais �a n'a pas march� => return */

  gtk_widget_hide ( selection_fichier );

  /*   si la fermeture du fichier se passe mal, on se barre */

  ancien_nom_fichier_comptes = nom_fichier_comptes;

  if ( !fermer_fichier() )
    {
      gtk_widget_hide ( selection_fichier );
      return;
    }


  /* on prend le nouveau nom du fichier */

  nom_fichier_comptes = g_strdup ( gtk_file_selection_get_filename ( GTK_FILE_SELECTION ( selection_fichier)) );

  gtk_widget_hide ( selection_fichier );

  /* on met le r�pertoire courant dans la variable correspondante */

  dernier_chemin_de_travail = g_strconcat ( GTK_LABEL ( GTK_BIN ( GTK_OPTION_MENU ( GTK_FILE_SELECTION ( selection_fichier ) -> history_pulldown )) -> child ) -> label,
					    "/",
					    NULL );

  ouverture_confirmee ();
}
/* ************************************************************************************************************ */



/* ************************************************************************************************************ */
/* Fonction ouverture_confirmee */
/* ouvre le fichier dont le nom est dans nom_fichier_comptes */
/* ne se pr�ocuppe pas d'un ancien fichier ou d'une initialisation de variables */
/* ************************************************************************************************************ */

void ouverture_confirmee ( void )
{
  mise_en_route_attente ( _("Load an accounts file") );

  /*   si nb_comptes est diff�rent de 0, c'est que l'on veut tout redessiner car on a chang� la fonte */
  /*  si charge op�rations renvoie FALSE, c'est qu'il y a eu un pb et un message est d�j� affich� */

  if ( !nb_comptes )
    {
      if ( !charge_operations () )
	{
	  /* 	  le chargement du fichier a plant�, si l'option sauvegarde � l'ouverture est activ�e, on */
	  /* propose de charger l'ancien fichier */
 
	  annulation_attente ();

	  if ( etat.sauvegarde_demarrage )
	    {
	      gchar *nom;
	      gint i, result;
	      gchar **parametres;

	      /* on cr�e le nom de la sauvegarde */

	      nom = nom_fichier_comptes;
	      i=0;

	      parametres = g_strsplit ( nom_fichier_comptes, "/", 0);
	      while ( parametres[i] )
		i++;

	      nom_fichier_comptes = g_strconcat ( getenv ("HOME"),
						  "/.",
						  parametres [i-1],
						  ".bak",
						  NULL );
	      g_strfreev ( parametres );

	      result = open ( nom_fichier_comptes, O_RDONLY);
	      if (result == -1)
		return;
	      else
		close (result);

	      mise_en_route_attente ( _("Loading backup") );

	      if ( charge_operations () )
		{
		  /* on a r�ussi a charger la sauvegarde */
		  dialogue ( _("Grisbi was unable to load file.  However, Grisbi loaded a backup file instead.\nHowever, all changes made since this backup were possibly lost."));
		  nom_fichier_comptes = nom;
		}
	      else
		{
		  /* le chargement de la sauvegarde a �chou� */

		  nom_fichier_comptes = nom;
		  annulation_attente ();
		  dialogue ( _("Grisbi was unable to load file.  Additionnaly, Grisbi was unable to load a backup file instead."));
		  return;
		}
	    }
	  else
	    {
	      init_variables ( FALSE );
	      return;
	    }
	}
      else
	{
	  /* 	si on veut faire une sauvegarde auto � chaque ouverture, c'est ici */

	  if ( etat.sauvegarde_demarrage )
	    {
	      gchar *nom;
	      gint i;
	      gchar **parametres;
	      gint save_force_enregistrement;

	      nom = nom_fichier_comptes;

	      i=0;

	      /* 	      on r�cup�re uniquement le nom du fichier, pas le chemin */

	      parametres = g_strsplit ( nom_fichier_comptes,
					"/",
					0);

	      while ( parametres[i] )
		i++;

	      nom_fichier_comptes = g_strconcat ( getenv ("HOME"),
						  "/.",
						  parametres [i-1],
						  ".bak",
						  NULL );

	      g_strfreev ( parametres );

	      /* on force l'enregistrement */

	      save_force_enregistrement = etat.force_enregistrement;
	      etat.force_enregistrement = 1;

	      enregistre_fichier ( TRUE );

	      etat.force_enregistrement = save_force_enregistrement;

	      nom_fichier_comptes = nom;
	    }
	}
    }

  update_attente ( _("Formatting transactions") );

  /* on save le nom du fichier dans les derniers ouverts */

  if (nom_fichier_comptes)
    ajoute_nouveau_fichier_liste_ouverture ( nom_fichier_comptes );

  /* affiche le nom du fichier de comptes dans le titre de la fenetre */

  affiche_titre_fenetre();

  /* d�grise les menus n�cessaire */

  init_variables ( TRUE );

  /* on cr�e le notebook principal */

  creation_fenetre_principale();

  /* remplit les listes des op�s */

  creation_listes_operations ();

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

  changement_compte ( GINT_TO_POINTER ( compte_courant ) );

  gtk_notebook_set_page ( GTK_NOTEBOOK ( notebook_general ),
			  0 );

  /* remplit les �ch�ances � l'accueil */

  /* on initialise les variables echeances_a_saisir et echeances_saisies utilis�es dans l'accueil */

  echeances_saisies = NULL;

  update_liste_echeances_manuelles_accueil ();
  update_liste_echeances_auto_accueil ();

  gnome_app_set_contents ( GNOME_APP ( window ),
			   notebook_general );

  /* on met la fonte sur les diff�rents widgets pour que kde la prenne en compte */

  /* FIXME FNONTS */
/*   if ( fonte_general ) */
/*     { */
/*** BENJ FIXME
      gtk_widget_get_style (label_temps) -> font = gdk_font_load ( fonte_general );
      gtk_widget_get_style (frame_etat_comptes_accueil) -> font = gdk_font_load ( fonte_general );
      gtk_widget_get_style (bouton_ok_equilibrage) -> font = gdk_font_load ( fonte_general );
      gtk_widget_get_style (widget_formulaire_operations[TRANSACTION_FORM_DATE]) -> font = gdk_font_load ( fonte_general );
      gtk_widget_get_style ( GTK_OPTION_MENU ( widget_formulaire_operations[TRANSACTION_FORM_DEVISE] )->menu_item ) -> font = gdk_font_load ( fonte_general );
***/
/*     } */


  annulation_attente ();
}
/* ************************************************************************************************************ */






/* ************************************************************************************************************ */
/* Fonction appel� lorsqu'on veut enregistrer le fichier ( fin de prog, fermeture fichier ... ) */
/* enregistre automatiquement ou demande */
/* si origine = -1 : provient de la fonction fermeture_grisbi */
/* si origine = -2 : provient de enregistrer sous */
/* retour : TRUE si tout va bien, cad on ne veut pas enregistrer ou c'est enregistr� */
/* ************************************************************************************************************ */

gboolean enregistrement_fichier ( gint origine )
{
  GtkWidget * dialog;
  gint etat_force, result;


  if ( !etat.modification_fichier && origine != -2 )
    return ( TRUE );

  if ( origine == -1 )
    {
      gchar * hint;

      hint = g_strdup_printf (_("Save changes to document '%s' before saving?"),
			      (nom_fichier_comptes ? nom_fichier_comptes : _("unnamed")));

      dialog = gtk_message_dialog_new ( GTK_WINDOW (window), 
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_WARNING, 
					GTK_BUTTONS_NONE,
					"" );
      gtk_dialog_add_buttons ( dialog,
			       _("Close without saving"), GTK_RESPONSE_NO,
			       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			       GTK_STOCK_SAVE, GTK_RESPONSE_OK,
			       NULL );
      gtk_label_set_markup ( GTK_LABEL ( GTK_MESSAGE_DIALOG(dialog)->label ), 
			     make_hint ( hint, _("If you close without saving, all of your changes will be discarded.")) );

      gtk_window_set_modal ( GTK_WINDOW ( dialog ), TRUE );

      switch ( gtk_dialog_run (GTK_DIALOG (dialog)) )
	{
	case GTK_RESPONSE_OK :
	  gtk_widget_destroy ( dialog );
	  break;

	case GTK_RESPONSE_NO :
	  gtk_widget_destroy ( dialog );
	  return ( TRUE );
	      
	default :
	  gtk_widget_destroy ( dialog );
	  return ( FALSE );
	}
    }

  /* si le fichier de comptes n'a pas de nom, on le demande ici */

  if ( !nom_fichier_comptes || origine == -2 )
    {
      GtkWidget *dialog;
      GtkWidget *label;
      GtkWidget *fenetre_nom;
      gint resultat;
      struct stat test_fichier;

      dialog = gnome_dialog_new ( _("Name the accounts file"),
				  GNOME_STOCK_BUTTON_OK,
				  GNOME_STOCK_BUTTON_CANCEL,
				  NULL );
      gtk_window_set_transient_for ( GTK_WINDOW ( dialog ),
				     GTK_WINDOW ( window ));
      gnome_dialog_set_default ( GNOME_DIALOG ( dialog ),
				 0 );
      gtk_signal_connect ( GTK_OBJECT ( dialog ),
			   "destroy",
			   GTK_SIGNAL_FUNC ( gtk_signal_emit_stop_by_name ),
			   "destroy" );

      label = gtk_label_new ( COLON(_("Enter a name for the account file")) );
      gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
			   label,
			   FALSE,
			   FALSE,
			   0 );
      gtk_widget_show ( label );

      fenetre_nom = gnome_file_entry_new ( "nom_fichier",
					   "nom_fichier" );
      gnome_file_entry_set_modal ( GNOME_FILE_ENTRY ( fenetre_nom ),
				   TRUE );
      gnome_file_entry_set_default_path ( GNOME_FILE_ENTRY ( fenetre_nom ),
					  dernier_chemin_de_travail );
      gtk_widget_set_usize ( gnome_file_entry_gnome_entry ( GNOME_FILE_ENTRY ( fenetre_nom )),
			     300,
			     FALSE );
      gtk_entry_set_text ( GTK_ENTRY ( gnome_file_entry_gtk_entry ( GNOME_FILE_ENTRY ( fenetre_nom ))),
			   g_strconcat ( dernier_chemin_de_travail,
					 ".gsb",
					 NULL ));
      gtk_entry_set_position ( GTK_ENTRY ( gnome_file_entry_gtk_entry ( GNOME_FILE_ENTRY ( fenetre_nom ))),
			       strlen (dernier_chemin_de_travail ));
      gtk_box_pack_start ( GTK_BOX ( GNOME_DIALOG ( dialog ) -> vbox ),
			   fenetre_nom,
			   FALSE,
			   FALSE,
			   0 );
      gtk_widget_show ( fenetre_nom );

      resultat = gnome_dialog_run ( GNOME_DIALOG ( dialog ));

      switch ( resultat )
	{
	case 0 :
	  ancien_nom_fichier_comptes = nom_fichier_comptes;
	  nom_fichier_comptes = g_strdup ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( gnome_file_entry_gtk_entry ( GNOME_FILE_ENTRY ( fenetre_nom ))))));

	  gnome_dialog_close ( GNOME_DIALOG ( dialog ));

	  /* v�rification que c'est possible */

	  if ( !strlen ( nom_fichier_comptes ))
	    {
	      nom_fichier_comptes = ancien_nom_fichier_comptes;
	      return(FALSE);
	    }


	  if ( stat ( nom_fichier_comptes,
		      &test_fichier ) != -1 )
	    {
	      if ( S_ISREG ( test_fichier.st_mode ) )
		{
		  GtkWidget *etes_vous_sur;
		  GtkWidget *label;

		  if ( ! question_yes_no_hint (_("File already exists"),
					       g_strdup_printf (_("Do you want to overwrite file \"%s\"?"), nom_fichier_comptes) ) )
		    {
		      nom_fichier_comptes = ancien_nom_fichier_comptes;
		      return(FALSE);
		    }
		}
	      else
		{
		  dialogue_error ( g_strdup_printf ( _("Invalid filename: \"%s\"!"),
						     nom_fichier_comptes ));
		  nom_fichier_comptes = ancien_nom_fichier_comptes;
		  return(FALSE);
		}
	    }
	  break;

	default :
	  gnome_dialog_close ( GNOME_DIALOG ( dialog ));
	  return ( FALSE );
	}
    }

  /*   on a maintenant un nom de fichier, on peut sauvegarder */
  if ( etat.sauvegarde_auto || origine != -1 )
    {
      /*       si on fait un enregistrer sous, on peut forcer l'enregistrement */
      etat_force = etat.force_enregistrement;

      if ( origine == -2 && !etat.force_enregistrement )
	etat.force_enregistrement = 1;
    }

  if ( nom_fichier_backup && strlen(nom_fichier_backup) )
    if ( !enregistrement_backup() )
      return ( FALSE );

  if ( patience_en_cours )
    update_attente ( _("Save file") );
  else
    mise_en_route_attente ( _("Save file") );

  result = enregistre_fichier ( 0 );
  annulation_attente();

  if ( etat.sauvegarde_auto || origine != -1 )
    {
      etat.force_enregistrement = etat_force;
    }

  modification_fichier ( FALSE );
  affiche_titre_fenetre ();
  fichier_marque_ouvert ( TRUE );
  ajoute_nouveau_fichier_liste_ouverture ( nom_fichier_comptes );

  return ( result );
}
/* ************************************************************************************************************ */


/* ************************************************************************************************************ */
gboolean enregistrer_fichier_sous ( void )
{
  return (  enregistrement_fichier ( -2 ) );
}
/* ************************************************************************************************************ */



/* ************************************************************************************************************ */
gboolean fermer_fichier ( void )
{
  int i;

  if ( !nb_comptes )
    return ( TRUE );


  /* si l'enregistrement s'est mal pass�, on se barre */

  if ( !enregistrement_fichier (-1) )
    return ( FALSE );


  /* si le fichier n'�tait pas d�j� ouvert, met � 0 l'ouverture */

  if ( !etat.fichier_deja_ouvert
       &&
       nb_comptes
       &&
       nom_fichier_comptes )
    fichier_marque_ouvert ( FALSE );

/*       stoppe le timer */

  if ( id_temps )
    {
      gtk_timeout_remove ( id_temps );
      id_temps = 0;
    }



  /* lib�re les op�rations de tous les comptes */

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

  for ( i=0 ; i< nb_comptes ; i++ )
    {
      if ( LISTE_OPERATIONS )
	g_slist_free ( LISTE_OPERATIONS );

      free ( *p_tab_nom_de_compte_variable );
      p_tab_nom_de_compte_variable++;
    };


  free ( p_tab_nom_de_compte );

  /* lib�re les �ch�ances */

  g_slist_free ( gsliste_echeances );
  g_slist_free ( echeances_a_saisir );
  g_slist_free ( echeances_saisies );
  g_slist_free ( ordre_comptes );
  g_slist_free ( liste_struct_etats );

  gtk_signal_disconnect_by_func ( GTK_OBJECT ( notebook_general ),
				  GTK_SIGNAL_FUNC ( change_page_notebook),
				  NULL );

  gtk_widget_destroy ( notebook_general );

  nom_fichier_comptes = NULL;
  affiche_titre_fenetre();


  init_variables ( FALSE );

  return ( TRUE );

}
/* ************************************************************************************************************ */




/* ************************************************************************************************************ */
/* Fonction appel�e une fois qu'on a nomm� le fichier de compte */
/* met juste le titre dans la fenetre principale */
/* ************************************************************************************************************ */
void affiche_titre_fenetre ( void )
{
  gchar **parametres;
  gchar *titre;
  gint i=0;

  if ( nom_fichier_comptes )
    {
      parametres = g_strsplit ( nom_fichier_comptes,
				"/",
				0);

      while ( parametres[i] )
	i++;

      titre = g_strconcat ( SPACIFY(COLON(_("Grisbi"))),
			    parametres [i-1],
			    NULL );

      g_strfreev ( parametres );
    }
  else
    {
      if ( nb_comptes )
	titre = _("Grisbi - No name");
      else
	titre = _("Grisbi");
    }

  gtk_window_set_title ( GTK_WINDOW ( window ),
			 titre );

}
/* ************************************************************************************************************ */



/* ************************************************************************************************************ */
/* Fonction enregistrement_backup */
/* appel�e si necessaire au d�but de l'enregistrement */
/* ************************************************************************************************************ */

gboolean enregistrement_backup ( void )
{
  gchar *buffer;
  gboolean retour;

  if ( !nom_fichier_backup || !strlen(nom_fichier_backup) )
    return FALSE;

  buffer = nom_fichier_comptes;
  nom_fichier_comptes = nom_fichier_backup;

  xmlSetCompressMode ( compression_backup );

  mise_en_route_attente ( _("Saving backup") );
  retour = enregistre_fichier( 1 );

  if ( !retour )
    annulation_attente();

  xmlSetCompressMode ( compression_fichier );
  nom_fichier_comptes = buffer;

  return ( retour );
}
/* ************************************************************************************************************ */


/* ************************************************************************************************************ */
void ajoute_nouveau_fichier_liste_ouverture ( gchar *path_fichier )
{
  gint i;
  gint position;
  gchar *dernier;

  if ( !nb_max_derniers_fichiers_ouverts ||
       ! path_fichier)
    return;

  /* ALAIN-FIXME */
  /* si on n'a pas un chemin absolu, on n'enregistre pas ce fichier
     dans la liste. Du moins jusqu'� ce que quelqu'un trouve un moyen
     pour r�cup�rer le chemein absolu */

  if ( !g_path_is_absolute ( nom_fichier_comptes ) )
    {
/*     gchar *tmp_name, *tmp_name2;
     tmp_name = realpath(nom_fichier_comptes, tmp_name2);
     
     dialogue(tmp_name);
     free(tmp_name);
     dialogue(tmp_name2);
     free(tmp_name2);*/

     return;
    }

  /* on commence par v�rifier si ce fichier n'est pas dans les nb_derniers_fichiers_ouverts noms */

  position = 0;

  for ( i=0 ; i<nb_derniers_fichiers_ouverts ; i++ )
    if ( !strcmp ( path_fichier,
		   tab_noms_derniers_fichiers_ouverts[i] ))
      {
	/* 	si ce fichier est d�j� le dernier ouvert, on laisse tomber */

	if ( !i )
	  return;

	position = i;
      }

  efface_derniers_fichiers_ouverts();

  if ( position )
    {
      /*       le fichier a �t� trouv�, on fait juste une rotation */

      for ( i=position ; i>0 ; i-- )
	tab_noms_derniers_fichiers_ouverts[i] = tab_noms_derniers_fichiers_ouverts[i-1];
      if ( path_fichier )
	tab_noms_derniers_fichiers_ouverts[0] = g_strdup ( path_fichier );
      else
	tab_noms_derniers_fichiers_ouverts[0] = g_strdup ( "<no file>" );

      affiche_derniers_fichiers_ouverts();

      return;
    }

  /*   le fichier est nouveau, on d�cale tout d'un cran et on met le nouveau � 0 */

  /*   si on est d�j� au max, c'est juste un d�calage avec perte du dernier */
  /* on garde le ptit dernier dans le cas contraire */

  if ( nb_derniers_fichiers_ouverts )
    dernier = tab_noms_derniers_fichiers_ouverts[nb_derniers_fichiers_ouverts-1];
  else
    dernier = NULL;

  for ( i= nb_derniers_fichiers_ouverts - 1 ; i>0 ; i-- )
    tab_noms_derniers_fichiers_ouverts[i] = tab_noms_derniers_fichiers_ouverts[i-1];

  if ( nb_derniers_fichiers_ouverts < nb_max_derniers_fichiers_ouverts )
    {
      tab_noms_derniers_fichiers_ouverts = realloc ( tab_noms_derniers_fichiers_ouverts,
						     ( ++nb_derniers_fichiers_ouverts ) * sizeof ( gpointer ));
      tab_noms_derniers_fichiers_ouverts[nb_derniers_fichiers_ouverts-1] = dernier;
    }

  tab_noms_derniers_fichiers_ouverts[0] = g_strdup ( path_fichier );

  
  affiche_derniers_fichiers_ouverts();
}
/* ************************************************************************************************************ */
