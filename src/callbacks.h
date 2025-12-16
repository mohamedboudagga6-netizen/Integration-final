#include <gtk/gtk.h>



void on_GCIA_buttonEnregistrer_clicked(GtkWidget *button, gpointer user_data);

void on_GCIA_comboType_entry_changed(GtkEntry *entry, gpointer user_data);

void on_GCIA_comboType_list_selected(GtkList *list, GtkWidget *child, gpointer user_data);




void
on_GCIA_radiobutton2_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_GCIA_radiobutton3_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_GCIA_radiobutton4_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_GCIA_spinbutton1_value_changed      (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_GCIP_buttonAjouter_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIA_radiobutton3_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIA_radiobuttonE_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_GCIM_spinbutton1_value_changed      (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_GCIM_buttonEnregistrer_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIM_radiobuttonTP_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_GCIM_radiobuttonAU_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_GCIM_radiobuttonEA_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_GCIP_buttonAjouter_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIP_buttonModifier_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIM_buttonChercher_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIM_radiobuttonAU_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_GCIM_radiobuttonAU_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_GCIA_buttonprecedent_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIA_buttonquitter_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIM_buttonprecedent_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIM_buttonquitter_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIP_treeview_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_GCIP_buttonquitter_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIP_button_Supprimer_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIP_button_Actualiser_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIP_button_Chercher_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIA_buttonGestion_centre_clicked   (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIA_buttonModifier_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIM_buttonAjouter_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIM_buttonGestion_centre_clicked   (GtkButton       *button,
                                        gpointer         user_data);

void
on_GCIA_Renitialiser_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_TIEC_buttonPrecedent_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_TIEC_buttonEnregistrer_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_TIEC_buttonQuitter_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_TIEC_spinbuttonJour_value_changed   (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_TIEC_spinbuttonMois_value_changed   (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_TIEC_spinbuttonAnnees_value_changed (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_TIEC_radiobuttonPermanent_toggled   (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_TIEC_radiobuttonContractuel_toggled (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_TIEC_radiobuttonVacataire_toggled   (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_TIEC_checkbuttonMatin_toggled       (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_TIEC_checkbuttonApr__s_midi_toggled (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_TIEC_checkbuttonSoir_toggled        (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_TIEC_checkbuttonWeek_end_toggled    (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_TIEC_buttonRenitialiser_clicked     (GtkButton       *button,
                                        gpointer         user_data);




void setup_treeview_columns(GtkWidget *treeview);
void afficher_equipements(GtkWidget *treeview, const char *filename);
void on_buttonEnregistrer_clicked(GtkButton *button,gpointer user_data);

void
on_sreserve_buttonPrecedent_clicked    (GtkButton       *button,
                                        gpointer         user_data);

void
on_sreserve_buttonEnregistrer_clicked  (GtkButton       *button,
                                        gpointer         user_data);

void
on_sreserve_buttonQuitter_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_smodifbuttonPrecedent_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_smodifbuttonQuitter_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_swarnbuttonCancel_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_Swarnbuttonok_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_scherbuttonChercher_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_scherbuttonPrecedent_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_scherbuttonQuitter_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_scherbuttonModifier_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_scherbuttonSupprimer_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_smodifbuttonEnregistrer_clicked     (GtkButton       *button,
                                        gpointer         user_data);


void
on_button_chercher_evenement_clicked   (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_ouvrir_ajout_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_modifer_ouvrire_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_supp_hela_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_window_gestion_evenement_show       (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_window_ajouter_evenement_show       (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_window_modifier_evenement_show      (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_radiobutton_annulee_mod_toggled     (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_radiobutton_terminer_mod_toggled    (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_radiobutton_encours_mod_toggled     (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_button_modfier_event_hela_clicked   (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_annulee_mod_hela_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_ajouter_event_hela_clicked   (GtkButton       *button,
                                        gpointer         user_data);

void
on_treeview_hela_evenment_cursor_changed
                                        (GtkTreeView     *treeview,
                                        gpointer         user_data);

void
on_windowsinscireevent_show            (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_treeview_sincrire_event_cursor_changed
                                        (GtkTreeView     *treeview,
                                        gpointer         user_data);

void
on_button_sinscire_event_hela_clicked  (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_voir_mes_inscription_clicked (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_annuler_inscription_clicked  (GtkButton       *button,
                                        gpointer         user_data);

void
on_treeview_sincrire_event_row_activated
                                        (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);

void
on_radiobutton_encours_mod_toggled     (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_radiobutton_terminer_mod_toggled    (GtkToggleButton *togglebutton,
                                        gpointer         user_data);


// Fonctions utilitaires
void afficher_message(GtkWidget *parent, const char *message, GtkMessageType type);
void afficher_entraineurs(GtkWidget *treeview);
void afficher_inscriptions(GtkWidget *treeview);
void on_treeview_row_activated(GtkTreeView *treeview, GtkTreePath *path,
                               GtkTreeViewColumn *column, gpointer user_data);
void on_treeview_cursor_changed(GtkTreeView *treeview, gpointer user_data);
void set_textview_text(GtkWidget *textview, const char *text);
char* format_date(int jour, int mois, int annee);

// Callbacks Login principal
void on_button47_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button48_mohamed_clicked(GtkWidget *button, gpointer user_data);

// Callbacks Login Admin
void on_button54_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button53_mohamed_clicked(GtkWidget *button, gpointer user_data);

// Callbacks Login Entraineur
void on_button74_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button73_mohamed_clicked(GtkWidget *button, gpointer user_data);

// Callbacks Accueil
void on_button67_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button70_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button69_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button68_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button71_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button72_mohamed_clicked(GtkWidget *button, gpointer user_data);

// Callbacks Ajouter
void on_button55_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button56_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button63_mohamed_clicked(GtkWidget *button, gpointer user_data);

// Callbacks Modifier
void on_button59_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button61_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button60_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button62_mohamed_clicked(GtkWidget *button, gpointer user_data);

// Callbacks Supprimer
void on_button10_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button15_mohamed_clicked(GtkWidget *button, gpointer user_data);

// Callbacks Inscription cours
void on_button77_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button76_mohamed_clicked(GtkWidget *button, gpointer user_data);
void on_button78_mohamed_clicked(GtkWidget *button, gpointer user_data);




