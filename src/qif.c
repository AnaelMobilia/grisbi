/* ce fichier de la gestion du format qif */


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
#include "qif.h"


#include "devises.h"
#include "dialog.h"
#include "search_glist.h"
#include "utils.h"




/* *******************************************************************************/
gboolean recuperation_donnees_qif ( FILE *fichier )
{
    gchar *pointeur_char;
    gchar **tab_char;
    struct struct_compte_importation *compte;
    gint retour = 0;
    gint format_date;
    GSList *liste_tmp;
    gchar **tab;
    gint pas_le_premier_compte = 0;

    /* fichier pointe sur le fichier qui a �t� reconnu comme qif */

    rewind ( fichier );
    
/*     on n'accepte que les types bank, cash, ccard, invst(avec warning), oth a, oth l */
/* 	le reste, on passe */

    do
    {
	do
	{
/* 	    si ce n'est pas le premier compte du fichier, pointeur_char est d�j� sur la ligne du nouveau compte */
/* 	    tans que pas_le_premier_compte = 1 ; du coup on le met � 2 s'il �tait � 1 */

	    if ( pas_le_premier_compte != 1 )
	    {
		retour = fscanf ( fichier,
				  "%a[^\n]\n",
				  &pointeur_char );
	    }
	    else
		pas_le_premier_compte = 2;

	    if ( retour
		 &&
		 retour != EOF
		 &&
		 pointeur_char
		 &&
		 !my_strncasecmp ( pointeur_char,
				   "!Type",
				   5 ))
	    {
		/* 	    � ce niveau on est sur un !type ou !Type, on v�rifie maintenant qu'on supporte */
		/* 		bien ce type ; si c'est le cas, on met retour � -1 */

		if ( !my_strncasecmp ( pointeur_char+6,
				       "bank",
				       4 )
		     ||
		     !my_strncasecmp ( pointeur_char+6,
				       "cash",
				       4 )
		     ||
		     !my_strncasecmp ( pointeur_char+6,
				       "ccard",
				       5 )
		     ||
		     !my_strncasecmp ( pointeur_char+6,
				       "invst",
				       5 )
		     ||
		     !my_strncasecmp ( pointeur_char+6,
				       "oth a",
				       5 )
		     ||
		     !my_strncasecmp ( pointeur_char+6,
				       "oth l",
				       5 ))
					   retour = -2;
	    }
	}
	while ( retour != EOF
		&&
		retour != -2 );

	/*     si on est d�j� � la fin du fichier,on se barre */

	if ( retour == EOF )
	{
	    if ( !pas_le_premier_compte )
	    {
		dialogue ( _("This file is empty!") );
		return FALSE;
	    }
	    else
		return TRUE;
	}

	/*     on est sur le d�but d'op�rations d'un compte, on cr�e un nouveau compte */

	compte = calloc ( 1,
			  sizeof ( struct struct_compte_importation ));

	/* c'est une importation qif */

	compte -> origine = 0;

	/* r�cup�ration du type de compte */

	if ( !my_strncasecmp ( pointeur_char+6,
			       "bank",
			       4 ))
	    compte -> type_de_compte = 0;
	else
	{
	    if ( !my_strncasecmp ( pointeur_char+6,
				   "invst",
				   5 ))
	    {
/* 		on consid�re le compte d'investissement comme un compte bancaire mais met un */
/* 		    warning car pas impl�ment� ; aucune id�e si �a passe ou pas... */
		compte -> type_de_compte = 0;
		dialogue_warning ( _("Grisbi found an investment account, which is not implemented yet.  Nevertheless, Grisbi will try to import it as a bank account." ));
	    }
	    else
	    {
		if ( !my_strncasecmp ( pointeur_char+6,
				       "cash",
				       4 ))
		    compte -> type_de_compte = 7;
		else
		{
		    if ( !my_strncasecmp ( pointeur_char+6,
					   "oth a",
					   5 ))
			compte -> type_de_compte = 2;
		    else
		    {
			if ( !my_strncasecmp ( pointeur_char+6,
					       "oth l",
					       5 ))
			    compte -> type_de_compte = 3;
			else
			    /* CCard */
			    compte -> type_de_compte = 5;
		    }
		}
	    }
	}
    


	/* r�cup�re les autres donn�es du compte */

	/*       pour un type CCard, le qif commence directement une op� sans donner les */
	/* 	caract�ristiques de d�part du compte, on cr�e donc un compte du nom */
	/* "carte de cr�dit" avec un solde init de 0 */

	if ( my_strncasecmp ( pointeur_char+6,
			      "ccard",
			      5 ))
	{
	    /* ce n'est pas une ccard, on r�cup�re les infos */

	    do
	    {
		free ( pointeur_char );

		retour = fscanf ( fichier,
				  "%a[^\n]\n",
				  &pointeur_char );


		/* r�cup�ration du solde initial ( on doit virer la , que money met pour s�parer les milliers ) */
		/* on ne vire la , que s'il y a un . */

		if ( pointeur_char[0] == 'T' )
		{
		    tab = g_strsplit ( pointeur_char,
				       ".",
				       2 );

		    if( tab[1] )
		    {
			tab_char = g_strsplit ( pointeur_char,
						",",
						FALSE );

			pointeur_char = g_strjoinv ( NULL,
						     tab_char );
			compte -> solde = my_strtod ( pointeur_char + 1,
						      NULL );
			g_strfreev ( tab_char );
		    }
		    else
			compte -> solde = my_strtod ( pointeur_char + 1,
						      NULL );

		    g_strfreev ( tab );

		}


		/* r�cup�ration du nom du compte */
		/* 	      parfois, le nom est entre crochet et parfois non ... */

		if ( pointeur_char[0] == 'L' )
		{
		    sscanf ( pointeur_char,
			     "L%a[^\n]",
			     &compte -> nom_de_compte );

		    /* on vire les crochets s'ils y sont */

		    if ( g_utf8_validate ( compte -> nom_de_compte,-1,NULL ))
		    {
			compte -> nom_de_compte = g_strdelimit ( compte -> nom_de_compte,
								 "[",
								 ' ' );
			compte -> nom_de_compte =  g_strdelimit ( compte -> nom_de_compte,
								  "]",
								  ' ' );
			compte -> nom_de_compte =  g_strstrip ( compte -> nom_de_compte );

		    }
		    else
		    {
			compte -> nom_de_compte = latin2utf8 ( g_strdelimit ( compte -> nom_de_compte,
									      "[",
									      ' ' ));
			compte -> nom_de_compte =  latin2utf8 (g_strdelimit ( compte -> nom_de_compte,
									      "]",
									      ' ' ));
			compte -> nom_de_compte =  latin2utf8 (g_strstrip ( compte -> nom_de_compte ));
		    }
		}

		/* on r�cup�re la date du fichier */
		/*  on ne la traite pas maintenant mais quand on traitera toutes les dates */

		if ( pointeur_char[0] == 'D' )
		    sscanf ( pointeur_char,
			     "D%a[^\n]",
			     &compte -> date_solde_qif );

	    }
	    while ( pointeur_char[0] != '^'
		    &&
		    retour != EOF );
	}
	else
	{
	    /* c'est un compte ccard */

	    compte -> nom_de_compte = g_strdup ( _("Credit card"));
	    compte -> solde = 0;
	    retour = 0;
	}

	/* si le compte n'a pas de nom, on en met un ici */

	if ( !compte -> nom_de_compte )
	    compte -> nom_de_compte = g_strdup ( _("Imported account with no name" ));

	if ( retour == EOF )
	{
	    free (compte);
	    if ( !pas_le_premier_compte )
	    {
		dialogue ( _("This file is empty!") );
		return (FALSE);
	    }
	    else
		return (TRUE);
	}


	/* r�cup�ration des op�rations en brut, on les traitera ensuite */

	do
	{
	    struct struct_ope_importation *operation;
	    struct struct_ope_importation *ventilation;

	    ventilation = NULL;

	    operation = calloc ( 1,
				 sizeof ( struct struct_ope_importation ));

	    do
	    {
		retour = fscanf ( fichier,
				  "%a[^\n]\n",
				  &pointeur_char );

		if ( retour != EOF
		     &&
		     pointeur_char[0] != '^'
		     &&
		     pointeur_char[0] != '!' )
		{
		    /* on vire le 0d � la fin de la cha�ne s'il y est  */

		    if ( pointeur_char [ strlen (pointeur_char)-1 ] == 13 )
			pointeur_char [ strlen (pointeur_char)-1 ] = 0;

		    /* r�cup�ration de la date */
		    /* on la met pour l'instant dans date_tmp, et apr�s avoir r�cup�r� toutes */
		    /* les op�s on transformera les dates en gdate */

		    if ( pointeur_char[0] == 'D' )
			operation -> date_tmp = g_strdup ( pointeur_char + 1 );


		    /* r�cup�ration du pointage */

		    if ( pointeur_char[0] == 'C' )
		    {
			if ( pointeur_char[1] == '*' )
			    operation -> p_r = 1;
			else
			    operation -> p_r = 2;
		    }


		    /* r�cup�ration de la note */

		    if ( pointeur_char[0] == 'M' )
		    {
			operation -> notes = g_strstrip ( g_strdelimit ( pointeur_char + 1,
									 ";",
									 '/' ));

			if ( !g_utf8_validate ( operation -> notes ,-1,NULL ))
			    operation -> notes = latin2utf8 (operation -> notes ); 

			if ( !strlen ( operation -> notes ))
			    operation -> notes = NULL;
		    }


		    /* r�cup�ration du montant ( on doit virer la , que money met pour s�parer les milliers ) */
		    /* on ne vire la , que s'il y a un . */

		    if ( pointeur_char[0] == 'T' )
		    {
			tab = g_strsplit ( pointeur_char,
					   ".",
					   2 );

			if( tab[1] )
			{
			    tab_char = g_strsplit ( pointeur_char,
						    ",",
						    FALSE );

			    pointeur_char = g_strjoinv ( NULL,
							 tab_char );
			    operation -> montant = my_strtod ( pointeur_char + 1,
							       NULL ); 

			    g_strfreev ( tab_char );
			}
			else
			    operation -> montant = my_strtod ( pointeur_char + 1,
							       NULL );


			g_strfreev ( tab );
		    }

		    /* r�cup�ration du ch�que */

		    if ( pointeur_char[0] == 'N' )
			operation -> cheque = my_strtod ( pointeur_char + 1,
							  NULL ); 



		    /* r�cup�ration du tiers */

		    if ( pointeur_char[0] == 'P' )
		    {
			if ( g_utf8_validate ( pointeur_char+1,-1,NULL ))
			{
			    operation -> tiers = g_strdup ( pointeur_char + 1 );
			}
			else
			    operation -> tiers = latin2utf8 (g_strdup ( pointeur_char + 1 )); 
		    }


		    /* r�cup�ration des cat�g */

		    if ( pointeur_char[0] == 'L' )
		    {
			if ( g_utf8_validate ( pointeur_char+1,-1,NULL ))
			{
			    operation -> categ = g_strdup ( pointeur_char + 1 );
			}
			else
			    operation -> categ = latin2utf8 (g_strdup ( pointeur_char + 1 )); 
		    }



		    /* r�cup�ration de la ventilation et de sa categ */

		    if ( pointeur_char[0] == 'S' )
		    {
			/* on commence une ventilation, si une op� �tait en cours, on l'enregistre */

			if ( retour != EOF && operation && operation -> date_tmp )
			{
			    if ( !ventilation )
				compte -> operations_importees = g_slist_append ( compte -> operations_importees,
										  operation );
			}
			else
			{
			    /*c'est la fin du fichier ou l'op� n'est pas valide, donc les ventils ne sont pas valides non plus */

			    free ( operation );

			    if ( ventilation )
				free ( ventilation );

			    operation = NULL;
			    ventilation = NULL;
			}

			/* si une ventilation �tait en cours, on l'enregistre */

			if ( ventilation )
			    compte -> operations_importees = g_slist_append ( compte -> operations_importees,
									      ventilation );

			ventilation = calloc ( 1,
					       sizeof ( struct struct_ope_importation ));

			if ( operation )
			{
			    operation -> operation_ventilee = 1;

			    /* r�cup�ration des donn�es de l'op�ration en cours */

			    ventilation -> date_tmp = g_strdup ( operation -> date_tmp );
			    ventilation -> tiers = operation -> tiers;
			    ventilation -> cheque = operation -> cheque;
			    ventilation -> p_r = operation -> p_r;
			    ventilation -> ope_de_ventilation = 1;
			}

			if ( g_utf8_validate ( pointeur_char+1,-1,NULL ))
			{
			    ventilation -> categ = g_strdup ( pointeur_char + 1 );
			}
			else
			    ventilation -> categ = latin2utf8 (g_strdup ( pointeur_char + 1 )); 


		    }


		    /* r�cup�ration de la note de ventilation */

		    if ( pointeur_char[0] == 'E'
			 &&
			 ventilation )
		    {
			ventilation -> notes = g_strstrip ( g_strdelimit ( pointeur_char + 1,
									   ";",
									   '/' ));

			if ( !g_utf8_validate ( ventilation -> notes ,-1,NULL ))
			    ventilation -> notes = latin2utf8 (ventilation -> notes ); 

			if ( !strlen ( ventilation -> notes ))
			    ventilation -> notes = NULL;
		    }

		    /* r�cup�ration du montant de la ventilation */
		    /* r�cup�ration du montant ( on doit virer la , que money met pour s�parer les milliers ) */
		    /* on ne vire la , que s'il y a un . */

		    if ( pointeur_char[0] == '$'
			 &&
			 ventilation )
		    {
			tab = g_strsplit ( pointeur_char,
					   ".",
					   2 );

			if( tab[1] )
			{
			    tab_char = g_strsplit ( pointeur_char,
						    ",",
						    FALSE );

			    pointeur_char = g_strjoinv ( NULL,
							 tab_char );
			    ventilation -> montant = my_strtod ( pointeur_char + 1,
								 NULL ); 


			    g_strfreev ( tab_char );
			}
			else
			    ventilation -> montant = my_strtod ( pointeur_char + 1,
								 NULL );


			g_strfreev ( tab );
		    }
		}
	    }
	    while ( pointeur_char[0] != '^'
		    &&
		    retour != EOF
		    &&
		    pointeur_char[0] != '!' );

	    /* 	� ce stade, soit on est � la fin d'une op�ration, soit � la fin du fichier */

	    /* 	    en th�orie, on a toujours ^ � la fin d'une op�ration */
	    /* 		donc si on en est � eof ou !, on n'enregistre pas l'op� */

	    if ( retour != EOF
		 &&
		 pointeur_char[0] != '!' )
	    {
		if ( ventilation )
		{
		    compte -> operations_importees = g_slist_append ( compte -> operations_importees,
								      ventilation );
		    ventilation = NULL;
		}
		else
		{
		    if ( !(operation -> date_tmp
			   && 
			   strlen ( g_strstrip (operation -> date_tmp ))))
		    {
			/* 	l'op�ration n'a pas de date, c'est pas normal. pour �viter de la perdre, on va lui */
			/* 	 donner la date 01/01/1970 et on ajoute au tiers [op�ration sans date] */

			operation -> date_tmp = g_strdup ( "01/01/1970" );
			if ( operation -> tiers )
			    operation -> tiers = g_strconcat ( operation -> tiers,
							       _(" [Transaction imported without date]"),
							       NULL );
			else
			    operation -> tiers = g_strdup ( _(" [Transaction imported without date]"));
		    }
		    compte -> operations_importees = g_slist_append ( compte -> operations_importees,
								      operation );
		}
	    }
	}
	/*     on continue � enregistrer les op�s jusqu'� la fin du fichier ou jusqu'� un changement de compte */
	while ( retour != EOF
		&&
		pointeur_char[0] != '!' );

	/* toutes les op�rations du compte ont �t� r�cup�r�es */
	/* on peut maintenant transformer la date_tmp en gdate */

	format_date = 0;

changement_format_date:

	liste_tmp = compte -> operations_importees;

	while ( liste_tmp )
	{
	    struct struct_ope_importation *operation;
	    gchar **tab_str;
	    gint jour, mois, annee;

	    operation = liste_tmp -> data;

	    /*   v�rification qu'il y a une date, sinon on vire l'op� de toute mani�re */

	    if ( operation -> date_tmp)
	    {	      
		/* r�cup�ration de la date qui est du format jj/mm/aaaa ou jj/mm/aa ou jj/mm'aa � partir de 2000 */
		/* 	      si format_date = 0, c'est sous la forme jjmm sinon mmjj */


		tab_str = g_strsplit ( operation -> date_tmp,
				       "/",
				       3 );

		if ( tab_str [2] && tab_str [1] )
		{
		    /* 		  le format est xx/xx/xx, pas d'apostrophe */

		    if ( format_date )
		    {
			mois = my_strtod ( tab_str[0],
					   NULL );
			jour = my_strtod ( tab_str[1],
					   NULL );
		    }
		    else
		    {
			jour = my_strtod ( tab_str[0],
					   NULL );
			mois = my_strtod ( tab_str[1],
					   NULL );
		    }

		    if ( strlen ( tab_str[2] ) == 4 )
			annee = my_strtod ( tab_str[2],
					    NULL );
		    else
		    {
			annee = my_strtod ( tab_str[2],
					    NULL );
			if ( annee < 80 )
			    annee = annee + 2000;
			else
			    annee = annee + 1900;
		    }
		}
		else
		{
		    if ( tab_str[1] )
		    {
			/* le format est xx/xx'xx */

			gchar **tab_str2;

			tab_str2 = g_strsplit ( tab_str[1],
						"'",
						2 );

			if ( format_date )
			{
			    mois = my_strtod ( tab_str[0],
					       NULL );
			    jour = my_strtod ( tab_str2[0],
					       NULL );
			}
			else
			{
			    jour = my_strtod ( tab_str[0],
					       NULL );
			    mois = my_strtod ( tab_str2[0],
					       NULL );
			}

			/* si on avait 'xx, en fait �a peut �tre 'xx ou 'xxxx ... */

			if ( strlen ( tab_str2[1] ) == 2 )
			    annee = my_strtod ( tab_str2[1],
						NULL ) + 2000;
			else
			    annee = my_strtod ( tab_str2[1],
						NULL );
			g_strfreev ( tab_str2 );

		    }
		    else
		    {
			/* le format est aaaa-mm-jj */

			tab_str = g_strsplit ( operation -> date_tmp,
					       "-",
					       3 );

			mois = my_strtod ( tab_str[1],
					   NULL );
			jour = my_strtod ( tab_str[2],
					   NULL );
			if ( strlen ( tab_str[0] ) == 4 )
			    annee = my_strtod ( tab_str[0],
						NULL );
			else
			{
			    annee = my_strtod ( tab_str[0],
						NULL );
			    if ( annee < 80 )
				annee = annee + 2000;
			    else
				annee = annee + 1900;
			}
		    }
		}

		g_strfreev ( tab_str );

		if ( g_date_valid_dmy ( jour,
					mois,
					annee ))
		    operation -> date = g_date_new_dmy ( jour,
							 mois,
							 annee );
		else
		{
		    if ( format_date )
		    {
			dialogue_error_hint ( _("Dates can't be parsed in QIF file."),
					      _("Grisbi automatically tries to parse dates from QIF files using heuristics.  Please double check that they are valid and contact grisbi development team for assistance if needed") );
			return (FALSE);
		    }

		    format_date = 1;

		    goto changement_format_date;
		}
	    }
	    else
	    {
		/* il n'y a pas de date, on vire l'op� de la liste */

		compte -> operations_importees = g_slist_remove ( compte -> operations_importees,
								  liste_tmp -> data );
	    }
	    liste_tmp = liste_tmp -> next;
	}


	/* r�cup�ration de la date du fichier  */
	/* si format_date = 0, c'est sous la forme jjmm sinon mmjj */

	if ( compte -> date_solde_qif )
	{
	    gchar **tab_str;
	    gint jour, mois, annee;

	    tab_str = g_strsplit ( compte -> date_solde_qif,
				   "/",
				   3 );

	    if ( tab_str [2] && tab_str [1] )
	    {
		/* 		  le format est xx/xx/xx, pas d'apostrophe */

		if ( format_date )
		{
		    mois = my_strtod ( tab_str[0],
				       NULL );
		    jour = my_strtod ( tab_str[1],
				       NULL );
		}
		else
		{
		    jour = my_strtod ( tab_str[0],
				       NULL );
		    mois = my_strtod ( tab_str[1],
				       NULL );
		}

		if ( strlen ( tab_str[2] ) == 4 )
		    annee = my_strtod ( tab_str[2],
					NULL );
		else
		{
		    annee = my_strtod ( tab_str[2],
					NULL );
		    if ( annee < 80 )
			annee = annee + 2000;
		    else
			annee = annee + 1900;
		}
	    }
	    else
	    {
		if ( tab_str[1] )
		{
		    /* le format est xx/xx'xx */

		    gchar **tab_str2;

		    tab_str2 = g_strsplit ( tab_str[1],
					    "'",
					    2 );

		    if ( format_date )
		    {
			mois = my_strtod ( tab_str[0],
					   NULL );
			jour = my_strtod ( tab_str2[0],
					   NULL );
		    }
		    else
		    {
			jour = my_strtod ( tab_str[0],
					   NULL );
			mois = my_strtod ( tab_str2[0],
					   NULL );
		    }

		    /* si on avait 'xx, en fait �a peut �tre 'xx ou 'xxxx ... */

		    if ( strlen ( tab_str2[1] ) == 2 )
			annee = my_strtod ( tab_str2[1],
					    NULL ) + 2000;
		    else
			annee = my_strtod ( tab_str2[1],
					    NULL );
		    g_strfreev ( tab_str2 );

		}
		else
		{
		    /* le format est aaaa-mm-jj */

		    tab_str = g_strsplit ( compte -> date_solde_qif,
					   "-",
					   3 );

		    mois = my_strtod ( tab_str[1],
				       NULL );
		    jour = my_strtod ( tab_str[2],
				       NULL );
		    if ( strlen ( tab_str[0] ) == 4 )
			annee = my_strtod ( tab_str[0],
					    NULL );
		    else
		    {
			annee = my_strtod ( tab_str[0],
					    NULL );
			if ( annee < 80 )
			    annee = annee + 2000;
			else
			    annee = annee + 1900;
		    }
		}
	    }

	    g_strfreev ( tab_str );

	    if ( g_date_valid_dmy ( jour,
				    mois,
				    annee ))
		compte -> date_fin = g_date_new_dmy ( jour,
						      mois,
						      annee );
	}


	/* ajoute ce compte aux autres comptes import�s */

	liste_comptes_importes = g_slist_append ( liste_comptes_importes,
						  compte );

	/*     si � la fin des op�rations c'�tait un changement de compte et pas la fin du fichier, */
	/*     on y retourne !!! */

	pas_le_premier_compte = 1;
    }
    while ( retour != EOF );

	    return ( TRUE );
}
/* *******************************************************************************/






/* *******************************************************************************/
/* Affiche la fen�tre de s�lection de fichier pour exporter en qif */
/* *******************************************************************************/

void exporter_fichier_qif ( void )
{
    GtkWidget *dialog, *table, *entree, *check_button, *paddingbox;
    gchar *nom_fichier_qif, *montant_tmp;
    GSList *liste_tmp;
    FILE *fichier_qif;
    gint i, resultat;


    if ( !nom_fichier_comptes )
    {
	dialogue_error ( _("Your file must have a name (saved) to be exported.") );
	return;
    }


    dialogue_conditional_info_hint ( _("QIF format does not define currencies."), 
				     _("All transactions will be converted into currency of their account."),
				     &etat.display_message_qif_export_currency ); 

    dialog = gtk_dialog_new_with_buttons ( _("Export QIF files"),
					   GTK_WINDOW(window),
					   GTK_DIALOG_DESTROY_WITH_PARENT,
					   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					   GTK_STOCK_OK, GTK_RESPONSE_OK,
					   NULL );

    gtk_signal_connect ( GTK_OBJECT ( dialog ), "destroy",
			 GTK_SIGNAL_FUNC ( gtk_signal_emit_stop_by_name ), "destroy" );

    paddingbox = new_paddingbox_with_title ( GTK_DIALOG(dialog)->vbox, FALSE,
					     _("Select accounts to export") );
    gtk_box_set_spacing ( GTK_BOX(GTK_DIALOG(dialog)->vbox), 6 );

    table = gtk_table_new ( 2, nb_comptes, FALSE );
    gtk_table_set_col_spacings ( GTK_TABLE ( table ), 12 );
    gtk_box_pack_start ( GTK_BOX(paddingbox), table, TRUE, TRUE, 0 );
    gtk_widget_show ( table );

    /* on met chaque compte dans la table */
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

    for ( i = 0 ; i < nb_comptes ; i++ )
    {

	check_button = gtk_check_button_new_with_label ( NOM_DU_COMPTE );
	gtk_table_attach ( GTK_TABLE ( table ),
			   check_button,
			   0, 1,
			   i, i+1,
			   GTK_SHRINK | GTK_FILL,
			   GTK_SHRINK | GTK_FILL,
			   0, 0 );
	gtk_widget_show ( check_button );

	entree = gtk_entry_new ();
	gtk_entry_set_text ( GTK_ENTRY ( entree ),
			     g_strconcat ( nom_fichier_comptes,
					   "_",
					   g_strdelimit ( g_strdup ( NOM_DU_COMPTE) , " ", '_' ),
					   ".qif",
					   NULL ));
	gtk_widget_set_sensitive ( entree,
				   FALSE );
	gtk_object_set_data ( GTK_OBJECT ( entree ),
			      "no_compte",
			      GINT_TO_POINTER ( i ));
	gtk_table_attach ( GTK_TABLE ( table ),
			   entree,
			   1, 2,
			   i, i+1,
			   GTK_EXPAND | GTK_FILL,
			   GTK_SHRINK | GTK_FILL,
			   0, 0 );
	gtk_widget_show ( entree );


	/*       si on clique sur le check bouton, �a rend �ditable l'entr�e */

	gtk_signal_connect ( GTK_OBJECT ( check_button ),
			     "toggled",
			     GTK_SIGNAL_FUNC ( click_compte_export_qif ),
			     entree );

	p_tab_nom_de_compte_variable++;
    }

    liste_entrees_exportation = NULL;
    gtk_widget_show_all ( dialog );

choix_liste_fichier:
    resultat = gtk_dialog_run ( GTK_DIALOG ( dialog ));

    if ( resultat != GTK_RESPONSE_OK || !liste_entrees_exportation )
    {
	if ( liste_entrees_exportation )
	    g_slist_free ( liste_entrees_exportation );

	gtk_widget_destroy ( dialog );
	return;
    }

    /* v�rification que tous les fichiers sont enregistrables */

    liste_tmp = liste_entrees_exportation;

    while ( liste_tmp )
    {
	struct stat test_fichier;


	nom_fichier_qif = g_strdup ( gtk_entry_get_text ( GTK_ENTRY ( liste_tmp -> data )));


	if ( stat ( nom_fichier_qif, &test_fichier ) != -1 )
	{
	    if ( S_ISREG ( test_fichier.st_mode ) )
	    {
		if ( ! question_yes_no_hint ( g_strdup_printf (_("File '%s' already exists"),
							       nom_fichier_qif),	     
					      _("This will irreversibly overwrite previous file.  There is no undo for this.")) )
		    goto choix_liste_fichier;
	    }
	    else
	    {
		dialogue ( g_strdup_printf ( _("File name '%s' invalid !"),
					     nom_fichier_qif ));
		goto choix_liste_fichier;
	    }
	}


	liste_tmp = liste_tmp -> next;
    }



    /* on est s�r de l'enregistrement, c'est parti ... */


    liste_tmp = liste_entrees_exportation;

    while ( liste_tmp )
    {

	/*       ouverture du fichier, si pb, on marque l'erreur et passe au fichier suivant */

	nom_fichier_qif = g_strdup ( gtk_entry_get_text ( GTK_ENTRY ( liste_tmp -> data )));

	if ( !( fichier_qif = fopen ( nom_fichier_qif,
				      "w" ) ))
	    dialogue ( g_strdup_printf ( _("Error for the file \"%s\" :\n%s"),
					 nom_fichier_qif, strerror ( errno ) ));
	else
	{
	    GSList *pointeur_tmp;
	    struct structure_operation *operation;

	    p_tab_nom_de_compte_variable = 
		p_tab_nom_de_compte
		+
		GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( liste_tmp -> data ),
							"no_compte" ));

	    /* met le type de compte */

	    if ( TYPE_DE_COMPTE == 1 )
		fprintf ( fichier_qif,
			  "!Type:Cash\n" );
	    else
		if ( TYPE_DE_COMPTE == 2
		     ||
		     TYPE_DE_COMPTE == 3 )
		    fprintf ( fichier_qif,
			      "!Type:Oth L\n" );
		else
		    fprintf ( fichier_qif,
			      "!Type:Bank\n" );


	    if ( LISTE_OPERATIONS )
	    {
		/* met la date de la 1�re op�ration comme d�te d'ouverture de compte */

		operation = LISTE_OPERATIONS -> data;

		fprintf ( fichier_qif,
			  "D%d/%d/%d\n",
			  operation -> jour,
			  operation -> mois,
			  operation -> annee );



		/* met le solde initial */

		montant_tmp = g_strdup_printf ( "%4.2f",
						SOLDE_INIT );
		montant_tmp = g_strdelimit ( montant_tmp,
					     ",",
					     '.' );

		fprintf ( fichier_qif,
			  "T%s\n",
			  montant_tmp );


		fprintf ( fichier_qif,
			  "CX\nPOpening Balance\n" );

		/* met le nom du compte */

		fprintf ( fichier_qif,
			  "L%s\n^\n",
			  g_strconcat ( "[",
					NOM_DU_COMPTE,
					"]",
					NULL ) );

		/* on met toutes les op�rations */

		pointeur_tmp = LISTE_OPERATIONS;

		while ( pointeur_tmp )
		{
		    GSList *pointeur;
		    gdouble montant;
		    struct struct_type_ope *type;

		    operation = pointeur_tmp -> data;


		    /* si c'est une op� de ventilation, on la saute pas elle sera recherch�e quand */
		    /* son op� ventil�e sera export�e */

		    if ( !operation -> no_operation_ventilee_associee )
		    {
			/* met la date */

			fprintf ( fichier_qif,
				  "D%d/%d/%d\n",
				  operation -> jour,
				  operation -> mois,
				  operation -> annee );


			/* met le pointage */

			if ( operation -> pointe == 1
			     ||
			     operation -> pointe == 3 )
			    fprintf ( fichier_qif,
				      "C*\n" );
			else
			    if ( operation -> pointe == 2 )
				fprintf ( fichier_qif,
					  "CX\n" );


			/* met les notes */

			if ( operation -> notes )
			    fprintf ( fichier_qif,
				      "M%s\n",
				      operation -> notes );


			/* met le montant, transforme la devise si necessaire */

			montant = calcule_montant_devise_renvoi ( operation -> montant,
								  DEVISE,
								  operation -> devise,
								  operation -> une_devise_compte_egale_x_devise_ope,
								  operation -> taux_change,
								  operation -> frais_change );

			montant_tmp = g_strdup_printf ( "%4.2f",
							montant );
			montant_tmp = g_strdelimit ( montant_tmp,
						     ",",
						     '.' );

			fprintf ( fichier_qif,
				  "T%s\n",
				  montant_tmp );


			/* met le ch�que si c'est un type � num�rotation automatique */

			pointeur = g_slist_find_custom ( TYPES_OPES,
							 GINT_TO_POINTER ( operation -> type_ope ),
							 (GCompareFunc) recherche_type_ope_par_no );

			if ( pointeur )
			{
			    type = pointeur -> data;

			    if ( type -> numerotation_auto )
				fprintf ( fichier_qif,
					  "N%s\n",
					  operation -> contenu_type );
			}

			/* met le tiers */

			pointeur = g_slist_find_custom ( liste_struct_tiers,
							 GINT_TO_POINTER ( operation -> tiers ),
							 (GCompareFunc) recherche_tiers_par_no );

			if ( pointeur )
			    fprintf ( fichier_qif,
				      "P%s\n",
				      ((struct struct_tiers *)(pointeur -> data )) -> nom_tiers );



			/*  on met soit un virement, soit une ventilation, soit les cat�gories */

			/* si c'est une ventilation, on recherche toutes les op�s de cette ventilation */
			/* et les met � la suite */
			/* la cat�gorie de l'op� sera celle de la premi�re op� de ventilation */

			if ( operation -> operation_ventilee )
			{
			    GSList *liste_ventil;
			    gint categ_ope_mise;

			    categ_ope_mise = 0;
			    liste_ventil = LISTE_OPERATIONS;

			    while ( liste_ventil )
			    {
				struct structure_operation *ope_test;

				ope_test = liste_ventil -> data;

				if ( ope_test -> no_operation_ventilee_associee == operation -> no_operation
				     &&
				     ( ope_test -> categorie
				       ||
				       ope_test -> relation_no_operation ))
				{
				    /* on commence par mettre la cat�g et sous categ de l'op� et de l'op� de ventilation */

				    if ( ope_test -> relation_no_operation )
				    {
					/* c'est un virement */

					gpointer **save_ptab;

					save_ptab = p_tab_nom_de_compte_variable;

					p_tab_nom_de_compte_variable = p_tab_nom_de_compte + ope_test -> relation_no_compte;

					if ( !categ_ope_mise )
					{
					    fprintf ( fichier_qif,
						      "L%s\n",
						      g_strconcat ( "[",
								    NOM_DU_COMPTE,
								    "]",
								    NULL ));
					    categ_ope_mise = 1;
					}

					fprintf ( fichier_qif,
						  "S%s\n",
						  g_strconcat ( "[",
								NOM_DU_COMPTE,
								"]",
								NULL ));

					p_tab_nom_de_compte_variable = save_ptab;
				    }
				    else
				    {
					/* c'est du type categ : sous categ */

					pointeur = g_slist_find_custom ( liste_struct_categories,
									 GINT_TO_POINTER ( ope_test -> categorie ),
									 (GCompareFunc) recherche_categorie_par_no );

					if ( pointeur )
					{
					    GSList *pointeur_2;
					    struct struct_categ *categorie;

					    categorie = pointeur -> data;

					    pointeur_2 = g_slist_find_custom ( categorie -> liste_sous_categ,
									       GINT_TO_POINTER ( ope_test -> sous_categorie ),
									       (GCompareFunc) recherche_sous_categorie_par_no );
					    if ( pointeur_2 )
					    {
						if ( !categ_ope_mise )
						{
						    fprintf ( fichier_qif,
							      "L%s\n",
							      g_strconcat ( categorie -> nom_categ,
									    ":",
									    ((struct struct_sous_categ *)(pointeur_2->data)) -> nom_sous_categ,
									    NULL ));
						    categ_ope_mise = 1;
						}

						fprintf ( fichier_qif,
							  "S%s\n",
							  g_strconcat ( categorie -> nom_categ,
									":",
									((struct struct_sous_categ *)(pointeur_2->data)) -> nom_sous_categ,
									NULL ));
					    }
					    else
					    {
						if ( !categ_ope_mise )
						{
						    fprintf ( fichier_qif,
							      "L%s\n",
							      categorie -> nom_categ );
						    categ_ope_mise = 1;
						}

						fprintf ( fichier_qif,
							  "S%s\n",
							  categorie -> nom_categ );
					    }
					}
				    }


				    /* met les notes de la ventilation */

				    if ( ope_test -> notes )
					fprintf ( fichier_qif,
						  "E%s\n",
						  ope_test -> notes );

				    /* met le montant de la ventilation */

				    montant = calcule_montant_devise_renvoi ( ope_test -> montant,
									      DEVISE,
									      operation -> devise,
									      operation -> une_devise_compte_egale_x_devise_ope,
									      operation -> taux_change,
									      operation -> frais_change );

				    montant_tmp = g_strdup_printf ( "%4.2f",
								    montant );
				    montant_tmp = g_strdelimit ( montant_tmp,
								 ",",
								 '.' );

				    fprintf ( fichier_qif,
					      "$%s\n",
					      montant_tmp );

				}

				liste_ventil = liste_ventil -> next;
			    }
			}
			else
			{
			    /* si c'est un virement vers un compte supprim�, �a sera pris comme categ normale vide */

			    if ( operation -> relation_no_operation
				 &&
				 operation -> relation_no_compte >= 0 )
			    {
				/* c'est un virement */

				gpointer **save_ptab;

				save_ptab = p_tab_nom_de_compte_variable;

				p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

				fprintf ( fichier_qif,
					  "L%s\n",
					  g_strconcat ( "[",
							NOM_DU_COMPTE,
							"]",
							NULL ));

				p_tab_nom_de_compte_variable = save_ptab;
			    }
			    else
			    {
				/* c'est du type categ : sous-categ */

				pointeur = g_slist_find_custom ( liste_struct_categories,
								 GINT_TO_POINTER ( operation -> categorie ),
								 (GCompareFunc) recherche_categorie_par_no );

				if ( pointeur )
				{
				    GSList *pointeur_2;
				    struct struct_categ *categorie;

				    categorie = pointeur -> data;

				    pointeur_2 = g_slist_find_custom ( categorie -> liste_sous_categ,
								       GINT_TO_POINTER ( operation -> sous_categorie ),
								       (GCompareFunc) recherche_sous_categorie_par_no );
				    if ( pointeur_2 )
					fprintf ( fichier_qif,
						  "L%s\n",
						  g_strconcat ( categorie -> nom_categ,
								":",
								((struct struct_sous_categ *)(pointeur_2->data)) -> nom_sous_categ,
								NULL ));
				    else
					fprintf ( fichier_qif,
						  "L%s\n",
						  categorie -> nom_categ );
				}
			    }
			}

			fprintf ( fichier_qif,
				  "^\n" );
		    }

		    pointeur_tmp = pointeur_tmp -> next;
		}
	    }
	    else
	    {
		/* le compte n'a aucune op�ration enregistr�e : on ne met pas de date, mais on fait l'ouverture du compte */

		/* met le solde initial */

		montant_tmp = g_strdup_printf ( "%4.2f",
						SOLDE_INIT );
		montant_tmp = g_strdelimit ( montant_tmp,
					     ",",
					     '.' );

		fprintf ( fichier_qif,
			  "T%s\n",
			  montant_tmp );


		fprintf ( fichier_qif,
			  "CX\nPOpening Balance\n" );

		/* met le nom du compte */

		fprintf ( fichier_qif,
			  "L%s\n^\n",
			  g_strconcat ( "[",
					NOM_DU_COMPTE,
					"]",
					NULL ) );
	    }
	    fclose ( fichier_qif );
	}
	liste_tmp = liste_tmp -> next;
    }

    gtk_widget_destroy ( dialog );
}
/* *******************************************************************************/





/* *******************************************************************************/
void click_compte_export_qif ( GtkWidget *bouton,
			       GtkWidget *entree )
{

    if ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( bouton ) ) )
    {
	gtk_widget_set_sensitive ( entree,
				   TRUE );
	liste_entrees_exportation = g_slist_append ( liste_entrees_exportation,
						     entree);
    }
    else
    {
	gtk_widget_set_sensitive ( entree,
				   FALSE );
	liste_entrees_exportation = g_slist_remove ( liste_entrees_exportation,
						     entree);
    }

}
/* *******************************************************************************/

