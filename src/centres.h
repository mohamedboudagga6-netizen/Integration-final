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

Centre Chercher_Centre(const char *id);

#endif
