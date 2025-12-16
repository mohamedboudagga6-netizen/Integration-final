#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "member.h"

int sexe_choice = 0; // 1:Homme, 2:Feme
int paiement_choice = 0; // 1:Espece, 2:Carte, 3:Virement
int objectif_choice = 0; // 1:Perte, 2:Gain, 3:Renforcement, 4:Remise, 5:Autre

// Helper for Yes/No Dialog
int show_confirmation(GtkWidget *window, const char *msg) {
    GtkWidget *dialog;
    gint response;

    dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_YES_NO,
                                    "%s", msg);
    
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return (response == GTK_RESPONSE_YES);
}

void show_error(GtkWidget *window, const char *msg) {
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_OK,
                                    "%s", msg);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_radiobuttonFemme_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if(gtk_toggle_button_get_active(togglebutton)) sexe_choice = 2;
}

void on_radiobuttonHomme_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if(gtk_toggle_button_get_active(togglebutton)) sexe_choice = 1;
}

void on_radiobuttonPerteDePoid_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if(gtk_toggle_button_get_active(togglebutton)) objectif_choice = 1;
}

void on_radiobuttonEspece_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if(gtk_toggle_button_get_active(togglebutton)) paiement_choice = 1;
}

void on_radiobuttonCarte_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if(gtk_toggle_button_get_active(togglebutton)) paiement_choice = 2;
}

void on_radiobutton5Virement_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if(gtk_toggle_button_get_active(togglebutton)) paiement_choice = 3;
}

void on_radiobuttonGainDuPoid_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if(gtk_toggle_button_get_active(togglebutton)) objectif_choice = 2;
}

void on_radiobuttonRenforcement_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if(gtk_toggle_button_get_active(togglebutton)) objectif_choice = 3;
}

void on_radiobuttonRemiseEnForme_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if(gtk_toggle_button_get_active(togglebutton)) objectif_choice = 4;
}

void on_radiobuttonAutre_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    if(gtk_toggle_button_get_active(togglebutton)) objectif_choice = 5;
}

void on_buttonEnregistrer_clicked(GtkButton *button, gpointer user_data) {
    Member m;
    GtkWidget *entryIdentifiant = lookup_widget(GTK_WIDGET(button), "entryIdentifiant");
    
    strcpy(m.id, gtk_entry_get_text(GTK_ENTRY(entryIdentifiant)));

    if (verifier_id(m.id) == 1) {
        show_error(gtk_widget_get_toplevel(GTK_WIDGET(button)), "Erreur: ID existe deja!");
        return;
    }

    if (!show_confirmation(gtk_widget_get_toplevel(GTK_WIDGET(button)), "Voulez-vous vraiment ajouter ce membre?")) {
        return;
    }

    GtkWidget *entryNom = lookup_widget(GTK_WIDGET(button), "entryNom");
    GtkWidget *entryPrenom = lookup_widget(GTK_WIDGET(button), "entryPrenom");
    GtkWidget *entryMotDePasse = lookup_widget(GTK_WIDGET(button), "entryMotDePasse");
    GtkWidget *entryEmail = lookup_widget(GTK_WIDGET(button), "entryEmail");
    GtkWidget *entryNumeroDeTelephone = lookup_widget(GTK_WIDGET(button), "entryNumeroDeTelephone");
    GtkWidget *entryCodePostal = lookup_widget(GTK_WIDGET(button), "entryCodePostal");
    GtkWidget *entryVille = lookup_widget(GTK_WIDGET(button), "entryVille");
    
    GtkWidget *comboboxentryTypeDabonnement = lookup_widget(GTK_WIDGET(button), "comboboxentryTypeDabonnement");
    GtkWidget *comboboxentryCours = lookup_widget(GTK_WIDGET(button), "comboboxentryCours");
    GtkWidget *comboboxentryCentre = lookup_widget(GTK_WIDGET(button), "comboboxentryCentre");
    GtkWidget *calendar1 = lookup_widget(GTK_WIDGET(button), "calendar1");
    GtkWidget *checkbuttonAuthorisation = lookup_widget(GTK_WIDGET(button), "checkbuttonAuthorisation");

    strcpy(m.nom, gtk_entry_get_text(GTK_ENTRY(entryNom)));
    strcpy(m.prenom, gtk_entry_get_text(GTK_ENTRY(entryPrenom)));
    strcpy(m.password, gtk_entry_get_text(GTK_ENTRY(entryMotDePasse)));
    strcpy(m.email, gtk_entry_get_text(GTK_ENTRY(entryEmail)));
    strcpy(m.phone, gtk_entry_get_text(GTK_ENTRY(entryNumeroDeTelephone)));
    strcpy(m.cp, gtk_entry_get_text(GTK_ENTRY(entryCodePostal)));
    strcpy(m.ville, gtk_entry_get_text(GTK_ENTRY(entryVille)));

    gchar *text_abonn = gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboboxentryTypeDabonnement));
    if (text_abonn) {
        strcpy(m.type_abonn, text_abonn);
        g_free(text_abonn);
    } else {
        strcpy(m.type_abonn, "");
    }

    gchar *text_cours = gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboboxentryCours));
    if (text_cours) {
        strcpy(m.cours, text_cours);
        g_free(text_cours);
    } else {
        strcpy(m.cours, "");
    }

    gchar *text_centre = gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboboxentryCentre));
    if (text_centre) {
        strcpy(m.centre, text_centre);
        g_free(text_centre);
    } else {
        strcpy(m.centre, "");
    }

    if(sexe_choice == 1) strcpy(m.sexe, "Homme");
    else if(sexe_choice == 2) strcpy(m.sexe, "Femme");
    else strcpy(m.sexe, "Inconnu");

    if(paiement_choice == 1) strcpy(m.mode_paiement, "Espece");
    else if(paiement_choice == 2) strcpy(m.mode_paiement, "Carte");
    else if(paiement_choice == 3) strcpy(m.mode_paiement, "Virement");
    else strcpy(m.mode_paiement, "Inconnu");

    if(objectif_choice == 1) strcpy(m.objectif, "PerteDePoids");
    else if(objectif_choice == 2) strcpy(m.objectif, "GainDePoids");
    else if(objectif_choice == 3) strcpy(m.objectif, "Renforcement");
    else if(objectif_choice == 4) strcpy(m.objectif, "RemiseEnForme");
    else if(objectif_choice == 5) strcpy(m.objectif, "Autre");
    else strcpy(m.objectif, "Inconnu");

    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(calendar1), &year, &month, &day);
    m.jour = day;
    m.mois = month + 1; 
    m.annee = year;

    m.auth_medicale = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonAuthorisation)) ? 1 : 0;

    ajouter_membre(m);
}

void on_buttonChercher_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *entryIdentifiant1 = lookup_widget(GTK_WIDGET(button), "entryIdentifiant1");
    GtkWidget *treeview1 = lookup_widget(GTK_WIDGET(button), "treeview1");
    char id[50];
    strcpy(id, gtk_entry_get_text(GTK_ENTRY(entryIdentifiant1)));
    chercher_membre(treeview1, id);

    // Pre-fill the Modify form (Using widgets ending in '1')
    Member m = get_member(id);
    if (strcmp(m.id, "") != 0) {
        GtkWidget *entryNom1 = lookup_widget(GTK_WIDGET(button), "entryNom1");
        GtkWidget *entryPrenom1 = lookup_widget(GTK_WIDGET(button), "entryPrenom1");
        GtkWidget *entryMotDePasse1 = lookup_widget(GTK_WIDGET(button), "entryMotDePasse1");
        GtkWidget *entryEmail1 = lookup_widget(GTK_WIDGET(button), "entryEmail1");
        GtkWidget *entryNumeroDeTelephone1 = lookup_widget(GTK_WIDGET(button), "entryNumeroDeTelephone1");
        GtkWidget *entryCodePostal1 = lookup_widget(GTK_WIDGET(button), "entryCodePostal1");
        GtkWidget *entryVille1 = lookup_widget(GTK_WIDGET(button), "entryVille1");
        GtkWidget *calendar1 = lookup_widget(GTK_WIDGET(button), "calendar1");
        GtkWidget *checkbuttonAuthorisation1 = lookup_widget(GTK_WIDGET(button), "checkbuttonAuthorisation1");

        // Text Fields
        gtk_entry_set_text(GTK_ENTRY(entryNom1), m.nom);
        gtk_entry_set_text(GTK_ENTRY(entryPrenom1), m.prenom);
        gtk_entry_set_text(GTK_ENTRY(entryMotDePasse1), m.password);
        gtk_entry_set_text(GTK_ENTRY(entryEmail1), m.email);
        gtk_entry_set_text(GTK_ENTRY(entryNumeroDeTelephone1), m.phone);
        gtk_entry_set_text(GTK_ENTRY(entryCodePostal1), m.cp);
        gtk_entry_set_text(GTK_ENTRY(entryVille1), m.ville);

        // Combos
        GtkWidget *child;
        GtkWidget *comboboxentryCentre1 = lookup_widget(GTK_WIDGET(button), "comboboxentryCentre1");
        child = gtk_bin_get_child(GTK_BIN(comboboxentryCentre1));
        if (child && GTK_IS_ENTRY(child)) gtk_entry_set_text(GTK_ENTRY(child), m.centre);

        GtkWidget *comboboxentryCours1 = lookup_widget(GTK_WIDGET(button), "comboboxentryCours1");
        child = gtk_bin_get_child(GTK_BIN(comboboxentryCours1));
        if (child && GTK_IS_ENTRY(child)) gtk_entry_set_text(GTK_ENTRY(child), m.cours);

        GtkWidget *comboboxentryTypeDabonnement1 = lookup_widget(GTK_WIDGET(button), "comboboxentryTypeDabonnement1");
        child = gtk_bin_get_child(GTK_BIN(comboboxentryTypeDabonnement1));
        if (child && GTK_IS_ENTRY(child)) gtk_entry_set_text(GTK_ENTRY(child), m.type_abonn);

        // Radios
        if(strcmp(m.sexe, "Femme") == 0) {
            GtkWidget *rb = lookup_widget(GTK_WIDGET(button), "radiobuttonFemme1");
            if(rb) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb), TRUE);
        } else if(strcmp(m.sexe, "Homme") == 0) {
            GtkWidget *rb = lookup_widget(GTK_WIDGET(button), "radiobuttonHomme1");
            if(rb) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb), TRUE);
        }

        if(strcmp(m.mode_paiement, "Espece") == 0) {
             gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonEspece1")), TRUE);
        } else if(strcmp(m.mode_paiement, "Carte") == 0) {
             gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonCarte1")), TRUE);
        } else if(strcmp(m.mode_paiement, "Virement") == 0) {
             gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonVirement1")), TRUE);
        }

        if(strcmp(m.objectif, "PerteDePoids") == 0) {
             gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonPerteDePoid1")), TRUE);
        } else if(strcmp(m.objectif, "GainDePoids") == 0) {
             gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonGainDePoid1")), TRUE);
        } else if(strcmp(m.objectif, "Renforcement") == 0) {
             gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonRenforcement1")), TRUE);
        } else if(strcmp(m.objectif, "RemiseEnForme") == 0) {
             gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonRemiseEnForme1")), TRUE);
        } else if(strcmp(m.objectif, "Autre") == 0) {
             gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonAutre1")), TRUE);
        }

        // Calendar
        gtk_calendar_select_month(GTK_CALENDAR(calendar1), m.mois - 1, m.annee);
        gtk_calendar_select_day(GTK_CALENDAR(calendar1), m.jour);

        // Checkbox
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbuttonAuthorisation1), m.auth_medicale);
    }
}

void on_buttonModifier_clicked(GtkButton *button, gpointer user_data) {
    if (!show_confirmation(gtk_widget_get_toplevel(GTK_WIDGET(button)), "Voulez-vous vraiment modifier ce membre?")) {
        return;
    }

    Member m;
    GtkWidget *entryIdentifiant1 = lookup_widget(GTK_WIDGET(button), "entryIdentifiant1");
    GtkWidget *entryNom1 = lookup_widget(GTK_WIDGET(button), "entryNom1");
    GtkWidget *entryPrenom1 = lookup_widget(GTK_WIDGET(button), "entryPrenom1");
    GtkWidget *entryMotDePasse1 = lookup_widget(GTK_WIDGET(button), "entryMotDePasse1");
    GtkWidget *entryEmail1 = lookup_widget(GTK_WIDGET(button), "entryEmail1");
    GtkWidget *entryNumeroDeTelephone1 = lookup_widget(GTK_WIDGET(button), "entryNumeroDeTelephone1");
    GtkWidget *entryCodePostal1 = lookup_widget(GTK_WIDGET(button), "entryCodePostal1");
    GtkWidget *entryVille1 = lookup_widget(GTK_WIDGET(button), "entryVille1");
    GtkWidget *calendar1 = lookup_widget(GTK_WIDGET(button), "calendar1");
    GtkWidget *checkbuttonAuthorisation1 = lookup_widget(GTK_WIDGET(button), "checkbuttonAuthorisation1");
    GtkWidget *comboboxentryTypeDabonnement1 = lookup_widget(GTK_WIDGET(button), "comboboxentryTypeDabonnement1");
    GtkWidget *comboboxentryCours1 = lookup_widget(GTK_WIDGET(button), "comboboxentryCours1");
    GtkWidget *comboboxentryCentre1 = lookup_widget(GTK_WIDGET(button), "comboboxentryCentre1");

    
    strcpy(m.id, gtk_entry_get_text(GTK_ENTRY(entryIdentifiant1)));
    strcpy(m.nom, gtk_entry_get_text(GTK_ENTRY(entryNom1)));
    strcpy(m.prenom, gtk_entry_get_text(GTK_ENTRY(entryPrenom1)));
    strcpy(m.password, gtk_entry_get_text(GTK_ENTRY(entryMotDePasse1)));
    strcpy(m.email, gtk_entry_get_text(GTK_ENTRY(entryEmail1)));
    strcpy(m.phone, gtk_entry_get_text(GTK_ENTRY(entryNumeroDeTelephone1)));
    strcpy(m.cp, gtk_entry_get_text(GTK_ENTRY(entryCodePostal1)));
    strcpy(m.ville, gtk_entry_get_text(GTK_ENTRY(entryVille1)));

    gchar *text_abonn = gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboboxentryTypeDabonnement1));
    if(text_abonn) { strcpy(m.type_abonn, text_abonn); g_free(text_abonn); } else strcpy(m.type_abonn, "");

    gchar *text_cours = gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboboxentryCours1));
    if(text_cours) { strcpy(m.cours, text_cours); g_free(text_cours); } else strcpy(m.cours, "");

    gchar *text_centre = gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboboxentryCentre1));
    if(text_centre) { strcpy(m.centre, text_centre); g_free(text_centre); } else strcpy(m.centre, "");


    // Check Sex directly (Using correct radio handles if available or groups)
    // Assuming widgets are present in the same window/fixed
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonHomme1"))))
        strcpy(m.sexe, "Homme");
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonFemme1"))))
        strcpy(m.sexe, "Femme");
    else
        strcpy(m.sexe, "Inconnu");

    // Check Payment directly
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonEspece1"))))
        strcpy(m.mode_paiement, "Espece");
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonCarte1"))))
        strcpy(m.mode_paiement, "Carte");
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonVirement1"))))
        strcpy(m.mode_paiement, "Virement");
    else
        strcpy(m.mode_paiement, "Inconnu");

    // Check Objective directly
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonPerteDePoid1"))))
        strcpy(m.objectif, "PerteDePoids");
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonGainDePoid1")))) // Note: GainDePoid1 (fixed typo form GainDuPoidMod usually)
        strcpy(m.objectif, "GainDePoids");
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonRenforcement1"))))
        strcpy(m.objectif, "Renforcement");
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonRemiseEnForme1"))))
        strcpy(m.objectif, "RemiseEnForme");
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(button), "radiobuttonAutre1"))))
        strcpy(m.objectif, "Autre");
    else
        strcpy(m.objectif, "Inconnu");

    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(calendar1), &year, &month, &day);
    m.jour = day;
    m.mois = month + 1; 
    m.annee = year;

    m.auth_medicale = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttonAuthorisation1)) ? 1 : 0;
    
    // Call modifier
    modifier_membre(m, m.id);

    // Refresh Treeview immediately
    GtkWidget *treeview1 = lookup_widget(GTK_WIDGET(button), "treeview1");
    chercher_membre(treeview1, ""); 
}

void on_buttonRechercher_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *entryIdentifiant2 = lookup_widget(GTK_WIDGET(button), "entryIdentifiant2");
    GtkWidget *treeview2 = lookup_widget(GTK_WIDGET(button), "treeview2");
    char id[50];
    strcpy(id, gtk_entry_get_text(GTK_ENTRY(entryIdentifiant2)));
    chercher_membre(treeview2, id);
}

void on_buttonActualiser_clicked(GtkButton *button, gpointer user_data) {
     // This callback is shared by buttonActualiser1 (Modify) and buttonActualiser2 (Delete)
     const gchar *button_name = gtk_widget_get_name(GTK_WIDGET(button));
     
     if (strcmp(button_name, "buttonActualiser1") == 0) {
        GtkWidget *treeview1 = lookup_widget(GTK_WIDGET(button), "treeview1");
        chercher_membre(treeview1, ""); 
     } else if (strcmp(button_name, "buttonActualiser2") == 0) {
        GtkWidget *treeview2 = lookup_widget(GTK_WIDGET(button), "treeview2");
        chercher_membre(treeview2, ""); 
     } else {
         // Fallback or find both? Safe to default to searching based on context if lookup works
         // But lookup might fail if we look for the wrong one.
         // Let's rely on name. If name is missing (e.g. manual creation without name), this might fail.
         // But interface.c sets names via GLADE_HOOKUP_OBJECT.
     }
}

void on_buttonSupprimer_clicked(GtkButton *button, gpointer user_data) {
    if (!show_confirmation(gtk_widget_get_toplevel(GTK_WIDGET(button)), "Voulez-vous vraiment supprimer ce membre?")) {
        return;
    }

    GtkWidget *entryIdentifiant2 = lookup_widget(GTK_WIDGET(button), "entryIdentifiant2");
    char id[50];
    strcpy(id, gtk_entry_get_text(GTK_ENTRY(entryIdentifiant2)));
    supprimer_membre(id);
    
    GtkWidget *treeview2 = lookup_widget(GTK_WIDGET(button), "treeview2");
    chercher_membre(treeview2, ""); 
}



void
on_button2_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *current_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *login_window = create_login();
    gtk_widget_show(login_window);
    gtk_widget_destroy(current_window);
}


void
on_buttonLogin_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *current_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    const gchar *title = gtk_window_get_title(GTK_WINDOW(current_window));

    // Admin Window Check
    if (strstr(title, "ADMIN") != NULL) { 
        GtkWidget *entryIdentifiant3 = lookup_widget(GTK_WIDGET(button), "entryIdentifiant3");
        GtkWidget *entryMotDePasse3 = lookup_widget(GTK_WIDGET(button), "entryMotDePasse3");
        
        const gchar *id = gtk_entry_get_text(GTK_ENTRY(entryIdentifiant3));
        const gchar *pw = gtk_entry_get_text(GTK_ENTRY(entryMotDePasse3));

        if (strcmp(id, "0000") == 0 && strcmp(pw, "0000") == 0) {
            GtkWidget *admin_window = create_interface_admin();
            gtk_widget_show(admin_window);
            gtk_widget_destroy(current_window);
        } else {
             show_error(current_window, "Admin ID or Password Incorrect!");
        }
    } 
    // Member Window Check
    else { 
         // Assuming if not Admin, it's Member (or we can check for "MEMBRE")
         GtkWidget *entryIdentifiant4 = lookup_widget(GTK_WIDGET(button), "entryIdentifiant4");
         GtkWidget *entryMotDePasse4 = lookup_widget(GTK_WIDGET(button), "entryMotDePasse4");

         char id[50];
         char pw[50];
         strcpy(id, gtk_entry_get_text(GTK_ENTRY(entryIdentifiant4)));
         strcpy(pw, gtk_entry_get_text(GTK_ENTRY(entryMotDePasse4)));

         if (check_login(id, pw) == 1) {
            GtkWidget *demande_window = create_Demande_d_un_entraineur_priv__();
            gtk_widget_show(demande_window);
            gtk_widget_destroy(current_window);
            
            // Auto-fill ID and Name in Demande window if possible
            GtkWidget *entryNom5 = lookup_widget(demande_window, "entryNom5");
            GtkWidget *entryIdentifiant5 = lookup_widget(demande_window, "entryIdentifiant5");
            Member m = get_member(id);
            if(entryNom5) gtk_entry_set_text(GTK_ENTRY(entryNom5), m.nom);
            if(entryIdentifiant5) gtk_entry_set_text(GTK_ENTRY(entryIdentifiant5), m.id);

         } else {
             show_error(current_window, "ID ou Mot de passe incorrect! Ou vous n'etes pas membre.");
         }
    }
}


void
on_buttonAnnuler_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *current_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *login_window = create_login(); // Return to main selection
    gtk_widget_show(login_window);
    gtk_widget_destroy(current_window);
}


void
on_radiobuttonHomme1_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_button3_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *current_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *admin_login = create_log_in_ADMIN();
    gtk_widget_show(admin_login);
    gtk_widget_destroy(current_window);
}


void
on_radiobuttonFemme1_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_button4_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
     GtkWidget *current_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
     GtkWidget *member_login = create_LOGIN_MEMBRE();
     gtk_widget_show(member_login);
     gtk_widget_destroy(current_window);
}




// Member sends request
void on_buttonEnvoyer_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *entryNom5 = lookup_widget(GTK_WIDGET(button), "entryNom5");
    GtkWidget *entryIdentifiant5 = lookup_widget(GTK_WIDGET(button), "entryIdentifiant5");
    GtkWidget *calendar5 = lookup_widget(GTK_WIDGET(button), "calendar5");
    GtkWidget *spinbuttonHeure = lookup_widget(GTK_WIDGET(button), "spinbuttonHeure");
    GtkWidget *treeviewCoach = lookup_widget(GTK_WIDGET(button), "treeviewCoach");
    GtkWidget *comboboxentrycours5 = lookup_widget(GTK_WIDGET(button), "comboboxentrycours5");

    char id[50], nom[50], cours[50];
    strcpy(id, gtk_entry_get_text(GTK_ENTRY(entryIdentifiant5)));
    strcpy(nom, gtk_entry_get_text(GTK_ENTRY(entryNom5)));
    
    // Get Cours
    gchar *c_text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboboxentrycours5));
    if(c_text) strcpy(cours, c_text); else strcpy(cours, "");

    // Get Selected Coach from TreeView
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeviewCoach));
    GtkTreeModel *model;
    GtkTreeIter iter;
    char *coach_id;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter, 0, &coach_id, -1); // Assuming ID is col 0
    } else {
        show_error(gtk_widget_get_toplevel(GTK_WIDGET(button)), "Veuillez selectionner un entraineur.");
        return;
    }

    // Date
    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(calendar5), &year, &month, &day);
    int heure = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbuttonHeure));

    CoachRequest r;
    strcpy(r.id_membre, id);
    strcpy(r.nom_membre, nom);
    strcpy(r.id_coach, coach_id);
    strcpy(r.cours, cours);
    r.jour = day;
    r.mois = month + 1;
    r.annee = year;
    r.heure = heure;

    enregistrer_demande(r);
    
    // Free string from model get
    g_free(coach_id);

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    "Demande envoyee avec succes!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Add Coach (Admin)
void
on_buttonAjouter_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *entryIdentifiantEntraineur = lookup_widget(GTK_WIDGET(button), "entryIdentifiantEntraineur");
    GtkWidget *entryNomEntraineur = lookup_widget(GTK_WIDGET(button), "entryNomEntraineur");
    GtkWidget *comboboxentryCours6 = lookup_widget(GTK_WIDGET(button), "comboboxentryCours6");

    Coach c;
    strcpy(c.id, gtk_entry_get_text(GTK_ENTRY(entryIdentifiantEntraineur)));
    strcpy(c.nom, gtk_entry_get_text(GTK_ENTRY(entryNomEntraineur)));
    strcpy(c.cours, gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboboxentryCours6)));

    ajouter_coach(c);

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    "Entraineur ajoute avec succes!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Search Coach (Member)
void
on_buttonChercherCoach_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *comboboxentrycours5 = lookup_widget(GTK_WIDGET(button), "comboboxentrycours5");
    GtkWidget *treeviewCoach = lookup_widget(GTK_WIDGET(button), "treeviewCoach");
    
    gchar *cours = gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboboxentrycours5));
    if(cours) {
        chercher_coach_par_cours(treeviewCoach, cours);
        g_free(cours);
    }
}









void
on_buttonEntraineur_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_buttonQuitter_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *current_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *login_window = create_login();
    gtk_widget_show(login_window);
    gtk_widget_destroy(current_window);
}


void
on_buttonAccepter_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_buttonRefuser_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_buttonChercher7_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_buttonQuitter10_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *current_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *login_window = create_login();
    gtk_widget_show(login_window);
    gtk_widget_destroy(current_window);
}


void
on_buttonQuitter1_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *current_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *login_window = create_login();
    gtk_widget_show(login_window);
    gtk_widget_destroy(current_window);
}


void
on_buttonQuitter2_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *current_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *login_window = create_login();
    gtk_widget_show(login_window);
    gtk_widget_destroy(current_window);
}


void
on_buttonQuitter8_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *current_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *login_window = create_login();
    gtk_widget_show(login_window);
    gtk_widget_destroy(current_window);
}


void
on_buttonQuitter9_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *current_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *login_window = create_login();
    gtk_widget_show(login_window);
    gtk_widget_destroy(current_window);
}

