#include "grille.h"
#include <stdbool.h>

// Fonction qui prend en agument une structure grille
// et qui renvoie son énergie H en fonction de la configuration de la grille.
// La fonction prend également en argument les indices i et j qui correspondent aux coordonnées du
// spin qui change de signe. Cela permet de calculer plus efficacement l'énergie d'une configuration voisine.
extern int calculVariationVoisin(struct grille * grille, int i, int j);


// Fonction qui cacule l'énergie initiale de la grille
extern int calculEnergieInitiale(struct grille * grille);

static int sommeVoisins(struct grille *, int, int, int);
