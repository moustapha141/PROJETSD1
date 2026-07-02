# Projet LMI ASD 2026 — Gestion de catalogue produits

Système de gestion de catalogue (produits, clients, commandes) implémenté
selon 3 structures de données distinctes, avec un outil de benchmark associé
pour comparer leurs performances.

## Structure du dépôt

```
.
├── PROJETSD1.c              # Programme principal (menu interactif)
├── benchmark.c               # Outil de mesure de performance (sans saisie utilisateur)
├── Makefile                  # Compilation des deux exécutables
├── README.md                 # Ce fichier
├── complexite.md              # Analyse de complexité théorique détaillée
├── resultats_operations.csv  # Résultats : insertion / recherche / suppression
├── resultats_tri.csv         # Résultats : tri par insertion vs 2e algorithme
└── donnees_exemple.csv       # Jeu de données généré (1000 produits)
```

## 1. Le programme principal — `PROJETSD1.c`

Menu interactif permettant de gérer un catalogue de produits selon 3
structures en parallèle :

1. **Tableau statique** (`table_produits[MAX_STATIQUE]`)
2. **Tableau dynamique de pointeurs** (réallocation par doublement)
3. **Liste doublement chaînée**

Pour chaque structure : insertion, recherche par ID, recherche multicritère
(catégorie + intervalle de prix), suppression (tableau dynamique et liste
uniquement — voir "État d'avancement" ci-dessous), mise à jour, tri par
insertion, statistiques (min/max/moyenne/médiane/écart-type) et persistance
binaire (`.dat`).

Gère également des **clients** et des **commandes** rattachées au catalogue
(tableau statique uniquement).

### Compilation et exécution

```bash
make projet
./projet
```

ou directement :

```bash
make run
```

## 2. L'outil de benchmark — `benchmark.c`

Programme **autonome et non interactif** (aucune saisie clavier requise) qui
réimplémente les opérations de base des 3 structures dans le seul but de les
chronométrer sur des jeux de données générés aléatoirement (graine fixe
`srand(42)` pour la reproductibilité des mesures).

### Ce qu'il mesure

- **Insertion**, **recherche par clé** et **suppression**, pour des tailles
  `N ∈ {100, 500, 1000, 2500, 5000, 10000, 20000, 50000}` → `resultats_operations.csv`
- **Tri par insertion** (existant dans le projet) comparé à un **2ᵉ
  algorithme** (tri rapide pour les tableaux, tri fusion pour la liste
  chaînée), pour `N ∈ {100, 500, 1000, 2000, 5000, 8000}` → `resultats_tri.csv`
- Chaque mesure est répétée 3 fois et moyennée ; les temps de recherche et de
  suppression sont eux-mêmes moyennés sur un échantillon de 200 opérations
  (ou `N` si `N < 200`) pour lisser le bruit de mesure.

### Compilation et exécution

```bash
make benchmark
./benchmark
```

ou directement :

```bash
make run-benchmark
```

Le programme affiche sa progression sur la sortie standard puis génère les
trois fichiers CSV (séparateur `;`, compatible Excel/LibreOffice/pgfplots) :

| Fichier | Contenu |
|---|---|
| `resultats_operations.csv` | Temps (ms) d'insertion / recherche / suppression pour chaque `N` et chaque structure |
| `resultats_tri.csv` | Temps (ms) de tri par insertion vs 2ᵉ algorithme pour chaque `N` et chaque structure |
| `donnees_exemple.csv` | 1000 produits générés (id, nom, catégorie, prix, stock, date), réutilisables comme jeu de test pour `PROJETSD1.c` |

### Utilisation dans le rapport LaTeX

Ces CSV sont directement exploitables avec `pgfplots` (`\addplot table
[x=N, y=insertion_statique_ms, col sep=semicolon] {resultats_operations.csv};`)
pour générer les courbes du chapitre 5, ou importables tels quels dans un
tableur pour produire les tableaux de résultats.

> **Important** : `benchmark.c` est un fichier **indépendant** de
> `PROJETSD1.c`. Il réimplémente ses propres versions (allégées, sans
> `printf`/`scanf` interactifs) des fonctions d'insertion, recherche,
> suppression et tri, en respectant exactement les mêmes signatures de
> structures (`Produit`, `Noeud`) et les mêmes algorithmes. Il ne modifie ni
> ne dépend du fichier principal.

## 3. Makefile

```bash
make            # compile projet + benchmark
make projet     # compile uniquement le programme principal
make benchmark  # compile uniquement l'outil de benchmark
make run        # compile et lance ./projet
make run-benchmark  # compile et lance ./benchmark
make clean      # supprime les binaires, fichiers .dat et .o
```

## 4. État d'avancement du projet (rappel de l'audit)

Ce qui est **fait** dans `PROJETSD1.c` : insertion, recherche par clé, mise à
jour, tri par insertion et statistiques sur les 3 structures ; recherche
multicritère et suppression sur tableau dynamique et liste chaînée ;
persistance binaire complète.

Ce qui **manque encore** :

| Élément | Concerné |
|---|---|
| Suppression sur tableau statique | fonction absente de `PROJETSD1.c` (une version de démonstration existe dans `benchmark.c` à des fins de mesure uniquement) |
| Recherche par préfixe (champ chaîne) | absente sur les 3 structures |
| 2ᵉ algorithme de tri dans le programme principal | seul le tri par insertion est présent ; `benchmark.c` démontre un tri rapide/fusion mais ne modifie pas `PROJETSD1.c` |
| Structures 4 et 5 du cahier des charges (ABR / Tas binaire) | à remplacer selon les consignes du professeur |

Ces points restent à intégrer directement dans `PROJETSD1.c` (et documentés
dans le rapport) pour répondre pleinement au cahier des charges.

## 5. Compilateur et dépendances

- `gcc` avec `-std=c11`
- Bibliothèque mathématique standard (`-lm`) pour `sqrtf`
- Aucune dépendance externe
