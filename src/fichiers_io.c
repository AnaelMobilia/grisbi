/* Fichier fichiers_io.c */
/* Contient toutes les proc�dures relatives � l'acc�s au disque */

/*     Copyright (C) 2000-2001  C�dric Auger */
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



/****************************************************************************/
/** Proc�dure qui charge les op�rations en m�moire sous forme de structures**/
/** elle rend la main en ayant initialis�e la variable p_tab_nom_de_compte,**/
/** tableau de pointeurs vers chaque compte :                              **/
/****************************************************************************/

gboolean charge_operations ( void )
{
  struct stat buffer_stat;
  xmlDocPtr doc;

  /* v�rification de la permission du fichier */

  stat ( nom_fichier_comptes,
	 &buffer_stat);
  if ( buffer_stat.st_mode != 33152 && etat.alerte_permission )
    dialogue ( "Attention, les permissions du fichier ne sont pas � \
600\n(Ce message peut �tre d�sactiv� dans les param�tres)" );
 
  /* on commence par ouvrir le fichier en xml */
  /*   si marche pas, essaye d'ouvrir la version 0.3.1 */

  doc = xmlParseFile ( nom_fichier_comptes );

  if ( doc )
    {
      /* v�rifications d'usage */

      if ( !doc->root
	   ||
	   !doc->root->name
	   ||
	   g_strcasecmp ( doc->root->name,
			  "Grisbi" ))
	{
	  dialogue ( "Fichier de compte invalide." );
	  xmlFreeDoc ( doc );
	  return ( FALSE );
	}

      /* r�cup�re la version */

        /* GDC : utilisation de la m�me fonction */
        if (( !strcmp (  xmlNodeGetContent ( doc->root->childs->childs ),
			 "0.3.2" )))
	return ( charge_operations_version_0_3_2 ( doc ));

        if (( !strcmp (  xmlNodeGetContent ( doc->root->childs->childs ),
			 "0.3.3" )))
	return ( charge_operations_version_0_3_2 ( doc ));

      dialogue ( " Version inconnue ");
      xmlFreeDoc ( doc );

      return ( FALSE );
    }

  /* ouverture du fichier */

  if (!(pointeur_fichier_comptes = fopen ( nom_fichier_comptes, "r+")) )
    {
      dialogue ( strerror ( errno ) );
      return ( FALSE );
    }

  /* passe les commentaires */

  boucle :
    fscanf ( pointeur_fichier_comptes, 
	     "%c",
	     buffer_en_tete );
  if ( buffer_en_tete[0] == '#' || buffer_en_tete[0] == '\n' )
    {
      fscanf ( pointeur_fichier_comptes, 
	       "%*[^\n]\n" );
      goto boucle;
    }
     
  /* r�cup�re la version de programme du fichier */

  fscanf ( pointeur_fichier_comptes, 
	   "%49s \n",
	   buffer_en_tete);

  if ( strcmp ( buffer_en_tete, "Comptes]" ) == 0 )
    {
      dialogue ( "Le fichier charg� est trop ancien :\n� partir de la version \
0.2.5, seule la version pr�c�dente\npeut �tre mise � jour" );
      fclose ( pointeur_fichier_comptes );
      return ( FALSE );
    }
  else
    if ( strcmp ( buffer_en_tete, "Version]" ) )
      {
	dialogue ( " Fichier de compte invalide " );
	fclose ( pointeur_fichier_comptes );
	return (FALSE);
      }


  /* r�cup�re le num�ro de version et branche o� il faut */

  fscanf ( pointeur_fichier_comptes, 
	   "%s \n",
	   version);

  if ( !strcmp ( version , "0.1.1" ) || !strcmp ( version , "0,1,1" ) ||
       !strcmp ( version , "0.1.2" ) || !strcmp ( version , "0,1,2" ) ||
       !strcmp ( version , "0.1.4" ) || !strcmp ( version , "0,1,4" ) ||
       !strcmp ( version , "0.1.5" ) || !strcmp ( version , "0,1,5" ) ||
       !strcmp ( version , "0.2.0" ) || !strcmp ( version , "0,2,0" ) ||
       !strcmp ( version , "0.2.1" ) || !strcmp ( version , "0,2,1" ) ||
       !strcmp ( version , "0.2.2" ) || !strcmp ( version , "0,2,2" ) ||
       !strcmp ( version , "0.2.4" ) || !strcmp ( version , "0,2,4" ) ||
       !strcmp ( version , "0.2.5" ) || !strcmp ( version , "0,2,5" ) )
    {
      dialogue ( "Le fichier est trop ancien :\ncette version de Grisbi ne peut\
 charger que les fichiers provenant\ndes versions 0.3.0, 0.3.1 et 0.3.2\nPour \
mettre � jour votre fichier, vous devez charger une version plus ancienne de Grisbi" );
      fclose ( pointeur_fichier_comptes );
      return ( FALSE );
    }

  if ( !strcmp ( version , "0.3.0" ) )
    {
      gboolean retour;
      retour = charge_operations_version_0_3_0 ();
      modification_fichier ( TRUE );
      fclose ( pointeur_fichier_comptes );
      return ( retour );
    }

  if ( !strcmp ( version , "0.3.1" ) )
    {
      gboolean retour;
      retour = charge_operations_version_0_3_0 ();
      fclose ( pointeur_fichier_comptes );
      return ( retour );
    }

  dialogue ( " Version inconnue ");

  fclose ( pointeur_fichier_comptes );
  return ( FALSE );

}
/***********************************************************************************************************/




/***********************************************************************************************************/
/* ajout de la 0.3.0 : bcp de changements */
/***********************************************************************************************************/

gboolean charge_operations_version_0_3_0 ( void )
{
  int i;
  gchar *buffer_montant[5];
  GSList *liste_ope_liees;
  GSList *liste_tmp;

  /* r�cup�re le nom de fichier de backup */

  do
    fscanf ( pointeur_fichier_comptes, 
	     "%49[^\n]\n",
	     buffer_en_tete);
  while ( strcmp ( buffer_en_tete, "[Backup]" ) );

  fscanf ( pointeur_fichier_comptes, 
	   "\nNom = %as\n",
	   &nom_fichier_backup );

  if ( !strcmp ( g_strstrip ( nom_fichier_backup ),
		 "(null)" ) )
    nom_fichier_backup = NULL;




  /* r�cup�re le nombre de comptes */

  do
    fscanf ( pointeur_fichier_comptes, 
	     "%49[^\n]\n",
	     buffer_en_tete);
  while ( strcmp ( buffer_en_tete, "[Comptes]" ) );

  fscanf ( pointeur_fichier_comptes, 
	   "\nnb comptes : %d\n\n",
	   &nb_comptes);

  if ( !nb_comptes )
    {
      dialogue ( " Fichier de compte invalide " );
      return ( FALSE );
    }

  /* Cr�ation du tableau de pointeur vers les noms de comptes */

  p_tab_nom_de_compte = malloc ( nb_comptes * sizeof ( gpointer ));
  p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

  for ( i=0 ; i<nb_comptes ; i++)
    {
      if ( !(*p_tab_nom_de_compte_variable = calloc ( 1,
						      sizeof (struct donnees_compte) )) )
	{
	  dialogue ( " Erreur d'allocation de m�moire " );
	  return ( FALSE );
	};
      /* rafraichit la fenetre d'attente */

      if ( patience_en_cours )
	while ( g_main_iteration ( FALSE ) );

      fscanf ( pointeur_fichier_comptes,
	       "%*d : %a[^\n]\n",
	       &NOM_DU_COMPTE);

      NOM_DU_COMPTE = g_strstrip ( NOM_DU_COMPTE );

      p_tab_nom_de_compte_variable++;
    };

  /* r�cup�ration de l'ordre d'affichage des comptes */
 
  ordre_comptes = NULL;

  fscanf ( pointeur_fichier_comptes,
	   "\nOrdre des comptes : ");

  while ( fscanf ( pointeur_fichier_comptes,
		   "%d - ",
		   &i ) )
    ordre_comptes = g_slist_append ( ordre_comptes,
				     GINT_TO_POINTER (i) );


  /*   r�cup�ration du compte courant */

  fscanf ( pointeur_fichier_comptes,
	   "%*[^\n]\nCompte courant : %d\n",
	   &compte_courant );

  p_tab_nom_de_compte_courant = p_tab_nom_de_compte + compte_courant;


  /* chaque op�ration � partir de la 0.3.2 n'a plus qu'un no d'op� unique, on va faire la transformation en */
  /* recommen�ant le d�compte � partir de 0 */

  no_derniere_operation = 0;
  liste_ope_liees = NULL;

  /* Mise en m�moire des op�rations pour chaque compte + des caract�ristiques de chacun */

  p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

  for ( i=0 ; i < nb_comptes ; i++)
    {
      gint nb_ope;
      gchar *buffer_date;
      gulong buffer_dernier_cheque;

      /* rafraichit la fenetre d'attente */

      if ( patience_en_cours )
	while ( g_main_iteration ( FALSE ) );

      /* on recherche le premier compte */
      do
	fscanf ( pointeur_fichier_comptes, 
		 "%49[^\n]\n",
		 buffer_en_tete);
      while ( strcmp ( buffer_en_tete, g_strconcat ( "[", NOM_DU_COMPTE, "]", NULL )) );

      fscanf ( pointeur_fichier_comptes,
	       "Type de compte = %d\nNombre d'operations = %d\nDevise = %d\nBanque = \
%d\nNo de compte = %a[^\n]\nDernier cheque = %lu\nSolde initial = %as\nSolde mini \
voulu = %as\nSolde mini autorise = %as\nSolde courant = %as\nNumero derniere \
operation = %*d\nDate du dernier releve = %a[^\n]\nSolde du dernier releve = %as\
\nCompte cloture = %d\nCommentaire = {%a[^}]}\nTri = %d\n\n",
	       &TYPE_DE_COMPTE,
	       &NB_OPE_COMPTE,
	       &DEVISE,
	       &BANQUE,
	       &NO_COMPTE_BANQUE,
	       &buffer_dernier_cheque,
	       &buffer_montant[0],
	       &buffer_montant[1],
	       &buffer_montant[2],
	       &buffer_montant[3],
	       &buffer_date,
	       &buffer_montant[4],
	       &COMPTE_CLOTURE,
	       &COMMENTAIRE,
	       &TRI);

      NO_COMPTE = i;

      /* met � null les variables cr��es dans la version 0.3.2 */
      /* �a sert pas � gd chose car j'ai fait un calloc, mais pour avoir les id�es plus claires */

      NO_GUICHET = NULL;
      CLE_COMPTE = NULL;
      TITULAIRE = NULL;
      ADRESSE_TITULAIRE = NULL;
      titre_fichier = NULL;
      adresse_commune = NULL;
      DERNIER_NO_RAPPROCHEMENT = 0;
      SOLDE_POINTE = 0;
      /*       par defaut, le tri des op�s lors du rapprochement se fait par date */

      TRI = 0;
      LISTE_TRI = NULL;
      NEUTRES_INCLUS = 0;

      /* mise en place des types d'op�rations en fonction du type de compte */

      creation_types_par_defaut ( i,
				  buffer_dernier_cheque);

      SOLDE_INIT = g_strtod ( buffer_montant[0],
			      NULL );

      if ( strcmp ( buffer_montant[0],
		    g_strdup_printf ( "%4.2f",
				      SOLDE_INIT )))
	{
	  buffer_montant[0] = g_strdelimit ( buffer_montant[0],
					     ",",
					     '.' );
	  SOLDE_INIT = g_strtod ( buffer_montant[0],
				  NULL );
	  if ( strcmp ( buffer_montant[0],
			g_strdup_printf ( "%4.2f",
					  SOLDE_INIT )))
	    {
	      buffer_montant[0] = g_strdelimit ( buffer_montant[0],
						 ".",
						 ',' );
	      SOLDE_INIT = g_strtod ( buffer_montant[0],
				      NULL );
	    }
	}

      free ( buffer_montant[0] );

      SOLDE_MINI_VOULU  = g_strtod ( buffer_montant[1],
				     NULL );

      if ( strcmp ( buffer_montant[1],
		    g_strdup_printf ( "%4.2f",
				      SOLDE_MINI_VOULU )))
	{
	  buffer_montant[1] = g_strdelimit ( buffer_montant[1],
					     ",",
					     '.' );
	  SOLDE_MINI_VOULU = g_strtod ( buffer_montant[1],
					NULL );
	  if ( strcmp ( buffer_montant[1],
			g_strdup_printf ( "%4.2f",
					  SOLDE_MINI_VOULU )))
	    {
	      buffer_montant[1] = g_strdelimit ( buffer_montant[1],
						 ".",
						 ',' );
	      SOLDE_MINI_VOULU  = g_strtod ( buffer_montant[1],
					     NULL );
	    }
	}

      free ( buffer_montant[1] );

      SOLDE_MINI = g_strtod ( buffer_montant[2],
			      NULL );

      if ( strcmp ( buffer_montant[2],
		    g_strdup_printf ( "%4.2f",
				      SOLDE_MINI )))
	{
	  buffer_montant[2] = g_strdelimit ( buffer_montant[2],
					     ",",
					     '.' );
	  SOLDE_MINI = g_strtod ( buffer_montant[2],
				  NULL );
	  if ( strcmp ( buffer_montant[2],
			g_strdup_printf ( "%4.2f",
					  SOLDE_MINI  )))
	    {
	      buffer_montant[2] = g_strdelimit ( buffer_montant[2],
						 ".",
						 ',' );
	      SOLDE_MINI = g_strtod ( buffer_montant[2],
				      NULL );
	    }
	}

      free ( buffer_montant[2] );

      SOLDE_COURANT = g_strtod ( buffer_montant[3],
				 NULL );

      if ( strcmp ( buffer_montant[3],
		    g_strdup_printf ( "%4.2f",
				      SOLDE_COURANT )))
	{
	  buffer_montant[3] = g_strdelimit ( buffer_montant[3],
					     ",",
					     '.' );
	  SOLDE_COURANT = g_strtod ( buffer_montant[3],
				     NULL );
	  if ( strcmp ( buffer_montant[3],
			g_strdup_printf ( "%4.2f",
					  SOLDE_COURANT )))
	    {
	      buffer_montant[3] = g_strdelimit ( buffer_montant[3],
						 ".",
						 ',' );
	      SOLDE_COURANT = g_strtod ( buffer_montant[3],
					 NULL );
	    }
	}

      free ( buffer_montant[3] );

      SOLDE_DERNIER_RELEVE = g_strtod ( buffer_montant[4],
					NULL );

      if ( strcmp ( buffer_montant[4],
		    g_strdup_printf ( "%4.2f",
				      SOLDE_DERNIER_RELEVE )))
	{
	  buffer_montant[4] = g_strdelimit ( buffer_montant[4],
					     ",",
					     '.' );
	  SOLDE_DERNIER_RELEVE = g_strtod ( buffer_montant[4],
					    NULL );
	  if ( strcmp ( buffer_montant[4],
			g_strdup_printf ( "%4.2f",
					  SOLDE_DERNIER_RELEVE )))
	    {
	      buffer_montant[4] = g_strdelimit ( buffer_montant[4],
						 ".",
						 ',' );
	      SOLDE_DERNIER_RELEVE = g_strtod ( buffer_montant[4],
						NULL );
	    }
	}

      free ( buffer_montant[4] );

      if ( !strcmp ( NO_COMPTE_BANQUE,
		     "(null)" ) )
	NO_COMPTE_BANQUE = NULL;

      if (  !strcmp ( COMMENTAIRE,
		      "(null)" ) )
	COMMENTAIRE = NULL;




      /* r�cup�ration de la date du dernier relev� */

      if ( strcmp ( buffer_date, "aucun" ))
	{
	  gchar **tab_char;

	  tab_char = g_strsplit ( buffer_date,
				  "/",
				  3 );

	  DATE_DERNIER_RELEVE = g_date_new_dmy ( atoi ( tab_char[0] ),
						 atoi ( tab_char[1] ),
						 atoi ( tab_char[2] ));
	  g_strfreev ( tab_char );
	}
      else
	DATE_DERNIER_RELEVE = NULL;

      free ( buffer_date );



      if ( SOLDE_COURANT < SOLDE_MINI_VOULU )
	MESSAGE_SOUS_MINI_VOULU = 1;
      else
	MESSAGE_SOUS_MINI_VOULU = 0;

      if ( SOLDE_COURANT < SOLDE_MINI )
	MESSAGE_SOUS_MINI = 1;
      else
	MESSAGE_SOUS_MINI = 0;

      /*       la s�lection au d�part est en bas de la liste */

      OPERATION_SELECTIONNEE = GINT_TO_POINTER ( -1 );


      /*       on va remplir la liste des op� maintenant */

      LISTE_OPERATIONS = NULL;

      for ( nb_ope = 0 ; nb_ope < NB_OPE_COMPTE ; nb_ope++)
	{ 
	  struct structure_operation *operation;
	  gulong buffer_cheque;
	  gint ancien_no_operation;

	  /* rafraichit la fenetre d'attente */

	  if ( patience_en_cours )
	    while ( g_main_iteration ( FALSE ) );

	  if ( !( operation = calloc ( 1,
				       sizeof (struct structure_operation ))) )
	    {
	      dialogue ( " Erreur d'allocation de m�moire " );
	      return ( FALSE );
	    }

	  /* r�cup�re l'op�ration */

		if ( !strcmp ( version , "0.3.1gdc" ) )
		{
		  if (	  fscanf ( pointeur_fichier_comptes,
			   "{ %u ; %d/%d/%d ; %d/%d/%d ; %as ; %d ; %d ; %as ; %as ; %u ; %u ; %u ; %d ; %a[^;] ; %lu ; %hd ; %hd ; %u ; %u ; %u }\n",
			   &operation -> no_operation,
			   &operation -> jour,
			   &operation -> mois,
			   &operation -> annee,
			   &operation -> jour_bancaire,
			   &operation -> mois_bancaire,
			   &operation -> annee_bancaire,
			   &buffer_montant[0],
			   &operation -> devise,
			   &operation -> une_devise_compte_egale_x_devise_ope,
			   &buffer_montant[1],
			   &buffer_montant[2],
			   &operation -> tiers,
			   &operation -> categorie,
			   &operation -> sous_categorie,
			   &operation -> operation_ventilee,
			   &operation -> notes,
			   &buffer_cheque,
			   &operation -> pointe,
			   &operation -> auto_man,
			   &operation -> relation_no_operation,
			   &operation -> relation_no_compte,
			   &operation -> no_operation_ventilee_associee ) < 20 )
			{
				dialogue ( g_strdup_printf ( "Probl�me dans la r�cup�ration des op�rations !\nOp�ration n�%d\nCompte n�%d\n",
					   operation -> no_operation,
					   i ));
	      return ( FALSE );
	    }
		}
		else
		{
	  if (	  fscanf ( pointeur_fichier_comptes,
			   "{ %u ; %d/%d/%d ; %as ; %d ; %d ; %as ; %as ; %u ; %u ; %u ; %d ; %a[^;] ; %lu ; %hd ; %hd ; %u ; %u ; %u }\n",
			   &ancien_no_operation,
			   &operation -> jour,
			   &operation -> mois,
			   &operation -> annee,
			   &buffer_montant[0],
			   &operation -> devise,
			   &operation -> une_devise_compte_egale_x_devise_ope,
			   &buffer_montant[1],
			   &buffer_montant[2],
			   &operation -> tiers,
			   &operation -> categorie,
			   &operation -> sous_categorie,
			   &operation -> operation_ventilee,
			   &operation -> notes,
			   &buffer_cheque,
			   &operation -> pointe,
			   &operation -> auto_man,
			   &operation -> relation_no_operation,
			   &operation -> relation_no_compte,
			   &operation -> no_operation_ventilee_associee ) < 20 )
	    {
	      dialogue ( g_strdup_printf ( "Probl�me dans la r�cup�ration des op�rations !\nOp�ration n�%d\nCompte n�%d\n",
					   operation -> no_operation,
					   i ));
	      return ( FALSE );
	    }
		}

	  /* 	  r�cup�ration du montant */

	  operation -> montant = g_strtod ( buffer_montant[0],
					    NULL );

	  if ( strcmp ( buffer_montant[0],
			g_strdup_printf ( "%4.2f",
					  operation -> montant )))
	    {
	      buffer_montant[0] = g_strdelimit ( buffer_montant[0],
						 ",",
						 '.' );
	      operation -> montant = g_strtod ( buffer_montant[0],
						NULL );
	      if ( strcmp ( buffer_montant[0],
			    g_strdup_printf ( "%4.2f",
					      operation -> montant )))
		{
		  buffer_montant[0] = g_strdelimit ( buffer_montant[0],
						     ".",
						     ',' );
		  operation -> montant = g_strtod ( buffer_montant[0],
						    NULL );
		}
	    }

	  free ( buffer_montant[0] );


	  /* modifie la variable ch�que pour la version 0.3.2 */
	  /*   selon le compte, on a maintenant : */
	  /*   compte bancaire : 1=virement, 2=d�pot, 3=cb, 4=pr�l�vement, 5=ch�que */
	  /*   compte caisse : 0 */
	  /*   compte passif : 1=virement */

	  operation -> type_ope = 0;
	  operation -> contenu_type = NULL;

	  if ( !TYPE_DE_COMPTE )
	    {
	      if ( buffer_cheque < 5 )
		{
		  /* ce n'est pas un ch�que */

		  if ( !buffer_cheque )
		    buffer_cheque = 2;
		  operation -> type_ope = buffer_cheque;
		}
	      else
		{
		  /* 		  c'est un ch�que */

		  /* 		  si on a fait un ch�que cr�dit, c'�tait pas pr�vu, on le transforme en virement */
		  /* car il n'y a pas de ch�que dans les cr�dit, et le no du ch�que sera enregistr� dans le contenu */
		  /* 		    type, vu que le virement peut en contenir */

		  if ( operation -> montant >= 0 )
		    operation -> type_ope = 1;
		  else
		    operation -> type_ope = 5;
		  operation -> contenu_type = itoa ( buffer_cheque );
		}
	    }
	  else
	    if ( TYPE_DE_COMPTE == 2 )
	      operation -> type_ope = 1;



	  /* 	  r�cup�ration du taux de change */

	  operation -> taux_change = g_strtod ( buffer_montant[1],
						NULL );

	  if ( strcmp ( buffer_montant[1],
			g_strdup_printf ( "%4.2f",
					  operation -> taux_change )))
	    {
	      buffer_montant[1] = g_strdelimit ( buffer_montant[1],
						 ",",
						 '.' );
	      operation -> taux_change = g_strtod ( buffer_montant[1],
						    NULL );
	      if ( strcmp ( buffer_montant[1],
			    g_strdup_printf ( "%4.2f",
					      operation -> taux_change )))
		{
		  buffer_montant[1] = g_strdelimit ( buffer_montant[1],
						     ".",
						     ',' );
		  operation -> taux_change = g_strtod ( buffer_montant[1],
							NULL );
		}
	    }

	  free ( buffer_montant[1] );

	  /* 	  r�cup�ration des frais de change */

	  operation -> frais_change = g_strtod ( buffer_montant[2],
						 NULL );

	  if ( strcmp ( buffer_montant[2],
			g_strdup_printf ( "%4.2f",
					  operation -> frais_change )))
	    {
	      buffer_montant[2] = g_strdelimit ( buffer_montant[2],
						 ",",
						 '.' );
	      operation -> frais_change = g_strtod ( buffer_montant[2],
						     NULL );
	      if ( strcmp ( buffer_montant[2],
			    g_strdup_printf ( "%4.2f",
					      operation -> frais_change )))
		{
		  buffer_montant[2] = g_strdelimit ( buffer_montant[2],
						     ".",
						     ',' );
		  operation -> frais_change = g_strtod ( buffer_montant[2],
							 NULL );
		}
	    }

	  free ( buffer_montant[2] );


	  operation->date = g_date_new_dmy ( operation->jour,
					     operation->mois,
					     operation->annee);

	  /* on met le compte associ� */

	  operation -> no_compte = i;

	  /* on vire l'espace apr�s la note et on met � null si n�cessaire */

	  if ( strcmp ( operation -> notes, "(null) " ) )
	    g_strstrip ( operation -> notes );
	  else
	    {
	      free ( operation -> notes );
	      operation -> notes = NULL;
	    }


	  /* on fait ici la conversion des no d'op� */

	  operation -> no_operation = ++no_derniere_operation;

	  /* 	  si c'�tait une op� li�e ( virement ou op� de ventil ), on recherche si l'autre op� a d�j� �t� transform�e, sinon */
	  /* on ajoute cette nouvelle op� � la liste des op�s d�j� transform�es */

	  /* recherche si c'est un virement */

	  if ( operation -> relation_no_operation )
	    {
	      /* c'est une liaison par virement */

	      GSList *result_ope_liee;

	      if ( ( result_ope_liee = g_slist_find_custom ( liste_ope_liees,
							     operation,
							     (GCompareFunc) recherche_ope_liee_par_no_et_par_compte )))
		{
		  /* on a trouv� l'op�ration correspondante */

		  struct struct_ope_liee *struct_ope_liee;

		  struct_ope_liee = result_ope_liee -> data;

		  operation -> relation_no_operation = struct_ope_liee -> nouveau_no_ope;

		  struct_ope_liee -> ope_liee -> relation_no_operation = operation -> no_operation;

		  liste_ope_liees = g_slist_remove ( liste_ope_liees,
						     result_ope_liee -> data );
		  free ( result_ope_liee -> data );
		}
	      else
		{
		  /* l'op� li�e n'a pas encore �t� modifi�e */

		  struct struct_ope_liee *struct_ope_liee;

		  struct_ope_liee = malloc ( sizeof ( struct struct_ope_liee ));
		  struct_ope_liee -> ancien_no_ope = ancien_no_operation;
		  struct_ope_liee -> compte_ope = operation -> no_compte;
		  struct_ope_liee -> nouveau_no_ope = operation -> no_operation;
		  struct_ope_liee -> ope_liee = operation;
		  struct_ope_liee -> ope_ventilee  = 0;

		  liste_ope_liees = g_slist_append ( liste_ope_liees,
						     struct_ope_liee );
		}
	    }

	  /* recherche si c'est une op� de ventilation */

	  if ( operation -> no_operation_ventilee_associee )
	    {
	      /* c'est une op� de ventilation */

	      GSList *result_ope_liee;

	      if ( ( result_ope_liee = g_slist_find_custom ( liste_ope_liees,
							     operation,
							     (GCompareFunc) recherche_ope_ventilee_liee_par_no_et_par_compte )))
		{
		  /* on a trouv� l'op�ration correspondante */

		  struct struct_ope_liee *struct_ope_liee;

		  struct_ope_liee = result_ope_liee -> data;

		  operation -> no_operation_ventilee_associee = struct_ope_liee -> nouveau_no_ope;

		  /* on ne retire pas l'op� ventil�e car d'autres op�s de ventil peuvent lui �tre associ�es */
		  
		  /* 		  liste_ope_liees = g_slist_remove ( liste_ope_liees, */
		  /* 						     result_ope_liee -> data ); */
		  /* 		  free ( result_ope_liee -> data ); */
		}
	      else
		{
		  /* la ventilation de cette op� n'a pas encore �t� modifi�e */


		  struct struct_ope_liee *struct_ope_liee;

		  struct_ope_liee = malloc ( sizeof ( struct struct_ope_liee ));
		  struct_ope_liee -> ancien_no_ope = ancien_no_operation;
		  struct_ope_liee -> compte_ope = operation -> no_compte;
		  struct_ope_liee -> nouveau_no_ope = operation -> no_operation;
		  struct_ope_liee -> ope_liee = operation;
		  struct_ope_liee -> ope_ventilee  = 0;

		  liste_ope_liees = g_slist_append ( liste_ope_liees,
						     struct_ope_liee );
		}
	    }

	  /* recherche si c'est une ventilation */

	  if ( operation -> operation_ventilee )
	    {
	      /* c'est une ventilation : on recherche si des op�s de ventilations ont d�j� �t� modifi�es */

	      GSList *result_ope_liee;
	      gint nouveau_no_ope;

	      /* on remet temporairement l'ancien no d'op� pour la recherche */

	      nouveau_no_ope = operation -> no_operation;
	      operation -> no_operation = ancien_no_operation;

	      if ( ( result_ope_liee = g_slist_find_custom ( liste_ope_liees,
							     operation,
							     (GCompareFunc) recherche_ope_de_ventil_liee_par_no_et_par_compte )))
		{
		  /* on a trouv� l'op�ration correspondante */
		  /* on r�it�re jusqu'� plus d'op� de ventil */

		  do
		    {
		      struct struct_ope_liee *struct_ope_liee;

		      struct_ope_liee = result_ope_liee -> data;

		      struct_ope_liee -> ope_liee -> no_operation_ventilee_associee = nouveau_no_ope;

		      liste_ope_liees = g_slist_remove ( liste_ope_liees,
							 result_ope_liee -> data );
		      free ( result_ope_liee -> data );
		    }
		  while (( result_ope_liee = g_slist_find_custom ( liste_ope_liees,
								   operation,
								   (GCompareFunc) recherche_ope_de_ventil_liee_par_no_et_par_compte )));
		}
	      else
		{
		  /* les op�s de ventil de cette ventilation n'ont pas encore �t� modifi�es */


		  struct struct_ope_liee *struct_ope_liee;

		  struct_ope_liee = malloc ( sizeof ( struct struct_ope_liee ));
		  struct_ope_liee -> ancien_no_ope = ancien_no_operation;
		  struct_ope_liee -> compte_ope = operation -> no_compte;
		  struct_ope_liee -> nouveau_no_ope = nouveau_no_ope;
		  struct_ope_liee -> ope_liee = operation;
		  struct_ope_liee -> ope_ventilee  = 1;

		  liste_ope_liees = g_slist_append ( liste_ope_liees,
						     struct_ope_liee );
		}

	      /* on remet le nouveau num�ro de l'op�ration */

	      operation -> no_operation = nouveau_no_ope;
	    }


	  LISTE_OPERATIONS = g_slist_append ( LISTE_OPERATIONS,
					      operation);
	}
      p_tab_nom_de_compte_variable++;
    }

  /*   on fait le tour des op�s li�es, si c'est des op�s ventil�es, on les efface de la liste */

  liste_tmp = liste_ope_liees;

  while ( liste_tmp )
    {
      struct struct_ope_liee *ope_liee;
      
      ope_liee = liste_tmp -> data;

      if ( ope_liee -> ope_ventilee )
	{
	  liste_ope_liees = g_slist_remove ( liste_ope_liees,
					     ope_liee );
	  free ( ope_liee );
	  liste_tmp = liste_ope_liees;
	}
      else
	liste_tmp = liste_tmp -> next;
    }

  /* si des op�rations n'ont pas trouv� de lien, on les affiche ici */

  if ( g_slist_length ( liste_ope_liees ))
    {
      gchar *texte;

      if ( g_slist_length ( liste_ope_liees ) < 10 )
	texte = "Les op�rations suivantes �taient li�es (virement ou ventilation)\
\nmais n'ont pas trouv� d'op�ration correspondante :\n\n";
      else
	dialogue ( texte = "Des op�rations �taient li�es (virement ou ventilation)\
\nmais n'ont pas trouv� d'op�ration correspondante\n(Elles vont �tre �crites \
sur la sortie standart).\nL'erreur provient s�rement d'un bug non connu d'une \
version pr�c�dente ...\nVous pouvez aller voir les op�rations directement dans \
le fichier de compte." );

      liste_tmp = liste_ope_liees;

      while ( liste_tmp )
	{
	  struct struct_ope_liee *ope_liee;

	  ope_liee = liste_tmp -> data;

	  if ( g_slist_length ( liste_ope_liees ) < 10 )
	    texte = g_strconcat ( texte,
				  "Ancien num�ro : ",
				  itoa ( ope_liee -> ancien_no_ope ),
				  " / Nouveau num�ro : ",
				  itoa ( ope_liee -> nouveau_no_ope ),
				  " / Compte n� ",
				  itoa ( ope_liee -> compte_ope ),
				  "\n",
				  NULL );
	  else
	    printf ( "%s\n", g_strconcat ( "Ancien num�ro : ",
					   itoa ( ope_liee -> ancien_no_ope ),
					   " / Nouveau num�ro : ",
					   itoa ( ope_liee -> nouveau_no_ope ),
					   " / Compte n� ",
					   itoa ( ope_liee -> compte_ope ),
					   "\n",
					   NULL ));

	  liste_tmp = liste_tmp -> next;
	}

      texte = g_strconcat ( texte,
			    "\nL'erreur provient s�rement d'un bug non connu d'une version \
pr�c�dente ...\nVous pouvez aller voir les op�rations directement dans le fichier \
de compte.",
			    NULL );

      if ( g_slist_length ( liste_ope_liees ) < 10 )
	dialogue ( texte );

      g_slist_free ( liste_ope_liees );
    }



  /* r�cup�ration des �ch�ances */


  do
    fscanf ( pointeur_fichier_comptes, 
	     "%49[^\n]\n",
	     buffer_en_tete);
  while ( strcmp ( buffer_en_tete, "[Echeances]" ) );

  /* r�cup�re le nb d'�ch�ances et le no de la derni�re */

  fscanf ( pointeur_fichier_comptes,
	   "Nombre d'echeances = %d\nNumero derniere echeance = %d\n\n",
	   &nb_echeances,
	   &no_derniere_echeance );

  /* on est maintenant sur le d�but de la premi�re �ch�ance */

  gsliste_echeances = NULL;

  for ( i = 0 ; i < nb_echeances ; i++ )
    {
      struct operation_echeance *operation_echeance;
      gulong buffer_cheque;


      /* rafraichit la fenetre d'attente */

      if ( patience_en_cours )
	while ( g_main_iteration ( FALSE ) );
	  
      if ( !(operation_echeance = calloc ( 1,
					   sizeof (struct operation_echeance ))) )
	{
	  dialogue ( " Erreur d'allocation de m�moire " );
	  return ( FALSE );
	}

      if ( fscanf ( pointeur_fichier_comptes,
		    "{ %u ; %d ; %u/%u/%u ; %as ; %d ; %u ; %u ; %u ; %d ; %a[^;] ; %lu ; %hd ; %hd ; %hd ; %u ; %u/%u/%u }\n",
		    &operation_echeance -> no_operation,
		    &operation_echeance -> compte,
		    &operation_echeance -> jour,
		    &operation_echeance -> mois,
		    &operation_echeance -> annee,
		    &buffer_montant[0],
		    &operation_echeance -> devise,
		    &operation_echeance -> tiers,
		    &operation_echeance -> categorie,
		    &operation_echeance -> sous_categorie,
		    &operation_echeance -> compte_virement,
		    &operation_echeance -> notes,
		    &buffer_cheque,
		    &operation_echeance -> auto_man,
		    &operation_echeance -> periodicite,
		    &operation_echeance -> intervalle_periodicite_personnalisee,
		    &operation_echeance -> periodicite_personnalisee,
		    &operation_echeance -> jour_limite,
		    &operation_echeance -> mois_limite,
		    &operation_echeance -> annee_limite ) < 20 )
	{
	  dialogue ( g_strdup_printf ( "Probl�me dans la r�cup�ration des �ch�ances !\n �ch�ance n�%d\n",
				       operation_echeance -> no_operation ));
	  return ( FALSE );
	}

      /* 	  ajout de la 0.3.2 : */
      /* 	  en fonction du type de compte, on r�gle le type de l'�ch�ance */
      /*   compte bancaire : 1=virement, 2=d�pot, 3=cb, 4=pr�l�vement, 5=ch�que */
      /*   compte caisse : 0 */
      /*   compte passif : 1=virement */

      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation_echeance->compte;

      if ( !TYPE_DE_COMPTE )
	{
	  /* c'est un compte bancaire */

	  if ( !buffer_cheque )
	    buffer_cheque = 2;
	  operation_echeance -> type_ope = buffer_cheque;
	}
      else
	if ( TYPE_DE_COMPTE == 1 )
	  /* c'est un compte d'esp�ces */
	  operation_echeance -> type_ope = 0;
	else
	  /* c'est un compte de passif */
	  operation_echeance -> type_ope = 1;


      /* 	  r�cup�ration du montant */

      operation_echeance -> montant = g_strtod ( buffer_montant[0],
						 NULL );

      if ( strcmp ( buffer_montant[0],
		    g_strdup_printf ( "%4.2f",
				      operation_echeance -> montant )))
	{
	  buffer_montant[0] = g_strdelimit ( buffer_montant[0],
					     ",",
					     '.' );
	  operation_echeance -> montant = g_strtod ( buffer_montant[0],
						     NULL );
	  if ( strcmp ( buffer_montant[0],
			g_strdup_printf ( "%4.2f",
					  operation_echeance -> montant )))
	    {
	      buffer_montant[0] = g_strdelimit ( buffer_montant[0],
						 ".",
						 ',' );
	      operation_echeance -> montant = g_strtod ( buffer_montant[0],
							 NULL );
	    }
	}

      free ( buffer_montant[0] );


      operation_echeance->date = g_date_new_dmy ( operation_echeance->jour,
						  operation_echeance->mois,
						  operation_echeance->annee);

      if ( !strcmp ( g_strstrip ( operation_echeance -> notes ),
		     "(null)" ) )
	operation_echeance -> notes = NULL;

      if ( ( operation_echeance -> periodicite != 4 )
	   &&
	   ( operation_echeance -> jour_limite
	     ||
	     operation_echeance -> mois_limite
	     ||
	     operation_echeance -> annee_limite ) )
	operation_echeance->date_limite = g_date_new_dmy ( operation_echeance->jour_limite,
							   operation_echeance->mois_limite,
							   operation_echeance->annee_limite);
      else
	operation_echeance -> date_limite = NULL;

      gsliste_echeances = g_slist_append ( gsliste_echeances,
					   operation_echeance);

    };
      

  /* chargement des tiers */

  do
    fscanf ( pointeur_fichier_comptes, 
	     "%49[^\n]\n",
	     buffer_en_tete);
  while ( strcmp ( buffer_en_tete, "[Tiers]" ) );

  /* r�cup�re le nb de tiers */

  fscanf ( pointeur_fichier_comptes,
	   "Nombre d'enregistrements = %d\nNumero dernier tiers = %d\n\n",
	   &nb_enregistrements_tiers,
	   &no_dernier_tiers );

  /* on est maintenant sur le d�but du premier tiers */

  for ( i = 0 ; i < nb_enregistrements_tiers ; i++ )
    {
      struct struct_tiers *tiers;

      /* rafraichit la fenetre d'attente */

      if ( patience_en_cours )
	while ( g_main_iteration ( FALSE ) );

      tiers = malloc ( sizeof ( struct struct_tiers ) );

      fscanf ( pointeur_fichier_comptes,
	       "{ %u ; %a[^;] ; %a[^;] ; %d }\n",
	       &tiers -> no_tiers,
	       &tiers -> nom_tiers,
	       &tiers -> texte,
	       &tiers -> liaison );
 
      tiers -> nom_tiers = g_strstrip ( tiers -> nom_tiers );

      if ( strcmp ( tiers -> texte, "(null) " ) )
	tiers -> texte = g_strstrip ( tiers -> texte );
      else
	{
	  free ( tiers -> texte );
	  tiers -> texte = NULL;
	}

      liste_struct_tiers = g_slist_append ( liste_struct_tiers,
					    tiers );


    }



  /* chargement des cat�gories */

  liste_struct_categories = NULL;

  do
    fscanf ( pointeur_fichier_comptes, 
	     "%49[^\n]\n",
	     buffer_en_tete );
  while ( strcmp ( buffer_en_tete, "[Categories]" ) );


  /* r�cup�re le nb de cat�g et la derni�re */

  fscanf ( pointeur_fichier_comptes,
	   "Nombre d'enregistrements = %d\nNumero derniere categorie = %d\n\n",
	   &nb_enregistrements_categories,
	   &no_derniere_categorie );

  /* on est maintenant sur le d�but de la premi�re cat�gorie */

  for ( i = 0 ; i < nb_enregistrements_categories ; i++ )
    {
      struct struct_categ *categorie;
      gint no_sous_categ_tmp;

      /* rafraichit la fenetre d'attente */

      if ( patience_en_cours )
	while ( g_main_iteration ( FALSE ) );

      categorie = malloc ( sizeof ( struct struct_categ ) );


      /* r�cup�re la cat�gorie */

      fscanf ( pointeur_fichier_comptes,
	       "{ %d ; %a[^;] ; %d ; { %d ; ",
	       &categorie -> no_categ,
	       &categorie -> nom_categ,
	       &categorie -> type_categ,
	       &categorie -> no_derniere_sous_categ );


      categorie -> nom_categ = g_strstrip ( categorie -> nom_categ );
      categorie -> liste_sous_categ = NULL;


      while ( fscanf ( pointeur_fichier_comptes,
		       "{ %d ;",
		       &no_sous_categ_tmp ) )
	{
	  struct struct_sous_categ *sous_categ;

	  sous_categ = malloc ( sizeof ( struct struct_sous_categ ) );

	  fscanf ( pointeur_fichier_comptes,
		   "%a[^}] } ",
		   &sous_categ -> nom_sous_categ );

	  sous_categ -> no_sous_categ = no_sous_categ_tmp;
	  sous_categ -> nom_sous_categ = g_strstrip ( sous_categ -> nom_sous_categ );


	  categorie -> liste_sous_categ = g_slist_append ( categorie -> liste_sous_categ,
							   sous_categ );

	}

      liste_struct_categories = g_slist_append ( liste_struct_categories,
						 categorie );

      /* termine la ligne */
 
      fscanf ( pointeur_fichier_comptes,
	       "%*[^\n]\n" );

    }

  /* cr�ation de la liste des cat�g pour le combofix */

  creation_liste_categ_combofix ();


  /* chargement des devises */

  liste_struct_devises = NULL;


  do
    fscanf ( pointeur_fichier_comptes, 
	     "%49[^\n]\n",
	     buffer_en_tete );
  while ( strcmp ( buffer_en_tete, "[Devises]" ) );


  /* r�cup�re le nb de devises et le dernier no */


  fscanf ( pointeur_fichier_comptes,
	   "Nombre de devises = %d\nNumero derniere devise = %d\n\n",
	   &nb_devises,
	   &no_derniere_devise );


  /* on est sur le d�but de la 1�re devise */


  for ( i = 0 ; i < nb_devises ; i++ )
    {
      struct struct_devise *devise;
      gint jour, mois, annee;

      /* rafraichit la fenetre d'attente */

      if ( patience_en_cours )
	while ( g_main_iteration ( FALSE ) );

      devise = malloc ( sizeof ( struct struct_devise ));

      fscanf ( pointeur_fichier_comptes,
	       "{ %d ; %a[^;] ; %a[^;] ; %d ; %d/%d/%d ; %d ; %d ; %as }\n",
	       &devise -> no_devise,
	       &devise -> nom_devise,
	       &devise -> code_devise,
	       &devise -> passage_euro,
	       &jour,
	       &mois,
	       &annee,
	       &devise -> une_devise_1_egale_x_devise_2,
	       &devise -> no_devise_en_rapport,
	       &buffer_montant[0] );

      /* 	  r�cup�ration du change */

      devise -> change = g_strtod ( buffer_montant[0],
				    NULL );

      if ( strcmp ( buffer_montant[0],
		    g_strdup_printf ( "%4.2f",
				      devise -> change )))
	{
	  buffer_montant[0] = g_strdelimit ( buffer_montant[0],
					     ",",
					     '.' );
	  devise -> change = g_strtod ( buffer_montant[0],
					NULL );
	  if ( strcmp ( buffer_montant[0],
			g_strdup_printf ( "%4.2f",
					  devise -> change )))
	    {
	      buffer_montant[0] = g_strdelimit ( buffer_montant[0],
						 ".",
						 ',' );
	      devise -> change = g_strtod ( buffer_montant[0],
					    NULL );
	    }
	}

      free ( buffer_montant[0] );


      /* on transforme la date */

      if ( jour )
	devise -> date_dernier_change = g_date_new_dmy ( jour,
							 mois,
							 annee);
      else
	devise -> date_dernier_change = NULL;

      /* on retire les espaces r�cup�r�s */

      devise -> nom_devise = g_strstrip ( devise -> nom_devise );
      devise -> code_devise = g_strstrip ( devise -> code_devise );

      liste_struct_devises = g_slist_append ( liste_struct_devises,
					      devise );
    }


  /* chargement des banques */

  liste_struct_banques = NULL;


  do
    fscanf ( pointeur_fichier_comptes, 
	     "%49[^\n]\n",
	     buffer_en_tete );
  while ( strcmp ( buffer_en_tete, "[Banques]" ) );


  /* r�cup�re le nb de banques et le dernier no */


  fscanf ( pointeur_fichier_comptes,
	   "Nombre de banques = %d\nNumero derniere banque = %d\n\n",
	   &nb_banques,
	   &no_derniere_banque );


  /* on est sur le d�but de la 1�re banque */


  for ( i = 0 ; i < nb_banques ; i++ )
    {
      struct struct_banque *banque;

      /* rafraichit la fenetre d'attente */

      if ( patience_en_cours )
	while ( g_main_iteration ( FALSE ) );

      banque = malloc ( sizeof ( struct struct_banque ));

      fscanf ( pointeur_fichier_comptes,
	       "{ %d ; %a[^;] ; %a[^;] ; %a[^;] ; %a[^;] ; %a[^;] ; {%a[^}]} }\n",
	       &banque -> no_banque,
	       &banque -> nom_banque,
	       &banque -> adr_banque,
	       &banque -> tel_banque,
	       &banque -> email_banque,
	       &banque -> web_banque,
	       &banque -> remarque_banque );

      /* on retire les espaces r�cup�r�s */

      banque -> nom_banque = g_strstrip ( banque -> nom_banque );

      banque -> adr_banque = g_strstrip ( banque -> adr_banque );
      if ( !strcmp ( banque -> adr_banque,
		     "(null)" ) )
	banque -> adr_banque = NULL;

      banque -> tel_banque = g_strstrip ( banque -> tel_banque );
      if ( !strcmp ( banque -> tel_banque,
		     "(null)"  ))
	banque -> tel_banque = NULL;

      banque -> email_banque = g_strstrip ( banque -> email_banque );
      if ( !strcmp ( banque -> email_banque,
		     "(null)" ) )
	banque -> email_banque = NULL;

      banque -> web_banque = g_strstrip ( banque -> web_banque );
      if ( !strcmp ( banque -> web_banque,
		     "(null)"  ))
	banque -> web_banque = NULL;

      banque -> remarque_banque = g_strstrip ( banque -> remarque_banque );
      if ( !strcmp ( banque -> remarque_banque,
		     "(null)" ) )
	banque -> remarque_banque = NULL;

      /* mise � z�ro de ce qui n'existait pas dans une pr�c�dente version */

      banque -> code_banque = NULL;
      banque -> nom_correspondant = NULL;
      banque -> tel_correspondant = NULL;
      banque -> email_correspondant = NULL;
      banque -> fax_correspondant = NULL;

      liste_struct_banques = g_slist_append ( liste_struct_banques,
					      banque );
    }


  /*   les exercices, c'est nouveau */

  liste_struct_exercices = NULL;
  no_derniere_exercice = 0;
  nb_exercices = 0;

  /* l'imputation budg�taire, c'est nouveau */

  liste_struct_imputation = NULL;
  liste_imputations_combofix = NULL;
  nb_enregistrements_imputations = 0;
  no_derniere_imputation = 0;

  etat.utilise_exercice = 1;
  etat.utilise_imputation_budgetaire = 1;
  etat.utilise_piece_comptable = 1;
  etat.utilise_info_banque_guichet = 1;
  etat.affiche_no_operation = 1;
  etat.affiche_boutons_valider_annuler = 1;
  liste_no_rapprochements = NULL;
  no_devise_totaux_tiers = 1;
  affichage_echeances = 3;
  affichage_echeances_perso_nb_libre = 0;
  affichage_echeances_perso_j_m_a = 0;

  /* on sauve les nouveaux param�tres */

  gnome_config_set_int ( "/Grisbi/Affichage/Affiche_no_operation",
			 etat.affiche_no_operation );
  gnome_config_set_int ( "/Grisbi/Affichage/Affiche_boutons_valider_annuler",
			 etat.affiche_boutons_valider_annuler );
  gnome_config_sync();

  modification_fichier ( TRUE );

  chemin_logo = CHEMIN_LOGO;
  nb_max_derniers_fichiers_ouverts = 3;

  return ( TRUE );
}
/***********************************************************************************************************/



/***********************************************************************************************************/
/* utilis�e pour le passage de la 0.3.1 � la 0.3.2 */
/***********************************************************************************************************/

gboolean recherche_ope_liee_par_no_et_par_compte ( struct struct_ope_liee *struct_ope_liee,
						   struct structure_operation *operation )
{

  return ( !(( struct_ope_liee -> ancien_no_ope == operation -> relation_no_operation )
	     &&
	     ( struct_ope_liee -> compte_ope == operation -> relation_no_compte )));

}
/***********************************************************************************************************/




/*****************************************************************************/
/* utilis�e pour le passage de la 0.3.1 � la 0.3.2 */
/*****************************************************************************/

gboolean recherche_ope_ventilee_liee_par_no_et_par_compte (
								struct struct_ope_liee *struct_ope_liee,
							    struct structure_operation *operation )
{

  return (!(( struct_ope_liee -> ancien_no_ope ==
				operation -> no_operation_ventilee_associee )
	    &&
	    ( struct_ope_liee -> compte_ope == operation -> no_compte )));


}
/*****************************************************************************/




/*****************************************************************************/
/* utilis�e pour le passage de la 0.3.1 � la 0.3.2 */
/*****************************************************************************/

gboolean recherche_ope_de_ventil_liee_par_no_et_par_compte (
								struct struct_ope_liee *struct_ope_liee,
							     struct structure_operation *operation )
{
  return (!(( struct_ope_liee -> ope_liee -> no_operation_ventilee_associee ==
					operation -> no_operation )
	    &&
	    ( struct_ope_liee -> compte_ope == operation -> no_compte )));

}
/*****************************************************************************/





/*****************************************************************************/
/* ajout de la 0.3.2 : passage au xml */
/*****************************************************************************/

gboolean charge_operations_version_0_3_2 ( xmlDocPtr doc )
{
  xmlNodePtr node_1;

  /* on place node_1 sur les g�n�ralit�s */

  node_1 = doc -> root -> childs;

  /*   on met en place la boucle de node_1, qui va successivement passer */
  /*     par les g�n�ralit�s, les comptes, les �ch�ances ... */

  while ( node_1 )
    {

      /* on r�cup�re ici les g�n�ralit�s */

      if ( !strcmp ( node_1 -> name,
		     "G�n�ralit�s" ) )
	{
	  xmlNodePtr node_generalites;

	  /* node_generalites va faire le tour des g�n�ralit�s */

	  node_generalites = node_1 -> childs;

	  while ( node_generalites )
	    {
	      if ( !strcmp ( node_generalites -> name,
			     "Fichier_ouvert" ))
		if ( (etat.fichier_deja_ouvert  = atoi ( xmlNodeGetContent ( node_generalites ))))
		  {
		    if ( etat.force_enregistrement )
		      dialogue ( "Attention, le fichier semble d�j� ouvert par un autre \
utilisateur ou n'a pas �t� ferm� correctement (plantage ?).\nCependant Grisbi va \
forcer l'enregistrement ; cette option est d�conseill�e\n sauf si vous �tes s�r \
que personne d'autre n'utilise le fichier pour le moment.\nLa d�sactivation de \
cette option s'effectue dans le menu \"Configuration/Fichiers\"." );
		    else
		      dialogue ( "Attention, le fichier semble d�j� ouvert par un autre \
utilisateur ou n'a pas �t� ferm� correctement (plantage ?).\nVous ne pourrez \
enregistrer vos modification qu'en activant l'option \"Forcer l'enregistrement\" \
dans le menu \"Configuration/Fichiers\"." );
		  }
	    

	      if ( !strcmp ( node_generalites -> name,
			     "Backup" ))
		nom_fichier_backup = xmlNodeGetContent ( node_generalites );

	      if ( !strcmp ( node_generalites -> name,
			     "Titre" ))
		titre_fichier = xmlNodeGetContent ( node_generalites );

	      if ( !strcmp ( node_generalites -> name,
			     "Adresse_commune" ))
		adresse_commune	= xmlNodeGetContent ( node_generalites );

	      if ( !strcmp ( node_generalites -> name,
			     "Utilise_exercices" ))
		etat.utilise_exercice = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Utilise_IB" ))
		etat.utilise_imputation_budgetaire = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Utilise_PC" ))
		etat.utilise_piece_comptable = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Utilise_info_BG" ))
		etat.utilise_info_banque_guichet = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Num�ro_devise_totaux_tiers" ))
		no_devise_totaux_tiers = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Type_affichage_des_�ch�ances" ))
		affichage_echeances = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Affichage_echeances_perso_nb_libre" ))
		affichage_echeances_perso_nb_libre = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Type_affichage_perso_echeances" ))
		affichage_echeances_perso_j_m_a = atoi ( xmlNodeGetContent ( node_generalites ));

	      if ( !strcmp ( node_generalites -> name,
			     "Num�ro_derniere_operation" ))
		no_derniere_operation= atoi ( xmlNodeGetContent ( node_generalites ));

	      node_generalites = node_generalites -> next;
	    }
	}

      /* on r�cup�re ici les comptes et op�rations */

      if ( !strcmp ( node_1 -> name,
		     "Comptes" ))
	{
	  xmlNodePtr node_comptes;

	  /* node_comptes va faire le tour de l'arborescence des comptes */

	  node_comptes = node_1 -> childs;

	  while ( node_comptes )
	    {

	      /* on va r�cup�rer ici les g�n�ralit�s des comptes */
	      
	      if ( !strcmp ( node_comptes -> name,
			     "G�n�ralit�s" ))
		{
		xmlNodePtr node_generalites;

		node_generalites = node_comptes -> childs;

		while ( node_generalites )
		  {
		    /* r�cup�re l'ordre des comptes */

		    if ( !strcmp ( node_generalites -> name,
				   "Ordre_des_comptes" ))
		      {
			gchar **pointeur_char;
			gint i;

			pointeur_char = g_strsplit ( xmlNodeGetContent ( node_generalites ),
						     "-",
						     0 );

			i = 0;
			ordre_comptes = NULL;

			while ( pointeur_char[i] )
			  {
			    ordre_comptes = g_slist_append ( ordre_comptes,
							     GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
			    i++;
			  }
			g_strfreev ( pointeur_char );

			/* calcule le nb de comptes */

			nb_comptes = g_slist_length ( ordre_comptes );

			/* Cr�ation du tableau de pointeur vers les structures de comptes */

			p_tab_nom_de_compte = malloc ( nb_comptes * sizeof ( gpointer ));
			p_tab_nom_de_compte_variable = p_tab_nom_de_compte;
		      }

		    /* r�cup�re le compte courant */

		    if ( !strcmp ( node_generalites -> name,
				   "Compte_courant" ))
		      {
			compte_courant = atoi ( xmlNodeGetContent ( node_generalites ));
			p_tab_nom_de_compte_courant = p_tab_nom_de_compte + compte_courant;
		      }

		    node_generalites = node_generalites -> next;
		  }
	      }
	    
		/* on r�cup�re les d�tails de chaque compte */

	      if ( !strcmp ( node_comptes -> name,
			     "Compte" ))
	      {
		xmlNodePtr node_nom_comptes;

		/* normalement p_tab_nom_de_compte_variable est d�j� plac� */

		/* on cr�e la structure du compte */

		*p_tab_nom_de_compte_variable = calloc ( 1,
							 sizeof (struct donnees_compte));

		/* on fait le tour dans l'arbre nom, cad : les d�tails, d�tails de type et d�tails des op�rations */

		node_nom_comptes = node_comptes -> childs;

		while ( node_nom_comptes )
		  {
		    /* on r�cup�re les d�tails du compte */

		    if ( !strcmp ( node_nom_comptes -> name,
				   "D�tails" ))
		    {
		      xmlNodePtr node_detail;

		      node_detail = node_nom_comptes -> childs;

		      while ( node_detail )
			{

			  if ( !strcmp ( node_detail -> name,
					 "Nom" ))
			       NOM_DU_COMPTE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "No_de_compte" ))
			    NO_COMPTE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Titulaire" ))
			   TITULAIRE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Type_de_compte" ))
			   TYPE_DE_COMPTE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Nb_op�rations" ))
			   NB_OPE_COMPTE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Devise" ))
			   DEVISE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Banque" ))
			   BANQUE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Guichet" ))
			   NO_GUICHET = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "No_compte_banque" ))
			   NO_COMPTE_BANQUE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Cl�_du_compte" ))
			   CLE_COMPTE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Solde_initial" ))
			   SOLDE_INIT = g_strtod ( xmlNodeGetContent ( node_detail ),
						   NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Solde_mini_voulu" ))
			   SOLDE_MINI_VOULU = g_strtod ( xmlNodeGetContent ( node_detail ),
							 NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Solde_mini_autoris�" ))
			   SOLDE_MINI = g_strtod ( xmlNodeGetContent ( node_detail ),
						   NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Solde_courant" ))
			   SOLDE_COURANT = g_strtod ( xmlNodeGetContent ( node_detail ),
						      NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Date_dernier_relev�" ))
			    {
			      gchar **pointeur_char;

			      if ( xmlNodeGetContent ( node_detail ))
				{
				  pointeur_char = g_strsplit ( xmlNodeGetContent ( node_detail ),
							       "/",
							       3 );
				  DATE_DERNIER_RELEVE = g_date_new_dmy ( atoi ( pointeur_char [0] ),
									 atoi ( pointeur_char [1] ),
									 atoi ( pointeur_char [2] ));
				  g_strfreev ( pointeur_char );
				}
			    }

			  if ( !strcmp ( node_detail -> name,
					 "Solde_dernier_relev�" ))
			   SOLDE_DERNIER_RELEVE = g_strtod ( xmlNodeGetContent ( node_detail ),
							     NULL );

			  if ( !strcmp ( node_detail -> name,
					 "Dernier_no_de_rapprochement" ))
			   DERNIER_NO_RAPPROCHEMENT = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Compte_clotur�" ))
			   COMPTE_CLOTURE = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Commentaires" ))
			   COMMENTAIRE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Adresse_du_titulaire" ))
			   ADRESSE_TITULAIRE = xmlNodeGetContent ( node_detail );

			  if ( !strcmp ( node_detail -> name,
					 "Type_d�faut_d�bit" ))
			   TYPE_DEFAUT_DEBIT = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Type_d�faut_cr�dit" ))
			   TYPE_DEFAUT_CREDIT = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Tri_par_type" ))
			   TRI = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Neutres_inclus" ))
			   NEUTRES_INCLUS = atoi ( xmlNodeGetContent ( node_detail ));

			  if ( !strcmp ( node_detail -> name,
					 "Ordre_du_tri" ))
			    {
			      LISTE_TRI = NULL;

			      if ( xmlNodeGetContent ( node_detail ))
				{
				  gchar **pointeur_char;
				  gint i;

				  pointeur_char = g_strsplit ( xmlNodeGetContent ( node_detail ),
							       "/",
							       0 );

				  i = 0;

				  while ( pointeur_char[i] )
				    {
				      LISTE_TRI = g_slist_append ( LISTE_TRI,
								   GINT_TO_POINTER ( atoi ( pointeur_char[i] )));
				      i++;
				    }
				  g_strfreev ( pointeur_char );
				}
			    }
			  node_detail = node_detail -> next;
			}
		    }
		  
		    /* on r�cup�re ici le d�tail des types */

		    if ( !strcmp ( node_nom_comptes -> name,
				   "D�tail_de_Types" ))
		    {
		      xmlNodePtr node_type;

		      node_type = node_nom_comptes -> childs;
		      TYPES_OPES = NULL;

		      while ( node_type )
			{
			  struct struct_type_ope *type;

			  type = malloc ( sizeof ( struct struct_type_ope ));

			  type -> no_type = atoi ( xmlGetProp ( node_type,
								"No" ));
			  type -> nom_type = xmlGetProp ( node_type,
							  "Nom" );
			  type -> signe_type = atoi ( xmlGetProp ( node_type,
								   "Signe" ));
			  type -> affiche_entree = atoi ( xmlGetProp ( node_type,
								       "Affiche_entree" ));
			  type -> numerotation_auto = atoi ( xmlGetProp ( node_type,
									  "Num�rotation_auto" ));
			  type -> no_en_cours = atoi ( xmlGetProp ( node_type,
								    "No_en_cours" ));

			  type -> no_compte = NO_COMPTE;

			  TYPES_OPES = g_slist_append ( TYPES_OPES,
							type );

			  node_type = node_type -> next;
			}
		    }


		    /* on r�cup�re ici le d�tail des op�s */

		    if ( !strcmp ( node_nom_comptes -> name,
				   "D�tail_des_op�rations" ))
		      {
		      xmlNodePtr node_ope;

		      node_ope = node_nom_comptes -> childs;
		      LISTE_OPERATIONS = NULL;

		      while ( node_ope )
			{
			  struct structure_operation *operation;
			  gchar **pointeur_char;
			  gchar *pointeur;

			  operation = malloc ( sizeof (struct structure_operation ));

			  operation -> no_operation = atoi ( xmlGetProp ( node_ope,
							      "No" ));

			  pointeur_char = g_strsplit ( xmlGetProp ( node_ope ,
								    "D" ),
						       "/",
						       3 );
			  operation -> jour = atoi ( pointeur_char[0] );
			  operation -> mois = atoi ( pointeur_char[1] );
			  operation -> annee = atoi ( pointeur_char[2] );
			  operation -> date = g_date_new_dmy ( operation -> jour,
							       operation -> mois,
							       operation -> annee );
			  g_strfreev ( pointeur_char );

			  /* GDC prise en compte de la lecture de la date bancaire */

			  pointeur = xmlGetProp ( node_ope,
						  "Db" );

			  if ( pointeur )
			    {
			      pointeur_char = g_strsplit ( pointeur,
							   "/",
							   3 );
			      operation -> jour_bancaire = atoi ( pointeur_char[0] );
			      operation -> mois_bancaire = atoi ( pointeur_char[1] );
			      operation -> annee_bancaire = atoi ( pointeur_char[2] );

			      if ( operation -> jour_bancaire )
				operation -> date_bancaire = g_date_new_dmy ( operation -> jour_bancaire,
									      operation -> mois_bancaire,
									      operation -> annee_bancaire );
			      else
				operation -> date_bancaire = NULL;

			      g_strfreev ( pointeur_char );
			    }
			  else
			    {
			      operation -> jour_bancaire = 0;
			      operation -> mois_bancaire = 0;
			      operation -> annee_bancaire = 0;
			      operation -> date_bancaire = NULL;
			    }
			  
			  /* GDCFin */

			  operation -> montant = g_strtod ( xmlGetProp ( node_ope,
									 "M" ),
							    NULL );

			  operation -> devise = atoi ( xmlGetProp ( node_ope,
								    "De" ));

			  operation -> une_devise_compte_egale_x_devise_ope = atoi ( xmlGetProp ( node_ope,
												  "Rdc" ));

			  operation -> taux_change = g_strtod ( xmlGetProp ( node_ope,
									     "Tc" ),
								NULL );

			  operation -> frais_change = g_strtod ( xmlGetProp ( node_ope,
									      "Fc" ),
								 NULL );

			  operation -> tiers = atoi ( xmlGetProp ( node_ope,
								   "T" ));

			  operation -> categorie = atoi ( xmlGetProp ( node_ope,
								       "C" ));

			  operation -> sous_categorie = atoi ( xmlGetProp ( node_ope,
									    "Sc" ));

			  operation -> operation_ventilee = atoi ( xmlGetProp ( node_ope,
										"Ov" ));

			  operation -> notes = xmlGetProp ( node_ope,
							    "N" );
			  if ( !strlen ( operation -> notes ))
			    operation -> notes = NULL;

			  operation -> type_ope = atoi ( xmlGetProp ( node_ope,
								      "Ty" ));

			  operation -> contenu_type = xmlGetProp ( node_ope,
								   "Ct" );
			  if ( !strlen ( operation -> contenu_type ))
			    operation -> contenu_type = NULL;

			  operation -> pointe = atoi ( xmlGetProp ( node_ope,
								    "P" ));

			  operation -> auto_man = atoi ( xmlGetProp ( node_ope,
								      "A" ));

			  operation -> no_rapprochement = atoi ( xmlGetProp ( node_ope,
									      "R" ));

			  operation -> no_exercice = atoi ( xmlGetProp ( node_ope,
									 "E" ));

			  operation -> imputation = atoi ( xmlGetProp ( node_ope,
									"I" ));

			  operation -> sous_imputation = atoi ( xmlGetProp ( node_ope,
									     "Si" ));

			  operation -> no_piece_comptable = xmlGetProp ( node_ope,
									 "Pc" );
			  if ( !strlen ( operation -> no_piece_comptable ))
			    operation -> no_piece_comptable = NULL;

			  operation -> info_banque_guichet = xmlGetProp ( node_ope,
									  "Ibg" );
			  if ( !strlen ( operation -> info_banque_guichet ))
			    operation -> info_banque_guichet = NULL;

			  operation -> relation_no_operation = atoi ( xmlGetProp ( node_ope,
										   "Ro" ));

			  operation -> relation_no_compte = atoi ( xmlGetProp ( node_ope,
										"Rc" ));

			  operation -> no_operation_ventilee_associee = atoi ( xmlGetProp ( node_ope,
											    "Va" ));


			  /* on met le compte associ� */
			  
			  operation -> no_compte = NO_COMPTE;

			  LISTE_OPERATIONS = g_slist_append ( LISTE_OPERATIONS,
							      operation);

			  node_ope = node_ope -> next;
			}
		    }
		    node_nom_comptes = node_nom_comptes -> next;
		  }

		/* 		    le compte est fini, on peut mettre � jour qques variables */


		if ( SOLDE_COURANT < SOLDE_MINI_VOULU )
		  MESSAGE_SOUS_MINI_VOULU = 1;
		else
		  MESSAGE_SOUS_MINI_VOULU = 0;

		if ( SOLDE_COURANT < SOLDE_MINI )
		  MESSAGE_SOUS_MINI = 1;
		else
		  MESSAGE_SOUS_MINI = 0;

		/*       la s�lection au d�part est en bas de la liste */

		OPERATION_SELECTIONNEE = GINT_TO_POINTER ( -1 );


		/* on incr�mente p_tab_nom_de_compte_variable pour le compte suivant */

		p_tab_nom_de_compte_variable++;
	      }
	      
	      node_comptes = node_comptes -> next;
	    }
	}
	
      /* on r�cup�re ici les �ch�ances */

      if ( !strcmp ( node_1 -> name,
		     "�ch�ances" ))
	{
	  xmlNodePtr node_echeances;

	  /* node_echeances va faire le tour de l'arborescence des �ch�ances */

	  node_echeances = node_1 -> childs;

	  while ( node_echeances )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des �ch�ances */

	      if ( !strcmp ( node_echeances -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_echeances -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_�ch�ances" ))
			nb_echeances = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derni�re_�ch�ance" ))
			no_derniere_echeance = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va r�cup�rer ici les �ch�ances */
	      
	      if ( !strcmp ( node_echeances -> name,
			     "D�tail_des_�ch�ances" ))
		{
		  xmlNodePtr node_detail;

		  gsliste_echeances = NULL;

		  node_detail = node_echeances -> childs;

		  while ( node_detail )
		    {
		      struct operation_echeance *operation_echeance;
		      gchar **pointeur_char;

		      operation_echeance = malloc ( sizeof (struct operation_echeance ));

		      operation_echeance -> no_operation = atoi ( xmlGetProp ( node_detail,
								      "No" ));

		      pointeur_char = g_strsplit ( xmlGetProp ( node_detail ,
								"Date" ),
						   "/",
						   3 );
		      operation_echeance -> jour = atoi ( pointeur_char[0] );
		      operation_echeance -> mois = atoi ( pointeur_char[1] );
		      operation_echeance -> annee = atoi ( pointeur_char[2] );
		      operation_echeance -> date = g_date_new_dmy ( operation_echeance -> jour,
								    operation_echeance -> mois,
								    operation_echeance -> annee );
		      g_strfreev ( pointeur_char );

		      operation_echeance -> compte = atoi ( xmlGetProp ( node_detail,
								      "Compte" ));

		      operation_echeance -> montant = g_strtod ( xmlGetProp ( node_detail,
									      "Montant" ),
								 NULL );

		      operation_echeance -> devise = atoi ( xmlGetProp ( node_detail,
									 "Devise" ));

		      operation_echeance -> tiers = atoi ( xmlGetProp ( node_detail,
									"Tiers" ));

		      operation_echeance -> categorie = atoi ( xmlGetProp ( node_detail,
								      "Cat�gorie" ));

		      operation_echeance -> sous_categorie = atoi ( xmlGetProp ( node_detail,
								      "Sous-cat�gorie" ));

		      operation_echeance -> compte_virement = atoi ( xmlGetProp ( node_detail,
								      "Virement_compte" ));

		      operation_echeance -> type_ope = atoi ( xmlGetProp ( node_detail,
								      "Type" ));

		      operation_echeance -> contenu_type = xmlGetProp ( node_detail,
							    "Contenu_du_type" );
		      if ( !strlen ( operation_echeance -> contenu_type ))
			operation_echeance -> contenu_type = NULL;

		      operation_echeance -> no_exercice = atoi ( xmlGetProp ( node_detail,
								      "Exercice" ));

		      operation_echeance -> imputation = atoi ( xmlGetProp ( node_detail,
								      "Imputation" ));

		      operation_echeance -> sous_imputation = atoi ( xmlGetProp ( node_detail,
								      "Sous-imputation" ));

		      operation_echeance -> no_piece_comptable = xmlGetProp ( node_detail,
								      "Pi�ce_comptable" );
		      if ( !strlen ( operation_echeance -> no_piece_comptable ))
			operation_echeance -> no_piece_comptable = NULL;

		      operation_echeance -> info_banque_guichet = xmlGetProp ( node_detail,
								      "Info_baque_guichet" );
		      if ( !strlen ( operation_echeance -> info_banque_guichet ))
			operation_echeance -> info_banque_guichet = NULL;

		      operation_echeance -> notes = xmlGetProp ( node_detail,
								      "Notes" );
		      if ( !strlen ( operation_echeance -> notes ))
			operation_echeance -> notes = NULL;

		      operation_echeance -> auto_man = atoi ( xmlGetProp ( node_detail,
								      "Automatique" ));

		      operation_echeance -> periodicite = atoi ( xmlGetProp ( node_detail,
								      "P�riodicit�" ));

		      operation_echeance -> intervalle_periodicite_personnalisee = atoi ( xmlGetProp ( node_detail,
								      "Intervalle_p�riodicit�" ));

		      operation_echeance -> periodicite_personnalisee = atoi ( xmlGetProp ( node_detail,
								      "P�riodicit�_personnalis�e" ));

		      if ( strlen ( xmlGetProp ( node_detail ,
						 "Date_limite" )))
			{
			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail ,
								    "Date_limite" ),
						       "/",
						       3 );

			  operation_echeance -> jour_limite = atoi ( pointeur_char[0] );
			  operation_echeance -> mois_limite = atoi ( pointeur_char[1] );
			  operation_echeance -> annee_limite = atoi ( pointeur_char[2] );
			  operation_echeance -> date_limite = g_date_new_dmy ( operation_echeance -> jour_limite,
									       operation_echeance -> mois_limite,
									       operation_echeance -> annee_limite );
			  g_strfreev ( pointeur_char );
			}
		      else
			{
			  operation_echeance -> jour_limite = 0;
			  operation_echeance -> mois_limite = 0;
			  operation_echeance -> annee_limite = 0;
			  operation_echeance -> date_limite = NULL;
			}


		      gsliste_echeances = g_slist_append ( gsliste_echeances,
							   operation_echeance);

		      node_detail = node_detail -> next;
		    }
		}
	      node_echeances = node_echeances -> next;
	    }
	}
	
      /* on r�cup�re ici les tiers */

      if ( !strcmp ( node_1 -> name,
		     "Tiers" ))
	{
	  xmlNodePtr node_tiers;

	  /* node_tiers va faire le tour de l'arborescence des tiers */

	  node_tiers = node_1 -> childs;

	  while ( node_tiers )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des tiers */
	      
	      if ( !strcmp ( node_tiers -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_tiers -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_tiers" ))
			nb_enregistrements_tiers = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_dernier_tiers" ))
			no_dernier_tiers = atoi ( xmlNodeGetContent ( node_generalites ));


			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va r�cup�rer ici les tiers */
	      
	      if ( !strcmp ( node_tiers -> name,
			     "D�tail_des_tiers" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_tiers = NULL;
		  node_detail = node_tiers -> childs;

		  while ( node_detail )
		    {
		      struct struct_tiers *tiers;

		      tiers = malloc ( sizeof ( struct struct_tiers ) );

		      tiers -> no_tiers = atoi ( xmlGetProp ( node_detail,
							      "No" ));
		      tiers -> nom_tiers = xmlGetProp ( node_detail,
							"Nom" );
		      tiers -> texte = xmlGetProp ( node_detail,
						    "Informations" );
		      if ( !strlen ( tiers -> texte ))
			tiers -> texte = NULL;

		      tiers -> liaison = atoi ( xmlGetProp ( node_detail,
							     "Liaison" ));
		      
		      liste_struct_tiers = g_slist_append ( liste_struct_tiers,
							    tiers );


		      node_detail = node_detail -> next;
		    }
		}
	      node_tiers = node_tiers -> next;
	    }
	}
	
      /* on r�cup�re ici les cat�gories */

      if ( !strcmp ( node_1 -> name,
		     "Cat�gories" ))
	{
	  xmlNodePtr node_categories;

	  /* node_categories va faire le tour de l'arborescence des cat�gories */

	  node_categories = node_1 -> childs;

	  while ( node_categories )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des cat�gories */
	      
	      if ( !strcmp ( node_categories -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_categories -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_cat�gories" ))
			nb_enregistrements_categories = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derni�re_cat�gorie" ))
			no_derniere_categorie = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va r�cup�rer ici les cat�gories */
	      
	      if ( !strcmp ( node_categories -> name,
			     "D�tail_des_cat�gories" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_categories = NULL;
		  node_detail = node_categories -> childs;

		  while ( node_detail )
		    {
		      struct struct_categ *categorie;
		      xmlNodePtr node_sous_categ;
		      
		      categorie = malloc ( sizeof ( struct struct_categ ) );

		      categorie -> no_categ = atoi ( xmlGetProp ( node_detail,
								  "No" ));
		      categorie -> nom_categ = xmlGetProp ( node_detail,
							    "Nom" );
		      categorie -> type_categ = atoi ( xmlGetProp ( node_detail,
								    "Type" ));
		      categorie -> no_derniere_sous_categ = atoi ( xmlGetProp ( node_detail,
										"No_derni�re_sous_cag�gorie" ));

		      /*  pour chaque cat�gorie, on r�cup�re les sous-cat�gories */

		      categorie -> liste_sous_categ = NULL;
		      node_sous_categ = node_detail -> childs;

		      while ( node_sous_categ )
			{
			  struct struct_sous_categ *sous_categ;

			  sous_categ = malloc ( sizeof ( struct struct_sous_categ ) );

			  sous_categ -> no_sous_categ = atoi ( xmlGetProp ( node_sous_categ,
									    "No" ));
			  sous_categ -> nom_sous_categ = xmlGetProp ( node_sous_categ,
									     "Nom" );

			  categorie -> liste_sous_categ = g_slist_append ( categorie -> liste_sous_categ,
									   sous_categ );
			  node_sous_categ = node_sous_categ -> next;
			}

		      liste_struct_categories = g_slist_append ( liste_struct_categories,
								 categorie );

		      node_detail = node_detail -> next;
		    }
		}
	      node_categories = node_categories -> next;
	    }
	  /* cr�ation de la liste des cat�g pour le combofix */

	  creation_liste_categ_combofix ();
	}
	
      /* on r�cup�re ici les imputations */

      if ( !strcmp ( node_1 -> name,
		     "Imputations" ))
	{
	  xmlNodePtr node_imputations;

	  /* node_imputations va faire le tour de l'arborescence des imputations */

	  node_imputations = node_1 -> childs;

	  while ( node_imputations )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des imputations */
	      
	      if ( !strcmp ( node_imputations -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_imputations -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_imputations" ))
			nb_enregistrements_imputations = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derni�re_imputation" ))
			no_derniere_imputation = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va r�cup�rer ici les imputations */
	      
	      if ( !strcmp ( node_imputations -> name,
			     "D�tail_des_imputations" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_imputation = NULL;
		  node_detail = node_imputations -> childs;

		  while ( node_detail )
		    {
		      struct struct_imputation *imputation;
		      xmlNodePtr node_sous_imputation;
		      
		      imputation = malloc ( sizeof ( struct struct_imputation ) );

		      imputation -> no_imputation = atoi ( xmlGetProp ( node_detail,
								  "No" ));
		      imputation -> nom_imputation = xmlGetProp ( node_detail,
							    "Nom" );
		      imputation -> type_imputation = atoi ( xmlGetProp ( node_detail,
								    "Type" ));
		      imputation -> no_derniere_sous_imputation = atoi ( xmlGetProp ( node_detail,
										"No_derni�re_sous_imputation" ));

		      /*  pour chaque cat�gorie, on r�cup�re les sous-cat�gories */

		      imputation -> liste_sous_imputation = NULL;
		      node_sous_imputation = node_detail -> childs;

		      while ( node_sous_imputation )
			{
			  struct struct_sous_imputation *sous_imputation;

			  sous_imputation = malloc ( sizeof ( struct struct_sous_imputation ) );

			  sous_imputation -> no_sous_imputation = atoi ( xmlGetProp ( node_sous_imputation,
									    "No" ));
			  sous_imputation -> nom_sous_imputation = xmlGetProp ( node_sous_imputation,
									     "Nom" );

			  imputation -> liste_sous_imputation = g_slist_append ( imputation -> liste_sous_imputation,
									   sous_imputation );
			  node_sous_imputation = node_sous_imputation -> next;
			}

		      liste_struct_imputation = g_slist_append ( liste_struct_imputation,
								 imputation );

		      node_detail = node_detail -> next;
		    }
		}
	      node_imputations = node_imputations -> next;
	    }
	  /* cr�ation de la liste des imputations pour le combofix */

	  creation_liste_imputation_combofix ();
	}

	
      /* on r�cup�re ici les devises */

      if ( !strcmp ( node_1 -> name,
		     "Devises" ))
	{
	  xmlNodePtr node_devises;

	  /* node_devises va faire le tour de l'arborescence des devises */

	  node_devises = node_1 -> childs;

	  while ( node_devises )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des devises */
	      
	      if ( !strcmp ( node_devises -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_devises -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_devises" ))
			nb_devises = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derni�re_devise" ))
			no_derniere_devise = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va r�cup�rer ici les devises */

	      if ( !strcmp ( node_devises -> name,
			     "D�tail_des_devises" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_devises = NULL;
		  node_detail = node_devises -> childs;

		  while ( node_detail )
		    {
		      struct struct_devise *devise;
		      
		      devise = malloc ( sizeof ( struct struct_devise ));


		      devise -> no_devise = atoi ( xmlGetProp ( node_detail,
						 "No" ));
		      devise -> nom_devise = xmlGetProp ( node_detail,
						 "Nom" );
		      devise -> code_devise = xmlGetProp ( node_detail,
						 "Code" );
		      if ( !strlen ( devise -> code_devise ))
			devise -> code_devise = NULL;

		      devise -> passage_euro = atoi ( xmlGetProp ( node_detail,
						 "Passage_euro" ));

		      if ( strlen ( xmlGetProp ( node_detail,
						 "Date_dernier_change" )))
			{
			  gchar **pointeur_char;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "Date_dernier_change" ),
						       "/",
						       3 );

			  devise -> date_dernier_change = g_date_new_dmy ( atoi ( pointeur_char[0] ),
									   atoi ( pointeur_char[1] ),
									   atoi ( pointeur_char[2] ));
			  g_strfreev ( pointeur_char );
			}
		      else
			devise -> date_dernier_change = NULL;

		      devise -> une_devise_1_egale_x_devise_2 = atoi ( xmlGetProp ( node_detail,
						 "Rapport_entre_devises" ));
		      devise -> no_devise_en_rapport = atoi ( xmlGetProp ( node_detail,
						 "Devise_en_rapport" ));
		      devise -> change = g_strtod ( xmlGetProp ( node_detail,
								 "Change" ),
						    NULL );

		      liste_struct_devises = g_slist_append ( liste_struct_devises,
							      devise );

		      node_detail = node_detail -> next;
		    }
		}
	      node_devises = node_devises -> next;
	    }
	}
	
      /* on r�cup�re ici les banques */

      if ( !strcmp ( node_1 -> name,
		     "Banques" ))
	{
	  xmlNodePtr node_banques;

	  /* node_banques va faire le tour de l'arborescence des banques */

	  node_banques = node_1 -> childs;

	  while ( node_banques )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des banques */
	      
	      if ( !strcmp ( node_banques -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_banques -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_banques" ))
			nb_banques = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_derni�re_banque" ))
			no_derniere_banque = atoi ( xmlNodeGetContent ( node_generalites ));


			node_generalites = node_generalites -> next;
		    }
		}


	      /* on va r�cup�rer ici les banques */
	      
	      if ( !strcmp ( node_banques -> name,
			     "D�tail_des_banques" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_banques = NULL;
		  node_detail = node_banques -> childs;

		  while ( node_detail )
		    {
		      struct struct_banque *banque;
		      
		      banque = malloc ( sizeof ( struct struct_banque ));

		      banque -> no_banque = atoi ( xmlGetProp ( node_detail,
						 "No" ));
		      banque -> nom_banque = xmlGetProp ( node_detail,
						 "Nom" );
		      banque -> code_banque = xmlGetProp ( node_detail,
						 "Code" );
		      if ( !strlen ( banque -> code_banque ))
			banque -> code_banque = NULL;
		      banque -> adr_banque = xmlGetProp ( node_detail,
						 "Adresse" );
		      if ( !strlen ( banque -> adr_banque ))
			banque -> adr_banque = NULL;
		      banque -> tel_banque = xmlGetProp ( node_detail,
						 "T�l" );
		      if ( !strlen ( banque -> tel_banque ))
			banque -> tel_banque = NULL;
		      banque -> email_banque = xmlGetProp ( node_detail,
						 "Mail" );
		      if ( !strlen ( banque -> email_banque ))
			banque -> email_banque = NULL;
		      banque -> web_banque = xmlGetProp ( node_detail,
						 "Web" );
		      if ( !strlen ( banque -> web_banque ))
			banque -> web_banque = NULL;
		      banque -> nom_correspondant = xmlGetProp ( node_detail,
						 "Nom_correspondant" );
		      if ( !strlen ( banque -> nom_correspondant ))
			banque -> nom_correspondant = NULL;
		      banque -> fax_correspondant = xmlGetProp ( node_detail,
						 "Fax_correspondant" );
		      if ( !strlen ( banque -> fax_correspondant ))
			banque -> fax_correspondant = NULL;
		      banque -> tel_correspondant = xmlGetProp ( node_detail,
						 "T�l_correspondant" );
		      if ( !strlen ( banque -> tel_correspondant ))
			banque -> tel_correspondant = NULL;
		      banque -> email_correspondant = xmlGetProp ( node_detail,
						 "Mail_correspondant" );
		      if ( !strlen ( banque -> email_correspondant ))
			banque -> email_correspondant = NULL;
		      banque -> remarque_banque = xmlGetProp ( node_detail,
						 "Remarques" );
		      if ( !strlen ( banque -> remarque_banque ))
			banque -> remarque_banque = NULL;

		      liste_struct_banques = g_slist_append ( liste_struct_banques,
							      banque );

		      node_detail = node_detail -> next;
		    }
		}
	      node_banques = node_banques -> next;
	    }
	}
	
      /* on r�cup�re ici les exercices */

      if ( !strcmp ( node_1 -> name,
		     "Exercices" ))
	{
	  xmlNodePtr node_exercices;

	  /* node_exercices va faire le tour de l'arborescence des exercices */

	  node_exercices = node_1 -> childs;

	  while ( node_exercices )
	    {
	      /* on va r�cup�rer ici les g�n�ralit�s des exercices */
	      
	      if ( !strcmp ( node_exercices -> name,
			     "G�n�ralit�s" ))
		{
		  xmlNodePtr node_generalites;

		  node_generalites = node_exercices -> childs;

		  while ( node_generalites )
		    {
		      if ( !strcmp ( node_generalites -> name,
				     "Nb_exercices" ))
			nb_exercices = atoi ( xmlNodeGetContent ( node_generalites ));

		      if ( !strcmp ( node_generalites -> name,
				     "No_dernier_exercice" ))
			no_derniere_exercice = atoi ( xmlNodeGetContent ( node_generalites ));

			node_generalites = node_generalites -> next;
		    }
		}

	 
	      /* on va r�cup�rer ici les exercices */
	      
	      if ( !strcmp ( node_exercices -> name,
			     "D�tail_des_exercices" ))
		{
		  xmlNodePtr node_detail;

		  liste_struct_exercices = NULL;
		  node_detail = node_exercices -> childs;

		  while ( node_detail )
		    {
		      struct struct_exercice *exercice;
		      
		      exercice = malloc ( sizeof ( struct struct_exercice ));

		      exercice -> no_exercice = atoi ( xmlGetProp ( node_detail,
						 "No" ));
		      exercice -> nom_exercice = xmlGetProp ( node_detail,
						 "Nom" );

		      if ( strlen ( xmlGetProp ( node_detail,
						 "Date_d�but" )))
			{
			  gchar **pointeur_char;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "Date_d�but" ),
						       "/",
						       3 );

			  exercice -> date_debut = g_date_new_dmy ( atoi ( pointeur_char[0] ),
								    atoi ( pointeur_char[1] ),
								    atoi ( pointeur_char[2] ));
			  g_strfreev ( pointeur_char );
			}
		      else
			exercice -> date_debut = NULL;

		      if ( strlen ( xmlGetProp ( node_detail,
						 "Date_fin" )))
			{
			  gchar **pointeur_char;

			  pointeur_char = g_strsplit ( xmlGetProp ( node_detail,
								    "Date_fin" ),
						       "/",
						       3 );

			  exercice -> date_fin = g_date_new_dmy ( atoi ( pointeur_char[0] ),
								  atoi ( pointeur_char[1] ),
								  atoi ( pointeur_char[2] ));
			  g_strfreev ( pointeur_char );
			}
		      else
			exercice -> date_fin = NULL;

		      exercice -> affiche_dans_formulaire = atoi ( xmlGetProp ( node_detail,
										"Affich�" ));

		      liste_struct_exercices = g_slist_append ( liste_struct_exercices,
								exercice );

		      node_detail = node_detail -> next;
		    }
		}
	      node_exercices = node_exercices -> next;
	    }
	}
	
      /* on r�cup�re ici les rapprochements */

      if ( !strcmp ( node_1 -> name,
		     "Rapprochements" ))
	{
	  xmlNodePtr node_rapprochements;

	  /* node_rapprochements va faire le tour de l'arborescence des rapprochements */

	  node_rapprochements = node_1 -> childs;

	  while ( node_rapprochements )
	    {
	      /* il n'y a pas de g�n�ralit�s ... */

	      /* on va r�cup�rer ici les rapprochements */
	      
	      if ( !strcmp ( node_rapprochements -> name,
			     "D�tail_des_rapprochements" ))
		{
		  xmlNodePtr node_detail;

		  liste_no_rapprochements = NULL;
		  node_detail = node_rapprochements -> childs;

		  while ( node_detail )
		    {
		      struct struct_no_rapprochement *rapprochement;

		      rapprochement = malloc ( sizeof ( struct struct_no_rapprochement ));

		      rapprochement -> no_rapprochement = atoi ( xmlGetProp ( node_detail,
						 "No" ));
		      rapprochement -> nom_rapprochement = xmlGetProp ( node_detail,
									"Nom" );
		      
		      rapprochement -> nom_rapprochement = g_strstrip ( rapprochement -> nom_rapprochement);

		      liste_no_rapprochements = g_slist_append ( liste_no_rapprochements,
								 rapprochement );

		      node_detail = node_detail -> next;
		    }
		}
	      node_rapprochements = node_rapprochements -> next;
	    }
	}
      node_1 = node_1 -> next;
    }


  /* on lib�re la m�moire */

  xmlFreeDoc ( doc );

  /* on marque le fichier comme ouvert */

  fichier_marque_ouvert ( TRUE );

  modification_fichier ( FALSE );

  return ( TRUE );
}
/***********************************************************************************************************/




/***********************************************************************************************************/
/* Proc�dure qui sauvegarde le fichier */
/* retourne TRUE : no problem */
/* retourne FALSE : pas de nom => gtkfileselection a pris la main ou erreur */
/***********************************************************************************************************/

gboolean enregistre_fichier ( void )
{
  xmlDocPtr doc;
  xmlNodePtr node;
  xmlNodePtr node_1;
  gint resultat;
  gchar *pointeur_char;
  GSList *pointeur_liste;
  gint i;

  /*   si le fichier est d�j� ouvert par un autre, ne peut enregistrer */

  if ( etat.fichier_deja_ouvert && !etat.force_enregistrement )
    {
      dialogue ( " Fichier ouvert par un autre utilisateur, enregistrement impossible\nVous pouvez forcer l'enregistrement � partir du menu \"Configuration/Fichiers\"." );
      return ( FALSE );
    }


  /* on met � jour les soldes des comptes */

  for ( i=0 ; i<nb_comptes ; i++ )
    mise_a_jour_solde ( i );


  /* cr�ation de l'arbre xml en m�moire */

  doc = xmlNewDoc("1.0");

  /* la racine est grisbi */

  doc->root = xmlNewDocNode ( doc,
			      NULL,
			      "Grisbi",
			      NULL );

  /* on commence � ajouter les g�n�ralit�s */

  node = xmlNewChild ( doc->root,
		       NULL,
		       "G�n�ralit�s",
		       NULL );
  xmlNewTextChild ( node,
		    NULL,
		    "Version",
		    VERSION );

  /* on met fichier_ouvert � 0 car si c'est une backup ... */
  /* qd c'est un enregistrement normal, la mise � 1 se fait plus tard */

  xmlNewTextChild ( node,
		    NULL,
		    "Fichier_ouvert",
		    "0" );

  etat.fichier_deja_ouvert = 0;

  xmlNewTextChild ( node,
		    NULL,
		    "Backup",
		    nom_fichier_backup );
  xmlNewTextChild ( node,
		    NULL,
		    "Titre",
		    titre_fichier);
  xmlNewTextChild ( node,
		    NULL,
		    "Adresse_commune",
		    adresse_commune);
  xmlNewTextChild ( node,
		    NULL,
		    "Utilise_exercices",
		    itoa (etat.utilise_exercice));
  xmlNewTextChild ( node,
		    NULL,
		    "Utilise_IB",
		    itoa (etat.utilise_imputation_budgetaire));
  xmlNewTextChild ( node,
		    NULL,
		    "Utilise_PC",
		    itoa (etat.utilise_piece_comptable));
  xmlNewTextChild ( node,
		    NULL,
		    "Utilise_info_BG",
		    itoa (etat.utilise_info_banque_guichet));
  xmlNewTextChild ( node,
		    NULL,
		    "Num�ro_devise_totaux_tiers",
		    itoa ( no_devise_totaux_tiers));
  xmlNewTextChild ( node,
		    NULL,
		    "Type_affichage_des_�ch�ances",
		    itoa (affichage_echeances));
  xmlNewTextChild ( node,
		    NULL,
		    "Affichage_echeances_perso_nb_libre",
		    itoa (affichage_echeances_perso_nb_libre));
  xmlNewTextChild ( node,
		    NULL,
		    "Type_affichage_perso_echeances",
		    itoa (affichage_echeances_perso_j_m_a));
  xmlNewTextChild ( node,
		    NULL,
		    "Num�ro_derniere_operation",
		    itoa ( no_derniere_operation));


  /*   on commence la sauvegarde des comptes : 2 parties, les g�n�ralit�s */
  /* puis les comptes 1 par 1 */

  node = xmlNewChild ( doc->root,
		       NULL,
		       "Comptes",
		       NULL );

  /* c'est node_1 qui va contenir les g�n�ralit�s puis les comptes */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "G�n�ralit�s",
			 NULL );

  /* cr�ation de l'ordre des comptes */

  pointeur_liste = ordre_comptes;
  pointeur_char = NULL;

  do
  {
    if ( pointeur_char )
      pointeur_char = g_strconcat ( pointeur_char,
				    "-",
				    itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
				    NULL );
    else
      pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

    pointeur_liste = pointeur_liste -> next;
  }
  while ( pointeur_liste );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Ordre_des_comptes",
		    pointeur_char );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Compte_courant",
		    itoa ( compte_courant ) );


  /* mise en place des comptes 1 par 1 */

  for ( i=0 ; i < nb_comptes ; i++)
    {
      xmlNodePtr node_compte;
   
      p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;

      node_1 = xmlNewChild ( node,
			     NULL,
			     "Compte",
			     NULL );

      /* on met d'abord le d�tail du compte, les types puis les op�rations */

      /* mise en forme de la date du dernier relev� */

      if ( DATE_DERNIER_RELEVE )
	pointeur_char = g_strconcat ( itoa ( g_date_day ( DATE_DERNIER_RELEVE ) ),
				      "/",
				      itoa ( g_date_month ( DATE_DERNIER_RELEVE ) ),
				      "/",
				      itoa ( g_date_year ( DATE_DERNIER_RELEVE ) ),
				      NULL );
      else
	pointeur_char = NULL;


      node_compte = xmlNewChild ( node_1,
				  NULL,
				  "D�tails",
				  NULL );

      xmlNewTextChild ( node_compte,
			NULL,
			"Nom",
			NOM_DU_COMPTE );

      xmlNewTextChild ( node_compte,
			NULL,
			"No_de_compte",
			itoa ( NO_COMPTE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Titulaire",
			TITULAIRE );

      xmlNewTextChild ( node_compte,
			NULL,
			"Type_de_compte",
			itoa ( TYPE_DE_COMPTE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Nb_op�rations",
			itoa ( NB_OPE_COMPTE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Devise",
			itoa ( DEVISE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Banque",
			itoa ( BANQUE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Guichet",
			NO_GUICHET );

      xmlNewTextChild ( node_compte,
			NULL,
			"No_compte_banque",
			NO_COMPTE_BANQUE );

      xmlNewTextChild ( node_compte,
			NULL,
			"Cl�_du_compte",
			CLE_COMPTE );

      xmlNewTextChild ( node_compte,
			NULL,
			"Solde_initial",
			g_strdup_printf ( "%4.2f",
					  SOLDE_INIT ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Solde_mini_voulu",
			g_strdup_printf ( "%4.2f",
					  SOLDE_MINI_VOULU ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Solde_mini_autoris�",
			g_strdup_printf ( "%4.2f",
					  SOLDE_MINI ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Solde_courant",
			g_strdup_printf ( "%4.2f",
					  SOLDE_COURANT ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Date_dernier_relev�",
			pointeur_char );

      xmlNewTextChild ( node_compte,
			NULL,
			"Solde_dernier_relev�",
			g_strdup_printf ( "%4.2f",
					  SOLDE_DERNIER_RELEVE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Dernier_no_de_rapprochement",
			itoa ( DERNIER_NO_RAPPROCHEMENT ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Compte_clotur�",
			itoa ( COMPTE_CLOTURE ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Commentaires",
			COMMENTAIRE );

      xmlNewTextChild ( node_compte,
			NULL,
			"Adresse_du_titulaire",
			ADRESSE_TITULAIRE );

      xmlNewTextChild ( node_compte,
			NULL,
			"Nombre_de_types",
			itoa ( g_slist_length ( TYPES_OPES ) ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Type_d�faut_d�bit",
			itoa ( TYPE_DEFAUT_DEBIT ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Type_d�faut_cr�dit",
			itoa ( TYPE_DEFAUT_CREDIT ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Tri_par_type",
			itoa ( TRI ));

      xmlNewTextChild ( node_compte,
			NULL,
			"Neutres_inclus",
			itoa ( NEUTRES_INCLUS ));

      /* mise en place de l'ordre du tri */


      pointeur_liste = LISTE_TRI;
      pointeur_char = NULL;

      while ( pointeur_liste )
	{
	  if ( pointeur_char )
	    pointeur_char = g_strconcat ( pointeur_char,
					  "/",
					  itoa ( GPOINTER_TO_INT ( pointeur_liste -> data )),
					  NULL );
	  else
	    pointeur_char = itoa ( GPOINTER_TO_INT ( pointeur_liste -> data ));

	  pointeur_liste = pointeur_liste -> next;
	}

      xmlNewTextChild ( node_compte,
			NULL,
			"Ordre_du_tri",
			pointeur_char );

      /* mise en place des types */

      node_compte = xmlNewChild ( node_1,
				  NULL,
				  "D�tail_de_Types",
				  NULL );

      pointeur_liste = TYPES_OPES;

      while ( pointeur_liste )
	{
	  struct struct_type_ope *type;
	  xmlNodePtr node_type;

	  node_type = xmlNewChild ( node_compte,
				    NULL,
				    "Type",
				    NULL );

	  type = pointeur_liste -> data;


	  xmlSetProp ( node_type,
		       "No",
		       itoa ( type -> no_type ));

	  xmlSetProp ( node_type,
		       "Nom",
		       type -> nom_type );

	  xmlSetProp ( node_type,
		       "Signe",
		       itoa ( type -> signe_type ));

	  xmlSetProp ( node_type,
		       "Affiche_entree",
		       itoa ( type -> affiche_entree ));

	  xmlSetProp ( node_type,
		       "Num�rotation_auto",
		       itoa ( type -> numerotation_auto ));

	  xmlSetProp ( node_type,
		       "No_en_cours",
		       itoa ( type -> no_en_cours ));

	  pointeur_liste = pointeur_liste -> next;
	}


      /* mise en place des op�rations */

      node_compte = xmlNewChild ( node_1,
				  NULL,
				  "D�tail_des_op�rations",
				  NULL );

      pointeur_liste = LISTE_OPERATIONS;

      while ( pointeur_liste )
	{
	  struct structure_operation *operation;
	  xmlNodePtr node_ope;

	  operation = pointeur_liste -> data;

	  node_ope = xmlNewChild ( node_compte,
				   NULL,
				   "Op�ration",
				   NULL );

	  xmlSetProp ( node_ope,
		       "No",
		       itoa ( operation -> no_operation ));

	  xmlSetProp ( node_ope,
		       "D",
		       g_strdup_printf ( "%d/%d/%d",
					 operation -> jour,
					 operation -> mois,
					 operation -> annee ));

    /* GDC : Ecriture de la date bancaire */
	  xmlSetProp ( node_ope,
		       "Db",
		       g_strdup_printf ( "%d/%d/%d",
					 operation -> jour_bancaire,
					 operation -> mois_bancaire,
					 operation -> annee_bancaire ));
    /* GDCFin */

	  xmlSetProp ( node_ope,
		       "M",
		       g_strdup_printf ( "%4.2f",
					 operation -> montant ));

	  xmlSetProp ( node_ope,
		       "De",
		       itoa ( operation -> devise ));

	  xmlSetProp ( node_ope,
		       "Rdc",
		       itoa ( operation -> une_devise_compte_egale_x_devise_ope ));

	  xmlSetProp ( node_ope,
		       "Tc",
		       g_strdup_printf ( "%4.2f",
					 operation -> taux_change ));

	  xmlSetProp ( node_ope,
		       "Fc",
		       g_strdup_printf ( "%4.2f",
					 operation -> frais_change ));

	  xmlSetProp ( node_ope,
		       "T",
		       itoa ( operation -> tiers ));

	  xmlSetProp ( node_ope,
		       "C",
		       itoa ( operation -> categorie ));

	  xmlSetProp ( node_ope,
		       "Sc",
		       itoa ( operation -> sous_categorie ));

	  xmlSetProp ( node_ope,
		       "Ov",
		       itoa ( operation -> operation_ventilee ));

	  xmlSetProp ( node_ope,
		       "N",
		       operation -> notes );

	  xmlSetProp ( node_ope,
		       "Ty",
		       itoa ( operation -> type_ope ));

	  xmlSetProp ( node_ope,
		       "Ct",
		       operation -> contenu_type );

	  xmlSetProp ( node_ope,
		       "P",
		       itoa ( operation -> pointe ));

	  xmlSetProp ( node_ope,
		       "A",
		       itoa ( operation -> auto_man ));

	  xmlSetProp ( node_ope,
		       "R",
		       itoa ( operation -> no_rapprochement ));

	  xmlSetProp ( node_ope,
		       "E",
		       itoa ( operation -> no_exercice ));

	  xmlSetProp ( node_ope,
		       "I",
		       itoa ( operation -> imputation ));

	  xmlSetProp ( node_ope,
		       "Si",
		       itoa ( operation -> sous_imputation ));

	  xmlSetProp ( node_ope,
		       "Pc",
		       operation -> no_piece_comptable );

	  xmlSetProp ( node_ope,
		       "Ibg",
		       operation -> info_banque_guichet );

	  xmlSetProp ( node_ope,
		       "Ro",
		       itoa ( operation -> relation_no_operation ));

	  xmlSetProp ( node_ope,
		       "Rc",
		       itoa ( operation -> relation_no_compte ));

	  xmlSetProp ( node_ope,
		       "Va",
		       itoa ( operation -> no_operation_ventilee_associee ));

	  pointeur_liste = pointeur_liste -> next;
	}
    }


  /* on sauvegarde maintenant les �ch�ances : g�n�ralit�s puis liste des �ch�ances */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "�ch�ances",
		       NULL );

  /* c'est node_1 qui va contenir les g�n�ralit�s */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "G�n�ralit�s",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_�ch�ances",
		    itoa ( nb_echeances ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_derni�re_�ch�ance",
		    itoa ( no_derniere_echeance ));

  /* on met les �ch�ances */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "D�tail_des_�ch�ances",
			 NULL );

  pointeur_liste = gsliste_echeances;

  while ( pointeur_liste )
    {
      struct operation_echeance *echeance;
      xmlNodePtr node_echeance;

      echeance = pointeur_liste -> data;


      node_echeance = xmlNewChild ( node_1,
				    NULL,
				    "�ch�ance",
				    NULL );

      xmlSetProp ( node_echeance,
		   "No",
		   itoa ( echeance -> no_operation ));

      xmlSetProp ( node_echeance,
		   "Date",
		   g_strdup_printf ( "%d/%d/%d",
				     echeance -> jour,
				     echeance -> mois,
				     echeance -> annee ));

      xmlSetProp ( node_echeance,
		   "Compte",
		   itoa ( echeance -> compte ));

      xmlSetProp ( node_echeance,
		   "Montant",
		   g_strdup_printf ( "%4.2f",
				     echeance -> montant ));

      xmlSetProp ( node_echeance,
		   "Devise",
		   itoa ( echeance -> devise ));

      xmlSetProp ( node_echeance,
		   "Tiers",
		   itoa ( echeance -> tiers ));

      xmlSetProp ( node_echeance,
		   "Cat�gorie",
		   itoa ( echeance -> categorie ));

      xmlSetProp ( node_echeance,
		   "Sous-cat�gorie",
		   itoa ( echeance -> sous_categorie ));

      xmlSetProp ( node_echeance,
		   "Virement_compte",
		   itoa ( echeance -> compte_virement ));

      xmlSetProp ( node_echeance,
		   "Type",
		   itoa ( echeance -> type_ope ));

      xmlSetProp ( node_echeance,
		   "Contenu_du_type",
		   echeance -> contenu_type );

      xmlSetProp ( node_echeance,
		   "Exercice",
		   itoa ( echeance -> no_exercice ));

      xmlSetProp ( node_echeance,
		   "Imputation",
		   itoa ( echeance -> imputation ));

      xmlSetProp ( node_echeance,
		   "Sous-imputation",
		   itoa ( echeance -> sous_imputation ));

      xmlSetProp ( node_echeance,
		   "Pi�ce_comptable",
		   echeance -> no_piece_comptable );

      xmlSetProp ( node_echeance,
		   "Info_baque_guichet",
		   echeance -> info_banque_guichet );

      xmlSetProp ( node_echeance,
		   "Notes",
		   echeance -> notes );

      xmlSetProp ( node_echeance,
		   "Automatique",
		   itoa ( echeance -> auto_man ));

      xmlSetProp ( node_echeance,
		   "P�riodicit�",
		   itoa ( echeance -> periodicite ));

      xmlSetProp ( node_echeance,
		   "Intervalle_p�riodicit�",
		   itoa ( echeance -> intervalle_periodicite_personnalisee ));

      xmlSetProp ( node_echeance,
		   "P�riodicit�_personnalis�e",
		   itoa ( echeance -> periodicite_personnalisee ));

      if ( echeance -> jour_limite )
	xmlSetProp ( node_echeance,
		     "Date_limite",
		     g_strdup_printf ( "%d/%d/%d",
				       echeance -> jour_limite,
				       echeance -> mois_limite,
				       echeance -> annee_limite ));
      else
	xmlSetProp ( node_echeance,
		     "Date_limite",
		     NULL );

      pointeur_liste = pointeur_liste -> next;
    }


  /* mise en place des tiers : g�n�ralit�s puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Tiers",
		       NULL );

  /* c'est node_1 qui va contenir les g�n�ralit�s */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "G�n�ralit�s",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_tiers",
		    itoa ( nb_enregistrements_tiers ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_dernier_tiers",
		    itoa ( no_dernier_tiers ));

  /* on met les tiers */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "D�tail_des_tiers",
			 NULL );

  pointeur_liste = liste_struct_tiers;

  while ( pointeur_liste )
    {
      struct struct_tiers *tiers;
      xmlNodePtr node_tiers;

      tiers = pointeur_liste -> data;


      node_tiers = xmlNewChild ( node_1,
				 NULL,
				 "Tiers",
				 NULL );

      xmlSetProp ( node_tiers,
		   "No",
		   itoa ( tiers -> no_tiers ));

      xmlSetProp ( node_tiers,
		   "Nom",
		   tiers -> nom_tiers );

      xmlSetProp ( node_tiers,
		   "Informations",
		   tiers -> texte );

      xmlSetProp ( node_tiers,
		   "Liaison",
		   itoa ( tiers -> liaison ));

      pointeur_liste = pointeur_liste -> next;
    }


  /* mise en place des cat�gories : g�n�ralit�s puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Cat�gories",
		       NULL );

  /* c'est node_1 qui va contenir les g�n�ralit�s */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "G�n�ralit�s",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_cat�gories",
		    itoa ( nb_enregistrements_categories ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_derni�re_cat�gorie",
		    itoa ( no_derniere_categorie ));

  /* on met les cat�gories */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "D�tail_des_cat�gories",
			 NULL );

  pointeur_liste = liste_struct_categories;

  while ( pointeur_liste )
    {
      struct struct_categ *categ;
      xmlNodePtr node_categ;
      GSList *pointeur_sous_categ;

      categ = pointeur_liste -> data;


      node_categ = xmlNewChild ( node_1,
				 NULL,
				 "Cat�gorie",
				 NULL );

      xmlSetProp ( node_categ,
		   "No",
		   itoa ( categ -> no_categ ));

      xmlSetProp ( node_categ,
		   "Nom",
		   categ -> nom_categ );

      xmlSetProp ( node_categ,
		   "Type",
		   itoa ( categ -> type_categ ));

      xmlSetProp ( node_categ,
		   "No_derni�re_sous_cag�gorie",
		   itoa ( categ -> no_derniere_sous_categ ));


      /* on ajoute les sous cat�gories */

      pointeur_sous_categ = categ -> liste_sous_categ;

      while ( pointeur_sous_categ )
	{
	  struct struct_sous_categ *sous_categ;
	  xmlNodePtr node_sous_categ;

	  sous_categ = pointeur_sous_categ -> data;

	  node_sous_categ = xmlNewChild ( node_categ,
					  NULL,
					  "Sous-cat�gorie",
					  NULL );

	  xmlSetProp ( node_sous_categ,
		       "No",
		       itoa ( sous_categ -> no_sous_categ ));

	  xmlSetProp ( node_sous_categ,
		       "Nom",
		       sous_categ -> nom_sous_categ );

	  pointeur_sous_categ = pointeur_sous_categ -> next;
	}
      pointeur_liste = pointeur_liste -> next;
    }


  /* mise en place des imputations budg�taires : g�n�ralit�s puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Imputations",
		       NULL );

  /* c'est node_1 qui va contenir les g�n�ralit�s */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "G�n�ralit�s",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_imputations",
		    itoa ( nb_enregistrements_imputations ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_derni�re_imputation",
		    itoa ( no_derniere_imputation ));

  /* on met les cat�gories */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "D�tail_des_imputations",
			 NULL );

  pointeur_liste = liste_struct_imputation;

  while ( pointeur_liste )
    {
      struct struct_imputation *imputation;
      xmlNodePtr node_imputation;
      GSList *pointeur_sous_imputation;

      imputation = pointeur_liste -> data;


      node_imputation = xmlNewChild ( node_1,
				      NULL,
				      "Imputation",
				      NULL );

      xmlSetProp ( node_imputation,
		   "No",
		   itoa ( imputation -> no_imputation ));

      xmlSetProp ( node_imputation,
		   "Nom",
		   imputation -> nom_imputation );

      xmlSetProp ( node_imputation,
		   "Type",
		   itoa ( imputation -> type_imputation ));

      xmlSetProp ( node_imputation,
		   "No_derni�re_sous_imputation",
		   itoa ( imputation -> no_derniere_sous_imputation ));

      /* on ajoute les sous cat�gories */

      pointeur_sous_imputation = imputation -> liste_sous_imputation;

      while ( pointeur_sous_imputation )
	{
	  struct struct_sous_imputation *sous_imputation;
	  xmlNodePtr node_sous_imputation;

	  sous_imputation = pointeur_sous_imputation -> data;

	  node_sous_imputation = xmlNewChild ( node_imputation,
					  NULL,
					  "Cat�gorie",
					  NULL );

	  xmlSetProp ( node_sous_imputation,
		       "No",
		       itoa ( sous_imputation -> no_sous_imputation ));

	  xmlSetProp ( node_sous_imputation,
		       "Nom",
		       sous_imputation -> nom_sous_imputation );

	  pointeur_sous_imputation = pointeur_sous_imputation -> next;
	}
      pointeur_liste = pointeur_liste -> next;
    }


  /* mise en place des devises : g�n�ralit�s puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Devises",
		       NULL );

  /* c'est node_1 qui va contenir les g�n�ralit�s */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "G�n�ralit�s",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_devises",
		    itoa ( nb_devises ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_derni�re_devise",
		    itoa ( no_derniere_devise ));

  /* on met les devises */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "D�tail_des_devises",
			 NULL );

  pointeur_liste = liste_struct_devises;

  while ( pointeur_liste )
    {
      struct struct_devise *devise;
      xmlNodePtr node_devise;

      devise = pointeur_liste -> data;


      node_devise = xmlNewChild ( node_1,
				  NULL,
				  "Devise",
				  NULL );

      xmlSetProp ( node_devise,
		   "No",
		   itoa ( devise -> no_devise ));

      xmlSetProp ( node_devise,
		   "Nom",
		   devise -> nom_devise );

      xmlSetProp ( node_devise,
		   "Code",
		   devise -> code_devise );

      xmlSetProp ( node_devise,
		   "Passage_euro",
		   itoa ( devise -> passage_euro ));

      if ( devise -> date_dernier_change )
	xmlSetProp ( node_devise,
		     "Date_dernier_change",
		     g_strdup_printf (  "%d/%d/%d",
					g_date_day ( devise -> date_dernier_change ),
					g_date_month ( devise -> date_dernier_change ),
					g_date_year ( devise -> date_dernier_change )));
      else
	xmlSetProp ( node_devise,
		     "Date_dernier_change",
		     NULL );

      xmlSetProp ( node_devise,
		   "Rapport_entre_devises",
		   itoa ( devise -> une_devise_1_egale_x_devise_2 ));

      xmlSetProp ( node_devise,
		   "Devise_en_rapport",
		   itoa ( devise -> no_devise_en_rapport ));

      xmlSetProp ( node_devise,
		   "Change",
		   g_strdup_printf ( "%f",
				     devise -> change ));

      pointeur_liste = pointeur_liste -> next;
    }


  /* mise en place des banques : g�n�ralit�s puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Banques",
		       NULL );

  /* c'est node_1 qui va contenir les g�n�ralit�s */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "G�n�ralit�s",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_banques",
		    itoa ( nb_banques ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_derni�re_banque",
		    itoa ( no_derniere_banque ));

  /* on met les banques */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "D�tail_des_banques",
			 NULL );

  pointeur_liste = liste_struct_banques;

  while ( pointeur_liste )
    {
      struct struct_banque *banque;
      xmlNodePtr node_banque;

      banque = pointeur_liste -> data;


      node_banque = xmlNewChild ( node_1,
				  NULL,
				  "Banque",
				  NULL );

      xmlSetProp ( node_banque,
		   "No",
		   itoa ( banque -> no_banque ));

      xmlSetProp ( node_banque,
		   "Nom",
		   banque -> nom_banque );

      xmlSetProp ( node_banque,
		   "Code",
		   banque -> code_banque );

      xmlSetProp ( node_banque,
		   "Adresse",
		   banque -> adr_banque );

      xmlSetProp ( node_banque,
		   "T�l",
		   banque -> tel_banque );

      xmlSetProp ( node_banque,
		   "Mail",
		   banque -> email_banque );

      xmlSetProp ( node_banque,
		   "Web",
		   banque -> web_banque );

      xmlSetProp ( node_banque,
		   "Nom_correspondant",
		   banque -> nom_correspondant );

      xmlSetProp ( node_banque,
		   "Fax_correspondant",
		   banque -> fax_correspondant );

      xmlSetProp ( node_banque,
		   "T�l_correspondant",
		   banque -> tel_correspondant );

      xmlSetProp ( node_banque,
		   "Mail_correspondant",
		   banque -> email_correspondant );

      xmlSetProp ( node_banque,
		   "Remarques",
		   banque -> remarque_banque );

      pointeur_liste = pointeur_liste -> next;
    }

  /* mise en place des exercices : g�n�ralit�s puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Exercices",
		       NULL );

  /* c'est node_1 qui va contenir les g�n�ralit�s */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "G�n�ralit�s",
			 NULL );

  xmlNewTextChild ( node_1,
		    NULL,
		    "Nb_exercices",
		    itoa ( nb_exercices ));

  xmlNewTextChild ( node_1,
		    NULL,
		    "No_dernier_exercice",
		    itoa ( no_derniere_exercice ));

  /* on met les exercices */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "D�tail_des_exercices",
			 NULL );

  pointeur_liste = liste_struct_exercices;

  while ( pointeur_liste )
    {
      struct struct_exercice *exercice;
      xmlNodePtr node_exercice;

      exercice = pointeur_liste -> data;


      node_exercice = xmlNewChild ( node_1,
				  NULL,
				  "Exercice",
				  NULL );

      xmlSetProp ( node_exercice,
		   "No",
		   itoa ( exercice -> no_exercice ));

      xmlSetProp ( node_exercice,
		   "Nom",
		   exercice -> nom_exercice );

      if ( exercice->date_debut )
	xmlSetProp ( node_exercice,
		     "Date_d�but",
		     g_strdup_printf ( "%d/%d/%d",
				       g_date_day ( exercice->date_debut ),
				       g_date_month ( exercice->date_debut ),
				       g_date_year ( exercice->date_debut )));
      else
	xmlSetProp ( node_exercice,
		     "Date_d�but",
		     NULL );

      if ( exercice->date_fin )
	xmlSetProp ( node_exercice,
		     "Date_fin",
		     g_strdup_printf ( "%d/%d/%d",
				       g_date_day ( exercice->date_fin ),
				       g_date_month ( exercice->date_fin ),
				       g_date_year ( exercice->date_fin )));
      else
	xmlSetProp ( node_exercice,
		     "Date_fin",
		     NULL );

      xmlSetProp ( node_exercice,
		   "Affich�",
		   itoa ( exercice -> affiche_dans_formulaire ));

      pointeur_liste = pointeur_liste -> next;
    }


  /* mise en place des no de rapprochement : g�n�ralit�s puis liste */


  node = xmlNewChild ( doc->root,
		       NULL,
		       "Rapprochements",
		       NULL );

  /* c'est node_1 qui va contenir les g�n�ralit�s */

  /* on n'a pas de g�n�ralit�s, peut �tre un jour ... */

/*   node_1 = xmlNewChild ( node, */
/* 			 NULL, */
/* 			 "G�n�ralit�s", */
/* 			 NULL ); */

/*   xmlNewTextChild ( node_1, */
/* 		    NULL, */
/* 		    "Nb_rapprochements", */
/* 		    itoa ( g_slist_length ( liste_no_rapprochements ))); */

  /* on met les rapprochements */

  node_1 = xmlNewChild ( node,
			 NULL,
			 "D�tail_des_rapprochements",
			 NULL );

  pointeur_liste = liste_no_rapprochements;

  while ( pointeur_liste )
    {
      struct struct_no_rapprochement *rapprochement;
      xmlNodePtr node_rapprochement;

      rapprochement = pointeur_liste -> data;


      node_rapprochement = xmlNewChild ( node_1,
					 NULL,
					 "Rapprochement",
					 NULL );

      xmlSetProp ( node_rapprochement,
		   "No",
		   itoa ( rapprochement -> no_rapprochement ));

      xmlSetProp ( node_rapprochement,
		   "Nom",
		   rapprochement -> nom_rapprochement );

      pointeur_liste = pointeur_liste -> next;
    }


  /* l'arbre est fait, on sauvegarde */

  resultat = xmlSaveFile ( nom_fichier_comptes,
			   doc );

  /* on lib�re la m�moire */

  xmlFreeDoc ( doc );


  if ( resultat == -1 )
    {
      dialogue ( g_strconcat ( "Erreur dans l'enregistrement du fichier :\n\n",
			       nom_fichier_comptes,
			       "\n\nErreur :\n",
			       strerror ( errno ),
			       NULL ));
      return ( FALSE );
    }

  modification_fichier ( FALSE );

  affiche_titre_fenetre ();

  return ( TRUE );
}
/***********************************************************************************************************/




/* ************************************************************************************************** */
/* itoa : transforme un integer en chaine ascii */
/* ************************************************************************************************** */

gchar *itoa ( gint integer )
{
  div_t result_div;
  gchar *chaine;
  gint i = 0;
  gint num;

  chaine = malloc ( 11*sizeof (char) );
  num = abs(integer);

  do
    {
      result_div = div ( num, 10 );
      chaine[i] = result_div.rem + 48;
      i++;
    }
  while ( ( num = result_div.quot ));

  chaine[i] = 0;

  g_strreverse ( chaine );

  if ( integer < 0 )
    chaine = g_strconcat ( "-",
			   chaine,
			   NULL );

  return ( chaine );
}
/***********************************************************************************************************/



/***********************************************************************************************************/
/* Fonction vire_les_points_virgules */
/* utilis�e � l'enregistrement de texte */
/* quand les ; sont utilis�s comme s�paration */
/***********************************************************************************************************/

gchar *vire_les_points_virgules ( gchar *origine )
{

  if ( !origine )
    return ( NULL );

  return ( g_strstrip ( g_strdelimit ( g_strdup ( origine ),
				       ";",
				       '/' )));

}
/***********************************************************************************************************/



/***********************************************************************************************************/
/* Fonction vire_les_accolades */
/* utilis�e � l'enregistrement de texte */
/* quand les {} sont utilis�s comme s�paration */
/***********************************************************************************************************/

gchar *vire_les_accolades ( gchar *origine )
{

  if ( !origine )
    return ( NULL );

  origine = g_strstrip ( g_strdelimit ( g_strdup ( origine ),
					"{",
					'/' ));
  return ( g_strstrip ( g_strdelimit ( g_strdup ( origine ),
				       "}",
				       '/' )));

}
/***********************************************************************************************************/




/***********************************************************************************************************/
/* ouvre le fichier, recherche <Fichier_ouvert> */
/* et le met � la valeur demand�e */
/***********************************************************************************************************/

void fichier_marque_ouvert ( gint ouvert )
{
  gchar buffer[17];
  gint retour;

  if ( compression_fichier )
    return;

  /* ouverture du fichier */

  if (!(pointeur_fichier_comptes = fopen ( nom_fichier_comptes, "r+")) )
    {
      dialogue ( g_strconcat ( "Erreur dans la tentative d'ouvrir le fichier\npour le d�clarer comme utilis�\n\n",
			       strerror ( errno ),
			       NULL ));
      return;
    }

  do
    retour = fscanf ( pointeur_fichier_comptes, 
		      "%16s",
		      buffer);
  while ( strcmp ( buffer,
		   "<Fichier_ouvert>" )
	  &&
	  retour != EOF );

  /*   s'il n'a rien trouv�, c'est que c'�tait la version 0.3.1 et on passe */

  if ( retour != EOF )
    fprintf ( pointeur_fichier_comptes,
	      itoa ( ouvert ));

  fclose ( pointeur_fichier_comptes );

  return;
}
/***********************************************************************************************************/
