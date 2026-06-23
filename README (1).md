# Catalogue d'un commerce en ligne — Projet Algo & SD (LMI2)

**Binôme :** [Nom 1] & [Nom 2]
**Filière :** Licence 2 Mathématiques-Informatique — UIDT Thiès
**Enseignant :** Dr Abdoulaye Diallo — Année 2025/2026

## 1. Description

Ce projet implémente et compare plusieurs structures de données (tableau statique,
tableau dynamique de pointeurs, liste doublement chaînée, [structure 4 à compléter],
[structure 5 à compléter]) manipulant le même jeu d'enregistrements : un catalogue
de produits, avec gestion des clients et des commandes pour un commerce en ligne.

## 2. Organisation du dépôt

```
.
├── src/
│   └── PROJETSD1.c        # Code source unique (à modulariser si besoin)
├── Makefile
├── README.md
├── docs/
│   ├── manuel_utilisateur.pdf
│   └── rapport/
│       ├── rapport.tex
│       └── rapport.pdf
├── data/                  # Jeux de données générés pour les benchmarks
├── scripts/                # Scripts de génération de données / mesure des temps
└── courbes/                # Graphiques produits (sources + PDF/PNG)
```

## 3. Compilation

```bash
make        # compile le projet (cible "all")
make clean  # supprime l'executable et les fichiers de données .dat
```

Compilé avec `gcc -Wall -Wextra -std=c11`, sans aucun warning.

## 4. Exécution

```bash
./projetsd1
```

Un menu interactif permet de :
- gérer le catalogue (ajout, affichage, tri, mise à jour, suppression) sur chacune
  des structures de données implémentées ;
- créer des comptes clients et gérer leurs commandes (panier, passage de commande) ;
- consulter les agrégations statistiques sur les prix (min, max, moyenne, médiane,
  écart-type) ;
- sauvegarder/recharger automatiquement l'ensemble des données entre deux exécutions
  (fichiers `produits.dat`, `clients.dat`, `commandes.dat`, `liste.dat`, `dynamique.dat`).

## 5. État d'avancement

| Structure | Opérations CRUD | Tri | Stats | Persistance |
|---|---|---|---|---|
| Tableau statique | [À compléter] | ✅ | ✅ | ✅ |
| Tableau dynamique | ✅ | ✅ | ✅ | ✅ |
| Liste doublement chaînée | ✅ | ✅ | ✅ | ✅ |
| [Structure 4] | [À compléter] | | | |
| [Structure 5] | [À compléter] | | | |

## 6. Auteurs et répartition du travail

[À compléter : qui a fait quoi dans le binôme]

## 7. Liens

- Dépôt Git : [URL à compléter]
- Rapport complet : `docs/rapport/rapport.pdf`
- Manuel utilisateur : `docs/manuel_utilisateur.pdf`
