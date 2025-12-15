#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include "Entraineur.h"

// ==================== FONCTIONS UTILITAIRES ====================

void enleverSautLigne(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

// Fonction pour parser une ligne avec des champs potentiellement vides
int parser_ligne_entraineur(char *ligne, Entraineur *E) {
    // Copier la ligne car on va la modifier
    char ligne_copy[1024];
    strncpy(ligne_copy, ligne, sizeof(ligne_copy) - 1);
    ligne_copy[sizeof(ligne_copy) - 1] = '\0';
    
    // Enlever le saut de ligne
    enleverSautLigne(ligne_copy);
    
    // Parser manuellement pour gérer les champs vides
    char *ptr = ligne_copy;
    char *champs[12];
    int field_count = 0;
    
    // Extraire tous les champs
    for (int i = 0; i < 12 && field_count < 12; i++) {
        char *next_pipe = strchr(ptr, '|');
        
        if (next_pipe != NULL) {
            *next_pipe = '\0';  // Terminer le champ
            champs[field_count++] = ptr;
            ptr = next_pipe + 1;  // Avancer au prochain champ
        } else {
            // Dernier champ (pas de pipe après)
            champs[field_count++] = ptr;
            break;
        }
    }
    
    // Vérifier qu'on a bien 12 champs
    if (field_count < 12) {
        g_print("[DEBUG parser] Seulement %d champs trouvés (attendu 12)\n", field_count);
        return 0;
    }
    
    // Remplir la structure
    strncpy(E->id, champs[0], sizeof(E->id) - 1); E->id[sizeof(E->id) - 1] = '\0';
    strncpy(E->nom, champs[1], sizeof(E->nom) - 1); E->nom[sizeof(E->nom) - 1] = '\0';
    strncpy(E->prenom, champs[2], sizeof(E->prenom) - 1); E->prenom[sizeof(E->prenom) - 1] = '\0';
    strncpy(E->mot_de_passe, champs[3], sizeof(E->mot_de_passe) - 1); E->mot_de_passe[sizeof(E->mot_de_passe) - 1] = '\0';
    strncpy(E->telephone, champs[4], sizeof(E->telephone) - 1); E->telephone[sizeof(E->telephone) - 1] = '\0';
    strncpy(E->sexe, champs[5], sizeof(E->sexe) - 1); E->sexe[sizeof(E->sexe) - 1] = '\0';
    strncpy(E->photo, champs[6], sizeof(E->photo) - 1); E->photo[sizeof(E->photo) - 1] = '\0';
    strncpy(E->cv, champs[7], sizeof(E->cv) - 1); E->cv[sizeof(E->cv) - 1] = '\0';
    strncpy(E->specialite, champs[8], sizeof(E->specialite) - 1); E->specialite[sizeof(E->specialite) - 1] = '\0';
    E->date_naissance.jour = atoi(champs[9]);
    E->date_naissance.mois = atoi(champs[10]);
    E->date_naissance.annee = atoi(champs[11]);
    
    return 1;
}

// ==================== FONCTIONS DE VALIDATION ====================

bool validerMotDePasse(const char *mdp) {
    if (strlen(mdp) < 8) return false;
    
    bool hasUpper = false, hasLower = false, hasDigit = false;
    
    for (int i = 0; mdp[i]; i++) {
        if (isupper(mdp[i])) hasUpper = true;
        if (islower(mdp[i])) hasLower = true;
        if (isdigit(mdp[i])) hasDigit = true;
    }
    
    return (hasUpper && hasLower && hasDigit);
}

bool validerTelephone(const char *tel) {
    if (strlen(tel) != 8) return false;
    
    for (int i = 0; i < 8; i++) {
        if (!isdigit(tel[i])) return false;
    }
    
    return true;
}

bool validerSexe(const char *sexe) {
    char s[20];
    strcpy(s, sexe);
    
    for (int i = 0; s[i]; i++) {
        s[i] = tolower(s[i]);
    }
    
    return (strcmp(s, "m") == 0 || strcmp(s, "f") == 0 ||
            strcmp(s, "male") == 0 || strcmp(s, "female") == 0 ||
            strcmp(s, "homme") == 0 || strcmp(s, "femme") == 0);
}

bool validerDate(int jour, int mois, int annee) {
    if (annee < 1900 || annee > 2100) return false;
    if (mois < 1 || mois > 12) return false;
    if (jour < 1 || jour > 31) return false;
    
    int joursMax = 31;
    
    if (mois == 4 || mois == 6 || mois == 9 || mois == 11) {
        joursMax = 30;
    } else if (mois == 2) {
        bool bissextile = ((annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0));
        joursMax = bissextile ? 29 : 28;
    }
    
    return (jour <= joursMax);
}

bool idExiste(char *filename, const char *id) {
    Entraineur E;
    FILE *f = fopen(filename, "r");
    
    g_print("[DEBUG idExiste] Fichier: %s, ID recherché: '%s'\n", filename, id);
    
    if (!f) {
        g_print("[DEBUG idExiste] ERREUR: Impossible d'ouvrir le fichier!\n");
        return false;
    }

    g_print("[DEBUG idExiste] Fichier ouvert avec succès\n");
    char ligne[1024];
    int line_num = 0;
    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        line_num++;
        g_print("[DEBUG idExiste] Ligne %d: %s", line_num, ligne);
        
        if (parser_ligne_entraineur(ligne, &E)) {
            g_print("[DEBUG idExiste] Parse OK - ID trouvé dans ligne: '%s'\n", E.id);
            if (strcmp(E.id, id) == 0) {
                g_print("[DEBUG idExiste] MATCH! ID '%s' existe déjà\n", id);
                fclose(f);
                return true;
            }
        } else {
            g_print("[DEBUG idExiste] Parse FAILED pour ligne %d\n", line_num);
        }
    }
    
    g_print("[DEBUG idExiste] Fin du fichier - ID '%s' n'existe PAS\n", id);
    fclose(f);
    return false;
}

// ==================== FONCTIONS CRUD ====================

int ajouter_entraineur(char *filename, Entraineur E) {
    // Vérifier si l'ID existe déjà
    if (idExiste(filename, E.id)) {
        return 0;  // ID déjà existant, échec
    }
    
    FILE *f = fopen(filename, "a");
    
    if (f == NULL) {
        return 0;
    }
    
    fprintf(f, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%d|%d|%d\n",
            E.id, E.nom, E.prenom, E.mot_de_passe, E.telephone,
            E.sexe, E.photo, E.cv, E.specialite,
            E.date_naissance.jour, E.date_naissance.mois, E.date_naissance.annee);
    
    fclose(f);
    return 1;
}

int modifier_entraineur(char *filename, char *id, Entraineur nouv) {
    int tr = 0;
    Entraineur E;
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("tmp.txt", "w");

    if (f == NULL || f2 == NULL) {
        if (f) fclose(f);
        if (f2) fclose(f2);
        return 0;
    }

    char ligne[1024];
    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        if (parser_ligne_entraineur(ligne, &E)) {
            if (strcmp(E.id, id) == 0) {
                fprintf(f2, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%d|%d|%d\n",
                        nouv.id, nouv.nom, nouv.prenom, nouv.mot_de_passe, nouv.telephone,
                        nouv.sexe, nouv.photo, nouv.cv, nouv.specialite,
                        nouv.date_naissance.jour, nouv.date_naissance.mois, 
                        nouv.date_naissance.annee);
                tr = 1;
            } else {
                fprintf(f2, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%d|%d|%d\n",
                        E.id, E.nom, E.prenom, E.mot_de_passe, E.telephone,
                        E.sexe, E.photo, E.cv, E.specialite,
                        E.date_naissance.jour, E.date_naissance.mois, E.date_naissance.annee);
            }
        }
    }
    
    fclose(f);
    fclose(f2);
    remove(filename);
    rename("tmp.txt", filename);
    
    return tr;
}

int supprimer_entraineur(char *filename, char *id) {
    int tr = 0;
    Entraineur E;
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("tmp.txt", "w");

    if (f == NULL || f2 == NULL) {
        if (f) fclose(f);
        if (f2) fclose(f2);
        return 0;
    }

    char ligne[1024];
    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        if (parser_ligne_entraineur(ligne, &E)) {
            if (strcmp(E.id, id) != 0) {
                fprintf(f2, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%d|%d|%d\n",
                        E.id, E.nom, E.prenom, E.mot_de_passe, E.telephone,
                        E.sexe, E.photo, E.cv, E.specialite,
                        E.date_naissance.jour, E.date_naissance.mois, E.date_naissance.annee);
            } else {
                tr = 1;
            }
        }
    }
    
    fclose(f);
    fclose(f2);
    remove(filename);
    rename("tmp.txt", filename);
    
    return tr;
}

Entraineur chercher_entraineur(char *filename, char *id) {
    Entraineur E;
    int tr = 0;
    FILE *f = fopen(filename, "r");
    
    if (f != NULL) {
        char ligne[1024];
        while (fgets(ligne, sizeof(ligne), f) != NULL) {
            if (parser_ligne_entraineur(ligne, &E)) {
                if (strcmp(E.id, id) == 0) {
                    tr = 1;
                    break;
                }
            }
        }
        fclose(f);
    }
    
    if (!tr) {
        strcpy(E.id, "-1");
    }
    
    return E;
}

// ==================== FONCTION INSCRIPTION COURS ====================

int inscrireCours(char *filename, Inscription I) {
    FILE *f = fopen(filename, "a");
    
    if (f == NULL) {
        return 0;
    }
    
    fprintf(f, "%s|%s|%s|%s\n",
            I.nom, I.prenom, I.session, I.type_cours);
    
    fclose(f);
    return 1;
}
