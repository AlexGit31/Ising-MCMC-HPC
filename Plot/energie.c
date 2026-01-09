#include "grille.h"
#include <stdio.h>

/**
 * Calcule la somme des spins voisins pour une position (i, j).
 * Cette fonction centrale évite la duplication de code.
 */
static int sommeVoisins(struct grille *grille, int i, int j, int taille) {
    int somme = 0;
    if (i > 0)          somme += grille->sigma[i - 1][j]; // Haut
    if (i < taille - 1) somme += grille->sigma[i + 1][j]; // Bas
    if (j > 0)          somme += grille->sigma[i][j - 1]; // Gauche
    if (j < taille - 1) somme += grille->sigma[i][j + 1]; // Droite
    return somme;
}

/**
 * Calcul de la variation d'énergie si on flippe le spin (i, j).
 * DeltaH = 2 * spin_central * Somme(voisins)
 */
int calculVariationVoisin(struct grille *grille, int i, int j) {
    int taille = 2 * N + 1;
    int s_i = grille->sigma[i][j];
    return 2 * s_i * sommeVoisins(grille, i, j, taille);
}

/**
 * Calcul de l'énergie totale de la grille.
 * H = - Sum(spin_i * spin_j) pour toutes les paires de voisins.
 */
int calculEnergieInitiale(struct grille *grille) {
    int H = 0;
    int taille = 2 * N + 1;
    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            int s_i = grille->sigma[i][j];
            // Pour éviter de compter chaque interaction deux fois,
            // on ne regarde que vers la DROITE et le BAS.

            if (j < taille - 1) {
                H -= s_i * grille->sigma[i][j + 1]; // Droite
            }
            if (i < taille - 1) {
                H -= s_i * grille->sigma[i + 1][j]; // Bas
            }
        }
    }
    return H;
}
