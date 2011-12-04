/* ************************************************************************** */
/*                                                                            */
/*     Copyright (C)    2000-2008 Cédric Auger (cedric@grisbi.org)            */
/*                      2003-2008 Benjamin Drieu (bdrieu@april.org)           */
/*          2008-2011 Pierre Biava (grisbi@pierre.biava.name)                 */
/*          http://www.grisbi.org                                             */
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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n.h>
#include <gtk/gtk.h>


/*START_INCLUDE*/
#include "etats_config_ui.h"
#include "gsb_etats_config.h"
#include "structures.h"
#include "utils.h"
#include "utils_buttons.h"
#include "utils_gtkbuilder.h"
#include "utils_str.h"
#include "erreur.h"
/*END_INCLUDE*/


/*START_STATIC*/
static void etats_config_ui_left_panel_add_line ( GtkTreeStore *tree_model,
                        GtkTreeIter *iter,
                        GtkWidget *notebook,
                        GtkWidget *child,
                        const gchar *title,
                        gint page );
GtkWidget *etats_config_ui_left_panel_create_tree_view ( void );
static void etats_config_ui_left_panel_notebook_change_page ( GtkNotebook *notebook,
                        GtkNotebookPage *npage,
                        gint page,
                        gpointer user_data );
static void etats_config_ui_left_panel_populate_tree_model ( GtkTreeStore *tree_model,
                        GtkWidget *notebook );
static gboolean etats_config_ui_left_panel_tree_view_selectable_func (GtkTreeSelection *selection,
                        GtkTreeModel *model,
                        GtkTreePath *path,
                        gboolean path_currently_selected,
                        gpointer data);
static gboolean etats_config_ui_left_panel_tree_view_selection_changed ( GtkTreeSelection *selection,
                        gpointer data );
static gboolean etats_config_ui_left_panel_tree_view_update_style ( GtkWidget *button,
                        gint *page_number );
static GtkWidget *etats_config_ui_onglet_affichage_devises_create_page ( gint page );
static GtkWidget *etats_config_ui_onglet_affichage_generalites_create_page ( gint page );
static GtkWidget *etats_config_ui_onglet_affichage_operations_create_page ( gint page );
static GtkWidget *etats_config_ui_onglet_affichage_titles_create_page ( gint page );

static GtkWidget *etats_config_ui_onglet_budgets_create_page ( gint page );
static void etats_config_ui_onglet_categ_budget_init_buttons_select_unselect ( gchar *name,
                        GtkWidget *tree_view,
                        gboolean is_categ );
static GtkWidget *etats_config_ui_onglet_categ_budget_tree_view_create ( gboolean is_categ );
static GtkWidget *etats_config_ui_onglet_categories_create_page ( gint page );
static GtkWidget *etats_config_ui_onglet_comptes_create_page ( gint page );
static void etats_config_ui_onglet_comptes_init_buttons_choix_utilisation_virements ( gint page );
static void etats_config_ui_onglet_comptes_init_buttons_selection ( gchar *name,
                        GtkWidget *tree_view );
static void etats_config_ui_onglet_data_grouping_button_clicked ( GtkWidget *button,
                        gpointer data );
static GtkWidget *etats_config_ui_onglet_data_grouping_create_page ( gint page );
static gboolean etats_config_ui_onglet_data_grouping_drag_data_get ( GtkTreeDragSource *drag_source,
                        GtkTreePath *path,
                        GtkSelectionData *selection_data );
static gboolean etats_config_ui_onglet_data_grouping_drag_data_received ( GtkTreeDragDest * drag_dest,
                        GtkTreePath *dest_path,
                        GtkSelectionData *selection_data );
static gboolean etats_config_ui_onglet_data_grouping_drop_possible ( GtkTreeDragDest *drag_dest,
                        GtkTreePath *dest_path,
                        GtkSelectionData *selection_data );
static gboolean etats_config_ui_onglet_data_grouping_init_tree_view ( void );
static void etats_config_ui_onglet_data_grouping_selection_changed ( GtkTreeSelection *selection,
                        GtkWidget *tree_view );
static GtkWidget *etats_config_ui_onglet_data_separation_create_page ( gint page );

static GtkWidget *etats_config_ui_onglet_divers_create_page ( gint page );
static gboolean etats_config_ui_onglet_divers_update_style_left_panel ( GtkWidget *button,
                        gint *page_number );
static GtkWidget *etats_config_ui_onglet_mode_paiement_create_page ( gint page );
static GtkWidget *etats_config_ui_onglet_montants_create_page ( gint page );
static GtkWidget *etats_config_ui_onglet_periode_create_page ( gint page );
static GtkTreeModel *etats_config_ui_onglet_periode_get_liste_dates ( void );
static gboolean etats_config_ui_onglet_periode_selection_dates_changed ( GtkTreeSelection *selection,
                        GtkWidget *widget );
static gboolean etats_config_ui_onglet_periode_update_style_left_panel ( GtkWidget *button,
                        gint *page_number );
static GtkWidget *etats_config_ui_onglet_textes_create_page ( gint page );
static GtkWidget *etats_config_ui_onglet_tiers_create_page ( gint page );
static void etats_config_ui_onglet_tiers_entry_delete_text ( GtkEditable *editable,
                        gint start_pos,
                        gint end_pos,
                        GtkWidget *tree_view );
static void etats_config_ui_onglet_tiers_search_iter_from_entry ( const gchar *text,
                        GtkTreeView *tree_view,
                        gint sens );
static void etats_config_ui_onglet_tiers_entry_insert_text ( GtkEditable *editable,
                        gchar *new_text,
                        gint new_text_length,
                        gpointer position,
                        GtkWidget *tree_view );
static gboolean etats_config_ui_onglet_tiers_select_first_last_item ( GtkWidget *button,
                        GdkEventButton *event,
                        gpointer ptr_sens );
static gboolean etats_config_ui_onglet_tiers_select_prev_next_item ( GtkWidget *button,
                        GdkEventButton *event,
                        gpointer ptr_sens );
static void etats_config_ui_onglet_tiers_selection_changed ( GtkTreeSelection *selection,
                        gpointer user_data );
static gboolean etats_config_ui_onglet_tiers_show_first_row_selected ( GtkWidget *tree_view,
                        GdkEventVisibility  *event,
                        gpointer   user_data);
static void etats_config_ui_onglet_tiers_show_hide_prev_next_buttons ( gint show_left,
                        gint show_right );
static GtkWidget *etats_config_ui_onglet_virements_create_page ( gint page );

static void etats_config_ui_toggle_button_init_button_expand ( gchar *name,
                        GtkWidget *tree_view );

static void etats_config_ui_tree_view_init ( const gchar *treeview_name,
                        GtkTreeModel *(*function) ( void ),
                        GtkSelectionMode type_selection,
                        GCallback selection_callback );
static GtkWidget *etats_config_ui_tree_view_new_with_model ( const gchar *treeview_name,
                        GtkTreeModel *model );


/*END_STATIC*/


/*START_EXTERN*/
/*END_EXTERN*/


/*START_GLOBAL_VARIABLES*/
/* definition of the columns of model for the left panel  */
enum left_panel_tree_columns
{
    LEFT_PANEL_TREE_TEXT_COLUMN,
    LEFT_PANEL_TREE_PAGE_COLUMN,
    LEFT_PANEL_TREE_BOLD_COLUMN,
    LEFT_PANEL_TREE_ITALIC_COLUMN,
    LEFT_PANEL_TREE_NUM_COLUMNS,
};

/* liste des plages de date possibles */
static gchar *etats_config_liste_plages_dates[] =
{
    N_("All"),
    N_("Custom"),
    N_("Total to now"),
    N_("Current month"),
    N_("Current year"),
    N_("Current month to now"),
    N_("Current year to now"),
    N_("Previous month"),
    N_("Previous year"),
    N_("Last 30 days"),
    N_("Last 3 months"),
    N_("Last 6 months"),
    N_("Last 12 months"),
    NULL,
};


static gchar *jours_semaine[] =
{
    N_("Monday"),
    N_("Tuesday"),
    N_("Wednesday"),
    N_("Thursday"),
    N_("Friday"),
    N_("Saturday"),
    N_("Sunday"),
    NULL,
};


static gchar *data_separation_periodes[] =
{
    N_("Day"),
    N_("Week"),
    N_("Month"),
    N_("Year"),
    NULL,
};


/* données de classement des opérations */
static gchar *etats_config_ui_classement_operations[] =
{
    N_("date"),
    N_("value date"),
    N_("transaction number"),
    N_("payee"),
    N_("category"),
    N_("budgetary line"),
    N_("note"),
    N_("method of payment"),
    N_("cheque/transfer number"),
    N_("voucher"),
    N_("bank reference"),
    N_("reconciliation reference"),
    NULL,
};


/* builder */
static GtkBuilder *etat_config_builder = NULL;

/* mémorisation du togglebutton utilisé servira à remplacer son label */
GtkToggleButton *prev_togglebutton = NULL;

/* variables utilisées pour la gestion des tiers*/
GtkTreePath *tiers_selected = NULL;

/* gint last_page */
static gint last_page;

/*END_GLOBAL_VARIABLES*/

/*GENERAL*/
/**
 * Création de la fenêtre de dialog
 *
 * \param
 *
 * \return dialog widget
 */
GtkWidget *etats_config_ui_create_dialog ( void )
{
    GtkWidget *dialog = NULL;
    GtkWidget *tree_view;

    devel_debug (NULL);

    /* Creation d'un nouveau GtkBuilder */
    etat_config_builder = gtk_builder_new ( );

    if ( etat_config_builder == NULL )
        return NULL;

    /* Chargement du XML dans etat_config_builder */
    if ( !utils_gtkbuilder_merge_ui_data_in_builder ( etat_config_builder, "etats_config.ui" ) )
        return NULL;

    /* Recuparation d'un pointeur sur la fenetre. */
    dialog = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "config_etats_dialog" ) );
    gtk_window_set_transient_for ( GTK_WINDOW ( dialog ), GTK_WINDOW ( run.window ) );

    /* Recupération d'un pointeur sur le gtk_tree_view. */
    tree_view = etats_config_ui_left_panel_create_tree_view ( );

    /* return widget */
    return dialog;
}


/**
 * free the gtk_builder
 *
 *
 *
 * */
void etats_config_ui_free_all_var ( void )
{
    g_object_unref ( G_OBJECT ( etat_config_builder ) );
}

/*LEFT_PANEL*/
/**
 * création du tree_view qui liste les onglets de la fenêtre de dialogue
 *
 *
 *\return tree_view or NULL;
 * */
GtkWidget *etats_config_ui_left_panel_create_tree_view ( void )
{
    GtkWidget *tree_view = NULL;
    GtkWidget *notebook;
    GtkTreeStore *model = NULL;
    GtkTreeViewColumn *column;
    GtkCellRenderer *cell;
    GtkTreeSelection *selection;

    devel_debug (NULL);

    /* Création du model */
    model = gtk_tree_store_new ( LEFT_PANEL_TREE_NUM_COLUMNS,
                        G_TYPE_STRING,  /* LEFT_PANEL_TREE_TEXT_COLUMN */
                        G_TYPE_INT,     /* LEFT_PANEL_TREE_PAGE_COLUMN */
                        G_TYPE_INT,     /* LEFT_PANEL_TREE_BOLD_COLUMN */
                        G_TYPE_INT );   /* LEFT_PANEL_TREE_ITALIC_COLUMN */

    /* Create container + TreeView */
    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "treeview_left_panel" ) );
    gtk_tree_view_set_model ( GTK_TREE_VIEW ( tree_view ), GTK_TREE_MODEL ( model ) );
    g_object_unref ( G_OBJECT ( model ) );

    /* set the color of selected row */
    utils_set_tree_view_selection_and_text_color ( tree_view );

    /* make column */
    cell = gtk_cell_renderer_text_new ( );
    column = gtk_tree_view_column_new_with_attributes ( "Categories",
                        cell,
                        "text", LEFT_PANEL_TREE_TEXT_COLUMN,
                        "weight", LEFT_PANEL_TREE_BOLD_COLUMN,
                        "style", LEFT_PANEL_TREE_ITALIC_COLUMN,
                        NULL );
    gtk_tree_view_column_set_sizing ( GTK_TREE_VIEW_COLUMN ( column ), GTK_TREE_VIEW_COLUMN_FIXED );
    gtk_tree_view_append_column ( GTK_TREE_VIEW ( tree_view ), GTK_TREE_VIEW_COLUMN ( column ) );

    /* Handle select */
    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );
    g_signal_connect ( selection,
                        "changed",
                        G_CALLBACK ( etats_config_ui_left_panel_tree_view_selection_changed ),
                        NULL );

    /* Choose which entries will be selectable */
    gtk_tree_selection_set_select_function ( selection,
                        etats_config_ui_left_panel_tree_view_selectable_func, NULL, NULL );

    /* expand all rows after the treeview widget has been realized */
    g_signal_connect ( tree_view,
                        "realize",
                        G_CALLBACK ( utils_tree_view_set_expand_all_and_select_path_realize ),
                        "0:0" );

    /* initialisation du notebook pour les pages de la configuration */
    notebook = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "notebook_config_etat" ) );
    gtk_notebook_set_show_tabs ( GTK_NOTEBOOK ( notebook ), FALSE );
    gtk_notebook_set_show_border ( GTK_NOTEBOOK ( notebook ), FALSE );
    gtk_container_set_border_width ( GTK_CONTAINER ( notebook ), 0 );

    /* remplissage du paned gauche */
    etats_config_ui_left_panel_populate_tree_model ( model, notebook );

    /* on met la connexion pour mémoriser la dernière page utilisée */
    g_signal_connect_after ( notebook,
                        "switch-page",
                        G_CALLBACK ( etats_config_ui_left_panel_notebook_change_page ),
                        NULL );

    /* show all widgets */
    gtk_widget_show_all ( tree_view );


    return tree_view;
}


/**
 * remplit le model pour la configuration des états
 *
 * \param
 * \param
 *
 * \return
 * */
void etats_config_ui_left_panel_populate_tree_model ( GtkTreeStore *tree_model,
                        GtkWidget *notebook )
{
    GtkWidget *widget;
    GtkTreeIter iter;
    gint page = 0;

    /* append group page */
    etats_config_ui_left_panel_add_line ( tree_model, &iter, NULL, NULL, _("Data selection"), -1 );

    /* append page Dates */
    widget = etats_config_ui_onglet_periode_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Dates"), page );
    page++;

    /* append page Transferts */
    widget = etats_config_ui_onglet_virements_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Transfers"), page );
    page++;

    /* append page Accounts */
    widget = etats_config_ui_onglet_comptes_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Accounts"), page );
    page++;

    /* append page Payee */
    widget = etats_config_ui_onglet_tiers_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Payee"), page );
    page++;

    /* append page Categories */
    widget = etats_config_ui_onglet_categories_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Categories"), page );
    page++;

    /* append page Budgetary lines */
    widget = etats_config_ui_onglet_budgets_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Budgetary lines"), page );
    page++;

    /* append page Texts */
    widget = etats_config_ui_onglet_textes_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Texts"), page );
    page++;

    /* append page Amounts */
    widget = etats_config_ui_onglet_montants_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Amounts"), page );
    page++;

    /* append page Payment methods */
    widget = etats_config_ui_onglet_mode_paiement_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Payment methods"), page );
    page++;

    /* append page Misc. */
    widget = etats_config_ui_onglet_divers_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Miscellaneous"), page );
    page++;

    /* remplissage de l'onglet d'organisation */
    etats_config_ui_left_panel_add_line ( tree_model, &iter, NULL, NULL, _("Data organization"), -1 );

    /* Data grouping */
    widget = etats_config_ui_onglet_data_grouping_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Data grouping"), page );
    page++;

    /* Data separation */
    widget = etats_config_ui_onglet_data_separation_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Data separation"), page );
    page++;

    /* remplissage de l'onglet d'affichage */
    etats_config_ui_left_panel_add_line ( tree_model, &iter, NULL, NULL, _("Data display"), -1 );

    /* append page Generalities */
    widget = etats_config_ui_onglet_affichage_generalites_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Generalities"), page );
    page++;

    /* append page divers */
    widget = etats_config_ui_onglet_affichage_titles_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Titles"), page );
    page++;

    /* append page Transactions */
    widget = etats_config_ui_onglet_affichage_operations_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Transactions"), page );
    page++;

    /* append page Currencies */
    widget = etats_config_ui_onglet_affichage_devises_create_page ( page );
    etats_config_ui_left_panel_add_line ( tree_model, &iter, notebook, widget, _("Currencies"), page );

    /* fin de fonction */
}


/**
 *
 *
 * \param
 *
 * \return
 */
gboolean etats_config_ui_left_panel_tree_view_selection_changed ( GtkTreeSelection *selection,
                        gpointer data )
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    gint selected;

    if (! gtk_tree_selection_get_selected ( selection, &model, &iter ) )
        return(FALSE);

    gtk_tree_model_get ( model, &iter, 1, &selected, -1 );

    gtk_notebook_set_current_page ( GTK_NOTEBOOK (
                        gtk_builder_get_object ( etat_config_builder, "notebook_config_etat" ) ),
                        selected );

    /* return */
    return FALSE;
}


/**
 *
 *
 * \param
 *
 * \return
 */
gboolean etats_config_ui_left_panel_tree_view_selectable_func (GtkTreeSelection *selection,
                        GtkTreeModel *model,
                        GtkTreePath *path,
                        gboolean path_currently_selected,
                        gpointer data )
{
    GtkTreeIter iter;
    gint selectable;

    gtk_tree_model_get_iter ( model, &iter, path );
    gtk_tree_model_get ( model, &iter, 1, &selectable, -1 );

    return ( selectable != -1 );
}


/**
 * ajoute une ligne dans le tree_model du panel de gauche
 *
 *
 *
 * */
void etats_config_ui_left_panel_add_line ( GtkTreeStore *tree_model,
                        GtkTreeIter *iter,
                        GtkWidget *notebook,
                        GtkWidget *child,
                        const gchar *title,
                        gint page )
{
    GtkTreeIter iter2;

    if ( page == -1 )
    {
        /* append page groupe */
        gtk_tree_store_append ( GTK_TREE_STORE ( tree_model ), iter, NULL );
        gtk_tree_store_set (GTK_TREE_STORE ( tree_model ), iter,
                        LEFT_PANEL_TREE_TEXT_COLUMN, title,
                        LEFT_PANEL_TREE_PAGE_COLUMN, -1,
                        LEFT_PANEL_TREE_BOLD_COLUMN, 800,
                        -1 );
    }
    else
    {
        /* append page onglet*/
        gtk_notebook_append_page ( GTK_NOTEBOOK ( notebook ),
                        child,
                        gtk_label_new ( title ) );

        gtk_tree_store_append (GTK_TREE_STORE ( tree_model ), &iter2, iter );
        gtk_tree_store_set (GTK_TREE_STORE ( tree_model ), &iter2,
                        LEFT_PANEL_TREE_TEXT_COLUMN, title,
                        LEFT_PANEL_TREE_PAGE_COLUMN, page,
                        LEFT_PANEL_TREE_BOLD_COLUMN, 400,
                        -1);
    }
}


/**
 * If applicable, update report navigation tree style to reflect which
 * pages have been changed.
 *
 * \param page_number Page that contained an interface element just
 *                      changed that triggered this event.
 *
 * \return      FALSE
 */
gboolean etats_config_ui_left_panel_tree_view_update_style ( GtkWidget *button,
                        gint *page_number )
{
    gint iter_page_number;

    iter_page_number = GPOINTER_TO_INT ( page_number );

    if ( iter_page_number )
    {
        GtkWidget *tree_view;
        GtkTreeModel *model;
        GtkTreeIter parent_iter;
        gint active;
        gboolean italic = 0;

        tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "treeview_left_panel" ) );
        model = gtk_tree_view_get_model ( GTK_TREE_VIEW ( tree_view ) );
        active = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( button ) );
        italic = active;

        if ( !gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( model ), &parent_iter ) )
            return FALSE;

        do
        {
            GtkTreeIter iter;

            if ( gtk_tree_model_iter_children ( GTK_TREE_MODEL ( model ), &iter, &parent_iter ) )
            {
                do
                {
                    gint page;

                    gtk_tree_model_get (GTK_TREE_MODEL ( model ),
                                &iter,
                                LEFT_PANEL_TREE_PAGE_COLUMN, &page,
                                -1 );

                    if ( page == iter_page_number )
                        gtk_tree_store_set ( GTK_TREE_STORE ( model ),
                                &iter,
                                LEFT_PANEL_TREE_ITALIC_COLUMN, italic,
                                -1 );
                }
                while ( gtk_tree_model_iter_next ( GTK_TREE_MODEL ( model ), &iter ) );
            }
        }
        while ( gtk_tree_model_iter_next ( GTK_TREE_MODEL ( model ), &parent_iter ) );

        return TRUE;
    }

    /* return */
    return FALSE;
}


/**
 * selectionne une page
 *
 * \param
 *
 * \return
 */
gboolean etats_config_ui_left_panel_tree_view_select_last_page ( void )
{
    GtkWidget *tree_view;
    GtkTreeModel *model;
    GtkTreeIter parent_iter;

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "treeview_left_panel" ) );
    model = gtk_tree_view_get_model ( GTK_TREE_VIEW ( tree_view ) );

    if ( !gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( model ), &parent_iter ) )
        return FALSE;

    do
    {
        GtkTreeIter iter;

        if ( gtk_tree_model_iter_children ( GTK_TREE_MODEL ( model ), &iter, &parent_iter ) )
        {
            do
            {
                gint page;

                gtk_tree_model_get (GTK_TREE_MODEL ( model ),
                                &iter,
                                LEFT_PANEL_TREE_PAGE_COLUMN, &page,
                                -1 );

                if ( page == last_page )
                {
                    GtkTreeSelection *sel;

                    sel = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );
                    gtk_tree_selection_select_iter ( sel, &iter );
                    gtk_notebook_set_current_page ( GTK_NOTEBOOK (
                                gtk_builder_get_object ( etat_config_builder, "notebook_config_etat" ) ),
                                page );
                    break;
                }
            }
            while ( gtk_tree_model_iter_next ( GTK_TREE_MODEL ( model ), &iter ) );
        }
    }
    while ( gtk_tree_model_iter_next ( GTK_TREE_MODEL ( model ), &parent_iter ) );

    /* return */
    return FALSE;
}


void etats_config_ui_left_panel_notebook_change_page ( GtkNotebook *notebook,
                        GtkNotebookPage *npage,
                        gint page,
                        gpointer user_data )
{
    last_page = page;
}
/*RIGHT_PANEL : ONGLET_PERIODE*/
/**
 * Création de la page de détermination de la période de requête
 *
 * \param
 *
 * \return la page
 */
GtkWidget *etats_config_ui_onglet_periode_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkWidget *button;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "onglet_etat_periode" ) );

    vbox = new_vbox_with_title_and_icon ( _("Date selection"), "scheduler.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    /* on traite la partie gauche de l'onglet dates */
    etats_config_ui_tree_view_init ( "treeview_dates",
                        etats_config_ui_onglet_periode_get_liste_dates,
                        GTK_SELECTION_SINGLE,
                        G_CALLBACK ( etats_config_ui_onglet_periode_selection_dates_changed ) );

    gtk_container_set_border_width ( GTK_CONTAINER (
                        gtk_builder_get_object ( etat_config_builder, "vbox_utilisation_date" ) ),
                        10 );

    /* on initialise les entrées pour les dates personnalisées */
    gsb_etats_config_onglet_periode_make_calendar_entry (  );

    etats_config_ui_onglet_periode_date_interval_sensitive ( FALSE );

    /* on traite la partie droite de l'onglet dates */
    etats_config_ui_tree_view_init ( "treeview_exer",
                        gsb_etats_config_onglet_periode_get_model_exercices,
                        GTK_SELECTION_MULTIPLE,
                        NULL );
    gtk_container_set_border_width ( GTK_CONTAINER (
                        gtk_builder_get_object ( etat_config_builder, "vbox_utilisation_exo" ) ),
                        10 );

    button = utils_gtkbuilder_get_widget_by_name ( etat_config_builder,
                        "radio_button_utilise_dates", NULL );
    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_onglet_periode_update_style_left_panel ),
                        GINT_TO_POINTER ( page ) );

    /* on met la connection pour rendre sensitif la frame vbox_utilisation_date */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "vbox_utilisation_date" ) );

    button = utils_gtkbuilder_get_widget_by_name ( etat_config_builder,
                        "radio_button_utilise_exo", NULL );
    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_onglet_periode_update_style_left_panel ),
                        GINT_TO_POINTER ( page ) );

    /* on met la connection pour rendre sensitif la frame vbox_utilisation_exo */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "vbox_utilisation_exo" ) );

    /* on connecte les signaux nécessaires pour gérer la sélection de l'exercice */
    g_signal_connect ( G_OBJECT ( utils_gtkbuilder_get_widget_by_name ( etat_config_builder,
                        "bouton_detaille_exo_etat", NULL ) ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "sw_exer" ) );

    gtk_widget_show_all ( vbox_onglet );

    return vbox_onglet;
}


/**
 *
 *
 * \param
 *
 * \return
 */
GtkTreeModel *etats_config_ui_onglet_periode_get_liste_dates ( void )
{
    GtkListStore *list_store;

    list_store = utils_list_store_create_from_string_array ( etats_config_liste_plages_dates );

    /* return */
    return GTK_TREE_MODEL ( list_store );
}


/**
 *
 *
 * \param
 *
 * \return
 */
gboolean etats_config_ui_onglet_periode_selection_dates_changed ( GtkTreeSelection *selection,
                        GtkWidget *widget )
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    gint selected;

    if ( !gtk_tree_selection_get_selected ( selection, &model, &iter ) )
        return FALSE;

    gtk_tree_model_get ( model, &iter, 1, &selected, -1 );
    etats_config_ui_onglet_periode_date_interval_sensitive ( selected );

    /* on regarde si on utilise des dates personalisées */
    etats_config_ui_onglet_periode_update_style_left_panel ( NULL, 0 );

    return TRUE;
}


/**
 * rend accessible ou nom l'ensemble des données de date configurables
 *
 * \param TRUE rend sensible FALSE rend insensible les données
 *
 * \return
 */
void etats_config_ui_onglet_periode_date_interval_sensitive ( gboolean show )
{
    if ( show > 1 )
        show = 0;

        etats_config_ui_widget_set_sensitive ( "hbox_select_dates", show );
        gtk_widget_set_sensitive ( utils_gtkbuilder_get_widget_by_name ( etat_config_builder,
                        "hbox_date_init", "entree_date_init_etat" ), show );
        gtk_widget_set_sensitive ( utils_gtkbuilder_get_widget_by_name ( etat_config_builder,
                        "hbox_date_finale", "entree_date_finale_etat" ), show );
}


/**
 * If applicable, update report navigation tree style to reflect which
 * pages have been changed.
 *
 * \param page_number Page that contained an interface element just
 *                      changed that triggered this event.
 *
 * \return      FALSE
 */
gboolean etats_config_ui_onglet_periode_update_style_left_panel ( GtkWidget *button,
                        gint *page_number )
{
    GtkWidget *tree_view;
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *path_string;
    gchar *tmp_str;
    gint index;
    gint iter_page_number;

    iter_page_number = GPOINTER_TO_INT ( page_number );

    index = etats_config_ui_buttons_radio_get_active_index ( "radio_button_utilise_exo" );

    if ( index == 0 )
    {
        if ( etats_config_ui_tree_view_get_single_row_selected ( "treeview_dates" ) == 1 )
        index = 1;
    }

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "treeview_left_panel" ) );
    model = gtk_tree_view_get_model ( GTK_TREE_VIEW ( tree_view ) );

    tmp_str = utils_str_itoa ( iter_page_number );
    path_string = g_strconcat ("0:", tmp_str, NULL );

    gtk_tree_model_get_iter_from_string ( GTK_TREE_MODEL ( model ), &iter, path_string );
    gtk_tree_store_set ( GTK_TREE_STORE ( model ),
                        &iter,
                        LEFT_PANEL_TREE_ITALIC_COLUMN, index,
                        -1 );

    g_free ( tmp_str );
    g_free ( path_string );

    /* return */
    return FALSE;
}

/*RIGHT_PANEL : ONGLET_VIREMENTS*/
/**
 * Création de l'onglet virements
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_virements_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkWidget *tree_view;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "onglet_etat_virements" ) );

    vbox = new_vbox_with_title_and_icon ( _("Transfers"), "transfer.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    etats_config_ui_widget_set_sensitive ( "hbox_liste_comptes_virements", FALSE );

    /* on crée la liste des comptes */
    etats_config_ui_tree_view_init ( "treeview_virements",
                        gsb_etats_config_onglet_get_liste_comptes,
                        GTK_SELECTION_MULTIPLE,
                        NULL );

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "treeview_virements" ) );

    /* on initialise les boutons pour sensibiliser bouton_exclure_non_virements_etat et la liste des comptes */
    etats_config_ui_onglet_comptes_init_buttons_choix_utilisation_virements ( page );

    /* on initialise les boutons pour sélectionner tout ou partie des comptes */
    etats_config_ui_onglet_comptes_init_buttons_selection ( "virements", tree_view );

    gtk_widget_show_all ( vbox_onglet );

    /* return */
    return vbox_onglet;
}


/**
 * Initialise les boutons pour sensibiliser bouton_exclure_non_virements_etat
 *  et la liste des comptes.
 *
 * \param
 *
 * \return
 */
void etats_config_ui_onglet_comptes_init_buttons_choix_utilisation_virements ( gint page )
{
    GtkWidget *button;

    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder,
                        "bouton_inclusion_virements_actifs_etat" ) );
    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_left_panel_tree_view_update_style ),
                        GINT_TO_POINTER ( page ) );
    /* on connecte le signal pour gérer la sensibilité du bouton bouton_bouton_inclusion_virements_actifs_etat */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_exclure_non_virements_etat" ) );

    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder,
                        "bouton_inclusion_virements_hors_etat" ) );
    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_left_panel_tree_view_update_style ),
                        GINT_TO_POINTER ( page ) );
    /* on connecte le signal pour gérer la sensibilité du bouton_inclusion_virements_hors_etat */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_exclure_non_virements_etat" ) );

    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder,
                        "bouton_inclusion_virements_perso" ) );
    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_left_panel_tree_view_update_style ),
                        GINT_TO_POINTER ( page ) );
    /* on connecte le signal pour gérer la sensibilité du bouton_inclusion_virements_perso */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_exclure_non_virements_etat" ) );

    /* on connecte le signal pour gérer l'affichage de la liste des comptes */
    g_signal_connect ( G_OBJECT ( gtk_builder_get_object ( etat_config_builder,
                        "bouton_inclusion_virements_perso" ) ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "hbox_liste_comptes_virements" ) );
}


/*RIGHT_PANEL : ONGLET_COMPTES*/
/**
 * Création de l'onglet comptes
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_comptes_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkWidget *button;
    GtkWidget *tree_view;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "onglet_etat_comptes" ) );

    vbox = new_vbox_with_title_and_icon ( _("Account selection"), "ac_bank.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    etats_config_ui_widget_set_sensitive ( "vbox_generale_comptes_etat", FALSE );

    /* on crée la liste des comptes */
    etats_config_ui_tree_view_init ( "treeview_comptes",
                        gsb_etats_config_onglet_get_liste_comptes,
                        GTK_SELECTION_MULTIPLE,
                        NULL );

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "treeview_comptes" ) );

    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "bouton_detaille_comptes_etat" ) );

    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_left_panel_tree_view_update_style ),
                        GINT_TO_POINTER ( page ) );

    /* on met la connection pour rendre sensitif la vbox_generale_comptes_etat */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "vbox_generale_comptes_etat" ) );

    /* on met la connection pour sélectionner une partie des comptes */
    etats_config_ui_onglet_comptes_init_buttons_selection ( "comptes", tree_view );

    gtk_widget_show_all ( vbox_onglet );

    /* return */
    return vbox_onglet;
}


/**
 * Initialise les boutons de sélection des comptes
 *
 * \param suffixe du bouton
 * \param tree_view
 *
 * \return
 */
void etats_config_ui_onglet_comptes_init_buttons_selection ( gchar *name,
                        GtkWidget *tree_view )
{
    GtkWidget *button;
    gchar *tmp_str;

    /* bouton select unselect all*/
    tmp_str = g_strconcat ( "togglebutton_select_all_", name, NULL );
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, tmp_str ) );
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( button ), FALSE );
    g_object_set_data ( G_OBJECT ( button ), "type_compte", GINT_TO_POINTER ( -9 ) );
    g_free ( tmp_str );

    tmp_str = g_strconcat ( "hbox_toggle_select_all_", name, NULL );
    g_object_set_data ( G_OBJECT ( button ), "hbox_select_all",
                        gtk_builder_get_object ( etat_config_builder, tmp_str ) );
    g_free ( tmp_str );

    tmp_str = g_strconcat ( "hbox_toggle_unselect_all_", name, NULL );
    g_object_set_data ( G_OBJECT ( button ), "hbox_unselect_all",
                        gtk_builder_get_object ( etat_config_builder, tmp_str ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_onglet_comptes_select_unselect ),
                        tree_view );

    /* bouton_bank */
    tmp_str = g_strconcat ( "button_bank_", name, NULL );
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, tmp_str ) );
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( button ), FALSE );
    g_object_set_data ( G_OBJECT ( button ), "type_compte", GINT_TO_POINTER ( 0 ) );
    g_signal_connect ( G_OBJECT  ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_onglet_comptes_select_unselect ),
                        tree_view );
    g_free ( tmp_str );

    /* bouton_cash */
    tmp_str = g_strconcat ( "button_cash_", name, NULL );
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, tmp_str ) );
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( button ), FALSE );
    g_object_set_data ( G_OBJECT ( button ), "type_compte", GINT_TO_POINTER ( 1 ) );
    g_signal_connect ( G_OBJECT  ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_onglet_comptes_select_unselect ),
                        tree_view );
    g_free ( tmp_str );

    /* button_liabilities */
    tmp_str = g_strconcat ( "button_liabilities_", name, NULL );
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, tmp_str ) );
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( button ), FALSE );
    g_object_set_data ( G_OBJECT ( button ), "type_compte", GINT_TO_POINTER ( 2 ) );
    g_signal_connect ( G_OBJECT  ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_onglet_comptes_select_unselect ),
                        tree_view );
    g_free ( tmp_str );

    /* "button_assets */
    tmp_str = g_strconcat ( "button_assets_", name, NULL );
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, tmp_str ) );
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( button ), FALSE );
    g_object_set_data ( G_OBJECT ( button ), "type_compte", GINT_TO_POINTER ( 3 ) );
    g_signal_connect ( G_OBJECT  ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_onglet_comptes_select_unselect ),
                        tree_view );
    g_free ( tmp_str );
}


/**
 * select or unselect all the rows of treeview
 *
 * \param toggle_button
 * \param tree_view
 *
 * \return
 * */
void etats_config_ui_onglet_comptes_select_unselect ( GtkToggleButton *togglebutton,
                        GtkWidget *tree_view )
{
    GtkTreeSelection *selection;
    gboolean toggle;
    gint type_compte;

    type_compte = GPOINTER_TO_INT ( g_object_get_data ( G_OBJECT ( togglebutton ), "type_compte" ) );
    toggle = gtk_toggle_button_get_active ( togglebutton );
    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );

    /* on mémorise l'état du bouton après changement d'état du bouton précédent */
    if ( prev_togglebutton && prev_togglebutton != togglebutton )
        utils_togglebutton_change_label_select_unselect ( GTK_TOGGLE_BUTTON ( prev_togglebutton ), FALSE );
    else
        prev_togglebutton = NULL;
    prev_togglebutton = togglebutton;

    switch ( type_compte )
    {
    case -9:
        {
            utils_togglebutton_select_unselect_all_rows ( togglebutton, tree_view );
            break;
        }
    default:
        {
            gtk_tree_selection_unselect_all ( selection );
            if ( toggle )
                gsb_etats_config_onglet_select_partie_liste_comptes ( tree_view, type_compte );
            utils_togglebutton_change_label_select_unselect ( GTK_TOGGLE_BUTTON ( togglebutton ), toggle );
        }
    }
}


/*RIGHT_PANEL : ONGLET_TIERS*/
/**
 * Création de l'onglet Tiers
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_tiers_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkWidget *tree_view;
    GtkWidget *entry;
    GtkWidget *button;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "onglet_etat_tiers" ) );

    vbox = new_vbox_with_title_and_icon ( _("Payees"), "payees.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    etats_config_ui_widget_set_sensitive ( "vbox_detaille_tiers_etat", FALSE );

    /* on crée la liste des tiers */
    etats_config_ui_tree_view_init ( "treeview_tiers",
                        gsb_etats_config_onglet_get_liste_tiers,
                        GTK_SELECTION_MULTIPLE,
                        NULL );

    /* on ajoute un callback pour aller au premier item sélectionné */
    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "treeview_tiers" ) );
    g_signal_connect ( G_OBJECT ( tree_view ),
                        "visibility-notify-event",
                        G_CALLBACK ( etats_config_ui_onglet_tiers_show_first_row_selected ),
                        NULL );
    /* on supprime la recherche intégrée */
    gtk_tree_view_set_enable_search ( GTK_TREE_VIEW ( tree_view ), FALSE );

    /* on rend les boutons premier, précédent, suivant et dernier actifs */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "button_tiers_premier" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "button-press-event",
                        G_CALLBACK ( etats_config_ui_onglet_tiers_select_first_last_item ),
                        GINT_TO_POINTER ( GDK_LEFTBUTTON ) );

    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "button_tiers_precedent" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "button-press-event",
                        G_CALLBACK ( etats_config_ui_onglet_tiers_select_prev_next_item ),
                        GINT_TO_POINTER ( GDK_LEFTBUTTON ) );

    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "button_tiers_suivant" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "button-press-event",
                        G_CALLBACK ( etats_config_ui_onglet_tiers_select_prev_next_item ),
                        GINT_TO_POINTER ( GDK_RIGHTBUTTON ) );

    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "button_tiers_dernier" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "button-press-event",
                        G_CALLBACK ( etats_config_ui_onglet_tiers_select_first_last_item ),
                        GINT_TO_POINTER ( GDK_RIGHTBUTTON ) );

    /* on rend insensible les bouton premier et précédent car on est positionné sur le 1er item sélectionné */
    etats_config_ui_onglet_tiers_show_hide_prev_next_buttons ( FALSE, -1 );

    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "bouton_detaille_tiers_etat" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_left_panel_tree_view_update_style ),
                        GINT_TO_POINTER ( page ) );
    /* on met la connection pour rendre sensitif la vbox_generale_comptes_etat */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "vbox_detaille_tiers_etat" ) );

    /* on met la connection pour (dé)sélectionner tous les tiers */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "togglebutton_select_all_tiers" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( utils_togglebutton_select_unselect_all_rows ),
                        tree_view );

    entry = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "search_entry_tiers" ) );
    g_signal_connect ( G_OBJECT ( entry ),
                        "insert-text",
                        G_CALLBACK ( etats_config_ui_onglet_tiers_entry_insert_text ),
                        tree_view );
    g_signal_connect ( G_OBJECT ( entry ),
                        "delete-text",
                        G_CALLBACK ( etats_config_ui_onglet_tiers_entry_delete_text ),
                        tree_view );

    gtk_widget_show_all ( vbox_onglet );

    /* return */
    return vbox_onglet;
}


/**
 * Gestion des flêches de déplacement des tiers sélextionnés
 *
 * \param button
 * \param event
 * \param un pointeur donnant le sens de déplacement GDK_LEFTBUTTON et GDK_RIGHTBUTTON
 *
 * \return
 */
gboolean etats_config_ui_onglet_tiers_select_prev_next_item ( GtkWidget *button,
                        GdkEventButton *event,
                        gpointer ptr_sens )
{
    GtkWidget *tree_view;
    GtkTreeSelection *selection;
    GtkTreePath *start_path;
    GtkTreePath *end_path;
    GtkTreePath *first_path;
    GtkTreePath *last_path;
    GtkTreePath *path = NULL;
    GList *liste;
    gint sens;
    gint nbre_selections;
    gboolean find = FALSE;
    gboolean return_value = FALSE;

    sens = GPOINTER_TO_INT ( ptr_sens );

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "treeview_tiers" ) );
    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );

    liste = gtk_tree_selection_get_selected_rows ( selection, NULL );
    if ( liste == NULL )
    {
        etats_config_ui_onglet_tiers_show_hide_prev_next_buttons ( FALSE, FALSE );
        return FALSE;
    }
    nbre_selections = g_list_length (liste );

    first_path = g_list_nth_data ( liste, 0 );
    last_path = g_list_nth_data ( liste, nbre_selections - 1 );

    liste = g_list_find_custom ( liste, tiers_selected, ( GCompareFunc ) gtk_tree_path_compare );

    if ( gtk_tree_view_get_visible_range ( GTK_TREE_VIEW ( tree_view ), &start_path, &end_path ) )
    {
        while ( liste )
        {
            path = ( GtkTreePath * ) liste->data;

            if ( gtk_tree_selection_path_is_selected ( selection, path ) )
            {
                if ( sens == GDK_LEFTBUTTON )
                {
                    if ( gtk_tree_path_compare ( start_path, path ) <= 0 )
                    {
                        liste = liste->prev;
                        continue;
                    }
                    else if ( gtk_tree_path_compare ( path, tiers_selected ) != 0 )
                    {
                        gtk_tree_view_scroll_to_cell ( GTK_TREE_VIEW ( tree_view ), path, NULL, FALSE, 0., 0. );
                        tiers_selected = path;
                        find = TRUE;
                        break;
                    }
                }
                else
                {
                    if ( gtk_tree_path_compare ( path, end_path ) <= 0 )
                    {
                        liste = liste->next;
                        continue;
                    }
                    else if ( gtk_tree_path_compare ( path, tiers_selected ) != 0 )
                    {
                        gtk_tree_view_scroll_to_cell ( GTK_TREE_VIEW ( tree_view ), path, NULL, FALSE, 0., 0. );
                        tiers_selected = path;
                        find = TRUE;
                        break;
                    }
                }
            }

            if ( sens == GDK_LEFTBUTTON )
                liste = liste->prev;
            else
                liste = liste->next;
        }

        gtk_tree_path_free ( start_path );
        gtk_tree_path_free ( end_path );

        if ( find )
            return_value = TRUE;
    }

    if ( !find )
    {
        liste = gtk_tree_selection_get_selected_rows ( selection, NULL );
        if ( liste == NULL )
            return FALSE;

        if ( sens == GDK_LEFTBUTTON )
            liste = g_list_first ( liste );
        else
            liste = g_list_last ( liste );

        path = ( GtkTreePath * ) liste->data;
        gtk_tree_view_scroll_to_cell ( GTK_TREE_VIEW ( tree_view ), path, NULL, FALSE, 0., 0. );
        tiers_selected = path;

        return_value = TRUE;
    }

    if ( gtk_tree_path_compare ( path, first_path ) == 0 )
        etats_config_ui_onglet_tiers_show_hide_prev_next_buttons ( FALSE, -1 );
    else
        etats_config_ui_onglet_tiers_show_hide_prev_next_buttons ( TRUE, -1 );

    if ( gtk_tree_path_compare ( path, last_path ) == 0 )
        etats_config_ui_onglet_tiers_show_hide_prev_next_buttons ( -1, FALSE );
    else
        etats_config_ui_onglet_tiers_show_hide_prev_next_buttons ( -1, TRUE );

    /* return */
    return return_value;
}


/**
 * Gestion des flèches de déplacement des tiers sélextionnés
 *
 * \param button
 * \param event
 * \param un pointeur donnant le sens de déplacement GDK_LEFTBUTTON et GDK_RIGHTBUTTON
 *
 * \return
 */
gboolean etats_config_ui_onglet_tiers_select_first_last_item ( GtkWidget *button,
                        GdkEventButton *event,
                        gpointer ptr_sens )
{
    GtkWidget *tree_view;
    GtkTreeSelection *selection;
    GtkTreePath *first_path;
    GtkTreePath *last_path;
    GList *liste;
    gint sens;
    gint nbre_selections;

    sens = GPOINTER_TO_INT ( ptr_sens );

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "treeview_tiers" ) );
    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );

    liste = gtk_tree_selection_get_selected_rows ( selection, NULL );
    if ( liste == NULL )
    {
        etats_config_ui_onglet_tiers_show_hide_prev_next_buttons ( FALSE, FALSE );
        return FALSE;
    }
    nbre_selections = g_list_length (liste );

    first_path = g_list_nth_data ( liste, 0 );
    last_path = g_list_nth_data ( liste, nbre_selections - 1 );

    if ( sens == GDK_LEFTBUTTON )
    {
        gtk_tree_view_scroll_to_cell ( GTK_TREE_VIEW ( tree_view ), first_path, NULL, FALSE, 0., 0. );
        tiers_selected = first_path;
        etats_config_ui_onglet_tiers_show_hide_prev_next_buttons ( FALSE, TRUE );
    }
    else
    {
        gtk_tree_view_scroll_to_cell ( GTK_TREE_VIEW ( tree_view ), last_path, NULL, FALSE, 0., 0. );
        tiers_selected = last_path;
        etats_config_ui_onglet_tiers_show_hide_prev_next_buttons ( TRUE, FALSE );
    }

    return TRUE;
}
/**
 * positionne le tree_view sur le premier tiers sélectionné
 *
 * \param tree_view
 * \param event
 * \param user_data = NULL
 *
 * \return
 */
gboolean etats_config_ui_onglet_tiers_show_first_row_selected ( GtkWidget *tree_view,
                        GdkEventVisibility  *event,
                        gpointer user_data)
{
    GtkTreeSelection *selection;
    GtkTreePath *start_path;
    GtkTreePath *end_path;
    GList *liste;

    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );
    liste = gtk_tree_selection_get_selected_rows ( selection, NULL );
    if ( liste )
        tiers_selected = ( GtkTreePath * ) liste->data;
    else
    {
        /* on ajoute un callback pour gérer le changement de sélection */
        g_signal_connect ( G_OBJECT ( selection ),
                        "changed",
                        G_CALLBACK ( etats_config_ui_onglet_tiers_selection_changed ),
                        NULL );

        return FALSE;
    }

    if ( gtk_tree_view_get_visible_range ( GTK_TREE_VIEW ( tree_view ), &start_path, &end_path ) )
    {
        if ( tiers_selected && gtk_tree_path_compare ( tiers_selected, end_path ) == 1 )
            gtk_tree_view_scroll_to_cell ( GTK_TREE_VIEW ( tree_view ), tiers_selected, NULL, FALSE, 0., 0. );

        gtk_tree_path_free ( start_path );
        gtk_tree_path_free ( end_path );
    }
    else if ( tiers_selected )
        gtk_tree_view_scroll_to_cell ( GTK_TREE_VIEW ( tree_view ), tiers_selected, NULL, FALSE, 0., 0. );

    /* on ajoute un callback pour gérer le changement de sélection */
    g_signal_connect ( G_OBJECT ( selection ),
                        "changed",
                        G_CALLBACK ( etats_config_ui_onglet_tiers_selection_changed ),
                        NULL );

    return TRUE;
}


/**
 * fonction de callback de changement de sélection
 *
 * \param selection
 * \param NULL
 *
 * \return
 */
void etats_config_ui_onglet_tiers_selection_changed ( GtkTreeSelection *selection,
                        gpointer user_data )
{
    GtkTreeView *tree_view;
    GtkTreePath *start_path;
    GtkTreePath *end_path;
    GtkTreePath *path = NULL;
    GList *liste;

    tree_view = gtk_tree_selection_get_tree_view ( selection );

    /* on récupère la liste des libnes sélectionnées */
    liste = gtk_tree_selection_get_selected_rows ( selection, NULL );

    /* on change la sensibilité des boutons de navigation si nécessaire */
    if ( g_list_length ( liste ) > 1 )
        etats_config_ui_onglet_tiers_show_hide_prev_next_buttons ( TRUE, TRUE );
    else
        etats_config_ui_onglet_tiers_show_hide_prev_next_buttons ( FALSE, FALSE );

    /* on positionne le tree_view sur la ligne sélectionnée visible la plus proche */
    if ( gtk_tree_view_get_visible_range ( GTK_TREE_VIEW ( tree_view ), &start_path, &end_path ) )
    {
        while ( liste )
        {
            path = ( GtkTreePath * ) liste->data;

            if ( gtk_tree_selection_path_is_selected ( selection, path ) )
            {
                if ( gtk_tree_path_compare ( start_path, path ) <= 0
                 &&
                 gtk_tree_path_compare ( path, end_path ) <= 0 )
                {
                    if ( gtk_tree_path_compare ( path, tiers_selected ) == 0 )
                    {
                        liste = liste->next;
                        continue;
                    }
                    tiers_selected = path;
                    break;
                }
            }
            liste = liste->next;
        }

        /* free the path */
        gtk_tree_path_free ( start_path );
        gtk_tree_path_free ( end_path );
    }
}


/**
 * gère la sensibilité des boutons premier,précédent, suivant et dernier pour les tiers
 *
 * \param show left buttons 0 = insensitif 1 = sensitif -1 = sans changement
 * \param show right buttons 0 = insensitif 1 = sensitif -1 = sans changement
 *
 * \return
 */
void etats_config_ui_onglet_tiers_show_hide_prev_next_buttons ( gint show_left,
                        gint show_right )
{
    if ( show_left >= 0 )
    {
        etats_config_ui_widget_set_sensitive ( "button_tiers_premier", show_left );
        etats_config_ui_widget_set_sensitive ( "button_tiers_precedent", show_left );
    }
    if ( show_right >= 0 )
    {
        etats_config_ui_widget_set_sensitive ( "button_tiers_suivant", show_right );
        etats_config_ui_widget_set_sensitive ( "button_tiers_dernier", show_right );
    }
}


/**
 * Fonction de CALLBACK pour la recherche de tiers
 *
 * \param editable
 * \param new_test
 * \param longueur ajoutée
 * \param position de l'ajout
 * \param tree_view pour la recherche
 *
 * \return
 */
void etats_config_ui_onglet_tiers_entry_insert_text ( GtkEditable *editable,
                        gchar *new_text,
                        gint new_text_length,
                        gpointer position,
                        GtkWidget *tree_view )
{
    gchar *text;

    /* on bloque l'appel de la fonction */
    g_signal_handlers_block_by_func ( G_OBJECT ( editable ),
                        G_CALLBACK ( etats_config_ui_onglet_tiers_entry_insert_text ),
                        tree_view );

    gtk_editable_insert_text ( editable, new_text, new_text_length, position );

    /* on lance la recherche de la chaine */
    text = gtk_editable_get_chars ( editable, 0, -1 );
    etats_config_ui_onglet_tiers_search_iter_from_entry ( text, GTK_TREE_VIEW ( tree_view ), GDK_RIGHTBUTTON );

    /* on débloque l'appel de la fonction */
    g_signal_handlers_unblock_by_func ( G_OBJECT ( editable ),
                        G_CALLBACK ( etats_config_ui_onglet_tiers_entry_insert_text ),
                        tree_view );
    /* evite d'écrire en double dans l'entry */
    g_signal_stop_emission_by_name ( editable, "insert_text" );
}

/**
 * Fonction de CALLBACK pour la recherche de tiers
 *
 * \param editable
 * \param début du caractère supprimé
 * \param fin du caractère supprimé
 * \param tree_view pour la recherche
 *
 * \return
 */
void etats_config_ui_onglet_tiers_entry_delete_text ( GtkEditable *editable,
                        gint start_pos,
                        gint end_pos,
                        GtkWidget *tree_view )
{
    gchar *text;

    text = gtk_editable_get_chars ( editable, 0, start_pos );
    etats_config_ui_onglet_tiers_search_iter_from_entry ( text, GTK_TREE_VIEW ( tree_view ), GDK_LEFTBUTTON );
}


/**
 * Fonction de de recherche de tiers
 *
 * \param text à rechercher
 * \param tree_view pour la recherche
 *
 * \return
 */
void etats_config_ui_onglet_tiers_search_iter_from_entry ( const gchar *text,
                        GtkTreeView *tree_view,
                        gint sens)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreePath *path;
    gint index = 1;
    gint longueur;

    if ( !text || strlen ( text ) == 0 )
        return;

    model = gtk_tree_view_get_model ( tree_view );
    path = gtk_tree_path_new_first ( );

    if ( !gtk_tree_model_get_iter ( model, &iter, path ) )
        return;

    do
    {
        gchar *str;
        gchar *tmp_str;

        gtk_tree_model_get ( model, &iter, 0, &str, -1 );

        longueur = g_utf8_strlen ( text, -1 );
        tmp_str = g_strndup ( str, longueur );

        if ( strcmp (  g_utf8_casefold ( tmp_str, -1 ),  g_utf8_casefold ( text, -1 ) ) == 0 )
        {
            gtk_tree_view_scroll_to_cell ( GTK_TREE_VIEW ( tree_view ), path, NULL, TRUE, 0.0, 0.0 );
            break;
        }
        if ( path )
        {
            gtk_tree_path_next ( path );
            if ( !gtk_tree_model_get_iter ( model, &iter, path ) )
                index--;
        }
    }
    /*termine la boucle si la lettre n'existe pas */
    while ( index );
}


/*RIGHT_PANEL : ONGLET_CATEGORIES*/
/**
 * Création de l'onglet Catégories
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_categories_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkWidget *tree_view;
    GtkWidget *button;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "onglet_etat_categories" ) );

    vbox = new_vbox_with_title_and_icon ( _("Categories"), "categories.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    etats_config_ui_widget_set_sensitive ( "vbox_detaille_categ_etat", FALSE );

    /* on crée la liste des catégories */
    tree_view = etats_config_ui_onglet_categ_budget_tree_view_create ( TRUE );

    /* on met la connection pour rendre sensitif la vbox_detaille_categ_etat */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "bouton_detaille_categ_etat" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "vbox_detaille_categ_etat" ) );

    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_left_panel_tree_view_update_style ),
                        GINT_TO_POINTER ( page ) );

    /* on met la connection pour déplier replier les catégories */
    etats_config_ui_toggle_button_init_button_expand ( "categ", tree_view );

    /* on met la connection pour (dé)sélectionner tout ou partie des catégories */
    etats_config_ui_onglet_categ_budget_init_buttons_select_unselect ( "categ", tree_view, TRUE );

    gtk_widget_show_all ( vbox_onglet );

    return vbox_onglet;
}


/*RIGHT_PANEL : ONGLET_BUDGETS*/
/**
 * Création de l'onglet Budgets
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_budgets_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkWidget *button;
    GtkWidget *tree_view;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "onglet_etat_ib" ) );

    vbox = new_vbox_with_title_and_icon ( _("Budgetary lines"), "budgetary_lines.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    etats_config_ui_widget_set_sensitive ( "vbox_detaille_budget_etat", FALSE );

    /* on crée la liste des IB */
    tree_view = etats_config_ui_onglet_categ_budget_tree_view_create ( FALSE );

    /* on met la connection pour rendre sensitif la hbox_detaille_budget_etat */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "bouton_detaille_budget_etat" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        utils_gtkbuilder_get_widget_by_name ( etat_config_builder, "vbox_detaille_budget_etat", NULL ) );

    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_left_panel_tree_view_update_style ),
                        GINT_TO_POINTER ( page ) );

    /* on met la connection pour déplier replier les IB */
    etats_config_ui_toggle_button_init_button_expand ( "budget", tree_view );

    /* on met la connection pour sélectionner tout ou partie des IB */
    etats_config_ui_onglet_categ_budget_init_buttons_select_unselect ( "budget", tree_view, FALSE );

    gtk_widget_show_all ( vbox_onglet );

    /* return */
    return vbox_onglet;
}


/**
 * create the category/budget list for reports
 * it's a tree with categories/budget and sub-categories/budgets,
 * and a check-button to select them
 *
 * \param
 *
 * \return a GtkWidget : the GtkTreeView
 * */
GtkWidget *etats_config_ui_onglet_categ_budget_tree_view_create ( gboolean is_categ )
{
    GtkWidget *tree_view;
    GtkTreeModel *model;
    GtkTreeViewColumn *column;
    GtkCellRenderer *cell_renderer;
    GtkCellRenderer *radio_renderer;
    gchar *treeview_name;

    /* get the model */
    model = gsb_etats_config_onglet_categ_budget_get_model ( is_categ );

    if ( is_categ )
        treeview_name = g_strdup ( "treeview_categ" );
    else
        treeview_name = g_strdup ( "treeview_budget" );

    /* create the tree view */
    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, treeview_name ) );
    gtk_tree_view_set_headers_visible ( GTK_TREE_VIEW ( tree_view ), FALSE );

    /* set the color of selected row */
    utils_set_tree_view_selection_and_text_color ( tree_view );

    if ( model )
    {
        gtk_tree_view_set_model ( GTK_TREE_VIEW ( tree_view ), model );
        g_object_unref ( G_OBJECT ( model ) );
    }

    /* create the column */
    column = gtk_tree_view_column_new ();

    /* create the toggle button part */
    radio_renderer = gtk_cell_renderer_toggle_new ();
    g_object_set ( G_OBJECT ( radio_renderer ), "xalign", 0.0, NULL );

    gtk_tree_view_column_pack_start ( column,
                        radio_renderer,
                        FALSE );
    gtk_tree_view_column_set_attributes ( column,
                        radio_renderer,
                        "active", 1,
                        "activatable", 2,
                        NULL);
    g_signal_connect ( G_OBJECT ( radio_renderer ),
                        "toggled",
                        G_CALLBACK ( gsb_etats_config_onglet_categ_budget_toggled ),
                        model );

    /* create the text part */
    cell_renderer = gtk_cell_renderer_text_new ( );
    g_object_set ( G_OBJECT ( cell_renderer ),
                        "xalign", 0.0,
                        NULL );
    gtk_tree_view_column_pack_start ( column,
                        cell_renderer,
                        TRUE );
    gtk_tree_view_column_set_attributes (column,
                        cell_renderer,
                        "text", 0,
                        NULL );

    gtk_tree_view_append_column ( GTK_TREE_VIEW ( tree_view ), column );

    g_free ( treeview_name );

    return tree_view;
}


/**
 * initialisation des boutons de sélection des catégories
 *
 * \param suffixe name of widgets
 * \param tree_view
 *
 * \return
 * */
void etats_config_ui_onglet_categ_budget_init_buttons_select_unselect ( gchar *name,
                        GtkWidget *tree_view,
                        gboolean is_categ )
{
    GtkWidget *button;
    gchar *tmp_str;

    tmp_str = g_strconcat ( "togglebutton_select_all_", name, NULL );
    button = utils_gtkbuilder_get_widget_by_name ( etat_config_builder, tmp_str, NULL );
    g_signal_connect ( G_OBJECT  ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_onglet_categ_budget_check_uncheck_all ),
                        tree_view );
    g_free ( tmp_str );

    tmp_str = g_strconcat ( "button_income_", name, NULL );
    button = utils_gtkbuilder_get_widget_by_name ( etat_config_builder, tmp_str, NULL );
    g_object_set_data ( G_OBJECT ( button ), "is_categ", GINT_TO_POINTER ( is_categ ) );
    g_object_set_data ( G_OBJECT ( button ), "type_div", GINT_TO_POINTER ( FALSE ) );
    g_signal_connect ( G_OBJECT  ( button ),
                        "button-press-event",
                        G_CALLBACK ( gsb_etats_config_onglet_categ_budget_select_partie_liste ),
                        tree_view );
    g_free ( tmp_str );

    tmp_str = g_strconcat ( "button_outgoing_", name, NULL );
    button = utils_gtkbuilder_get_widget_by_name ( etat_config_builder, tmp_str, NULL );
    g_object_set_data ( G_OBJECT ( button ), "is_categ", GINT_TO_POINTER ( is_categ ) );
    g_object_set_data ( G_OBJECT ( button ), "type_div", GINT_TO_POINTER ( TRUE ) );
    g_signal_connect ( G_OBJECT  ( button ),
                        "button-press-event",
                        G_CALLBACK ( gsb_etats_config_onglet_categ_budget_select_partie_liste ),
                        tree_view );
    g_free ( tmp_str );
}


/**
 * coche ou décoche toutes les cases du tree_view
 *
 * \param toggle_button
 * \param tree_view
 *
 * \return
 * */
void etats_config_ui_onglet_categ_budget_check_uncheck_all ( GtkToggleButton *togglebutton,
                        GtkWidget *tree_view )
{
    GtkTreeModel *model;
    gchar *label;
    gboolean toggle;

    toggle = gtk_toggle_button_get_active ( togglebutton );
    model = gtk_tree_view_get_model ( GTK_TREE_VIEW ( tree_view ) );

    gsb_etats_config_onglet_categ_budget_tree_model_check_uncheck_all ( model, toggle );

    if ( ( toggle ) )
        label = g_strdup ( _("Unselect all") );
    else
        label = g_strdup ( _("Select all") );

    gtk_button_set_label ( GTK_BUTTON ( togglebutton ), label );
}


/*RIGHT_PANEL : ONGLET_TEXTES*/
/**
 * Création de l'onglet recherche de textes
 *
 * \param gint page
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_textes_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkWidget *button;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "onglet_etat_texte" ) );

    vbox = new_vbox_with_title_and_icon ( _("Transaction content"), "text.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "bouton_utilise_texte" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_left_panel_tree_view_update_style ),
                        GINT_TO_POINTER ( page ) );

    /* on met la connection pour rendre sensitif la vbox_generale_textes_etat */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "vbox_generale_textes_etat" ) );

    gtk_widget_show_all ( vbox_onglet );

    /* on retourne la vbox */
    return vbox_onglet;
}


/*RIGHT_PANEL : ONGLET_MONTANTS*/
/**
 * Création de l'onglet recherche de montants
 *
 * \param gint page
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_montants_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkWidget *button;

    devel_debug (NULL);

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "onglet_etat_montant" ) );

    vbox = new_vbox_with_title_and_icon ( _("Amount"), "amount.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "bouton_utilise_montant" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_left_panel_tree_view_update_style ),
                        GINT_TO_POINTER ( page ) );

    /* on met la connection pour rendre sensitif la vbox_generale_textes_etat */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "vbox_generale_montants_etat" ) );

    gtk_widget_show_all ( vbox_onglet );

    /* on retourne la vbox */
    return vbox_onglet;
}


/*RIGHT_PANEL : ONGLET_MODE_PAIEMENT*/
/**
 * Création de l'onglet moyens de paiement
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_mode_paiement_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkWidget *tree_view;
    GtkWidget *button;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "onglet_etat_mode_paiement" ) );

    vbox = new_vbox_with_title_and_icon ( _("Payment methods"), "payment.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    etats_config_ui_widget_set_sensitive ( "vbox_mode_paiement_etat", FALSE );

    /* on crée la liste des moyens de paiement */
    etats_config_ui_tree_view_init ( "treeview_mode_paiement",
                        gsb_etats_config_onglet_mode_paiement_get_model,
                        GTK_SELECTION_MULTIPLE,
                        NULL );

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "treeview_mode_paiement" ) );

    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "bouton_detaille_mode_paiement_etat" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_left_panel_tree_view_update_style ),
                        GINT_TO_POINTER ( page ) );

    /* on met la connection pour rendre sensitif la vbox_generale_comptes_etat */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "vbox_mode_paiement_etat" ) );

    /* on met la connection pour (dé)sélectionner tous les tiers */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder,
                        "togglebutton_select_all_mode_paiement" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( utils_togglebutton_select_unselect_all_rows ),
                        tree_view );

    gtk_widget_show_all ( vbox_onglet );

    return vbox_onglet;
}


/**
 * Sélectionne les iters en fonction des données de la liste
 *
 * \param liste des lignes à sélectionner
 * \param nom du tree_view concerné
 *
 * \return
 */
void etats_config_ui_onglet_mode_paiement_select_rows_from_list ( GSList *liste,
                        const gchar *treeview_name )
{
    GtkWidget *tree_view;
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    GtkTreeIter iter;
    GSList *tmp_list;

    if ( !liste )
        return;

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, treeview_name ) );
    model = gtk_tree_view_get_model ( GTK_TREE_VIEW ( tree_view ) );
    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );

    if ( gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( model ), &iter ) )
    {
        do
        {
            gchar *tmp_str;

            gtk_tree_model_get ( GTK_TREE_MODEL ( model ), &iter, 0, &tmp_str, -1 );

            tmp_list = liste;
            while ( tmp_list )
            {
                gchar *str;
                
                str = tmp_list -> data;

                if ( strcmp ( str, tmp_str ) == 0 )
                    gtk_tree_selection_select_iter ( GTK_TREE_SELECTION ( selection ), &iter );

                tmp_list = tmp_list -> next;
            }
            g_free ( tmp_str );
        }
        while ( gtk_tree_model_iter_next ( GTK_TREE_MODEL ( model ), &iter ) );
    }
}


/**
 * récupère la liste des libellés des item sélectionnés
 *
 * \param nom du tree_view
 *
 * \return numéro de la ligne sélectionnée
 */
GSList *etats_config_ui_onglet_mode_paiement_get_list_rows_selected ( const gchar *treeview_name )
{
    GtkWidget *tree_view;
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    GtkTreeIter iter;
    GSList *tmp_list = NULL;
    GList *rows_list;

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, treeview_name ) );
    if ( !tree_view )
        return NULL;

    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );
    rows_list = gtk_tree_selection_get_selected_rows ( selection, &model );
    while ( rows_list )
    {
        GtkTreePath *path;
        gchar *tmp_str;

        path = rows_list->data;

        gtk_tree_model_get_iter ( model, &iter, path) ;
        gtk_tree_model_get ( GTK_TREE_MODEL ( model ), &iter, 0, &tmp_str, -1 );

        tmp_list = g_slist_append ( tmp_list, tmp_str );
        
        gtk_tree_path_free ( path );
        rows_list = rows_list->next;
    }
    g_list_free ( rows_list );

    return tmp_list;
}


/*RIGHT_PANEL : ONGLET_DIVERS*/
/**
 * Création de l'onglet Divers
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_divers_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkWidget *button;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "onglet_etat_divers" ) );

    vbox = new_vbox_with_title_and_icon ( _("Miscellaneous"), "generalities.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "radiobutton_marked" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_onglet_divers_update_style_left_panel ),
                        GINT_TO_POINTER ( page ) );

    /* on met la connection pour rendre sensitif la vbox_marked_buttons */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "vbox_marked_buttons" ) );

    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "radiobutton_marked_No_R" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_onglet_divers_update_style_left_panel ),
                        GINT_TO_POINTER ( page ) );

    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "bouton_pas_detailler_ventilation" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_onglet_divers_update_style_left_panel ),
                        GINT_TO_POINTER ( page ) );

    gtk_widget_show_all ( vbox_onglet );

    return vbox_onglet;
}


/**
 * If applicable, update report navigation tree style to reflect which
 * pages have been changed.
 *
 * \param page_number Page that contained an interface element just
 *                      changed that triggered this event.
 *
 * \return      FALSE
 */
gboolean etats_config_ui_onglet_divers_update_style_left_panel ( GtkWidget *button,
                        gint *page_number )
{
    gint active;
    gint index;

    index = etats_config_ui_buttons_radio_get_active_index ( "radiobutton_marked_all" );
    active = etats_config_ui_toggle_button_get_actif ( "bouton_pas_detailler_ventilation" );

    if ( GTK_IS_RADIO_BUTTON ( button ) )
    {
        if ( active == 0 )
            etats_config_ui_left_panel_tree_view_update_style ( button, page_number );
    }
    else
    {
        if ( index == 0 )
            etats_config_ui_left_panel_tree_view_update_style ( button, page_number );
    }

    return TRUE;
}


/*RIGHT_PANEL : ONGLET_DATA_GROUPING*/
/**
 * Création de l'onglet groupement des donnés
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_data_grouping_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkWidget *button;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "page_data_grouping" ) );

    vbox = new_vbox_with_title_and_icon ( _("Data grouping"), "organization.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    etats_config_ui_onglet_data_grouping_init_tree_view ( );

    /* on met la connection pour modifier l'ordre des données dans le tree_view data_grouping */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "button_data_grouping_up" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "clicked",
                        G_CALLBACK ( etats_config_ui_onglet_data_grouping_button_clicked ),
                        GINT_TO_POINTER ( GSB_UP ) );

    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "button_data_grouping_down" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "clicked",
                        G_CALLBACK ( etats_config_ui_onglet_data_grouping_button_clicked ),
                        GINT_TO_POINTER ( GSB_DOWN ) );

    gtk_widget_show_all ( vbox_onglet );

    /* return */
    return vbox_onglet;
}


/**
 * crée un nouveau tree_view initialisé avec model.
 * le modèle comporte 3 colonnes : G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT
 * le tree_view n'affiche que la colonne texte.
 *
 * \return the tree_wiew
 */
gboolean etats_config_ui_onglet_data_grouping_init_tree_view ( void )
{
    GtkWidget *tree_view;
    GtkListStore *store;
    GtkTreeSelection *selection;
    GtkTreeViewColumn *column;
    GtkCellRenderer *cell;
    GtkTreeDragDestIface *dst_iface;
    GtkTreeDragSourceIface *src_iface;
    static GtkTargetEntry row_targets[] =
    {
        { "GTK_TREE_MODEL_ROW", GTK_TARGET_SAME_WIDGET, 0 }
    };

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "treeview_data_grouping" ) );
    if ( !tree_view )
        return FALSE;

    /* colonnes du list_store :
     *  1 : chaine affichée
     *  2 : numéro de ligne dans le modèle
     *  3 : type de donnée : 1 Categ, 3 IB, 5 Account, 6 Payee.
     */
    store = gtk_list_store_new ( 3, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT );
    gtk_tree_view_set_model ( GTK_TREE_VIEW ( tree_view ), GTK_TREE_MODEL ( store ) );
    g_object_unref ( G_OBJECT ( store ) );

    utils_set_tree_view_selection_and_text_color ( tree_view );

    /* set the column */
    cell = gtk_cell_renderer_text_new ( );

    column = gtk_tree_view_column_new_with_attributes ( NULL,
                        cell,
                        "text", 0,
                        NULL);
    gtk_tree_view_column_set_sizing ( GTK_TREE_VIEW_COLUMN ( column ), GTK_TREE_VIEW_COLUMN_FIXED );
    gtk_tree_view_append_column ( GTK_TREE_VIEW ( tree_view ),
                        GTK_TREE_VIEW_COLUMN ( column ) );
    gtk_tree_view_column_set_resizable ( column, TRUE );

    /* Enable drag & drop */
    gtk_tree_view_enable_model_drag_source ( GTK_TREE_VIEW ( tree_view ),
                        GDK_BUTTON1_MASK,
                        row_targets,
                        1,
                        GDK_ACTION_MOVE );
    gtk_tree_view_enable_model_drag_dest ( GTK_TREE_VIEW ( tree_view ),
                        row_targets,
                        1,
                        GDK_ACTION_MOVE );
    gtk_tree_view_set_reorderable ( GTK_TREE_VIEW (tree_view), TRUE );

    dst_iface = GTK_TREE_DRAG_DEST_GET_IFACE ( store );
    if ( dst_iface )
    {
        dst_iface -> drag_data_received = &etats_config_ui_onglet_data_grouping_drag_data_received;
        dst_iface -> row_drop_possible = &etats_config_ui_onglet_data_grouping_drop_possible;
    }

    src_iface = GTK_TREE_DRAG_SOURCE_GET_IFACE ( store );
    if ( src_iface )
    {
        gtk_selection_add_target ( tree_view,
                      GDK_SELECTION_PRIMARY,
                      GDK_SELECTION_TYPE_ATOM,
                      1 );
        src_iface -> drag_data_get = &etats_config_ui_onglet_data_grouping_drag_data_get;
    }

    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );
    g_signal_connect ( G_OBJECT ( selection ),
                        "changed",
                        G_CALLBACK ( etats_config_ui_onglet_data_grouping_selection_changed ),
                        tree_view );

    /* return */
    return TRUE;
}


/**
 * callback when treeview_data_grouping receive a drag and drop signal
 *
 * \param drag_dest
 * \param dest_path
 * \param selection_data
 *
 * \return FALSE
 */
gboolean etats_config_ui_onglet_data_grouping_drag_data_received ( GtkTreeDragDest *drag_dest,
                        GtkTreePath *dest_path,
                        GtkSelectionData *selection_data )
{
    if ( dest_path && selection_data )
    {
        GtkTreeModel *model;
        GtkTreeIter src_iter;
        GtkTreeIter dest_iter;
        GtkTreePath *src_path;
        gint src_pos = 0;
        gint dest_pos;
        gint src_type_data;
        gint dest_type_data;

        /* On récupère le model et le path d'origine */
        gtk_tree_get_row_drag_data ( selection_data, &model, &src_path );
        
        /* On récupère les données des 2 lignes à modifier */
        if ( gtk_tree_model_get_iter ( model, &src_iter, src_path ) )
            gtk_tree_model_get ( model, &src_iter, 1, &src_pos, 2, &src_type_data, -1 );

        if ( gtk_tree_model_get_iter ( model, &dest_iter, dest_path ) )
            gtk_tree_model_get ( model, &dest_iter, 1, &dest_pos, 2, &dest_type_data, -1 );
        else
            return FALSE;

        /* on met à jour la liste des types pour le tri de données */
        gsb_etats_config_onglet_data_grouping_move_in_list ( src_pos,
                        src_type_data,
                        dest_pos );

        return TRUE;
    }

    /* return */
    return FALSE;
}


/**
 * Fill the drag & drop structure with the path of selected column.
 * This is an interface function called from GTK, much like a callback.
 *
 * \param drag_source       Not used.
 * \param path              Original path for the gtk selection.
 * \param selection_data    A pointer to the drag & drop structure.
 *
 * \return FALSE, to allow future processing by the callback chain.
 */
gboolean etats_config_ui_onglet_data_grouping_drag_data_get ( GtkTreeDragSource *drag_source,
                        GtkTreePath *path,
                        GtkSelectionData *selection_data )
{
    if ( path )
    {
        GtkWidget *tree_view;
        GtkTreeModel *model;

        tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "treeview_data_grouping" ) );
        model = gtk_tree_view_get_model ( GTK_TREE_VIEW ( tree_view ) );

        gtk_tree_set_row_drag_data ( selection_data, GTK_TREE_MODEL ( model ), path );
    }

    return FALSE;
}
/**
 * Checks the validity of the change of position
 * This is an interface function called from GTK, much like a callback.
 *
 * \param drag_dest         Not used.
 * \param path              Original path for the gtk selection.
 * \param selection_data    A pointer to the drag & drop structure.
 *
 * \return FALSE, to allow future processing by the callback chain.
 */
gboolean etats_config_ui_onglet_data_grouping_drop_possible ( GtkTreeDragDest *drag_dest,
                        GtkTreePath *dest_path,
                        GtkSelectionData *selection_data )
{
    GtkTreePath *orig_path;
    GtkTreeModel *model;
    gint src_pos;
    gint dst_pos = 0;
    GtkTreeIter iter;

    gtk_tree_get_row_drag_data ( selection_data, &model, &orig_path );

    if ( gtk_tree_model_get_iter ( model, &iter, orig_path ) )
        gtk_tree_model_get ( model, &iter, 1, &src_pos, -1 );

    if ( gtk_tree_model_get_iter ( model, &iter, dest_path ) )
        gtk_tree_model_get ( model, &iter, 1, &dst_pos, -1 );

    if ( dst_pos < 0 || dst_pos > 3 )
        return FALSE;

    if ( src_pos != dst_pos )
        return TRUE;
    else
        return FALSE;
}


/**
 * callback when a button receive a clicked signal
 *
 * \param the button
 * \param a pointer for the direction of movement
 *
 * \return
 */
void etats_config_ui_onglet_data_grouping_button_clicked ( GtkWidget *button,
                        gpointer data )
{
    GtkWidget *tree_view;
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter orig_iter;

    /* On récupère le model et le path d'origine */
    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "treeview_data_grouping" ) );
    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );

    if ( gtk_tree_selection_get_selected ( selection, &model, &orig_iter ) )
    {
        GtkTreeIter dest_iter;
        GtkTreePath *path;
        gchar *string = NULL;
        gint orig_pos = 0;
        gint dest_pos;
        gint orig_type_data;
        gint dest_type_data;
        gint sens;

        sens = GPOINTER_TO_INT ( data );

        path = gtk_tree_model_get_path ( model, &orig_iter );

        /* On récupère les données des 2 lignes à modifier */
        gtk_tree_model_get ( model, &orig_iter, 1, &orig_pos, 2, &orig_type_data, -1 );

        if ( sens == GSB_UP )
            gtk_tree_path_prev ( path );
        else
            gtk_tree_path_next ( path );
            
        if ( gtk_tree_model_get_iter ( model, &dest_iter, path ) )
            gtk_tree_model_get ( model, &dest_iter, 1, &dest_pos, 2, &dest_type_data, -1 );
        else
            return;
        /* on met à jour la ligne de destination */
        string = gsb_etats_config_onglet_data_grouping_get_string ( orig_type_data, dest_pos );
        gtk_list_store_set ( GTK_LIST_STORE ( model ), &dest_iter, 0, string, 2, orig_type_data, -1 );

        g_free ( string );

        /* on met à jour la ligne d'origine */
        string = gsb_etats_config_onglet_data_grouping_get_string ( dest_type_data, orig_pos );
        gtk_list_store_set ( GTK_LIST_STORE ( model ), &orig_iter, 0, string, 2, dest_type_data, -1 );

        /* on garde la sélection sur le même iter */
        gtk_tree_selection_select_path ( selection, path );

        g_free ( string );
    }
}


/**
 * fonction de callback de changement de sélection
 *
 * \param selection
 * \param NULL
 *
 * \return
 */
void etats_config_ui_onglet_data_grouping_selection_changed ( GtkTreeSelection *selection,
                        GtkWidget *tree_view )
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    if ( gtk_tree_selection_get_selected ( selection, &model, &iter ) )
    {
        gint pos;

        gtk_tree_model_get ( model, &iter, 1, &pos, -1 );
        switch ( pos )
        {
            case 0:
                desensitive_widget ( NULL,
                                GTK_WIDGET ( gtk_builder_get_object (
                                etat_config_builder, "button_data_grouping_up" ) ) );
                sensitive_widget ( NULL,
                                GTK_WIDGET ( gtk_builder_get_object (
                                etat_config_builder, "button_data_grouping_down" ) ) );
                break;
            case 3:
                sensitive_widget ( NULL,
                                GTK_WIDGET ( gtk_builder_get_object (
                                etat_config_builder, "button_data_grouping_up" ) ) );
                desensitive_widget ( NULL,
                                GTK_WIDGET ( gtk_builder_get_object (
                                etat_config_builder, "button_data_grouping_down" ) ) );
                break;
            default:
                sensitive_widget ( NULL,
                                GTK_WIDGET ( gtk_builder_get_object (
                                etat_config_builder, "button_data_grouping_up" ) ) );
                sensitive_widget ( NULL,
                                GTK_WIDGET ( gtk_builder_get_object (
                                etat_config_builder, "button_data_grouping_down" ) ) );
                break;
        }
    }
}


/*RIGHT_PANEL : ONGLET_DATA_SEPARATION*/
/**
 * Création de l'onglet séparation des donnés
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_data_separation_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkComboBox *combo_1;
    GtkComboBox *combo_2;
    GtkTreeModel *model;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "page_data_separation" ) );

    vbox = new_vbox_with_title_and_icon ( _("Data separation"), "organization.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    /* on met la connexion pour la séparation par exercice avec le bouton radio_button_utilise_exo */
    g_signal_connect ( G_OBJECT ( gtk_builder_get_object ( etat_config_builder, "radio_button_utilise_exo" ) ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_separe_exo_etat" ) );

    /* on met la connexion pour rendre sensible la boite avec le bouton bouton_type_separe_plages_etat */
    g_signal_connect ( G_OBJECT ( gtk_builder_get_object ( etat_config_builder, "bouton_separe_plages_etat" ) ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "paddingbox_data_separation2" ) );

    /* on crée le bouton avec les pérodes pour la séparation de l'état */
    model = GTK_TREE_MODEL ( utils_list_store_create_from_string_array ( data_separation_periodes ) );
    combo_1 = GTK_COMBO_BOX ( gtk_builder_get_object ( etat_config_builder, "bouton_type_separe_plages_etat" ) );
    gtk_combo_box_set_model ( combo_1, model );
    utils_gtk_combo_box_set_text_renderer ( GTK_COMBO_BOX ( combo_1 ), 0 );

    model = GTK_TREE_MODEL ( utils_list_store_create_from_string_array ( jours_semaine ) );
    combo_2 = GTK_COMBO_BOX ( gtk_builder_get_object ( etat_config_builder, "bouton_debut_semaine" ) );
    gtk_combo_box_set_model ( combo_2, model );
    utils_gtk_combo_box_set_text_renderer ( GTK_COMBO_BOX ( combo_2 ), 0 );

    /* on connecte le signal "changed" au bouton bouton_type_separe_plages_etat
     * pour rendre insensible le choix du jour de la semaine pour les choix
     * autres que la semaine. On le met ici pour que l'initialisation se fasse
     * proprement */
    g_signal_connect ( G_OBJECT ( combo_1 ),
                        "changed",
                        G_CALLBACK ( gsb_etats_config_onglet_data_separation_combo_changed ),
                        combo_2 );

    gtk_widget_show_all ( vbox_onglet );

    /* return */
    return vbox_onglet;
}


/*RIGHT_PANEL : ONGLET_AFFICHAGE_GENERALITES*/
/**
 * Création de l'onglet affichage des generalites
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_affichage_generalites_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "affichage_etat_generalites" ) );

    vbox = new_vbox_with_title_and_icon ( _("Generalities"), "generalities.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    gtk_widget_show_all ( vbox_onglet );

    /* return */
    return vbox_onglet;
}


/*RIGHT_PANEL : ONGLET_AFFICHAGE_TITLES*/
/**
 * Création de l'onglet affichage de diverses données
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_affichage_titles_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkWidget *button;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "affichage_etat_titles" ) );

    vbox = new_vbox_with_title_and_icon ( _("Titles"), "title.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    /* on met les connexions pour sensibiliser désensibiliser les données associées */
    g_signal_connect ( G_OBJECT ( gtk_builder_get_object ( etat_config_builder, "bouton_regroupe_ope_compte_etat" ) ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_affiche_sous_total_compte" ) );

    /* affichage possible des tiers */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "bouton_utilise_tiers_etat" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_afficher_noms_tiers" ) );

    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_affiche_sous_total_tiers" ) );

    /* affichage possible des categories */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "bouton_group_by_categ" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_afficher_noms_categ" ) );

    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_affiche_sous_total_categ" ) );

    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_afficher_sous_categ" ) );

    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_affiche_sous_total_sous_categ" ) );

    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_afficher_pas_de_sous_categ" ) );

    /* affichage possible des ib */
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "bouton_utilise_ib_etat" ) );
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_afficher_noms_ib" ) );

    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_affiche_sous_total_ib" ) );

    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_afficher_sous_ib" ) );

    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_affiche_sous_total_sous_ib" ) );

    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "bouton_afficher_pas_de_sous_ib" ) );

    gtk_widget_show_all ( vbox_onglet );

    /* return */
    return vbox_onglet;
}


/*RIGHT_PANEL : ONGLET_AFFICHAGE_OPERATIONS*/
/**
 * Création de l'onglet affichage de opérations
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_affichage_operations_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;
    GtkWidget *button;
    GtkComboBox *combo;
    GtkTreeModel *model;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "affichage_etat_operations" ) );

    vbox = new_vbox_with_title_and_icon ( _("Transactions display"), "transdisplay.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "bouton_afficher_opes" ) );
    /* on met la connection pour changer le style de la ligne du panneau de gauche */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( etats_config_ui_left_panel_tree_view_update_style ),
                        GINT_TO_POINTER ( page ) );

    /* on met la connection pour rendre sensitif la vbox_show_transactions */
    g_signal_connect ( G_OBJECT ( button ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object ( etat_config_builder, "vbox_show_transactions" ) );

    /* on crée le bouton avec les types de classement des opérations */
    model = GTK_TREE_MODEL ( utils_list_store_create_from_string_array ( etats_config_ui_classement_operations ) );
    combo = GTK_COMBO_BOX ( gtk_builder_get_object ( etat_config_builder, "bouton_choix_classement_ope_etat" ) );
    gtk_combo_box_set_model ( combo, model );
    utils_gtk_combo_box_set_text_renderer ( GTK_COMBO_BOX ( combo ), 0 );

    /* on met les connexions */
    g_signal_connect ( G_OBJECT ( gtk_builder_get_object (
                        etat_config_builder, "bouton_afficher_categ_opes" ) ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object (
                        etat_config_builder, "bouton_afficher_sous_categ_opes" ) );

    g_signal_connect ( G_OBJECT ( gtk_builder_get_object (
                        etat_config_builder, "bouton_afficher_ib_opes" ) ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object (
                        etat_config_builder, "bouton_afficher_sous_ib_opes" ) );

    g_signal_connect ( G_OBJECT ( gtk_builder_get_object (
                        etat_config_builder, "bouton_afficher_titres_colonnes" ) ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object (
                        etat_config_builder, "bouton_titre_changement" ) );

    g_signal_connect ( G_OBJECT ( gtk_builder_get_object (
                        etat_config_builder, "bouton_afficher_titres_colonnes" ) ),
                        "toggled",
                        G_CALLBACK ( sens_desensitive_pointeur ),
                        gtk_builder_get_object (
                        etat_config_builder, "bouton_titre_en_haut" ) );

    gtk_widget_show_all ( vbox_onglet );

    return vbox_onglet;
}


/*RIGHT_PANEL : ONGLET_AFFICHAGE_DEVISES*/
/**
 * Création de l'onglet affichage des devises
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_onglet_affichage_devises_create_page ( gint page )
{
    GtkWidget *vbox_onglet;
    GtkWidget *vbox;

    vbox_onglet =  GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, "affichage_etat_devises" ) );

    vbox = new_vbox_with_title_and_icon ( _("Totals currencies"), "currencies.png" );

    gtk_box_pack_start ( GTK_BOX ( vbox_onglet ), vbox, FALSE, FALSE, 0 );
    gtk_box_reorder_child ( GTK_BOX ( vbox_onglet ), vbox, 0 );

    gtk_widget_show_all ( vbox_onglet );

    gsb_etats_config_onglet_affichage_devises_make_combobox ( );

    gtk_widget_show_all ( vbox_onglet );

    return vbox_onglet;
}


/*FONCTIONS UTILITAIRES COMMUNES*/
/**
 * retourne l'index du radiobutton actif.
 *
 * \param nom du radio_button
 *
 * \return index bouton actif
 */
gint etats_config_ui_buttons_radio_get_active_index ( const gchar *button_name )
{
    GtkWidget *radio_button;
    GSList *liste;
    GSList *tmp_list;
    gint index = 0;

    radio_button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, button_name ) );

    liste = g_slist_copy ( gtk_radio_button_get_group ( GTK_RADIO_BUTTON ( radio_button ) ) );
    tmp_list = g_slist_reverse ( liste );

    while ( tmp_list )
    {
        GtkWidget *button;

        button = tmp_list->data;
        if ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( button ) ) )
            break;

        index++;
        tmp_list = tmp_list->next;
    }

    g_slist_free ( liste );

    return index;
}

/**
 * rend actif le button qui correspond à l'index passé en paramètre.
 *
 * \param nom du radio_button
 * \param index du bouton à rendre actif
 *
 * \return index bouton actif
 */
void etats_config_ui_buttons_radio_set_active_index ( const gchar *button_name,
                        gint index )
{
    GtkWidget *radio_button;
    GSList *liste;
    GSList *tmp_list;

    radio_button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, button_name ) );

    liste = g_slist_copy ( gtk_radio_button_get_group ( GTK_RADIO_BUTTON ( radio_button ) ) );
    tmp_list = g_slist_reverse ( liste );

    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( g_slist_nth_data ( tmp_list, index ) ), TRUE );

    g_slist_free ( liste );
}


/**
 * initialise le bouton expand collapse all
 *
 * \param suffixe name
 * \param tree_view
 *
 * \return
 */
void etats_config_ui_toggle_button_init_button_expand ( gchar *name,
                        GtkWidget *tree_view )
{
    GtkWidget *button;
    gchar *tmp_str;

    tmp_str = g_strconcat ( "togglebutton_expand_", name, NULL );
    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, tmp_str ) );
    g_free ( tmp_str );

    tmp_str = g_strconcat ( "hbox_toggle_expand_", name, NULL );
    g_object_set_data ( G_OBJECT ( button ), "hbox_expand",
                        gtk_builder_get_object ( etat_config_builder, tmp_str ) );
    g_free ( tmp_str );

    tmp_str = g_strconcat ( "hbox_toggle_collapse_", name, NULL );
    g_object_set_data ( G_OBJECT ( button ), "hbox_collapse",
                        gtk_builder_get_object ( etat_config_builder, tmp_str ) );

    g_signal_connect ( G_OBJECT ( button ),
                        "clicked",
                        G_CALLBACK ( utils_togglebutton_collapse_expand_all_rows ),
                        tree_view );
}


/**
 * retourne l'état du togglebutton dont le nom est passé en paramètre.
 *
 * \param widget name
 *
 * \return activ
 */
gboolean etats_config_ui_toggle_button_get_actif ( const gchar *button_name )
{
    GtkWidget *button = NULL;

    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, button_name ) );
    if ( !button )
        return FALSE;

    /* return */
    return gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( button ) );
}


/**
 *  Positionne letogglebutton demandé par son nom en fonction de actif
 *
 *\param widget name
 *\param actif
 *
 * \return TRUE if success FALSE otherwise
 */
gboolean etats_config_ui_toggle_button_set_actif ( const gchar *button_name,
                        gboolean actif )
{
    GtkWidget *button = NULL;

    button = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, button_name ) );
    if ( !button )
        return FALSE;

    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( button ), actif );

    return TRUE;
}


/**
 * crée un nouveau tree_view initialisé avec model.
 * le modèle comporte deux colonnes : G_TYPE_STRING, G_TYPE_INT
 * le tree_view n'affiche que la colonne texte.
 *
 * \return the tree_wiew
 */
GtkWidget *etats_config_ui_tree_view_new_with_model ( const gchar *treeview_name,
                        GtkTreeModel *model )
{
    GtkWidget *tree_view;
    GtkCellRenderer *cell;
    GtkTreeViewColumn *column;

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, treeview_name ) );
    if ( !tree_view )
        return NULL;

    gtk_tree_view_set_model ( GTK_TREE_VIEW ( tree_view ), GTK_TREE_MODEL ( model ) );
    g_object_unref ( G_OBJECT ( model ) );

    utils_set_tree_view_selection_and_text_color ( tree_view );

    /* set the column */
    cell = gtk_cell_renderer_text_new ( );

    column = gtk_tree_view_column_new_with_attributes ( NULL,
                        cell,
                        "text", 0,
                        NULL);
    gtk_tree_view_column_set_sizing ( GTK_TREE_VIEW_COLUMN ( column ), GTK_TREE_VIEW_COLUMN_FIXED );
    gtk_tree_view_append_column ( GTK_TREE_VIEW ( tree_view ),
                        GTK_TREE_VIEW_COLUMN ( column ) );
    gtk_tree_view_column_set_resizable ( column, TRUE );

    return tree_view;
}


/**
 * récupère l'index l'iter selectionné
 *
 * \param nom du tree_view
 *
 * \return numéro de la ligne sélectionnée
 */
gint etats_config_ui_tree_view_get_single_row_selected ( const gchar *treeview_name )
{
    GtkWidget *tree_view;
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    GtkTreeIter iter;

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, treeview_name ) );
    if ( !tree_view )
        return -1;

    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );

    if ( gtk_tree_selection_get_selected ( selection, &model, &iter ) )
    {
        gint index;

        gtk_tree_model_get ( GTK_TREE_MODEL ( model ), &iter, 1, &index, -1 );

        return index;
    }
    return -1;
}


/**
 *
 *
 * \param
 *
 * \return
 */
void etats_config_ui_tree_view_select_single_row ( const gchar *treeview_name,
                        gint numero )
{
    GtkWidget *tree_view;
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    GtkTreeIter iter;

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, treeview_name ) );
    if ( !tree_view )
        return;

    model = gtk_tree_view_get_model ( GTK_TREE_VIEW ( tree_view ) );
    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );

    if ( gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( model ), &iter ) )
    {
        do
        {
            gint index;

            gtk_tree_model_get ( GTK_TREE_MODEL ( model ), &iter, 1, &index, -1 );

            if ( numero == index )
            {
                gtk_tree_selection_select_iter ( GTK_TREE_SELECTION ( selection ), &iter );
                break;
            }
        }
        while ( gtk_tree_model_iter_next ( GTK_TREE_MODEL ( model ), &iter ) );
    }
}


/**
 *
 *
 * \param
 *
 * \return
 */
GtkWidget *etats_config_ui_widget_get_widget_by_name ( const gchar *parent_name,
                        const gchar *child_name )
{
    return utils_gtkbuilder_get_widget_by_name ( etat_config_builder, parent_name, child_name );
}


/**
 * récupère l'index l'iter selectionné
 *
 * \param nom du tree_view
 *
 * \return numéro de la ligne sélectionnée
 */
GSList *etats_config_ui_tree_view_get_list_rows_selected ( const gchar *treeview_name )
{
    GtkWidget *tree_view;
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    GtkTreeIter iter;
    GSList *tmp_list = NULL;
    GList *rows_list;

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, treeview_name ) );
    if ( !tree_view )
        return NULL;

    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );
    rows_list = gtk_tree_selection_get_selected_rows ( selection, &model );
    while ( rows_list )
    {
        GtkTreePath *path;
        gint index;

        path = rows_list->data;

        gtk_tree_model_get_iter ( model, &iter, path) ;
        gtk_tree_model_get ( GTK_TREE_MODEL ( model ), &iter, 1, &index, -1 );

        tmp_list = g_slist_append ( tmp_list, GINT_TO_POINTER ( index ) );
        
        gtk_tree_path_free ( path );
        rows_list = rows_list->next;
    }
    g_list_free ( rows_list );

    return tmp_list;
}


/**
 * initialise le tree_view avec son modèle et son type de sélection
 *
 * \param
 *
 * \return
 */
void etats_config_ui_tree_view_init ( const gchar *treeview_name,
                        GtkTreeModel *(*function) ( void ),
                        GtkSelectionMode type_selection,
                        GCallback selection_callback )
{
    GtkWidget *tree_view;
    GtkTreeModel *model;
    GtkTreeSelection *selection;

    /* on récupère le model par appel à function */
    model = function ( );

    tree_view = etats_config_ui_tree_view_new_with_model ( treeview_name, GTK_TREE_MODEL ( model ) );
    gtk_tree_view_set_fixed_height_mode ( GTK_TREE_VIEW ( tree_view ), TRUE );
    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );
    gtk_tree_selection_set_mode ( selection, type_selection );

    if ( selection_callback )
        g_signal_connect ( G_OBJECT ( selection ),
                        "changed",
                        G_CALLBACK ( selection_callback ),
                        NULL );
}


/**
 * Sélectionne les iters en fonction des données de la liste
 *
 * \param liste des lignes à sélectionner
 * \param nom du tree_view concerné
 * \param numéro de la colonne contenant la donnée testée
 *
 * \return
 */
void etats_config_ui_tree_view_select_rows_from_list ( GSList *liste,
                        const gchar *treeview_name,
                        gint column )
{
    GtkWidget *tree_view;
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    GtkTreeIter iter;
    GSList *tmp_list;

    if ( !liste )
        return;

    tree_view = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, treeview_name ) );
    model = gtk_tree_view_get_model ( GTK_TREE_VIEW ( tree_view ) );
    selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( tree_view ) );

    if ( gtk_tree_model_get_iter_first ( GTK_TREE_MODEL ( model ), &iter ) )
    {
        do
        {
            gint tmp_number;

            gtk_tree_model_get ( GTK_TREE_MODEL ( model ), &iter, column, &tmp_number, -1 );

            tmp_list = liste;
            while ( tmp_list )
            {
                gint result;
                
                result = GPOINTER_TO_INT ( tmp_list -> data );

                if ( result == tmp_number )
                    gtk_tree_selection_select_iter ( GTK_TREE_SELECTION ( selection ), &iter );

                tmp_list = tmp_list -> next;
            }
        }
        while ( gtk_tree_model_iter_next ( GTK_TREE_MODEL ( model ), &iter ) );
    }
}


/**
 *  rend sensible le widget demandé par son nom en fonction de sensible
 *
 *\param widget name
 *\param sensitive
 *
 * \return TRUE if success FALSE otherwise
 */
gboolean etats_config_ui_widget_set_sensitive ( const gchar *widget_name,
                        gboolean sensitive )
{
    GtkWidget *widget = NULL;

    widget = GTK_WIDGET ( gtk_builder_get_object ( etat_config_builder, widget_name ) );
    if ( !widget )
        return FALSE;

    gtk_widget_set_sensitive ( widget, sensitive );

    return TRUE;
}


/*END_PRIVATE_FUNCTIONS*/
/* Local Variables: */
/* c-basic-offset: 4 */
/* End: */

