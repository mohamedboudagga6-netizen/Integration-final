#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <ctype.h>
#include "evenement.h"

/* ============================================= */
/*               EVENT CRUD FUNCTIONS             */
/* ============================================= */

int get_next_event_id() {
    evenement ev;
    int max_id = 0;
    FILE *f = fopen("evenement.txt", "r");
    if (f) {
        while (fscanf(f, "%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\t%19[^\n]\n",
                      &ev.id_event, ev.nom_event, ev.type_event, ev.centre, ev.salle, ev.description,
                      &ev.date_e.j, &ev.date_e.m, &ev.date_e.a,
                      &ev.heure_e.heure, &ev.heure_e.minute, &ev.prix, ev.statut) != EOF) {
            if (ev.id_event > max_id) {
                max_id = ev.id_event;
            }
        }
        fclose(f);
    }
    return max_id + 1;
}

void ajouter_evenement(evenement ev) {
    FILE *f = fopen("evenement.txt", "a");
    if (!f) return;
    ev.id_event = get_next_event_id();
    fprintf(f, "%d\t%s\t%s\t%s\t%s\t%s\t%02d/%02d/%04d\t%02d:%02d\t%.2f\t%s\n",
            ev.id_event,
            ev.nom_event,
            ev.type_event,
            ev.centre,
            ev.salle,
            ev.description,
            ev.date_e.j, ev.date_e.m, ev.date_e.a,
            ev.heure_e.heure, ev.heure_e.minute,
            ev.prix,
            ev.statut);
    fflush(f);
    fclose(f);
}

int evenement_existe(int id_event) {
    evenement ev;
    FILE *f = fopen("evenement.txt", "r");
    if (!f) return 0;
    while (fscanf(f, "%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\t%19[^\n]\n",
                  &ev.id_event, ev.nom_event, ev.type_event, ev.centre, ev.salle, ev.description,
                  &ev.date_e.j, &ev.date_e.m, &ev.date_e.a,
                  &ev.heure_e.heure, &ev.heure_e.minute, &ev.prix, ev.statut) != EOF) {
        if (ev.id_event == id_event) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

void supprimer_evenement(int id_event) {
    evenement ev;
    FILE *f = fopen("evenement.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!f || !temp) { if(f) fclose(f); if(temp) fclose(temp); return; }

    while (fscanf(f, "%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\t%19[^\n]\n",
                  &ev.id_event, ev.nom_event, ev.type_event, ev.centre, ev.salle, ev.description,
                  &ev.date_e.j, &ev.date_e.m, &ev.date_e.a,
                  &ev.heure_e.heure, &ev.heure_e.minute, &ev.prix, ev.statut) != EOF) {
        if (ev.id_event != id_event) {
            fprintf(temp, "%d\t%s\t%s\t%s\t%s\t%s\t%02d/%02d/%04d\t%02d:%02d\t%.2f\t%s\n",
                    ev.id_event, ev.nom_event, ev.type_event, ev.centre, ev.salle, ev.description,
                    ev.date_e.j, ev.date_e.m, ev.date_e.a,
                    ev.heure_e.heure, ev.heure_e.minute, ev.prix, ev.statut);
        }
    }
    fclose(f); fclose(temp);
    remove("evenement.txt");
    rename("temp.txt", "evenement.txt");
}

void modifier_evenement(evenement ev) {
    evenement tmp;
    FILE *f = fopen("evenement.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!f || !temp) { if(f) fclose(f); if(temp) fclose(temp); return; }

    while (fscanf(f, "%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\t%19[^\n]\n",
                  &tmp.id_event, tmp.nom_event, tmp.type_event, tmp.centre, tmp.salle, tmp.description,
                  &tmp.date_e.j, &tmp.date_e.m, &tmp.date_e.a,
                  &tmp.heure_e.heure, &tmp.heure_e.minute, &tmp.prix, tmp.statut) != EOF) {
        if (tmp.id_event == ev.id_event) {
            fprintf(temp, "%d\t%s\t%s\t%s\t%s\t%s\t%02d/%02d/%04d\t%02d:%02d\t%.2f\t%s\n",
                    ev.id_event, ev.nom_event, ev.type_event, ev.centre, ev.salle, ev.description,
                    ev.date_e.j, ev.date_e.m, ev.date_e.a,
                    ev.heure_e.heure, ev.heure_e.minute, ev.prix, ev.statut);
        } else {
            fprintf(temp, "%d\t%s\t%s\t%s\t%s\t%s\t%02d/%02d/%04d\t%02d:%02d\t%.2f\t%s\n",
                    tmp.id_event, tmp.nom_event, tmp.type_event, tmp.centre, tmp.salle, tmp.description,
                    tmp.date_e.j, tmp.date_e.m, tmp.date_e.a,
                    tmp.heure_e.heure, tmp.heure_e.minute, tmp.prix, tmp.statut);
        }
    }
    fclose(f); fclose(temp);
    remove("evenement.txt");
    rename("temp.txt", "evenement.txt");
}

evenement trouver_evenement(int id_event) {
    evenement ev = {0};
    FILE *f = fopen("evenement.txt", "r");
    if (!f) return ev;

    while (fscanf(f, "%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\t%19[^\n]\n",
                  &ev.id_event, ev.nom_event, ev.type_event, ev.centre, ev.salle, ev.description,
                  &ev.date_e.j, &ev.date_e.m, &ev.date_e.a,
                  &ev.heure_e.heure, &ev.heure_e.minute, &ev.prix, ev.statut) != EOF) {
        if (ev.id_event == id_event) {
            fclose(f);
            return ev;
        }
    }
    fclose(f);
    return ev;
}

/* ============================================= */
/*                DISPLAY & SEARCH               */
/* ============================================= */

enum {
    COL_ID_EVENT,
    COL_NOM_EVENT,
    COL_TYPE_EVENT,
    COL_CENTRE,
    COL_SALLE,
    COL_DATE_EVENT,
    COL_HEURE_EVENT,
    COL_PRIX,
    COL_DESCRIPTION_EVENT,
    COL_STATUT,
    COL_COUNT_EVENT
};

void afficher_evenements(GtkWidget *treeview) {
    GtkListStore *store;
    GtkTreeIter iter;
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
    if (!store) {
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "ID", renderer, "text", COL_ID_EVENT, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Nom", renderer, "text", COL_NOM_EVENT, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Type", renderer, "text", COL_TYPE_EVENT, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Centre", renderer, "text", COL_CENTRE, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Salle", renderer, "text", COL_SALLE, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Date", renderer, "text", COL_DATE_EVENT, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Heure", renderer, "text", COL_HEURE_EVENT, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Prix", renderer, "text", COL_PRIX, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Description", renderer, "text", COL_DESCRIPTION_EVENT, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Statut", renderer, "text", COL_STATUT, NULL);

        store = gtk_list_store_new(COL_COUNT_EVENT,
                                   G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                   G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                   G_TYPE_STRING, G_TYPE_STRING);
        gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
        g_object_unref(store);
    }
    gtk_list_store_clear(store);

    FILE *f = fopen("evenement.txt", "r");
    if (!f) return;

    int id, j, m, a, h, min;
    float prix;
    char nom[300], type[100], centre[200], salle[100], desc[1000], statut[20];
    char date_str[12], heure_str[6], prix_str[20], desc_short[104];

    while (fscanf(f, "%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\t%19[^\n]\n",
                  &id, nom, type, centre, salle, desc, &j, &m, &a, &h, &min, &prix, statut) != EOF) {
        snprintf(date_str, sizeof(date_str), "%02d/%02d/%04d", j, m, a);
        snprintf(heure_str, sizeof(heure_str), "%02d:%02d", h, min);
        snprintf(prix_str, sizeof(prix_str), "%.2f DT", prix);
        strncpy(desc_short, desc, 100);
        desc_short[100] = '\0';
        if (strlen(desc) > 100) strcat(desc_short, "...");

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           COL_ID_EVENT, id,
                           COL_NOM_EVENT, nom,
                           COL_TYPE_EVENT, type,
                           COL_CENTRE, centre,
                           COL_SALLE, salle,
                           COL_DATE_EVENT, date_str,
                           COL_HEURE_EVENT, heure_str,
                           COL_PRIX, prix_str,
                           COL_DESCRIPTION_EVENT, desc_short,
                           COL_STATUT, statut,
                           -1);
    }
    fclose(f);
}

int rechercher_evenements(const char *critere, GtkWidget *treeview) {
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
    if (!store) return 0;
    gtk_list_store_clear(store);

    char lower_critere[600];
    strcpy(lower_critere, critere);
    for(size_t i = 0; lower_critere[i]; i++) lower_critere[i] = tolower(lower_critere[i]);

    FILE *f = fopen("evenement.txt", "r");
    if (!f) return 0;

    int found = 0;
    int id, j, m, a, h, min;
    float prix;
    char nom[300], type[100], centre[200], salle[100], desc[1000], statut[20];
    char date_str[12], heure_str[6], prix_str[20], desc_short[104];

    while (fscanf(f, "%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\t%19[^\n]\n",
                  &id, nom, type, centre, salle, desc, &j, &m, &a, &h, &min, &prix, statut) != EOF) {

        char temp[2500];
        snprintf(temp, sizeof(temp), "%s %s %s %s %s %s", nom, type, centre, salle, desc, statut);
        for(size_t i = 0; temp[i]; i++) temp[i] = tolower(temp[i]);

        if (strstr(temp, lower_critere)) {
            found++;
            snprintf(date_str, sizeof(date_str), "%02d/%02d/%04d", j, m, a);
            snprintf(heure_str, sizeof(heure_str), "%02d:%02d", h, min);
            snprintf(prix_str, sizeof(prix_str), "%.2f DT", prix);
            strncpy(desc_short, desc, 100); desc_short[100] = '\0';
            if (strlen(desc) > 100) strcat(desc_short, "...");

            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                               COL_ID_EVENT, id,
                               COL_NOM_EVENT, nom,
                               COL_TYPE_EVENT, type,
                               COL_CENTRE, centre,
                               COL_SALLE, salle,
                               COL_DATE_EVENT, date_str,
                               COL_HEURE_EVENT, heure_str,
                               COL_PRIX, prix_str,
                               COL_DESCRIPTION_EVENT, desc_short,
                               COL_STATUT, statut,
                               -1);
        }
    }
    fclose(f);
    return found;
}

/* ============================================= */
/*             INSCRIPTION FUNCTIONS             */
/* ============================================= */

int get_next_inscription_id() {
    inscription_event insc;
    int max = 0;
    FILE *f = fopen("inscription_event.txt", "r");
    if (f) {
        while (fscanf(f, "%d\t%19[^\t]\t%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\n",
                      &insc.id_inscription, insc.cin_membre, &insc.id_event,
                      insc.nom_event, insc.type_event, insc.centre, insc.salle, insc.description,
                      &insc.date_e.j, &insc.date_e.m, &insc.date_e.a,
                      &insc.heure_e.heure, &insc.heure_e.minute, &insc.prix) != EOF) {
            if (insc.id_inscription > max) max = insc.id_inscription;
        }
        fclose(f);
    }
    return max + 1;
}

void ajouter_inscription_event(inscription_event insc) {
    FILE *f = fopen("inscription_event.txt", "a");
    if (!f) return;
    insc.id_inscription = get_next_inscription_id();
    fprintf(f, "%d\t%s\t%d\t%s\t%s\t%s\t%s\t%s\t%02d/%02d/%04d\t%02d:%02d\t%.2f\n",
            insc.id_inscription, insc.cin_membre, insc.id_event,
            insc.nom_event, insc.type_event, insc.centre, insc.salle, insc.description,
            insc.date_e.j, insc.date_e.m, insc.date_e.a,
            insc.heure_e.heure, insc.heure_e.minute, insc.prix);
    fflush(f);
    fclose(f);
}

int inscription_existe(const char *cin_membre, int id_event) {
    inscription_event insc;
    FILE *f = fopen("inscription_event.txt", "r");
    if (!f) return 0;
    while (fscanf(f, "%d\t%19[^\t]\t%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\n",
                  &insc.id_inscription, insc.cin_membre, &insc.id_event,
                  insc.nom_event, insc.type_event, insc.centre, insc.salle, insc.description,
                  &insc.date_e.j, &insc.date_e.m, &insc.date_e.a,
                  &insc.heure_e.heure, &insc.heure_e.minute, &insc.prix) != EOF) {
        if (strcmp(insc.cin_membre, cin_membre) == 0 && insc.id_event == id_event) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

void supprimer_inscription_event(int id_inscription) {
    inscription_event insc;
    FILE *f = fopen("inscription_event.txt", "r");
    FILE *temp = fopen("temp_insc.txt", "w");
    if (!f || !temp) { if(f) fclose(f); if(temp) fclose(temp); return; }

    while (fscanf(f, "%d\t%19[^\t]\t%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\n",
                  &insc.id_inscription, insc.cin_membre, &insc.id_event,
                  insc.nom_event, insc.type_event, insc.centre, insc.salle, insc.description,
                  &insc.date_e.j, &insc.date_e.m, &insc.date_e.a,
                  &insc.heure_e.heure, &insc.heure_e.minute, &insc.prix) != EOF) {
        if (insc.id_inscription != id_inscription) {
            fprintf(temp, "%d\t%s\t%d\t%s\t%s\t%s\t%s\t%s\t%02d/%02d/%04d\t%02d:%02d\t%.2f\n",
                    insc.id_inscription, insc.cin_membre, insc.id_event,
                    insc.nom_event, insc.type_event, insc.centre, insc.salle, insc.description,
                    insc.date_e.j, insc.date_e.m, insc.date_e.a,
                    insc.heure_e.heure, insc.heure_e.minute, insc.prix);
        }
    }
    fclose(f); fclose(temp);
    remove("inscription_event.txt");
    rename("temp_insc.txt", "inscription_event.txt");
}

enum {
    COL_ID_INSC,
    COL_ID_EVENT_INSC,
    COL_NOM_INSC,
    COL_TYPE_INSC,
    COL_CENTRE_INSC,
    COL_SALLE_INSC,
    COL_DATE_INSC,
    COL_HEURE_INSC,
    COL_PRIX_INSC,
    COL_DESC_INSC,
    COL_COUNT_INSC
};

void afficher_inscriptions_membre(GtkWidget *treeview, const char *cin_membre) {
    GtkListStore *store;
    GtkTreeIter iter;
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
    if (!store) {
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "ID Insc", renderer, "text", COL_ID_INSC, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "ID Event", renderer, "text", COL_ID_EVENT_INSC, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Nom", renderer, "text", COL_NOM_INSC, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Type", renderer, "text", COL_TYPE_INSC, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Centre", renderer, "text", COL_CENTRE_INSC, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Salle", renderer, "text", COL_SALLE_INSC, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Date", renderer, "text", COL_DATE_INSC, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Heure", renderer, "text", COL_HEURE_INSC, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Prix", renderer, "text", COL_PRIX_INSC, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Description", renderer, "text", COL_DESC_INSC, NULL);

        store = gtk_list_store_new(COL_COUNT_INSC,
                                   G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING,
                                   G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                   G_TYPE_STRING, G_TYPE_STRING);
        gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
        g_object_unref(store);
    }
    gtk_list_store_clear(store);

    FILE *f = fopen("inscription_event.txt", "r");
    if (!f) return;

    inscription_event insc;
    char date_str[12], heure_str[6], prix_str[20], desc_short[104];

    while (fscanf(f, "%d\t%19[^\t]\t%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\n",
                  &insc.id_inscription, insc.cin_membre, &insc.id_event,
                  insc.nom_event, insc.type_event, insc.centre, insc.salle, insc.description,
                  &insc.date_e.j, &insc.date_e.m, &insc.date_e.a,
                  &insc.heure_e.heure, &insc.heure_e.minute, &insc.prix) != EOF) {
        if (strcmp(insc.cin_membre, cin_membre) == 0) {
            snprintf(date_str, sizeof(date_str), "%02d/%02d/%04d", insc.date_e.j, insc.date_e.m, insc.date_e.a);
            snprintf(heure_str, sizeof(heure_str), "%02d:%02d", insc.heure_e.heure, insc.heure_e.minute);
            snprintf(prix_str, sizeof(prix_str), "%.2f DT", insc.prix);
            strncpy(desc_short, insc.description, 100); desc_short[100] = '\0';
            if (strlen(insc.description) > 100) strcat(desc_short, "...");

            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                               COL_ID_INSC, insc.id_inscription,
                               COL_ID_EVENT_INSC, insc.id_event,
                               COL_NOM_INSC, insc.nom_event,
                               COL_TYPE_INSC, insc.type_event,
                               COL_CENTRE_INSC, insc.centre,
                               COL_SALLE_INSC, insc.salle,
                               COL_DATE_INSC, date_str,
                               COL_HEURE_INSC, heure_str,
                               COL_PRIX_INSC, prix_str,
                               COL_DESC_INSC, desc_short,
                               -1);
        }
    }
    fclose(f);
}

/* ============================================= */
/*              UTILITY FUNCTIONS                */
/* ============================================= */

void remplir_combobox_centres(GtkWidget *combobox) {
    GtkListStore *store = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(combobox)));

    if (!store) {
        store = gtk_list_store_new(1, G_TYPE_STRING);
        gtk_combo_box_set_model(GTK_COMBO_BOX(combobox), GTK_TREE_MODEL(store));
        g_object_unref(store);
    }

    gtk_list_store_clear(store);

    FILE *f = fopen("centre.txt", "r");
    if (!f) return;

    char ligne[2048], id[50], nom[100];
    GtkTreeIter iter;

    while (fgets(ligne, sizeof(ligne), f)) {
        if (sscanf(ligne, "%[^\t]\t%[^\t]", id, nom) == 2) {
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, 0, nom, -1);
        }
    }

    fclose(f);

    gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), 0);
}

/* ============================================= */
/*        MEMBER INSCRIPTION FUNCTIONS          */
/* ============================================= */

// Inscrire un membre à un événement
int inscrire_membre_event(const char *cin_membre, int id_event) {
    // Vérifier si l'événement existe
    if (!evenement_existe(id_event)) {
        return -1; // Événement n'existe pas
    }
    
    // Vérifier si le membre est déjà inscrit
    if (inscription_existe(cin_membre, id_event)) {
        return -2; // Déjà inscrit
    }
    
    // Récupérer les informations de l'événement
    evenement ev = trouver_evenement(id_event);
    if (ev.id_event == 0) {
        return -1; // Événement introuvable
    }
    
    // Créer l'inscription
    inscription_event insc = {0};
    strcpy(insc.cin_membre, cin_membre);
    insc.id_event = id_event;
    strcpy(insc.nom_event, ev.nom_event);
    strcpy(insc.type_event, ev.type_event);
    strcpy(insc.centre, ev.centre);
    strcpy(insc.salle, ev.salle);
    strcpy(insc.description, ev.description);
    insc.date_e = ev.date_e;
    insc.heure_e = ev.heure_e;
    insc.prix = ev.prix;
    
    // Ajouter l'inscription
    ajouter_inscription_event(insc);
    
    return 1; // Succès
}

// Afficher tous les événements disponibles pour inscription dans un TreeView
void afficher_evenements_disponibles(GtkWidget *treeview) {
    GtkListStore *store;
    GtkTreeIter iter;
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
    if (!store) {
        // Créer les colonnes
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "ID", renderer, "text", 0, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Nom", renderer, "text", 1, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Type", renderer, "text", 2, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Centre", renderer, "text", 3, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Salle", renderer, "text", 4, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Date", renderer, "text", 5, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Heure", renderer, "text", 6, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Prix", renderer, "text", 7, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Statut", renderer, "text", 8, NULL);

        store = gtk_list_store_new(9,
                                   G_TYPE_INT,    // ID
                                   G_TYPE_STRING, // Nom
                                   G_TYPE_STRING, // Type
                                   G_TYPE_STRING, // Centre
                                   G_TYPE_STRING, // Salle
                                   G_TYPE_STRING, // Date
                                   G_TYPE_STRING, // Heure
                                   G_TYPE_STRING, // Prix
                                   G_TYPE_STRING); // Statut
        gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
        g_object_unref(store);
    }
    gtk_list_store_clear(store);

    FILE *f = fopen("evenement.txt", "r");
    if (!f) return;

    int id, j, m, a, h, min;
    float prix;
    char nom[300], type[100], centre[200], salle[100], desc[1000], statut[20];
    char date_str[12], heure_str[6], prix_str[20];

    while (fscanf(f, "%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\t%19[^\n]\n",
                  &id, nom, type, centre, salle, desc, &j, &m, &a, &h, &min, &prix, statut) != EOF) {
        
        // N'afficher que les événements en cours
        if (strcmp(statut, "en_cours") == 0) {
            snprintf(date_str, sizeof(date_str), "%02d/%02d/%04d", j, m, a);
            snprintf(heure_str, sizeof(heure_str), "%02d:%02d", h, min);
            snprintf(prix_str, sizeof(prix_str), "%.2f DT", prix);

            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                               0, id,
                               1, nom,
                               2, type,
                               3, centre,
                               4, salle,
                               5, date_str,
                               6, heure_str,
                               7, prix_str,
                               8, statut,
                               -1);
        }
    }
    fclose(f);
}

// Compter le nombre d'inscriptions pour un membre
int compter_inscriptions_membre(const char *cin_membre) {
    int count = 0;
    inscription_event insc;
    FILE *f = fopen("inscription_event.txt", "r");
    if (!f) return 0;
    
    while (fscanf(f, "%d\t%19[^\t]\t%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\n",
                  &insc.id_inscription, insc.cin_membre, &insc.id_event,
                  insc.nom_event, insc.type_event, insc.centre, insc.salle, insc.description,
                  &insc.date_e.j, &insc.date_e.m, &insc.date_e.a,
                  &insc.heure_e.heure, &insc.heure_e.minute, &insc.prix) != EOF) {
        if (strcmp(insc.cin_membre, cin_membre) == 0) {
            count++;
        }
    }
    fclose(f);
    return count;
}

// Calculer le montant total des inscriptions pour un membre
float calculer_total_inscriptions(const char *cin_membre) {
    float total = 0.0;
    inscription_event insc;
    FILE *f = fopen("inscription_event.txt", "r");
    if (!f) return 0.0;
    
    while (fscanf(f, "%d\t%19[^\t]\t%d\t%299[^\t]\t%99[^\t]\t%199[^\t]\t%99[^\t]\t%999[^\t]\t%d/%d/%d\t%d:%d\t%f\n",
                  &insc.id_inscription, insc.cin_membre, &insc.id_event,
                  insc.nom_event, insc.type_event, insc.centre, insc.salle, insc.description,
                  &insc.date_e.j, &insc.date_e.m, &insc.date_e.a,
                  &insc.heure_e.heure, &insc.heure_e.minute, &insc.prix) != EOF) {
        if (strcmp(insc.cin_membre, cin_membre) == 0) {
            total += insc.prix;
        }
    }
    fclose(f);
    return total;
}
