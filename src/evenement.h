#ifndef EVENEMENT_H
#define EVENEMENT_H
#include <gtk/gtk.h>




// Structure pour la date de l'événement
typedef struct {
    int j;
    int m;
    int a;
} date_event;

// Structure pour l'heure de l'événement
typedef struct {
    int heure;   // 0-23
    int minute;  // 0-59
} heure_event;

// Structure pour Événement
typedef struct {
    int id_event;
    char nom_event[300];
    char type_event[100];
    char centre[200];
    char salle[100];          // NEW: Room/Hall name
    char description[1000];
    date_event date_e;
    heure_event heure_e;
    float prix;               // NEW: Event price
    char statut[20];          // "en_cours", "terminer", "Annule"
} evenement;

// Structure pour Inscription à un événement
typedef struct {
    int id_inscription;
    char cin_membre[20];
    int id_event;
    char nom_event[300];
    char type_event[100];
    char centre[200];
    char salle[100];          // NEW: Room/Hall name
    char description[1000];
    date_event date_e;
    heure_event heure_e;
    float prix;               // NEW: Event price
} inscription_event;

// ==== PROTOTYPES - EVENEMENT CRUD ====
void ajouter_evenement(evenement ev);
int evenement_existe(int id_event);
void supprimer_evenement(int id_event);
void modifier_evenement(evenement ev);
evenement trouver_evenement(int id_event);
int get_next_event_id();

// ==== PROTOTYPES - AFFICHAGE & RECHERCHE ====
void afficher_evenements(GtkWidget *treeview);
int rechercher_evenements(const char *critere, GtkWidget *treeview);

// ==== PROTOTYPES - INSCRIPTION ====
void ajouter_inscription_event(inscription_event insc);
void supprimer_inscription_event(int id_inscription);
int inscription_existe(const char *cin_membre, int id_event);
void afficher_inscriptions_membre(GtkWidget *treeview, const char *cin_membre);
int get_next_inscription_id();

// ==== PROTOTYPES - UTILITY ====
void remplir_combobox_centres(GtkWidget *combobox);
void remplir_combobox_type_evenement(GtkWidget *combobox);

#endif // EVENEMENT_H
