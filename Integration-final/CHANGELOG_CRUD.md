# Documentation des modifications CRUD - FitVerse

## Vue d'ensemble

Ce document décrit toutes les modifications apportées pour implémenter les opérations CRUD (Create, Read, Update, Delete) dans l'application FitVerse de gestion de cours.

## Fichiers modifiés

### 1. `src/callbacks.c`
Fichier principal contenant toute la logique CRUD.

### 2. `src/callbacks.h`
Ajout de la déclaration de la fonction `on_GC_notebook_value_changed`.

### 3. `src/main.c`
Modification pour ne pas afficher la fenêtre TIEC par défaut.

## Structure de données

### Structure `Cours`
```c
typedef struct {
    gchar *id;              // Identifiant unique du cours
    gchar *nom;             // Nom du cours
    gchar *type;             // Type de cours (Cardio, Musculation, etc.)
    gchar *niveau;           // Niveau (Débutant, Intermédiaire, Avancé)
    gchar *entraineur;       // Nom de l'entraîneur
    gchar *date;             // Date du cours (format: JJ/MM/AAAA)
    gint duree;              // Durée en minutes
    gint capacite;           // Capacité maximale
    gdouble prix;            // Prix du cours
    gchar *description;      // Description détaillée
    gchar *visibilite;       // Visibilité (Public, Privé, Membres seulements)
} Cours;
```

## Variables globales

- `liste_cours` : Liste chaînée (GList) contenant tous les cours
- `store` : Modèle de données pour le GtkTreeView
- `cours_selectionne_id` : ID du cours actuellement sélectionné pour modification
- `main_window_ref` : Référence à la fenêtre principale

## Fonctions implémentées

### Fonctions utilitaires

#### `cours_free(Cours *cours)`
Libère la mémoire allouée pour un cours.

#### `charger_cours()`
Charge tous les cours depuis le fichier `cours.dat` dans la liste globale.

**Format du fichier :**
```
COURS:
ID: COURS001
NOM: Yoga Matin
TYPE: Bien être/Yoga
NIVEAU: Débutant
ENTRAINEUR: Marie Dupont
DATE: 15/12/2024
DUREE: 60
CAPACITE: 20
PRIX: 25.00
DESCRIPTION: Cours de yoga relaxant le matin
VISIBILITE: Public
END
```

#### `sauvegarder_cours()`
Sauvegarde tous les cours de la liste dans le fichier `cours.dat`.

#### `mettre_a_jour_treeview()`
Met à jour l'affichage du treeview avec tous les cours.
- Crée les colonnes si nécessaire
- Remplit le treeview avec les données

### Opérations CRUD

## CREATE - Créer un cours

### Fonction : `on_Aj_buttonEnregistrer_clicked()`

**Description :** Ajoute un nouveau cours à la liste.

**Fonctionnalités :**
- Validation des champs obligatoires (ID et Nom)
- Vérification de l'unicité de l'ID
- Récupération de tous les champs du formulaire :
  - ID, Nom, Type, Niveau, Entraineur
  - Date (depuis le calendrier)
  - Durée, Capacité, Prix
  - Description (depuis le textview)
  - Visibilité (radio buttons)
- Sauvegarde automatique dans le fichier
- Mise à jour de l'affichage
- Vidage des champs après enregistrement
- Message de confirmation

**Validation :**
- ID et Nom sont obligatoires
- L'ID doit être unique

## READ - Lire/Afficher les cours

### Fonction : `mettre_a_jour_treeview()`

**Description :** Affiche tous les cours dans le treeview.

**Colonnes affichées :**
1. ID
2. Nom
3. Type
4. Niveau
5. Entraineur
6. Date
7. Durée (minutes)
8. Capacité
9. Prix

### Fonction : `on_Ac_buttonrechercher_clicked()`

**Description :** Recherche des cours par nom.

**Fonctionnalités :**
- Recherche partielle dans le nom du cours
- Filtre le treeview pour afficher uniquement les résultats
- Si le champ est vide, affiche tous les cours

### Fonction : `on_Ac_treeview_check_resize()`

**Description :** Initialise le treeview au démarrage.

**Fonctionnalités :**
- Charge les cours depuis le fichier
- Initialise l'affichage du treeview

### Fonction : `on_GC_notebook_value_changed()`

**Description :** Gère le changement d'onglet dans le notebook.

**Fonctionnalités :**
- Lors du retour à l'onglet principal, met à jour le treeview
- Charge les cours si nécessaire

## UPDATE - Modifier un cours

### Fonction : `on_Ac_buttonmodifier_clicked()`

**Description :** Charge un cours sélectionné pour modification.

**Fonctionnalités :**
- Récupère le cours sélectionné dans le treeview
- Charge toutes les données dans le formulaire de modification :
  - Remplit tous les champs (nom, type, niveau, etc.)
  - Configure le calendrier avec la date
  - Configure les spinbuttons (durée, capacité)
  - Remplit le textview avec la description
  - Sélectionne le bon radio button pour la visibilité
- Affiche l'ID dans le label
- Change d'onglet vers le formulaire de modification

**Validation :**
- Un cours doit être sélectionné dans le treeview

### Fonction : `on_Mod_buttonChercher_clicked()`

**Description :** Recherche un cours par ID pour modification.

**Fonctionnalités :**
- Recherche un cours par son ID
- Charge les données dans le formulaire si trouvé
- Affiche un message d'erreur si non trouvé

### Fonction : `on_Mod_buttonEnregistrer_clicked()`

**Description :** Sauvegarde les modifications d'un cours.

**Fonctionnalités :**
- Met à jour tous les champs du cours sélectionné
- Sauvegarde dans le fichier
- Met à jour l'affichage du treeview
- Retourne à l'onglet principal
- Affiche un message de confirmation

## DELETE - Supprimer un cours

### Fonction : `on_Ac_buttonsupprimer_clicked()`

**Description :** Supprime un cours de la liste.

**Fonctionnalités :**
- Récupère le cours sélectionné dans le treeview
- Affiche une boîte de dialogue de confirmation
- Supprime le cours de la liste si confirmé
- Libère la mémoire
- Sauvegarde les modifications
- Met à jour l'affichage

**Validation :**
- Un cours doit être sélectionné
- Confirmation requise avant suppression

## Navigation

### Fonctions de navigation

#### `on_Ac_buttonajouter_clicked()`
Change d'onglet vers le formulaire d'ajout.

#### `on_Aj_buttonprecedent_clicked()`
Retourne à l'onglet principal depuis le formulaire d'ajout.

#### `on_Mod_buttonprecedent_clicked()`
Retourne à l'onglet principal depuis le formulaire de modification.
Libère également l'ID du cours sélectionné.

#### `on_Ac_buttonquitter_clicked()`
Quitte l'application.

#### `on_Aj_buttonquitter_clicked()`
Quitte l'application.

#### `on_Mod_buttonquitter_clicked()`
Quitte l'application.

## Stockage des données

### Fichier : `cours.dat`

**Emplacement :** Répertoire de l'application

**Format :** Fichier texte structuré

**Structure :**
- Chaque cours commence par `COURS:`
- Chaque champ est sur une ligne avec le format `CHAMP: valeur`
- Chaque cours se termine par `END`
- Les champs sont :
  - ID
  - NOM
  - TYPE
  - NIVEAU
  - ENTRAINEUR
  - DATE
  - DUREE
  - CAPACITE
  - PRIX
  - DESCRIPTION
  - VISIBILITE

**Exemple :**
```
COURS:
ID: COURS001
NOM: Yoga Matin
TYPE: Bien être/Yoga
NIVEAU: Débutant
ENTRAINEUR: Marie Dupont
DATE: 15/12/2024
DUREE: 60
CAPACITE: 20
PRIX: 25.00
DESCRIPTION: Cours de yoga relaxant le matin
VISIBILITE: Public
END
COURS:
ID: COURS002
NOM: Cardio Intense
TYPE: Cardio
NIVEAU: Avancé
ENTRAINEUR: Jean Martin
DATE: 16/12/2024
DUREE: 45
CAPACITE: 15
PRIX: 30.00
DESCRIPTION: Entraînement cardio intensif
VISIBILITE: Membres seulements
END
```

## Interface utilisateur

### Onglet 1 : Gestion Cours (Principal)
- **Treeview** : Affichage de tous les cours
- **Bouton Rechercher** : Recherche par nom
- **Bouton Ajouter** : Accès au formulaire d'ajout
- **Bouton Modifier** : Accès au formulaire de modification
- **Bouton Supprimer** : Suppression d'un cours
- **Bouton Précédent** : Retour/Quitter
- **Bouton Quitter** : Fermer l'application

### Onglet 2 : Ajouter
- Formulaire complet pour créer un nouveau cours
- Tous les champs du cours
- Bouton Enregistrer : Sauvegarde le nouveau cours
- Bouton Précédent : Retour à l'onglet principal
- Bouton Quitter : Fermer l'application

### Onglet 3 : Modifier
- Formulaire pré-rempli avec les données du cours
- Champ de recherche par ID
- Bouton Chercher : Recherche un cours par ID
- Bouton Enregistrer : Sauvegarde les modifications
- Bouton Précédent : Retour à l'onglet principal
- Bouton Quitter : Fermer l'application

## Messages utilisateur

### Messages de confirmation
- "Cours ajouté avec succès !"
- "Cours modifié avec succès !"

### Messages d'erreur
- "Veuillez remplir au moins l'ID et le nom du cours."
- "Un cours avec cet ID existe déjà."
- "Veuillez sélectionner un cours à modifier."
- "Veuillez sélectionner un cours à supprimer."
- "Veuillez entrer un ID de cours."
- "Aucun cours trouvé avec cet ID."

### Messages de confirmation
- "Êtes-vous sûr de vouloir supprimer ce cours ?" (dialogue Oui/Non)

## Gestion de la mémoire

- Utilisation de `g_new0()` pour l'allocation
- Utilisation de `g_strdup()` pour les chaînes
- Fonction `cours_free()` pour libérer la mémoire
- Libération de `cours_selectionne_id` après modification

## Points techniques importants

1. **Initialisation du treeview** : Se fait automatiquement au premier affichage
2. **Persistance des données** : Sauvegarde automatique après chaque modification
3. **Validation** : Vérification des champs obligatoires et unicité de l'ID
4. **Gestion d'erreurs** : Messages clairs pour l'utilisateur
5. **Interface réactive** : Mise à jour automatique de l'affichage

## Améliorations futures possibles

1. Export/Import des données (CSV, JSON)
2. Recherche avancée (par type, niveau, prix, etc.)
3. Tri des colonnes dans le treeview
4. Validation plus poussée des données (format date, prix positif, etc.)
5. Support de plusieurs fichiers de sauvegarde
6. Historique des modifications
7. Statistiques sur les cours

## Notes de compilation

Aucune dépendance supplémentaire n'est requise. Le code utilise uniquement :
- GTK+ (déjà dans le projet)
- GLib (déjà dans le projet)
- Fonctions standard C (stdio.h, stdlib.h, string.h)

## Tests recommandés

1. **Créer un cours** : Vérifier que tous les champs sont sauvegardés
2. **Modifier un cours** : Vérifier que les modifications sont persistantes
3. **Supprimer un cours** : Vérifier la suppression et la mise à jour
4. **Rechercher** : Tester la recherche par nom
5. **Navigation** : Vérifier tous les changements d'onglets
6. **Validation** : Tester les cas d'erreur (ID vide, ID dupliqué, etc.)
7. **Persistance** : Fermer et rouvrir l'application pour vérifier le chargement

