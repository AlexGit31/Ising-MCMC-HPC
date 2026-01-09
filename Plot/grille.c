#include "grille.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// energie initialisée à 0
void init_grille(struct grille ** grille, int id){
    // initialisation d'une séquence aléatoire
    //srand(time(NULL));

    *grille = (struct grille *) malloc(sizeof(struct grille));
    (*grille)->id = id;
    (*grille)->energie = 0;
    int rd;
    for (int i=0;i<DIM;i++){
        for (int j=0;j<DIM;j++){
            rd = rand() % 2;
            (*grille)->sigma[i][j] = (rd==1) ? POS : NEG;
        }
    }
}

void init_grille_border(struct grille ** grille, int id, int PosOrNeg){
    // initialisation d'une séquence aléatoire
    //srand(time(NULL));

    *grille = (struct grille *) malloc(sizeof(struct grille));
    (*grille)->id = id;
    (*grille)->energie = 0;
    int rd;
    for (int i=0;i<DIM;i++){
        for (int j=0;j<DIM;j++){
            if (i == 0  || j == 0 || i == DIM-1 ||  j == DIM-1){
                (*grille)->sigma[i][j] = PosOrNeg;
            } else {
                rd = rand() % 2;
                (*grille)->sigma[i][j] = (rd==1) ? POS : NEG;
            }
        }
    }
}

void clear_grille(struct grille ** grille){
    free(*grille);
}

void init_energy_grille(struct grille * grille, int energy){
    grille->energie=energy;
}

void voisin_grille(struct grille ** grille, int i, int j){
    // permute les signes <=> *(-1)
    (*grille)->sigma[i][j]*=-1;
    (*grille)->id++;
}

void print_grille(struct grille * grille){
    printf("Grille n°%d :\n",grille->id);
    for (int i=0;i<DIM;i++){
        for (int j=0;j<DIM;j++){
            if (grille->sigma[i][j]==NEG){
                printf("\033[34;01m%2d\033[00m ",grille->sigma[i][j]);
            }
            else {
                printf("\033[31m%2d\033[00m ", grille->sigma[i][j]);
            }
        }
        printf("\n");
    }
    printf("Energie : %d\n",grille->energie);
}

int center_pos(struct grille * grille){
    return (grille->sigma[N][N] == 1) ? 1 : 0;
}


