/* ce fichier de la gestion du format ofx */


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
#include "ofx.h"

#ifdef NOOFX
/* dummy recuperation_donnees_ofx function implementation for system with no LIBOFX */
gboolean recuperation_donnees_ofx ( gchar *nom_fichier )
{
  dialogue_error_hint(_("This build of Grisbi does not support OFX, please recompile Grisbi with OFX support enabled"), 
                      g_strdup_printf (_("Cannot process OFX file '%s'"), nom_fichier));
  return FALSE;
}
#else

#include <libofx/libofx.h>

#include "dialog.h"
#include "utils.h"


/* on doit mettre le compte en cours d'importation en global pour que la libofx puisse le traiter */
/* de plus un fichier ofx peut int�grer plusieurs comptes, donc on cr�e une liste... */

GSList *liste_comptes_importes_ofx;
struct struct_compte_importation *compte_ofx_importation_en_cours;
gint erreur_import_ofx;
gint  message_erreur_operation;

/* *******************************************************************************/
gboolean recuperation_donnees_ofx ( gchar *nom_fichier )
{
    gchar *argv[2];

    liste_comptes_importes_ofx = NULL;
    compte_ofx_importation_en_cours = NULL;
    erreur_import_ofx = 0;
    message_erreur_operation = 0;

    /* 	la lib ofx ne tient pas compte du 1er argument */
    argv[1] = nom_fichier;

    ofx_proc_file ( 2,
		    argv );

    /*     le dernier compte n'a pas �t� ajout� � la liste */

    liste_comptes_importes_ofx = g_slist_append ( liste_comptes_importes_ofx,
						  compte_ofx_importation_en_cours	);

    if ( !compte_ofx_importation_en_cours )
    {
	dialogue_error ( _("The new account has not been created."));
	return ( FALSE );
    }

    if ( erreur_import_ofx )
	erreur_import_ofx = !question_yes_no_hint ( _("Warning" ),
						    _( "An error or warning has occured. Do you still want to import the file ?" ));

    if ( !erreur_import_ofx )
    {
	GSList *liste_tmp;

	liste_tmp = liste_comptes_importes_ofx;

	/* ajoute le ou les compte aux autres comptes import�s */

	while ( liste_tmp )
	{
	    liste_comptes_importes = g_slist_append ( liste_comptes_importes,
						      liste_tmp -> data );
	    liste_tmp = liste_tmp -> next;
	}
    }


    return ( TRUE );
}
/* *******************************************************************************/



/* *******************************************************************************/
int ofx_proc_status_cb(struct OfxStatusData data)
{
    /*     printf ( "ofx_proc_status_cb:\n" ); */
    /*     printf ( "ofx_element_name_valid %d\n", data . ofx_element_name_valid); */
    /*     printf ( "ofx_element_name %s\n", data . ofx_element_name ); */
    /*     printf ( "code_valid %d\n", data . code_valid ); */
    /*     printf ( "code %d\n", data . code); */
    /*     printf ( "name %s\n", data . name); */
    /*     printf ( "description %s\n", data . description); */
    /*     printf ( "severity_valid %d\n", data . severity_valid ); */
    /*     printf ( "severity %d\n", data . severity); */
    /*     printf ( "server_message_valid %d\n", data.server_message_valid ); */
    /*     printf ( "server_message %s\n\n", data . server_message ); */

    /*     	on v�rifie l'�tat, si c'est un warning, on l'affiche, si c'est une erreur, on vire */

    if ( data.severity_valid )
    {
	switch ( data.severity )
	{
	    case INFO :
		/* 		pas de pb, on fait rien */
		break;

	    case WARN :
		if ( data.code_valid )
		    dialogue_warning ( g_strconcat ( _("The file has returned the next message :\n"),
						     data.name,
						     "\n",
						     data.description,
						     NULL ));
		else
		    dialogue_warning ( _("The file has returned a warning message which is no valid."));
		erreur_import_ofx = 1;
		break;

	    case ERROR:
		if ( data.code_valid )
		    dialogue_error ( g_strconcat ( _("The file has returned the next message :\n"),
						   data.name,
						   "\n",
						   data.description,
						   NULL ));
		else
		    dialogue_error ( _("The file has returned a warning message which is no valid."));
		erreur_import_ofx = 1;
		break;
	}		
    }


    return 0;
}
/* *******************************************************************************/






/* *******************************************************************************/
int ofx_proc_security_cb(struct OfxSecurityData data)
{
    printf ( "ofx_proc_security_cb :\n" );
    printf ( "unique_id_valid %d\n", data.unique_id_valid );
    printf ( "unique_id %s\n", data.unique_id );
    printf ( "unique_id_type_valid %d\n", data.unique_id_type_valid );
    printf ( "unique_id_type %s\n", data.unique_id_type );
    printf ( "secname_valid %d\n", data.secname_valid );
    printf ( "secname %s\n", data.secname );
    printf ( "ticker_valid %d\n", data.ticker_valid );
    printf ( "ticker %s\n", data.ticker );
    printf ( "unitprice_valid %d\n", data.unitprice_valid );
    printf ( "unitprice %f\n", data.unitprice );
    printf ( "date_unitprice_valid %d\n", data.date_unitprice_valid );
    printf ( "date_unitprice %s\n", ctime ( &data.date_unitprice ));
    printf ( "currency_valid %d\n", data.currency_valid );
    printf ( "currency %s\n", data.currency );
    printf ( "memo_valid %d\n", data.memo_valid );
    printf ( "memo %s\n", data.memo );

    dialogue ( _("That file contains something on proc_security_cb.\nI didn't have any file to test that function.\nPlease contact the grisbi team to implement that function."));


    return 0;
}
/* *******************************************************************************/




/* *******************************************************************************/

int ofx_proc_account_cb(struct OfxAccountData data)
{
    /*     printf ( "ofx_proc_account_cb\n" );  */
    /*     printf ( "account_id_valid %d\n", data.account_id_valid ); */
    /*     printf ( "account_id %s\n", data.account_id ); */
    /*     printf ( "account_name %s\n", data.account_name ); */
    /*     printf ( "account_type_valid %d\n", data.account_type_valid ); */
    /*     printf ( "account_type %d\n", data.account_type ); */
    /*     printf ( "currency_valid %d\n", data.currency_valid ); */
    /*     printf ( "currency %s\n", data. currency); */

    /*     si on revient ici et qu'un compte �tait en cours, c'est qu'il est fini et qu'on passe au compte */
    /* 	suivant... */

    if ( compte_ofx_importation_en_cours )
	liste_comptes_importes_ofx = g_slist_append ( liste_comptes_importes_ofx,
						      compte_ofx_importation_en_cours );

    compte_ofx_importation_en_cours = calloc ( 1,
					       sizeof ( struct struct_compte_importation ));

    if ( data.account_id_valid )
    {
	compte_ofx_importation_en_cours -> id_compte = g_strdup ( data.account_id );
	compte_ofx_importation_en_cours -> nom_de_compte = g_strdup ( data.account_name );
    }

    compte_ofx_importation_en_cours -> origine = 1;

    if ( data.account_type_valid )
	compte_ofx_importation_en_cours -> type_de_compte = data.account_type;

    if ( data.currency_valid )
	compte_ofx_importation_en_cours -> devise = g_strdup ( data.currency );


    return 0;
}
/* *******************************************************************************/




/* *******************************************************************************/
int ofx_proc_transaction_cb(struct OfxTransactionData data)
{
    struct struct_ope_importation *ope_import;
    GDate *date;


    /*     printf ( "ofx_proc_transaction_cb\n" );  */
    /*     printf ( "account_id_valid : %d  \n", data.account_id_valid ); */
    /*     printf ( "account_id : %s  \n", data.account_id ); */
    /*     printf ( "transactiontype_valid : %d  \n", data.transactiontype_valid ); */
    /*     printf ( "transactiontype : %d  \n", data.transactiontype ); */
    /*     printf ( "invtransactiontype_valid : %d  \n", data.invtransactiontype_valid ); */
    /*     printf ( "invtransactiontype : %d  \n", data.invtransactiontype ); */
    /*     printf ( "units_valid : %d  \n", data. units_valid); */
    /*     printf ( "units : %f  \n", data.units ); */
    /*     printf ( "unitprice_valid : %d  \n", data.unitprice_valid ); */
    /*     printf ( "unitprice : %f  \n", data.unitprice ); */
    /*     printf ( "amount_valid : %d  \n", data.amount_valid ); */
    /*     printf ( "amount : %f  \n", data.amount ); */
    /*     printf ( "fi_id_valid : %d  \n", data.fi_id_valid ); */
    /*     printf ( "fi_id : %s  \n", data.fi_id ); */
    /*     printf ( "unique_id_valid : %d  \n", data.unique_id_valid ); */
    /*     printf ( "unique_id : %s  \n", data.unique_id ); */
    /*     printf ( "unique_id_type_valid : %d  \n", data.unique_id_type_valid ); */
    /*     printf ( "unique_id_type : %s  \n", data.unique_id_type ); */
    /*     printf ( "security_data_valid : %d  \n", data.security_data_valid ); */
    /*     printf ( "security_data_ptr : %s  \n", data.security_data_ptr ); */
    /*     printf ( "date_posted_valid : %d  \n", data.date_posted_valid ); */
    /*     printf ( "date_posted : %s  \n", ctime ( &data.date_posted )); */
    /*     printf ( "date_initiated_valid : %d  \n", data.date_initiated_valid ); */
    /*     printf ( "date_initiated : %s  \n", ctime ( &data.date_initiated )); */
    /*     printf ( "date_funds_available_valid : %d  \n", data.date_funds_available_valid ); */
    /*     printf ( "date_funds_available : %s  \n", ctime ( &data.date_funds_available )); */
    /*     printf ( "fi_id_corrected_valid : %d  \n", data.fi_id_corrected_valid ); */
    /*     printf ( "fi_id_corrected : %s  \n", data.fi_id_corrected ); */
    /*     printf ( "fi_id_correction_action_valid : %d  \n", data.fi_id_correction_action_valid ); */
    /*     printf ( "fi_id_correction_action : %d  \n", data.fi_id_correction_action ); */
    /*     printf ( "server_transaction_id_valid : %d  \n", data.server_transaction_id_valid ); */
    /*     printf ( "server_transaction_id : %s  \n", data.server_transaction_id ); */
    /*     printf ( "check_number_valid : %d  \n", data.check_number_valid ); */
    /*     printf ( "check_number : %s  \n", data.check_number ); */
    /*     printf ( "reference_number_valid : %d  \n", data.reference_number_valid ); */
    /*     printf ( "reference_number : %s  \n", data.reference_number ); */
    /*     printf ( "standard_industrial_code_valid : %d  \n", data.standard_industrial_code_valid ); */
    /*     printf ( "standard_industrial_code : %s  \n", data.standard_industrial_code ); */
    /*     printf ( "payee_id_valid : %d  \n", data.payee_id_valid ); */
    /*     printf ( "payee_id : %s  \n", data.payee_id ); */
    /*     printf ( "name_valid : %d  \n", data.name_valid ); */
    /*     printf ( "name : %s  \n", data.name ); */
    /*     printf ( "memo_valid : %d  \n", data.memo_valid ); */
    /*     printf ( "memo : %s  \n\n\n\n", data.memo ); */

    /* si � ce niveau le comtpe n'est pas cr��, c'est qu'il y a un pb... */

    if ( !compte_ofx_importation_en_cours )
    {
	if ( !message_erreur_operation )
	{
	    dialogue_error ( _("A transaction try to be saved but no account was created...\n"));
	    message_erreur_operation = 1;
	    erreur_import_ofx = 1;
	}
	return 0;
    }

    /*     c'est parti, on cr�e et remplit l'op�ration */

    ope_import = calloc ( 1,
			  sizeof ( struct struct_ope_importation ));

    if ( data.fi_id_valid )
	ope_import -> id_operation = g_strdup ( data.fi_id );

    date = g_date_new ();
    if ( data.date_posted_valid )
    {
	g_date_set_time ( date,
			  data.date_posted );
	if ( g_date_valid ( date ))
	    ope_import -> date_de_valeur = date;
	else
	    ope_import -> date_de_valeur = NULL;
    }

    if ( data.date_initiated_valid )
    {
	g_date_set_time ( date,
			  data.date_initiated );
	if ( g_date_valid ( date ))
	    ope_import -> date = date;
	else
	    ope_import -> date = ope_import -> date_de_valeur;
    }
    else
	ope_import -> date = ope_import -> date_de_valeur;

    if ( data.name_valid )
	ope_import -> tiers = g_strdup ( data.name );

    if ( data.memo_valid )
	ope_import -> notes = g_strdup ( data.memo );

    if ( data.check_number_valid )
	ope_import -> cheque = my_atoi ( data.check_number );

    if ( data.amount_valid )
	ope_import -> montant = data.amount;

    if ( data.transactiontype_valid )
	ope_import -> type_de_transaction = data.transactiontype;

    /*     on peut faire ici des ptites modifs en fonction du type de transaction, */
    /*     mais tout n'est pas utilis� par les banques... */

    if ( data.transactiontype_valid )
    {
	switch ( data.transactiontype )
	{
	    case OFX_CHECK:
		/* 		   si c'est un ch�que, svt ya pas de tiers, on va mettre ch�que...  */

		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Check"));
		break;
	    case OFX_INT:
		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Interest"));
		break;
	    case OFX_DIV:
		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Dividend"));
		break;
	    case OFX_SRVCHG:
		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Service charge"));
		break;
	    case OFX_FEE:
		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Fee"));
		break;
	    case OFX_DEP:
		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Deposit"));
		break;
	    case OFX_ATM:
		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Cash dispenser"));
		break;
	    case OFX_POS:
		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Point of sale"));
		break;
	    case OFX_XFER:
		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Transfer"));
		break;
	    case OFX_PAYMENT:
		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Electronic payment"));
		break;
	    case OFX_CASH:
		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Cash"));
		break;
	    case OFX_DIRECTDEP:
		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Direct deposit"));
		break;
	    case OFX_DIRECTDEBIT:
		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Merchant initiated debit"));
		break;
	    case OFX_REPEATPMT:
		if ( !ope_import -> tiers )
		    ope_import -> tiers = g_strdup ( _("Repeating payment/standing order"));
		break;

	    case OFX_DEBIT:
	    case OFX_CREDIT:
	    case OFX_OTHER:
		break;
	}
    }
    /*     on ajoute l'op� � son compte */

    compte_ofx_importation_en_cours -> operations_importees = g_slist_append ( compte_ofx_importation_en_cours -> operations_importees,
									       ope_import );


    return 0; 
}
/* *******************************************************************************/



/* *******************************************************************************/
int ofx_proc_statement_cb(struct OfxStatementData data)
{
    GDate *date;

    /*     printf ( "ofx_proc_statement_cb\n" ); */
    /*     printf ( "currency_valid : %d\n", data.currency_valid ); */
    /*     printf ( "currency : %s\n", data.currency ); */
    /*     printf ( "account_id_valid : %d\n", data.account_id_valid ); */
    /*     printf ( "account_id : %s\n", data.account_id ); */
    /*     printf ( "ledger_balance_valid : %d\n", data.ledger_balance_valid ); */
    /*     printf ( "ledger_balance : %f\n", data.ledger_balance ); */
    /*     printf ( "ledger_balance_date_valid : %d\n", data.ledger_balance_date_valid ); */
    /*     printf ( "ledger_balance_date : %s\n", ctime ( &data.ledger_balance_date)); */
    /*     printf ( "available_balance_valid : %d\n", data.available_balance_valid ); */
    /*     printf ( "available_balance : %f\n", data.available_balance ); */
    /*     printf ( "available_balance_date_valid : %d\n", data.available_balance_date_valid ); */
    /*     printf ( "available_balance_date : %s\n", ctime ( &data.available_balance_date )); */
    /*     printf ( "date_start_valid : %d\n", data.date_start_valid ); */
    /*     printf ( "date_start : %s\n", ctime ( &data.date_start )); */
    /*     printf ( "date_end_valid : %d\n", data.date_end_valid ); */
    /*     printf ( "date_end : %s\n", ctime ( &data.date_end )); */
    /*     printf ( "marketing_info_valid : %d\n", data.marketing_info_valid ); */
    /*     printf ( "marketing_info : %s\n", data.marketing_info ); */

    if ( data.date_start_valid )
    {
	date = g_date_new ();

	g_date_set_time ( date,
			  data.date_start );
	if ( g_date_valid ( date ))
	    compte_ofx_importation_en_cours -> date_depart = date;
    }

    if ( data.date_end_valid )
    {
	date = g_date_new ();

	g_date_set_time ( date,
			  data.date_end );
	if ( g_date_valid ( date ))
	    compte_ofx_importation_en_cours -> date_fin = date;
    }

    return 0;
}
#endif
/* *******************************************************************************/



