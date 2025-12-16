#ifndef COURS_H
#define COURS_H

#include <gtk/gtk.h>
#include <glib.h>

/* Structure pour représenter un cours */
typedef struct {
    gchar *id;              /* Identifiant unique du cours */
    gchar *nom;             /* Nom du cours */
    gchar *type;            /* Type de cours (Cardio, Musculation, etc.) */
    gchar *niveau;          /* Niveau (Débutant, Intermédiaire, Avancé) */
    gchar *entraineur;      /* Nom de l'entraîneur */
    gchar *date;            /* Date du cours (format: JJ/MM/AAAA) */
    gint duree;             /* Durée en minutes */
    gint capacite;          /* Capacité maximale */
    gdouble prix;           /* Prix du cours */
    gchar *description;     /* Description détaillée */
    gchar *visibilite;      /* Visibilité (Public, Privé, Membres seulements) */
} Cours;

/* Fonctions de gestion des cours */

/* Créer un nouveau cours */
Cours* cours_new(void);

/* Libérer la mémoire d'un cours */
void cours_free(Cours *cours);

/* Libérer la mémoire d'un cours (pour GList) */
void cours_free_func(gpointer data, gpointer user_data);

/* Charger tous les cours depuis le fichier */
GList* cours_charger_tous(const gchar *filename);

/* Sauvegarder tous les cours dans le fichier */
gboolean cours_sauvegarder_tous(GList *liste_cours, const gchar *filename);

/* Ajouter un cours à la liste */
GList* cours_ajouter(GList *liste_cours, Cours *cours);

/* Supprimer un cours de la liste par ID */
GList* cours_supprimer_par_id(GList *liste_cours, const gchar *id);

/* Trouver un cours par ID */
Cours* cours_trouver_par_id(GList *liste_cours, const gchar *id);

/* Vérifier si un ID existe déjà */
gboolean cours_id_existe(GList *liste_cours, const gchar *id);

/* Rechercher des cours par nom (recherche partielle) */
GList* cours_rechercher_par_nom(GList *liste_cours, const gchar *nom);

/* Obtenir le nombre de cours */
guint cours_get_count(GList *liste_cours);

/* Libérer toute la liste de cours */
void cours_liste_free(GList *liste_cours);

/* Fonctions pour le treeview */

/* Initialiser le treeview avec les colonnes */
void cours_treeview_init(GtkWidget *treeview);

/* Mettre à jour le treeview avec la liste des cours */
void cours_treeview_update(GtkWidget *treeview, GList *liste_cours);

/* Filtrer le treeview avec une liste de cours */
void cours_treeview_filter(GtkWidget *treeview, GList *liste_cours);

#endif /* COURS_H */

