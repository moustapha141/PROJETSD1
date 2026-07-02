/* ============================================================================
 * benchmark.c
 * ----------------------------------------------------------------------------
 * Outil de benchmarking - Projet LMI ASD 2026
 *
 * Compare les performances (temps d'execution) des 3 structures de donnees
 * implementees pour la gestion du catalogue Produit :
 *      1. Tableau statique
 *      2. Tableau dynamique de pointeurs (realloc)
 *      3. Liste doublement chainee
 *
 * Operations mesurees : insertion, recherche par cle, suppression,
 * tri par insertion (existant) et un 2e algorithme de tri (tri rapide pour
 * les tableaux, tri fusion pour la liste chainee).
 *
 * Fichiers generes (dans le repertoire courant) :
 *      - resultats_operations.csv  : insertion / recherche / suppression
 *      - resultats_tri.csv         : tri par insertion vs 2e algorithme
 *      - donnees_exemple.csv       : jeu de donnees d'exemple (1000 produits)
 *
 * NOTE IMPORTANTE
 * ----------------------------------------------------------------------------
 * A la date de redaction, la suppression sur tableau statique n'existe pas
 * encore dans PROJETSD1.c (ecart identifie lors de l'audit du projet).
 * Une implementation minimale (supprimerStatique) est fournie ICI, dans ce
 * fichier de benchmark independant, uniquement a des fins de mesure et de
 * comparaison equitable entre les 3 structures. Elle devra etre recopiee
 * dans le programme principal (PROJETSD1.c) pour repondre pleinement au
 * cahier des charges.
 *
 * Compilation : voir le Makefile fourni (cible "benchmark").
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* ----------------------------------------------------------------------
 * 1. STRUCTURES DE DONNEES (identiques a PROJETSD1.c)
 * ---------------------------------------------------------------------- */
typedef struct Produit {
    int idProduit;
    char nom[50];
    char categorie[30];
    float prix;
    int stock;
    char dateAjout[11];
} Produit;

typedef struct Noeud {
    Produit data;
    struct Noeud *suiv;
    struct Noeud *prec;
} Noeud;

/* ----------------------------------------------------------------------
 * 2. PARAMETRES DU BENCHMARK
 * ---------------------------------------------------------------------- */
#define NB_TAILLES 8
static const int TAILLES[NB_TAILLES] = {100, 500, 1000, 2500, 5000, 10000, 20000, 50000};

#define NB_TAILLES_TRI 6
static const int TAILLES_TRI[NB_TAILLES_TRI] = {100, 500, 1000, 2000, 5000, 8000};

#define NB_OPS_MESUREES 200   /* nb de recherches/suppressions moyennees par mesure   */
#define NB_REPETITIONS   3    /* nb de repetitions de chaque mesure (moyenne retenue) */

static const char *CATEGORIES[] = {
    "Alimentaire", "Electronique", "Vetements", "Mobilier",
    "Papeterie", "Hygiene", "Jouets", "Informatique"
};
#define NB_CATEGORIES 8

/* ----------------------------------------------------------------------
 * 3. UTILITAIRES
 * ---------------------------------------------------------------------- */
static double ms_ecoule(clock_t debut, clock_t fin) {
    return (double)(fin - debut) * 1000.0 / CLOCKS_PER_SEC;
}

static void genererProduit(Produit *p, int id) {
    p->idProduit = id;
    snprintf(p->nom, sizeof(p->nom), "Article_%d", id);
    snprintf(p->categorie, sizeof(p->categorie), "%s", CATEGORIES[id % NB_CATEGORIES]);
    p->prix = 500.0f + (float)(rand() % 500000) / 100.0f;   /* 500.00 a 5500.00 FCFA */
    p->stock = rand() % 500;
    snprintf(p->dateAjout, sizeof(p->dateAjout), "2026-%02d-%02d", 1 + (id % 12), 1 + (id % 28));
}

/* melange Fisher-Yates : sert a tirer des IDs existants au hasard
 * pour les mesures de recherche / suppression */
static void melanger(int *tab, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = tab[i]; tab[i] = tab[j]; tab[j] = tmp;
    }
}

/* ----------------------------------------------------------------------
 * 4. TABLEAU STATIQUE
 * ---------------------------------------------------------------------- */
static int insererStatique(Produit *tab, int taille, int capacite, Produit p) {
    if (taille >= capacite) return taille;
    tab[taille] = p;
    return taille + 1;
}

static Produit* rechercherStatique(Produit *tab, int taille, int id) {
    for (int i = 0; i < taille; i++)
        if (tab[i].idProduit == id) return &tab[i];
    return NULL;
}

/* Suppression par decalage - voir NOTE IMPORTANTE en tete de fichier */
static int supprimerStatique(Produit *tab, int taille, int id) {
    int idx = -1;
    for (int i = 0; i < taille; i++) {
        if (tab[i].idProduit == id) { idx = i; break; }
    }
    if (idx == -1) return taille;
    for (int i = idx; i < taille - 1; i++) tab[i] = tab[i + 1];
    return taille - 1;
}

static void triInsertionStatique(Produit *tab, int taille) {
    for (int i = 1; i < taille; i++) {
        Produit cle = tab[i];
        int j = i - 1;
        while (j >= 0 && tab[j].prix > cle.prix) { tab[j + 1] = tab[j]; j--; }
        tab[j + 1] = cle;
    }
}

static int partitionQS(Produit *tab, int g, int d) {
    float pivot = tab[d].prix;
    int i = g - 1;
    for (int j = g; j < d; j++) {
        if (tab[j].prix <= pivot) {
            i++;
            Produit t = tab[i]; tab[i] = tab[j]; tab[j] = t;
        }
    }
    Produit t = tab[i + 1]; tab[i + 1] = tab[d]; tab[d] = t;
    return i + 1;
}

/* 2e algorithme de tri pour les tableaux : tri rapide (quicksort) */
static void triRapideStatique(Produit *tab, int g, int d) {
    if (g < d) {
        int p = partitionQS(tab, g, d);
        triRapideStatique(tab, g, p - 1);
        triRapideStatique(tab, p + 1, d);
    }
}

/* ----------------------------------------------------------------------
 * 5. TABLEAU DYNAMIQUE DE POINTEURS
 * ---------------------------------------------------------------------- */
typedef struct {
    Produit **data;
    int taille;
    int capacite;
} TableauDyn;

static void initDyn(TableauDyn *t, int capaciteInitiale) {
    t->data = malloc(sizeof(Produit*) * (size_t)capaciteInitiale);
    t->taille = 0;
    t->capacite = capaciteInitiale;
}

static void insererDyn(TableauDyn *t, Produit p) {
    if (t->taille >= t->capacite) {
        t->capacite *= 2;
        t->data = realloc(t->data, sizeof(Produit*) * (size_t)t->capacite);
    }
    t->data[t->taille] = malloc(sizeof(Produit));
    *(t->data[t->taille]) = p;
    t->taille++;
}

static Produit* rechercherDyn(TableauDyn *t, int id) {
    for (int i = 0; i < t->taille; i++)
        if (t->data[i]->idProduit == id) return t->data[i];
    return NULL;
}

static void supprimerDyn(TableauDyn *t, int id) {
    int idx = -1;
    for (int i = 0; i < t->taille; i++)
        if (t->data[i]->idProduit == id) { idx = i; break; }
    if (idx == -1) return;
    free(t->data[idx]);
    for (int i = idx; i < t->taille - 1; i++) t->data[i] = t->data[i + 1];
    t->taille--;
}

static void libererDyn(TableauDyn *t) {
    for (int i = 0; i < t->taille; i++) free(t->data[i]);
    free(t->data);
    t->data = NULL; t->taille = 0; t->capacite = 0;
}

static void triInsertionDyn(TableauDyn *t) {
    for (int i = 1; i < t->taille; i++) {
        Produit *cle = t->data[i];
        int j = i - 1;
        while (j >= 0 && t->data[j]->prix > cle->prix) { t->data[j + 1] = t->data[j]; j--; }
        t->data[j + 1] = cle;
    }
}

static int partitionQSDyn(Produit **tab, int g, int d) {
    float pivot = tab[d]->prix;
    int i = g - 1;
    for (int j = g; j < d; j++) {
        if (tab[j]->prix <= pivot) {
            i++;
            Produit *tmp = tab[i]; tab[i] = tab[j]; tab[j] = tmp;
        }
    }
    Produit *tmp = tab[i + 1]; tab[i + 1] = tab[d]; tab[d] = tmp;
    return i + 1;
}

/* 2e algorithme de tri pour les tableaux : tri rapide (quicksort) */
static void triRapideDyn(Produit **tab, int g, int d) {
    if (g < d) {
        int p = partitionQSDyn(tab, g, d);
        triRapideDyn(tab, g, p - 1);
        triRapideDyn(tab, p + 1, d);
    }
}

/* ----------------------------------------------------------------------
 * 6. LISTE DOUBLEMENT CHAINEE
 * ---------------------------------------------------------------------- */
static Noeud* insererTeteListe(Noeud *tete, Produit p) {
    Noeud *n = malloc(sizeof(Noeud));
    n->data = p;
    n->suiv = tete;
    n->prec = NULL;
    if (tete) tete->prec = n;
    return n;
}

static Produit* rechercherListe(Noeud *tete, int id) {
    for (Noeud *c = tete; c; c = c->suiv)
        if (c->data.idProduit == id) return &c->data;
    return NULL;
}

static Noeud* supprimerListe(Noeud *tete, int id) {
    Noeud *c = tete;
    while (c && c->data.idProduit != id) c = c->suiv;
    if (!c) return tete;
    if (c->prec) c->prec->suiv = c->suiv; else tete = c->suiv;
    if (c->suiv) c->suiv->prec = c->prec;
    free(c);
    return tete;
}

static void libererListe(Noeud *tete) {
    while (tete) { Noeud *s = tete->suiv; free(tete); tete = s; }
}

static void triInsertionListe(Noeud **tete) {
    if (!*tete || !(*tete)->suiv) return;
    Noeud *trie = *tete;
    Noeud *courant = (*tete)->suiv;
    trie->suiv = NULL; trie->prec = NULL;
    while (courant) {
        Noeud *suivant = courant->suiv;
        if (courant->data.prix <= trie->data.prix) {
            courant->suiv = trie; courant->prec = NULL;
            trie->prec = courant; trie = courant;
        } else {
            Noeud *pos = trie;
            while (pos->suiv && pos->suiv->data.prix < courant->data.prix) pos = pos->suiv;
            courant->suiv = pos->suiv; courant->prec = pos;
            if (pos->suiv) pos->suiv->prec = courant;
            pos->suiv = courant;
        }
        courant = suivant;
    }
    *tete = trie;
}

/* 2e algorithme de tri pour la liste chainee : tri fusion (merge sort) */
static Noeud* fusionner(Noeud *a, Noeud *b) {
    if (!a) return b;
    if (!b) return a;
    Noeud *res;
    if (a->data.prix <= b->data.prix) { res = a; res->suiv = fusionner(a->suiv, b); }
    else { res = b; res->suiv = fusionner(a, b->suiv); }
    if (res->suiv) res->suiv->prec = res;
    res->prec = NULL;
    return res;
}

static Noeud* milieu(Noeud *tete) {
    Noeud *lent = tete, *rapide = tete->suiv;
    while (rapide && rapide->suiv) { lent = lent->suiv; rapide = rapide->suiv->suiv; }
    return lent;
}

static Noeud* triFusionListe(Noeud *tete) {
    if (!tete || !tete->suiv) return tete;
    Noeud *mid = milieu(tete);
    Noeud *droite = mid->suiv;
    mid->suiv = NULL;
    if (droite) droite->prec = NULL;
    Noeud *g = triFusionListe(tete);
    Noeud *d = triFusionListe(droite);
    return fusionner(g, d);
}

/* ----------------------------------------------------------------------
 * 7. PROGRAMME PRINCIPAL DU BENCHMARK
 * ---------------------------------------------------------------------- */
int main(void) {
    srand(42); /* graine fixe pour la reproductibilite des mesures */

    FILE *fops = fopen("resultats_operations.csv", "w");
    FILE *ftri = fopen("resultats_tri.csv", "w");
    if (!fops || !ftri) { perror("Erreur ouverture des fichiers de resultats"); return 1; }

    fprintf(fops,
        "N;insertion_statique_ms;insertion_dynamique_ms;insertion_liste_ms;"
        "recherche_statique_ms;recherche_dynamique_ms;recherche_liste_ms;"
        "suppression_statique_ms;suppression_dynamique_ms;suppression_liste_ms\n");
    fprintf(ftri,
        "N;tri_insertion_statique_ms;tri_rapide_statique_ms;"
        "tri_insertion_dynamique_ms;tri_rapide_dynamique_ms;"
        "tri_insertion_liste_ms;tri_fusion_liste_ms\n");

    printf("=== Benchmark LMI ASD 2026 ===\n");
    printf("Tableau statique  vs  Tableau dynamique de pointeurs  vs  Liste doublement chainee\n\n");

    /* ---- 1. Insertion / Recherche / Suppression ---- */
    for (int t = 0; t < NB_TAILLES; t++) {
        int N = TAILLES[t];
        printf("[operations] N = %d ...\n", N);

        double t_ins_s = 0, t_ins_d = 0, t_ins_l = 0;
        double t_rech_s = 0, t_rech_d = 0, t_rech_l = 0;
        double t_supp_s = 0, t_supp_d = 0, t_supp_l = 0;

        for (int rep = 0; rep < NB_REPETITIONS; rep++) {
            Produit *jeu = malloc(sizeof(Produit) * (size_t)N);
            for (int i = 0; i < N; i++) genererProduit(&jeu[i], i + 1);

            /* -- Insertion -- */
            Produit *tabStat = malloc(sizeof(Produit) * (size_t)N);
            int tailleStat = 0;
            clock_t d0 = clock();
            for (int i = 0; i < N; i++) tailleStat = insererStatique(tabStat, tailleStat, N, jeu[i]);
            clock_t d1 = clock();
            t_ins_s += ms_ecoule(d0, d1);

            TableauDyn tabDyn; initDyn(&tabDyn, 10);
            d0 = clock();
            for (int i = 0; i < N; i++) insererDyn(&tabDyn, jeu[i]);
            d1 = clock();
            t_ins_d += ms_ecoule(d0, d1);

            Noeud *liste = NULL;
            d0 = clock();
            for (int i = 0; i < N; i++) liste = insererTeteListe(liste, jeu[i]);
            d1 = clock();
            t_ins_l += ms_ecoule(d0, d1);

            /* -- Echantillon d'IDs existants tires au hasard -- */
            int m = (N < NB_OPS_MESUREES) ? N : NB_OPS_MESUREES;
            int *ids = malloc(sizeof(int) * (size_t)N);
            for (int i = 0; i < N; i++) ids[i] = i + 1;
            melanger(ids, N);

            /* -- Recherche (moyenne par operation) -- */
            d0 = clock();
            for (int i = 0; i < m; i++) rechercherStatique(tabStat, tailleStat, ids[i]);
            d1 = clock();
            t_rech_s += ms_ecoule(d0, d1) / m;

            d0 = clock();
            for (int i = 0; i < m; i++) rechercherDyn(&tabDyn, ids[i]);
            d1 = clock();
            t_rech_d += ms_ecoule(d0, d1) / m;

            d0 = clock();
            for (int i = 0; i < m; i++) rechercherListe(liste, ids[i]);
            d1 = clock();
            t_rech_l += ms_ecoule(d0, d1) / m;

            /* -- Suppression (moyenne par operation) -- */
            d0 = clock();
            for (int i = 0; i < m; i++) tailleStat = supprimerStatique(tabStat, tailleStat, ids[i]);
            d1 = clock();
            t_supp_s += ms_ecoule(d0, d1) / m;

            d0 = clock();
            for (int i = 0; i < m; i++) supprimerDyn(&tabDyn, ids[i]);
            d1 = clock();
            t_supp_d += ms_ecoule(d0, d1) / m;

            d0 = clock();
            for (int i = 0; i < m; i++) liste = supprimerListe(liste, ids[i]);
            d1 = clock();
            t_supp_l += ms_ecoule(d0, d1) / m;

            free(ids);
            free(jeu);
            free(tabStat);
            libererDyn(&tabDyn);
            libererListe(liste);
        }

        fprintf(fops, "%d;%.4f;%.4f;%.4f;%.6f;%.6f;%.6f;%.6f;%.6f;%.6f\n",
                N,
                t_ins_s / NB_REPETITIONS, t_ins_d / NB_REPETITIONS, t_ins_l / NB_REPETITIONS,
                t_rech_s / NB_REPETITIONS, t_rech_d / NB_REPETITIONS, t_rech_l / NB_REPETITIONS,
                t_supp_s / NB_REPETITIONS, t_supp_d / NB_REPETITIONS, t_supp_l / NB_REPETITIONS);
    }
    fclose(fops);

    /* ---- 2. Tri : insertion (existant) vs 2e algorithme ---- */
    for (int t = 0; t < NB_TAILLES_TRI; t++) {
        int N = TAILLES_TRI[t];
        printf("[tri]        N = %d ...\n", N);

        double ti_s = 0, tr_s = 0, ti_d = 0, tr_d = 0, ti_l = 0, tf_l = 0;

        for (int rep = 0; rep < NB_REPETITIONS; rep++) {
            Produit *jeu = malloc(sizeof(Produit) * (size_t)N);
            for (int i = 0; i < N; i++) genererProduit(&jeu[i], i + 1);

            /* Tableau statique : insertion vs rapide */
            Produit *a1 = malloc(sizeof(Produit) * (size_t)N);
            memcpy(a1, jeu, sizeof(Produit) * (size_t)N);
            clock_t d0 = clock(); triInsertionStatique(a1, N); clock_t d1 = clock();
            ti_s += ms_ecoule(d0, d1);

            Produit *a2 = malloc(sizeof(Produit) * (size_t)N);
            memcpy(a2, jeu, sizeof(Produit) * (size_t)N);
            d0 = clock(); triRapideStatique(a2, 0, N - 1); d1 = clock();
            tr_s += ms_ecoule(d0, d1);

            /* Tableau dynamique : insertion vs rapide */
            TableauDyn td1; initDyn(&td1, N);
            for (int i = 0; i < N; i++) insererDyn(&td1, jeu[i]);
            d0 = clock(); triInsertionDyn(&td1); d1 = clock();
            ti_d += ms_ecoule(d0, d1);

            TableauDyn td2; initDyn(&td2, N);
            for (int i = 0; i < N; i++) insererDyn(&td2, jeu[i]);
            d0 = clock(); triRapideDyn(td2.data, 0, N - 1); d1 = clock();
            tr_d += ms_ecoule(d0, d1);

            /* Liste chainee : insertion vs fusion */
            Noeud *l1 = NULL;
            for (int i = 0; i < N; i++) l1 = insererTeteListe(l1, jeu[i]);
            d0 = clock(); triInsertionListe(&l1); d1 = clock();
            ti_l += ms_ecoule(d0, d1);

            Noeud *l2 = NULL;
            for (int i = 0; i < N; i++) l2 = insererTeteListe(l2, jeu[i]);
            d0 = clock(); l2 = triFusionListe(l2); d1 = clock();
            tf_l += ms_ecoule(d0, d1);

            free(jeu); free(a1); free(a2);
            libererDyn(&td1); libererDyn(&td2);
            libererListe(l1); libererListe(l2);
        }

        fprintf(ftri, "%d;%.4f;%.4f;%.4f;%.4f;%.4f;%.4f\n",
                N, ti_s / NB_REPETITIONS, tr_s / NB_REPETITIONS,
                ti_d / NB_REPETITIONS, tr_d / NB_REPETITIONS,
                ti_l / NB_REPETITIONS, tf_l / NB_REPETITIONS);
    }
    fclose(ftri);

    /* ---- 3. Jeu de donnees d'exemple exportable ---- */
    FILE *fdata = fopen("donnees_exemple.csv", "w");
    if (fdata) {
        fprintf(fdata, "idProduit;nom;categorie;prix;stock;dateAjout\n");
        for (int i = 1; i <= 1000; i++) {
            Produit p; genererProduit(&p, i);
            fprintf(fdata, "%d;%s;%s;%.2f;%d;%s\n",
                    p.idProduit, p.nom, p.categorie, p.prix, p.stock, p.dateAjout);
        }
        fclose(fdata);
    }

    printf("\nBenchmark termine. Fichiers generes :\n");
    printf("  - resultats_operations.csv (insertion / recherche / suppression)\n");
    printf("  - resultats_tri.csv        (tri par insertion vs 2e algorithme)\n");
    printf("  - donnees_exemple.csv      (1000 produits generes)\n");

    return 0;
}
