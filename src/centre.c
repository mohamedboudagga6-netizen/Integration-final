#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "centre.h"


void Ajouter(const char *id,const char *nom,const char *adresse,const char *tel,const char *email,const char *type,int capacite,const char *public_vise,const char *description)
{
    FILE *f = fopen("centre.txt", "a"); 

    if (f == NULL)
    {
        printf("Erreur : impossible d'ouvrir centre.txt\n");
        return;
    }

    fprintf(f, "%s\t%s\t%s\t%s\t%s\t%s\t%d\t%s\t%s\n",id,nom,adresse,tel,email,type,capacite,public_vise,description);
    fclose(f);
}
Centre Chercher(const char *id) {
    Centre c;
    FILE *fichier = fopen("centre.txt", "r");
    
    strcpy(c.id, "");
    strcpy(c.nom, "");
    strcpy(c.adresse, "");
    strcpy(c.tel, "");
    strcpy(c.email, "");
    strcpy(c.type, "");
    c.capacite = 0;
    strcpy(c.public_vise, "");
    strcpy(c.description, "");
    
    if (fichier == NULL) {
        printf("Erreur: Impossible d'ouvrir centre.txt\n");
        return c;
    }
    
    char ligne[1024];
    
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        Centre temp;
        
        strcpy(temp.description, "");
        
        int nb_champs = sscanf(ligne, "%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%d\t%[^\t]\t%[^\n]",
                   temp.id, temp.nom, temp.adresse, temp.tel, temp.email,
                   temp.type, &temp.capacite, temp.public_vise, temp.description);
        
        if (nb_champs >= 8) {
            if (strcmp(temp.id, id) == 0) {
                c = temp;
                fclose(fichier);
                return c;
            }
        }
    }
    
    fclose(fichier);
    return c;
}
int Modifier(const char *id, const char *nom, const char *adresse, const char *tel,
             const char *email, const char *type, int capacite,
             const char *public_vise, const char *description) 
{
    FILE *fichier = fopen("centre.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    
    if (fichier == NULL || temp == NULL) {
        printf("Erreur: Impossible d'ouvrir les fichiers\n");
        if (fichier) fclose(fichier);
        if (temp) fclose(temp);
        return 0;
    }
    
    char ligne[1024];
    int trouve = 0;
    
    // Parcourir le fichier ligne par ligne
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        Centre c;
        
        // Lire la ligne actuelle
        if (sscanf(ligne, "%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%d\t%[^\t]\t%[^\n]",
                   c.id, c.nom, c.adresse, c.tel, c.email,
                   c.type, &c.capacite, c.public_vise, c.description) >= 9) {
            
            // Si c'est l'ID à modifier
            if (strcmp(c.id, id) == 0) {
                // Écrire les nouvelles données
                fprintf(temp, "%s\t%s\t%s\t%s\t%s\t%s\t%d\t%s\t%s\n",
                        id, nom, adresse, tel, email, type, capacite, public_vise, description);
                trouve = 1;
            } else {
                // Garder la ligne telle quelle
                fprintf(temp, "%s", ligne);
            }
        } else {
            // Ligne mal formatée, la garder
            fprintf(temp, "%s", ligne);
        }
    }
    
    fclose(fichier);
    fclose(temp);
    
    // Remplacer l'ancien fichier par le nouveau
    if (trouve) {
        remove("centre.txt");
        rename("temp.txt", "centre.txt");
        printf("Centre modifié avec succès!\n");
        return 1;
    } else {
        remove("temp.txt");
        printf("ID non trouvé, modification impossible\n");
        return 0;
    }
}
int Rechercher_Centre(GtkWidget *treeview, const char *id) {
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    gchar *current_id;
    int trouve = 0;
    
    if (treeview == NULL || id == NULL) {
        printf("ERREUR: TreeView ou ID NULL dans Rechercher_Centre\n");
        return 0;
    }
    
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
    if (model == NULL) {
        printf("ERREUR: Aucun modèle associé au TreeView\n");
        return 0;
    }
    
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    
    
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            gtk_tree_model_get(model, &iter, 0, &current_id, -1);
            
            
            if (current_id != NULL && strcmp(g_strstrip(current_id), id) == 0) {
                
                gtk_tree_selection_select_iter(selection, &iter);
                
                
                GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
                gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(treeview), path, NULL, TRUE, 0.5, 0.0);
                gtk_tree_path_free(path);
                
                g_free(current_id);
                trouve = 1;
                break;
            }
            
            g_free(current_id);
        } while (gtk_tree_model_iter_next(model, &iter));
    }
    
    if (!trouve) {
       
        gtk_tree_selection_unselect_all(selection);
    }
    
    return trouve;
}
int Supprimer(const char *id) {
    FILE *fichier = fopen("centre.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    
    if (fichier == NULL || temp == NULL) {
        printf("Erreur: Impossible d'ouvrir les fichiers\n");
        if (fichier) fclose(fichier);
        if (temp) fclose(temp);
        return 0;
    }
    
    char ligne[1024];
    int trouve = 0;
    
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        Centre c;
        
      
        if (sscanf(ligne, " %[^\t]\t %[^\t]\t %[^\t]\t %[^\t]\t %[^\t]\t %[^\t]\t %d\t %[^\t]\t %[^\n]",
                   c.id, c.nom, c.adresse, c.tel, c.email,
                   c.type, &c.capacite, c.public_vise, c.description) >= 7) {
            
            // Si c'est l'ID à supprimer, ne pas l'écrire dans temp
            if (strcmp(c.id, id) == 0) {
                trouve = 1;
                printf("Centre '%s' supprimé\n", id);
            } else {
                
                fprintf(temp, "%s", ligne);
            }
        } else {
            
            fprintf(temp, "%s", ligne);
        }
    }
    
    fclose(fichier);
    fclose(temp);
    
    // Remplacer l'ancien fichier par le nouveau
    if (trouve) {
        remove("centre.txt");
        rename("temp.txt", "centre.txt");
        return 1;
    } else {
        remove("temp.txt");
        printf("ID '%s' non trouvé\n", id);
        return 0;
    }
}
//////////////////fonction de treeview //////////////////////////////////
void Afficher_Centres(GtkWidget *treeview) {
    GtkListStore *store;
    GtkTreeIter iter;
    FILE *f;
    char ligne[1024];
    Centre c;
    int count = 0;
    
    printf("=== Début Afficher_Centres ===\n");
    
    if (treeview == NULL) {
        printf("ERREUR: treeview est NULL!\n");
        return;
    }
    
    store = gtk_list_store_new(7, 
                               G_TYPE_STRING,  
                               G_TYPE_STRING,  
                               G_TYPE_STRING,  
                               G_TYPE_STRING,  
                               G_TYPE_STRING, 
                               G_TYPE_INT,     
                               G_TYPE_STRING); 
    
    printf("ListStore créé avec 7 colonnes\n");
    
    f = fopen("centre.txt", "r");
    if (f == NULL) {
        printf("ERREUR: Impossible d'ouvrir centre.txt\n");
        gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
        g_object_unref(store);
        return;
    }
    
    printf("Fichier centre.txt ouvert avec succès\n");
    
    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        if (sscanf(ligne, " %[^\t]\t %[^\t]\t %[^\t]\t %[^\t]\t %[^\t]\t %[^\t]\t %d\t %[^\t]\t %[^\n]",
                   c.id, c.nom, c.adresse, c.tel, c.email,
                   c.type, &c.capacite, c.public_vise, c.description) >= 7) {
            
            printf("  Centre: ID='%s', Nom='%s', Tel='%s', Email='%s', Type='%s', Capacité=%d, Public='%s'\n", 
                   c.id, c.nom, c.tel, c.email, c.type, c.capacite, c.public_vise);
            
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                              0, c.id,
                              1, c.nom,
                              2, c.tel,
                              3, c.email,
                              4, c.type,
                              5, c.capacite,
                              6, c.public_vise,
                              -1);
            count++;
        } else {
            printf("  Ligne ignorée: %s", ligne);
        }
    }
    
    fclose(f);
    
    printf("Nombre total de centres chargés: %d\n", count);
    
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    printf("=== Fin Afficher_Centres ===\n");
}
void Ajouter_Coach_Centre(const char *nom,const char *prenom,const char *date_nai,const char *tel,const char *email,const char *adresse,const char * cin,const char *specialite,const char *disponibilite,const char *contrat){

 FILE *f = fopen("InscriCoach.txt", "a"); 

    if (f == NULL)
    {
        printf("Erreur : impossible d'ouvrir InscriCoach.txt\n");
        return;
    }

    fprintf(f, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",nom,prenom,date_nai,tel,email,adresse,cin,specialite,disponibilite,contrat);
    fclose(f);
}


