#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glib.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "cours.h"


/* Variables globales pour la gestion des cours */
static GList *liste_cours = NULL;
static gchar *cours_selectionne_id = NULL;
static GtkWidget *main_window_ref = NULL;
static const gchar *COURS_FILENAME = "cour.txt";

/* Fonction helper pour charger les données d'un cours dans le formulaire de modification */
static void charger_cours_dans_formulaire(Cours *cours, GtkWidget *window) {
    if (!cours || !window) return;
    
    GtkWidget *mod_entryNom = lookup_widget(window, "Mod_entryNom");
    GtkWidget *mod_comboType = lookup_widget(window, "Mod_comboType");
    GtkWidget *mod_comboNiveau = lookup_widget(window, "Mod_comboNiveau");
    GtkWidget *mod_entryEntaineur = lookup_widget(window, "Mod_entryEntaineur");
    GtkWidget *mod_calendar2 = lookup_widget(window, "calendar2");
    GtkWidget *mod_spinbutton2 = lookup_widget(window, "Mod_spinbutton2");
    GtkWidget *mod_spinbutton1 = lookup_widget(window, "Mod_spinbutton1");
    GtkWidget *mod_entryPrix = lookup_widget(window, "Mod_entryPrix");
    GtkWidget *mod_textview1 = lookup_widget(window, "Mod_textview1");
    GtkWidget *mod_labelID = lookup_widget(window, "Mod__labelID");
    
    if (mod_entryNom && cours->nom)
        gtk_entry_set_text(GTK_ENTRY(mod_entryNom), cours->nom);
    if (mod_comboType && cours->type) {
        GtkEntry *entry = GTK_ENTRY(GTK_COMBO(mod_comboType)->entry);
        gtk_entry_set_text(entry, cours->type);
    }
    if (mod_comboNiveau && cours->niveau) {
        /* Pour GTK2 ComboBoxEntry, on doit utiliser l'entry directement */
        GtkWidget *entry = gtk_bin_get_child(GTK_BIN(mod_comboNiveau));
        if (entry && GTK_IS_ENTRY(entry)) {
            gtk_entry_set_text(GTK_ENTRY(entry), cours->niveau);
        } else {
            /* Essayer de trouver l'index et le définir */
            GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(mod_comboNiveau));
            if (model) {
                gint n_items = gtk_tree_model_iter_n_children(model, NULL);
                gint i;
                for (i = 0; i < n_items; i++) {
                    GtkTreeIter iter;
                    if (gtk_tree_model_iter_nth_child(model, &iter, NULL, i)) {
                        gchar *text = NULL;
                        gtk_tree_model_get(model, &iter, 0, &text, -1);
                        if (text && strcmp(text, cours->niveau) == 0) {
                            gtk_combo_box_set_active(GTK_COMBO_BOX(mod_comboNiveau), i);
                            g_free(text);
                            break;
                        }
                        if (text) g_free(text);
                    }
                }
            }
        }
    }
    if (mod_entryEntaineur && cours->entraineur)
        gtk_entry_set_text(GTK_ENTRY(mod_entryEntaineur), cours->entraineur);
    if (mod_calendar2 && cours->date) {
        guint year, month, day;
        if (sscanf(cours->date, "%u/%u/%u", &day, &month, &year) == 3) {
            gtk_calendar_select_month(GTK_CALENDAR(mod_calendar2), month - 1, year);
            gtk_calendar_select_day(GTK_CALENDAR(mod_calendar2), day);
        }
    }
    if (mod_spinbutton2)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(mod_spinbutton2), cours->duree);
    if (mod_spinbutton1)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(mod_spinbutton1), cours->capacite);
    if (mod_entryPrix) {
        gchar prix_str[32];
        sprintf(prix_str, "%.2f", cours->prix);
        gtk_entry_set_text(GTK_ENTRY(mod_entryPrix), prix_str);
    }
    if (mod_textview1 && cours->description) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(mod_textview1));
        gtk_text_buffer_set_text(buffer, cours->description, -1);
    }
    if (mod_labelID && cours->id) {
        gchar *label_text = g_strdup_printf("<b>Identifiant: %s</b>", cours->id);
        gtk_label_set_markup(GTK_LABEL(mod_labelID), label_text);
        g_free(label_text);
    }
    
    /* Définir la visibilité */
    GtkWidget *mod_radiobutton1 = lookup_widget(window, "Mod_radiobutton1");
    GtkWidget *mod_radiobutton2 = lookup_widget(window, "Mod_radiobutton2");
    GtkWidget *mod_radiobuttonEA = lookup_widget(window, "Mod_radiobuttonEA");
    
    if (cours->visibilite) {
        if (strcmp(cours->visibilite, "Public") == 0 && mod_radiobutton1)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mod_radiobutton1), TRUE);
        else if (strcmp(cours->visibilite, "Privé") == 0 && mod_radiobutton2)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mod_radiobutton2), TRUE);
        else if (strcmp(cours->visibilite, "Membres seulements") == 0 && mod_radiobuttonEA)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mod_radiobuttonEA), TRUE);
    }
}

/* Fonction pour mettre à jour le treeview */
static void mettre_a_jour_treeview() {
    fprintf(stderr, "[DEBUG] mettre_a_jour_treeview: Début\n");
    
    if (!main_window_ref) {
        fprintf(stderr, "[DEBUG] Recherche de la fenêtre principale\n");
        /* Trouver la fenêtre principale */
        GList *toplevels = gtk_window_list_toplevels();
        GList *l;
        for (l = toplevels; l != NULL; l = l->next) {
            GtkWidget *widget = GTK_WIDGET(l->data);
            if (GTK_IS_WINDOW(widget)) {
                main_window_ref = widget;
                fprintf(stderr, "[DEBUG] Fenêtre principale trouvée: %p\n", (void*)main_window_ref);
                break;
            }
        }
        g_list_free(toplevels);
    }
    
    if (!main_window_ref) {
        fprintf(stderr, "[ERROR] Fenêtre principale non trouvée\n");
        return;
    }
    
    GtkWidget *treeview = lookup_widget(main_window_ref, "Ac_treeview");
    if (!treeview) {
        fprintf(stderr, "[ERROR] Treeview non trouvé\n");
        return;
    }
    
    fprintf(stderr, "[DEBUG] Treeview trouvé: %p\n", (void*)treeview);
    
    /* Initialiser le treeview si nécessaire */
    static gboolean treeview_initialise = FALSE;
    if (!treeview_initialise) {
        fprintf(stderr, "[DEBUG] Initialisation du treeview\n");
        cours_treeview_init(treeview);
        treeview_initialise = TRUE;
    }
    
    guint nb_cours = liste_cours ? g_list_length(liste_cours) : 0;
    fprintf(stderr, "[DEBUG] Mise à jour du treeview avec %u cours(s)\n", nb_cours);
    
    /* Mettre à jour le treeview même si la liste est vide (NULL) */
    cours_treeview_update(treeview, liste_cours);
    fprintf(stderr, "[DEBUG] Treeview mis à jour\n");
}


/* Fonction pour mettre les images en arrière-plan (GTK2 compatible) */
static void mettre_images_en_arriere_plan(GtkWidget *window) {
    if (!window) {
        fprintf(stderr, "[DEBUG] mettre_images_en_arriere_plan: window est NULL\n");
        return;
    }
    
    fprintf(stderr, "[DEBUG] mettre_images_en_arriere_plan: Début\n");
    
    /* Trouver les images et les mettre en arrière-plan */
    /* En GTK2, on ne peut pas utiliser gtk_widget_lower qui n'existe pas */
    /* Les images sont déjà en arrière-plan si elles sont ajoutées avant les autres widgets */
    /* Cette fonction est conservée pour compatibilité mais ne fait rien en GTK2 */
    GtkWidget *image37 = lookup_widget(window, "image37");
    GtkWidget *image38 = lookup_widget(window, "image38");
    GtkWidget *image39 = lookup_widget(window, "image39");
    
    fprintf(stderr, "[DEBUG] Images trouvées - image37: %p, image38: %p, image39: %p\n", 
            (void*)image37, (void*)image38, (void*)image39);
    
    /* S'assurer que les images sont visibles */
    if (image37 && !gtk_widget_get_visible(image37)) {
        gtk_widget_show(image37);
    }
    if (image38 && !gtk_widget_get_visible(image38)) {
        gtk_widget_show(image38);
    }
    if (image39 && !gtk_widget_get_visible(image39)) {
        gtk_widget_show(image39);
    }
    
    /* S'assurer que tous les boutons sont visibles et au-dessus de l'image */
    GtkWidget *button_ajouter = lookup_widget(window, "Ac_buttonajouter");
    GtkWidget *button_modifier = lookup_widget(window, "Ac_buttonmodifier");
    GtkWidget *button_supprimer = lookup_widget(window, "Ac_buttonsupprimer");
    
    fprintf(stderr, "[DEBUG] Boutons trouvés - ajouter: %p, modifier: %p, supprimer: %p\n",
            (void*)button_ajouter, (void*)button_modifier, (void*)button_supprimer);
    
    if (button_ajouter) {
        gboolean visible = gtk_widget_get_visible(button_ajouter);
        fprintf(stderr, "[DEBUG] Bouton ajouter visible: %s\n", visible ? "OUI" : "NON");
        if (!visible) {
            gtk_widget_show(button_ajouter);
            fprintf(stderr, "[DEBUG] Bouton ajouter rendu visible\n");
        }
        /* S'assurer que le parent (eventbox) est aussi visible */
        GtkWidget *parent = gtk_widget_get_parent(button_ajouter);
        if (parent) {
            gboolean parent_visible = gtk_widget_get_visible(parent);
            fprintf(stderr, "[DEBUG] Parent (eventbox) du bouton ajouter visible: %s\n", parent_visible ? "OUI" : "NON");
            if (!parent_visible) {
                gtk_widget_show(parent);
                fprintf(stderr, "[DEBUG] Parent (eventbox) rendu visible\n");
            }
        }
    } else {
        fprintf(stderr, "[WARNING] Bouton ajouter non trouvé!\n");
    }
    if (button_modifier) {
        if (!gtk_widget_get_visible(button_modifier)) {
            gtk_widget_show(button_modifier);
        }
        GtkWidget *parent = gtk_widget_get_parent(button_modifier);
        if (parent && !gtk_widget_get_visible(parent)) {
            gtk_widget_show(parent);
        }
    }
    if (button_supprimer) {
        if (!gtk_widget_get_visible(button_supprimer)) {
            gtk_widget_show(button_supprimer);
        }
        GtkWidget *parent = gtk_widget_get_parent(button_supprimer);
        if (parent && !gtk_widget_get_visible(parent)) {
            gtk_widget_show(parent);
        }
    }
    fprintf(stderr, "[DEBUG] mettre_images_en_arriere_plan: Fin\n");
}

/* Fonction pour s'assurer que l'image est chargée et affichée */
static void verifier_et_charger_image(GtkWidget *window, const gchar *image_name, const gchar *filename) {
    if (!window) return;
    
    GtkWidget *image = lookup_widget(window, image_name);
    if (!image) {
        /* L'image n'existe pas, la créer */
        GtkWidget *parent = lookup_widget(window, "Mod_fixed");
        if (parent && GTK_IS_FIXED(parent)) {
            image = create_pixmap(window, filename);
            if (image) {
                gtk_widget_show(image);
                /* En GTK2, l'ordre de rendu dépend de l'ordre d'insertion */
                /* On ajoute l'image en premier pour qu'elle soit en arrière-plan */
                gtk_fixed_put(GTK_FIXED(parent), image, 656, 0);
                gtk_widget_set_size_request(image, 344, 736);
                /* Enregistrer l'image pour lookup_widget */
                g_object_set_data(G_OBJECT(window), image_name, image);
            }
        }
    } else {
        /* S'assurer que l'image est visible */
        if (!gtk_widget_get_visible(image)) {
            gtk_widget_show(image);
        }
    }
}

void
on_GC_notebook_value_changed           (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        guint            page_num,
                                        gpointer         user_data)
{
    fprintf(stderr, "[DEBUG] on_GC_notebook_value_changed: page_num=%u\n", page_num);
    
    if (!main_window_ref) {
        main_window_ref = gtk_widget_get_toplevel(GTK_WIDGET(notebook));
        fprintf(stderr, "[DEBUG] main_window_ref défini: %p\n", (void*)main_window_ref);
    }
    
    /* Mettre les images en arrière-plan et s'assurer que les boutons sont visibles */
    mettre_images_en_arriere_plan(main_window_ref);
    
    /* page_num est le numéro de la page (0-indexed) */
    if (page_num == 0) {
        fprintf(stderr, "[DEBUG] Page principale (0)\n");
        /* Page principale - charger les cours et mettre à jour le treeview */
        /* Recharger les cours à chaque fois qu'on revient sur la page principale */
        /* pour s'assurer d'avoir les données les plus récentes */
        if (liste_cours) {
            cours_liste_free(liste_cours);
        }
        liste_cours = cours_charger_tous(COURS_FILENAME);
        guint nb_cours = liste_cours ? g_list_length(liste_cours) : 0;
        fprintf(stderr, "[DEBUG] Cours chargés: %u\n", nb_cours);
        /* Si le fichier n'existe pas encore, liste_cours sera NULL, ce qui est normal */
        mettre_a_jour_treeview();
    } else if (page_num == 1) {
        fprintf(stderr, "[DEBUG] Page d'ajout (1)\n");
    } else if (page_num == 2) {
        fprintf(stderr, "[DEBUG] Page de modification (2)\n");
        /* Page de modification - vérifier que l'image est chargée */
        verifier_et_charger_image(main_window_ref, "image39", "cours.jpg");
        mettre_images_en_arriere_plan(main_window_ref);
    }
}

void
on_Ac_treeview_check_resize            (GtkContainer    *container,
                                        gpointer         user_data)
{
    /* Initialiser le treeview au démarrage */
    static gboolean initialise = FALSE;
    if (!initialise) {
        initialise = TRUE;
        if (!main_window_ref) {
            main_window_ref = gtk_widget_get_toplevel(GTK_WIDGET(container));
        }
        /* S'assurer que tous les boutons sont visibles */
        mettre_images_en_arriere_plan(main_window_ref);
        /* Toujours charger les cours au démarrage */
        if (!liste_cours) {
            liste_cours = cours_charger_tous(COURS_FILENAME);
            /* Si le fichier n'existe pas, liste_cours sera NULL, ce qui est normal */
        }
        mettre_a_jour_treeview();
/* forcer la réalisation avant de toucher à la visibilité */
       if (!gtk_widget_get_realized(main_window_ref))
       gtk_widget_realize(main_window_ref);

      mettre_images_en_arriere_plan(main_window_ref);
      }
}


void
on_Ac_buttonajouter_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    fprintf(stderr, "[DEBUG] on_Ac_buttonajouter_clicked: Début\n");
    
    /* Trouver la fenêtre principale */
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (!window) {
        fprintf(stderr, "[DEBUG] Fenêtre non trouvée via get_toplevel, utilisation de main_window_ref\n");
        window = main_window_ref;
    }
    
    if (!window) {
        fprintf(stderr, "[ERROR] Impossible de trouver la fenêtre principale\n");
        return;
    }
    
    fprintf(stderr, "[DEBUG] Fenêtre trouvée: %p\n", (void*)window);

   
    /* Trouver le notebook */
    GtkWidget *notebook = lookup_widget(window, "GC_notebook");
    if (!notebook) {
        fprintf(stderr, "[DEBUG] Notebook non trouvé via window, essai via button\n");
        /* Essayer de trouver le notebook autrement */
        GtkWidget *notebook2 = lookup_widget(GTK_WIDGET(button), "GC_notebook");
        if (notebook2) {
            notebook = notebook2;
            fprintf(stderr, "[DEBUG] Notebook trouvé via button\n");
        }
    } else {
        fprintf(stderr, "[DEBUG] Notebook trouvé via window: %p\n", (void*)notebook);
    }
    
    if (notebook && GTK_IS_NOTEBOOK(notebook)) {
        fprintf(stderr, "[DEBUG] Changement vers l'onglet d'ajout (page 1)\n");
        /* Aller à l'onglet d'ajout (page 1) */
        gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1);
        fprintf(stderr, "[DEBUG] Page changée avec succès\n");
    } else {
        fprintf(stderr, "[ERROR] Notebook non valide ou non trouvé\n");
    }
}


void
on_Ac_buttonmodifier_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    fprintf(stderr, "[DEBUG] on_Ac_buttonmodifier_clicked: Début\n");
    
    GtkWidget *treeview = lookup_widget(GTK_WIDGET(button), "Ac_treeview");
    if (!treeview) {
        fprintf(stderr, "[ERROR] Treeview non trouvé\n");
        return;
    }
    
    fprintf(stderr, "[DEBUG] Treeview trouvé: %p\n", (void*)treeview);
    
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        fprintf(stderr, "[DEBUG] Ligne sélectionnée dans le treeview\n");
        gchar *id;
        gtk_tree_model_get(model, &iter, 0, &id, -1);
        
        if (id) {
            fprintf(stderr, "[DEBUG] ID sélectionné: '%s'\n", id);
            cours_selectionne_id = g_strdup(id);
            g_free(id);
            
            /* Charger les données du cours dans le formulaire de modification */
            Cours *cours = cours_trouver_par_id(liste_cours, cours_selectionne_id);
            if (cours) {
                fprintf(stderr, "[DEBUG] Cours trouvé, chargement dans le formulaire\n");
                GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
                charger_cours_dans_formulaire(cours, window);
            } else {
                fprintf(stderr, "[WARNING] Cours avec ID '%s' non trouvé dans la liste\n", cours_selectionne_id);
            }
            
            /* Aller à l'onglet de modification */
            GtkWidget *notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
            if (notebook) {
                fprintf(stderr, "[DEBUG] Changement vers l'onglet de modification (page 2)\n");
                gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 2);
                /* S'assurer que l'image est chargée après le changement d'onglet */
                GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
                if (window) {
                    verifier_et_charger_image(window, "image39", "cours.jpg");
                    mettre_images_en_arriere_plan(window);
                }
            } else {
                fprintf(stderr, "[ERROR] Notebook non trouvé\n");
            }
        } else {
            fprintf(stderr, "[WARNING] ID est NULL\n");
        }
    } else {
        fprintf(stderr, "[WARNING] Aucune ligne sélectionnée\n");
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_WARNING,
                                                   GTK_BUTTONS_OK,
                                                   "Veuillez sélectionner un cours à modifier.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}


void
on_Ac_buttonsupprimer_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *treeview = lookup_widget(GTK_WIDGET(button), "Ac_treeview");
    if (!treeview) return;
    
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *id;
        gtk_tree_model_get(model, &iter, 0, &id, -1);
        
        if (id) {
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                                                       GTK_DIALOG_MODAL,
                                                       GTK_MESSAGE_QUESTION,
                                                       GTK_BUTTONS_YES_NO,
                                                       "Êtes-vous sûr de vouloir supprimer ce cours ?");
            gint response = gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            
            if (response == GTK_RESPONSE_YES) {
                liste_cours = cours_supprimer_par_id(liste_cours, id);
                cours_sauvegarder_tous(liste_cours, COURS_FILENAME);
                mettre_a_jour_treeview();
            }
            g_free(id);
        }
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_WARNING,
                                                   GTK_BUTTONS_OK,
                                                   "Veuillez sélectionner un cours à supprimer.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}


void
on_Ac_buttonquitter_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_main_quit();
}


void
on_Ac_buttonrechercher_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *entry = lookup_widget(GTK_WIDGET(button), "entry6");
    if (!entry) return;
    
    const gchar *recherche = gtk_entry_get_text(GTK_ENTRY(entry));
    
    if (g_strcmp0(recherche, "") == 0) {
        mettre_a_jour_treeview();
        return;
    }
    
    GList *resultats = cours_rechercher_par_nom(liste_cours, recherche);
    GtkWidget *treeview = lookup_widget(GTK_WIDGET(button), "Ac_treeview");
    if (treeview) {
        cours_treeview_filter(treeview, resultats);
    }
    g_list_free(resultats);
}


void
on_Ac_buttonprecedent_clicked(GtkButton *button, gpointer user_data)
{
    
}


void
on_Aj_spinbutton1_value_changed        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{

}


void
on_Aj_buttonprecedent_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
    if (notebook) {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
    }
}


void
on_Aj_buttonEnregistrer_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
    fprintf(stderr, "[DEBUG] on_Aj_buttonEnregistrer_clicked: Début\n");
    
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *entryID = lookup_widget(window, "Aj_entryID");
    GtkWidget *entryNom = lookup_widget(window, "Aj_entryNom");
    
    fprintf(stderr, "[DEBUG] entryID: %p, entryNom: %p\n", (void*)entryID, (void*)entryNom);
    GtkWidget *comboType = lookup_widget(window, "Aj_comboType");
    GtkWidget *comboNiveau = lookup_widget(window, "Aj_comboNiveau");
    GtkWidget *entryEntraineur = lookup_widget(window, "Aj_entryEntraineur");
    GtkWidget *calendar1 = lookup_widget(window, "calendar1");
    GtkWidget *spinbutton2 = lookup_widget(window, "Aj_spinbutton2");
    GtkWidget *spinbutton1 = lookup_widget(window, "Aj_spinbutton1");
    GtkWidget *entryPrix = lookup_widget(window, "Aj_entryPrix");
    GtkWidget *textview1 = lookup_widget(window, "Aj_textview1");
    GtkWidget *radiobutton1 = lookup_widget(window, "Aj_radiobutton1");
    GtkWidget *radiobutton2 = lookup_widget(window, "Aj_radiobutton2");
    GtkWidget *radiobuttonM = lookup_widget(window, "Aj_radiobuttonM");
    
    if (!entryID || !entryNom) return;
    
    const gchar *id = gtk_entry_get_text(GTK_ENTRY(entryID));
    const gchar *nom = gtk_entry_get_text(GTK_ENTRY(entryNom));
    
    fprintf(stderr, "[DEBUG] ID: '%s', Nom: '%s'\n", id ? id : "(NULL)", nom ? nom : "(NULL)");
    
    if (!id || strlen(id) == 0 || !nom || strlen(nom) == 0) {
        fprintf(stderr, "[WARNING] ID ou Nom manquant\n");
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_OK,
                                                   "Veuillez remplir au moins l'ID et le nom du cours.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    /* S'assurer que la liste est initialisée */
    if (!liste_cours) {
        fprintf(stderr, "[DEBUG] Chargement des cours depuis %s\n", COURS_FILENAME);
        liste_cours = cours_charger_tous(COURS_FILENAME);
        fprintf(stderr, "[DEBUG] Nombre de cours chargés: %u\n", liste_cours ? g_list_length(liste_cours) : 0);
    }
    
    /* Vérifier si l'ID existe déjà */
    if (cours_id_existe(liste_cours, id)) {
        fprintf(stderr, "[WARNING] ID '%s' existe déjà\n", id);
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_OK,
                                                   "Un cours avec cet ID existe déjà.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    Cours *cours = cours_new();
    cours->id = g_strdup(id);
    cours->nom = g_strdup(nom);
    
    if (comboType) {
        GtkEntry *entry = GTK_ENTRY(GTK_COMBO(comboType)->entry);
        if (entry) {
            const gchar *type_text = gtk_entry_get_text(entry);
            cours->type = g_strdup(type_text ? type_text : "");
        } else {
            cours->type = g_strdup("");
        }
    } else {
        cours->type = g_strdup("");
    }
    
    if (comboNiveau) {
        gchar *niveau_text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboNiveau));
        cours->niveau = g_strdup(niveau_text ? niveau_text : "");
        if (niveau_text) {
            g_free(niveau_text);
        }
    } else {
        cours->niveau = g_strdup("");
    }
    
    if (entryEntraineur) {
        const gchar *entraineur_text = gtk_entry_get_text(GTK_ENTRY(entryEntraineur));
        cours->entraineur = g_strdup(entraineur_text ? entraineur_text : "");
    } else {
        cours->entraineur = g_strdup("");
    }
    
    if (calendar1) {
        guint year, month, day;
        gtk_calendar_get_date(GTK_CALENDAR(calendar1), &year, &month, &day);
        cours->date = g_strdup_printf("%02u/%02u/%04u", day, month + 1, year);
    } else {
        cours->date = g_strdup("");
    }
    
    if (spinbutton2) {
        cours->duree = (gint)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton2));
    } else {
        cours->duree = 0;
    }
    
    if (spinbutton1) {
        cours->capacite = (gint)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton1));
    } else {
        cours->capacite = 0;
    }
    
    if (entryPrix) {
        const gchar *prix_str = gtk_entry_get_text(GTK_ENTRY(entryPrix));
        if (prix_str && strlen(prix_str) > 0) {
            cours->prix = atof(prix_str);
        } else {
            cours->prix = 0.0;
        }
    } else {
        cours->prix = 0.0;
    }
    
    if (textview1) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_end_iter(buffer, &end);
        gchar *desc_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        cours->description = g_strdup(desc_text ? desc_text : "");
        if (desc_text) {
            g_free(desc_text);
        }
    } else {
        cours->description = g_strdup("");
    }
    
    if (radiobutton1 && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1))) {
        cours->visibilite = g_strdup("Public");
    } else if (radiobutton2 && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2))) {
        cours->visibilite = g_strdup("Privé");
    } else if (radiobuttonM && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobuttonM))) {
        cours->visibilite = g_strdup("Membres seulements");
    } else {
        cours->visibilite = g_strdup("Public");
    }
    
    fprintf(stderr, "[DEBUG] Création du cours terminée, ajout à la liste\n");
    
    /* Ajouter le cours à la liste */
    liste_cours = cours_ajouter(liste_cours, cours);
    fprintf(stderr, "[DEBUG] Cours ajouté, nombre total: %u\n", g_list_length(liste_cours));
    
    /* Sauvegarder dans le fichier */
    fprintf(stderr, "[DEBUG] Sauvegarde dans %s\n", COURS_FILENAME);
    if (!cours_sauvegarder_tous(liste_cours, COURS_FILENAME)) {
        fprintf(stderr, "[ERROR] Échec de la sauvegarde\n");
        /* Erreur de sauvegarde */
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_OK,
                                                   "Erreur lors de la sauvegarde du cours.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        /* Libérer le cours en cas d'erreur */
        cours_free(cours);
        liste_cours = cours_supprimer_par_id(liste_cours, id);
        return;
    }
    
    fprintf(stderr, "[DEBUG] Sauvegarde réussie, mise à jour du treeview\n");
    
    /* Mettre à jour le treeview */
    mettre_a_jour_treeview();
    
    fprintf(stderr, "[DEBUG] Vidage des champs\n");
    /* Vider les champs */
    if (entryID) gtk_entry_set_text(GTK_ENTRY(entryID), "");
    if (entryNom) gtk_entry_set_text(GTK_ENTRY(entryNom), "");
    if (entryEntraineur) gtk_entry_set_text(GTK_ENTRY(entryEntraineur), "");
    if (entryPrix) gtk_entry_set_text(GTK_ENTRY(entryPrix), "");
    if (comboType) {
        GtkEntry *entry = GTK_ENTRY(GTK_COMBO(comboType)->entry);
        if (entry) {
            gtk_entry_set_text(entry, "");
        }
    }
    if (comboNiveau) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(comboNiveau), -1);
    }
    if (spinbutton2) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton2), 1);
    }
    if (spinbutton1) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton1), 1);
    }
    if (calendar1) {
        time_t now = time(NULL);
        struct tm *tm_now = localtime(&now);
        if (tm_now) {
            gtk_calendar_select_month(GTK_CALENDAR(calendar1), 
                                      tm_now->tm_mon, 
                                      tm_now->tm_year + 1900);
            gtk_calendar_select_day(GTK_CALENDAR(calendar1), tm_now->tm_mday);
        }
    }
    if (radiobutton1) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton1), TRUE);
    }
    if (textview1) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
        gtk_text_buffer_set_text(buffer, "", -1);
    }
    
    /* Retour à l'onglet principal */
    GtkWidget *notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
    if (notebook) {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
    }
    
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Cours ajouté avec succès !");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}


void
on_Aj_buttonquitter_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_main_quit();
}


void
on_Aj_spinbutton2_value_changed        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{

}


void
on_calendar1_day_selected              (GtkCalendar     *calendar,
                                        gpointer         user_data)
{

}


void
on_Aj_radiobutton1_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_Aj_radiobutton2_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_Aj_radiobuttonM_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_Mod_buttonprecedent_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
    if (notebook) {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
    }
    if (cours_selectionne_id) {
        g_free(cours_selectionne_id);
        cours_selectionne_id = NULL;
    }
}


void
on_Mod_buttonEnregistrer_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    if (!cours_selectionne_id) return;
    
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *entryNom = lookup_widget(window, "Mod_entryNom");
    GtkWidget *comboType = lookup_widget(window, "Mod_comboType");
    GtkWidget *comboNiveau = lookup_widget(window, "Mod_comboNiveau");
    GtkWidget *entryEntaineur = lookup_widget(window, "Mod_entryEntaineur");
    GtkWidget *calendar2 = lookup_widget(window, "calendar2");
    GtkWidget *spinbutton2 = lookup_widget(window, "Mod_spinbutton2");
    GtkWidget *spinbutton1 = lookup_widget(window, "Mod_spinbutton1");
    GtkWidget *entryPrix = lookup_widget(window, "Mod_entryPrix");
    GtkWidget *textview1 = lookup_widget(window, "Mod_textview1");
    GtkWidget *radiobutton1 = lookup_widget(window, "Mod_radiobutton1");
    GtkWidget *radiobutton2 = lookup_widget(window, "Mod_radiobutton2");
    GtkWidget *radiobuttonEA = lookup_widget(window, "Mod_radiobuttonEA");
    
    Cours *cours = cours_trouver_par_id(liste_cours, cours_selectionne_id);
    if (cours) {
        fprintf(stderr, "[DEBUG] Cours trouvé, mise à jour des champs\n");
            if (entryNom) {
                g_free(cours->nom);
                const gchar *nom_text = gtk_entry_get_text(GTK_ENTRY(entryNom));
                cours->nom = g_strdup(nom_text ? nom_text : "");
            }
            
            if (comboType) {
                GtkEntry *entry = GTK_ENTRY(GTK_COMBO(comboType)->entry);
                if (entry) {
                    g_free(cours->type);
                    const gchar *type_text = gtk_entry_get_text(entry);
                    cours->type = g_strdup(type_text ? type_text : "");
                }
            }
            
            if (comboNiveau) {
                g_free(cours->niveau);
                gchar *niveau_text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboNiveau));
                cours->niveau = g_strdup(niveau_text ? niveau_text : "");
                if (niveau_text) {
                    g_free(niveau_text);
                }
            }
            
            if (entryEntaineur) {
                g_free(cours->entraineur);
                const gchar *entraineur_text = gtk_entry_get_text(GTK_ENTRY(entryEntaineur));
                cours->entraineur = g_strdup(entraineur_text ? entraineur_text : "");
            }
            
            if (calendar2) {
                guint year, month, day;
                gtk_calendar_get_date(GTK_CALENDAR(calendar2), &year, &month, &day);
                g_free(cours->date);
                cours->date = g_strdup_printf("%02u/%02u/%04u", day, month + 1, year);
            }
            
            if (spinbutton2) {
                cours->duree = (gint)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton2));
            }
            
            if (spinbutton1) {
                cours->capacite = (gint)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinbutton1));
            }
            
            if (entryPrix) {
                const gchar *prix_str = gtk_entry_get_text(GTK_ENTRY(entryPrix));
                if (prix_str && strlen(prix_str) > 0) {
                    cours->prix = atof(prix_str);
                } else {
                    cours->prix = 0.0;
                }
            }
            
            if (textview1) {
                GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
                GtkTextIter start, end;
                gtk_text_buffer_get_start_iter(buffer, &start);
                gtk_text_buffer_get_end_iter(buffer, &end);
                g_free(cours->description);
                gchar *desc_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
                cours->description = g_strdup(desc_text ? desc_text : "");
                if (desc_text) {
                    g_free(desc_text);
                }
            }
            
            if (radiobutton1 && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1))) {
                g_free(cours->visibilite);
                cours->visibilite = g_strdup("Public");
            } else if (radiobutton2 && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2))) {
                g_free(cours->visibilite);
                cours->visibilite = g_strdup("Privé");
            } else if (radiobuttonEA && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobuttonEA))) {
                g_free(cours->visibilite);
                cours->visibilite = g_strdup("Membres seulements");
            }
            
            /* Sauvegarder dans le fichier */
            fprintf(stderr, "[DEBUG] Sauvegarde des modifications\n");
            if (!cours_sauvegarder_tous(liste_cours, COURS_FILENAME)) {
                fprintf(stderr, "[ERROR] Échec de la sauvegarde des modifications\n");
                /* Erreur de sauvegarde */
                GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                           GTK_DIALOG_MODAL,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Erreur lors de la sauvegarde des modifications.");
                gtk_dialog_run(GTK_DIALOG(dialog));
                gtk_widget_destroy(dialog);
                return;
            }
            
            fprintf(stderr, "[DEBUG] Sauvegarde réussie, mise à jour du treeview\n");
            mettre_a_jour_treeview();
            
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                       GTK_DIALOG_MODAL,
                                                       GTK_MESSAGE_INFO,
                                                       GTK_BUTTONS_OK,
                                                       "Cours modifié avec succès !");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            
            /* Retour à l'onglet principal */
            GtkWidget *notebook = lookup_widget(GTK_WIDGET(button), "GC_notebook");
            if (notebook) {
                gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
            }
            
            g_free(cours_selectionne_id);
            cours_selectionne_id = NULL;
    } else {
        /* Cours non trouvé */
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_OK,
                                                   "Erreur : Cours non trouvé.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}


void
on_Mod_buttonquitter_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_main_quit();
}


void
on_calendar2_day_selected              (GtkCalendar     *calendar,
                                        gpointer         user_data)
{

}


void
on_Mod_spinbutton2_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{

}


void
on_Mod_spinbutton1_value_changed       (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{

}


void
on_Mod_radiobutton2_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_Mod_radiobuttonTP_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_Mod_radiobuttonEA_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_Mod_buttonChercher_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *entry = lookup_widget(GTK_WIDGET(button), "Mod_entryChercher");
    if (!entry) return;
    
    const gchar *id_recherche = gtk_entry_get_text(GTK_ENTRY(entry));
    
    if (!id_recherche || strlen(id_recherche) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_WARNING,
                                                   GTK_BUTTONS_OK,
                                                   "Veuillez entrer un ID de cours.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    Cours *cours = cours_trouver_par_id(liste_cours, id_recherche);
    if (cours) {
        cours_selectionne_id = g_strdup(cours->id);
        GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
        charger_cours_dans_formulaire(cours, window);
        return;
    }
    
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_WARNING,
                                               GTK_BUTTONS_OK,
                                               "Aucun cours trouvé avec cet ID.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}


void
on_spinbutton1_value_changed           (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{

}





void
on_button15_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *fenetre = gtk_widget_get_toplevel(GTK_WIDGET(button));

    /* Récupération des champs */
    const gchar *nom      = gtk_entry_get_text(GTK_ENTRY(lookup_widget(fenetre, "entry12")));
    const gchar *prenom   = gtk_entry_get_text(GTK_ENTRY(lookup_widget(fenetre, "entry13")));
    const gchar *email    = gtk_entry_get_text(GTK_ENTRY(lookup_widget(fenetre, "entry15")));
    const gchar *adresse  = gtk_entry_get_text(GTK_ENTRY(lookup_widget(fenetre, "entry14")));
    const gchar *cin      = gtk_entry_get_text(GTK_ENTRY(lookup_widget(fenetre, "entry18")));

    gint jour   = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(fenetre, "spinbutton4")));
    gint mois   = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(fenetre, "spinbutton5")));
    gint annee  = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(fenetre, "spinbutton6")));

    /* Affichage dans le terminal */
    g_print("=== Données TIEC ===\n");
    g_print("Nom        : %s\n", nom);
    g_print("Prénom     : %s\n", prenom);
    g_print("Email      : %s\n", email);
    g_print("Adresse    : %s\n", adresse);
    g_print("CIN        : %s\n", cin);
    g_print("Date       : %02d/%02d/%04d\n", jour, mois, annee);
    g_print("====================\n");

    /* ===== SAUVEGARDE DANS UN FICHIER ===== */
    FILE *f = fopen("membre_tiec.txt", "a");
    if (f) {
        fprintf(f, "Nom:%s|Prenom:%s|Email:%s|Adresse:%s|CIN:%s|Date:%02d/%02d/%04d\n",
                nom, prenom, email, adresse, cin, jour, mois, annee);
        fclose(f);
        g_print("[SAUVEGARDE] Données enregistrées dans 'membre_tiec.txt'\n");
    } else {
        g_print("[ERREUR] Impossible d'ouvrir le fichier\n");
    }

    /* ===== MESSAGE POP-UP ===== */
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(fenetre),               // fenêtre parent = TIEC
        GTK_DIALOG_MODAL,                  // bloque les autres fenêtres
        GTK_MESSAGE_INFO,                  // icône "info"
        GTK_BUTTONS_OK,                    // bouton OK
        "Inscription enregistrée avec succès !");
    gtk_dialog_run(GTK_DIALOG(dialog));    // affiche la boîte
    gtk_widget_destroy(dialog);            // ferme la boîte après clic
} 

 // ← MAINTENANT la fonction se termine correctement


void
on_button14_clicked(GtkButton *button, gpointer user_data)
{
    g_print("[DEBUG] Bouton précédent TIEC cliqué\n");
    
    // TIEC est dans une fenêtre séparée, fermer cette fenêtre
    GtkWidget *fenetre_tiec = gtk_widget_get_toplevel(GTK_WIDGET(button));
    
    if (fenetre_tiec && GTK_IS_WINDOW(fenetre_tiec)) {
        g_print("[INFO] Fermeture de la fenêtre TIEC\n");
        // Détruire la fenêtre TIEC pour revenir à Fitverse
        gtk_widget_destroy(fenetre_tiec);
    } else {
        g_print("[ERREUR] Impossible de trouver la fenêtre TIEC\n");
    }
}

void
on_button16_clicked(GtkButton *button, gpointer user_data)
{
    g_print("[DEBUG] Bouton 16 (précédent) cliqué\n");
    
    GtkWidget *fenetre_tiec = gtk_widget_get_toplevel(GTK_WIDGET(button));
    
    if (fenetre_tiec && GTK_IS_WINDOW(fenetre_tiec)) {
        g_print("[INFO] Fermeture de la fenêtre TIEC\n");
        gtk_widget_destroy(fenetre_tiec);
    }
}


void
on_spinbutton4_value_changed(GtkSpinButton *spinbutton, gpointer user_data)
{

}


void
on_spinbutton5_value_changed(GtkSpinButton *spinbutton, gpointer user_data)
{

}


void
on_spinbutton6_value_changed(GtkSpinButton *spinbutton, gpointer user_data)
{

}


void
on_radiobutton3_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{

}


void
on_radiobutton4_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{

}


void
on_radiobutton5_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{

}

void
on_TIEC_window_show(GtkWidget *widget, gpointer user_data)
{
    g_print("[DEBUG] Fenêtre TIEC affichée - décocher les radiobuttons\n");
    
    GtkWidget *radiobutton3 = lookup_widget(widget, "radiobutton3");
    GtkWidget *radiobutton4 = lookup_widget(widget, "radiobutton4");
    GtkWidget *radiobutton5 = lookup_widget(widget, "radiobutton5");
    
    // Décocher tous les radiobuttons
    if (radiobutton3) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton3), FALSE);
    }
    if (radiobutton4) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton4), FALSE);
    }
    if (radiobutton5) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton5), FALSE);
    }
    
    g_print("[DEBUG] Tous les radiobuttons décochés\n");
}

void on_button_administrateur_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *fenetre_gcs;
    GtkWidget *fenetre_login;
    
    /* Fermer la fenêtre GCS */
    fenetre_gcs = lookup_widget(GTK_WIDGET(button), "GCS");
    if (fenetre_gcs)
        gtk_widget_destroy(fenetre_gcs);
    
    /* Ouvrir la fenêtre Administrateur */
    fenetre_login = create_Administrateur();   // nom exact du .glade
    gtk_widget_show(fenetre_login);
}
void
on_button_membre_clicked(GtkButton *button, gpointer user_data)
{
    g_print("[DEBUG] Bouton Membre cliqué\n");
    
    // Ouvrir directement FitVerse pour les membres (sans connexion)
    GtkWidget *fenetre_fitverse = create_FitVerse();
    
    if (fenetre_fitverse) {
        gtk_widget_show(fenetre_fitverse);
        g_print("[INFO] Fenêtre FitVerse ouverte (mode Membre)\n");
        
        // Fermer la fenêtre de choix
        GtkWidget *fenetre_choix = gtk_widget_get_toplevel(GTK_WIDGET(button));
        gtk_widget_destroy(fenetre_choix);
    } else {
        g_print("[ERREUR] Impossible de créer FitVerse\n");
    }
}

void
on_button_se_connecter_clicked(GtkButton *button, gpointer user_data)
{
    g_print("[DEBUG] Bouton Se connecter cliqué\n");
    
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    
    // Récupérer les champs (adaptez les noms selon votre Glade)
    GtkWidget *entry_identifiant = lookup_widget(window, "entry_identifiant");
    GtkWidget *entry_mot_de_passe = lookup_widget(window, "entry_mot_de_passe");
    
    if (!entry_identifiant || !entry_mot_de_passe) {
        g_print("[ERREUR] Champs non trouvés\n");
        // Afficher un message d'erreur
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Erreur : Champs non trouvés dans l'interface !");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    const gchar *identifiant = gtk_entry_get_text(GTK_ENTRY(entry_identifiant));
    const gchar *mot_de_passe = gtk_entry_get_text(GTK_ENTRY(entry_mot_de_passe));
    
    g_print("[DEBUG] Tentative de connexion - Identifiant: %s\n", identifiant);
    
    // Vérifier les identifiants
    if (strcmp(identifiant, "admin") == 0 && strcmp(mot_de_passe, "admin123") == 0) {
        g_print("[INFO] ✓ Connexion admin réussie\n");
        
        // Ouvrir FitVerse
        GtkWidget *fenetre_fitverse = create_FitVerse();
        
        if (fenetre_fitverse) {
            gtk_widget_show(fenetre_fitverse);
            g_print("[INFO] Fenêtre FitVerse ouverte (mode Admin)\n");
            
            // Fermer la fenêtre de connexion
            gtk_widget_destroy(window);
        }
        
    } else {
        g_print("[WARNING] ✗ Identifiants incorrects\n");
        
        // Afficher un message d'erreur
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Identifiant ou mot de passe incorrect !\n\nIdentifiants par défaut :\nIdentifiant : admin\nMot de passe : admin123");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        // Vider le champ mot de passe pour sécurité
        gtk_entry_set_text(GTK_ENTRY(entry_mot_de_passe), "");
    }
}

void
on_btn_quitter_clicked(GtkButton *button, gpointer user_data)
{
    g_print("[DEBUG] Bouton Quitter cliqué\n");
    
    // Quitter l'application
    gtk_main_quit();
}

void
on_btn_quitter2_clicked(GtkButton *button, gpointer user_data)
{
    g_print("[DEBUG] Bouton Quitter 2 cliqué\n");
    
    // Quitter l'application
    gtk_main_quit();
}
void
on_btn_conn1_clicked(GtkButton *button, gpointer user_data)
{
    
}
