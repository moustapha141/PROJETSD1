#!/usr/bin/env python3
"""
Genere les 4 categories de courbes exigees par le sujet (section 5.3) a
partir de resultats_benchmark.csv :

  1. Temps d'execution en fonction de n, par operation, toutes structures
     superposees, en echelle lineaire ET logarithmique.
  2. Temps d'execution en fonction de la distribution des donnees, a n fixe.
  3. Courbes empiriques superposees a la courbe theorique attendue
     (ajustement lineaire pour insertion/recherche, quadratique pour le tri).
  4. Histogramme du facteur de ralentissement (pire structure / meilleure
     structure) pour chaque operation, a n fixe.

Usage :
    python3 generer_courbes.py [chemin_vers_resultats_benchmark.csv]

Les figures sont ecrites dans le dossier ../courbes/ en PDF (pour le rapport
LaTeX) et en PNG (pour visualisation rapide).
"""

import sys
import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

CHEMIN_CSV = sys.argv[1] if len(sys.argv) > 1 else "resultats_benchmark.csv"
DOSSIER_SORTIE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "courbes")
os.makedirs(DOSSIER_SORTIE, exist_ok=True)

COULEURS = {"statique": "tab:blue", "dynamique": "tab:orange", "liste": "tab:green"}
DISTRIBUTION_REFERENCE = "aleatoire"  # cas "moyen" / typique, utilise pour les categories 1 et 3


def sauvegarder(fig, nom):
    fig.savefig(os.path.join(DOSSIER_SORTIE, nom + ".pdf"), bbox_inches="tight")
    fig.savefig(os.path.join(DOSSIER_SORTIE, nom + ".png"), bbox_inches="tight", dpi=150)
    plt.close(fig)
    print(f"  -> {nom}.pdf / {nom}.png")


def charger_donnees(chemin):
    df = pd.read_csv(chemin)
    # Moyenne sur les repetitions pour chaque (structure, operation, n, distribution)
    moyennes = df.groupby(["structure", "operation", "n", "distribution"], as_index=False)["temps_secondes"].mean()
    return moyennes


# ---------------------------------------------------------------------------
# Categorie 1 : temps en fonction de n, par operation, toutes structures
# superposees, echelle lineaire puis logarithmique (distribution "aleatoire")
# ---------------------------------------------------------------------------
def categorie1_temps_vs_n(moyennes):
    print("\n[1] Temps d'execution en fonction de n (par operation)")
    sous = moyennes[moyennes["distribution"] == DISTRIBUTION_REFERENCE]

    for operation in sous["operation"].unique():
        d = sous[sous["operation"] == operation]

        for echelle in ("lineaire", "log"):
            fig, ax = plt.subplots(figsize=(6, 4.5))
            for structure in ("statique", "dynamique", "liste"):
                dd = d[d["structure"] == structure].sort_values("n")
                if dd.empty:
                    continue
                ax.plot(dd["n"], dd["temps_secondes"], marker="o", label=structure,
                        color=COULEURS[structure])

            if echelle == "log":
                ax.set_xscale("log")
                ax.set_yscale("log")

            ax.set_xlabel("Taille n du jeu de donnees")
            ax.set_ylabel("Temps d'execution moyen (s)")
            ax.set_title(f"Temps de '{operation}' en fonction de n — echelle {echelle}\n"
                         f"(distribution {DISTRIBUTION_REFERENCE})")
            ax.legend(title="Structure")
            ax.grid(True, which="both", linestyle="--", alpha=0.4)
            sauvegarder(fig, f"1_temps_vs_n_{operation}_{echelle}")


# ---------------------------------------------------------------------------
# Categorie 2 : temps en fonction de la distribution, a n fixe (le plus grand
# n disponible dans le CSV)
# ---------------------------------------------------------------------------
def categorie2_temps_vs_distribution(moyennes):
    print("\n[2] Temps d'execution en fonction de la distribution (n fixe)")
    n_fixe = moyennes["n"].max()
    sous = moyennes[moyennes["n"] == n_fixe]
    distributions = ["aleatoire", "triee", "inversee"]
    structures = ["statique", "dynamique", "liste"]

    for operation in sous["operation"].unique():
        d = sous[sous["operation"] == operation]
        fig, ax = plt.subplots(figsize=(6, 4.5))
        largeur = 0.25
        positions = np.arange(len(distributions))

        for i, structure in enumerate(structures):
            valeurs = [
                d[(d["structure"] == structure) & (d["distribution"] == dist)]["temps_secondes"].values
                for dist in distributions
            ]
            valeurs = [v[0] if len(v) else 0 for v in valeurs]
            ax.bar(positions + i * largeur, valeurs, width=largeur, label=structure,
                   color=COULEURS[structure])

        ax.set_xticks(positions + largeur)
        ax.set_xticklabels(distributions)
        ax.set_xlabel("Distribution des donnees")
        ax.set_ylabel("Temps d'execution moyen (s)")
        ax.set_title(f"Temps de '{operation}' selon la distribution (n = {n_fixe})")
        ax.legend(title="Structure")
        ax.grid(True, axis="y", linestyle="--", alpha=0.4)
        sauvegarder(fig, f"2_temps_vs_distribution_{operation}")


# ---------------------------------------------------------------------------
# Categorie 3 : empirique vs theorique (ajustement par les moindres carres)
# ---------------------------------------------------------------------------
def ajuster_lineaire(x, y):
    a, b = np.polyfit(x, y, 1)
    return a * x + b, f"ajustement lineaire : {a:.2e}*n + {b:.2e}"


def ajuster_quadratique(x, y):
    a, b, c = np.polyfit(x, y, 2)
    return a * x ** 2 + b * x + c, f"ajustement quadratique : {a:.2e}*n^2 + {b:.2e}*n + {c:.2e}"


def ajuster_nlogn(x, y):
    base = x * np.log2(np.maximum(x, 2))
    a, residus, _, _ = np.linalg.lstsq(np.vstack([base, np.ones_like(base)]).T, y, rcond=None)
    pente, decalage = a
    return pente * base + decalage, f"ajustement n.log(n) : {pente:.2e}*n.log2(n) + {decalage:.2e}"


MODELE_THEORIQUE = {
    "insertion": ("O(n)", ajuster_lineaire),
    "recherche": ("O(n)", ajuster_lineaire),
    "tri": ("O(n^2)", ajuster_quadratique),
}


def categorie3_empirique_vs_theorique(moyennes):
    print("\n[3] Empirique vs theorique (ajustement)")
    sous = moyennes[moyennes["distribution"] == DISTRIBUTION_REFERENCE]

    for operation in sous["operation"].unique():
        d = sous[sous["operation"] == operation]
        modele_nom, fonction_ajustement = MODELE_THEORIQUE.get(operation, ("O(n)", ajuster_lineaire))

        fig, ax = plt.subplots(figsize=(6, 4.5))
        for structure in ("statique", "dynamique", "liste"):
            dd = d[d["structure"] == structure].sort_values("n")
            if dd.empty or len(dd) < 2:
                continue
            x = dd["n"].values.astype(float)
            y = dd["temps_secondes"].values.astype(float)
            y_theo, description = fonction_ajustement(x, y)

            ax.plot(x, y, "o", color=COULEURS[structure], label=f"{structure} (mesure)")
            ax.plot(x, y_theo, "--", color=COULEURS[structure], alpha=0.7,
                    label=f"{structure} ({modele_nom})")

        ax.set_xlabel("Taille n du jeu de donnees")
        ax.set_ylabel("Temps d'execution moyen (s)")
        ax.set_title(f"'{operation}' — mesure empirique vs modele theorique {modele_nom}")
        ax.legend(fontsize=8)
        ax.grid(True, linestyle="--", alpha=0.4)
        sauvegarder(fig, f"3_empirique_vs_theorique_{operation}")


# ---------------------------------------------------------------------------
# Categorie 4 : facteur de ralentissement (pire / meilleure structure), a n
# fixe, pour chaque operation
# ---------------------------------------------------------------------------
def categorie4_facteur_ralentissement(moyennes):
    print("\n[4] Facteur de ralentissement pire/meilleure structure")
    n_fixe = moyennes["n"].max()
    sous = moyennes[(moyennes["n"] == n_fixe) & (moyennes["distribution"] == DISTRIBUTION_REFERENCE)]

    operations = sorted(sous["operation"].unique())
    facteurs = []
    details = []
    for operation in operations:
        d = sous[sous["operation"] == operation]
        meilleure = d["temps_secondes"].min()
        pire = d["temps_secondes"].max()
        facteurs.append(pire / meilleure if meilleure > 0 else 0)
        details.append((d.loc[d["temps_secondes"].idxmin(), "structure"],
                         d.loc[d["temps_secondes"].idxmax(), "structure"]))

    fig, ax = plt.subplots(figsize=(6, 4.5))
    barres = ax.bar(operations, facteurs, color="tab:red", alpha=0.8)
    for barre, (meilleure_s, pire_s) in zip(barres, details):
        ax.text(barre.get_x() + barre.get_width() / 2, barre.get_height(),
                f"{pire_s}/{meilleure_s}", ha="center", va="bottom", fontsize=8)

    ax.set_xlabel("Operation")
    ax.set_ylabel("Facteur de ralentissement (pire / meilleure structure)")
    ax.set_title(f"Facteur de ralentissement par operation (n = {n_fixe}, distribution {DISTRIBUTION_REFERENCE})")
    ax.grid(True, axis="y", linestyle="--", alpha=0.4)
    sauvegarder(fig, "4_facteur_ralentissement")


def main():
    print(f"Lecture de {CHEMIN_CSV} ...")
    moyennes = charger_donnees(CHEMIN_CSV)
    print(f"{len(moyennes)} lignes agregees (moyenne sur les repetitions).")

    categorie1_temps_vs_n(moyennes)
    categorie2_temps_vs_distribution(moyennes)
    categorie3_empirique_vs_theorique(moyennes)
    categorie4_facteur_ralentissement(moyennes)

    print(f"\nTermine. Figures ecrites dans : {os.path.abspath(DOSSIER_SORTIE)}")


if __name__ == "__main__":
    main()
