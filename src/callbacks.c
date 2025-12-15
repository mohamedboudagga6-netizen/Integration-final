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
#include "centre.h"
#include "evenement.h"
#include "Entraineur.h"


// Equipment window callbacks
void on_buttonEnregistrer_clicked(GtkButton *button, gpointer user_data)
{
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

    if (!entryNom || !entryMarque || !entryID ||  !comboCentre || 
        !entryPrix || !entryQuantite || !checkEtat || !comboType || !calendar) {
        printf("Erreur: Un ou plusieurs widgets non trouvés\n");
        return;
    }

    const gchar *temp_nom = gtk_entry_get_text(GTK_ENTRY(entryNom));
    const gchar *temp_marque = gtk_entry_get_text(GTK_ENTRY(entryMarque));
    const gchar *temp_id = gtk_entry_get_text(GTK_ENTRY(entryID));
    const gchar *temp_prix = gtk_entry_get_text(GTK_ENTRY(entryPrix));
    const gchar *temp_quantite = gtk_entry_get_text(GTK_ENTRY(entryQuantite));

    GtkWidget *entry_centre = gtk_bin_get_child(GTK_BIN(comboCentre));
    const gchar *temp_centre = gtk_entry_get_text(GTK_ENTRY(entry_centre));

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkEtat))) {
        strcpy(nouvel_equip.etat, "nouveau");
    } else {
        strcpy(nouvel_equip.etat, "utilisation");
    }

    gchar *type_text = NULL;
    GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(comboType));
    GtkTreeIter iter;
    if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(comboType), &iter)) {
        gtk_tree_model_get(model, &iter, 0, &type_text, -1);
    }

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioAquatique))) {
        strcpy(nouvel_equip.utilisation, "Aquatique");
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioPlainAir))) {
        strcpy(nouvel_equip.utilisation, "Plain air");
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioInterieur))) {
        strcpy(nouvel_equip.utilisation, "Interieur");
    } else {
        strcpy(nouvel_equip.utilisation, "Non");
    }

    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(calendar), &year, &month, &day);
    sprintf(nouvel_equip.date_achat, "%04d-%02d-%02d", year, month+1, day);

    strcpy(nouvel_equip.nom, temp_nom ? temp_nom : "");
    strcpy(nouvel_equip.marque, temp_marque ? temp_marque : "");
    strcpy(nouvel_equip.id, temp_id ? temp_id : "");
    strcpy(nouvel_equip.id_centre, temp_centre ? temp_centre : "");
    nouvel_equip.prix = temp_prix ? atof(temp_prix) : 0.0;
    nouvel_equip.quantite = temp_quantite ? atoi(temp_quantite) : 0;
    
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
    
    printf("Nom: %s\n", nouvel_equip.nom);
    printf("Marque: %s\n", nouvel_equip.marque);
    printf("ID: %s\n", nouvel_equip.id);
    printf("Quantité: %d\n", nouvel_equip.quantite);
    printf("Centre: %s\n", nouvel_equip.id_centre);
    printf("Prix: %.2f\n", nouvel_equip.prix);
    printf("État: %s\n", nouvel_equip.etat);
    printf("Type: %s\n", nouvel_equip.type);
    printf("Utilisation: %s\n", nouvel_equip.utilisation);
    printf("Date achat: %s\n", nouvel_equip.date_achat);
    
    if (ajouter_equip( "equipements.txt", &nouvel_equip) == 1) {
        printf("Équipement enregistré avec succès!\n");
        
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Équipement enregistré avec succès!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        printf("Erreur lors de l'enregistrement\n");
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

void on_sreserve_buttonEnregistrer_clicked(GtkButton *button, gpointer user_data)
{
    // Get the reservation window
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    
    // Get all reservation widgets
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
        !entryHeureDebut || !comboCentre || !checkDuree1 || !checkDuree2 || !checkDuree3 ) {
        printf("Erreur: Un ou plusieurs widgets de réservation non trouvés\n");
        return;
    }

    // Get equipment information
    const gchar *nom = gtk_entry_get_text(GTK_ENTRY(entryNom));
    const gchar *marque = gtk_entry_get_text(GTK_ENTRY(entryMarque));
    
    // Get quantity
    const gchar *temp_quantite = gtk_entry_get_text(GTK_ENTRY(entryQuantite));
    int quantite = temp_quantite ? atoi(temp_quantite) : 0;
    
    // Get date from calendar
    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(calendar), &year, &month, &day);
    char date_str[12];
    sprintf(date_str, "%04d-%02d-%02d", year, month + 1, day);
    
    // Get start time
    const gchar *heure_debut_str = gtk_entry_get_text(GTK_ENTRY(entryHeureDebut));
    int heure_debut = atoi(heure_debut_str);
    
    // Get duration
    int duree = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkDuree1))) {
        duree = 60;
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkDuree2))) {
        duree = 90;
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkDuree3))) {
        duree = 120;
    }
    
    // Get sports center
    GtkWidget *centre_entry = gtk_bin_get_child(GTK_BIN(comboCentre));
    const gchar *centre_text = gtk_entry_get_text(GTK_ENTRY(centre_entry));

    // Validate inputs
    if (strlen(nom) == 0 || strlen(marque) == 0 || duree == 0 || strlen(heure_debut_str) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Veuillez remplir tous les champs obligatoires: nom, marque, heure début et durée.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // Create reservation object
    Reservation rv;
    strcpy(rv.date, date_str);
    rv.heure_debut = heure_debut;
    rv.duree = duree;

    // Call reserver function
    int result = reserver_equip( "equipements.txt", (char*)nom, &rv);

    if (result) {
        // Success message
        char message[512];
        snprintf(message, sizeof(message),
            "Réservation enregistrée avec succès!\n\n"
            "Équipement: %s %s\n"
            "Quantité: %d\n"
            "Date: %s à %sh\n"
            "Durée: %d minutes\n"
            "Centre: %s",
            nom, marque, 
            quantite,
            date_str, heure_debut_str, duree, 
            centre_text ? centre_text : "Non spécifié");
        
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "%s", message);
        
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        // Clear the form after successful reservation
        gtk_entry_set_text(GTK_ENTRY(entryNom), "");
        gtk_entry_set_text(GTK_ENTRY(entryMarque), "");
        gtk_entry_set_text(GTK_ENTRY(entryQuantite), "");
        gtk_entry_set_text(GTK_ENTRY(entryHeureDebut), "");
        gtk_entry_set_text(GTK_ENTRY(centre_entry), "");
        
        // Uncheck all duration buttons
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkDuree1), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkDuree2), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkDuree3), FALSE);
    } else {
        // Error message
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Erreur: Équipement non trouvé ou aucun créneau disponible (maximum 16 réservations).");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
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

void on_smodifbuttonEnregistrer_clicked(GtkButton *button, gpointer user_data)
{
    Equipement modif_equip;
    
    // Get the modification window
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
        printf("Erreur: Un ou plusieurs widgets non trouvés\n");
        return;
    }
    
    // Get values from widgets
    const gchar *nom = gtk_entry_get_text(GTK_ENTRY(entryNom));
    const gchar *marque = gtk_entry_get_text(GTK_ENTRY(entryMarque));
    const gchar *id = gtk_entry_get_text(GTK_ENTRY(entryId));
    int quantite = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinQuantite));
    const gchar *prix_str = gtk_entry_get_text(GTK_ENTRY(entryPrix));
    
    GtkWidget *entry_centre = gtk_bin_get_child(GTK_BIN(comboCentre));
    const gchar *centre = gtk_entry_get_text(GTK_ENTRY(entry_centre));
    
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
    
    // Fill the Equipement structure
    strcpy(modif_equip.nom, nom ? nom : "");
    strcpy(modif_equip.marque, marque ? marque : "");
    strcpy(modif_equip.id, id ? id : "");
    modif_equip.quantite = quantite;
    strcpy(modif_equip.id_centre, centre ? centre : "");
    sprintf(modif_equip.date_achat, "%04d-%02d-%02d", year, month + 1, day);
    strcpy(modif_equip.etat, etat_nouveau ? "nouveau" : "utilisation");
    strcpy(modif_equip.utilisation, utilisation);
    strcpy(modif_equip.type, type_text ? type_text : "");
    modif_equip.prix = prix_str ? atof(prix_str) : 0.0;
    
    if (type_text) g_free(type_text);
    
    // Validate inputs
    if (strlen(modif_equip.nom) == 0 || strlen(modif_equip.id) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Veuillez remplir tous les champs obligatoires.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Call modifier function
    int result = modifier_equip( "equipements.txt", modif_equip.id, &modif_equip);
    
    if (result) {
        // Success
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Équipement modifié avec succès!\n\n%s %s (ID: %s)",
            modif_equip.nom, modif_equip.marque, modif_equip.id);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        // Close the modification window
        gtk_widget_destroy(window);
        
        printf("Équipement modifié: %s %s (ID: %s)\n", 
               modif_equip.nom, modif_equip.marque, modif_equip.id);
    } else {
        // Error
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Erreur: Impossible de modifier l'équipement.\nID non trouvé: %s",
            modif_equip.id);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
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




/////////////////// aaaaaaaaaaaaaaa /////////////



char public_vise[50] = "";
char type_contrat[50] = "";
///////////////////////////f_quitter/////////////////////////
void confirm_quit(GtkWidget *parent_window) {
    GtkWidget *dialog;
    GtkResponseType result;

    dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                    GTK_DIALOG_MODAL,
                                    GTK_MESSAGE_WARNING,
                                    GTK_BUTTONS_NONE,
                                    "Vous êtes sûr de quitter FitVerse ?");

    gtk_dialog_add_button(GTK_DIALOG(dialog), "Oui", GTK_RESPONSE_YES);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Non", GTK_RESPONSE_NO);

    result = gtk_dialog_run(GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_YES) {
        gtk_main_quit();
    }

    gtk_widget_destroy(dialog);
}
/////////////////////////////////////Fonction verif/////////////////////////////////////
int valider_telephone(const char *tel, GtkWidget *parent_window)
{
    if (tel == NULL || strlen(tel) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Erreur: Le numéro de téléphone est obligatoire!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return 0;
    }
  
    if (strlen(tel) != 8) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Erreur: Le numéro de téléphone doit contenir exactement 8 chiffres!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return 0;
    }
    
    for (int i = 0; i < 8; i++) {
        if (!g_ascii_isdigit(tel[i])) {
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                        GTK_DIALOG_MODAL,
                                                        GTK_MESSAGE_ERROR,
                                                        GTK_BUTTONS_OK,
                                                        "Erreur: Le numéro de téléphone doit contenir uniquement des chiffres!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return 0;
        }
    }
    
    return 1;
}
int valider_cin(const char *cin, GtkWidget *parent_window)
{
    if (cin == NULL || strlen(cin) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Erreur: Le CIN/Passeport est obligatoire!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return 0;
    }
  
    if (strlen(cin) != 8) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Erreur: Le CIN/Passeport doit contenir exactement 8 chiffres!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return 0;
    }
    
    for (int i = 0; i < 8; i++) {
        if (!g_ascii_isdigit(cin[i])) {
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                        GTK_DIALOG_MODAL,
                                                        GTK_MESSAGE_ERROR,
                                                        GTK_BUTTONS_OK,
                                                        "Erreur: Le CIN/Passeport doit contenir uniquement des chiffres!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return 0;
        }
    }
    
    return 1;
}

int valider_champ_obligatoire(const char *valeur, const char *nom_champ)
{
    if (valeur == NULL || strlen(valeur) == 0) {
        char message[200];
        sprintf(message, "Erreur: %s est obligatoire!", nom_champ);
        
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "%s", message);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return 0;
    }
    return 1;
}
// Fonction pour vérifier si l'ID existe déjà dans le fichier
int id_existe(const char *id) {
    FILE *fichier = fopen("centre.txt", "r");
    if (fichier == NULL) {
        return 0; 
    }
    char ligne[500];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        char id_existant[50];
        if (sscanf(ligne, "%s", id_existant) == 1) {
            if (strcmp(id_existant, id) == 0) {
                fclose(fichier);
                return 1; 
            }
        }
    }
    
    fclose(fichier);
    return 0; // ID non trouvé
}
// Fonction pour valider le format de l'ID
int valider_format_id(const char *id, GtkWidget *parent_window) {
    if (id == NULL || strlen(id) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Erreur: L'identifiant est obligatoire!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return 0;
    }
    
    if (strlen(id) != 8) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Erreur: L'identifiant doit contenir exactement 8 caractères!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return 0;
    }
    
    int a_lettre = 0;
    int a_chiffre = 0;
    
    for (int i = 0; i < 8; i++) {
        if (g_ascii_isalpha(id[i])) {
            a_lettre = 1;
        } else if (g_ascii_isdigit(id[i])) {
            a_chiffre = 1;
        } else {
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                        GTK_DIALOG_MODAL,
                                                        GTK_MESSAGE_ERROR,
                                                        GTK_BUTTONS_OK,
                                                        "Erreur: L'identifiant ne doit contenir que des lettres et des chiffres (pas de symboles)!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return 0;
        }
    }
    
    if (!a_lettre || !a_chiffre) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Erreur: L'identifiant doit contenir au moins une lettre ET un chiffre!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return 0;
    }
    
    return 1; 
}
// Fonction complète pour valider l'ID (format + unicité)
int valider_id(const char *id, GtkWidget *parent_window) {
    if (!valider_format_id(id, parent_window)) {
        return 0;
    }
    
    if (id_existe(id)) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Erreur: Cet identifiant existe déjà! Veuillez en choisir un autre.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return 0;
    }
    
    return 1; 
}

// Fonction pour valider l'ID lors de la modification (permet le même ID)
int valider_id_modification(const char *id_nouveau, const char *id_ancien, GtkWidget *parent_window) {
  
    if (!valider_format_id(id_nouveau, parent_window)) {
        return 0;
    }
    
    if (strcmp(id_nouveau, id_ancien) == 0) {
        return 1;
    }
    
    if (id_existe(id_nouveau)) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Erreur: Cet identifiant existe déjà! Veuillez en choisir un autre.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return 0;
    }
    
    return 1; 
}

//////////////////////////////Interface Ajouter///////////////////////////////////
void on_GCIA_buttonEnregistrer_clicked(GtkWidget *button, gpointer user_data)
{
    
    GtkWidget *entryID = lookup_widget(button, "GCIA_entryID");
    GtkWidget *entryNom = lookup_widget(button, "GCIA_entryNom");
    GtkWidget *entryAdresse = lookup_widget(button, "GCIA_entryAdresse");
    GtkWidget *entryTel = lookup_widget(button, "GCIA_entryTel");
    GtkWidget *entryEmail = lookup_widget(button, "GCIA_entryEmail");
    GtkWidget *entryType = lookup_widget(button, "GCIA_comboType");
    GtkWidget *spinCapacite = lookup_widget(button, "GCIA_spinbutton1");
    GtkWidget *textview = lookup_widget(button, "GCIA_textview1");
    
    const char *id = gtk_entry_get_text(GTK_ENTRY(entryID));
    const char *nom = gtk_entry_get_text(GTK_ENTRY(entryNom));
    const char *adresse = gtk_entry_get_text(GTK_ENTRY(entryAdresse));
    const char *tel = gtk_entry_get_text(GTK_ENTRY(entryTel));
    if (!valider_telephone(tel, NULL)) {
        return; 
    }
    
    const char *email = gtk_entry_get_text(GTK_ENTRY(entryEmail));
    const char *type = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(entryType)->entry));
    gint capacite = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinCapacite));

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *description = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    
    if (!valider_id(id, NULL)) {
        g_free(description);
        return;
    }

    if (!valider_champ_obligatoire(nom, "Le nom du centre")) return;
    if (!valider_champ_obligatoire(id, "Identifiant")) return;
    if (!valider_champ_obligatoire(adresse, "Adresse")) return;
    if (!valider_telephone(tel, NULL)) {
        return; 
    }
    if (!valider_champ_obligatoire(email, "Email")) return;
    if (!valider_champ_obligatoire(type, "Type")) return;
    if (strlen(public_vise) == 0) {
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_ERROR,
                                                GTK_BUTTONS_OK,
                                                "Erreur: Veuillez sélectionner un public visé!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    g_free(description);
    return;
    }

    
    Ajouter(id,nom,adresse,tel,email,type,capacite,public_vise, description);

    printf("Donné enregistré : %s\t %s\t  %s\t  %s\t  %s\t %s\t %d\t %s\t %s\n", id,nom,adresse,tel,email,type,capacite,public_vise, description);
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                            GTK_DIALOG_MODAL,
                                            GTK_MESSAGE_INFO,
                                            GTK_BUTTONS_OK,
                                            "Centre ajouté avec succès!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    g_free(description);
    
}

void on_GCIA_comboType_entry_changed(GtkEntry *entry, gpointer user_data)
{
    const char *text = gtk_entry_get_text(entry);
    if (text && *text != '\0') {
        printf("Texte saisi : %s\n", text);
    }
}
void on_GCIA_comboType_list_selected(GtkList *list, GtkWidget *child, gpointer user_data)
{
   
    GtkEntry *entry = GTK_ENTRY(user_data);
    const char *text = gtk_entry_get_text(entry);

    if (text && *text != '\0') {
        printf("Élément choisi : %s\n", text);
    }
}

void
on_GCIA_radiobutton2_toggled(GtkToggleButton *togglebutton,gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
        	strcpy(public_vise, "Adultes uniquement");
       		printf("Public visé sélectionné : %s\n", public_vise);
   	}
}

void
on_GCIA_radiobutton4_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
        	strcpy(public_vise, "Tout publics");
        	printf("Public visé sélectionné : %s\n", public_vise);
    	}
}

void
on_GCIA_radiobuttonE_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
  	      strcpy(public_vise, "Enfants/Ados");
  	      printf("Public visé sélectionné : %s\n", public_vise);
  	}
}



void
on_GCIA_spinbutton1_value_changed      (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
	gint valeur = gtk_spin_button_get_value_as_int(spinbutton);
        printf("Capacité modifiée : %d\n", valeur);
}
void
on_GCIA_Renitialiser_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *entryID = lookup_widget(GTK_WIDGET(button), "GCIA_entryID");
    GtkWidget *entryNom = lookup_widget(GTK_WIDGET(button), "GCIA_entryNom");
    GtkWidget *entryAdresse = lookup_widget(GTK_WIDGET(button), "GCIA_entryAdresse");   
    GtkWidget *entryTel = lookup_widget(GTK_WIDGET(button), "GCIA_entryTel");
    GtkWidget *entryEmail = lookup_widget(GTK_WIDGET(button), "GCIA_entryEmail");
    GtkWidget *comboType = lookup_widget(GTK_WIDGET(button), "GCIA_comboType");
    GtkWidget *spinCapacite = lookup_widget(GTK_WIDGET(button), "GCIA_spinbutton1");
    GtkWidget *radiobuttonTP = lookup_widget(GTK_WIDGET(button), "GCIA_radiobutton4");
    GtkWidget *radiobuttonAU = lookup_widget(GTK_WIDGET(button), "GCIA_radiobutton2");
    GtkWidget *radiobuttonEA = lookup_widget(GTK_WIDGET(button), "GCIA_radiobuttonE");
    GtkWidget *textview = lookup_widget(GTK_WIDGET(button), "GCIA_textview1");

    printf("=== Réinitialisation de l'interface Ajouter ===\n");

    if (entryID) {
       gtk_entry_set_text(GTK_ENTRY(entryID), "");
       printf("ID vidé\n");
    }
    if (entryNom) {
       gtk_entry_set_text(GTK_ENTRY(entryNom), "");
       printf("Nom vidé\n");
    }
    if (entryAdresse) {
       gtk_entry_set_text(GTK_ENTRY(entryAdresse), "");
       printf("Adresse vidée\n");
    }   
    if (entryTel) {
       gtk_entry_set_text(GTK_ENTRY(entryTel), "");
       printf("Téléphone vidé\n");
    }
    if (entryEmail) {
        gtk_entry_set_text(GTK_ENTRY(entryEmail), "");
        printf("Email vidé\n");
    }  

    if (comboType) {
       GtkEntry *combo_entry = GTK_ENTRY(GTK_COMBO(comboType)->entry);
       gtk_entry_set_text(combo_entry, "");
       printf("Type vidé\n");
    }

    if (spinCapacite) {
       gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinCapacite), 0);
       printf("Capacité réinitialisée à 0\n");
    }

    if (radiobuttonTP) {
       gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobuttonTP), FALSE);
    }
    if (radiobuttonAU) {
       gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobuttonAU), FALSE);
    }
    if (radiobuttonEA) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobuttonEA), FALSE);
    }
    printf("RadioButtons désélectionnés\n");

    if (textview) {
       GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
       gtk_text_buffer_set_text(buffer, "", -1);
       printf("Description vidée\n");
    }

    strcpy(public_vise, "");
    printf("Variable public_vise réinitialisée\n");

    printf("=== Interface Ajouter réinitialisée avec succès ===\n");

}

void
on_GCIA_buttonGestion_centre_clicked   (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *notebook;
    GtkWidget *GCIP_fixed;
    
    notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
    
    GCIP_fixed = lookup_widget(GTK_WIDGET(button), "GCIP_fixed");
    
    gtk_widget_show(GCIP_fixed);
    
    gtk_widget_show_all(GCIP_fixed);
    
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
}


void
on_GCIA_buttonModifier_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *notebook;
    GtkWidget *GCIM_fixed;
    
    notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
    
    GCIM_fixed = lookup_widget(GTK_WIDGET(button), "GCIM_fixed");
    
    gtk_widget_show(GCIM_fixed);
    
    gtk_widget_show_all(GCIM_fixed);
    
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 2);	
}
void
on_GCIA_buttonprecedent_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *notebook;
    GtkWidget *GCIP_fixed;
    
    notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
    
    GCIP_fixed = lookup_widget(GTK_WIDGET(button), "GCIP_fixed");
    
    gtk_widget_show(GCIP_fixed);
    
    gtk_widget_show_all(GCIP_fixed);
    
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
}


void
on_GCIA_buttonquitter_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	confirm_quit(GTK_WIDGET(user_data)); 
}

//////////////////////////////Interface Modifier///////////////////////////////////
void
on_GCIM_buttonChercher_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
    
    GtkWidget *entryChercher = lookup_widget(GTK_WIDGET(button), "GCIM_entryChercher");
    GtkWidget *labelID = lookup_widget(GTK_WIDGET(button), "GCIM_labelID");
    GtkWidget *entryNom = lookup_widget(GTK_WIDGET(button), "GCIM_entryNom");
    GtkWidget *entryAdresse = lookup_widget(GTK_WIDGET(button), "GCIM_entryAdresse");
    GtkWidget *entryTel = lookup_widget(GTK_WIDGET(button), "GCIM_entryTel");
    GtkWidget *entryEmail = lookup_widget(GTK_WIDGET(button), "GCIM_entryEmail");
    GtkWidget *comboType = lookup_widget(GTK_WIDGET(button), "GCIM_comboType");
    GtkWidget *spinCapacite = lookup_widget(GTK_WIDGET(button), "GCIM_spinbutton1");
    GtkWidget *radiobuttonTP = lookup_widget(GTK_WIDGET(button), "GCIM_radiobuttonTP");
    GtkWidget *radiobuttonAU = lookup_widget(GTK_WIDGET(button), "GCIM_radiobuttonAU");
    GtkWidget *radiobuttonEA = lookup_widget(GTK_WIDGET(button), "GCIM_radiobuttonEA");
    GtkWidget *textview = lookup_widget(GTK_WIDGET(button), "GCIM_textview1");
    
    // Récupérer l'ID saisi
    const char *id_recherche = gtk_entry_get_text(GTK_ENTRY(entryChercher));
    if (!valider_champ_obligatoire(id_recherche, "Identifiant")) return;
    // Chercher le centre
    Centre c = Chercher(id_recherche);
    
    // Vérifier si le centre a été trouvé
    if (strlen(c.id) == 0) {
        gtk_label_set_text(GTK_LABEL(labelID), "ID n'existe pas!");
        printf("ID %s introuvable.\n", id_recherche);
    } else {
        gtk_label_set_text(GTK_LABEL(labelID), "ID bien trouvé!");
        printf("Centre trouvé : %s\n", c.nom);
        
        // Remplir les champs 
        if (entryNom) 
            gtk_entry_set_text(GTK_ENTRY(entryNom), c.nom);
        if (entryAdresse) 
            gtk_entry_set_text(GTK_ENTRY(entryAdresse), c.adresse);
        if (entryTel) 
            gtk_entry_set_text(GTK_ENTRY(entryTel), c.tel);
        if (entryEmail) 
            gtk_entry_set_text(GTK_ENTRY(entryEmail), c.email);
        if (comboType) {
            GtkEntry *combo_entry = GTK_ENTRY(GTK_COMBO(comboType)->entry);
            gtk_entry_set_text(combo_entry, c.type);
        }
        if (spinCapacite) 
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinCapacite), c.capacite);
        if (strcmp(c.public_vise, "Tout publics") == 0) {
            if (radiobuttonTP)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobuttonTP), TRUE);
        } else if (strcmp(c.public_vise, "Adultes uniquement") == 0) {
            if (radiobuttonAU)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobuttonAU), TRUE);
        } else if (strcmp(c.public_vise, "Enfants/Ados") == 0) {
            if (radiobuttonEA)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobuttonEA), TRUE);
        }
        if (textview) {
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
            gtk_text_buffer_set_text(buffer, c.description, -1);
        }
    }
}

void
on_GCIM_spinbutton1_value_changed      (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{

}

void on_GCIM_buttonEnregistrer_clicked(GtkButton *button, gpointer user_data)
{
    // Récupérer tous les widgets
    GtkWidget *entryChercher = lookup_widget(GTK_WIDGET(button), "GCIM_entryChercher");
    GtkWidget *entryNom = lookup_widget(GTK_WIDGET(button), "GCIM_entryNom");
    GtkWidget *entryAdresse = lookup_widget(GTK_WIDGET(button), "GCIM_entryAdresse");
    GtkWidget *entryTel = lookup_widget(GTK_WIDGET(button), "GCIM_entryTel");
    GtkWidget *entryEmail = lookup_widget(GTK_WIDGET(button), "GCIM_entryEmail");
    GtkWidget *comboType = lookup_widget(GTK_WIDGET(button), "GCIM_comboType");
    GtkWidget *spinCapacite = lookup_widget(GTK_WIDGET(button), "GCIM_spinbutton1");
    GtkWidget *radiobuttonTP = lookup_widget(GTK_WIDGET(button), "GCIM_radiobuttonTP");
    GtkWidget *radiobuttonAU = lookup_widget(GTK_WIDGET(button), "GCIM_radiobuttonAU");
    GtkWidget *radiobuttonEA = lookup_widget(GTK_WIDGET(button), "GCIM_radiobuttonEA");
    GtkWidget *textview = lookup_widget(GTK_WIDGET(button), "GCIM_textview1");
    
    // Récupérer l'ID (celui qui a été cherché)
    const char *id = gtk_entry_get_text(GTK_ENTRY(entryChercher));
    if (!valider_champ_obligatoire(id, "Identifiant")) return;
    
    // Récupérer les nouvelles valeurs
    const char *nom = gtk_entry_get_text(GTK_ENTRY(entryNom));
    const char *adresse = gtk_entry_get_text(GTK_ENTRY(entryAdresse));
    const char *tel = gtk_entry_get_text(GTK_ENTRY(entryTel));
    const char *email = gtk_entry_get_text(GTK_ENTRY(entryEmail));
    const char *type = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboType)->entry));
    gint capacite = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinCapacite));
    
    // Vérifier que tous les champs sont remplis
    if (!valider_champ_obligatoire(nom, "Le nom du centre")) return;
    if (!valider_champ_obligatoire(adresse, "Adresse")) return;
    if (!valider_telephone(tel, NULL)) {
        return; 
    }
    if (!valider_champ_obligatoire(email, "Email")) return;
    if (!valider_champ_obligatoire(type, "Type")) return;
    // Récupérer le public visé sélectionné
    char public_vise_modif[50] = "";
    
    if (radiobuttonTP && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobuttonTP))) {
        strcpy(public_vise_modif, "Tout publics");
    } else if (radiobuttonAU && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobuttonAU))) {
        strcpy(public_vise_modif, "Adultes uniquement");
    } else if (radiobuttonEA && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobuttonEA))) {
        strcpy(public_vise_modif, "Enfants/Ados");
    }
    
    // Vérifier qu'un public visé a été sélectionné
    if (strlen(public_vise_modif) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Erreur: Veuillez sélectionner un public visé!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Récupérer la description
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *description = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    
    // Debug - afficher les valeurs récupérées
    printf("=== DEBUG MODIFICATION ===\n");
    printf("ID: %s\n", id);
    printf("Nom: %s\n", nom);
    printf("Adresse: %s\n", adresse);
    printf("Tel: %s\n", tel);
    printf("Email: %s\n", email);
    printf("Type: %s\n", type);
    printf("Capacité: %d\n", capacite);
    printf("Public visé: %s\n", public_vise_modif);
    printf("Description: %s\n", description);
    printf("========================\n");
    
    
    int resultat = Modifier(id, nom, adresse, tel, email, type, capacite, public_vise_modif, description);
    
    
    if (resultat == 1) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_INFO,
                                                    GTK_BUTTONS_OK,
                                                    "Centre modifié avec succès!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        printf("Centre %s modifié avec succès!\n", id);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Erreur: Impossible de modifier le centre (ID non trouvé)!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    
    
    g_free(description);
}



void
on_GCIM_radiobuttonTP_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	
}

void
on_GCIM_radiobuttonAU_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	
}

void
on_GCIM_radiobuttonEA_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}
void
on_GCIM_buttonAjouter_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *notebook;
    GtkWidget *GCIA_fixed;
    
    notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
    
    GCIA_fixed = lookup_widget(GTK_WIDGET(button), "GCIA_fixed");

    gtk_widget_show(GCIA_fixed);
    
    gtk_widget_show_all(GCIA_fixed);
    
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1);
}


void
on_GCIM_buttonGestion_centre_clicked   (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *notebook;
    GtkWidget *GCIP_fixed;
    
    notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
    
    GCIP_fixed = lookup_widget(GTK_WIDGET(button), "GCIP_fixed");
    
    gtk_widget_show(GCIP_fixed);
    
    gtk_widget_show_all(GCIP_fixed);
    
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
}
void
on_GCIM_buttonprecedent_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *notebook;
    GtkWidget *GCIP_fixed;
    
    notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
    
    GCIP_fixed = lookup_widget(GTK_WIDGET(button), "GCIP_fixed");
    
    gtk_widget_show(GCIP_fixed);
    
    gtk_widget_show_all(GCIP_fixed);
    
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
}


void
on_GCIM_buttonquitter_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	confirm_quit(GTK_WIDGET(user_data)); 
}
//////////////////////////////Interface Principal///////////////////////////////////
void
on_GCIP_treeview_realize               (GtkWidget       *widget,
                                        gpointer         user_data)
{
    GtkWidget *treeview;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GList *columns;
    
    treeview = widget;  
    
    printf("=== TreeView realize appelé ===\n");
     
    columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview));
    
    if (g_list_length(columns) == 0) {
        printf("Création des colonnes...\n");
        
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("ID",
                                                           renderer,
                                                           "text", 0,
                                                           NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Nom",
                                                           renderer,
                                                           "text", 1,
                                                           NULL);
	gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);        

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Num_tel",                             
                                                           renderer,
                                                           "text", 2,
                                                           NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

	renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Email",                             
                                                           renderer,
                                                           "text", 3,
                                                           NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

	renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Type",                           
                                                           renderer,
                                                           "text", 4,
                                                           NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

	renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Capacité",                             
                                                           renderer,
                                                           "text", 5,
                                                           NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("Public",                              
                                                           renderer,
                                                           "text", 6,
                                                           NULL);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        
        
        
        printf("Colonnes créées avec succès\n");
    }
    
    g_list_free(columns);
    
    printf("Chargement des données...\n");
    Afficher_Centres(treeview);
    printf("=== TreeView initialisé ===\n");

}


void
on_GCIP_button_Actualiser_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *treeview;
    
    treeview = lookup_widget(GTK_WIDGET(button), "GCIP_treeview");
    
    if (treeview == NULL) {
        printf("ERREUR: TreeView introuvable!\n");
        return;
    }
    
    printf("=== Actualisation du TreeView ===\n");
    
    Afficher_Centres(treeview);
    
    printf("TreeView actualisé avec succès\n");
}


void
on_GCIP_button_Chercher_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *entry;
    GtkWidget *treeview;
    const char *id_recherche;
    
    printf("=== Recherche lancée ===\n");
    
    entry = lookup_widget(GTK_WIDGET(button), "GCIP_entry_Chercher");
    if (entry == NULL) {
        printf("ERREUR: Entry de recherche introuvable!\n");
        return;
    }
    
    id_recherche = gtk_entry_get_text(GTK_ENTRY(entry));
    
    if (id_recherche == NULL || strlen(id_recherche) == 0) {
        printf("ATTENTION: Aucun ID saisi pour la recherche\n");
        
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_WARNING,
                                                    GTK_BUTTONS_OK,
                                                    "Veuillez saisir un ID à rechercher");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    printf("Recherche de l'ID: '%s'\n", id_recherche);
    
    treeview = lookup_widget(GTK_WIDGET(button), "GCIP_treeview");
    if (treeview == NULL) {
        printf("ERREUR: TreeView introuvable!\n");
        return;
    }
    
    if (Rechercher_Centre(treeview, id_recherche)) {
        printf("Centre trouvé et sélectionné\n");
    } else {
        printf("Centre non trouvé\n");
        
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_INFO,
                                                    GTK_BUTTONS_OK,
                                                    "Aucun centre trouvé avec l'ID: %s",
                                                    id_recherche);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    
    printf("=== Fin de recherche ===\n");
}
void
on_GCIP_buttonAjouter_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *notebook;
    GtkWidget *GCIA_fixed;
    
    notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
    
    GCIA_fixed = lookup_widget(GTK_WIDGET(button), "GCIA_fixed");

    gtk_widget_show(GCIA_fixed);
    
    gtk_widget_show_all(GCIA_fixed);
    
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1);
}

void
on_GCIP_buttonModifier_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *notebook;
    GtkWidget *GCIM_fixed;
    GtkWidget *treeview;
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *id;
    
    // SCÉNARIO 1
    treeview = lookup_widget(GTK_WIDGET(button), "GCIP_treeview");
    if (treeview == NULL) {
        printf("ERREUR: TreeView introuvable!\n");
        return;
    }
    
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
       
        gtk_tree_model_get(model, &iter, 0, &id, -1);
        printf("Centre sélectionné pour modification: '%s'\n", id);
        
        notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
        GCIM_fixed = lookup_widget(GTK_WIDGET(button), "GCIM_fixed");
        
        gtk_widget_show(GCIM_fixed);
        gtk_widget_show_all(GCIM_fixed);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 2);
        
       
        GtkWidget *entryChercher = lookup_widget(GCIM_fixed, "GCIM_entryChercher");
        GtkWidget *labelID = lookup_widget(GCIM_fixed, "GCIM_labelID");
        GtkWidget *entryNom = lookup_widget(GCIM_fixed, "GCIM_entryNom");
        GtkWidget *entryAdresse = lookup_widget(GCIM_fixed, "GCIM_entryAdresse");
        GtkWidget *entryTel = lookup_widget(GCIM_fixed, "GCIM_entryTel");
        GtkWidget *entryEmail = lookup_widget(GCIM_fixed, "GCIM_entryEmail");
        GtkWidget *comboType = lookup_widget(GCIM_fixed, "GCIM_comboType");
        GtkWidget *spinCapacite = lookup_widget(GCIM_fixed, "GCIM_spinbutton1");
        GtkWidget *radiobuttonTP = lookup_widget(GCIM_fixed, "GCIM_radiobuttonTP");
        GtkWidget *radiobuttonAU = lookup_widget(GCIM_fixed, "GCIM_radiobuttonAU");
        GtkWidget *radiobuttonEA = lookup_widget(GCIM_fixed, "GCIM_radiobuttonEA");
        GtkWidget *textview = lookup_widget(GCIM_fixed, "GCIM_textview1");
        
        if (entryChercher)
            gtk_entry_set_text(GTK_ENTRY(entryChercher), id);
        
        Centre c = Chercher(id);
        
        if (strlen(c.id) > 0) {
            if (labelID)
                gtk_label_set_text(GTK_LABEL(labelID), "ID bien trouvé!");
            
            if (entryNom) 
                gtk_entry_set_text(GTK_ENTRY(entryNom), c.nom);
            if (entryAdresse) 
                gtk_entry_set_text(GTK_ENTRY(entryAdresse), c.adresse);
            if (entryTel) 
                gtk_entry_set_text(GTK_ENTRY(entryTel), c.tel);
            if (entryEmail) 
                gtk_entry_set_text(GTK_ENTRY(entryEmail), c.email);
            if (comboType) {
                GtkEntry *combo_entry = GTK_ENTRY(GTK_COMBO(comboType)->entry);
                gtk_entry_set_text(combo_entry, c.type);
            }
            if (spinCapacite) 
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinCapacite), c.capacite);
            
            if (strcmp(c.public_vise, "Tout publics") == 0) {
                if (radiobuttonTP)
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobuttonTP), TRUE);
            } else if (strcmp(c.public_vise, "Adultes uniquement") == 0) {
                if (radiobuttonAU)
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobuttonAU), TRUE);
            } else if (strcmp(c.public_vise, "Enfants/Ados") == 0) {
                if (radiobuttonEA)
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobuttonEA), TRUE);
            }
            
            if (textview) {
                GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
                gtk_text_buffer_set_text(buffer, c.description, -1);
            }
            
            printf("Données du centre chargées automatiquement\n");
        }
        
        g_free(id);
        
    } else {
        // SCÉNARIO 2
        printf("Aucune ligne sélectionnée - mode manuel\n");
        
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_INFO,
                                                    GTK_BUTTONS_OK,
                                                    "Aucun centre sélectionné.\nVous pouvez saisir manuellement l'ID à modifier.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        
        notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
        GCIM_fixed = lookup_widget(GTK_WIDGET(button), "GCIM_fixed");
        
        gtk_widget_show(GCIM_fixed);
        gtk_widget_show_all(GCIM_fixed);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 2);
        
        
        GtkWidget *entryChercher = lookup_widget(GCIM_fixed, "GCIM_entryChercher");
        GtkWidget *labelID = lookup_widget(GCIM_fixed, "GCIM_labelID");
        GtkWidget *entryNom = lookup_widget(GCIM_fixed, "GCIM_entryNom");
        GtkWidget *entryAdresse = lookup_widget(GCIM_fixed, "GCIM_entryAdresse");
        GtkWidget *entryTel = lookup_widget(GCIM_fixed, "GCIM_entryTel");
        GtkWidget *entryEmail = lookup_widget(GCIM_fixed, "GCIM_entryEmail");
        GtkWidget *comboType = lookup_widget(GCIM_fixed, "GCIM_comboType");
        GtkWidget *spinCapacite = lookup_widget(GCIM_fixed, "GCIM_spinbutton1");
        GtkWidget *textview = lookup_widget(GCIM_fixed, "GCIM_textview1");
        
        if (entryChercher)
            gtk_entry_set_text(GTK_ENTRY(entryChercher), "");
        if (labelID)
            gtk_label_set_text(GTK_LABEL(labelID), "");
        if (entryNom)
            gtk_entry_set_text(GTK_ENTRY(entryNom), "");
        if (entryAdresse)
            gtk_entry_set_text(GTK_ENTRY(entryAdresse), "");
        if (entryTel)
            gtk_entry_set_text(GTK_ENTRY(entryTel), "");
        if (entryEmail)
            gtk_entry_set_text(GTK_ENTRY(entryEmail), "");
        if (comboType) {
            GtkEntry *combo_entry = GTK_ENTRY(GTK_COMBO(comboType)->entry);
            gtk_entry_set_text(combo_entry, "");
        }
        if (spinCapacite)
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinCapacite), 0);
        if (textview) {
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
            gtk_text_buffer_set_text(buffer, "", -1);
        }
    }
}

void
on_GCIP_button_Supprimer_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *treeview;
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *id;
    
    treeview = lookup_widget(GTK_WIDGET(button), "GCIP_treeview");
    if (treeview == NULL) {
        printf("ERREUR: TreeView introuvable!\n");
        return;
    }
    
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        printf("Aucune ligne sélectionnée\n");
        
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_WARNING,
                                                    GTK_BUTTONS_OK,
                                                    "Veuillez sélectionner un centre à supprimer");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    gtk_tree_model_get(model, &iter, 0, &id, -1);
    printf("ID sélectionné: '%s'\n", id);
    
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_QUESTION,
                                                GTK_BUTTONS_YES_NO,
                                                "Êtes-vous sûr de vouloir supprimer le centre '%s' ?",
                                                id);
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    if (response == GTK_RESPONSE_YES) {
        if (Supprimer(id)) {
            printf("Centre supprimé avec succès\n");
            
           
            Afficher_Centres(treeview);
            
           
            dialog = gtk_message_dialog_new(NULL,
                                           GTK_DIALOG_MODAL,
                                           GTK_MESSAGE_INFO,
                                           GTK_BUTTONS_OK,
                                           "Centre supprimé avec succès!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        } else {
            printf("Échec de la suppression\n");
            
            dialog = gtk_message_dialog_new(NULL,
                                           GTK_DIALOG_MODAL,
                                           GTK_MESSAGE_ERROR,
                                           GTK_BUTTONS_OK,
                                           "Erreur lors de la suppression");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
    } else {
        printf("Suppression annulée par l'utilisateur\n");
    }
    
    g_free(id);
    printf("=== Fin Supprimer ===\n");
}

void
on_GCIP_buttonquitter_clicked(GtkButton *button,
                              gpointer   user_data)
{
    confirm_quit(GTK_WIDGET(user_data));
}
//////////////////////////////Interface Inscri Coach au centre///////////////////////////////////

void
on_TIEC_buttonEnregistrer_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *entryNom = lookup_widget(GTK_WIDGET(button), "TIEC_entryNom");
    GtkWidget *entryPrenom = lookup_widget(GTK_WIDGET(button), "TIEC_entryPrenom");
    GtkWidget *entryTel = lookup_widget(GTK_WIDGET(button), "TIEC_entryTel");
    GtkWidget *entryEmail = lookup_widget(GTK_WIDGET(button), "TIEC_entryEmail");
    GtkWidget *entryAdresse = lookup_widget(GTK_WIDGET(button), "TIEC_entryAdresse");
    GtkWidget *entryCIN = lookup_widget(GTK_WIDGET(button), "TIEC_entryCIN");
    
    GtkWidget *spinJour = lookup_widget(GTK_WIDGET(button), "TIEC_spinbuttonJour");
    GtkWidget *spinMois = lookup_widget(GTK_WIDGET(button), "TIEC_spinbuttonMois");
    GtkWidget *spinAnnee = lookup_widget(GTK_WIDGET(button), "TIEC_spinbuttonAnnees");
    
    GtkWidget *radioPermanent = lookup_widget(GTK_WIDGET(button), "TIEC_radiobuttonPermanent");
    GtkWidget *radioContractuel = lookup_widget(GTK_WIDGET(button), "TIEC_radiobuttonContractuel");
    GtkWidget *radioVacataire = lookup_widget(GTK_WIDGET(button), "TIEC_radiobuttonVacataire");
    
    GtkWidget *checkMatin = lookup_widget(GTK_WIDGET(button), "TIEC_checkbuttonMatin");
    GtkWidget *checkApresMidi = lookup_widget(GTK_WIDGET(button), "TIEC_checkbuttonApr__s_midi");
    GtkWidget *checkSoir = lookup_widget(GTK_WIDGET(button), "TIEC_checkbuttonSoir");
    GtkWidget *checkWeekEnd = lookup_widget(GTK_WIDGET(button), "TIEC_checkbuttonWeek_end");
    
    GtkWidget *comboSpecialite = lookup_widget(GTK_WIDGET(button), "TIEC_comboSpecialite");
    
    
    
    // Valeurs de base
    const char *nom = gtk_entry_get_text(GTK_ENTRY(entryNom));
    const char *prenom = gtk_entry_get_text(GTK_ENTRY(entryPrenom));
    const char *tel = gtk_entry_get_text(GTK_ENTRY(entryTel));
    const char *email = gtk_entry_get_text(GTK_ENTRY(entryEmail));
    const char *adresse = gtk_entry_get_text(GTK_ENTRY(entryAdresse));
    const char *cin = gtk_entry_get_text(GTK_ENTRY(entryCIN));
    

    if (!valider_champ_obligatoire(nom, "Le nom")) return;
    if (!valider_champ_obligatoire(prenom, "Le prénom")) return;
    if (!valider_champ_obligatoire(tel, "Le numéro de téléphone")) return;
    if (!valider_telephone(tel, NULL)) return;
    if (!valider_champ_obligatoire(email, "L'email")) return;
    if (!valider_champ_obligatoire(adresse, "L'adresse")) return;
    if (!valider_champ_obligatoire(cin, "Le CIN/Passeport")) return;
    if (!valider_cin(cin, NULL)) return;

   
    int jour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinJour));
    int mois = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinMois));
    int annee = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinAnnee));
    
    char date_nai[50];
    sprintf(date_nai, "%02d/%02d/%04d", jour, mois, annee);
    
    
    const char *specialite = "";
    if (comboSpecialite) {
        specialite = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(comboSpecialite)->entry));
    }
    
    if (!valider_champ_obligatoire(specialite, "La spécialité")) return;
    
  
    char contrat[50] = "";
    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioPermanent))) {
        strcpy(contrat, "Permanent");
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioContractuel))) {
        strcpy(contrat, "Contractuel");
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioVacataire))) {
        strcpy(contrat, "Vacataire");
    }
    
    if (strlen(contrat) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Erreur: Veuillez sélectionner un type de contrat!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
  
    char disponibilite[200] = "";
    int premiere = 1;  
    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkMatin))) {
        strcat(disponibilite, "Matin");
        premiere = 0;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkApresMidi))) {
        if (!premiere) strcat(disponibilite, ", ");
        strcat(disponibilite, "Après-midi");
        premiere = 0;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkSoir))) {
        if (!premiere) strcat(disponibilite, ", ");
        strcat(disponibilite, "Soir");
        premiere = 0;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkWeekEnd))) {
        if (!premiere) strcat(disponibilite, ", ");
        strcat(disponibilite, "Week-end");
        premiere = 0;
    }
    
    if (strlen(disponibilite) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Erreur: Veuillez sélectionner au moins une disponibilité!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    Ajouter_Coach_Centre(nom, prenom, date_nai, tel, email, adresse, cin, 
                         specialite, disponibilite, contrat);
    
    printf("Données enregistrées : %s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
           nom, prenom, date_nai, tel, email, adresse, cin, specialite, disponibilite, contrat);
    
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_INFO,
                                                GTK_BUTTONS_OK,
                                                "Votre demande a bien été reçue. Merci!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
   
    
}
void
on_TIEC_spinbuttonJour_value_changed   (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{

}


void
on_TIEC_spinbuttonMois_value_changed   (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{

}


void
on_TIEC_spinbuttonAnnees_value_changed (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{

}


void
on_TIEC_radiobuttonPermanent_toggled   (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
           strcpy(type_contrat, "Permanent");
           printf("Type de contrat sélectionné : %s\n", type_contrat);
        }
}


void
on_TIEC_radiobuttonContractuel_toggled (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
           strcpy(type_contrat, "Contractuel");
           printf("Type de contrat sélectionné : %s\n", type_contrat);
        }
}


void
on_TIEC_radiobuttonVacataire_toggled   (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
           strcpy(type_contrat, "Vacataire");
           printf("Type de contrat sélectionné : %s\n", type_contrat);
        }
}


void
on_TIEC_checkbuttonMatin_toggled       (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_TIEC_checkbuttonApr__s_midi_toggled (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_TIEC_checkbuttonSoir_toggled        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_TIEC_checkbuttonWeek_end_toggled    (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_TIEC_buttonRenitialiser_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
	 GtkWidget *entryNom = lookup_widget(GTK_WIDGET(button), "TIEC_entryNom");
    GtkWidget *entryPrenom = lookup_widget(GTK_WIDGET(button), "TIEC_entryPrenom");
    GtkWidget *entryTel = lookup_widget(GTK_WIDGET(button), "TIEC_entryTel");
    GtkWidget *entryEmail = lookup_widget(GTK_WIDGET(button), "TIEC_entryEmail");
    GtkWidget *entryAdresse = lookup_widget(GTK_WIDGET(button), "TIEC_entryAdresse");
    GtkWidget *entryCIN = lookup_widget(GTK_WIDGET(button), "TIEC_entryCIN");
    
    GtkWidget *spinJour = lookup_widget(GTK_WIDGET(button), "TIEC_spinbuttonJour");
    GtkWidget *spinMois = lookup_widget(GTK_WIDGET(button), "TIEC_spinbuttonMois");
    GtkWidget *spinAnnee = lookup_widget(GTK_WIDGET(button), "TIEC_spinbuttonAnnees");
    
    GtkWidget *radioPermanent = lookup_widget(GTK_WIDGET(button), "TIEC_radiobuttonPermanent");
    GtkWidget *radioContractuel = lookup_widget(GTK_WIDGET(button), "TIEC_radiobuttonContractuel");
    GtkWidget *radioVacataire = lookup_widget(GTK_WIDGET(button), "TIEC_radiobuttonVacataire");
    
    GtkWidget *checkMatin = lookup_widget(GTK_WIDGET(button), "TIEC_checkbuttonMatin");
    GtkWidget *checkApresMidi = lookup_widget(GTK_WIDGET(button), "TIEC_checkbuttonApr__s_midi");
    GtkWidget *checkSoir = lookup_widget(GTK_WIDGET(button), "TIEC_checkbuttonSoir");
    GtkWidget *checkWeekEnd = lookup_widget(GTK_WIDGET(button), "TIEC_checkbuttonWeek_end");
    
    GtkWidget *comboSpecialite = lookup_widget(GTK_WIDGET(button), "TIEC_comboSpecialite");
    
    if (entryNom) {
        gtk_entry_set_text(GTK_ENTRY(entryNom), "");
        printf("Nom vidé\n");
    }
    if (entryPrenom) {
        gtk_entry_set_text(GTK_ENTRY(entryPrenom), "");
        printf("Prénom vidé\n");
    }
    if (entryTel) {
        gtk_entry_set_text(GTK_ENTRY(entryTel), "");
        printf("Téléphone vidé\n");
    }
    if (entryEmail) {
        gtk_entry_set_text(GTK_ENTRY(entryEmail), "");
        printf("Email vidé\n");
    }
    if (entryAdresse) {
        gtk_entry_set_text(GTK_ENTRY(entryAdresse), "");
        printf("Adresse vidée\n");
    }
    if (entryCIN) {
        gtk_entry_set_text(GTK_ENTRY(entryCIN), "");
        printf("CIN vidé\n");
    }
    
    if (spinJour) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinJour), 1);
        printf("Jour réinitialisé à 1\n");
    }
    if (spinMois) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinMois), 1);
        printf("Mois réinitialisé à 1\n");
    }
    if (spinAnnee) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinAnnee), 2000);
        printf("Année réinitialisée à 2000\n");
    }
    
    if (radioPermanent) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioPermanent), FALSE);
    }
    if (radioContractuel) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioContractuel), FALSE);
    }
    if (radioVacataire) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioVacataire), FALSE);
    }
    printf("RadioButtons type de contrat désélectionnés\n");
     
    if (checkMatin) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkMatin), FALSE);
    }
    if (checkApresMidi) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkApresMidi), FALSE);
    }
    if (checkSoir) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkSoir), FALSE);
    }
    if (checkWeekEnd) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkWeekEnd), FALSE);
    }
    printf("CheckButtons disponibilité décochés\n");
    
    if (comboSpecialite) {
        GtkEntry *combo_entry = GTK_ENTRY(GTK_COMBO(comboSpecialite)->entry);
        gtk_entry_set_text(combo_entry, "");
        printf("Spécialité vidée\n");
    }
    
    printf("=== Interface Coach réinitialisée avec succès ===\n");
}
void
on_TIEC_buttonPrecedent_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{

}
void
on_TIEC_buttonQuitter_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	 confirm_quit(GTK_WIDGET(user_data));
}



/////////// BBBBBBBBBBBBBBBB






/* ============================================= */
/* VARIABLES GLOBALES POUR RADIO BUTTONS */
/* ============================================= */
static int z = 1;  // Statut pour AJOUTER (1=en_cours, 2=terminer, 3=annule)
static int w = 1;  // Statut pour MODIFIER (1=en_cours, 2=terminer, 3=annule)

/* ============================================= */
/* HELPER FUNCTION - Get active text from combo */
/* ============================================= */
gchar* get_combo_box_active_text(GtkComboBox *combo_box) {
    GtkTreeIter iter;
    GtkTreeModel *model;
    gchar *text = NULL;
    
    if (gtk_combo_box_get_active_iter(combo_box, &iter)) {
        model = gtk_combo_box_get_model(combo_box);
        gtk_tree_model_get(model, &iter, 0, &text, -1);
    }
    
    return text;
}

/* ============================================= */
/* HELPER FUNCTION - Get text from ComboBoxEntry */
/* ============================================= */
const gchar* get_combo_box_entry_text(GtkWidget *comboboxentry) {
    GtkWidget *entry = gtk_bin_get_child(GTK_BIN(comboboxentry));
    if (entry && GTK_IS_ENTRY(entry)) {
        return gtk_entry_get_text(GTK_ENTRY(entry));
    }
    return "";
}

/* ============================================= */
/* HELPER FUNCTION - Set text in ComboBoxEntry */
/* ============================================= */
void set_combo_box_entry_text(GtkWidget *comboboxentry, const gchar *text) {
    GtkWidget *entry = gtk_bin_get_child(GTK_BIN(comboboxentry));
    if (entry && GTK_IS_ENTRY(entry)) {
        gtk_entry_set_text(GTK_ENTRY(entry), text);
    }
}

/* ============================================= */
/* GESTION ÉVÉNEMENTS - FENÊTRE PRINCIPALE */
/* ============================================= */

void on_window_gestion_evenement_show(GtkWidget *widget, gpointer user_data) {
    GtkWidget *treeview = lookup_widget(widget, "treeview_hela_evenment");
    if (treeview) {
        afficher_evenements(treeview);
    }
}

void on_button_chercher_evenement_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *entry = lookup_widget(window, "entry_hela_recherche");
    GtkWidget *treeview = lookup_widget(window, "treeview_hela_evenment");

    if (!entry || !treeview) return;

    const gchar *critere = gtk_entry_get_text(GTK_ENTRY(entry));

    if (strlen(critere) == 0) {
        afficher_evenements(treeview);
    } else {
        int found = rechercher_evenements(critere, treeview);
        if (found == 0) {
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                "Aucun événement trouvé avec ce critère.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
    }
}

void on_button_ouvrir_ajout_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window_ajout = create_window_ajouter_evenement();
    gtk_widget_show(window_ajout);
}

void on_button_modifer_ouvrire_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *treeview = lookup_widget(window, "treeview_hela_evenment");

    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
            "Veuillez sélectionner un événement à modifier.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    gint id;
    gtk_tree_model_get(model, &iter, 0, &id, -1);

    evenement ev = trouver_evenement(id);
    if (ev.id_event == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "Événement introuvable !");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    GtkWidget *win_mod = create_window_modifier_evenement();

    // Remplir les champs
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(win_mod, "entry_nom_event_mod")), ev.nom_event);
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(win_mod, "description_event_mod")), ev.description);
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(win_mod, "entry_salle_modif_hela")), ev.salle);

    // Set prix value in comboboxentry
    char prix_str[20];
    snprintf(prix_str, sizeof(prix_str), "%.2f", ev.prix);
    GtkWidget *combo_prix = lookup_widget(win_mod, "comboboxentry_modif_prix_hela");
    if (combo_prix) {
        set_combo_box_entry_text(combo_prix, prix_str);
    }

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(win_mod, "spinbutton_mod_jrs_hela")), ev.date_e.j);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(win_mod, "spinbutton_mois_mod_hela")), ev.date_e.m);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(win_mod, "spinbutton_annee_mod_hela")), ev.date_e.a);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(win_mod, "spinbutton_heure_mod")), ev.heure_e.heure);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(win_mod, "spinbutton_min_hela_mod")), ev.heure_e.minute);

    // Sélectionner le bon type dans le combobox
    GtkComboBox *cb_type = GTK_COMBO_BOX(lookup_widget(win_mod, "combobox_type_event_mod"));
    GtkTreeModel *model_type = gtk_combo_box_get_model(cb_type);
    GtkTreeIter iter_type;
    gboolean valid = gtk_tree_model_get_iter_first(model_type, &iter_type);
    int i = 0;
    while (valid) {
        gchar *text;
        gtk_tree_model_get(model_type, &iter_type, 0, &text, -1);
        if (g_strcmp0(text, ev.type_event) == 0) {
            gtk_combo_box_set_active(cb_type, i);
            g_free(text);
            break;
        }
        g_free(text);
        valid = gtk_tree_model_iter_next(model_type, &iter_type);
        i++;
    }

    // Sélectionner le bon centre
    GtkComboBox *cb_centre = GTK_COMBO_BOX(lookup_widget(win_mod, "combobox_centre_mod_event"));
    remplir_combobox_centres(GTK_WIDGET(cb_centre));
    GtkTreeModel *model_centre = gtk_combo_box_get_model(cb_centre);
    GtkTreeIter iter_centre;
    valid = gtk_tree_model_get_iter_first(model_centre, &iter_centre);
    i = 0;
    while (valid) {
        gchar *text;
        gtk_tree_model_get(model_centre, &iter_centre, 0, &text, -1);
        if (g_strcmp0(text, ev.centre) == 0) {
            gtk_combo_box_set_active(cb_centre, i);
            g_free(text);
            break;
        }
        g_free(text);
        valid = gtk_tree_model_iter_next(model_centre, &iter_centre);
        i++;
    }

    // Initialiser w selon le statut
    if (g_strcmp0(ev.statut, "en_cours") == 0) {
        w = 1;
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(win_mod, "radiobutton_encours_mod")), TRUE);
    } else if (g_strcmp0(ev.statut, "terminer") == 0) {
        w = 2;
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(win_mod, "radiobutton_terminer_mod")), TRUE);
    } else {
        w = 3;
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(win_mod, "radiobutton_annulee_mod")), TRUE);
    }

    g_object_set_data(G_OBJECT(win_mod), "event_id", GINT_TO_POINTER(ev.id_event));
    g_object_set_data(G_OBJECT(win_mod), "parent_window", window);
    gtk_widget_show(win_mod);
}

void on_button_supp_hela_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *treeview = lookup_widget(window, "treeview_hela_evenment");

    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        GtkWidget *d = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "Sélectionnez un événement à supprimer.");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        return;
    }

    gint id;
    gchar *nom;
    gtk_tree_model_get(model, &iter, 0, &id, 1, &nom, -1);

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
        GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
        "Supprimer l'événement \"%s\" ?", nom);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {
        supprimer_evenement(id);
        afficher_evenements(treeview);
        GtkWidget *ok = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Supprimé !");
        gtk_dialog_run(GTK_DIALOG(ok));
        gtk_widget_destroy(ok);
    }
    gtk_widget_destroy(dialog);
    g_free(nom);
}

/* ============================================= */
/* FENÊTRE AJOUTER */
/* ============================================= */

void on_window_ajouter_evenement_show(GtkWidget *widget, gpointer user_data) {
    // Remplir le combobox des centres
    GtkWidget *combo_centre = lookup_widget(widget, "combobox_centre_hela");
    if (combo_centre) {
        remplir_combobox_centres(combo_centre);
    }

    // Par défaut : en cours
    GtkWidget *radio = lookup_widget(widget, "radiobutton_encours");
    if (radio) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
        z = 1;
    }
}

void on_button_ajouter_event_hela_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *win = gtk_widget_get_toplevel(GTK_WIDGET(button));

    const gchar *nom = gtk_entry_get_text(GTK_ENTRY(lookup_widget(win, "entry_nom_event")));
    const gchar *desc = gtk_entry_get_text(GTK_ENTRY(lookup_widget(win, "entry_description_hela")));
    const gchar *salle = gtk_entry_get_text(GTK_ENTRY(lookup_widget(win, "entry_salle_hela")));
    
    // Get prix from comboboxentry
    GtkWidget *combo_prix = lookup_widget(win, "comboboxentry_prix_hela");
    const gchar *prix_str = get_combo_box_entry_text(combo_prix);
    float prix = atof(prix_str);
    
    // Use helper function to get combo box text
    gchar *type = get_combo_box_active_text(GTK_COMBO_BOX(lookup_widget(win, "combobox_type_hela")));
    gchar *centre = get_combo_box_active_text(GTK_COMBO_BOX(lookup_widget(win, "combobox_centre_hela")));

    int j = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(win, "spinbutton_jrs_hela")));
    int m = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(win, "spinbutton_mois_hela")));
    int a = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(win, "spinbutton_annee_hela")));
    int h = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(win, "spinbutton_heure")));
    int min = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(win, "spinbutton5")));

    if (strlen(nom) < 3) {
        GtkWidget *d = gtk_message_dialog_new(GTK_WINDOW(win), GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "Nom trop court !");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        g_free(type); g_free(centre);
        return;
    }

    if (strlen(salle) < 1) {
        GtkWidget *d = gtk_message_dialog_new(GTK_WINDOW(win), GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "Veuillez spécifier une salle !");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        g_free(type); g_free(centre);
        return;
    }

    if (prix <= 0) {
        GtkWidget *d = gtk_message_dialog_new(GTK_WINDOW(win), GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "Le prix doit être supérieur à 0 !");
        gtk_dialog_run(GTK_DIALOG(d));
        gtk_widget_destroy(d);
        g_free(type); g_free(centre);
        return;
    }

    evenement ev = {0};
    strcpy(ev.nom_event, nom);
    strcpy(ev.type_event, type ? type : "Autre");
    strcpy(ev.centre, centre ? centre : "Non_spécifié");
    strcpy(ev.salle, salle);
    strcpy(ev.description, desc);
    ev.prix = prix;
    ev.date_e.j = j; ev.date_e.m = m; ev.date_e.a = a;
    ev.heure_e.heure = h; ev.heure_e.minute = min;

    // Utiliser la variable z
    if (z == 1) strcpy(ev.statut, "en_cours");
    else if (z == 2) strcpy(ev.statut, "terminer");
    else strcpy(ev.statut, "Annulé");

    ajouter_evenement(ev);

    GtkWidget *ok = gtk_message_dialog_new(GTK_WINDOW(win), GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Événement ajouté !");
    gtk_dialog_run(GTK_DIALOG(ok));
    gtk_widget_destroy(ok);

    // NOUVEAU CODE: Rafraîchir le treeview de la fenêtre principale
    // Chercher la fenêtre de gestion des événements parmi les fenêtres ouvertes
    GList *windows = gtk_window_list_toplevels();
    GList *iter_win;
    for (iter_win = windows; iter_win != NULL; iter_win = iter_win->next) {
        GtkWidget *window = GTK_WIDGET(iter_win->data);
        if (GTK_IS_WINDOW(window)) {
            // Chercher le treeview dans cette fenêtre
            GtkWidget *treeview = lookup_widget(window, "treeview_hela_evenment");
            if (treeview) {
                // Rafraîchir le treeview
                afficher_evenements(treeview);
                break;
            }
        }
    }
    g_list_free(windows);

    // Fermer la fenêtre d'ajout
    gtk_widget_destroy(win);
    
    // Libérer la mémoire
    g_free(type);
    g_free(centre);
    
    // Réinitialiser z
    z = 1;
}

void on_button_annule_event_hela_clicked(GtkButton *button, gpointer user_data) {
    gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    z = 1;
}

/* ============================================= */
/* FENÊTRE MODIFIER */
/* ============================================= */

void on_window_modifier_evenement_show(GtkWidget *widget, gpointer user_data) {
    GtkWidget *combo_centre = lookup_widget(widget, "combobox_centre_mod_event");
    if (combo_centre) {
        remplir_combobox_centres(combo_centre);
    }
}

void on_button_modfier_event_hela_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *win = gtk_widget_get_toplevel(GTK_WIDGET(button));
    int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(win), "event_id"));

    if (id <= 0) return;

    const gchar *nom = gtk_entry_get_text(GTK_ENTRY(lookup_widget(win, "entry_nom_event_mod")));
    const gchar *desc = gtk_entry_get_text(GTK_ENTRY(lookup_widget(win, "description_event_mod")));
    const gchar *salle = gtk_entry_get_text(GTK_ENTRY(lookup_widget(win, "entry_salle_modif_hela")));
    
    // Get prix from comboboxentry
    GtkWidget *combo_prix = lookup_widget(win, "comboboxentry_modif_prix_hela");
    const gchar *prix_str = get_combo_box_entry_text(combo_prix);
    float prix = atof(prix_str);
    
    // Use helper function
    gchar *type = get_combo_box_active_text(GTK_COMBO_BOX(lookup_widget(win, "combobox_type_event_mod")));
    gchar *centre = get_combo_box_active_text(GTK_COMBO_BOX(lookup_widget(win, "combobox_centre_mod_event")));

    int j = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(win, "spinbutton_mod_jrs_hela")));
    int m = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(win, "spinbutton_mois_mod_hela")));
    int a = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(win, "spinbutton_annee_mod_hela")));
    int h = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(win, "spinbutton_heure_mod")));
    int min = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(win, "spinbutton_min_hela_mod")));

    evenement ev = trouver_evenement(id);
    ev.id_event = id;
    strcpy(ev.nom_event, nom);
    strcpy(ev.type_event, type ? type : "Autre");
    strcpy(ev.centre, centre ? centre : "Non_spécifié");
    strcpy(ev.salle, salle);
    strcpy(ev.description, desc);
    ev.prix = prix;
    ev.date_e.j = j; ev.date_e.m = m; ev.date_e.a = a;
    ev.heure_e.heure = h; ev.heure_e.minute = min;

    // Utiliser la variable w
    if (w == 1) strcpy(ev.statut, "en_cours");
    else if (w == 2) strcpy(ev.statut, "terminer");
    else strcpy(ev.statut, "Annulé");

    modifier_evenement(ev);

    GtkWidget *ok = gtk_message_dialog_new(GTK_WINDOW(win), GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Événement modifié !");
    gtk_dialog_run(GTK_DIALOG(ok));
    gtk_widget_destroy(ok);

    // Get parent window reference
    GtkWidget *parent = g_object_get_data(G_OBJECT(win), "parent_window");
    if (parent) {
        GtkWidget *tv = lookup_widget(parent, "treeview_hela_evenment");
        if (tv) afficher_evenements(tv);
    }

    gtk_widget_destroy(win);
    g_free(type);
    g_free(centre);
    w = 1;
}

void on_button_annulee_mod_hela_clicked(GtkButton *button, gpointer user_data) {
    gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    w = 1;
}

/* ============================================= */
/* RADIO BUTTONS - FENÊTRE AJOUTER */
/* ============================================= */
void on_radiobutton_encours_toggled(GtkToggleButton *b, gpointer d) {
    if (gtk_toggle_button_get_active(b)) z = 1;
}

void on_radiobutton_terminer_toggled(GtkToggleButton *b, gpointer d) {
    if (gtk_toggle_button_get_active(b)) z = 2;
}

void on_radiobutton_annule_toggled(GtkToggleButton *b, gpointer d) {
    if (gtk_toggle_button_get_active(b)) z = 3;
}

/* ============================================= */
/* RADIO BUTTONS - FENÊTRE MODIFIER */
/* ============================================= */
void on_radiobutton_encours_mod_toggled(GtkToggleButton *b, gpointer d) {
    if (gtk_toggle_button_get_active(b)) w = 1;
}

void on_radiobutton_terminer_mod_toggled(GtkToggleButton *b, gpointer d) {
    if (gtk_toggle_button_get_active(b)) w = 2;
}

void on_radiobutton_annulee_mod_toggled(GtkToggleButton *b, gpointer d) {
    if (gtk_toggle_button_get_active(b)) w = 3;
}

void
on_treeview_hela_evenment_cursor_changed
                                        (GtkTreeView     *treeview,
                                        gpointer         user_data)
{

}
/* ============================================= */
/* FENÊTRE INSCRIPTION MEMBRE AUX ÉVÉNEMENTS */
/* ============================================= */

void on_windowsinscireevent_show(GtkWidget *widget, gpointer user_data) {
    // Afficher tous les événements disponibles dans le treeview
    GtkWidget *treeview = lookup_widget(widget, "treeview_sincrire_event");
    if (treeview) {
        afficher_evenements_disponibles(treeview);
    }
}

void on_button_sinscire_event_hela_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    
    // Récupérer le CIN du membre
    GtkWidget *entry_cin = lookup_widget(window, "entry_cin_inscription_hela");
    const gchar *cin = gtk_entry_get_text(GTK_ENTRY(entry_cin));
    
    // Vérifier que le CIN n'est pas vide
    if (strlen(cin) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
            "Veuillez entrer votre CIN !");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Vérifier qu'un événement est sélectionné
    GtkWidget *treeview = lookup_widget(window, "treeview_sincrire_event");
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
            "Veuillez sélectionner un événement !");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Récupérer l'ID de l'événement sélectionné
    gint id_event;
    gchar *nom_event = NULL;
    gtk_tree_model_get(model, &iter, 0, &id_event, 1, &nom_event, -1);
    
    // Inscrire le membre à l'événement
    int result = inscrire_membre_event(cin, id_event);
    
    GtkWidget *dialog;
    if (result == 1) {
        dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
            "Inscription réussie à l'événement \"%s\" !", nom_event);
    } else if (result == -1) {
        dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "Événement introuvable !");
    } else if (result == -2) {
        dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
            "Vous êtes déjà inscrit à cet événement !");
    } else {
        dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "Erreur lors de l'inscription !");
    }
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    g_free(nom_event);
}

void on_button_voir_mes_inscription_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    
    // Récupérer le CIN du membre
    GtkWidget *entry_cin = lookup_widget(window, "entry_cin_inscription_hela");
    const gchar *cin = gtk_entry_get_text(GTK_ENTRY(entry_cin));
    
    // Vérifier que le CIN n'est pas vide
    if (strlen(cin) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
            "Veuillez entrer votre CIN !");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Créer une nouvelle fenêtre pour afficher les inscriptions
    GtkWidget *dialog_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog_window), "Mes Inscriptions");
    gtk_window_set_default_size(GTK_WINDOW(dialog_window), 900, 500);
    gtk_window_set_position(GTK_WINDOW(dialog_window), GTK_WIN_POS_CENTER);
    gtk_window_set_modal(GTK_WINDOW(dialog_window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog_window), GTK_WINDOW(window));
    
    // Stocker le CIN dans la fenêtre pour pouvoir le réutiliser
    g_object_set_data_full(G_OBJECT(dialog_window), "cin_membre", 
                          g_strdup(cin), (GDestroyNotify)g_free);
    
    // Créer un conteneur vertical
    GtkWidget *vbox = gtk_vbox_new(FALSE, 10);
    gtk_container_add(GTK_CONTAINER(dialog_window), vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    // Ajouter un label avec le CIN et statistiques
    int nb_inscriptions = compter_inscriptions_membre(cin);
    float total = calculer_total_inscriptions(cin);
    
    gchar *info_text = g_strdup_printf(
        "CIN: %s\nNombre d'inscriptions: %d\nMontant total: %.2f DT",
        cin, nb_inscriptions, total
    );
    
    GtkWidget *label_info = gtk_label_new(info_text);
    gtk_box_pack_start(GTK_BOX(vbox), label_info, FALSE, FALSE, 5);
    g_free(info_text);
    
    // Créer un scrolled window pour le treeview
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    
    // Créer le treeview
    GtkWidget *treeview = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled), treeview);
    
    // Afficher les inscriptions du membre
    afficher_inscriptions_membre(treeview, cin);
    
    // Ajouter un bouton pour supprimer une inscription
    GtkWidget *hbox_buttons = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_buttons, FALSE, FALSE, 5);
    
    GtkWidget *button_supprimer = gtk_button_new_with_label("Annuler l'inscription sélectionnée");
    gtk_box_pack_start(GTK_BOX(hbox_buttons), button_supprimer, FALSE, FALSE, 5);
    
    // Callback pour supprimer l'inscription - pass treeview as user_data
    g_signal_connect(button_supprimer, "clicked",
        G_CALLBACK(on_button_annuler_inscription_clicked),
        treeview);
    
    GtkWidget *button_fermer = gtk_button_new_with_label("Fermer");
    gtk_box_pack_start(GTK_BOX(hbox_buttons), button_fermer, FALSE, FALSE, 5);
    
    g_signal_connect_swapped(button_fermer, "clicked",
        G_CALLBACK(gtk_widget_destroy),
        dialog_window);
    
    gtk_widget_show_all(dialog_window);
}

void on_button_annuler_inscription_clicked(GtkButton *button, gpointer user_data) {
    // user_data contains the treeview widget
    GtkWidget *treeview = GTK_WIDGET(user_data);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        GtkWidget *window = gtk_widget_get_toplevel(treeview);
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
            "Veuillez sélectionner une inscription à annuler !");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Récupérer l'ID de l'inscription
    gint id_inscription;
    gchar *nom_event = NULL;
    gtk_tree_model_get(model, &iter, 0, &id_inscription, 2, &nom_event, -1);
    
    GtkWidget *window = gtk_widget_get_toplevel(treeview);
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
        GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
        "Voulez-vous vraiment annuler votre inscription à \"%s\" ?", nom_event);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {
        supprimer_inscription_event(id_inscription);
        
        // Rafraîchir le treeview - récupérer le CIN depuis le parent
        GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
        const gchar *cin = g_object_get_data(G_OBJECT(parent_window), "cin_membre");
        
        if (cin) {
            afficher_inscriptions_membre(treeview, cin);
        }
        
        GtkWidget *ok_dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
            "Inscription annulée avec succès !");
        gtk_dialog_run(GTK_DIALOG(ok_dialog));
        gtk_widget_destroy(ok_dialog);
    }
    
    gtk_widget_destroy(dialog);
    g_free(nom_event);
}

void on_treeview_sincrire_event_row_activated(GtkTreeView *treeview,
                                               GtkTreePath *path,
                                               GtkTreeViewColumn *column,
                                               gpointer user_data)
{
    // Double-clic pour voir les détails de l'événement
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    model = gtk_tree_view_get_model(treeview);
    
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        gint id_event;
        gchar *nom = NULL;
        gchar *type = NULL;
        gchar *centre = NULL;
        gchar *salle = NULL;
        gchar *date = NULL;
        gchar *heure = NULL;
        gchar *prix = NULL;
        gchar *statut = NULL;
        
        // Récupérer toutes les informations de l'événement
        gtk_tree_model_get(model, &iter,
                          0, &id_event,
                          1, &nom,
                          2, &type,
                          3, &centre,
                          4, &salle,
                          5, &date,
                          6, &heure,
                          7, &prix,
                          8, &statut,
                          -1);
        
        // Récupérer l'événement complet pour avoir la description
        evenement ev = trouver_evenement(id_event);
        
        // Créer un dialogue pour afficher les détails
        GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(treeview));
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Détails de l'événement");
        
        // Formater le message avec tous les détails
        gchar *message = g_strdup_printf(
            "<b>Nom:</b> %s\n"
            "<b>Type:</b> %s\n"
            "<b>Centre:</b> %s\n"
            "<b>Salle:</b> %s\n"
            "<b>Date:</b> %s\n"
            "<b>Heure:</b> %s\n"
            "<b>Prix:</b> %s\n"
            "<b>Statut:</b> %s\n\n"
            "<b>Description:</b>\n%s",
            nom ? nom : "N/A",
            type ? type : "N/A",
            centre ? centre : "N/A",
            salle ? salle : "N/A",
            date ? date : "N/A",
            heure ? heure : "N/A",
            prix ? prix : "N/A",
            statut ? statut : "N/A",
            ev.description
        );
        
        gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(dialog), message);
        
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        // Libérer la mémoire
        g_free(message);
        g_free(nom);
        g_free(type);
        g_free(centre);
        g_free(salle);
        g_free(date);
        g_free(heure);
        g_free(prix);
        g_free(statut);
    }
}

void on_treeview_sincrire_event_cursor_changed(GtkTreeView *treeview,
                                               gpointer user_data)
{
    // Afficher un aperçu rapide de l'événement sélectionné dans la barre de statut
    // ou mettre à jour des informations dans l'interface
    
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    selection = gtk_tree_view_get_selection(treeview);
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gint id_event;
        gchar *nom = NULL;
        gchar *prix = NULL;
        
        gtk_tree_model_get(model, &iter,
                          0, &id_event,
                          1, &nom,
                          7, &prix,
                          -1);
        
        // Vous pouvez utiliser cette information pour mettre à jour l'interface
        // Par exemple, afficher le nom et le prix dans un label de statut
        // Pour l'instant, on utilise juste le tooltip
        gchar *tooltip = g_strdup_printf(
            "Événement sélectionné: %s (Prix: %s)\nDouble-cliquez pour voir les détails complets",
            nom ? nom : "N/A",
            prix ? prix : "N/A"
        );
        
        gtk_widget_set_tooltip_text(GTK_WIDGET(treeview), tooltip);
        
        g_free(tooltip);
        g_free(nom);
        g_free(prix);
    } else {
        gtk_widget_set_tooltip_text(GTK_WIDGET(treeview), 
            "Sélectionnez un événement pour vous inscrire");
    }
}


//////////CCCCCCCCCCCCCCCCC





// Chemin vers le fichier des entraineurs (fichiers dans le même répertoire que
// l'exécutable)
#define FILE_ENTRAINEURS "entraineurs.txt"
#define FILE_INSCRIPTIONS "inscriptions.txt"

// Variables globales pour stocker l'état
static char id_selected[20] = "";
static GtkWidget *current_treeview = NULL;

#define DEBUG_PRINT(msg)                                                       \
  g_print("[DEBUG] %s:%d - %s\n", __FILE__, __LINE__, msg)

// ==================== FONCTIONS UTILITAIRES ====================

void afficher_message(GtkWidget *parent, const char *message,
                      GtkMessageType type) {
  DEBUG_PRINT("Début afficher_message");

  if (!parent || !GTK_IS_WIDGET(parent)) {
    g_warning("Parent est NULL ou invalide dans afficher_message!");
    return;
  }

  // Vérifier que le parent est toujours valide
  if (!gtk_widget_get_visible(parent) && !GTK_IS_WINDOW(parent)) {
    g_warning("Parent n'est pas visible dans afficher_message!");
    return;
  }

  GtkWidget *dialog = gtk_message_dialog_new(
      GTK_WINDOW(parent), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      type, GTK_BUTTONS_OK, "%s", message);

  if (dialog) {
    DEBUG_PRINT("Dialog créé");
    gtk_dialog_run(GTK_DIALOG(dialog));
    DEBUG_PRINT("Dialog fermé");
    gtk_widget_destroy(dialog);
  }

  DEBUG_PRINT("Fin afficher_message");
}

char *get_entry_text(GtkWidget *entry) {
  if (!entry) {
    g_warning("Entry est NULL dans get_entry_text!");
    return "";
  }
  return (char *)gtk_entry_get_text(GTK_ENTRY(entry));
}

void set_entry_text(GtkWidget *entry, const char *text) {
  if (!entry) {
    g_warning("Entry est NULL dans set_entry_text!");
    return;
  }
  gtk_entry_set_text(GTK_ENTRY(entry), text);
}

void set_textview_text(GtkWidget *textview, const char *text) {
  if (!textview)
    return;
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
  gtk_text_buffer_set_text(buffer, text, -1);
}

char *format_date(int jour, int mois, int annee) {
  static char date_str[32];
  snprintf(date_str, sizeof(date_str), "%02d/%02d/%04d", jour, mois, annee);
  return date_str;
}

// ==================== GESTION DU TREEVIEW ====================

void afficher_entraineurs(GtkWidget *treeview) {
  DEBUG_PRINT("Début afficher_entraineurs");

  // Afficher le répertoire courant pour debug
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    g_print("[DEBUG] Répertoire courant: %s\n", cwd);
  }
  g_print("[DEBUG] Tentative d'ouverture: entraineurs.txt\n");

  if (!treeview) {
    g_warning("TreeView est NULL!");
    return;
  }

  GtkListStore *store;
  GtkTreeIter iter;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GList *columns;
  int num_columns;

  // Vérifier correctement si les colonnes existent déjà
  columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview));
  num_columns = g_list_length(columns);
  g_list_free(columns);

  // Créer les colonnes SEULEMENT si elles n'existent pas
  if (num_columns == 0) {
    DEBUG_PRINT("Création des colonnes");
    renderer = gtk_cell_renderer_text_new();

    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0,
                                                      NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text",
                                                      1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Prénom", renderer,
                                                      "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Téléphone", renderer,
                                                      "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Sexe", renderer, "text",
                                                      4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Spécialité", renderer,
                                                      "text", 5, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
  } else {
    DEBUG_PRINT("Colonnes déjà créées, rafraîchissement du contenu seulement");
  }

  DEBUG_PRINT("Création du store");
  store = gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                             G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

  DEBUG_PRINT("Lecture du fichier");
  g_print("\n========================================\n");
  g_print("AFFICHER_ENTRAINEURS - Tentative lecture\n");
  g_print("Fichier: %s\n", FILE_ENTRAINEURS);
  g_print("========================================\n");

  FILE *f = fopen(FILE_ENTRAINEURS, "r");
  if (f != NULL) {
    g_print("✓ Fichier OUVERT avec succès!\n");
    char ligne[1024];
    int count = 0;

    while (fgets(ligne, sizeof(ligne), f) != NULL) {
      Entraineur E;

      // Parser la ligne manuellement pour gérer les champs vides
      char *token;
      char *saveptr;
      int field = 0;

      // Copier la ligne car strtok_r la modifie
      char ligne_copy[1024];
      strncpy(ligne_copy, ligne, sizeof(ligne_copy) - 1);
      ligne_copy[sizeof(ligne_copy) - 1] = '\0';

      // Nettoyer le saut de ligne
      ligne_copy[strcspn(ligne_copy, "\n")] = '\0';

      // Ignorer les lignes vides
      if (strlen(ligne_copy) == 0)
        continue;

      token = strtok_r(ligne_copy, "|", &saveptr);
      while (token != NULL && field < 12) {
        switch (field) {
        case 0:
          strncpy(E.id, token, sizeof(E.id) - 1);
          E.id[sizeof(E.id) - 1] = '\0';
          break;
        case 1:
          strncpy(E.nom, token, sizeof(E.nom) - 1);
          E.nom[sizeof(E.nom) - 1] = '\0';
          break;
        case 2:
          strncpy(E.prenom, token, sizeof(E.prenom) - 1);
          E.prenom[sizeof(E.prenom) - 1] = '\0';
          break;
        case 3:
          strncpy(E.mot_de_passe, token, sizeof(E.mot_de_passe) - 1);
          E.mot_de_passe[sizeof(E.mot_de_passe) - 1] = '\0';
          break;
        case 4:
          strncpy(E.telephone, token, sizeof(E.telephone) - 1);
          E.telephone[sizeof(E.telephone) - 1] = '\0';
          break;
        case 5:
          strncpy(E.sexe, token, sizeof(E.sexe) - 1);
          E.sexe[sizeof(E.sexe) - 1] = '\0';
          break;
        case 6:
          strncpy(E.photo, token, sizeof(E.photo) - 1);
          E.photo[sizeof(E.photo) - 1] = '\0';
          break;
        case 7:
          strncpy(E.cv, token, sizeof(E.cv) - 1);
          E.cv[sizeof(E.cv) - 1] = '\0';
          break;
        case 8:
          strncpy(E.specialite, token, sizeof(E.specialite) - 1);
          E.specialite[sizeof(E.specialite) - 1] = '\0';
          break;
        case 9:
          E.date_naissance.jour = atoi(token);
          break;
        case 10:
          E.date_naissance.mois = atoi(token);
          break;
        case 11:
          E.date_naissance.annee = atoi(token);
          break;
        }
        field++;
        token = strtok_r(NULL, "|", &saveptr);
      }

      if (field >= 12) { // Vérifier qu'on a tous les champs
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, E.id, 1, E.nom, 2, E.prenom, 3,
                           E.telephone, 4, E.sexe, 5, E.specialite, -1);
        count++;
      }
    }
    fclose(f);
    g_print("[DEBUG] %d entraîneurs chargés\n", count);
  } else {
    DEBUG_PRINT("Fichier entraineurs.txt non trouvé ou erreur de lecture");
  }

  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
  g_object_unref(store);

  DEBUG_PRINT("Fin afficher_entraineurs");
}

void on_treeview_row_activated(GtkTreeView *treeview, GtkTreePath *path,
                               GtkTreeViewColumn *column, gpointer user_data) {
  DEBUG_PRINT("Début on_treeview_row_activated");

  GtkTreeModel *model;
  GtkTreeIter iter;
  gchar *id;

  model = gtk_tree_view_get_model(treeview);
  if (gtk_tree_model_get_iter(model, &iter, path)) {
    gtk_tree_model_get(model, &iter, 0, &id, -1);
    strcpy(id_selected, id);
    g_print("========================================\n");
    g_print("✓ LIGNE SÉLECTIONNÉE (double-clic)\n");
    g_print("ID sélectionné: '%s'\n", id_selected);
    g_print("========================================\n\n");
    g_free(id);
    current_treeview = GTK_WIDGET(treeview);
  }

  DEBUG_PRINT("Fin on_treeview_row_activated");
}

// Gestionnaire pour la sélection simple (un seul clic)
void on_treeview_cursor_changed(GtkTreeView *treeview, gpointer user_data) {
  DEBUG_PRINT("Début on_treeview_cursor_changed");

  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  gchar *id;

  selection = gtk_tree_view_get_selection(treeview);
  if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
    gtk_tree_model_get(model, &iter, 0, &id, -1);
    strcpy(id_selected, id);
    g_print("========================================\n");
    g_print("✓ LIGNE SÉLECTIONNÉE (simple clic)\n");
    g_print("ID sélectionné: '%s'\n", id_selected);
    g_print("========================================\n\n");
    g_free(id);
    current_treeview = GTK_WIDGET(treeview);
  } else {
    g_print("[DEBUG] Pas de sélection\n");
  }

  DEBUG_PRINT("Fin on_treeview_cursor_changed");
}

// ==================== CALLBACKS LOGIN ====================

void on_button47_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button47_mohamed_clicked (Espace Admin)");

  GtkWidget *login_mohamed = gtk_widget_get_toplevel(button);
  g_print("[DEBUG] login_mohamed = %p\n", (void *)login_mohamed);

  GtkWidget *login_admin = create_login_admin_mohamed();
  g_print("[DEBUG] login_admin créé = %p\n", (void *)login_admin);

  DEBUG_PRINT("Destruction de login_mohamed");
  gtk_widget_destroy(login_mohamed);

  DEBUG_PRINT("Affichage de login_admin");
  gtk_window_set_default_size(GTK_WINDOW(login_admin), 1024, 768);
  gtk_widget_show(login_admin);

  DEBUG_PRINT("FIN on_button47_mohamed_clicked");
}

void on_button48_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button48_mohamed_clicked (Espace Entraineur)");

  GtkWidget *login_mohamed = gtk_widget_get_toplevel(button);
  GtkWidget *login_entraineur = create_login_entraineur_mohamed();

  gtk_widget_destroy(login_mohamed);
  gtk_window_set_default_size(GTK_WINDOW(login_entraineur), 1024, 768);
  gtk_widget_show(login_entraineur);

  DEBUG_PRINT("FIN on_button48_mohamed_clicked");
}

void on_button54_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button54_mohamed_clicked (Login Admin)");

  GtkWidget *login_admin = gtk_widget_get_toplevel(button);
  g_print("[DEBUG] login_admin toplevel = %p\n", (void *)login_admin);

  DEBUG_PRINT("Récupération des entries");
  GtkWidget *entry_id = lookup_widget(button, "entry52_mohamed");
  GtkWidget *entry_mdp = lookup_widget(button, "entry53_mohamed");

  g_print("[DEBUG] entry_id = %p, entry_mdp = %p\n", (void *)entry_id,
          (void *)entry_mdp);

  if (!entry_id || !entry_mdp) {
    g_warning("Les entries sont NULL!");
    return;
  }

  char *id = get_entry_text(entry_id);
  char *mdp = get_entry_text(entry_mdp);

  g_print("[DEBUG] ID saisi: '%s', MDP: '%s'\n", id, mdp);

  if (strcmp(id, "admin") == 0 && strcmp(mdp, "admin123") == 0) {
    DEBUG_PRINT("Login réussi, création de l'accueil");

    GtkWidget *acceuil = create_acceuil_mohamed();
    g_print("[DEBUG] acceuil créé = %p\n", (void *)acceuil);

    GtkWidget *treeview = lookup_widget(acceuil, "treeview5_mohamed");
    g_print("[DEBUG] treeview = %p\n", (void *)treeview);

    if (treeview) {
      DEBUG_PRINT("Affichage des entraîneurs");
      afficher_entraineurs(treeview);
      g_signal_connect(G_OBJECT(treeview), "row-activated",
                       G_CALLBACK(on_treeview_row_activated), NULL);
      g_signal_connect(G_OBJECT(treeview), "cursor-changed",
                       G_CALLBACK(on_treeview_cursor_changed), NULL);
    } else {
      g_warning("TreeView est NULL!");
    }

    DEBUG_PRINT("Destruction de login_admin");
    gtk_widget_destroy(login_admin);

    DEBUG_PRINT("Affichage de l'accueil");
    gtk_window_set_default_size(GTK_WINDOW(acceuil), 1024, 768);
    gtk_widget_show(acceuil);

    DEBUG_PRINT("Login admin terminé avec succès");
  } else {
    DEBUG_PRINT("Login échoué");
    afficher_message(login_admin, "Identifiant ou mot de passe incorrect!",
                     GTK_MESSAGE_ERROR);
  }

  DEBUG_PRINT("FIN on_button54_mohamed_clicked");
}

void on_button53_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("Annuler Login Admin");

  GtkWidget *login_admin = gtk_widget_get_toplevel(button);
  GtkWidget *login_mohamed = create_login_mohamed();

  gtk_widget_destroy(login_admin);
  gtk_window_set_default_size(GTK_WINDOW(login_mohamed), 1024, 768);
  gtk_widget_show(login_mohamed);
}

// ==================== CALLBACKS ACCUEIL ====================

void on_button70_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button70_mohamed_clicked (Ajouter)");

  GtkWidget *acceuil = gtk_widget_get_toplevel(button);
  GtkWidget *ajouter = create_ajouter_un_entraineur_mohamed();

  gtk_widget_destroy(acceuil);
  gtk_window_set_default_size(GTK_WINDOW(ajouter), 1024, 768);
  gtk_widget_show(ajouter);

  DEBUG_PRINT("FIN on_button70_mohamed_clicked");
}

void on_button55_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button55_mohamed_clicked (Enregistrer)");

  GtkWidget *window_ajouter = gtk_widget_get_toplevel(button);
  g_print("[DEBUG] window_ajouter = %p\n", (void *)window_ajouter);

  DEBUG_PRINT("Récupération de TOUS les widgets");

  // --- Récupération des entries ---
  GtkWidget *entry_id = lookup_widget(window_ajouter, "entry57_mohamed");
  GtkWidget *entry_nom = lookup_widget(window_ajouter, "entry56_mohamed");
  GtkWidget *entry_prenom = lookup_widget(window_ajouter, "entry55_mohamed");
  GtkWidget *entry_mdp = lookup_widget(window_ajouter, "entry54_mohamed");
  GtkWidget *entry_tel = lookup_widget(window_ajouter, "entry58_mohamed");

  // --- Récupération des autres widgets ---
  GtkWidget *checkbox = lookup_widget(window_ajouter, "checkbutton4_mohamed");
  GtkWidget *calendar = lookup_widget(window_ajouter, "calendar3_mohamed");
  GtkWidget *radio_homme =
      lookup_widget(window_ajouter, "radiobutton17_mohamed");
  GtkWidget *radio_femme =
      lookup_widget(window_ajouter, "radiobutton18_mohamed");
  GtkWidget *combobox = lookup_widget(window_ajouter, "comboboxentry6_mohamed");

  g_print("[DEBUG] Widgets: id=%p nom=%p prenom=%p mdp=%p tel=%p\n",
          (void *)entry_id, (void *)entry_nom, (void *)entry_prenom,
          (void *)entry_mdp, (void *)entry_tel);
  g_print("[DEBUG] Autres: checkbox=%p calendar=%p radio_h=%p radio_f=%p "
          "combo=%p\n",
          (void *)checkbox, (void *)calendar, (void *)radio_homme,
          (void *)radio_femme, (void *)combobox);

  // --- Vérifier que tous les widgets existent ---
  if (!entry_id || !entry_nom || !entry_prenom || !entry_mdp || !entry_tel ||
      !checkbox || !calendar || !radio_homme || !radio_femme || !combobox) {
    g_warning("Un ou plusieurs widgets sont NULL!");
    afficher_message(window_ajouter, "Erreur interne: widgets non trouvés",
                     GTK_MESSAGE_ERROR);
    return;
  }

  // --- VALIDATION 1: Vérifier que le checkbox est coché ---
  if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox))) {
    afficher_message(
        window_ajouter,
        "Vous devez cocher la case de confirmation pour continuer!",
        GTK_MESSAGE_WARNING);
    return;
  }

  // --- Récupération des données ---
  Entraineur E;
  const char *id = get_entry_text(entry_id);
  const char *nom = get_entry_text(entry_nom);
  const char *prenom = get_entry_text(entry_prenom);
  const char *mdp = get_entry_text(entry_mdp);
  const char *tel = get_entry_text(entry_tel);

  g_print("[DEBUG] Données: ID='%s' Nom='%s' Prenom='%s'\n", id, nom, prenom);

  // --- VALIDATION 2: Champs obligatoires ---
  if (strlen(id) == 0) {
    afficher_message(window_ajouter, "L'identifiant est obligatoire!",
                     GTK_MESSAGE_WARNING);
    return;
  }
  if (strlen(nom) == 0) {
    afficher_message(window_ajouter, "Le nom est obligatoire!",
                     GTK_MESSAGE_WARNING);
    return;
  }
  if (strlen(prenom) == 0) {
    afficher_message(window_ajouter, "Le prénom est obligatoire!",
                     GTK_MESSAGE_WARNING);
    return;
  }
  if (strlen(mdp) == 0) {
    afficher_message(window_ajouter, "Le mot de passe est obligatoire!",
                     GTK_MESSAGE_WARNING);
    return;
  }
  if (strlen(tel) == 0) {
    afficher_message(window_ajouter, "Le téléphone est obligatoire!",
                     GTK_MESSAGE_WARNING);
    return;
  }

  // --- VALIDATION 3: Vérifier l'unicité de l'ID ---
  if (idExiste(FILE_ENTRAINEURS, id)) {
    afficher_message(
        window_ajouter,
        "Cet identifiant existe déjà! Veuillez en choisir un autre.",
        GTK_MESSAGE_ERROR);
    return;
  }

  // --- VALIDATION 4: Mot de passe (8+ caractères, maj, min, chiffres) ---
  if (!validerMotDePasse(mdp)) {
    afficher_message(
        window_ajouter,
        "Le mot de passe doit contenir au moins 8 caractères, incluant:\n"
        "- Au moins une majuscule\n"
        "- Au moins une minuscule\n"
        "- Au moins un chiffre",
        GTK_MESSAGE_WARNING);
    return;
  }

  // --- VALIDATION 5: Téléphone (8 chiffres) ---
  if (!validerTelephone(tel)) {
    afficher_message(
        window_ajouter,
        "Le numéro de téléphone doit contenir exactement 8 chiffres!",
        GTK_MESSAGE_WARNING);
    return;
  }

  // --- Récupération du SEXE depuis les radiobuttons ---
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_homme))) {
    strcpy(E.sexe, "Homme");
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_femme))) {
    strcpy(E.sexe, "Femme");
  } else {
    strcpy(E.sexe, "Homme"); // Par défaut
  }

  // --- Récupération de la SPÉCIALITÉ depuis le combobox ---
  gchar *specialite = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combobox));
  if (specialite && strlen(specialite) > 0) {
    strncpy(E.specialite, specialite, sizeof(E.specialite) - 1);
    E.specialite[sizeof(E.specialite) - 1] = '\0';
    g_free(specialite);
  } else {
    strcpy(E.specialite, "Musculation"); // Par défaut
  }

  // --- Récupération de la DATE depuis le calendrier ---
  guint year, month, day;
  gtk_calendar_get_date(GTK_CALENDAR(calendar), &year, &month, &day);
  E.date_naissance.jour = day;
  E.date_naissance.mois = month + 1; // GTK mois commence à 0
  E.date_naissance.annee = year;

  g_print("[DEBUG] Date: %d/%d/%d\n", E.date_naissance.jour,
          E.date_naissance.mois, E.date_naissance.annee);

  // --- VALIDATION 6: Date valide ---
  if (!validerDate(E.date_naissance.jour, E.date_naissance.mois,
                   E.date_naissance.annee)) {
    afficher_message(window_ajouter, "La date de naissance est invalide!",
                     GTK_MESSAGE_WARNING);
    return;
  }

  // --- Copier les données validées ---
  strncpy(E.id, id, sizeof(E.id) - 1);
  E.id[sizeof(E.id) - 1] = '\0';
  strncpy(E.nom, nom, sizeof(E.nom) - 1);
  E.nom[sizeof(E.nom) - 1] = '\0';
  strncpy(E.prenom, prenom, sizeof(E.prenom) - 1);
  E.prenom[sizeof(E.prenom) - 1] = '\0';
  strncpy(E.mot_de_passe, mdp, sizeof(E.mot_de_passe) - 1);
  E.mot_de_passe[sizeof(E.mot_de_passe) - 1] = '\0';
  strncpy(E.telephone, tel, sizeof(E.telephone) - 1);
  E.telephone[sizeof(E.telephone) - 1] = '\0';

  strcpy(E.photo, "");
  strcpy(E.cv, "");

  g_print("[DEBUG] Toutes les validations passées. Ajout de: %s %s (ID: %s)\n",
          E.nom, E.prenom, E.id);

  // --- Ajout dans le fichier ---
  DEBUG_PRINT("Ajout dans le fichier");
  if (ajouter_entraineur(FILE_ENTRAINEURS, E)) {
    g_print("[DEBUG] Entraineur ajouté avec succès: %s %s\n", E.nom, E.prenom);

    DEBUG_PRINT("Affichage du message de succès");
    afficher_message(window_ajouter, "Entraineur ajouté avec succès!",
                     GTK_MESSAGE_INFO);

    DEBUG_PRINT("Création de l'accueil");
    GtkWidget *acceuil = create_acceuil_mohamed();

    DEBUG_PRINT("Récupération du treeview");
    GtkWidget *treeview = lookup_widget(acceuil, "treeview5_mohamed");

    if (treeview) {
      DEBUG_PRINT("Affichage des entraîneurs");
      afficher_entraineurs(treeview);
      g_signal_connect(G_OBJECT(treeview), "row-activated",
                       G_CALLBACK(on_treeview_row_activated), NULL);
      g_signal_connect(G_OBJECT(treeview), "cursor-changed",
                       G_CALLBACK(on_treeview_cursor_changed), NULL);
    }

    DEBUG_PRINT("Destruction de la fenêtre ajout");
    gtk_widget_destroy(window_ajouter);

    DEBUG_PRINT("Affichage de l'accueil");
    gtk_window_set_default_size(GTK_WINDOW(acceuil), 1024, 768);
    gtk_widget_show(acceuil);

    DEBUG_PRINT("Enregistrement terminé avec succès");
  } else {
    g_warning("Erreur lors de l'ajout (ID existe déjà ou erreur fichier)!");
    afficher_message(window_ajouter,
                     "Erreur lors de l'ajout! L'ID existe peut-être déjà.",
                     GTK_MESSAGE_ERROR);
  }

  DEBUG_PRINT("FIN on_button55_mohamed_clicked");
}

void on_button56_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("Annuler Ajout");

  GtkWidget *window_ajouter = gtk_widget_get_toplevel(button);
  GtkWidget *acceuil = create_acceuil_mohamed();
  GtkWidget *treeview = lookup_widget(acceuil, "treeview5_mohamed");

  if (treeview) {
    afficher_entraineurs(treeview);
    g_signal_connect(G_OBJECT(treeview), "row-activated",
                     G_CALLBACK(on_treeview_row_activated), NULL);
    g_signal_connect(G_OBJECT(treeview), "cursor-changed",
                     G_CALLBACK(on_treeview_cursor_changed), NULL);
  }

  gtk_widget_destroy(window_ajouter);
  gtk_window_set_default_size(GTK_WINDOW(acceuil), 1024, 768);
  gtk_widget_show(acceuil);
}

// ==================== CALLBACKS RECHERCHE ====================

void on_button67_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button67_mohamed_clicked (Rechercher)");

  GtkWidget *acceuil = gtk_widget_get_toplevel(button);
  GtkWidget *entry_recherche = lookup_widget(acceuil, "entry71_mohamed");
  GtkWidget *treeview = lookup_widget(acceuil, "treeview5_mohamed");

  if (!entry_recherche || !treeview) {
    g_warning("Widgets not found!");
    return;
  }

  char *id_recherche = get_entry_text(entry_recherche);

  if (strlen(id_recherche) == 0) {
    afficher_message(acceuil, "Veuillez saisir un ID à rechercher!",
                     GTK_MESSAGE_WARNING);
    return;
  }

  Entraineur E = chercher_entraineur(FILE_ENTRAINEURS, id_recherche);

  if (strcmp(E.id, "-1") == 0) {
    afficher_message(acceuil, "Aucun entraineur trouvé avec cet ID!",
                     GTK_MESSAGE_INFO);
    afficher_entraineurs(treeview);
  } else {
    GtkListStore *store =
        gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeIter iter;
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, E.id, 1, E.nom, 2, E.prenom, 3,
                       E.telephone, 4, E.sexe, 5, E.specialite, -1);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);

    afficher_message(acceuil, "Entraineur trouvé!", GTK_MESSAGE_INFO);
  }

  DEBUG_PRINT("FIN on_button67_mohamed_clicked");
}

// ==================== CALLBACKS MODIFICATION ====================

void on_button69_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button69_mohamed_clicked (Ouvrir Modifier)");

  if (strlen(id_selected) == 0) {
    GtkWidget *acceuil = gtk_widget_get_toplevel(button);
    afficher_message(acceuil,
                     "Veuillez sélectionner un entraineur dans le tableau!",
                     GTK_MESSAGE_WARNING);
    return;
  }

  GtkWidget *acceuil = gtk_widget_get_toplevel(button);
  GtkWidget *modifier_win = create_modifer_un_entraineur_mohamed();

  GtkWidget *entry_id = lookup_widget(modifier_win, "entry65_mohamed");
  if (entry_id) {
    set_entry_text(entry_id, id_selected);
  }

  gtk_widget_destroy(acceuil);
  gtk_window_set_default_size(GTK_WINDOW(modifier_win), 1024, 768);
  gtk_widget_show(modifier_win);

  DEBUG_PRINT("FIN on_button69_mohamed_clicked");
}

void on_button59_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button59_mohamed_clicked (Afficher données)");

  GtkWidget *modifier_win = gtk_widget_get_toplevel(button);
  GtkWidget *entry_id = lookup_widget(modifier_win, "entry65_mohamed");

  if (!entry_id)
    return;

  char *id = get_entry_text(entry_id);

  if (strlen(id) == 0) {
    afficher_message(modifier_win, "Veuillez saisir un ID!",
                     GTK_MESSAGE_WARNING);
    return;
  }

  Entraineur E = chercher_entraineur(FILE_ENTRAINEURS, id);

  if (strcmp(E.id, "-1") == 0) {
    afficher_message(modifier_win, "Entraineur non trouvé!", GTK_MESSAGE_ERROR);
    return;
  }

  set_textview_text(lookup_widget(modifier_win, "textview35_mohamed"), E.nom);
  set_textview_text(lookup_widget(modifier_win, "textview36_mohamed"),
                    E.prenom);
  set_textview_text(lookup_widget(modifier_win, "textview37_mohamed"), E.id);
  set_textview_text(lookup_widget(modifier_win, "textview38_mohamed"),
                    E.mot_de_passe);
  set_textview_text(lookup_widget(modifier_win, "textview39_mohamed"),
                    format_date(E.date_naissance.jour, E.date_naissance.mois,
                                E.date_naissance.annee));
  set_textview_text(lookup_widget(modifier_win, "textview40_mohamed"),
                    E.telephone);
  set_textview_text(lookup_widget(modifier_win, "textview41_mohamed"), E.sexe);

  set_entry_text(lookup_widget(modifier_win, "entry66_mohamed"), E.nom);
  set_entry_text(lookup_widget(modifier_win, "entry67_mohamed"), E.prenom);
  set_entry_text(lookup_widget(modifier_win, "entry68_mohamed"), E.id);
  set_entry_text(lookup_widget(modifier_win, "entry69_mohamed"),
                 E.mot_de_passe);
  set_entry_text(lookup_widget(modifier_win, "entry70_mohamed"), E.telephone);

  GtkWidget *spin_jour = lookup_widget(modifier_win, "spinbutton16_mohamed");
  GtkWidget *spin_mois = lookup_widget(modifier_win, "spinbutton17_mohamed");
  GtkWidget *spin_annee = lookup_widget(modifier_win, "spinbutton18_mohamed");

  if (spin_jour)
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_jour),
                              E.date_naissance.jour);
  if (spin_mois)
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_mois),
                              E.date_naissance.mois);
  if (spin_annee)
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_annee),
                              E.date_naissance.annee);

  GtkWidget *radio_homme = lookup_widget(modifier_win, "radiobutton21_mohamed");
  GtkWidget *radio_femme = lookup_widget(modifier_win, "radiobutton20_mohamed");

  if (strcmp(E.sexe, "Homme") == 0 && radio_homme)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_homme), TRUE);
  else if (radio_femme)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_femme), TRUE);

  afficher_message(modifier_win, "Données affichées avec succès!",
                   GTK_MESSAGE_INFO);

  DEBUG_PRINT("FIN on_button59_mohamed_clicked");
}

void on_button61_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button61_mohamed_clicked (Valider Modification)");

  GtkWidget *modifier_win = gtk_widget_get_toplevel(button);

  GtkWidget *checkbox = lookup_widget(modifier_win, "checkbutton5_mohamed");

  if (!checkbox || !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox))) {
    afficher_message(modifier_win, "Vous devez cocher la case de confirmation!",
                     GTK_MESSAGE_WARNING);
    return;
  }

  GtkWidget *entry_id_old = lookup_widget(modifier_win, "entry65_mohamed");
  char *id_old = get_entry_text(entry_id_old);

  char *id_new = get_entry_text(lookup_widget(modifier_win, "entry68_mohamed"));
  char *nom = get_entry_text(lookup_widget(modifier_win, "entry66_mohamed"));
  char *prenom = get_entry_text(lookup_widget(modifier_win, "entry67_mohamed"));
  char *mdp = get_entry_text(lookup_widget(modifier_win, "entry69_mohamed"));
  char *tel = get_entry_text(lookup_widget(modifier_win, "entry70_mohamed"));

  if (strlen(nom) == 0 || strlen(prenom) == 0 || strlen(id_new) == 0 ||
      strlen(mdp) == 0 || strlen(tel) == 0) {
    afficher_message(modifier_win, "Tous les champs sont obligatoires!",
                     GTK_MESSAGE_WARNING);
    return;
  }

  if (!validerMotDePasse(mdp)) {
    afficher_message(
        modifier_win,
        "Le mot de passe doit contenir au moins 8 caractères, incluant:\n"
        "- Au moins une majuscule\n"
        "- Au moins une minuscule\n"
        "- Au moins un chiffre",
        GTK_MESSAGE_WARNING);
    return;
  }

  if (!validerTelephone(tel)) {
    afficher_message(
        modifier_win,
        "Le numéro de téléphone doit contenir exactement 8 chiffres!",
        GTK_MESSAGE_WARNING);
    return;
  }

  if (strcmp(id_old, id_new) != 0) {
    if (idExiste(FILE_ENTRAINEURS, id_new)) {
      afficher_message(modifier_win, "Ce nouvel ID existe déjà!",
                       GTK_MESSAGE_ERROR);
      return;
    }
  }

  Entraineur E;
  strncpy(E.id, id_new, sizeof(E.id) - 1);
  E.id[sizeof(E.id) - 1] = '\0';
  strncpy(E.nom, nom, sizeof(E.nom) - 1);
  E.nom[sizeof(E.nom) - 1] = '\0';
  strncpy(E.prenom, prenom, sizeof(E.prenom) - 1);
  E.prenom[sizeof(E.prenom) - 1] = '\0';
  strncpy(E.mot_de_passe, mdp, sizeof(E.mot_de_passe) - 1);
  E.mot_de_passe[sizeof(E.mot_de_passe) - 1] = '\0';
  strncpy(E.telephone, tel, sizeof(E.telephone) - 1);
  E.telephone[sizeof(E.telephone) - 1] = '\0';

  GtkWidget *spin_jour = lookup_widget(modifier_win, "spinbutton16_mohamed");
  GtkWidget *spin_mois = lookup_widget(modifier_win, "spinbutton17_mohamed");
  GtkWidget *spin_annee = lookup_widget(modifier_win, "spinbutton18_mohamed");

  E.date_naissance.jour =
      spin_jour ? gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_jour))
                : 1;
  E.date_naissance.mois =
      spin_mois ? gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_mois))
                : 1;
  E.date_naissance.annee =
      spin_annee ? gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_annee))
                 : 1990;

  if (!validerDate(E.date_naissance.jour, E.date_naissance.mois,
                   E.date_naissance.annee)) {
    afficher_message(modifier_win, "La date de naissance est invalide!",
                     GTK_MESSAGE_WARNING);
    return;
  }

  GtkWidget *radio_homme = lookup_widget(modifier_win, "radiobutton21_mohamed");
  GtkWidget *radio_femme = lookup_widget(modifier_win, "radiobutton20_mohamed");

  if (radio_homme &&
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_homme)))
    strcpy(E.sexe, "Homme");
  else if (radio_femme &&
           gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_femme)))
    strcpy(E.sexe, "Femme");
  else
    strcpy(E.sexe, "Homme");

  strcpy(E.photo, "");
  strcpy(E.cv, "");
  strcpy(E.specialite, "Musculation");

  if (modifier_entraineur(FILE_ENTRAINEURS, id_old, E)) {
    afficher_message(modifier_win, "Entraineur modifié avec succès!",
                     GTK_MESSAGE_INFO);

    strcpy(id_selected, "");

    GtkWidget *acceuil = create_acceuil_mohamed();
    GtkWidget *treeview = lookup_widget(acceuil, "treeview5_mohamed");
    if (treeview) {
      afficher_entraineurs(treeview);
      g_signal_connect(G_OBJECT(treeview), "row-activated",
                       G_CALLBACK(on_treeview_row_activated), NULL);
      g_signal_connect(G_OBJECT(treeview), "cursor-changed",
                       G_CALLBACK(on_treeview_cursor_changed), NULL);
    }
    gtk_widget_destroy(modifier_win);
    gtk_window_set_default_size(GTK_WINDOW(acceuil), 1024, 768);
    gtk_widget_show(acceuil);
  } else {
    afficher_message(modifier_win, "Erreur lors de la modification!",
                     GTK_MESSAGE_ERROR);
  }

  DEBUG_PRINT("FIN on_button61_mohamed_clicked");
}

void on_button60_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button60_mohamed_clicked (Annuler Modifier)");

  GtkWidget *modifier_win = gtk_widget_get_toplevel(button);
  GtkWidget *acceuil = create_acceuil_mohamed();

  GtkWidget *treeview = lookup_widget(acceuil, "treeview5_mohamed");
  if (treeview) {
    afficher_entraineurs(treeview);
    g_signal_connect(G_OBJECT(treeview), "row-activated",
                     G_CALLBACK(on_treeview_row_activated), NULL);
    g_signal_connect(G_OBJECT(treeview), "cursor-changed",
                     G_CALLBACK(on_treeview_cursor_changed), NULL);
  }

  gtk_widget_destroy(modifier_win);
  gtk_window_set_default_size(GTK_WINDOW(acceuil), 1024, 768);
  gtk_widget_show(acceuil);

  DEBUG_PRINT("FIN on_button60_mohamed_clicked");
}

// ==================== CALLBACKS SUPPRESSION ====================

void on_button68_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button68_mohamed_clicked (Supprimer)");

  if (strlen(id_selected) == 0) {
    GtkWidget *acceuil = gtk_widget_get_toplevel(button);
    afficher_message(acceuil,
                     "Veuillez sélectionner un entraineur à supprimer!",
                     GTK_MESSAGE_WARNING);
    return;
  }

  GtkWidget *acceuil = gtk_widget_get_toplevel(button);
  GtkWidget *supprimer = create_supprimer_un_entraineur_mohamed();

  gtk_widget_destroy(acceuil);
  gtk_window_set_default_size(GTK_WINDOW(supprimer), 1024, 768);
  gtk_widget_show(supprimer);

  DEBUG_PRINT("FIN on_button68_mohamed_clicked");
}

void on_button10_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button10_mohamed_clicked (Confirmer Suppression)");

  GtkWidget *supprimer_win = gtk_widget_get_toplevel(button);

  if (supprimer_entraineur(FILE_ENTRAINEURS, id_selected)) {
    afficher_message(supprimer_win, "Entraineur supprimé avec succès!",
                     GTK_MESSAGE_INFO);

    strcpy(id_selected, "");

    GtkWidget *acceuil = create_acceuil_mohamed();
    GtkWidget *treeview = lookup_widget(acceuil, "treeview5_mohamed");
    if (treeview) {
      afficher_entraineurs(treeview);
      g_signal_connect(G_OBJECT(treeview), "row-activated",
                       G_CALLBACK(on_treeview_row_activated), NULL);
      g_signal_connect(G_OBJECT(treeview), "cursor-changed",
                       G_CALLBACK(on_treeview_cursor_changed), NULL);
    }
    gtk_widget_destroy(supprimer_win);
    gtk_window_set_default_size(GTK_WINDOW(acceuil), 1024, 768);
    gtk_widget_show(acceuil);
  } else {
    afficher_message(supprimer_win, "Erreur lors de la suppression!",
                     GTK_MESSAGE_ERROR);
  }

  DEBUG_PRINT("FIN on_button10_mohamed_clicked");
}

void on_button15_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button15_mohamed_clicked (Annuler Suppression)");

  GtkWidget *supprimer_win = gtk_widget_get_toplevel(button);
  GtkWidget *acceuil = create_acceuil_mohamed();

  GtkWidget *treeview = lookup_widget(acceuil, "treeview5_mohamed");
  if (treeview) {
    afficher_entraineurs(treeview);
    g_signal_connect(G_OBJECT(treeview), "row-activated",
                     G_CALLBACK(on_treeview_row_activated), NULL);
    g_signal_connect(G_OBJECT(treeview), "cursor-changed",
                     G_CALLBACK(on_treeview_cursor_changed), NULL);
  }

  gtk_widget_destroy(supprimer_win);
  gtk_window_set_default_size(GTK_WINDOW(acceuil), 1024, 768);
  gtk_widget_show(acceuil);

  DEBUG_PRINT("FIN on_button15_mohamed_clicked");
}

// ==================== CALLBACKS INSCRIPTION COURS ====================

void afficher_inscriptions(GtkWidget *treeview) {
  DEBUG_PRINT("Début afficher_inscriptions");

  if (!treeview) {
    g_warning("TreeView est NULL!");
    return;
  }

  GtkListStore *store;
  GtkTreeIter iter;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GList *columns;
  int num_columns;

  columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview));
  num_columns = g_list_length(columns);
  g_list_free(columns);

  if (num_columns == 0) {
    DEBUG_PRINT("Création des colonnes inscriptions");

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text",
                                                      0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Prénom", renderer,
                                                      "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Session", renderer,
                                                      "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Type de cours", renderer,
                                                      "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
  }

  store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                             G_TYPE_STRING);

  FILE *f = fopen(FILE_INSCRIPTIONS, "r");
  if (f != NULL) {
    char ligne[512];
    int count = 0;

    while (fgets(ligne, sizeof(ligne), f) != NULL) {
      Inscription I;
      char *token, *saveptr;
      int field = 0;

      ligne[strcspn(ligne, "\n")] = '\0';
      if (strlen(ligne) == 0)
        continue;

      token = strtok_r(ligne, "|", &saveptr);
      while (token != NULL && field < 4) {
        switch (field) {
        case 0:
          strncpy(I.nom, token, sizeof(I.nom) - 1);
          I.nom[sizeof(I.nom) - 1] = '\0';
          break;
        case 1:
          strncpy(I.prenom, token, sizeof(I.prenom) - 1);
          I.prenom[sizeof(I.prenom) - 1] = '\0';
          break;
        case 2:
          strncpy(I.session, token, sizeof(I.session) - 1);
          I.session[sizeof(I.session) - 1] = '\0';
          break;
        case 3:
          strncpy(I.type_cours, token, sizeof(I.type_cours) - 1);
          I.type_cours[sizeof(I.type_cours) - 1] = '\0';
          break;
        }
        field++;
        token = strtok_r(NULL, "|", &saveptr);
      }

      if (field >= 4) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, I.nom, 1, I.prenom, 2, I.session, 3,
                           I.type_cours, -1);
        count++;
      }
    }
    fclose(f);
    g_print("[DEBUG] %d inscriptions chargées\n", count);
  }

  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
  g_object_unref(store);

  DEBUG_PRINT("Fin afficher_inscriptions");
}

void on_button77_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button77_mohamed_clicked (Valider Inscription)");

  GtkWidget *inscrire = gtk_widget_get_toplevel(button);

  GtkWidget *checkbox = lookup_widget(inscrire, "checkbutton7_mohamed");

  if (!checkbox || !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox))) {
    afficher_message(inscrire, "Vous devez cocher la case de confirmation!",
                     GTK_MESSAGE_WARNING);
    return;
  }

  GtkWidget *entry_nom = lookup_widget(inscrire, "entry74_mohamed");
  GtkWidget *entry_prenom = lookup_widget(inscrire, "entry75_mohamed");

  char *nom = get_entry_text(entry_nom);
  char *prenom = get_entry_text(entry_prenom);

  if (strlen(nom) == 0 || strlen(prenom) == 0) {
    afficher_message(inscrire, "Le nom et le prénom sont obligatoires!",
                     GTK_MESSAGE_WARNING);
    return;
  }

  GtkWidget *radio_jour = lookup_widget(inscrire, "radiobutton22_mohamed");
  GtkWidget *radio_soir = lookup_widget(inscrire, "radiobutton23_mohamed");

  char session[20];
  if (radio_jour && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_jour)))
    strcpy(session, "jour");
  else if (radio_soir &&
           gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_soir)))
    strcpy(session, "soir");
  else
    strcpy(session, "jour");

  GtkWidget *combobox = lookup_widget(inscrire, "comboboxentry7_mohamed");
  gchar *type_cours = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combobox));

  if (!type_cours || strlen(type_cours) == 0) {
    afficher_message(inscrire, "Veuillez sélectionner un type de cours!",
                     GTK_MESSAGE_WARNING);
    if (type_cours)
      g_free(type_cours);
    return;
  }

  Inscription I;
  strncpy(I.nom, nom, sizeof(I.nom) - 1);
  I.nom[sizeof(I.nom) - 1] = '\0';
  strncpy(I.prenom, prenom, sizeof(I.prenom) - 1);
  I.prenom[sizeof(I.prenom) - 1] = '\0';
  strncpy(I.session, session, sizeof(I.session) - 1);
  I.session[sizeof(I.session) - 1] = '\0';
  strncpy(I.type_cours, type_cours, sizeof(I.type_cours) - 1);
  I.type_cours[sizeof(I.type_cours) - 1] = '\0';
  g_free(type_cours);

  if (inscrireCours(FILE_INSCRIPTIONS, I)) {
    afficher_message(inscrire, "Inscription réussie!", GTK_MESSAGE_INFO);

    GtkWidget *treeview = lookup_widget(inscrire, "treeview6_mohamed");
    if (treeview) {
      afficher_inscriptions(treeview);
    }

    set_entry_text(entry_nom, "");
    set_entry_text(entry_prenom, "");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox), FALSE);
  } else {
    afficher_message(inscrire, "Erreur lors de l'inscription!",
                     GTK_MESSAGE_ERROR);
  }

  DEBUG_PRINT("FIN on_button77_mohamed_clicked");
}

void on_button76_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button76_mohamed_clicked (Annuler Inscription)");

  GtkWidget *inscrire = gtk_widget_get_toplevel(button);
  GtkWidget *acceuil = create_acceuil_mohamed();

  GtkWidget *treeview = lookup_widget(acceuil, "treeview5_mohamed");
  if (treeview) {
    afficher_entraineurs(treeview);
    g_signal_connect(G_OBJECT(treeview), "row-activated",
                     G_CALLBACK(on_treeview_row_activated), NULL);
    g_signal_connect(G_OBJECT(treeview), "cursor-changed",
                     G_CALLBACK(on_treeview_cursor_changed), NULL);
  }

  gtk_widget_destroy(inscrire);
  gtk_window_set_default_size(GTK_WINDOW(acceuil), 1024, 768);
  gtk_widget_show(acceuil);

  DEBUG_PRINT("FIN on_button76_mohamed_clicked");
}

void on_button78_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button78_mohamed_clicked (Déconnexion Inscription)");

  GtkWidget *inscrire = gtk_widget_get_toplevel(button);
  GtkWidget *login_admin = create_login_admin_mohamed();

  gtk_widget_destroy(inscrire);
  gtk_window_set_default_size(GTK_WINDOW(login_admin), 1024, 768);
  gtk_widget_show(login_admin);

  DEBUG_PRINT("FIN on_button78_mohamed_clicked");
}

// ==================== CALLBACKS NAVIGATION ====================

void on_button71_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button71_mohamed_clicked (Précédent)");

  GtkWidget *acceuil = gtk_widget_get_toplevel(button);
  GtkWidget *login = create_login_mohamed();

  strcpy(id_selected, "");

  gtk_widget_destroy(acceuil);
  gtk_window_set_default_size(GTK_WINDOW(login), 1024, 768);
  gtk_widget_show(login);

  DEBUG_PRINT("FIN on_button71_mohamed_clicked");
}

void on_button72_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button72_mohamed_clicked (Déconnexion Accueil)");

  GtkWidget *acceuil = gtk_widget_get_toplevel(button);
  GtkWidget *login_admin = create_login_admin_mohamed();

  strcpy(id_selected, "");

  gtk_widget_destroy(acceuil);
  gtk_window_set_default_size(GTK_WINDOW(login_admin), 1024, 768);
  gtk_widget_show(login_admin);

  DEBUG_PRINT("FIN on_button72_mohamed_clicked");
}

void on_button62_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button62_mohamed_clicked (Déconnexion Modifier)");

  GtkWidget *modifier_win = gtk_widget_get_toplevel(button);
  GtkWidget *login_admin = create_login_admin_mohamed();

  strcpy(id_selected, "");

  gtk_widget_destroy(modifier_win);
  gtk_window_set_default_size(GTK_WINDOW(login_admin), 1024, 768);
  gtk_widget_show(login_admin);

  DEBUG_PRINT("FIN on_button62_mohamed_clicked");
}

void on_button63_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button63_mohamed_clicked (Déconnexion Ajouter)");

  GtkWidget *ajouter = gtk_widget_get_toplevel(button);
  GtkWidget *login_admin = create_login_admin_mohamed();

  gtk_widget_destroy(ajouter);
  gtk_window_set_default_size(GTK_WINDOW(login_admin), 1024, 768);
  gtk_widget_show(login_admin);

  DEBUG_PRINT("FIN on_button63_mohamed_clicked");
}

// ==================== CALLBACKS LOGIN ENTRAINEUR ====================

void on_button74_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button74_mohamed_clicked (Login Entraineur)");

  GtkWidget *login_entraineur = gtk_widget_get_toplevel(button);

  // Récupérer les entries
  GtkWidget *entry_id = lookup_widget(login_entraineur, "entry72_mohamed");
  GtkWidget *entry_mdp = lookup_widget(login_entraineur, "entry73_mohamed");

  if (!entry_id || !entry_mdp) {
    g_warning("Widgets entry non trouvés!");
    return;
  }

  char *id = get_entry_text(entry_id);
  char *mdp = get_entry_text(entry_mdp);

  g_print("[DEBUG LOGIN ENTRAINEUR] ID: '%s', MDP: '%s'\n", id, mdp);

  // Validation des champs vides
  if (strlen(id) == 0 || strlen(mdp) == 0) {
    afficher_message(login_entraineur,
                     "Veuillez saisir l'ID et le mot de passe!",
                     GTK_MESSAGE_WARNING);
    return;
  }

  // Chercher l'entraineur dans le fichier
  Entraineur E = chercher_entraineur(FILE_ENTRAINEURS, id);

  if (strcmp(E.id, "-1") == 0) {
    afficher_message(login_entraineur, "ID inexistant!", GTK_MESSAGE_ERROR);
    return;
  }

  // Vérifier le mot de passe
  if (strcmp(E.mot_de_passe, mdp) != 0) {
    afficher_message(login_entraineur, "Mot de passe incorrect!",
                     GTK_MESSAGE_ERROR);
    return;
  }

  // Login réussi
  g_print("[DEBUG LOGIN ENTRAINEUR] Login réussi pour: %s %s\n", E.nom,
          E.prenom);
  afficher_message(login_entraineur, "Bienvenue!", GTK_MESSAGE_INFO);

  // Ouvrir l'interface d'inscription à un cours
  GtkWidget *inscrire = create_inscrire____un_cours_sportif_mohamed();

  // Connecter le TreeView des inscriptions
  GtkWidget *treeview_inscriptions =
      lookup_widget(inscrire, "treeview6_mohamed");
  if (treeview_inscriptions) {
    afficher_inscriptions(treeview_inscriptions);
  }

  gtk_widget_destroy(login_entraineur);
  gtk_window_set_default_size(GTK_WINDOW(inscrire), 1024, 768);
  gtk_widget_show(inscrire);

  DEBUG_PRINT("FIN on_button74_mohamed_clicked");
}

void on_button73_mohamed_clicked(GtkWidget *button, gpointer user_data) {
  DEBUG_PRINT("DÉBUT on_button73_mohamed_clicked (Annuler Login Entraineur)");

  GtkWidget *login_entraineur = gtk_widget_get_toplevel(button);
  GtkWidget *login = create_login_mohamed();

  gtk_widget_destroy(login_entraineur);
  gtk_window_set_default_size(GTK_WINDOW(login), 1024, 768);
  gtk_widget_show(login);

  DEBUG_PRINT("FIN on_button73_mohamed_clicked");
}
