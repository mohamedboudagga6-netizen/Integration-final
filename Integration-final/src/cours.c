#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "cours.h"

/* Définir un préfixe pour les logs */
#define LOG_PREFIX "[COURS] "

/* Créer un nouveau cours */
Cours* cours_new(void) {
    return g_new0(Cours, 1);
}

/* Libérer la mémoire d'un cours */
void cours_free(Cours *cours) {
    if (cours) {
        g_free(cours->id);
        g_free(cours->nom);
        g_free(cours->type);
        g_free(cours->niveau);
        g_free(cours->entraineur);
        g_free(cours->date);
        g_free(cours->description);
        g_free(cours->visibilite);
        g_free(cours);
    }
}

/* Libérer la mémoire d'un cours (pour GList) */
void cours_free_func(gpointer data, gpointer user_data) {
    cours_free((Cours*)data);
}

/* Charger tous les cours depuis le fichier */
GList* cours_charger_tous(const gchar *filename) {
    fprintf(stderr, "[DEBUG] cours_charger_tous: Ouverture de '%s'\n", filename);
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "[WARNING] Impossible d'ouvrir '%s' (fichier n'existe pas encore?)\n", filename);
        /* Le fichier n'existe pas encore, retourner une liste vide */
        return NULL;
    }
    fprintf(stderr, "[DEBUG] Fichier ouvert avec succès\n");
    
    GList *liste_cours = NULL;
    char ligne[1024];
    Cours *cours = NULL;
    
    while (fgets(ligne, sizeof(ligne), f)) {
        /* Supprimer le saut de ligne */
        size_t len = strlen(ligne);
        if (len > 0 && ligne[len-1] == '\n') {
            ligne[len-1] = '\0';
        }
        
        if (strncmp(ligne, "COURS:", 6) == 0) {
            if (cours) {
                liste_cours = g_list_append(liste_cours, cours);
            }
            cours = cours_new();
        } else if (strncmp(ligne, "END", 3) == 0) {
            if (cours) {
                liste_cours = g_list_append(liste_cours, cours);
                cours = NULL;
            }
        } else if (cours) {
            gchar *value = NULL;
            if (strncmp(ligne, "ID:", 3) == 0) {
                value = g_strstrip(ligne + 3);
                cours->id = g_strdup(value);
            } else if (strncmp(ligne, "NOM:", 4) == 0) {
                value = g_strstrip(ligne + 4);
                cours->nom = g_strdup(value);
            } else if (strncmp(ligne, "TYPE:", 5) == 0) {
                value = g_strstrip(ligne + 5);
                cours->type = g_strdup(value);
            } else if (strncmp(ligne, "NIVEAU:", 7) == 0) {
                value = g_strstrip(ligne + 7);
                cours->niveau = g_strdup(value);
            } else if (strncmp(ligne, "ENTRAINEUR:", 11) == 0) {
                value = g_strstrip(ligne + 11);
                cours->entraineur = g_strdup(value);
            } else if (strncmp(ligne, "DATE:", 5) == 0) {
                value = g_strstrip(ligne + 5);
                cours->date = g_strdup(value);
            } else if (strncmp(ligne, "DUREE:", 6) == 0) {
                cours->duree = atoi(ligne + 6);
            } else if (strncmp(ligne, "CAPACITE:", 9) == 0) {
                cours->capacite = atoi(ligne + 9);
            } else if (strncmp(ligne, "PRIX:", 5) == 0) {
                cours->prix = atof(ligne + 5);
            } else if (strncmp(ligne, "DESCRIPTION:", 12) == 0) {
                value = g_strstrip(ligne + 12);
                cours->description = g_strdup(value);
            } else if (strncmp(ligne, "VISIBILITE:", 11) == 0) {
                value = g_strstrip(ligne + 11);
                cours->visibilite = g_strdup(value);
            }
        }
    }
    
    if (cours) {
        liste_cours = g_list_append(liste_cours, cours);
    }
    
    fclose(f);
    guint nb_cours = liste_cours ? g_list_length(liste_cours) : 0;
    fprintf(stderr, "[DEBUG] cours_charger_tous: %u cours(s) chargé(s)\n", nb_cours);
    return liste_cours;
}

/* Sauvegarder tous les cours dans le fichier */
gboolean cours_sauvegarder_tous(GList *liste_cours, const gchar *filename) {
    guint nb_cours = liste_cours ? g_list_length(liste_cours) : 0;
    fprintf(stderr, "[DEBUG] cours_sauvegarder_tous: Sauvegarde de %u cours(s) dans '%s'\n", nb_cours, filename);
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "[ERROR] Impossible d'ouvrir '%s' en écriture\n", filename);
        return FALSE;
    }
    fprintf(stderr, "[DEBUG] Fichier ouvert en écriture\n");
    
    GList *l;
    for (l = liste_cours; l != NULL; l = l->next) {
        Cours *cours = (Cours *)l->data;
        if (!cours) continue;
        
        fprintf(f, "COURS:\n");
        fprintf(f, "ID: %s\n", cours->id ? cours->id : "");
        fprintf(f, "NOM: %s\n", cours->nom ? cours->nom : "");
        fprintf(f, "TYPE: %s\n", cours->type ? cours->type : "");
        fprintf(f, "NIVEAU: %s\n", cours->niveau ? cours->niveau : "");
        fprintf(f, "ENTRAINEUR: %s\n", cours->entraineur ? cours->entraineur : "");
        fprintf(f, "DATE: %s\n", cours->date ? cours->date : "");
        fprintf(f, "DUREE: %d\n", cours->duree);
        fprintf(f, "CAPACITE: %d\n", cours->capacite);
        fprintf(f, "PRIX: %.2f\n", cours->prix);
        fprintf(f, "DESCRIPTION: %s\n", cours->description ? cours->description : "");
        fprintf(f, "VISIBILITE: %s\n", cours->visibilite ? cours->visibilite : "");
        fprintf(f, "END\n");
    }
    
    fclose(f);
    fprintf(stderr, "[DEBUG] cours_sauvegarder_tous: Sauvegarde réussie\n");
    return TRUE;
}

/* Ajouter un cours à la liste */
GList* cours_ajouter(GList *liste_cours, Cours *cours) {
    if (!cours) {
        return liste_cours;
    }
    return g_list_append(liste_cours, cours);
}

/* Supprimer un cours de la liste par ID */
GList* cours_supprimer_par_id(GList *liste_cours, const gchar *id) {
    if (!id) {
        return liste_cours;
    }
    
    GList *l;
    for (l = liste_cours; l != NULL; l = l->next) {
        Cours *cours = (Cours *)l->data;
        if (cours && cours->id && strcmp(cours->id, id) == 0) {
            cours_free(cours);
            return g_list_delete_link(liste_cours, l);
        }
    }
    
    return liste_cours;
}

/* Trouver un cours par ID */
Cours* cours_trouver_par_id(GList *liste_cours, const gchar *id) {
    if (!id) {
        return NULL;
    }
    
    GList *l;
    for (l = liste_cours; l != NULL; l = l->next) {
        Cours *cours = (Cours *)l->data;
        if (cours && cours->id && strcmp(cours->id, id) == 0) {
            return cours;
        }
    }
    
    return NULL;
}

/* Vérifier si un ID existe déjà */
gboolean cours_id_existe(GList *liste_cours, const gchar *id) {
    return cours_trouver_par_id(liste_cours, id) != NULL;
}

/* Rechercher des cours par nom (recherche partielle) */
GList* cours_rechercher_par_nom(GList *liste_cours, const gchar *nom) {
    if (!nom || strlen(nom) == 0) {
        return liste_cours;
    }
    
    GList *resultats = NULL;
    GList *l;
    
    for (l = liste_cours; l != NULL; l = l->next) {
        Cours *cours = (Cours *)l->data;
        if (cours && cours->nom && strstr(cours->nom, nom) != NULL) {
            resultats = g_list_append(resultats, cours);
        }
    }
    
    return resultats;
}

/* Obtenir le nombre de cours */
guint cours_get_count(GList *liste_cours) {
    return g_list_length(liste_cours);
}

/* Libérer toute la liste de cours */
void cours_liste_free(GList *liste_cours) {
    g_list_foreach(liste_cours, cours_free_func, NULL);
    g_list_free(liste_cours);
}

/* Initialiser le treeview avec les colonnes */
void cours_treeview_init(GtkWidget *treeview) {
    if (!treeview || !GTK_IS_TREE_VIEW(treeview)) {
        return;
    }
    
    /* Vérifier si le treeview a déjà un modèle */
    GtkTreeModel *existing_model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
    if (existing_model) {
        /* Déjà initialisé */
        return;
    }
    
    GtkTreeStore *store = gtk_tree_store_new(9, 
                                             G_TYPE_STRING,  /* ID */
                                             G_TYPE_STRING,  /* Nom */
                                             G_TYPE_STRING,  /* Type */
                                             G_TYPE_STRING,  /* Niveau */
                                             G_TYPE_STRING,  /* Entraineur */
                                             G_TYPE_STRING,  /* Date */
                                             G_TYPE_INT,     /* Durée */
                                             G_TYPE_INT,     /* Capacité */
                                             G_TYPE_STRING); /* Prix (affiché comme string) */
    
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    /* Créer les colonnes */
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    /* Colonne ID */
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "ID");
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 0);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    /* Colonne Nom */
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Nom");
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 1);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    /* Colonne Type */
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Type");
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 2);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    /* Colonne Niveau */
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Niveau");
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 3);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    /* Colonne Entraineur */
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Entraineur");
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 4);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    /* Colonne Date */
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Date");
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 5);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    /* Colonne Durée */
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Durée");
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 6);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    /* Colonne Capacité */
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Capacité");
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 7);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    /* Colonne Prix */
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Prix");
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 8);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

/* Mettre à jour le treeview avec la liste des cours */
void cours_treeview_update(GtkWidget *treeview, GList *liste_cours) {
    fprintf(stderr, "[DEBUG] cours_treeview_update: Début\n");
    
    if (!treeview || !GTK_IS_TREE_VIEW(treeview)) {
        fprintf(stderr, "[ERROR] Treeview invalide\n");
        return;
    }
    
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
    if (!model || !GTK_IS_TREE_STORE(model)) {
        fprintf(stderr, "[DEBUG] Initialisation du treeview\n");
        cours_treeview_init(treeview);
        model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
    }
    
    GtkTreeStore *store = GTK_TREE_STORE(model);
    gtk_tree_store_clear(store);
    
    guint count = 0;
    GList *l;
    for (l = liste_cours; l != NULL; l = l->next) {
        Cours *cours = (Cours *)l->data;
        if (!cours) continue;
        
        GtkTreeIter iter;
        gchar duree_str[32], capacite_str[32], prix_str[32];
        
        sprintf(duree_str, "%d", cours->duree);
        sprintf(capacite_str, "%d", cours->capacite);
        sprintf(prix_str, "%.2f", cours->prix);
        
        gtk_tree_store_append(store, &iter, NULL);
        gtk_tree_store_set(store, &iter,
                          0, cours->id ? cours->id : "",
                          1, cours->nom ? cours->nom : "",
                          2, cours->type ? cours->type : "",
                          3, cours->niveau ? cours->niveau : "",
                          4, cours->entraineur ? cours->entraineur : "",
                          5, cours->date ? cours->date : "",
                          6, cours->duree,
                          7, cours->capacite,
                          8, prix_str,
                          -1);
        count++;
    }
    fprintf(stderr, "[DEBUG] cours_treeview_update: %u cours(s) ajouté(s) au treeview\n", count);
}

/* Filtrer le treeview avec une liste de cours */
void cours_treeview_filter(GtkWidget *treeview, GList *liste_cours) {
    cours_treeview_update(treeview, liste_cours);
}

