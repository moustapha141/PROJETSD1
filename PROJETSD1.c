#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_STATIQUE 100000 
#define MAX_CLIENTS 100000
#define MAX_COMMANDES 100000
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

//PROTOTYPES DES FONCTIONS 
// Recherches
Client* rechercherClient(Client* liste, int id_client);
Produit* rechercherProduit(Produit* liste, int id_produit);

// Catalogue
Produit* saisirEtAjouterProduit(Produit* liste);
void afficherCatalogue(Produit* liste);
void sauvegarderCatalogue(Produit* liste);
Produit* chargerCatalogue(Produit* liste);
void genererDateCourante(char* buffer, size_t size);
void sauvegarderClients(Client* liste_clients);
Client* chargerClients(Client* liste_clients);
void sauvegarderCommandes(Commande* liste_commandes);
void chargerCommandes(Commande* liste_commandes, Client* liste_clients, Produit* catalogue);

// Clients & Panier
Client* enregistrerClient(Client* liste_clients);
void afficherListeClients(Client* liste_clients);
void ajouterAuPanier(Client* liste_clients, Produit* catalogue);
void afficherPanier(Client* client);

// Commandes
Commande* passerCommande(Commande* liste_commandes, Client* liste_clients, Produit* catalogue, int* compteur_id_commande);
void afficherToutesLesCommandes(Commande* liste_commandes);

// Sécurité & Nettoyage
void libererTout(Produit* cat, Client* clt, Commande* cmd);


// Variables globales simulant nos bases de données statiques (initialisées à 0)
Produit table_produits[MAX_STATIQUE];
Client table_clients[MAX_CLIENTS];
Commande table_commandes[MAX_COMMANDES];

//  Gestion Du CATALOGUE (PRODUITS)
Produit* saisirEtAjouterProduit(Produit* liste) {
    int i = 0;
    while (i < MAX_STATIQUE && liste[i].idProduit != 0) {
        i++;
    }
    
    if (i >= MAX_STATIQUE) {
        printf("Le catalogue statique est plein de %d du produits\n", MAX_STATIQUE);
        return NULL;
    }

    printf("Ajout d'un nouveau produit\n");
    printf("ID unique : ");
    scanf("%d", &liste[i].idProduit);
    getchar(); 

    printf("Nom du produit : ");
    fgets(liste[i].nom, 50, stdin);
    liste[i].nom[strcspn(liste[i].nom, "\n")] = 0; 

    printf("Categorie : ");
    fgets(liste[i].categorie, 30, stdin);
    liste[i].categorie[strcspn(liste[i].categorie, "\n")] = 0;

    printf("Prix : ");
    scanf("%f", &liste[i].prix);

    printf("Stock initial : ");
    scanf("%d", &liste[i].stock);
    getchar();
    
    genererDateCourante(liste[i].dateAjout, sizeof(liste[i].dateAjout));

    printf("Le produit a ete ajoute au tableau statique.\n");
    return &liste[i];
}

void afficherCatalogue(Produit* liste) {
    printf("Catalogue Produit\n");
    printf("%-6s | %-20s | %-15s | %-12s | %-6s\n", "ID", "Nom", "Categorie", "Prix", "Stock");

    for (int i = 0; i < MAX_STATIQUE; i++) {
        if (liste[i].idProduit == 0) break;
        printf("%-6d | %-20s | %-15s | %-10.2f F | %-6d\n", liste[i].idProduit, liste[i].nom, liste[i].categorie, liste[i].prix, liste[i].stock);
    }
}

void sauvegarderCatalogue(Produit* liste) {
    FILE* f = fopen("produits.dat", "wb");
    if (!f) {
        perror("Erreur de sauvegarde");
        return;
    }
    int total = 0;
    while (total < MAX_STATIQUE && liste[total].idProduit != 0) {
        total++;
    }
    fwrite(&total, sizeof(int), 1, f);
    fwrite(liste, sizeof(Produit), total, f);
    fclose(f);
    printf("Catalogue sauvegarde des %d du produits enregistres\n", total);
}

Produit* chargerCatalogue(Produit* liste) {
    FILE* f = fopen("produits.dat", "rb");
    if (!f) {
        /* printf("Aucun fichier 'produits.dat' trouve. Catalogue vide.\n")*/;
        return liste;
    }
    int total = 0;
    fread(&total, sizeof(int), 1, f);
    fread(liste, sizeof(Produit), total, f);
    fclose(f);
    printf("Catalogue charge de %d du produits\n", total);
    return liste;
}

void genererDateCourante(char* buffer, size_t size) {
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    if (!tm_info) {
        strncpy(buffer, "0000-00-00", size);
        buffer[size - 1] = '\0';
        return;
    }
    strftime(buffer, size, "%Y-%m-%d", tm_info);

}

//  FONCTIONS DES CLIENTS ET PANIERS
Client* enregistrerClient(Client* liste_clients) {
    int i = 0;
    while (i < MAX_CLIENTS && liste_clients[i].idClient != 0) {
        i++;
    }
    if (i >= MAX_CLIENTS) {
        printf("Limite de clients atteinte.\n");
        return NULL;
    }

    printf("Enregistrement Client\n");
    printf("ID Client : ");
    scanf("%d", &liste_clients[i].idClient);
    getchar();

    printf("Nom complet : ");
    fgets(liste_clients[i].nom, 50, stdin);
    liste_clients[i].nom[strcspn(liste_clients[i].nom, "\n")] = 0;

    printf("Adresse de livraison : ");
    fgets(liste_clients[i].adresse, 100, stdin);
    liste_clients[i].adresse[strcspn(liste_clients[i].adresse, "\n")] = 0;
    liste_clients[i].montantTotalAchats = 0.0f;

    printf("Client cree.\n");
    return &liste_clients[i];
}

void afficherListeClients(Client* liste_clients) {
    printf("LISTE DES CLIENTS\n");
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (liste_clients[i].idClient == 0) break;
        printf("ID: %d | Nom: %s | Adresse: %s | Total Achats: %.2f F\n",liste_clients[i].idClient, liste_clients[i].nom, liste_clients[i].adresse, liste_clients[i].montantTotalAchats);
    }
}
void sauvegarderClients(Client* liste_clients) {
    FILE* f = fopen("clients.dat", "wb");
    if (!f) {
        perror("Erreur de sauvegarde des clients");
        return;
    }
    int total = 0;
    while (total < MAX_CLIENTS && liste_clients[total].idClient != 0) {
        total++;
    }
    fwrite(&total, sizeof(int), 1, f);
    fwrite(liste_clients, sizeof(Client), total, f);
    fclose(f);
    printf("Clients sauvegardes : %d\n", total);
}

Client* chargerClients(Client* liste_clients) {
    FILE* f = fopen("clients.dat", "rb");
    if (!f) {
        /*printf("Aucun fichier 'clients.dat' trouve. Liste clients vide.\n")*/;
        return liste_clients;
    }
    int total = 0;
    fread(&total, sizeof(int), 1, f);
    fread(liste_clients, sizeof(Client), total, f);
    fclose(f);
    printf("Clients charges : %d\n", total);
    return liste_clients;
}
//  FONCTIONS DE RECHERCHE
Client* rechercherClient(Client* liste, int id_client) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (liste[i].idClient == id_client) {
            return &liste[i];
        }
        if (liste[i].idClient == 0) break; 
    }
    return NULL;
}
Produit* rechercherProduit(Produit* liste, int id_produit) {
    for (int i = 0; i < MAX_STATIQUE; i++) {
        if (liste[i].idProduit == id_produit) {
            return &liste[i];
        }
        if (liste[i].idProduit == 0) break;
    }
    return NULL;
}

void ajouterAuPanier(Client* liste_clients, Produit* catalogue) {
    int id_c, id_p;
    printf("ID du client qui fait ses achats : ");
    scanf("%d", &id_c);
    Client* c = rechercherClient(liste_clients, id_c);
    if (!c) {
        printf("Client introuvable.\n");
        return;
    }
    printf("ID du produit a ajouter : ");
    scanf("%d", &id_p);
    Produit* p = rechercherProduit(catalogue, id_p);
    if (!p || p->stock <= 0) {
        printf("Produit indisponible ou stock épuisé.\n");
        return;
    }
    p->stock--;
    printf("Produit '%s' reserve et ajoute au panier de %s.\n", p->nom, c->nom);
}

void afficherPanier(Client* client) {
    if (!client) return;
    printf("Panier de %s consultable lors du passage de commande final.\n", client->nom);
}

void noterProduit(Produit* catalogue) {
    int id_p;
    printf("ID du produit a noter : ");
    scanf("%d", &id_p);
    Produit* p = rechercherProduit(catalogue, id_p);
    if (p) {
        printf("Le produit '%s' a ete selectionne pour evaluation.\n", p->nom);
    } else {
        printf("Produit introuvable.\n");
    }
}

//  FONCTIONS DES COMMANDES
Commande* passerCommande(Commande* liste_commandes, Client* liste_clients, Produit* catalogue, int* compteur_id_commande) {
    int i = 0;
    while (i < MAX_COMMANDES && liste_commandes[i].idCommande != 0) {
        i++;
    }
    if (i >= MAX_COMMANDES) {
        printf("Plus de place pour de nouvelles commandes.\n");
        return NULL;
    }
    int id_c, id_p;
    printf("Validation de Commande\n");
    printf("ID Client de la commande : ");
    scanf("%d", &id_c);
    Client* clt = rechercherClient(liste_clients, id_c);
    if (!clt) {
        printf("Impossible de passer la commande : client non inscrit.\n");
        return NULL;
    }
    printf("ID du produit a acheter : ");
    scanf("%d", &id_p);
    Produit* prod = rechercherProduit(catalogue, id_p);
    if (!prod) {
        printf("Produit introuvable.\n");
        return NULL;
    }
    if (prod->stock <= 0) {
        printf("Le produit '%s' est en rupture de stock.\n", prod->nom);
        return NULL;
    }
    prod->stock--;
    liste_commandes[i].idCommande = (*compteur_id_commande)++;
    genererDateCourante(liste_commandes[i].dateCommande, sizeof(liste_commandes[i].dateCommande));
    liste_commandes[i].client = clt;
    memset(liste_commandes[i].produits, 0, sizeof(liste_commandes[i].produits));
    liste_commandes[i].produits[0] = prod;
    liste_commandes[i].nombreProduits = 1;
    clt->montantTotalAchats += prod->prix;
    printf("Commande %d enregistree \n", liste_commandes[i].idCommande);
    return &liste_commandes[i];
}
void sauvegarderCommandes(Commande* liste_commandes) {
    FILE* f = fopen("commandes.dat", "wb");
    if (!f) {
        perror("Erreur de sauvegarde des commandes");
        return;
    }
    int total = 0;
    while (total < MAX_COMMANDES && liste_commandes[total].idCommande != 0) {
        total++;
    }
    fwrite(&total, sizeof(int), 1, f);
    for (int i = 0; i < total; i++) {
        int id_client = liste_commandes[i].client ? liste_commandes[i].client->idClient : 0;
        fwrite(&liste_commandes[i].idCommande, sizeof(int), 1, f);
        fwrite(liste_commandes[i].dateCommande, sizeof(char), 11, f);
        fwrite(&liste_commandes[i].nombreProduits, sizeof(int), 1, f);
        fwrite(&id_client, sizeof(int), 1, f);
        for (int j = 0; j < 20; j++) {
            int produit_id = liste_commandes[i].produits[j] ? liste_commandes[i].produits[j]->idProduit : 0;
            fwrite(&produit_id, sizeof(int), 1, f);
        }
    }
    fclose(f);
    printf("Commandes sauvegardees : %d\n", total);
}

void chargerCommandes(Commande* liste_commandes, Client* liste_clients, Produit* catalogue) {
    FILE* f = fopen("commandes.dat", "rb");
    if (!f) {
       /* printf("Aucun fichier 'commandes.dat' trouve. Liste commandes vide.\n")*/;
        return;
    }
    int total = 0;
    fread(&total, sizeof(int), 1, f);
    for (int i = 0; i < total && i < MAX_COMMANDES; i++) {
        int id_client;
        fread(&liste_commandes[i].idCommande, sizeof(int), 1, f);
        fread(liste_commandes[i].dateCommande, sizeof(char), 11, f);
        liste_commandes[i].dateCommande[10] = '\0';
        fread(&liste_commandes[i].nombreProduits, sizeof(int), 1, f);
        fread(&id_client, sizeof(int), 1, f);
        liste_commandes[i].client = rechercherClient(liste_clients, id_client);
        for (int j = 0; j < 20; j++) {
            int produit_id;
            fread(&produit_id, sizeof(int), 1, f);
            liste_commandes[i].produits[j] = produit_id ? rechercherProduit(catalogue, produit_id) : NULL;
        }
    }
    fclose(f);
    printf("Commandes charges : %d\n", total);
}
void afficherToutesLesCommandes(Commande* liste_commandes) {
    if (!liste_commandes) {
        printf("Aucune commande disponible.\n");
        return;
    }
    int trouve = 0;
    for (int i = 0; i < MAX_COMMANDES; i++) {
        if (liste_commandes[i].idCommande == 0) break;
        trouve = 1;
        printf("La liste des commandes passees :\n");
        printf("Commande n°%d | Date: %s | Client: %s\n",liste_commandes[i].idCommande,liste_commandes[i].dateCommande,liste_commandes[i].client ? liste_commandes[i].client->nom : "<client inconnu>");
        if (liste_commandes[i].nombreProduits <= 0) {
            printf("  Aucun produit associe a cette commande.\n");
            continue;
        }

        float total_commande = 0.0f;
        for (int j = 0; j < liste_commandes[i].nombreProduits; j++) {
            Produit* produit = liste_commandes[i].produits[j];
            if (!produit) continue;
            printf("  Article %d : %s | Prix: %.2f FCF | Stock restant: %d\n", j + 1, produit->nom, produit->prix, produit->stock);
            total_commande += produit->prix;
        }
        printf("  Nombre de produits : %d | Total commande : %.2f FCF\n", liste_commandes[i].nombreProduits, total_commande);
    }

    if (!trouve) {
        printf("Aucune commande enregistreee pour le moment.\n");
    }
}
void libererTout(Produit* cat, Client* clt, Commande* cmd) {
    memset(cat, 0, MAX_STATIQUE * sizeof(Produit));
    memset(clt, 0, MAX_CLIENTS * sizeof(Client));
    memset(cmd, 0, MAX_COMMANDES * sizeof(Commande));
    printf("Nettoyage des tables statiques termine.\n");
}
// AGREGATIONS STATISTIQUES TABLEAU STATISTIQUE
static void echangerFloat(float* a, float* b) {
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

static int partition(float* copie, int gauche, int droite) {
    float pivot = copie[droite];
    int i = gauche - 1;
    for (int j = gauche; j < droite; j++) {
        if (copie[j] <= pivot) {
            i++;
            echangerFloat(&copie[i], &copie[j]);
        }
    }
    echangerFloat(&copie[i + 1], &copie[droite]);
    return i + 1;
}

static float quickselect(float* copie, int gauche, int droite, int k) {
    if (gauche == droite) {
        return copie[gauche];
    }
    int indice_pivot = partition(copie, gauche, droite);
    if (k == indice_pivot) {
        return copie[indice_pivot];
    } else if (k < indice_pivot) {
        return quickselect(copie, gauche, indice_pivot - 1, k);
    } else {
        return quickselect(copie, indice_pivot + 1, droite, k);
    }
}

static int compterProduitsStatique(Produit* liste) {
    int n = 0;
    while (n < MAX_STATIQUE && liste[n].idProduit != 0) {
        n++;
    }
    return n;
}

float minPrix(Produit* liste) {
    int n = compterProduitsStatique(liste);
    if (n == 0) {
        printf("Catalogue vide, impossible de calculer le minimum.\n");
        return -1.0f;
    }
    float min = liste[0].prix;
    for (int i = 1; i < n; i++) {
        if (liste[i].prix < min) min = liste[i].prix;
    }
    return min;
}

float maxPrix(Produit* liste) {
    int n = compterProduitsStatique(liste);
    if (n == 0) {
        printf("Catalogue vide, impossible de calculer le maximum.\n");
        return -1.0f;
    }
    float max = liste[0].prix;
    for (int i = 1; i < n; i++) {
        if (liste[i].prix > max) max = liste[i].prix;
    }
    return max;
}

float moyennePrix(Produit* liste) {
    int n = compterProduitsStatique(liste);
    if (n == 0) {
        printf("Catalogue vide, impossible de calculer la moyenne.\n");
        return -1.0f;
    }
    float somme = 0.0f;
    for (int i = 0; i < n; i++) somme += liste[i].prix;
    return somme / n;
}

float medianePrix(Produit* liste) {
    int n = compterProduitsStatique(liste);
    if (n == 0) {
        printf("Catalogue vide, impossible de calculer la mediane.\n");
        return -1.0f;
    }
    float* copie = (float*)malloc(n * sizeof(float));
    if (!copie) {
        perror("Erreur allocation memoire (mediane)");
        return -1.0f;
    }
    for (int i = 0; i < n; i++) copie[i] = liste[i].prix;
 
    float mediane;
    if (n % 2 == 1) {
        mediane = quickselect(copie, 0, n - 1, n / 2);
    } else {
        float m1 = quickselect(copie, 0, n - 1, n / 2 - 1);
        float m2 = quickselect(copie, 0, n - 1, n / 2);
        mediane = (m1 + m2) / 2.0f;
    }
    free(copie);
    return mediane;
}

float ecartTypePrix(Produit* liste) {
    int n = compterProduitsStatique(liste);
    if (n == 0) {
        printf("[Stats] Catalogue vide, impossible de calculer l'ecart-type.\n");
        return -1.0f;
    }
    float moy = moyennePrix(liste);
    float somme_carres = 0.0f;
    for (int i = 0; i < n; i++) {
        float diff = liste[i].prix - moy;
        somme_carres += diff * diff;
    }
    return sqrtf(somme_carres / n);
}

void afficherStatsStatique(Produit* liste) {
    int n = compterProduitsStatique(liste);
    if (n == 0) {
        printf("[Stats] Aucun produit dans le catalogue statique.\n");
        return;
    }
    printf("AGREGATIONS STATISTIQUES - Tableau Statique de %d du produits\n", n);
    printf("Minimum : %.2f FCF\n", minPrix(liste));
    printf("Maximum : %.2f FCF\n", maxPrix(liste));
    printf("Moyenne : %.2f FCF\n", moyennePrix(liste));
    printf("Mediane : %.2f FCF\n", medianePrix(liste));
    printf("Ecart-type : %.2f FCF\n", ecartTypePrix(liste));
}
// Prototypes Tableau Dynamique de pointeurs 
Produit** initialiserTableauDynamique();
Produit* rechercherProduitDyn(Produit** tab, int id_produit);
void rechercherMulticritereDyn(Produit** tab, const char* categorie, float prix_min, float prix_max);
Produit** insererProduitDyn(Produit** tab, Produit p);
void supprimerProduitDyn(Produit** tab, int id_produit);
void afficherCatalogueDyn(Produit** tab);
void sauvegarderTableauDynamique(Produit** tab);
Produit** chargerTableauDynamique(Produit** tab);
void libererTableauDynamique(Produit** tab);

int capacite_dyn = 10; 
int taille_dyn = 0;   

Produit** initialiserTableauDynamique() {
    Produit** tab = (Produit**)malloc(capacite_dyn * sizeof(Produit*));
    if (!tab) {
        perror("Erreur d'allocation initiale du tableau dynamique");
        exit(EXIT_FAILURE);
    }
    taille_dyn = 0;
    return tab;
}

Produit** insererProduitDyn(Produit** tab, Produit p) {
    if (taille_dyn >= capacite_dyn) {
        capacite_dyn *= 2;
        Produit** nouveau_tab = (Produit**)realloc(tab, capacite_dyn * sizeof(Produit*));
        if (!nouveau_tab) {
            perror("Erreur de realloc lors de l'agrandissement du tableau");
            return tab;
        }
        tab = nouveau_tab;
        printf("Redimensionnement du tableau dynamique : Nouvelle capacite = %d\n", capacite_dyn);
    }

    tab[taille_dyn] = (Produit*)malloc(sizeof(Produit));
    if (!tab[taille_dyn]) {
        perror("Erreur allocation memoire pour la structure produit");
        return tab;
    }

    *(tab[taille_dyn]) = p;
    taille_dyn++;

    return tab;
}

void supprimerProduitDyn(Produit** tab, int id_produit) {
    int indice_trouve = -1;
    for (int i = 0; i < taille_dyn; i++) {
        if (tab[i]->idProduit == id_produit) {
            indice_trouve = i;
            break;
        }
    }

    if (indice_trouve == -1) {
        printf("Produit ID %d introuvable dans le tableau dynamique.\n", id_produit);
        return;
    }
    free(tab[indice_trouve]);

    for (int i = indice_trouve; i < taille_dyn - 1; i++) {
        tab[i] = tab[i + 1];
    }

    taille_dyn--;
    printf("Produit ID %d supprime du tableau dynamique\n", id_produit);
}

void afficherCatalogueDyn(Produit** tab) {
    printf("Catalogue de tableau dynamique\n");
    printf("%s | %s | %s | %s FCF | %s\n", "ID", "Nom", "Categorie", "Prix", "Stock");
    for (int i = 0; i < taille_dyn; i++) {
        printf("%d | %s | %s | %.2f FCF | %d\n", tab[i]->idProduit, tab[i]->nom, tab[i]->categorie, tab[i]->prix, tab[i]->stock);
    }
}

Produit* rechercherProduitDyn(Produit** tab, int id_produit) {
    for (int i = 0; i < taille_dyn; i++) {
        if (tab[i]->idProduit == id_produit) {
            return tab[i]; 
        }
    }
    return NULL;
}

void rechercherMulticritereDyn(Produit** tab, const char* categorie, float prix_min, float prix_max) {
    int trouve = 0;
    printf("Recherche Multicritere de Tableau Dynamique\n");
    for (int i = 0; i < taille_dyn; i++) {
        if (strcmp(tab[i]->categorie, categorie) == 0 && tab[i]->prix >= prix_min && tab[i]->prix <= prix_max) {
            printf("ID: %d | Nom: %s | Prix: %.2f FCF | Stock: %d\n", tab[i]->idProduit, tab[i]->nom, tab[i]->prix, tab[i]->stock);
            trouve = 1;
        }
    }
    if (!trouve) printf("Aucun produit ne correspond aux critères.\n");
}
void sauvegarderTableauDynamique(Produit** tab) {
    FILE* f = fopen("dynamique.dat", "wb");
    if (!f) { perror("Erreur de sauvegarde"); return; }
    fwrite(&taille_dyn, sizeof(int), 1, f);
    for (int i = 0; i < taille_dyn; i++) {
        fwrite(tab[i], sizeof(Produit), 1, f);
    }
    fclose(f);
    printf("Tableau dynamique sauvegarde(s) : %d\n", taille_dyn);
}

Produit** chargerTableauDynamique(Produit** tab) {
    FILE* f = fopen("dynamique.dat", "rb");
    if (!f) 
    { /*printf("Aucun fichier 'dynamique.dat' trouve. Tableau dynamique vide.\n"); */
        return tab; 
    };
    int total = 0;
    fread(&total, sizeof(int), 1, f);
    for (int i = 0; i < total; i++) {
        Produit p;
        fread(&p, sizeof(Produit), 1, f);
        tab = insererProduitDyn(tab, p);
    }
    fclose(f);
    printf("Tableau dynamique charge(s) : %d\n", taille_dyn);
    return tab;
}
// AGREGATIONS STATISTIQUES TABLEAU DYNAMIQUE DE POINTEURS
float minPrixDyn(Produit** tab) {
    if (taille_dyn == 0) {
        printf("Tableau dynamique vide.\n");
        return -1.0f;
    }
    float min = tab[0]->prix;
    for (int i = 1; i < taille_dyn; i++) {
        if (tab[i]->prix < min) min = tab[i]->prix;
    }
    return min;
}

float maxPrixDyn(Produit** tab) {
    if (taille_dyn == 0) {
        printf("Tableau dynamique vide.\n");
        return -1.0f;
    }
    float max = tab[0]->prix;
    for (int i = 1; i < taille_dyn; i++) {
        if (tab[i]->prix > max) max = tab[i]->prix;
    }
    return max;
}

float moyennePrixDyn(Produit** tab) {
    if (taille_dyn == 0) {
        printf("Tableau dynamique vide.\n");
        return -1.0f;
    }
    float somme = 0.0f;
    for (int i = 0; i < taille_dyn; i++) somme += tab[i]->prix;
    return somme / taille_dyn;
}

float medianePrixDyn(Produit** tab) {
    if (taille_dyn == 0) {
        printf("Tableau dynamique vide.\n");
        return -1.0f;
    }
 
    float* copie = (float*)malloc(taille_dyn * sizeof(float));
    if (!copie) {
        perror("Erreur allocation memoire (mediane dyn)");
        return -1.0f;
    }
    for (int i = 0; i < taille_dyn; i++) copie[i] = tab[i]->prix;
 
    float mediane;
    if (taille_dyn % 2 == 1) {
        mediane = quickselect(copie, 0, taille_dyn - 1, taille_dyn / 2);
    } else {
        float m1 = quickselect(copie, 0, taille_dyn - 1, taille_dyn / 2 - 1);
        float m2 = quickselect(copie, 0, taille_dyn - 1, taille_dyn / 2);
        mediane = (m1 + m2) / 2.0f;
    }
    free(copie);
    return mediane;
}

float ecartTypePrixDyn(Produit** tab) {
    if (taille_dyn == 0) {
        printf("Tableau dynamique vide.\n");
        return -1.0f;
    }
    float moy = moyennePrixDyn(tab);
    float somme_carres = 0.0f;
    for (int i = 0; i < taille_dyn; i++) {
        float diff = tab[i]->prix - moy;
        somme_carres += diff * diff;
    }
    return sqrtf(somme_carres / taille_dyn);
}

void afficherStatsDynamique(Produit** tab) {
    if (taille_dyn == 0) {
        printf("Aucun produit dans le tableau dynamique.\n");
        return;
    }
    printf(" AGREGATIONS STATISTIQUES - Tableau Dynamique de %d du produits\n", taille_dyn);
    printf("Minimum : %.2f FCF\n", minPrixDyn(tab));
    printf("Maximum : %.2f FCF\n", maxPrixDyn(tab));
    printf("Moyenne : %.2f FCF\n", moyennePrixDyn(tab));
    printf("Mediane : %.2f FCF\n", medianePrixDyn(tab));
    printf("Ecart-type : %.2f FCF\n", ecartTypePrixDyn(tab));
}

void libererTableauDynamique(Produit** tab) {
    if (tab != NULL) {
        for (int i = 0; i < taille_dyn; i++) {
            free(tab[i]); 
        }
        free(tab); 
        printf("Memoire du tableau dynamique de pointeurs est liberee\n");
    }
}

// Prototypes Liste Doublement Chaînée 
Produit* rechercherProduitListe(Noeud* tete, int id_produit);
void rechercherMulticritereListe(Noeud* tete, const char* categorie, float prix_min, float prix_max);
Noeud* insererTeteListe(Noeud* tete, Produit p);
Noeud* supprimerProduitListe(Noeud* tete, int id_produit);
void afficherCatalogueListe(Noeud* tete);
void sauvegarderCatalogueListe(const char* nom_fichier, Noeud* tete);
Noeud* chargerCatalogueListe(const char* nom_fichier);
void libererListe(Noeud* tete);

//  INSERTION ET SUPPRESSION
Noeud* insererTeteListe(Noeud* tete, Produit p) {
    Noeud* nouveau = (Noeud*)malloc(sizeof(Noeud));
    if (!nouveau) {
        perror("Erreur allocation maillon");
        return tete;
    }
    nouveau->data = p;
    nouveau->suiv = tete;
    nouveau->prec = NULL;

    if (tete != NULL) {
        tete->prec = nouveau;
    }
    return nouveau; 
}
Noeud* insererQueueListe(Noeud* tete, Produit p) {
    Noeud* nouveau = (Noeud*)malloc(sizeof(Noeud));
    if (!nouveau) { perror("Erreur allocation maillon queue"); return tete; }
    nouveau->data = p;
    nouveau->suiv = NULL;

    if (tete == NULL) {
        nouveau->prec = NULL;
        return nouveau;
    }
    Noeud* courant = tete;
    while (courant->suiv != NULL) courant = courant->suiv;
    courant->suiv = nouveau;
    nouveau->prec = courant;
    return tete;
}
Noeud* supprimerProduitListe(Noeud* tete, int id_produit) {
    Noeud* courant = tete;

    while (courant != NULL && courant->data.idProduit != id_produit) {
        courant = courant->suiv;
    }

    if (courant == NULL) {
        printf("Produit ID %d introuvable. Suppression impossible.\n", id_produit);
        return tete; 
    }
    if (courant->prec != NULL) {
        courant->prec->suiv = courant->suiv;
    } else {
        tete = courant->suiv; 
    }

    if (courant->suiv != NULL) {
        courant->suiv->prec = courant->prec;
    }

    free(courant);
    printf("Produit ID %d supprime de la liste avec succes.\n", id_produit);
    return tete;
}
// 3. AFFICHAGE, PERSISTANCE & NETTOYAGE
void afficherCatalogueListe(Noeud* tete) {
    printf("CATALOGUE de LISTE DOUBLE\n");
    printf("%-6s | %-20s | %-15s | %-12s | %-6s\n", "ID", "Nom", "Categorie", "Prix", "Stock");
    Noeud* courant = tete;
    while (courant != NULL) {
        printf("%-6d | %-20s | %-15s | %-10.2f F | %-6d\n", courant->data.idProduit, courant->data.nom, courant->data.categorie, courant->data.prix, courant->data.stock);
        courant = courant->suiv;
    }
}
// RECHERCHE 
Produit* rechercherProduitListe(Noeud* tete, int id_produit) {
    Noeud* courant = tete;
    while (courant != NULL) {
        if (courant->data.idProduit == id_produit) {
            return &(courant->data); 
        }
        courant = courant->suiv;
    }
    return NULL; 
}
void rechercherMulticritereListe(Noeud* tete, const char* categorie, float prix_min, float prix_max) {
    Noeud* courant = tete;
    int trouve = 0;
    printf("Resultat Recherche de %s, %.2fF , %.2fF\n", categorie, prix_min, prix_max);
    
    while (courant != NULL) {
        if (strcmp(courant->data.categorie, categorie) == 0 && 
            courant->data.prix >= prix_min && courant->data.prix <= prix_max) {
            printf("ID: %d | Nom: %s | Prix: %.2f F | Stock: %d\n", courant->data.idProduit, courant->data.nom, courant->data.prix, courant->data.stock);
            trouve = 1;
        }
        courant = courant->suiv;
    }
    if (!trouve) printf("Aucun produit ne correspond à ces critères.\n");
}
void sauvegarderCatalogueListe(const char* nom_fichier, Noeud* tete) {
    FILE* f = fopen(nom_fichier, "wb");
    if (!f) return;
    int total = 0;
    Noeud* courant = tete;
    while (courant != NULL) {
        total++;
        courant = courant->suiv;
    }
    fwrite(&total, sizeof(int), 1, f);
    courant = tete;
    while (courant != NULL) {
        fwrite(&(courant->data), sizeof(Produit), 1, f);
        courant = courant->suiv;
    }
    fclose(f);
    printf("Liste sauvegardee de %d du produits enregistres\n", total);
}

Noeud* chargerCatalogueListe(const char* nom_fichier) {
    FILE* f = fopen(nom_fichier, "rb");
    if (!f) return NULL;

    int total = 0;
    if (fread(&total, sizeof(int), 1, f) != 1) {
        fclose(f);
        return NULL;
    }

    Noeud* tete = NULL;
    Produit p;
    for (int i = 0; i < total; i++) {
        fread(&p, sizeof(Produit), 1, f);
        tete = insererTeteListe(tete, p);
    }
    fclose(f);
    printf("Liste chargee depuis le fichier de %s du produits\n", nom_fichier);
    return tete;
}
// AGREGATIONS STATISTIQUES - LISTE CHAINEE
void afficherStatsListe(Noeud* tete) {
    if (!tete) { printf("Liste chainee vide.\n"); return; }
    int n = 0;
    float somme = 0, min, max, somme_carres = 0;
    min = max = tete->data.prix;
    int cap = 1000;
    float* copie = (float*)malloc(cap * sizeof(float));
    if (!copie) { perror("Erreur allocation stats liste"); return; }

    Noeud* c = tete;
    while (c) {
        if (n >= cap) { cap *= 2; copie = realloc(copie, cap * sizeof(float)); }
        copie[n] = c->data.prix;
        somme += c->data.prix;
        if (c->data.prix < min) min = c->data.prix;
        if (c->data.prix > max) max = c->data.prix;
        n++; c = c->suiv;
    }
    float moy = somme / n;
    for (int i = 0; i < n; i++) { float d = copie[i] - moy; somme_carres += d * d; }
    float ecart = sqrtf(somme_carres / n);
    float mediane;
    if (n % 2 == 1) mediane = quickselect(copie, 0, n-1, n/2);
    else { float m1 = quickselect(copie, 0, n-1, n/2-1); float m2 = quickselect(copie, 0, n-1, n/2); mediane = (m1+m2)/2.0f; }
    free(copie);

    printf("AGREGATIONS STATISTIQUES - Liste Chainee de %d du produits\n", n);
    printf("Minimum  : %.2f FCF\n", min);
    printf("Maximum  : %.2f FCF\n", max);
    printf("Moyenne  : %.2f FCF\n", moy);
    printf("Mediane  : %.2f FCF\n", mediane);
    printf("Ecart-type: %.2f FCF\n", ecart);
}
void libererListe(Noeud* tete) {
    Noeud* courant = tete;
    while (courant != NULL) {
        Noeud* a_supprimer = courant;
        courant = courant->suiv;
        free(a_supprimer);
    }
    printf("Memoire de la liste doublement chainee liberee.\n");
}

// Prototypes de tri 
void triInsertionStatique(Produit* tab, int taille);
void triInsertionDynamique(Produit** tab);
void triInsertionListe(Noeud** tete);
void mettreAJourProduitStatique(Produit* liste);
void mettreAJourProduitDyn(Produit** tab);
void mettreAJourProduitListe(Noeud* tete);
void afficherStatsListe(Noeud* tete);
Noeud* insererQueueListe(Noeud* tete, Produit p);

// 1. TRI PAR INSERTION - TABLEAU STATIQUE
// Critère : Prix (Ordre Croissant) 
void triInsertionStatique(Produit* tab, int taille) {
    if (taille <= 1) return;

    for (int i = 1; i < taille; i++) {
        Produit cle = tab[i];
        int j = i - 1;
        while (j >= 0 && tab[j].prix > cle.prix) {
            tab[j + 1] = tab[j];
            j--;
        }
        tab[j + 1] = cle;
    }
    printf("Tableau statique trie par prix croissant\n");
}

// 2. TRI PAR INSERTION - TABLEAU DYNAMIQUE DE POINTEURS
// Critère : Stock (Ordre Décroissant)
void triInsertionDynamique(Produit** tab) {
    if (taille_dyn <= 1) return;

    for (int i = 1; i < taille_dyn; i++) {
        Produit* cle_pointeur = tab[i];
        int j = i - 1;
        while (j >= 0 && tab[j]->stock < cle_pointeur->stock) {
            tab[j + 1] = tab[j];
            j--;
        }
        tab[j + 1] = cle_pointeur;
    }
    printf("Tableau dynamique trie par stock decroissant\n");
}

// TRI PAR INSERTION - LISTE DOUBLEMENT CHAINEE
// Critere : prix croissant
void triInsertionListe(Noeud** tete) {
    if (!tete || !(*tete) || !(*tete)->suiv) return;
    Noeud* trie = (*tete);
    Noeud* courant = (*tete)->suiv;
    trie->suiv = NULL;
    trie->prec = NULL;

    while (courant != NULL) {
        Noeud* suivant = courant->suiv;
        if (courant->data.prix <= trie->data.prix) {
            courant->suiv = trie;
            courant->prec = NULL;
            trie->prec = courant;
            trie = courant;
        } else {
            Noeud* pos = trie;
            while (pos->suiv != NULL && pos->suiv->data.prix < courant->data.prix)
                pos = pos->suiv;
            courant->suiv = pos->suiv;
            courant->prec = pos;
            if (pos->suiv) pos->suiv->prec = courant;
            pos->suiv = courant;
        }
        courant = suivant;
    }
    *tete = trie;
    printf("Liste chainee triee par prix croissant.\n");
}

void mettreAJourProduitStatique(Produit* liste) {
    int id; printf("ID du produit a modifier : "); 
	scanf("%d", &id);
    Produit* p = rechercherProduit(liste, id);
    if (!p) { printf("Produit introuvable.\n"); return; }
    int champ;
    printf("Que modifier ? 1.Nom  2.Categorie  3.Prix  4.Stock\nChoix : ");
    scanf("%d", &champ); getchar();
    if (champ == 1) { 
	printf("Nouveau nom : "); 
	fgets(p->nom, 50, stdin); p->nom[strcspn(p->nom,"\n")]=0; }
    else if (champ == 2) { 
	printf("Nouvelle categorie : ");
	fgets(p->categorie, 30, stdin); p->categorie[strcspn(p->categorie,"\n")]=0; }
    else if (champ == 3) { 
	printf("Nouveau prix : "); 
	scanf("%f", &p->prix); }
    else if (champ == 4) { 
	printf("Nouveau stock : ");
    scanf("%d", &p->stock); }
    else { 
	printf("Champ invalide.\n"); return; }
    printf("Produit ID %d mis a jour (tableau statique).\n", id);
}

void mettreAJourProduitDyn(Produit** tab) {
    int id; printf("ID du produit a modifier : "); 
	scanf("%d", &id);
    Produit* p = rechercherProduitDyn(tab, id);
    if (!p) { 
	printf("Produit introuvable.\n"); return; }
    int champ;
    printf("Que modifier ? 1.Nom  2.Categorie  3.Prix  4.Stock\nChoix : ");
    scanf("%d", &champ); getchar();
    if (champ == 1) { 
	printf("Nouveau nom : "); 
	fgets(p->nom, 50, stdin); p->nom[strcspn(p->nom,"\n")]=0; }
    else if (champ == 2) {
	printf("Nouvelle categorie : "); 
	fgets(p->categorie, 30, stdin); p->categorie[strcspn(p->categorie,"\n")]=0; }
    else if (champ == 3) { 
	printf("Nouveau prix : "); 
	scanf("%f", &p->prix); }
    else if (champ == 4) { 
	printf("Nouveau stock : "); 
	scanf("%d", &p->stock); }
    else { 
	printf("Champ invalide.\n"); return; }
    printf("Produit ID %d mis a jour (tableau dynamique).\n", id);
}

void mettreAJourProduitListe(Noeud* tete) {
    int id; 
	printf("ID du produit a modifier : "); 
	scanf("%d", &id);
    Produit* p = rechercherProduitListe(tete, id);
    if (!p) { 
	printf("Produit introuvable.\n"); return; }
    int champ;
    printf("Que modifier ? 1.Nom  2.Categorie  3.Prix  4.Stock\nChoix : ");
    scanf("%d", &champ); getchar();
    if (champ == 1) { 
	printf("Nouveau nom : "); 
	fgets(p->nom, 50, stdin); p->nom[strcspn(p->nom,"\n")]=0; }
    else if (champ == 2) { 
	printf("Nouvelle categorie : "); 
	fgets(p->categorie, 30, stdin); p->categorie[strcspn(p->categorie,"\n")]=0; }
    else if (champ == 3) { 
	printf("Nouveau prix : "); 
	scanf("%f", &p->prix); }
    else if (champ == 4) { 
	printf("Nouveau stock : "); 
	scanf("%d", &p->stock); }
    else { printf("Champ invalide.\n"); return; }
    printf("Produit ID %d mis a jour (liste chainee).\n", id);
}

int main() {


					    printf("\n");
					    printf("%58s\n", "Universite Iba Der Thiam de Thies");
					    printf("\n");
					    printf("%53s\n", "UFR Sciences et Technologies");
					    printf("%64s\n", "Departement de Mathematiques et Informatique");
					    printf("\n");
					    printf("%53s\n", "__________________________________");
					    printf("\n\n\n");
					
					    printf("%53s\n","RAPPORT DE PROJET");
					    printf("%57s\n",     "Algorithmique et Structures de Donnees");
					    printf("\n\n");
					
					   
					    printf("         +-------------------------------------------------------------+\n");
					    printf("         |                                                             |\n");
					    printf("         |     Conception, Implementation et Evaluation Comparative    |\n");
					    printf("         |                                                             |\n");
					    printf("         |             de Structures de Donnees pour un                |\n");
					    printf("         |                                                             |\n");
					    printf("         |           Systeme de Gestion de Commerce en Ligne           |\n");
					    printf("         |                                                             |\n");
					    printf("         | Tableau Statique . Tableau Dynamique de Pointeurs . Liste DC |\n");
					    printf("         +-------------------------------------------------------------+\n");
					    printf("\n\n\n");
					
					   
					    printf("         Filiere :             Licence 2 Mathematiques-Informatique (LMI 2)\n");
					    printf("         Annee universitaire : 2025-2026\n");
					    printf("         Enseignant resp. :    Dr Abdoulaye DIALLO\n");
					    printf("         Modalite :            Binome\n");
					    printf("         Langage :             C (compile avec gcc -Wall -Wextra)\n");
					    printf("\n\n\n");
					
					   
					    printf("%43s\n", "Juin 2026");
					    printf("\n");
					    printf("%53s\n", "__________________________________");
					    printf("\n");

  

    memset(table_produits, 0, sizeof(table_produits));
    memset(table_clients, 0, sizeof(table_clients));
    memset(table_commandes, 0, sizeof(table_commandes));

    Produit* catalogue = table_produits;
    Client* liste_clients = table_clients;
    Commande* liste_commandes = table_commandes;

    Produit** catalogue_dyn = initialiserTableauDynamique();
    Noeud* ma_liste = NULL;

    ma_liste = chargerCatalogueListe("liste.dat");
    if (!ma_liste) 
	printf("Aucune liste chainee trouvee. Liste vide.\n");

    int id_commande_auto = 1;
    int choix;

    chargerCatalogue(catalogue);
    chargerClients(liste_clients);
    chargerCommandes(liste_commandes, liste_clients, catalogue);
    catalogue_dyn = chargerTableauDynamique(catalogue_dyn);
    printf("\n\n\n");
    

    do {
        printf("  BIENVENUE SUR MON ESPACE-COMMERCE EN LIGNE:\n");
        printf("    GESTION DU CATALOGUE\n");
        printf("case1.  Gestion du catalogue (Ajouter/Afficher produits)\n");
        printf("case7.  Trier et afficher - Tableau Statique (par prix)\n");
        printf("case8.  Trier et afficher - Tableau Dynamique (par stock)\n");
        printf("case9.  Gestion Liste Doublement Chainee\n");
        printf("case10. Mise a jour d'un produit\n");
        printf("   GESTION du CLIENTS et COMMANDES\n");
        printf("case2.  Creer un compte client\n");
        printf("case3.  Ajouter au panier et Voir le catalogue\n");
        printf("case4.  Voir mon panier et Passer commande\n");
        printf("case5.  Voir toutes les commandes passees\n");
        printf("   GESTION STATISTIQUES \n");
        printf("case6.  Agregations statistiques sur les prix\n");
        printf("case0.  Quitter le programme\n");
        printf("Votre choix : ");
        
        if (scanf("%d", &choix) != 1) {
            printf("Saisie invalide ! Veuillez entrer un nombre.\n");
            while (getchar() != '\n'); 
            choix = -1; 
            continue;
        }

        switch (choix) {
            case 1:
                printf("Gestion du catalogue\n");
                int sous_choix;
                printf("case1. Ajouter un produit\n");
                printf("case2. Afficher le catalogue\n");
                printf("Votre choix : ");
                scanf("%d", &sous_choix);
                if (sous_choix == 1) {
                    Produit* newProduit = saisirEtAjouterProduit(catalogue);
                    if (newProduit) {
                        catalogue_dyn = insererProduitDyn(catalogue_dyn, *newProduit);
                    }
                } else if (sous_choix == 2) {
                    afficherCatalogue(catalogue);
                } else {
                    printf("Option invalide.\n");
                }
                break;
            case 2:
                printf("Création de compte du client\n");
                int sous_choix_client;
                printf("case1. Enregistrer un nouveau client\n");
                printf("case2. Afficher la liste des clients\n");
                printf(" Votre choix : ");
                scanf("%d", &sous_choix_client);
                if (sous_choix_client == 1) {
                    enregistrerClient(liste_clients);
                } else if (sous_choix_client == 2) {
                    afficherListeClients(liste_clients);
                } else {
                    printf("Option invalide.\n");
                }
                break;
            case 3:
                printf("Ajouter au panier\n");
                int sous_choix_achat;
                printf("case1. Ajouter un produit au panier\n");
                printf("case2. Voir le catalogue complet\n");
                
                printf("Votre choix : ");
                scanf("%d", &sous_choix_achat);
                if (sous_choix_achat == 1) {
                    ajouterAuPanier(liste_clients, catalogue);
                } else if (sous_choix_achat == 2) {
                    afficherCatalogue(catalogue);
                }else {
                    printf("Option invalide.\n");
                }
                break;
            case 4:
               {
			    int sous_choix_commande;
                printf("case1. Visualiser mon panier\n");
                printf("case2. Passer une commande\n");
                printf("Votre choix : ");
                scanf("%d", &sous_choix_commande);

                if (sous_choix_commande == 1) {
                    int id_c;
                    printf("ID Client : ");
                    scanf("%d", &id_c);
                    Client* clt = rechercherClient(liste_clients, id_c);
                    if (clt != NULL) 
                        afficherPanier(clt);
                    else 
                        printf("Client introuvable.\n");
                }
                else if (sous_choix_commande == 2){
                    passerCommande(liste_commandes, liste_clients, catalogue, &id_commande_auto);
                }
                else {
                    printf("Option invalide.\n");
                }
                break;}         
            case 5:
                afficherToutesLesCommandes(liste_commandes);
                break;
            case 6: 
                printf("Agregations statistiques sur les prix\n");
                int sous_choix_stats;
                printf("case1. Stats tableau statique\n");
                printf("case2. Stats tableau dynamique\n");
                printf("Votre choix : ");
                scanf("%d", &sous_choix_stats);
                if (sous_choix_stats == 1) {
                    afficherStatsStatique(catalogue);
                } else if (sous_choix_stats == 2) {
                    afficherStatsDynamique(catalogue_dyn);
                }
              break;
              case 7: 
               {
				int taille_actuelle_stat = 0;
                while(taille_actuelle_stat < MAX_STATIQUE && table_produits[taille_actuelle_stat].idProduit != 0) {
                taille_actuelle_stat++;
                }
                triInsertionStatique(table_produits, taille_actuelle_stat);
                afficherCatalogue(table_produits);
                break;}
            case 8:
                triInsertionDynamique(catalogue_dyn); 
                afficherCatalogueDyn(catalogue_dyn);
                break;
            case 9: {
                int sous_choix_liste;
                printf("=== LISTE DOUBLEMENT CHAINEE ===\n");
                printf("case1. Ajouter un produit (en tete)\n");
                printf("case2. Ajouter un produit (en queue)\n");
                printf("case3. Afficher la liste\n");
                printf("case4. Rechercher par ID\n");
                printf("case5. Recherche multicritere de categorie et de prix\n");
                printf("case6. Supprimer un produit par ID\n");
                printf("case7. Trier la liste par prix\n");
                printf("case8. Statistiques de la liste\n");
                printf("Votre choix : ");
                scanf("%d", &sous_choix_liste);
                switch (sous_choix_liste) {
                    case 1: {
                        Produit p;
                        printf("ID : "); scanf("%d", &p.idProduit); getchar();
                        printf("Nom : "); fgets(p.nom, 50, stdin); p.nom[strcspn(p.nom, "\n")] = 0;
                        printf("Categorie : "); fgets(p.categorie, 30, stdin); p.categorie[strcspn(p.categorie, "\n")] = 0;
                        printf("Prix : "); scanf("%f", &p.prix);
                        printf("Stock : "); scanf("%d", &p.stock);
                        genererDateCourante(p.dateAjout, sizeof(p.dateAjout));
                        ma_liste = insererTeteListe(ma_liste, p);
                        printf("Produit ajoute en tete de liste.\n");
                        break;
                    }
                    case 2: {
                        Produit p;
                        printf("ID : "); scanf("%d", &p.idProduit); getchar();
                        printf("Nom : "); fgets(p.nom, 50, stdin); p.nom[strcspn(p.nom, "\n")] = 0;
                        printf("Categorie : "); fgets(p.categorie, 30, stdin); p.categorie[strcspn(p.categorie, "\n")] = 0;
                        printf("Prix : "); scanf("%f", &p.prix);
                        printf("Stock : "); scanf("%d", &p.stock);
                        genererDateCourante(p.dateAjout, sizeof(p.dateAjout));
                        ma_liste = insererQueueListe(ma_liste, p);
                        printf("Produit ajoute en queue de liste.\n");
                        break;
                    }
                    case 3:
                        afficherCatalogueListe(ma_liste);
                        break;
                    case 4: {
                        int id; printf("ID du produit : "); scanf("%d", &id);
                        Produit* p = rechercherProduitListe(ma_liste, id);
                        if (p) printf("Trouve : %s | %.2f FCF | Stock: %d\n", p->nom, p->prix, p->stock);
                        else printf("Produit ID %d introuvable.\n", id);
                        break;
                    }
                    case 5: {
                        char cat[30]; float pmin, pmax;
                        getchar();
                        printf("Categorie : "); fgets(cat, 30, stdin); cat[strcspn(cat, "\n")] = 0;
                        printf("Prix min : "); scanf("%f", &pmin);
                        printf("Prix max : "); scanf("%f", &pmax);
                        rechercherMulticritereListe(ma_liste, cat, pmin, pmax);
                        break;
                    }
                    case 6: {
                        int id; printf("ID a supprimer : "); scanf("%d", &id);
                        ma_liste = supprimerProduitListe(ma_liste, id);
                        break;
                    }
                    case 7:
                        triInsertionListe(&ma_liste);
                        afficherCatalogueListe(ma_liste);
                        break;
                    case 8:
                        afficherStatsListe(ma_liste);
                        break;
                    default:
                        printf("Option invalide.\n");
                }
                break;}
            
            case 10: {
                int sous_choix_maj;
                printf("=== MISE A JOUR D'UN PRODUIT ===\n");
                printf("case1. Tableau Statique\n");
                printf("case2. Tableau Dynamique\n");
                printf("case3. Liste Chainee\n");
                printf("Votre choix : ");
                scanf("%d", &sous_choix_maj);
                if (sous_choix_maj == 1) mettreAJourProduitStatique(catalogue);
                else if (sous_choix_maj == 2) mettreAJourProduitDyn(catalogue_dyn);
                else if (sous_choix_maj == 3) mettreAJourProduitListe(ma_liste);
                else printf("Option invalide.\n");
                break;}
            
            case 0:
                printf("Merci de votre visite ! Liberation de la memoire et fermeture.\n");
                sauvegarderCatalogue(catalogue);
                sauvegarderClients(liste_clients);
                sauvegarderCommandes(liste_commandes);
                sauvegarderCatalogueListe("liste.dat", ma_liste);
                sauvegarderTableauDynamique(catalogue_dyn);
                libererTout(catalogue, liste_clients, liste_commandes);
                libererTableauDynamique(catalogue_dyn);
                libererListe(ma_liste);
                printf("Memoire nettoyee. Au revoir !\n");
                break;

            default:
                printf("Option indisponible. Veuillez choisir un nombre.\n");
                break;
        }
    } while (choix != 0);

    return 0;
}

               
