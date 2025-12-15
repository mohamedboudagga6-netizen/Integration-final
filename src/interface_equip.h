#include <stdio.h>



typedef struct Reservation {

    char date[12];

    int heure_debut;

    int duree;

} Reservation;



typedef struct Equipement {

    char nom[32];

    char marque[32];

    char id[20];           

    int quantite;

    char id_centre[20];    

    char date_achat[24];

    char etat[20];         

    char utilisation[12];

    char type[12];

    float prix;            

    Reservation tab_reservation[16];

} Equipement;



int ajouter_equip(const char * filename ,Equipement *eq);

int supprimer_equip(const char *filename, const char *id);

int rechercher_equip(const char *filename, const char name[], Equipement result[]);

int reserver_equip(const char * filename ,char* id , Reservation *rv);

int modifier_equip(const char *filename, const char *id, Equipement *new_eq);
