/* fichier d'en t�te structures.h */
/* contient toutes les structures du prog */


#define VERSION_FICHIER "0.4.0"
#define VERSION_FICHIER_ETAT "0.4.0"
#define VERSION_FICHIER_CATEG "0.4.0"
#define VERSION_FICHIER_IB "0.4.0"

/* Chemin des fichiers */

#define FICHIER_CONF "Grisbi-dev"       /* � mettre � Grisbi-dev pdt le dvt et Grisbi � la sortie d'une version */

/* initialisation des couleurs */

#define COULEUR1_RED  55000
#define COULEUR1_GREEN  55000
#define COULEUR1_BLUE  65535
#define COULEUR2_RED  65535
#define COULEUR2_GREEN  65535
#define COULEUR2_BLUE  65535
#define COULEUR_SELECTION_RED  63000
#define COULEUR_SELECTION_GREEN  40000
#define COULEUR_SELECTION_BLUE  40000
#define COULEUR_ROUGE_RED  65535
#define COULEUR_ROUGE_GREEN  0
#define COULEUR_ROUGE_BLUE  0
#define COULEUR_NOIRE_RED 0
#define COULEUR_NOIRE_GREEN 0
#define COULEUR_NOIRE_BLUE 0
#define COULEUR_GRISE_RED 50000
#define COULEUR_GRISE_GREEN 50000
#define COULEUR_GRISE_BLUE 50000

/* variables contenant juste 0 ou 1 */

struct {
  guint ouvre_fichier : 1;
  guint modification_fichier : 1;
  guint formulaire_en_cours : 1;
  guint ancienne_date : 1;
  guint ctrl : 1;
  guint equilibrage : 1;
  guint valeur_r_avant_rapprochement : 1;
  guint r_modifiable : 1;
  guint dernier_fichier_auto : 1;
  guint sauvegarde_auto : 1;
  guint entree : 1;
  guint alerte_mini : 1;
  guint formulaire_toujours_affiche : 1;
  guint formulaire_echeancier_toujours_affiche : 1;
  guint formulaire_echeance_dans_fenetre : 1;        /* � 1 lorsque le formulaire de l'�ch�ancier est dans une fenetre � part */
  guint alerte_permission : 1;   /* � un si le message d'alerte s'affiche */
  guint fichier_deja_ouvert : 1;   /* � un si lors de l'ouverture, le fichier semblait d�j� ouvert */
  guint force_enregistrement : 1;    /* � un si on force l'enregistrement */
  guint affichage_exercice_automatique : 1;   /* � 1 si exercice automatique selon la date, 0 si affiche le dernier choisi */
  guint affiche_tous_les_types : 1;      /* � 1 si tous les types sont dans le check button et non pas juste les d�bits ou cr�dits */
  guint affiche_no_operation :1;
  guint affiche_date_bancaire :1; /* GDC : � 1 si on veut afficher la colonne et le champ de la date r�elle */
  guint utilise_exercice :1;
  guint utilise_imputation_budgetaire :1;
  guint utilise_piece_comptable :1;
  guint utilise_info_banque_guichet :1;
  guint affiche_boutons_valider_annuler :1;
  guint affiche_nb_ecritures_listes :1;
  guint classement_par_date :1;   /* � 1 si le classement de la liste d'op� se fait par date */
  guint largeur_auto_colonnes :1;
  guint retient_affichage_par_compte :1;   /* � 1 si les caract�ristiques de l'affichage (R, non R ...) diff�rent par compte */
} etat;



struct structure_operation
{
  guint no_operation;

  GDate *date;
  guint jour;
  guint mois;
  guint annee;

  GDate *date_bancaire;
  guint jour_bancaire;
  guint mois_bancaire;
  guint annee_bancaire;

  guint no_compte;

  gdouble montant;
  guint devise;
  guint une_devise_compte_egale_x_devise_ope;       /* si = 1 : 1 devise_compte = "taux_change * montant" devise_en_rapport */
  gdouble taux_change;
  gdouble frais_change;

  guint tiers;        /*  no du tiers */
  guint categorie;
  guint sous_categorie;
  gint operation_ventilee;        /* � 1 si c'est une op� ventil�e */

  gchar *notes;

  guint type_ope;               /* variable suivant le type de compte */
  gchar *contenu_type;          /* ce peut �tre un no de ch�que, de virement ou tout ce qu'on veut */

  gshort pointe;            /*  0=rien, 1=point�e, 2=rapproch�e */
  gshort auto_man;           /* 0=manuel, 1=automatique */
  gint no_rapprochement;          /* contient le num�ro du rapprochement si elle est rapproch�e */

  guint no_exercice;             /* exercice de l'op� */
  guint imputation;
  guint sous_imputation;
  gchar *no_piece_comptable;
  gchar *info_banque_guichet;

  guint relation_no_operation;
  guint relation_no_compte;

  guint no_operation_ventilee_associee;      /* si c'est une op� d'une ventilation, contient le no de l'op� ventil�e */
};




struct donnees_compte
{
  gint no_compte;
  gint type_de_compte;          /* 0 = bancaire, 1 = esp�ce, 2 = passif, 3= actif */
  gchar *nom_de_compte;
  gint nb_operations;
  gdouble solde_initial;
  gdouble solde_mini_voulu;
  gdouble solde_mini_autorise;
  gdouble solde_courant;
  gdouble solde_pointe;
  GSList *gsliste_operations;
  GtkWidget *clist_operations;        /* adr de la liste des op� */
  struct structure_operation *operation_selectionnee;       /* contient l'adr de la struct de la ligne s�lectinn�e */
  gint message_sous_mini;
  gint message_sous_mini_voulu;
  GDate *date_releve;
  gdouble solde_releve;
  gint dernier_no_rapprochement;
  GtkWidget *icone_ouverte;              /* adr de l'icone ouverte */
  GtkWidget *icone_fermee;              /* adr de l'icone ferm�e */
  gint mise_a_jour;                      /* � 1 lorsque la liste doit �tre rafraichie � l'affichage */
  gint devise;                         /* contient le no de la devise du compte */
  gint banque;                                      /* 0 = aucune, sinon, no de banque */
  gchar *no_guichet;
  gchar *no_compte_banque;
  gchar *cle_compte;
  gint compte_cloture;                          /* si = 1 => clotur� */
  gchar *commentaire;
  gint tri;                           /* si = 1 => tri en fonction des types, si 0 => des dates */
  GSList *liste_tri;                         /* contient la liste des types dans l'ordre du tri du compte */
  gint neutres_inclus;                           /* si = 1 => les neutres sont inclus dans les d�bits/cr�dits */
  gchar *titulaire;                  /* contient le nom du titulaire de ce compte */
  gchar *adresse_titulaire;          /* � NULL si c'est l'adresse commune, contient l'adresse sinon */
  GSList *liste_types_ope;         /* liste qui contient les struct de types d'op� du compte */
  gint type_defaut_debit;            /* no du type par d�faut */
  gint type_defaut_credit;            /* no du type par d�faut */
  gint affichage_r;            /* � 1 si les R sont affich�s pour ce compte */
  gint nb_lignes_ope;           /* contient le nb de lignes pour une op� (1, 2, 3, 4 ) */
};



/* pointeurs vers les comptes en fonction de p...variable */

#define NO_COMPTE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> no_compte
#define TYPE_DE_COMPTE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> type_de_compte
#define NOM_DU_COMPTE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> nom_de_compte
#define NB_OPE_COMPTE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> nb_operations
#define SOLDE_INIT ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> solde_initial
#define SOLDE_MINI_VOULU ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> solde_mini_voulu 
#define SOLDE_MINI ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> solde_mini_autorise
#define SOLDE_COURANT ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> solde_courant
#define SOLDE_POINTE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> solde_pointe
#define LISTE_OPERATIONS ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> gsliste_operations
#define CLIST_OPERATIONS ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> clist_operations
#define OPERATION_SELECTIONNEE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> operation_selectionnee
#define MESSAGE_SOUS_MINI ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) ->  message_sous_mini
#define MESSAGE_SOUS_MINI_VOULU ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) ->  message_sous_mini_voulu
#define DATE_DERNIER_RELEVE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) ->  date_releve
#define SOLDE_DERNIER_RELEVE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) ->  solde_releve
#define DERNIER_NO_RAPPROCHEMENT ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> dernier_no_rapprochement
#define ICONE_OUVERTE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> icone_ouverte
#define ICONE_FERMEE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> icone_fermee
#define MISE_A_JOUR ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> mise_a_jour
#define DEVISE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> devise
#define BANQUE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> banque
#define NO_GUICHET ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> no_guichet
#define NO_COMPTE_BANQUE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> no_compte_banque
#define CLE_COMPTE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> cle_compte
#define COMPTE_CLOTURE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> compte_cloture
#define COMMENTAIRE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> commentaire
#define TRI ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> tri
#define LISTE_TRI ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> liste_tri
#define NEUTRES_INCLUS ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> neutres_inclus
#define TITULAIRE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> titulaire
#define ADRESSE_TITULAIRE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> adresse_titulaire
#define TYPES_OPES ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> liste_types_ope
#define TYPE_DEFAUT_DEBIT ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> type_defaut_debit
#define TYPE_DEFAUT_CREDIT ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> type_defaut_credit
#define AFFICHAGE_R ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> affichage_r
#define NB_LIGNES_OPE ((struct donnees_compte *) (*p_tab_nom_de_compte_variable)) -> nb_lignes_ope


struct operation_echeance
{
  guint no_operation;
  gint compte;

  GDate *date;
  guint jour;
  guint mois;
  guint annee;

  gdouble montant;
  gint devise;

  guint tiers;
  guint categorie;
  guint sous_categorie;
  gint compte_virement;
  gchar *notes;

  gint type_ope;        /* variable suivant le compte */
  gchar *contenu_type;          /* ce peut �tre un no de ch�que, de virement ou tout ce qu'on veut */

  gshort auto_man;           /* 0=manuel, 1=automatique */

  guint no_exercice;             /* exercice de l'op� */
  guint imputation;
  guint sous_imputation;

  gshort periodicite;             /*  0=une fois, 1=hebdo, 2=mensuel, 3=annuel, 4=perso */
  gshort intervalle_periodicite_personnalisee;   /* 0=jours, 1=mois, 2=annees */
  guint periodicite_personnalisee;

  GDate *date_limite;
  gint jour_limite;
  gint mois_limite;
  gint annee_limite;
};



#define COMPTE_ECHEANCE GTK_LABEL (GTK_BIN ( widget_formulaire_echeancier[5]  )  -> child ) -> label
#define ECHEANCE_COURANTE ((struct operation_echeance *)  ( pointeur_liste -> data ))


struct struct_tiers
{
  guint no_tiers;
  gchar *nom_tiers;
  gchar *texte;                  /* contient le texte associ� au tiers */
  gint liaison;                    /* � 1 si ce tiers est li� � un autre logiciel */
};


struct struct_categ
{
  gint no_categ;
  gchar *nom_categ;
  gint type_categ;                   /* 0=cr�dit ; 1 = d�bit ; 2 = sp�cial */
  gint no_derniere_sous_categ;
  GSList *liste_sous_categ;
};

struct struct_sous_categ
{
  gint no_sous_categ;
  gchar *nom_sous_categ;
};




/* devises */

struct struct_devise
{
  gint no_devise;
  gchar *nom_devise;
  gchar *code_devise;
  gint passage_euro;                                  /* � 1 si cette devise doit passer � l'euro */

  GDate *date_dernier_change;                     /*   derni�re mise � jour du change, NULL si aucun change */
  gint une_devise_1_egale_x_devise_2;       /* si = 1 : 1 nom_devise = "change" devise_en_rapport */
  gint no_devise_en_rapport;
  gdouble change;                                          /* taux de change */
};

#define NOM_DEVISE_ASSOCIEE GTK_LABEL (GTK_BIN ( GTK_BUTTON ( &(GTK_OPTION_MENU (option_menu_devises) -> button ))) -> child ) -> label


/* banques */

struct struct_banque
{
  gint no_banque;
  gchar *code_banque;
  gchar *nom_banque;
  gchar *adr_banque;
  gchar *tel_banque;
  gchar *email_banque;
  gchar *web_banque;
  gchar *remarque_banque;
  gchar *nom_correspondant;
  gchar *tel_correspondant;
  gchar *email_correspondant;
  gchar *fax_correspondant;
};


struct struct_operation_qif
{
  gchar *date;

  gchar *tiers;
  gchar *categ;
  gchar *notes;

  gdouble montant;
  gulong cheque;
  gint operation_ventilee;

  gint p_r;

  gint ope_de_ventilation;
};



struct struct_exercice
{
  gint no_exercice;
  gchar *nom_exercice;
  GDate *date_debut;
  GDate *date_fin;
  gint affiche_dans_formulaire;
};

struct struct_type_ope
{
  gint no_type;
  gchar *nom_type;
  gint signe_type;   /* 0=neutre, 1=d�bit, 2=cr�dit */
  gint affiche_entree;    /* par ex pour virement, ch�ques ... */
  gint numerotation_auto;        /* pour les ch�ques */
  gulong no_en_cours;
  gint no_compte;
};

struct struct_imputation
{
  gint no_imputation;
  gchar *nom_imputation;
  gint type_imputation;                   /* 0=cr�dit ; 1 = d�bit */
  gint no_derniere_sous_imputation;
  GSList *liste_sous_imputation;
};

struct struct_sous_imputation
{
  gint no_sous_imputation;
  gchar *nom_sous_imputation;
};


/* la struct_ope_liee est utilis�e pour le passage de la 0.3.1 � la 0.3.2 */
/* pour transformer les no d'op�s */
/* � virer ensuite */

struct struct_ope_liee
{
  gint ancien_no_ope;
  gint compte_ope;
  gint nouveau_no_ope;
  struct structure_operation *ope_liee;
  gint ope_ventilee;
};

/* si en 2 parties, s�par� d'un - */
/* si la 2�me est un num�ro, elle est incr�ment�e � chaque �quilibrage */

struct struct_no_rapprochement
{
  gint no_rapprochement;
  gchar *nom_rapprochement;
};


struct struct_etat
{
  gint no_etat;
  gchar *nom_etat;

  gint afficher_r;         /* 0=ttes les op�s, 1=que les op�s non R, 2=que les op�s R */
  gint afficher_opes;
  gint afficher_nb_opes;
  gint afficher_date_ope;
  gint afficher_tiers_ope;
  gint afficher_categ_ope;
  gint afficher_sous_categ_ope;
  gint afficher_ib_ope;
  gint afficher_sous_ib_ope;
  gint afficher_notes_ope;
  gint afficher_pc_ope;
  gint afficher_infobd_ope;
  gint afficher_no_ope;
  gint afficher_type_ope;
  gint afficher_cheque_ope;
  gint afficher_rappr_ope;
  gint afficher_exo_ope;
  gint pas_detailler_ventilation;
  gint separer_revenus_depenses;
  gint devise_de_calcul_general;
  gint afficher_titre_colonnes;
  gint type_affichage_titres;        /* 0 = en haut, 1 = � chaque changement de section */
  gint inclure_dans_tiers;        /* � 1 si on veut que cet �tat apparaisse dans la liste des tiers du formulaire */

  gint exo_date;         /* 1-> utilise l'exo / 0 -> utilise une plage de date */
  gint utilise_detail_exo;   /* 0=tous, 1=exercice courant, 2=exercice pr�c�dent, 3=perso */
  GSList *no_exercices;            /* liste des no d'exos utilis�s dans l'�tat */
  gint separation_par_exo;       /* 1=oui, 0=non */
  gint no_plage_date;       /* 0=perso, 1=toutes ... */
  GDate *date_perso_debut;
  GDate *date_perso_fin;
  gint separation_par_plage;       /* 1=oui, 0=non */
  gint type_separation_plage;        /*  0=semaines, 1=mois, 2=ann�e */
  gint jour_debut_semaine;           /* 0=lundi ... */

  GList *type_classement;  /* liste de no : 1=cat�,2=ss categ,3=ib,4=ss ib,5=compte,6=tiers */

  gint utilise_detail_comptes;
  GSList *no_comptes;
  gint regroupe_ope_par_compte;
  gint affiche_sous_total_compte;
  gint afficher_nom_compte;

  gint type_virement;   /* 0: pas de virements / 1:seulement les virements vers comptes actifs-passifs/2:virements hors �tat/3:perso */
  GSList *no_comptes_virements;
  gint exclure_ope_non_virement;

  gint utilise_categ;
  gint utilise_detail_categ;
  GSList *no_categ;
  gint afficher_sous_categ;
  gint affiche_sous_total_categ;
  gint affiche_sous_total_sous_categ;
  gint exclure_ope_sans_categ;
  gint devise_de_calcul_categ;
  gint afficher_pas_de_sous_categ;
  gint afficher_nom_categ;

  gint utilise_ib;
  gint utilise_detail_ib;
  GSList *no_ib;
  gint afficher_sous_ib;
  gint affiche_sous_total_ib;
  gint affiche_sous_total_sous_ib;
  gint exclure_ope_sans_ib;
  gint devise_de_calcul_ib;
  gint afficher_pas_de_sous_ib;
  gint afficher_nom_ib;

  gint utilise_tiers;
  gint utilise_detail_tiers;
  GSList *no_tiers;
  gint affiche_sous_total_tiers;
  gint devise_de_calcul_tiers;
  gint afficher_nom_tiers;

  gchar *texte;

  gint utilise_montant_neg_pos;
  gint type_neg_pos;               /* 1=positif, 0=n�gatif */
  gint utilise_valeur;
  gint type_operateur_valeur;     /* 0=, 1<, 2<=, 3>, 4>= */
  gfloat montant_valeur;
  gint utilise_inclusion;
  gfloat montant_inclusion_inf;
  gint type_operateur_inf_inclusion;   /* 0<, 1<= */
  gint type_operateur_sup_inclusion;    /* 0<, 1<= */
  gfloat montant_inclusion_sup;
  gint choix_montant_nul;     /* 1=exclu, 0=pos et neg, 2=neg, 3=pos */
  gint choix_devise_montant;
};


struct struct_etat_affichage
{
  gint (* init) ( );
  gint (* afficher_titre) ( gint );
  gint (* afficher_separateur) ( gint );
  gint (* affiche_total_categories) ( gint ligne );
  gint (* affiche_total_sous_categ) ( gint ligne );
  gint (* affiche_total_ib) ( gint ligne );
  gint (* affiche_total_sous_ib) ( gint ligne );
  gint (* affiche_total_compte) ( gint ligne );
  gint (* affiche_total_tiers) ( gint ligne );
  gint (* affichage_ligne_ope) ( struct structure_operation *operation,
				 gint ligne );
  gint (* affiche_total_partiel) ( gdouble total_partie,
				   gint ligne,
				   gint type );
  gint (* affiche_total_general) ( gdouble total_general,
				   gint ligne );
  gint (* affiche_categ_etat) ( struct structure_operation *operation,
				gchar *decalage_categ,
				gint ligne );
  gint (* affiche_sous_categ_etat) ( struct structure_operation *operation,
				     gchar *decalage_sous_categ,
				     gint ligne );
  gint (* affiche_ib_etat) ( struct structure_operation *operation,
			     gchar *decalage_ib,
			     gint ligne );
  gint (* affiche_sous_ib_etat) ( struct structure_operation *operation,
				  gchar *decalage_sous_ib,
				  gint ligne );
  gint (* affiche_compte_etat) ( struct structure_operation *operation,
				 gchar *decalage_compte,
				 gint ligne );
  gint (* affiche_tiers_etat) ( struct structure_operation *operation,
				gchar *decalage_tiers,
				gint ligne );
  gint (* affiche_titre_revenus_etat) ( gint ligne );
  gint (* affiche_titre_depenses_etat) ( gint ligne );
  gint (* affiche_totaux_sous_jaccent) ( gint origine,
					 gint ligne );
  gint (* affiche_titres_colonnes) ( gint ligne );
  gint (* finish) ( );
};




/* struture d'une op� de ventil */

struct struct_ope_ventil
{
  guint no_operation;

  gdouble montant;

  guint categorie;
  guint sous_categorie;

  gchar *notes;

  guint imputation;
  guint sous_imputation;
  gchar *no_piece_comptable;

  guint relation_no_operation;      /* contient le no de l'op� associ�e quand elle est d�j� cr��e, -1 si c'est un virement non cr�� */
  guint relation_no_compte;
  gint no_type_associe;

  gint pointe;

  gint supprime;   /* � 1 quand cette op� doit �tre supprim�e */
};

