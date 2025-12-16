#ifndef MEMBER_H
#define MEMBER_H

#include <gtk/gtk.h>

typedef struct {
    char id[50];
    char nom[50];
    char prenom[50];
    char password[50];
    char email[100];
    char phone[50];
    char cp[20];
    char ville[50];
    char sexe[20];
    char type_abonn[50];
    char mode_paiement[50];
    char cours[50];
    char centre[50];
    char objectif[100];
    int jour;
    int mois;
    int annee;
    int auth_medicale; // 1 = oui, 0 = non
} Member;


typedef struct {
    char id[50];
    char nom[50];
    char cours[50];
} Coach;

typedef struct {
    char id_membre[50];
    char nom_membre[50];
    char id_coach[50]; // Selected coach
    char cours[50];
    int jour;
    int mois;
    int annee;
    int heure; // Just hour for simplicity as per UI
} CoachRequest;

void ajouter_membre(Member m);
void modifier_membre(Member m, char *id_to_modify);
void supprimer_membre(char *id);
void chercher_membre(GtkWidget *treeview, char *id);
void afficher_membre(GtkWidget *treeview);
int verifier_id(char *id);
Member get_member(char *id);

// New Functions
int check_login(char *id, char *password); 
void ajouter_coach(Coach c);
void chercher_coach_par_cours(GtkWidget *treeview, char *cours);
void enregistrer_demande(CoachRequest r);

#endif

