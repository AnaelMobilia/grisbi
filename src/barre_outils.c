/* fichier qui barre d'outils */
/*           barre_outils.c */

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
#include "barre_outils.h"
#include "operations_liste.h"
#include "operations_formulaire.h"
#include "echeancier_formulaire.h"
#include "traitement_variables.h"


#include "./xpm/ope_1.xpm"
#include "./xpm/ope_2.xpm"
#include "./xpm/ope_3.xpm"
#include "./xpm/ope_4.xpm"
#include "./xpm/ope_sans_r.xpm"
#include "./xpm/ope_avec_r.xpm"
#include "./xpm/image_fleche_haut.xpm"
#include "./xpm/image_fleche_bas.xpm"
#include "./xpm/liste_0.xpm"
#include "./xpm/liste_1.xpm"
#include "./xpm/liste_2.xpm"
#include "./xpm/liste_3.xpm"
#include "./xpm/comments.xpm"

/** Used to display/hide comments in scheduler list */
GtkWidget *scheduler_display_hide_comments;

GtkWidget *bouton_affiche_cache_formulaire_echeancier;
GtkWidget *bouton_affiche_commentaire_echeancier;
GtkWidget *fleche_bas_echeancier;
GtkWidget *fleche_haut_echeancier;
GtkWidget *bouton_ope_lignes[4];
GtkWidget *bouton_affiche_r;
GtkWidget *bouton_enleve_r;
GtkWidget *label_proprietes_operations_compte;




extern GtkTooltips *tooltips_general_grisbi;
extern GtkWidget *tree_view_listes_operations;
extern GtkWidget *arbre_imputation;

/*******************************************************************************************/
GtkWidget *creation_barre_outils ( void )
{
    GtkWidget *hbox;
    GtkWidget *separateur;
    GtkWidget *icone;
    GtkWidget *hbox2;


    /*     on utilise le tooltip g�n�ral */

    if ( !tooltips_general_grisbi )
	tooltips_general_grisbi = gtk_tooltips_new ();


    hbox = gtk_hbox_new ( FALSE,
			  5 );


    separateur = gtk_vseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( separateur );



    /* bouton affiche / cache le formulaire */

    bouton_affiche_cache_formulaire = gtk_button_new ();
    gtk_button_set_relief ( GTK_BUTTON ( bouton_affiche_cache_formulaire ),
			    GTK_RELIEF_NONE );
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton_affiche_cache_formulaire,
			   _("Display/Hide form"),
			   _("Display/Hide form") );
    gtk_widget_set_usize ( bouton_affiche_cache_formulaire,
			   15,
			   15 );

    hbox2 = gtk_hbox_new ( TRUE,
			   0 );
    gtk_container_add ( GTK_CONTAINER ( bouton_affiche_cache_formulaire ),
			hbox2 );
    gtk_widget_show ( hbox2 );

    fleche_haut = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) image_fleche_haut_xpm ));
    gtk_box_pack_start ( GTK_BOX ( hbox2 ),
			 fleche_haut,
			 FALSE,
			 FALSE,
			 0 );

    fleche_bas = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) image_fleche_bas_xpm ));
    gtk_box_pack_start ( GTK_BOX ( hbox2 ),
			 fleche_bas,
			 FALSE,
			 FALSE,
			 0 );


    if ( etat.formulaire_toujours_affiche )
	gtk_widget_show ( fleche_bas );
    else
	gtk_widget_show ( fleche_haut );

    gtk_signal_connect ( GTK_OBJECT ( bouton_affiche_cache_formulaire ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( affiche_cache_le_formulaire ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton_affiche_cache_formulaire,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( bouton_affiche_cache_formulaire );


    separateur = gtk_vseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( separateur );


    /* bouton op�rations 1 ligne */

    bouton_ope_lignes[0] = gtk_radio_button_new ( FALSE );
    gtk_toggle_button_set_mode ( GTK_TOGGLE_BUTTON ( bouton_ope_lignes[0]),
				 FALSE );
    gtk_button_set_relief ( GTK_BUTTON ( bouton_ope_lignes[0] ),
			    GTK_RELIEF_NONE );
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton_ope_lignes[0],
			   _("One line per transaction"),
			   _("One line per transaction") );

    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) ope_1_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton_ope_lignes[0] ),
			icone );
    gtk_widget_set_usize ( bouton_ope_lignes[0],
			   15,
			   15 );
    g_signal_connect_swapped ( GTK_OBJECT ( bouton_ope_lignes[0] ),
			       "button-press-event",
			       GTK_SIGNAL_FUNC ( change_aspect_liste ),
			       GINT_TO_POINTER ( 1 ) );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton_ope_lignes[0],
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton_ope_lignes[0] );


    /* bouton op�rations 2 lignes */

    bouton_ope_lignes[1] = gtk_radio_button_new_from_widget ( GTK_RADIO_BUTTON ( bouton_ope_lignes[0] ));
    gtk_toggle_button_set_mode ( GTK_TOGGLE_BUTTON ( bouton_ope_lignes[1]),
				 FALSE );
    gtk_button_set_relief ( GTK_BUTTON ( bouton_ope_lignes[1] ),
			    GTK_RELIEF_NONE );
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton_ope_lignes[1],
			   _("Two lines per transaction"),
			   _("Two lines per transaction") );

    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) ope_2_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton_ope_lignes[1] ),
			icone );
    g_signal_connect_swapped ( GTK_OBJECT ( bouton_ope_lignes[1] ),
			       "button-press-event",
			       GTK_SIGNAL_FUNC ( change_aspect_liste ),
			       GINT_TO_POINTER ( 2 ) );
    gtk_widget_set_usize ( bouton_ope_lignes[1],
			   15,
			   15 );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton_ope_lignes[1],
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton_ope_lignes[1] );




    /* bouton op�rations 3 lignes */

    bouton_ope_lignes[2] = gtk_radio_button_new_from_widget ( GTK_RADIO_BUTTON ( bouton_ope_lignes[0] ));
    gtk_toggle_button_set_mode ( GTK_TOGGLE_BUTTON ( bouton_ope_lignes[2]),
				 FALSE );
    gtk_button_set_relief ( GTK_BUTTON ( bouton_ope_lignes[2] ),
			    GTK_RELIEF_NONE );
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton_ope_lignes[2],
			   _("Three lines per transaction"),
			   _("Three lines per transaction") );

    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) ope_3_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton_ope_lignes[2] ),
			icone );
    g_signal_connect_swapped ( GTK_OBJECT ( bouton_ope_lignes[2] ),
			       "button-press-event",
			       GTK_SIGNAL_FUNC ( change_aspect_liste ),
			       GINT_TO_POINTER ( 3 ) );
    gtk_widget_set_usize ( bouton_ope_lignes[2],
			   15,
			   15 );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton_ope_lignes[2],
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton_ope_lignes[2] );


    /* bouton op�rations 4 lignes */

    bouton_ope_lignes[3] = gtk_radio_button_new_from_widget ( GTK_RADIO_BUTTON ( bouton_ope_lignes[0] ));
    gtk_toggle_button_set_mode ( GTK_TOGGLE_BUTTON ( bouton_ope_lignes[3]),
				 FALSE );
    gtk_button_set_relief ( GTK_BUTTON ( bouton_ope_lignes[3] ),
			    GTK_RELIEF_NONE );
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton_ope_lignes[3],
			   _("Four lines per transaction"),
			   _("Four lines per transaction") );

    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) ope_4_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton_ope_lignes[3] ),
			icone );
    g_signal_connect_swapped ( GTK_OBJECT ( bouton_ope_lignes[3] ),
			       "button-press-event",
			       GTK_SIGNAL_FUNC ( change_aspect_liste ),
			       GINT_TO_POINTER ( 4 ) );
    gtk_widget_set_usize ( bouton_ope_lignes[3],
			   15,
			   15 );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton_ope_lignes[3],
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton_ope_lignes[3] );


    separateur = gtk_vseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( separateur );


    /* bouton affiche op�rations relev�es */

    bouton_affiche_r = gtk_radio_button_new ( NULL);
    gtk_toggle_button_set_mode ( GTK_TOGGLE_BUTTON ( bouton_affiche_r ),
				 FALSE );
    gtk_button_set_relief ( GTK_BUTTON ( bouton_affiche_r ),
			    GTK_RELIEF_NONE );
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton_affiche_r,
			   _("Display reconciled transactions"),
			   _("Display reconciled transactions") );
    gtk_widget_set_usize ( bouton_affiche_r,
			   15,
			   15 );
    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) (const gchar **) ope_avec_r ));
    gtk_container_add ( GTK_CONTAINER ( bouton_affiche_r ),
			icone );
    g_signal_connect_swapped ( GTK_OBJECT ( bouton_affiche_r ),
			       "button-press-event",
			       GTK_SIGNAL_FUNC ( change_aspect_liste ),
			       GINT_TO_POINTER ( 5 ) );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton_affiche_r,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton_affiche_r );


    /* bouton efface op�rations relev�es */

    bouton_enleve_r = gtk_radio_button_new_from_widget ( GTK_RADIO_BUTTON ( bouton_affiche_r ));
    gtk_toggle_button_set_mode ( GTK_TOGGLE_BUTTON (bouton_enleve_r ),
				 FALSE );
    gtk_button_set_relief ( GTK_BUTTON ( bouton_enleve_r ),
			    GTK_RELIEF_NONE );
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton_enleve_r,
			   _("Mask reconciled transactions"),
			   _("Mask reconciled transactions") );
    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) ope_sans_r ));
    gtk_container_add ( GTK_CONTAINER ( bouton_enleve_r ),
			icone );
    gtk_widget_set_usize ( bouton_enleve_r,
			   15,
			   15 );
    g_signal_connect_swapped ( GTK_OBJECT ( bouton_enleve_r ),
			       "button-press-event",
			       GTK_SIGNAL_FUNC ( change_aspect_liste ),
			       GINT_TO_POINTER ( 6 ) );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton_enleve_r,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton_enleve_r );


    separateur = gtk_vseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( separateur );



    label_proprietes_operations_compte = gtk_label_new (_("Account transactions"));
    gtk_box_pack_end ( GTK_BOX ( hbox ),
		       label_proprietes_operations_compte,
		       FALSE,
		       FALSE,
		       0 );
    gtk_widget_show ( label_proprietes_operations_compte );

    return ( hbox );
}
/*******************************************************************************************/




/****************************************************************************************************/
gboolean change_aspect_liste ( gint demande )
{
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

    switch ( demande )
    {
	case 1 :
	    /*       si retient_affichage_par_compte est mis, on ne change que le compte courant, */
	    /* sinon on change tous les comptes */

	    if ( NB_LIGNES_OPE != 1 )
	    {
		if ( etat.retient_affichage_par_compte )
		{
		    NB_LIGNES_OPE = 1;
		    MISE_A_JOUR = 1;
		}
		else
		{
		    gint i;

		    for ( i=0 ; i<nb_comptes ; i++ )
		    {
			p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;
			NB_LIGNES_OPE = 1;
			MISE_A_JOUR = 1;
		    }
		    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
		}
	    }
	    gtk_button_clicked ( GTK_BUTTON ( bouton_ope_lignes[0] ));
/* 	    doit mettre un g_main_iteration pour calmer le jeu en cas d'un clic tr�s rapide */
/* 		sinon il va r�afficher 3 fois la liste d'op�... */
	    while ( g_main_iteration ( FALSE ));

	    break;


	case 2 :

	    /* ope 2 lignes */

	    /*       si retient_affichage_par_compte est mis, on ne change que le compte courant, */
	    /* sinon on change tous les comptes */

	    if ( NB_LIGNES_OPE != 2 )
	    {
		if ( etat.retient_affichage_par_compte )
		{
		    NB_LIGNES_OPE = 2;
		    MISE_A_JOUR = 1;
		}
		else
		{
		    gint i;

		    for ( i=0 ; i<nb_comptes ; i++ )
		    {
			p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;
			NB_LIGNES_OPE = 2;
			MISE_A_JOUR = 1;
		    }
		    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
		}
	    }
	    gtk_button_clicked ( GTK_BUTTON ( bouton_ope_lignes[1] ));
/* 	    doit mettre un g_main_iteration pour calmer le jeu en cas d'un clic tr�s rapide */
/* 		sinon il va r�afficher 3 fois la liste d'op�... */
	    while ( g_main_iteration ( FALSE ));

	    break;


	case 3 :

	    /* ope 3 lignes */
	    /*       si retient_affichage_par_compte est mis, on ne change que le compte courant, */
	    /* sinon on change tous les comptes */

	    if ( NB_LIGNES_OPE != 3 )
	    {
		if ( etat.retient_affichage_par_compte )
		{
		    NB_LIGNES_OPE = 3;
		    MISE_A_JOUR = 1;
		}
		else
		{
		    gint i;

		    for ( i=0 ; i<nb_comptes ; i++ )
		    {
			p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;
			NB_LIGNES_OPE = 3;
			MISE_A_JOUR = 1;
		    }
		    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
		}
	    }
	    gtk_button_clicked ( GTK_BUTTON ( bouton_ope_lignes[2] ));
/* 	    doit mettre un g_main_iteration pour calmer le jeu en cas d'un clic tr�s rapide */
/* 		sinon il va r�afficher 3 fois la liste d'op�... */
	    while ( g_main_iteration ( FALSE ));

	    break;

	case 4 :

	    /*       si retient_affichage_par_compte est mis, on ne change que le compte courant, */
	    /* sinon on change tous les comptes */

	    if ( NB_LIGNES_OPE != 4 )
	    {
		if ( etat.retient_affichage_par_compte )
		{
		    NB_LIGNES_OPE = 4;
		    MISE_A_JOUR = 1;
		}
		else
		{
		    gint i;

		    for ( i=0 ; i<nb_comptes ; i++ )
		    {
			p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;
			NB_LIGNES_OPE = 4;
			MISE_A_JOUR = 1;
		    }
		    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
		}
	    }

	    gtk_button_clicked ( GTK_BUTTON ( bouton_ope_lignes[3] ));
/* 	    doit mettre un g_main_iteration pour calmer le jeu en cas d'un clic tr�s rapide */
/* 		sinon il va r�afficher 3 fois la liste d'op�... */
	    while ( g_main_iteration ( FALSE ));

	    break;



	case 5 :

	    /* ope avec r */
	    /*       si retient_affichage_par_compte est mis, on ne change que le compte courant, */
	    /* sinon on change tous les comptes */

	    if ( !AFFICHAGE_R )
	    {
		if ( etat.retient_affichage_par_compte )
		{
		    AFFICHAGE_R = 1;
		    MISE_A_JOUR = 1;
		}
		else
		{
		    gint i;

		    for ( i=0 ; i<nb_comptes ; i++ )
		    {
			p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;
			AFFICHAGE_R = 1;
			MISE_A_JOUR = 1;
		    }
		    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
		}
	    }
		    gtk_button_clicked ( GTK_BUTTON ( bouton_affiche_r ));
/* 	    doit mettre un g_main_iteration pour calmer le jeu en cas d'un clic tr�s rapide */
/* 		sinon il va r�afficher 3 fois la liste d'op�... */
	    while ( g_main_iteration ( FALSE ));

    break;

	case 6 :

	    /* ope sans r */
	    /*       si retient_affichage_par_compte est mis, on ne change que le compte courant, */
	    /* sinon on change tous les comptes */

	    if ( AFFICHAGE_R )
	    {
		if ( etat.retient_affichage_par_compte )
		{
		    AFFICHAGE_R = 0;
		    MISE_A_JOUR = 1;
		}
		else
		{
		    gint i;

		    for ( i=0 ; i<nb_comptes ; i++ )
		    {
			p_tab_nom_de_compte_variable = p_tab_nom_de_compte + i;
			AFFICHAGE_R = 0;
			MISE_A_JOUR = 1;
		    }
		    p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;
		}
	    }
	    gtk_button_clicked ( GTK_BUTTON (bouton_enleve_r ));
/* 	    doit mettre un g_main_iteration pour calmer le jeu en cas d'un clic tr�s rapide */
/* 		sinon il va r�afficher 3 fois la liste d'op�... */
	    while ( g_main_iteration ( FALSE ));

	    break;

    }

    /*   si un compte �tait affich� en ce moment, on le met � jour si necessaire */

    if ( gtk_notebook_get_current_page ( GTK_NOTEBOOK ( notebook_general )) == 1 )
	verification_mise_a_jour_liste ();

    modification_fichier ( TRUE );
    return ( TRUE );
}
/* ***************************************************************************************************** */




/*******************************************************************************************/
GtkWidget *creation_barre_outils_echeancier ( void )
{
    GtkWidget *hbox;
    GtkWidget *separateur;
    GtkWidget *hbox2;


    hbox = gtk_hbox_new ( FALSE,
			  5 );
    gtk_widget_show ( hbox );

    separateur = gtk_vseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( separateur );



    /* bouton affiche / cache le formulaire */

    bouton_affiche_cache_formulaire_echeancier = gtk_button_new ();
    gtk_button_set_relief ( GTK_BUTTON ( bouton_affiche_cache_formulaire_echeancier ),
			    GTK_RELIEF_NONE );
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton_affiche_cache_formulaire_echeancier,
			   _("Display/hide form"),
			   _("Display/hide form") );
    gtk_widget_set_usize ( bouton_affiche_cache_formulaire_echeancier,
			   15,
			   15 );

    hbox2 = gtk_hbox_new ( TRUE,
			   0 );
    gtk_container_add ( GTK_CONTAINER ( bouton_affiche_cache_formulaire_echeancier ),
			hbox2 );
    gtk_widget_show ( hbox2 );

    fleche_haut_echeancier = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) image_fleche_haut_xpm ));
    gtk_box_pack_start ( GTK_BOX ( hbox2 ),
			 fleche_haut_echeancier,
			 FALSE,
			 FALSE,
			 0 );

    fleche_bas_echeancier = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) image_fleche_bas_xpm ));
    gtk_box_pack_start ( GTK_BOX ( hbox2 ),
			 fleche_bas_echeancier,
			 FALSE,
			 FALSE,
			 0 );


    if ( etat.formulaire_echeancier_toujours_affiche )
	gtk_widget_show ( fleche_bas_echeancier );
    else
	gtk_widget_show ( fleche_haut_echeancier );

    gtk_signal_connect ( GTK_OBJECT ( bouton_affiche_cache_formulaire_echeancier ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( affiche_cache_le_formulaire_echeancier ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton_affiche_cache_formulaire_echeancier,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( bouton_affiche_cache_formulaire_echeancier );


    separateur = gtk_vseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( separateur );

    /* dOm : ajout commutateur d'affichage de commentaires */
    /* bouton affiche / cache le commentaire dans la liste de l'echeancier */
    scheduler_display_hide_comments = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) comments_xpm ));
    bouton_affiche_commentaire_echeancier = gtk_button_new ();
    gtk_widget_show ( scheduler_display_hide_comments );
    gtk_container_add ( GTK_CONTAINER ( bouton_affiche_commentaire_echeancier ),
			scheduler_display_hide_comments );
    gtk_button_set_relief ( GTK_BUTTON ( bouton_affiche_commentaire_echeancier ),
			    GTK_RELIEF_NONE );
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi ),
			   bouton_affiche_commentaire_echeancier ,
			   _("Display/hide comments"),
			   _("Display/hide comments") );
    gtk_widget_set_usize ( bouton_affiche_commentaire_echeancier ,
			   16, 16 );
    gtk_signal_connect ( GTK_OBJECT ( bouton_affiche_commentaire_echeancier ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( affiche_cache_commentaire_echeancier ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton_affiche_commentaire_echeancier,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( bouton_affiche_commentaire_echeancier );

    return ( hbox );
}
/*******************************************************************************************/




/*******************************************************************************************/
GtkWidget *creation_barre_outils_tiers ( void )
{
    GtkWidget *hbox;
    GtkWidget *separateur;
    GtkWidget *icone;
    GtkWidget *bouton;

    hbox = gtk_hbox_new ( FALSE,
			  5 );
    gtk_widget_show ( hbox );

    separateur = gtk_vseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( separateur );


    /* bouton fermeture de l'arbre */

    bouton = gtk_button_new ();
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_object_set_data ( GTK_OBJECT ( bouton ),
			  "profondeur",
			  GINT_TO_POINTER ( 0 ));
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton,
			   _("Close tree"),
			   _("Close tree") );
    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) liste_0_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton ),
			icone );
    gtk_widget_set_usize ( bouton,
			   15,
			   15 );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( demande_expand_arbre ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton );

    /* bouton ouverture de l'arbre niveau 1 */

    bouton = gtk_button_new ();
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_object_set_data ( GTK_OBJECT ( bouton ),
			  "profondeur",
			  GINT_TO_POINTER ( 1 ));
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton,
			   _("Display accounts"),
			   _("Display accounts") );
    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) liste_1_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton ),
			icone );
    gtk_widget_set_usize ( bouton,
			   15,
			   15 );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( demande_expand_arbre ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton );

    /* bouton ouverture de l'arbre niveau 2 */

    bouton = gtk_button_new ();
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_object_set_data ( GTK_OBJECT ( bouton ),
			  "profondeur",
			  GINT_TO_POINTER ( 2 ));
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton,
			   _("Display transactions"),
			   _("Display transactions") );
    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) liste_2_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton ),
			icone );
    gtk_widget_set_usize ( bouton,
			   15,
			   15 );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( demande_expand_arbre ),
			 NULL );
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton );

    separateur = gtk_vseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( separateur );


    return ( hbox );
}
/*******************************************************************************************/





/*******************************************************************************************/
GtkWidget *creation_barre_outils_categ ( void )
{
    GtkWidget *hbox;
    GtkWidget *separateur;
    GtkWidget *icone;
    GtkWidget *bouton;

    hbox = gtk_hbox_new ( FALSE,
			  5 );
    gtk_widget_show ( hbox );

    separateur = gtk_vseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( separateur );


    /* bouton fermeture de l'arbre */

    bouton = gtk_button_new ();
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_object_set_data ( GTK_OBJECT ( bouton ),
			  "profondeur",
			  GINT_TO_POINTER ( 0 ));
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton,
			   _("Close tree"),
			   _("Close tree") );
    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) liste_0_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton ),
			icone );
    gtk_widget_set_usize ( bouton,
			   15,
			   15 );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( demande_expand_arbre ),
			 GINT_TO_POINTER (1));
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton );

    /* bouton ouverture de l'arbre niveau 1 */

    bouton = gtk_button_new ();
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_object_set_data ( GTK_OBJECT ( bouton ),
			  "profondeur",
			  GINT_TO_POINTER ( 1 ));
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton,
			   _("Display sub-divisions"),
			   _("Display sub-divisions") );
    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) liste_1_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton ),
			icone );
    gtk_widget_set_usize ( bouton,
			   15,
			   15 );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( demande_expand_arbre ),
			 GINT_TO_POINTER (1));
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton );

    /* bouton ouverture de l'arbre niveau 2 */

    bouton = gtk_button_new ();
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_object_set_data ( GTK_OBJECT ( bouton ),
			  "profondeur",
			  GINT_TO_POINTER ( 2 ));
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton,
			   _("Display accounts"),
			   _("Display accounts") );
    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) liste_2_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton ),
			icone );
    gtk_widget_set_usize ( bouton,
			   15,
			   15 );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( demande_expand_arbre ),
			 GINT_TO_POINTER (1));
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton );

    /* bouton ouverture de l'arbre niveau 3 */

    bouton = gtk_button_new ();
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_object_set_data ( GTK_OBJECT ( bouton ),
			  "profondeur",
			  GINT_TO_POINTER ( 3 ));
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton,
			   _("Display transactions"),
			   _("Display transactions") );
    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) liste_3_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton ),
			icone );
    gtk_widget_set_usize ( bouton,
			   15,
			   15 );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( demande_expand_arbre ),
			 GINT_TO_POINTER (1));
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton );

    separateur = gtk_vseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( separateur );


    return ( hbox );
}
/*******************************************************************************************/






/*******************************************************************************************/
GtkWidget *creation_barre_outils_imputation ( void )
{
    GtkWidget *hbox;
    GtkWidget *separateur;
    GtkWidget *icone;
    GtkWidget *bouton;

    hbox = gtk_hbox_new ( FALSE,
			  5 );
    gtk_widget_show ( hbox );

    separateur = gtk_vseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( separateur );


    /* bouton fermeture de l'arbre */

    bouton = gtk_button_new ();
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_object_set_data ( GTK_OBJECT ( bouton ),
			  "profondeur",
			  GINT_TO_POINTER ( 0 ));
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton,
			   _("Close tree"),
			   _("Close tree") );
    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) liste_0_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton ),
			icone );
    gtk_widget_set_usize ( bouton,
			   15,
			   15 );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( demande_expand_arbre ),
			 GINT_TO_POINTER (2));
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton );

    /* bouton ouverture de l'arbre niveau 1 */

    bouton = gtk_button_new ();
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_object_set_data ( GTK_OBJECT ( bouton ),
			  "profondeur",
			  GINT_TO_POINTER ( 1 ));
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton,
			   _("Display sub-divisions"),
			   _("Display sub-divisions") );
    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) liste_1_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton ),
			icone );
    gtk_widget_set_usize ( bouton,
			   15,
			   15 );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( demande_expand_arbre ),
			 GINT_TO_POINTER (2));
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton );

    /* bouton ouverture de l'arbre niveau 2 */

    bouton = gtk_button_new ();
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_object_set_data ( GTK_OBJECT ( bouton ),
			  "profondeur",
			  GINT_TO_POINTER ( 2 ));
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton,
			   _("Display accounts"),
			   _("Display accounts") );
    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) liste_2_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton ),
			icone );
    gtk_widget_set_usize ( bouton,
			   15,
			   15 );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( demande_expand_arbre ),
			 GINT_TO_POINTER (2));
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton );

    /* bouton ouverture de l'arbre niveau 3 */

    bouton = gtk_button_new ();
    gtk_button_set_relief ( GTK_BUTTON ( bouton ),
			    GTK_RELIEF_NONE );
    gtk_object_set_data ( GTK_OBJECT ( bouton ),
			  "profondeur",
			  GINT_TO_POINTER ( 3 ));
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tooltips_general_grisbi  ),
			   bouton,
			   _("Display transactions"),
			   _("Display transactions") );
    icone = gtk_image_new_from_pixbuf ( gdk_pixbuf_new_from_xpm_data ( (const gchar **) liste_3_xpm ));
    gtk_container_add ( GTK_CONTAINER ( bouton ),
			icone );
    gtk_widget_set_usize ( bouton,
			   15,
			   15 );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( demande_expand_arbre ),
			 GINT_TO_POINTER (2));
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 bouton,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show_all ( bouton );

    separateur = gtk_vseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( hbox ),
			 separateur,
			 FALSE,
			 FALSE,
			 0 );
    gtk_widget_show ( separateur );


    return ( hbox );
}
/*******************************************************************************************/


/*******************************************************************************************/
/* �tend l'arbre donn� en argument en fonction du bouton cliqué (profondeur contenue */
/* dans le bouton) */
/*******************************************************************************************/

void demande_expand_arbre ( GtkWidget *bouton,
			    gint *liste )
{
    GtkWidget *ctree;
    gint i;
    GtkCTreeNode *noeud_selectionne;

    if ( liste )
    {
	if ( GPOINTER_TO_INT ( liste ) == 1 )
	    ctree = arbre_categ;
	else
	    ctree = arbre_imputation;
    }
    else
	ctree = arbre_tiers;

    gtk_clist_freeze ( GTK_CLIST ( ctree ));

    /* on doit faire �a �tage par �tage car il y a des ajouts � chaque ouverture de noeud */

    /*   r�cup�re le noeud s�lectionn�, s'il n'y en a aucun, fera tout l'arbre */

    noeud_selectionne = NULL;

    if ( GTK_CLIST ( ctree ) -> selection )
	noeud_selectionne = GTK_CLIST ( ctree ) -> selection -> data;

    gtk_ctree_collapse_recursive ( GTK_CTREE ( ctree ),
				   noeud_selectionne );

    for ( i=0 ; i < GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( bouton ),
							    "profondeur" )) ; i++ )
	gtk_ctree_expand_to_depth ( GTK_CTREE ( ctree ),
				    noeud_selectionne,
				    i + 1);
    gtk_clist_thaw ( GTK_CLIST ( ctree ));
}
/*******************************************************************************************/


/*******************************************************************************************/
/* cette fonction met les boutons du nb lignes par op� et de l'affichage de R en fonction du compte */
/* envoy� en argument */
/*******************************************************************************************/

void mise_a_jour_boutons_caract_liste ( gint no_compte )
{
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;

    switch ( NB_LIGNES_OPE )
    {
	case 1:
	    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_ope_lignes[0] ),
					   TRUE );
	    break;
	case 2:
	    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_ope_lignes[1] ),
					   TRUE );
	    break;
	case 3:
	    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_ope_lignes[2] ),
					   TRUE );
	    break;
	case 4:
	    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_ope_lignes[4] ),
					   TRUE );
	    break;
    }



    /* on met maintenant le bouton r ou pas r */

    if ( AFFICHAGE_R )
	gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_affiche_r ),
				       TRUE );
    else
	gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( bouton_enleve_r ),
				       TRUE );
}
/*******************************************************************************************/
