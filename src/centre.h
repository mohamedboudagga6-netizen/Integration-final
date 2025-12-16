#ifndef CENTRE_H
#define CENTRE_H
#include <gtk/gtk.h>


typedef struct {
    char id[50];
    char nom[100];
    char adresse[200];
    char tel[20];
    char email[100];
    char type[50];
    int capacite;
    char public_vise[50];
    char description[500];
} Centre;
typedef struct {
    char nom[50];
    char prenom[100];
    char date_nai[50];
    char tel[20];
    char email[100];
    char adresse[200];
    char cin[20];
    char specialite[50];
    char disponibilite[50];
    char contrat[500];
} Coach; 

void Ajouter_Coach_Centre(const char *nom,const char *prenom,const char *date_nai,const char *tel,const char *email,const char *adresse,const char * cin,const char *specialite,const char *disponibilite,const char *contrat);

void Ajouter(const char *id,const char *nom,const char *adresse,const char *tel,const char *email,const char *type,int capacite,const char *public_vise,const char *description);

Centre Chercher(const char *id);
int Rechercher_Centre(GtkWidget *treeview, const char *id);
int Modifier(const char *id, const char *nom, const char *adresse, const char *tel,
             const char *email, const char *type, int capacite,
             const char *public_vise, const char *description);
void Afficher_Centres(GtkWidget *treeview);
int Supprimer(const char *id);
#endif

