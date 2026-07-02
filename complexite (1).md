# Analyse de complexité — Projet LMI ASD 2026

Ce document synthétise la complexité théorique (notation de Landau) de chaque
opération pour les 3 structures de données implémentées dans `PROJETSD1.c`,
et sert de référence pour le chapitre 4 du rapport ainsi que pour
l'interprétation des mesures produites par `benchmark.c`.

Notation : `n` = nombre d'éléments actuellement stockés dans la structure.

## 1. Tableau statique (`Produit table_produits[MAX_STATIQUE]`)

| Opération | Complexité | Justification |
|---|---|---|
| Insertion (en fin, place libre) | **Θ(1)** amorti / **O(n)** dans le pire cas | `saisirEtAjouterProduit` parcourt le tableau jusqu'à la première case libre (`idProduit == 0`) : O(n) pour *trouver* la place, mais l'écriture elle-même est en O(1). |
| Recherche par clé (`rechercherProduit`) | **Θ(n)** | Parcours séquentiel, aucun tri ni index n'est maintenu. |
| Recherche multicritère (intervalle) | **Θ(n)** | Même principe : chaque élément est testé une fois. |
| Recherche par préfixe | **Θ(n × k)** | `k` = longueur moyenne de la chaîne comparée (chaque comparaison de préfixe coûte jusqu'à `k`). |
| Suppression (par décalage — voir note) | **Θ(n)** | Recherche en O(n) + décalage de tous les éléments suivants en O(n). |
| Mise à jour (`mettreAJourProduitStatique`) | **Θ(n)** | Dominée par la recherche préalable ; la modification en place est O(1). |
| Tri par insertion | **O(n²)**, **Ω(n)** si déjà trié | Deux boucles imbriquées ; best case linéaire sur données déjà triées. |
| Tri rapide (2ᵉ algorithme, `benchmark.c`) | **O(n log n)** en moyenne, **O(n²)** au pire (pivot défavorable) | Partitionnement de Lomuto ; pire cas rare avec des données aléatoires. |
| Statistiques (min/max/moyenne) | **Θ(n)** | Un seul parcours. |
| Médiane (`quickselect`) | **Θ(n)** en moyenne, **O(n²)** au pire | Sélection par partitionnement, sans tri complet. |
| Écart-type | **Θ(n)** | Nécessite la moyenne (Θ(n)) puis un second parcours (Θ(n)). |
| Persistance binaire (`fwrite`/`fread`) | **Θ(n)** | Écriture/lecture séquentielle de blocs contigus. |
| **Espace mémoire** | **Θ(MAX_STATIQUE)** fixe | Allocation statique à la compilation, indépendante du nombre réel d'éléments — gaspillage potentiel si `n ≪ MAX_STATIQUE`. |

## 2. Tableau dynamique de pointeurs (`Produit **tab`)

| Opération | Complexité | Justification |
|---|---|---|
| Insertion (`insererProduitDyn`) | **Θ(1)** amorti | Ajout en fin de tableau ; le doublement de capacité (`realloc ×2`) ne coûte O(n) qu'occasionnellement, ce qui donne un coût **amorti** constant sur une série d'insertions. |
| Recherche par clé | **Θ(n)** | Parcours séquentiel des pointeurs. |
| Recherche multicritère | **Θ(n)** | Idem, un test par élément. |
| Suppression (`supprimerProduitDyn`) | **Θ(n)** | Recherche O(n) + décalage des pointeurs restants O(n) + `free` O(1). |
| Mise à jour | **Θ(n)** | Dominée par la recherche. |
| Tri par insertion | **O(n²)** | Tri de pointeurs (échange de pointeurs, pas de structures entières → constante plus faible que le tableau statique). |
| Tri rapide (2ᵉ algorithme) | **O(n log n)** en moyenne | Idem tableau statique, mais échanges de pointeurs (8 octets) au lieu de structures complètes → plus rapide en pratique. |
| Statistiques / médiane / écart-type | **Θ(n)** / **Θ(n)** moyen / **Θ(n)** | Mêmes principes que le tableau statique. |
| Persistance binaire | **Θ(n)** | Une écriture par structure pointée (`fwrite` unitaire dans une boucle). |
| **Espace mémoire** | **Θ(n)** utile + surcoût pointeurs | `n × sizeof(Produit*)` pour le tableau d'indirection + `n × sizeof(Produit)` pour les blocs alloués individuellement ; capacité peut dépasser `n` (facteur de charge du doublement, ≤ 2×). |

## 3. Liste doublement chaînée (`Noeud`)

| Opération | Complexité | Justification |
|---|---|---|
| Insertion en tête (`insererTeteListe`) | **Θ(1)** | Aucun parcours nécessaire, simple réaffectation de pointeurs. |
| Insertion en queue (`insererQueueListe`) | **Θ(n)** | La liste ne maintient pas de pointeur de queue : il faut parcourir jusqu'au dernier nœud. *(Amélioration possible : maintenir un pointeur `queue` pour passer en Θ(1).)* |
| Recherche par clé | **Θ(n)** | Parcours séquentiel des maillons, pas d'accès direct par indice. |
| Recherche multicritère | **Θ(n)** | Idem. |
| Suppression (`supprimerProduitListe`) | **Θ(n)** recherche + **Θ(1)** reroutage | La recherche du nœud domine ; le retrait proprement dit (reroutage `prec`/`suiv`) est en O(1) grâce au double chaînage. |
| Mise à jour | **Θ(n)** | Dominée par la recherche. |
| Tri par insertion (`triInsertionListe`) | **O(n²)** | Reconstruction de la liste triée par insertions successives, avec un parcours de la partie déjà triée. |
| Tri fusion (2ᵉ algorithme, `benchmark.c`) | **Θ(n log n)** dans tous les cas | Découpage récursif en deux moitiés (recherche du milieu en O(n) par parcours "lent/rapide") + fusion linéaire ; stable et sans pire cas dégradé, contrairement au tri rapide. |
| Statistiques / médiane / écart-type | **Θ(n)** / **Θ(n)** moyen / **Θ(n)** | Nécessite une copie temporaire des prix dans un tableau (`realloc` dynamique) pour appliquer `quickselect`. |
| Persistance binaire | **Θ(n)** | Un `fwrite` par nœud parcouru séquentiellement. |
| **Espace mémoire** | **Θ(n)** + 2 pointeurs par nœud | Pas de gaspillage lié à une capacité surallouée (contrairement au tableau dynamique), mais surcoût fixe de `2 × sizeof(Noeud*)` par élément et moins bonne localité mémoire (accès dispersés → plus de défauts de cache). |

## 4. Tableau récapitulatif (opérations principales)

| Opération | Tableau statique | Tableau dynamique | Liste chaînée |
|---|---|---|---|
| Insertion | Θ(1) amorti | Θ(1) amorti | **Θ(1)** (tête) |
| Recherche par clé | Θ(n) | Θ(n) | Θ(n) |
| Suppression | Θ(n) | Θ(n) | Θ(n) recherche + **Θ(1)** retrait |
| Tri (insertion) | O(n²) | O(n²) | O(n²) |
| Tri (2ᵉ algo) | O(n log n) moyen | O(n log n) moyen | **Θ(n log n)** garanti |
| Espace | Θ(MAX) fixe | Θ(n) + surcoût pointeurs | Θ(n) + 2 pointeurs/nœud |
| Localité mémoire | Excellente (contiguë) | Bonne (indirection simple) | Faible (nœuds dispersés) |

## 5. Lecture des résultats expérimentaux (`benchmark.c`)

Les mesures produites dans `resultats_operations.csv` et `resultats_tri.csv`
confirment ces bornes théoriques :

- Le **tri par insertion** croît de façon quadratique avec `n` sur les 3
  structures (temps ×~4 quand `n` double), alors que le **tri rapide /
  tri fusion** croît de façon quasi-linéarithmique (temps ×~2 à 2,5 quand
  `n` double), avec un écart qui se creuse fortement à partir de `n ≥ 2000`.
- L'**insertion** et la **recherche** restent globalement linéaires en `n`
  sur les 3 structures, la liste chaînée étant pénalisée aux grandes tailles
  par les défauts de cache liés à la dispersion mémoire des nœuds.
- La **suppression sur tableau statique**, mesurée uniquement dans
  `benchmark.c` (absente de `PROJETSD1.c` à ce jour), confirme le coût
  Θ(n) du décalage des éléments restants.

> Voir `README.md` pour la procédure de génération de ces fichiers et leur
> intégration dans les tableaux/graphiques (pgfplots) du rapport LaTeX.
