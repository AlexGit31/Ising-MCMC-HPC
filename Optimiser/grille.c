#include "grille.h"
#include <stdio.h>
#include <stdlib.h>

// energie initialisée à 0
void init_grille(struct grille ** grille, int N){
    // initialisation d'une séquence aléatoire

    *grille = (struct grille *) malloc(sizeof(struct grille));
    (*grille)->id = 1;
    (*grille)->energie = 0;
    (*grille)->size = N;

    int taille = 2 * N + 1;

    (*grille)->sigma = malloc( taille * taille * sizeof(int) );

    int rd;
    for (int i=0;i<taille;i++){
        for (int j=0;j<taille;j++){
            rd = rand() % 2;
            (*grille)->sigma[i*taille+j] = (rd==1) ? POS : NEG;
        }
    }
}

void init_grille_border(struct grille ** grille, int N, int PosOrNeg){
    // initialisation d'une séquence aléatoire
    int taille = 2 * N + 1;

    *grille = (struct grille *) malloc(sizeof(struct grille));
    (*grille)->id = 1;
    (*grille)->energie = 0;
    (*grille)->size = N;
    (*grille)->sigma = malloc( taille * taille * sizeof(int) );

    int rd;
    for (int i=0;i<taille;i++){
        for (int j=0;j<taille;j++){
            if (i == 0  || j == 0 || i == (taille -1) ||  j == (taille -1) ){
                (*grille)->sigma[i*taille+j] = PosOrNeg;
            } else {
                rd = rand() % 2;
                (*grille)->sigma[i*taille+j] = (rd==1) ? POS : NEG;
            }
        }
    }
}

void clear_grille(struct grille ** grille){
    // free le sigma ?
    free((*grille)->sigma);
    free(*grille);

}

void init_energy_grille(struct grille * grille, int energy){
    grille->energie=energy;
}

void voisin_grille(struct grille ** grille, int i, int j){
    // permute les signes <=> *(-1)
    int taille = 2 * (*grille)->size + 1;
    (*grille)->sigma[i*taille+j]*=-1;
    (*grille)->id++;
}

void print_grille(struct grille * grille){
    printf("Grille n°%d :\n",grille->id);
    int taille = 2 * grille->size + 1;
    for (int i=0;i<taille;i++){
        for (int j=0;j<taille;j++){
            if (grille->sigma[i*taille+j]==NEG){
                printf("\033[34;01m%2d\033[00m ",grille->sigma[i*taille+j]);
            }
            else {
                printf("\033[31m%2d\033[00m ", grille->sigma[i*taille+j]);
            }
        }
        printf("\n");
    }
    printf("Energie : %d\n",grille->energie);
}

int center_pos(struct grille * grille, int N){
    return (grille->sigma[N*(2*N+1)+N] == 1) ? 1 : 0;
}
