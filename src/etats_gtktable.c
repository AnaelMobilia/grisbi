/*  Fichier qui s'occupe d'afficher les �tats via une gtktable */
/*      etats_gtktable.c */

/*     Copyright (C)	2000-2003 C�dric Auger (cedric@grisbi.org)	      */
/*			2004 Benjamin Drieu (bdrieu@april.org)		      */
/* 			http://www.grisbi.org				      */

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
#include "etats_gtktable.h"

#include "barre_outils.h"
#include "etats.h"
#include "operations_comptes.h"
#include "operations_liste.h"
#include "search_glist.h"
#include "utils.h"
#include "operations_onglet.h"



gint gtktable_initialise (GSList * opes_selectionnees);
gint gtktable_finish ();
void gtktable_attach_hsep ( int x, int x2, int y, int y2);
void gtktable_attach_vsep ( int x, int x2, int y, int y2);
void gtktable_attach_label ( gchar * text, gdouble properties, int x, int x2, int y, int y2, 
			     enum alignement align, struct structure_operation * ope );

struct struct_etat_affichage gtktable_affichage = {
    gtktable_initialise,
    gtktable_finish,
    gtktable_attach_hsep,
    gtktable_attach_vsep,
    gtktable_attach_label,
};


/**
 * Draw a label 
 *
 */
void gtktable_attach_label ( gchar * text, gdouble properties, int x, int x2, int y, int y2, 
			     enum alignement align, struct structure_operation * ope )
{
    GtkWidget * label;
    GtkStyle * style;

    if (!text)
    {
	text = "";
    }

    label = gtk_label_new ( text );
    gtk_label_set_line_wrap ( GTK_LABEL(label), TRUE );

    switch (align) 
    {
	case LEFT:
	    gtk_misc_set_alignment ( GTK_MISC ( label ), 0.0, 0.0 );
	    break;
	case CENTER:
	    gtk_misc_set_alignment ( GTK_MISC ( label ), 0.5, 0.0 );
	    break;
	case RIGHT:
	    gtk_misc_set_alignment ( GTK_MISC ( label ), 1.0, 0.0 );
	    break;
    }

    style = gtk_style_copy ( gtk_widget_get_style (label));

    if ( ope )
    {
	GtkWidget *event_box;
	GdkColor color;

	/* Put prelight */
	color.red =   1.00 * 65535 ;
	color.green = 0.00 * 65535 ;
	color.blue =  0.00 * 65535 ;
	style->fg[GTK_STATE_PRELIGHT] = color;

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
				    (GtkObject *) ope );
	gtk_table_attach ( GTK_TABLE ( table_etat ), event_box,
			   x, x2, y, y2,
			   GTK_SHRINK | GTK_FILL,
			   GTK_SHRINK | GTK_FILL,
			   0, 0 );
	gtk_widget_show ( event_box );
	gtk_container_add ( GTK_CONTAINER ( event_box ), label );
    }
    else
    {
	gtk_table_attach ( GTK_TABLE ( table_etat ), label,
			   x, x2, y, y2,
			   GTK_SHRINK | GTK_FILL,
			   GTK_SHRINK | GTK_FILL,
			   0, 0 );
    }

    if ( ((gint) properties) & TEXT_ITALIC)
	pango_font_description_set_style ( style -> font_desc, 
					   PANGO_STYLE_ITALIC );
    if ( ((gint) properties) & TEXT_BOLD)
	pango_font_description_set_weight ( style -> font_desc, 
					    PANGO_WEIGHT_BOLD );
    if ( ((gint) properties) & TEXT_HUGE )
	pango_font_description_set_size ( style -> font_desc, 
					  pango_font_description_get_size(style->font_desc) + 100 );
    if ( ((gint) properties) & TEXT_LARGE )
	pango_font_description_set_size ( style -> font_desc, 
					  pango_font_description_get_size(style->font_desc) + 2 );
    if ( ((gint) properties) & TEXT_SMALL )
	pango_font_description_set_size ( style -> font_desc, 
					  pango_font_description_get_size(style->font_desc) - 2 );

    gtk_widget_set_style ( label, style );
    gtk_widget_show ( label );
}



/**
 * FIXME: TODO
 *
 */
void gtktable_attach_vsep ( int x, int x2, int y, int y2)
{
    GtkWidget * separateur;

    separateur = gtk_vseparator_new ();
    gtk_table_attach ( GTK_TABLE ( table_etat ),
		       separateur,
		       x, x2, y, y2,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0 );
    gtk_widget_show ( separateur );
}


/**
 * FIXME: TODO
 *
 */
void gtktable_attach_hsep ( int x, int x2, int y, int y2)
{
    GtkWidget * separateur;

    separateur = gtk_hseparator_new ();
    gtk_table_attach ( GTK_TABLE ( table_etat ),
		       separateur,
		       x, x2, y, y2,
		       GTK_SHRINK | GTK_FILL,
		       GTK_SHRINK | GTK_FILL,
		       0, 0 );
    gtk_widget_show ( separateur );
}



/*****************************************************************************************************/
gint gtktable_initialise (GSList * opes_selectionnees)
{
    /* on peut maintenant cr�er la table */
    /* pas besoin d'indiquer la hauteur, elle grandit automatiquement */

    if ( GTK_IS_WIDGET(table_etat) )
	gtk_widget_destroy (table_etat);

    if ( GTK_BIN ( scrolled_window_etat ) -> child )
	gtk_widget_hide ( GTK_BIN ( scrolled_window_etat ) -> child );

    table_etat = gtk_table_new ( 0, nb_colonnes, FALSE );
    gtk_table_set_col_spacings ( GTK_TABLE ( table_etat ), 5 );

    return 1;
}
/*****************************************************************************************************/


/*****************************************************************************************************/
gint gtktable_finish ()
{

    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW ( scrolled_window_etat ), table_etat );
    gtk_scrolled_window_set_shadow_type ( GTK_SCROLLED_WINDOW ( scrolled_window_etat ), GTK_SHADOW_NONE );

    gtk_container_set_border_width ( GTK_CONTAINER(table_etat), 6 );
    gtk_widget_show_all ( table_etat );

    if ( GTK_BIN ( scrolled_window_etat ) -> child )
	gtk_widget_show ( GTK_BIN ( scrolled_window_etat ) -> child );

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

    operation = operation_par_no ( operation -> no_operation_ventilee_associee,
				   operation -> no_compte );

    if ( operation )
    {
	/* passage sur le compte concern� */

	changement_compte ( GINT_TO_POINTER ( operation -> no_compte ));

	/* r�cup�ration de la ligne de l'op� dans la liste ; affichage de toutes les op� si n�cessaire */

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte_courant;

	if ( operation -> pointe == 3 && !AFFICHAGE_R )
	    change_aspect_liste ( GINT_TO_POINTER (5));

	selectionne_ligne ( OPERATION_SELECTIONNEE );
    }
}
/*****************************************************************************************************/
