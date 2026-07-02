#ifndef PRODUITS_H
#define PRODUITS_H

#include <stddef.h>

// =====================================================================
// CONSTANTES
// =====================================================================
#define MAX_STATIQUE 100000
#define MAX_CLIENTS 100000
#define MAX_COMMANDES 100000

// =====================================================================
// TYPES DE DONNEES
// =====================================================================
typedef struct {
    int jour;
    int mois;
    int annee;
} Date;

typedef struct Produit {
    int idProduit;
    char nom[50];
    char categorie[30];
    float prix;
    int stock;
    char dateAjout[11]; // Format "AAAA-MM-JJ"
} Produit;

typedef struct Client {
    int idClient;
    char nom[50];
    char adresse[100];
    float montantTotalAchats;
} Client;

typedef struct Commande {
    int idCommande;
    char dateCommande[11];
    int nombreProduits;
    Produit *produits[20];
    Client *client;
} Commande;

typedef struct Noeud {
    Produit data;
    struct Noeud *suiv;
    struct Noeud *prec;
} Noeud;

// =====================================================================
// CATALOGUE - TABLEAU STATIQUE
// =====================================================================
Produit* rechercherProduit(Produit* liste, int id_produit);
Produit* saisirEtAjouterProduit(Produit* liste);
void afficherCatalogue(Produit* liste);
void sauvegarderCatalogue(Produit* liste);
Produit* chargerCatalogue(Produit* liste);
void triInsertionStatique(Produit* tab, int taille);
void mettreAJourProduitStatique(Produit* liste);
void genererDateCourante(char* buffer, size_t size);

// =====================================================================
// CATALOGUE - TABLEAU DYNAMIQUE DE POINTEURS
// =====================================================================
Produit** initialiserTableauDynamique(void);
Produit* rechercherProduitDyn(Produit** tab, int id_produit);
void rechercherMulticritereDyn(Produit** tab, const char* categorie, float prix_min, float prix_max);
Produit** insererProduitDyn(Produit** tab, Produit p);
void supprimerProduitDyn(Produit** tab, int id_produit);
void afficherCatalogueDyn(Produit** tab);
void sauvegarderTableauDynamique(Produit** tab);
Produit** chargerTableauDynamique(Produit** tab);
void libererTableauDynamique(Produit** tab);
void triInsertionDynamique(Produit** tab);
void mettreAJourProduitDyn(Produit** tab);

// =====================================================================
// CATALOGUE - LISTE DOUBLEMENT CHAINEE
// =====================================================================
Produit* rechercherProduitListe(Noeud* tete, int id_produit);
void rechercherMulticritereListe(Noeud* tete, const char* categorie, float prix_min, float prix_max);
Noeud* insererTeteListe(Noeud* tete, Produit p);
Noeud* insererQueueListe(Noeud* tete, Produit p);
Noeud* supprimerProduitListe(Noeud* tete, int id_produit);
void afficherCatalogueListe(Noeud* tete);
void sauvegarderCatalogueListe(const char* nom_fichier, Noeud* tete);
Noeud* chargerCatalogueListe(const char* nom_fichier);
void libererListe(Noeud* tete);
void triInsertionListe(Noeud** tete);
void mettreAJourProduitListe(Noeud* tete);
void afficherStatsListe(Noeud* tete);

// =====================================================================
// CLIENTS & PANIER
// =====================================================================
Client* rechercherClient(Client* liste, int id_client);
Client* enregistrerClient(Client* liste_clients);
void afficherListeClients(Client* liste_clients);
void ajouterAuPanier(Client* liste_clients, Produit* catalogue);
void afficherPanier(Client* client);
void sauvegarderClients(Client* liste_clients);
Client* chargerClients(Client* liste_clients);

// =====================================================================
// COMMANDES
// =====================================================================
Commande* passerCommande(Commande* liste_commandes, Client* liste_clients, Produit* catalogue, int* compteur_id_commande);
void afficherToutesLesCommandes(Commande* liste_commandes);
void sauvegarderCommandes(Commande* liste_commandes);
void chargerCommandes(Commande* liste_commandes, Client* liste_clients, Produit* catalogue);

// =====================================================================
// SECURITE & NETTOYAGE
// =====================================================================
void libererTout(Produit* cat, Client* clt, Commande* cmd);

#endif // PRODUITS_H
