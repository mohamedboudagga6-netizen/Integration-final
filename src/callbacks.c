#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "interface_equip.h"




gboolean valider_texte_alphabetique(const gchar *text) {
    if (!text || strlen(text) == 0) {
        return FALSE;
    }
    
    for (int i = 0; text[i] != '\0'; i++) {
        if (!g_ascii_isalpha(text[i]) && text[i] != ' ' && text[i] != '-') {
            return FALSE;
        }
    }
    return TRUE;
}

// Validate that a string contains only alphanumeric characters (for IDs)
gboolean id_valider(const gchar *text) {
    if (!text || strlen(text) == 0) {
        return FALSE;
    }
    
    for (int i = 0; text[i] != '\0'; i++) {
        if (!g_ascii_isalnum(text[i]) && text[i] != '_' && text[i] != '-') {
            return FALSE;
        }
    }
    return TRUE;
}

// Validate numeric input (for price)
gboolean valider_nombre_decimal(const gchar *text) {
    if (!text || strlen(text) == 0) {
        return FALSE;
    }
    
    gboolean point_trouve = FALSE;
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] == '.' || text[i] == ',') {
            if (point_trouve) return FALSE; // Only one decimal point allowed
            point_trouve = TRUE;
        } else if (!g_ascii_isdigit(text[i])) {
            return FALSE;
        }
    }
    
    // Check if value is positive
    double value = atof(text);
    return value > 0;
}

// Validate integer input (for quantity)
gboolean valider_nombre_entier(const gchar *text) {
    if (!text || strlen(text) == 0) {
        return FALSE;
    }
    
    for (int i = 0; text[i] != '\0'; i++) {
        if (!g_ascii_isdigit(text[i])) {
            return FALSE;
        }
    }
    
    int value = atoi(text);
    return value > 0;
}

// Validate that at least one radio button is selected
gboolean valider_radio_group(GtkWidget *radio1, GtkWidget *radio2, GtkWidget *radio3) {
    return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio1)) ||
           gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio2)) ||
           gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio3));
}

// Validate combobox selection
gboolean valider_combobox(GtkWidget *combo) {
    gchar *text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo));
    if (!text || strlen(text) == 0) {
        if (text) g_free(text);
        return FALSE;
    }
    g_free(text);
    return TRUE;
}

// Show error dialog
void afficher_erreur(GtkWindow *parent, const gchar *message) {
    GtkWidget *dialog = gtk_message_dialog_new(parent,
                                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                                GTK_MESSAGE_ERROR,
                                                GTK_BUTTONS_OK,
                                                "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}






void on_buttonEnregistrer_clicked(GtkButton *button, gpointer user_data) {
    Equipement nouvel_equip;
    
    // Get the top-level window
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    
    // Get all widgets from the equipment window
    GtkWidget *entryNom = lookup_widget(window, "entryNom");
    GtkWidget *entryMarque = lookup_widget(window, "entryMarque");
    GtkWidget *entryID = lookup_widget(window, "entryID");
    GtkWidget *comboCentre = lookup_widget(window, "entryCentreSportif");
    GtkWidget *entryPrix = lookup_widget(window, "entryPrix");
    GtkWidget *entryQuantite = lookup_widget(window, "entryQuantite");
    GtkWidget *checkEtat = lookup_widget(window, "checkbutton1");
    GtkWidget *comboType = lookup_widget(window, "comboType");
    GtkWidget *radioAquatique = lookup_widget(window, "radiobutton1");
    GtkWidget *radioPlainAir = lookup_widget(window, "radiobutton2");
    GtkWidget *radioInterieur = lookup_widget(window, "radiobutton3");
    GtkWidget *calendar = lookup_widget(window, "calendarDateAchat");

    if (!entryNom || !entryMarque || !entryID || !comboCentre || !entryPrix || 
        !entryQuantite || !checkEtat || !comboType || !calendar) {
        afficher_erreur(GTK_WINDOW(window), "Erreur: Un ou plusieurs widgets non trouvés");
        return;
    }

    // Get values
    const gchar *temp_nom = gtk_entry_get_text(GTK_ENTRY(entryNom));
    const gchar *temp_marque = gtk_entry_get_text(GTK_ENTRY(entryMarque));
    const gchar *temp_id = gtk_entry_get_text(GTK_ENTRY(entryID));
    const gchar *temp_prix = gtk_entry_get_text(GTK_ENTRY(entryPrix));
    const gchar *temp_quantite = gtk_entry_get_text(GTK_ENTRY(entryQuantite));
    
    GtkWidget *entry_centre = gtk_bin_get_child(GTK_BIN(comboCentre));
    const gchar *temp_centre = gtk_entry_get_text(GTK_ENTRY(entry_centre));

    // ==================== VALIDATION ====================
    
    // 1. Validate Nom (required, alphabetic only)
    if (!valider_texte_alphabetique(temp_nom)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: Le nom est obligatoire et doit contenir uniquement des lettres et des espaces.");
        gtk_widget_grab_focus(entryNom);
        return;
    }

    // 2. Validate Marque (required, alphabetic only)
    if (!valider_texte_alphabetique(temp_marque)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: La marque est obligatoire et doit contenir uniquement des lettres et des espaces.");
        gtk_widget_grab_focus(entryMarque);
        return;
    }

    // 3. Validate ID (required, alphanumeric)
    if (!id_valider(temp_id)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: L'ID est obligatoire et doit contenir uniquement des lettres, chiffres, tirets ou underscores.");
        gtk_widget_grab_focus(entryID);
        return;
    }

    // 4. Validate Centre Sportif (required)
    if (!temp_centre || strlen(temp_centre) == 0) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: Le centre sportif est obligatoire. Veuillez sélectionner un centre.");
        gtk_widget_grab_focus(comboCentre);
        return;
    }

    // 5. Validate Prix (required, positive decimal number)
    if (!valider_nombre_decimal(temp_prix)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: Le prix est obligatoire et doit être un nombre positif (ex: 150.00 ou 150).");
        gtk_widget_grab_focus(entryPrix);
        return;
    }

    // 6. Validate Quantité (required, positive integer)
    if (!valider_nombre_entier(temp_quantite)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: La quantité est obligatoire et doit être un nombre entier positif.");
        gtk_widget_grab_focus(entryQuantite);
        return;
    }

    // 7. Validate Type (required from combobox)
    if (!valider_combobox(comboType)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: Le type est obligatoire. Veuillez sélectionner un type.");
        gtk_widget_grab_focus(comboType);
        return;
    }

    // 8. Validate Utilisation (at least one radio button must be selected)
    if (!valider_radio_group(radioAquatique, radioPlainAir, radioInterieur)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: L'utilisation est obligatoire. Veuillez sélectionner une option (Aquatique, Plain air ou Intérieur).");
        return;
    }

    // ==================== ALL VALIDATIONS PASSED - SAVE DATA ====================

    // Get état
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkEtat))) {
        strcpy(nouvel_equip.etat, "nouveau");
    } else {
        strcpy(nouvel_equip.etat, "utilisation");
    }

    // Get type
    gchar *type_text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboType));
    
    // Get utilisation
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioAquatique))) {
        strcpy(nouvel_equip.utilisation, "Aquatique");
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioPlainAir))) {
        strcpy(nouvel_equip.utilisation, "Plain air");
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioInterieur))) {
        strcpy(nouvel_equip.utilisation, "Interieur");
    }

    // Get date
    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(calendar), &year, &month, &day);
    sprintf(nouvel_equip.date_achat, "%04d-%02d-%02d", year, month+1, day);

    // Copy validated data
    strcpy(nouvel_equip.nom, temp_nom);
    strcpy(nouvel_equip.marque, temp_marque);
    strcpy(nouvel_equip.id, temp_id);
    strcpy(nouvel_equip.id_centre, temp_centre);
    nouvel_equip.prix = atof(temp_prix);
    nouvel_equip.quantite = atoi(temp_quantite);

    if (type_text) {
        strcpy(nouvel_equip.type, type_text);
        g_free(type_text);
    } else {
        strcpy(nouvel_equip.type, "");
    }

    // Initialize reservation array
    for (int i = 0; i < 16; i++) {
        strcpy(nouvel_equip.tab_reservation[i].date, "0");
        nouvel_equip.tab_reservation[i].heure_debut = 0;
        nouvel_equip.tab_reservation[i].duree = 0;
    }

    // Save to file
    if (ajouter_equip("equipements.txt", &nouvel_equip) == 1) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    GTK_MESSAGE_INFO,
                                                    GTK_BUTTONS_OK,
                                                    "Équipement enregistré avec succès!\n\n"
                                                    "Nom: %s\nMarque: %s\nID: %s",
                                                    nouvel_equip.nom, nouvel_equip.marque, nouvel_equip.id);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        // Clear form after successful save
        gtk_entry_set_text(GTK_ENTRY(entryNom), "");
        gtk_entry_set_text(GTK_ENTRY(entryMarque), "");
        gtk_entry_set_text(GTK_ENTRY(entryID), "");
        gtk_entry_set_text(GTK_ENTRY(entryPrix), "");
        gtk_entry_set_text(GTK_ENTRY(entryQuantite), "");
        gtk_entry_set_text(GTK_ENTRY(entry_centre), "");
        gtk_combo_box_set_active(GTK_COMBO_BOX(comboType), -1);
    } else {
        afficher_erreur(GTK_WINDOW(window), "Erreur lors de l'enregistrement de l'équipement.");
    }
}


void on_buttonQuitter_clicked(GtkButton *button, gpointer user_data)
{
    gtk_main_quit();
}

void on_buttonPrecedent_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (GTK_IS_WINDOW(window)) {
        gtk_widget_destroy(window);
    }
}

// Reservation window callbacks
void on_sreserve_buttonPrecedent_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (GTK_IS_WINDOW(window)) {
        gtk_widget_destroy(window);
    }
}

void on_sreserve_buttonEnregistrer_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    
    // Get all widgets
    GtkWidget *entryNom = lookup_widget(window, "sreserveentryNom");
    GtkWidget *entryMarque = lookup_widget(window, "sreserveentryMarque");
    GtkWidget *entryQuantite = lookup_widget(window, "sreservespinbuttonQuantite");
    GtkWidget *calendar = lookup_widget(window, "sreservecalendar");
    GtkWidget *entryHeureDebut = lookup_widget(window, "sreserveentryHeuredebut");
    GtkWidget *comboCentre = lookup_widget(window, "sreservecomboboxentryCentre");
    GtkWidget *checkDuree1 = lookup_widget(window, "sreservecheckbuttonDuree1");
    GtkWidget *checkDuree2 = lookup_widget(window, "sreservecheckbuttonDuree2");
    GtkWidget *checkDuree3 = lookup_widget(window, "sreservecheckbuttonDuree3");

    if (!entryNom || !entryMarque || !entryQuantite || !calendar || 
        !entryHeureDebut || !comboCentre || !checkDuree1 || !checkDuree2 || !checkDuree3) {
        afficher_erreur(GTK_WINDOW(window), "Erreur: Un ou plusieurs widgets non trouvés");
        return;
    }

    // Get values
    const gchar *nom = gtk_entry_get_text(GTK_ENTRY(entryNom));
    const gchar *marque = gtk_entry_get_text(GTK_ENTRY(entryMarque));
    const gchar *heure_debut_str = gtk_entry_get_text(GTK_ENTRY(entryHeureDebut));
    
    GtkWidget *centre_entry = gtk_bin_get_child(GTK_BIN(comboCentre));
    const gchar *centre_text = gtk_entry_get_text(GTK_ENTRY(centre_entry));

    // ==================== VALIDATION ====================
    
    // 1. Validate Nom (required, alphabetic)
    if (!valider_texte_alphabetique(nom)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: Le nom est obligatoire et doit contenir uniquement des lettres.");
        gtk_widget_grab_focus(entryNom);
        return;
    }

    // 2. Validate Marque (required, alphabetic)
    if (!valider_texte_alphabetique(marque)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: La marque est obligatoire et doit contenir uniquement des lettres.");
        gtk_widget_grab_focus(entryMarque);
        return;
    }

    // 3. Validate Centre (required)
    if (!centre_text || strlen(centre_text) == 0) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: Le centre sportif est obligatoire.");
        gtk_widget_grab_focus(comboCentre);
        return;
    }

    // 4. Validate Heure début (required, must be between 0-23)
    if (!valider_nombre_entier(heure_debut_str)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: L'heure de début est obligatoire et doit être un nombre entier.");
        gtk_widget_grab_focus(entryHeureDebut);
        return;
    }
    
    int heure_debut = atoi(heure_debut_str);
    if (heure_debut < 0 || heure_debut > 23) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: L'heure de début doit être entre 0 et 23.");
        gtk_widget_grab_focus(entryHeureDebut);
        return;
    }

    // 5. Validate Durée (at least one checkbox must be selected)
    int duree = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkDuree1))) {
        duree = 60;
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkDuree2))) {
        duree = 90;
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkDuree3))) {
        duree = 120;
    }
    
    if (duree == 0) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: La durée est obligatoire. Veuillez sélectionner 60, 90 ou 120 minutes.");
        return;
    }

    // ==================== ALL VALIDATIONS PASSED - SAVE RESERVATION ====================

    // Get date
    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(calendar), &year, &month, &day);
    char date_str[12];
    sprintf(date_str, "%04d-%02d-%02d", year, month + 1, day);

    // Get quantity
    const gchar *temp_quantite = gtk_entry_get_text(GTK_ENTRY(entryQuantite));
    int quantite = temp_quantite ? atoi(temp_quantite) : 0;

    // Create reservation
    Reservation rv;
    strcpy(rv.date, date_str);
    rv.heure_debut = heure_debut;
    rv.duree = duree;

    // Save reservation
    int result = reserver_equip("equipements.txt", (char*)nom, &rv);

    if (result) {
        char message[512];
        snprintf(message, sizeof(message),
                "Réservation enregistrée avec succès!\n\n"
                "Équipement: %s %s\n"
                "Quantité: %d\n"
                "Date: %s à %dh\n"
                "Durée: %d minutes\n"
                "Centre: %s",
                nom, marque, quantite, date_str, heure_debut, duree, centre_text);
        
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    GTK_MESSAGE_INFO,
                                                    GTK_BUTTONS_OK,
                                                    "%s", message);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        // Clear form
        gtk_entry_set_text(GTK_ENTRY(entryNom), "");
        gtk_entry_set_text(GTK_ENTRY(entryMarque), "");
        gtk_entry_set_text(GTK_ENTRY(entryQuantite), "");
        gtk_entry_set_text(GTK_ENTRY(entryHeureDebut), "");
        gtk_entry_set_text(GTK_ENTRY(centre_entry), "");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkDuree1), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkDuree2), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkDuree3), FALSE);
    } else {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: Équipement non trouvé ou aucun créneau disponible (maximum 16 réservations).");
    }
}


void on_sreserve_buttonQuitter_clicked(GtkButton *button, gpointer user_data)
{
    gtk_main_quit();
}


void setup_treeview_columns(GtkWidget *treeview) {
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    // Column 0: Nom
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    // Column 1: Marque
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Marque", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    // Column 2: ID
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    // Column 3: Quantité
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Quantité", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    // Column 4: Centre
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Centre", renderer, "text", 4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    // Column 5: Date Achat
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Date Achat", renderer, "text", 5, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    // Column 6: État
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("État", renderer, "text", 6, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    // Column 7: Type
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Type", renderer, "text", 7, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    // Column 8: Prix
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Prix", renderer, "text", 8, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

// Function to load all equipment from file and display in treeview
void afficher_equipements(GtkWidget *treeview, const char *filename) {
    GtkListStore *store;
    GtkTreeIter iter;
    FILE *f;
    Equipement e;
    char prix_str[20];
    char quantite_str[20];
    
    // Create list store with 9 columns (all strings)
    store = gtk_list_store_new(9, 
        G_TYPE_STRING,  // Nom
        G_TYPE_STRING,  // Marque
        G_TYPE_STRING,  // ID
        G_TYPE_STRING,  // Quantité
        G_TYPE_STRING,  // Centre
        G_TYPE_STRING,  // Date Achat
        G_TYPE_STRING,  // État
        G_TYPE_STRING,  // Type
        G_TYPE_STRING   // Prix
    );
    
    // Open file
    f = fopen(filename, "r");
    if (f == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", filename);
        gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
        g_object_unref(store);
        return;
    }
    
    // Read all equipment from file
    while (1) {
        int ret = fscanf(f, "%31[^|]|%31[^|]|%19[^|]|%d|%19[^|]|%23[^|]|%19[^|]|%11[^|]|%11[^|]|%f",
                         e.nom, e.marque, e.id, &e.quantite, e.id_centre,
                         e.date_achat, e.etat, e.utilisation, e.type, &e.prix);
        
        if (ret == EOF) {
            break;
        }
        
        if (ret != 10) {
            // Skip malformed lines
            char buffer[2048];
            fgets(buffer, sizeof(buffer), f);
            continue;
        }
        
        // Skip the reservation data (16 * 3 fields)
        for (int i = 0; i < 16; i++) {
            char temp_date[12];
            int temp_heure, temp_duree;
            fscanf(f, "|%11[^|]|%d|%d", temp_date, &temp_heure, &temp_duree);
        }
        fscanf(f, "\n");
        
        // Convert numeric values to strings
        sprintf(quantite_str, "%d", e.quantite);
        sprintf(prix_str, "%.2f", e.prix);
        
        // Add row to list store
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, e.nom,
                          1, e.marque,
                          2, e.id,
                          3, quantite_str,
                          4, e.id_centre,
                          5, e.date_achat,
                          6, e.etat,
                          7, e.type,
                          8, prix_str,
                          -1);
    }
    
    fclose(f);
    
    // Set the model to the treeview
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);
}





void
on_swarnbuttonCancel_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_Swarnbuttonok_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}


void on_scherbuttonChercher_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *entryNom;
    GtkWidget *treeview;
    const gchar *nom_recherche;
    GtkListStore *store;
    GtkTreeIter iter;
    Equipement results[100];  // Array to store up to 100 results
    int count;
    char prix_str[20];
    char quantite_str[20];
    
    // Get the window
    window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    
    // Get widgets
    entryNom = lookup_widget(window, "shcerentryNom");
    treeview = lookup_widget(window, "schertreeviewChercher");
    
    if (!entryNom || !treeview) {
        printf("Erreur: Widget non trouvé\n");
        return;
    }
    
    // Get the search text
    nom_recherche = gtk_entry_get_text(GTK_ENTRY(entryNom));
    
    // If search box is empty, show all equipment
    if (strlen(nom_recherche) == 0) {
        afficher_equipements(treeview, "equipements.txt");
        return;
    }
    
    // Search using the rechercher function
    count = rechercher_equip( "equipements.txt", nom_recherche, results);
    
    // Create list store
    store = gtk_list_store_new(9, 
        G_TYPE_STRING,  // Nom
        G_TYPE_STRING,  // Marque
        G_TYPE_STRING,  // ID
        G_TYPE_STRING,  // Quantité
        G_TYPE_STRING,  // Centre
        G_TYPE_STRING,  // Date Achat
        G_TYPE_STRING,  // État
        G_TYPE_STRING,  // Type
        G_TYPE_STRING   // Prix
    );
    
    // Add results to the treeview
    for (int i = 0; i < count; i++) {
        sprintf(quantite_str, "%d", results[i].quantite);
        sprintf(prix_str, "%.2f", results[i].prix);
        
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, results[i].nom,
                          1, results[i].marque,
                          2, results[i].id,
                          3, quantite_str,
                          4, results[i].id_centre,
                          5, results[i].date_achat,
                          6, results[i].etat,
                          7, results[i].type,
                          8, prix_str,
                          -1);
    }
    
    // Set the model to the treeview
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    // Show message if nothing found
    if (count == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Aucun équipement trouvé avec le nom '%s'", nom_recherche);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        printf("Recherche terminée: %d équipement(s) trouvé(s)\n", count);
    }
}

void on_scherbuttonPrecedent_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (GTK_IS_WINDOW(window)) {
        gtk_widget_destroy(window);
    }
}

void on_scherbuttonQuitter_clicked(GtkButton *button, gpointer user_data)
{
    gtk_main_quit();
}

void on_scherbuttonModifier_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *treeview;
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *nom, *marque, *id, *quantite_str, *centre, *date_achat, *etat, *type, *prix_str;
    
    // Get the window
    window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    
    // Get treeview
    treeview = lookup_widget(window, "schertreeviewChercher");
    
    if (!treeview) {
        printf("Erreur: TreeView non trouvé\n");
        return;
    }
    
    // Get the selection
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    
    // Check if a row is selected
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "Veuillez sélectionner un équipement à modifier.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Get all data from the selected row
    gtk_tree_model_get(model, &iter, 
                       0, &nom,           // Column 0: Nom
                       1, &marque,        // Column 1: Marque
                       2, &id,            // Column 2: ID
                       3, &quantite_str,  // Column 3: Quantité
                       4, &centre,        // Column 4: Centre
                       5, &date_achat,    // Column 5: Date Achat
                       6, &etat,          // Column 6: État
                       7, &type,          // Column 7: Type
                       8, &prix_str,      // Column 8: Prix
                       -1);
    
    printf("DEBUG: Ouverture de la fenêtre de modification pour: %s %s (ID: %s)\n", nom, marque, id);
    
    // Create modification window
    GtkWidget *modif_window = create_modification();
    
    // Get all widgets from modification window
    GtkWidget *entryNom = lookup_widget(modif_window, "smodifentryNom");
    GtkWidget *entryMarque = lookup_widget(modif_window, "smodifentryMarque");
    GtkWidget *entryId = lookup_widget(modif_window, "smodifentryId");
    GtkWidget *spinQuantite = lookup_widget(modif_window, "smodifspinbuttonQuantite");
    GtkWidget *comboCentre = lookup_widget(modif_window, "smodifcomboboxentryCentre");
    GtkWidget *calendar = lookup_widget(modif_window, "smodifcalendarDate");
    GtkWidget *checkEtat = lookup_widget(modif_window, "smodifcheckbuttonEtat");
    GtkWidget *comboType = lookup_widget(modif_window, "smodifcomboboxType");
    GtkWidget *entryPrix = lookup_widget(modif_window, "smodifentryPrix");
    
    // Fill the form with existing data
    if (entryNom) gtk_entry_set_text(GTK_ENTRY(entryNom), nom);
    if (entryMarque) gtk_entry_set_text(GTK_ENTRY(entryMarque), marque);
    if (entryId) {
        gtk_entry_set_text(GTK_ENTRY(entryId), id);
        gtk_widget_set_sensitive(entryId, FALSE); // Make ID read-only
    }
    if (spinQuantite) gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinQuantite), atoi(quantite_str));
    
    // Set centre in combobox
    if (comboCentre) {
        GtkWidget *entry_centre = gtk_bin_get_child(GTK_BIN(comboCentre));
        gtk_entry_set_text(GTK_ENTRY(entry_centre), centre);
    }
    
    // Set date in calendar (format: YYYY-MM-DD or YYYY_MM_DD)
    if (calendar && date_achat) {
        int year, month, day;
        if (sscanf(date_achat, "%d-%d-%d", &year, &month, &day) == 3 ||
            sscanf(date_achat, "%d_%d_%d", &year, &month, &day) == 3) {
            gtk_calendar_select_month(GTK_CALENDAR(calendar), month - 1, year);
            gtk_calendar_select_day(GTK_CALENDAR(calendar), day);
        }
    }
    
    // Set etat checkbox
    if (checkEtat) {
        if (strcmp(etat, "nouveau") == 0) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkEtat), TRUE);
        } else {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkEtat), FALSE);
        }
    }
    
    // Set type in combobox
    if (comboType) {
        if (strcmp(type, "Cardio") == 0) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(comboType), 0);
        } else if (strcmp(type, "Fitness") == 0) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(comboType), 1);
        } else if (strcmp(type, "Zumba") == 0) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(comboType), 2);
        }
    }
    
    // Set prix
    if (entryPrix) gtk_entry_set_text(GTK_ENTRY(entryPrix), prix_str);
    
    // Note: We need to get the full equipment data to set utilisation radio buttons
    // Search for the equipment to get utilisation field
    Equipement results[100];
    int count = rechercher_equip( "equipements.txt", nom, results);
    
    if (count > 0) {
        // Find the exact equipment by ID
        for (int i = 0; i < count; i++) {
            if (strcmp(results[i].id, id) == 0) {
                // Set utilisation radio buttons
                GtkWidget *radioAquatique = lookup_widget(modif_window, "smodifradiobuttonUtilisation1");
                GtkWidget *radioPlainAir = lookup_widget(modif_window, "smodifradiobuttonUtilisation2");
                GtkWidget *radioInterieur = lookup_widget(modif_window, "smodifradiobuttonUtilisation3");
                
                if (strcmp(results[i].utilisation, "Aquatique") == 0) {
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioAquatique), TRUE);
                } else if (strcmp(results[i].utilisation, "Plain air") == 0) {
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioPlainAir), TRUE);
                } else if (strcmp(results[i].utilisation, "Interieur") == 0) {
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioInterieur), TRUE);
                }
                break;
            }
        }
    }
    
    // Show the modification window
    gtk_widget_show(modif_window);
    
    // Free allocated strings
    g_free(nom);
    g_free(marque);
    g_free(id);
    g_free(quantite_str);
    g_free(centre);
    g_free(date_achat);
    g_free(etat);
    g_free(type);
    g_free(prix_str);
}

void on_scherbuttonSupprimer_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *treeview;
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *id;
    gchar *nom;
    gchar *marque;
    
    // Get the window
    window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    
    // Get treeview
    treeview = lookup_widget(window, "schertreeviewChercher");
    
    if (!treeview) {
        printf("Erreur: TreeView non trouvé\n");
        return;
    }
    
    // Get the selection
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    
    // Check if a row is selected
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "Veuillez sélectionner un équipement à supprimer.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Get the ID, name and brand from the selected row
    gtk_tree_model_get(model, &iter, 
                       0, &nom,      // Column 0: Nom
                       1, &marque,   // Column 1: Marque
                       2, &id,       // Column 2: ID
                       -1);
    
    // Show confirmation dialog
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Voulez-vous vraiment supprimer l'équipement:\n\n%s %s (ID: %s)?",
        nom, marque, id);
    
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    if (response == GTK_RESPONSE_YES) {
        // Call supprimer function
        int result = supprimer_equip( "equipements.txt", id);
        
        if (result) {
            // Success - show message
            GtkWidget *success_dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Équipement supprimé avec succès!\n\n%s %s (ID: %s)",
                nom, marque, id);
            gtk_dialog_run(GTK_DIALOG(success_dialog));
            gtk_widget_destroy(success_dialog);
            
            // Refresh the treeview - show all equipment
            afficher_equipements(treeview, "equipements.txt");
            
            printf("Équipement supprimé: %s %s (ID: %s)\n", nom, marque, id);
        } else {
            // Error - show message
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Erreur: Impossible de supprimer l'équipement.\nID non trouvé: %s",
                id);
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
    } else {
        printf("Suppression annulée par l'utilisateur\n");
    }
    
    // Free allocated strings
    g_free(nom);
    g_free(marque);
    g_free(id);
}

void on_smodifbuttonEnregistrer_clicked(GtkButton *button, gpointer user_data) {
    Equipement modif_equip;
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    // Get all widgets
    GtkWidget *entryNom = lookup_widget(window, "smodifentryNom");
    GtkWidget *entryMarque = lookup_widget(window, "smodifentryMarque");
    GtkWidget *entryId = lookup_widget(window, "smodifentryId");
    GtkWidget *spinQuantite = lookup_widget(window, "smodifspinbuttonQuantite");
    GtkWidget *comboCentre = lookup_widget(window, "smodifcomboboxentryCentre");
    GtkWidget *calendar = lookup_widget(window, "smodifcalendarDate");
    GtkWidget *checkEtat = lookup_widget(window, "smodifcheckbuttonEtat");
    GtkWidget *comboType = lookup_widget(window, "smodifcomboboxType");
    GtkWidget *radioAquatique = lookup_widget(window, "smodifradiobuttonUtilisation1");
    GtkWidget *radioPlainAir = lookup_widget(window, "smodifradiobuttonUtilisation2");
    GtkWidget *radioInterieur = lookup_widget(window, "smodifradiobuttonUtilisation3");
    GtkWidget *entryPrix = lookup_widget(window, "smodifentryPrix");

    if (!entryNom || !entryMarque || !entryId || !spinQuantite || !comboCentre || 
        !calendar || !checkEtat || !comboType || !entryPrix) {
        afficher_erreur(GTK_WINDOW(window), "Erreur: Un ou plusieurs widgets non trouvés");
        return;
    }

    // Get values
    const gchar *nom = gtk_entry_get_text(GTK_ENTRY(entryNom));
    const gchar *marque = gtk_entry_get_text(GTK_ENTRY(entryMarque));
    const gchar *id = gtk_entry_get_text(GTK_ENTRY(entryId));
    int quantite = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinQuantite));
    const gchar *prix_str = gtk_entry_get_text(GTK_ENTRY(entryPrix));
    
    GtkWidget *entry_centre = gtk_bin_get_child(GTK_BIN(comboCentre));
    const gchar *centre = gtk_entry_get_text(GTK_ENTRY(entry_centre));

    // ==================== VALIDATION ====================
    
    // Validate Nom
    if (!valider_texte_alphabetique(nom)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: Le nom est obligatoire et doit contenir uniquement des lettres.");
        gtk_widget_grab_focus(entryNom);
        return;
    }

    // Validate Marque
    if (!valider_texte_alphabetique(marque)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: La marque est obligatoire et doit contenir uniquement des lettres.");
        gtk_widget_grab_focus(entryMarque);
        return;
    }

    // Validate ID
    if (!id_valider(id)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: L'ID est obligatoire et doit contenir uniquement des caractères alphanumériques.");
        return;
    }

    // Validate Prix
    if (!valider_nombre_decimal(prix_str)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: Le prix est obligatoire et doit être un nombre positif.");
        gtk_widget_grab_focus(entryPrix);
        return;
    }

    // Validate Centre
    if (!centre || strlen(centre) == 0) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: Le centre sportif est obligatoire.");
        gtk_widget_grab_focus(comboCentre);
        return;
    }

    // Validate Type
    if (!valider_combobox(comboType)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: Le type est obligatoire.");
        gtk_widget_grab_focus(comboType);
        return;
    }

    // Validate Utilisation
    if (!valider_radio_group(radioAquatique, radioPlainAir, radioInterieur)) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: L'utilisation est obligatoire.");
        return;
    }

    // Validate Quantité
    if (quantite <= 0) {
        afficher_erreur(GTK_WINDOW(window), 
            "Erreur: La quantité doit être supérieure à 0.");
        gtk_widget_grab_focus(spinQuantite);
        return;
    }

    // ==================== ALL VALIDATIONS PASSED ====================

    // Get date
    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(calendar), &year, &month, &day);

    // Get etat
    gboolean etat_nouveau = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkEtat));

    // Get type
    gchar *type_text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboType));

    // Get utilisation
    const gchar *utilisation = "Non";
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioAquatique))) {
        utilisation = "Aquatique";
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioPlainAir))) {
        utilisation = "Plain air";
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioInterieur))) {
        utilisation = "Interieur";
    }

    // Fill structure
    strcpy(modif_equip.nom, nom);
    strcpy(modif_equip.marque, marque);
    strcpy(modif_equip.id, id);
    modif_equip.quantite = quantite;
    strcpy(modif_equip.id_centre, centre);
    sprintf(modif_equip.date_achat, "%04d-%02d-%02d", year, month + 1, day);
    strcpy(modif_equip.etat, etat_nouveau ? "nouveau" : "utilisation");
    strcpy(modif_equip.utilisation, utilisation);
    strcpy(modif_equip.type, type_text ? type_text : "");
    modif_equip.prix = atof(prix_str);

    if (type_text) g_free(type_text);

    // Save modifications
    int result = modifier_equip("equipements.txt", modif_equip.id, &modif_equip);

    if (result) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    GTK_MESSAGE_INFO,
                                                    GTK_BUTTONS_OK,
                                                    "Équipement modifié avec succès!\n\n%s %s (ID: %s)",
                                                    modif_equip.nom, modif_equip.marque, modif_equip.id);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        gtk_widget_destroy(window);
    } else {
        afficher_erreur(GTK_WINDOW(window), "Erreur: Impossible de modifier l'équipement.");
    }
}
void on_smodifbuttonPrecedent_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (GTK_IS_WINDOW(window)) {
        gtk_widget_destroy(window);
    }
}

void on_smodifbuttonQuitter_clicked(GtkButton *button, gpointer user_data)
{
    gtk_main_quit();
}


