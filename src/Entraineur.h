#ifndef ENTRAINEUR_H_INCLUDED
#define ENTRAINEUR_H_INCLUDED
#include <stdbool.h>

typedef struct {
    int jour;
    int mois;
    int annee;
} Date;

typedef struct {
    char id[20];
    char nom[50];
    char prenom[50];
    char mot_de_passe[50];
    char telephone[20];
    char sexe[10];
    char photo[200];
    char cv[200];
    char specialite[50];
    Date date_naissance;
} Entraineur;

typedef struct {
    char nom[50];
    char prenom[50];
    char session[20];       // "jour" ou "soir"
    char type_cours[50];    // Boxe, Musculation, YOGA, Fitness, etc.
} Inscription;

// Fonctions CRUD (sans printf/scanf)
int ajouter_entraineur(char *filename, Entraineur E);
int modifier_entraineur(char *filename, char *id, Entraineur nouv);
int supprimer_entraineur(char *filename, char *id);
Entraineur chercher_entraineur(char *filename, char *id);

// Fonctions de validation (retournent true/false uniquement)
bool validerMotDePasse(const char *mdp);
bool validerTelephone(const char *tel);
bool validerSexe(const char *sexe);
bool validerDate(int jour, int mois, int annee);
bool idExiste(char *filename, const char *id);

// Fonctions utilitaires
void enleverSautLigne(char *str);

// Fonction d'inscription aux cours
int inscrireCours(char *filename, Inscription I);

#endif
