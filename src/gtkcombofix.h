/* ComboFix Widget
 * Copyright (C) 2001 C�dric Auger
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#ifndef __GTK_COMBOFIX__H__
#define __GTK_COMBOFIX__H__


#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
  
  
#define GTK_COMBOFIX(obj) GTK_CHECK_CAST(obj, gtk_combofix_get_type(), GtkComboFix )
#define GTK_COMBOFIX_CLASS(klass) GTK_CHECK_CLASS_CAST( klass, gtk_combofix_get_type(), GtkComboFixClass )
#define GTK_IS_COMBOFIX(obj) GTK_CHECK_TYPE ( obj, gtk_combofix_get_type() )
  
  
  typedef struct _GtkComboFix GtkComboFix;
  typedef struct _GtkComboFixClass GtkComboFixClass;


  /* stucture de base de la ComboFix */

  struct _GtkComboFix
  {
    GtkVBox vbox;

    /* adr de l'entr�e */

    GtkWidget *entry;

    /*  adr de la liste contenant les labels */

    GSList *liste;

    /* � 1 si on veut que la liste s'affiche en tapant, � 0 sinon */

    gint affiche_liste;

    /* � 1 si le texte doit obligatoirement correspondre � un mot dans la liste, � 0 sinon  */

    gint force_text;


    /* variables priv�es */

    /* liste complexe */
    gint complex;
    /* adresse de la fl�che  */
    GtkWidget *arrow; 
    /*   adresse de la fenetre de la liste de la combofix */
    GtkWidget *popup;
    /* adresse de la boite ( scrolledWindow ) contenant la liste */
    GtkWidget *box;
    /* liste des event_box */
    GSList *event_box;
    /* label s�lectionn� */
    gint label_selectionne;
    /* liste des compl�tions */
    GSList *liste_completion;
    /* automatiquement class� */
    gint auto_sort;

  };

  struct _GtkComboFixClass
  {
    GtkVBoxClass parent_class;
  };


  guint gtk_combofix_get_type ( void );


  /* **************************************************************************************************** */
  /* gtk_combofix_new : */
  /* arguments :             */
  /* liste :                      liste de noms ( \t en d�but de chaine pour une sous cat�gorie ) */
  /* force_text :            TRUE ( le texte doit correspondre � la liste ) / FALSE */
  /* affiche_liste :         TRUE ( la liste sera affich�e en tapant le mot ) / FALSE */
  /* classement_auto : TRUE ( la liste est toujours class�e par ordre alphab�tique ) / FALSE  */
  /*                                                                                                                                      */
  /* retour : le widget gtk_combofix ( une hbox contenant l'entr�e et d'une arrow ) */
  /* **************************************************************************************************** */

  GtkWidget *gtk_combofix_new ( GSList *liste,
				gint force_text,
				gint affiche_liste,
				gint classement_auto );

  /* **************************************************************************************************** */
  /* gtk_combofix_new_complex : */
  /* arguments :                             */
  /* liste :                      liste de liste de noms ( \t en d�but de chaine pour une sous cat�gorie ) */
  /*                               chaque liste sera s�par�e d'une ligne */
  /* force_text :            TRUE ( le texte doit correspondre � la liste ) / FALSE */
  /* affiche_liste :         TRUE ( la liste sera affich�e en tapant le mot ) / FALSE */
  /* classement_auto : TRUE ( la liste est toujours class�e par ordre alphab�tique ) / FALSE  */
  /*                                                                                                                                      */
  /* retour : le widget gtk_combofix ( une hbox contenant l'entr�e et d'une arrow ) */
  /* **************************************************************************************************** */
  
  GtkWidget *gtk_combofix_new_complex ( GSList *liste,
					gint force_text,
					gint affiche_liste,
					gint classement_auto );


  /* **************************************************************************************************** */
  /* fonction gtk_combofix_set_text */
  /* arguments : le combofix et le texte � mettre dedans */
  /* affiche le texte correspondant dans le combofix */
  /* **************************************************************************************************** */

  void gtk_combofix_set_text ( GtkComboFix *combofix,
			       gchar *text );


  /* **************************************************************************************************** */
  /* fonction gtk_combofix_get_text */
  /* arguments : le combofix  */
  /* retourne un pointeur vers le texte  dans le combofix */
  /* **************************************************************************************************** */

  gchar *gtk_combofix_get_text ( GtkComboFix *combofix );


  /* **************************************************************************************************** */
  /* Fonction gtk_combofix_force_text : */
  /* lorsqu'il est activ�, l'utilisateur est oblig� de taper un texte pr�sent dans la liste */
  /* argument : la combofix */
  /*                     booleen TRUE ( force ) / FALSE */
  /* **************************************************************************************************** */

  void gtk_combofix_force_text ( GtkComboFix *combofix,
				 gint valeur );


  /* **************************************************************************************************** */
  /* Fonction gtk_combofix_view_list : */
  /* lorsqu'il est activ�, une liste d�roulante contenant les compl�tions possibles */
  /* s'affiche avec la frappe de l'utilisateur */
  /* argument :  la combofix */
  /*                      booleen TRUE ( met la liste ) / FALSE */
  /* **************************************************************************************************** */

  void gtk_combofix_view_list ( GtkComboFix *combofix,
				gint valeur );


  /* **************************************************************************************************** */
  /* Fonction gtk_combofix_show_arrow : */
  /* affiche ou non la fl�che permettant d'afficher la liste */
  /* argument : la combofix */
  /*                     booleen TRUE ( affiche la fl�che ) / FALSE */
  /* **************************************************************************************************** */

  void gtk_combofix_show_arrow ( GtkComboFix *combofix,
				 gint valeur );


  /* **************************************************************************************************** */
  /* Fonction gtk_combofix_sort : */
  /* classe la liste en respectant les diff�rentes listes si c'est un complex, et en respectant les cat�g / sous-categ  */
  /* argument : la combofix */
  /* **************************************************************************************************** */

  void gtk_combofix_sort ( GtkComboFix *combofix );


  /* **************************************************************************************************** */
  /* Fonction gtk_combofix_set_list : */
  /* efface l'ancienne liste si elle existe et met la nouvelle  */
  /* arguments :                           */
  /*       combofix : la combofix          */
  /*       liste : une gslist              */
  /*       complex : TRUE : complex / FALSE */
  /*       classement_auto : TRUE / FALSE   */
  /* **************************************************************************************************** */

  void gtk_combofix_set_list ( GtkComboFix *combofix,
			       GSList *liste,
			       gint complex,
			       gint classement_auto );





#ifdef __cplusplus
}
#endif				/* __cplusplus */

#endif				/* __GTK_COMBOFIX_H__ */
