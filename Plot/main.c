#include "energie.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double expo(double x){
    double facteur = x;
    double expo = 1 + facteur;
    for (int i=2;i<10;i++){
        facteur *= x/i;
        expo += facteur;
    }
    return expo;
}

// Algorithme de Metropolis-Hastings avec nb la taille de la boucle for
// la fonction prend également en argument la grille initiale
// l'argument T est la température
void metropolis_hastings(int nb, struct grille * grille, double T) {
    // 1. Ouvrir le fichier de sauvegarde
    FILE *f = fopen("evolution.dat", "w");
    if (f == NULL) {
        printf("Erreur d'ouverture du fichier !\n");
        return;
    }

    // 2. Sauvegarder la DIM et la grille initiale dans le fichier
    fprintf(f, "%d\n", DIM);

    for (int x = 0; x < DIM; x++) {
        for (int y = 0; y < DIM; y++) {
             fprintf(f, "%d ", grille->sigma[x][y]);
        }
        fprintf(f, "\n");
    }

    //printf("Dimension :%d\n",DIM);
    int n;
    int i,j;
    int H_sigma = calculEnergieInitiale(grille);
    int Variation;


    for (n=0; n < nb; n ++ ) {
        // On doit choisir le couple (i,j) avec une proba de 1 / (2nb+1)**2
        // ce qui revient à tirer uniformément i avec une proba 1 / 2nb+1 et j avec proba 1/ 2nb+1


        i = ((double)rand() /  (RAND_MAX) ) * (DIM); // nombre entre 0 et DIM-1
        j = ((double)rand() /  (RAND_MAX) ) * (DIM);

        //printf("(i,j)=(%d,%d)\n",i,j);


        //printf("Energie initiale :%d\n",H_sigma);
        voisin_grille(&grille, i,j);
        //int H_sigma_prim = calculEnergieInitiale(grille);

        Variation = calculVariationVoisin(grille,i,j);


        //printf("Energie Sigma : %d , Energie Voisin : % d , Variation : %d\n", H_sigma,H_sigma_prim,Variation);


        //printf("Energie voisin :%d\n",H_sigma_prim);

        //double difference = - (H_sigma - H_sigma_prim) / T;
        double difference = Variation/T;
        double rho = expo(difference);
        // printf("rho :%2f\n",rho);
    
        int accepte = 1; // On suppose qu'on accepte le changement par défaut

        // Si rho >= 1 alors on garde la grille actuelle
        if ( rho < 1 ) {
            double u = ((double) rand()) / (double )RAND_MAX; // tire une loi uniforme sur [0,1]

            // Si u < rho alors on garde la grille actuelle
            // sinon on récupére la grille avant changement
            //printf("u %f\n",u);
            if (u >= rho ) {
                voisin_grille(&grille, i,j);
                accepte = 0; // Le changement a été annulé
                //printf("grille gardée\n");
            } /*else {
                printf("grille modifiée2\n");
            }*/
        }
        H_sigma += Variation;
        // else {
        //     printf("Grille modififée1\n ");
        // }
        
        // 3. Si le changement est conservé, on l'écrit dans le fichier
        if (accepte == 1) {
            // On écrit juste les coordonnées du flip
            fprintf(f, "%d %d\n", i, j);
        }
     }
    fclose(f); // Très important de fermer le fichier !
    printf("Simulation terminée et sauvegardée dans evolution.dat\n");

}

int main() {
    srand((unsigned int)time(NULL));

    struct grille *p_grille;
    
    init_grille_border(&p_grille,1,1);
    printf("Avant l'algo\n");
    printf("expo:%lf\n",expo(1));
    print_grille(p_grille);

    int energy = calculEnergieInitiale(p_grille);
    init_energy_grille(p_grille, energy);

    metropolis_hastings(100000000, p_grille,1);
    printf("Après l'algo\n");
    print_grille(p_grille);
    (center_pos(p_grille)==1) ? printf("La grille a un centre positif\n")
    : printf("La grille a un centre négatif\n");
    clear_grille(&p_grille);
    return 0;
}



