

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "centre.h"

Centre Chercher_Centre(const char *id)
{
    Centre c = {0};
    FILE *f = fopen("centre.txt", "r");
    if (!f) return c;

    char ligne[2048];
    while (fgets(ligne, sizeof(ligne), f)) {
        Centre temp;
        if (sscanf(ligne, "%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%d\t%[^\t]\t%[^\n]",
                   temp.id, temp.nom, temp.adresse, temp.tel, temp.email,
                   temp.type, &temp.capacite, temp.public_vise, temp.description) == 9) {
            if (strcmp(temp.id, id) == 0) {
                fclose(f);
                return temp;
            }
        }
    }
    fclose(f);
    return c;
}

