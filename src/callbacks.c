#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "centre.h"

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

