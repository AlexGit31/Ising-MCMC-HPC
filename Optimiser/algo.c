#include "energie.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define NOMBRE_THREADS 20
#define NOMBRE_ITERATION_METROPOLIS 1000000

int nbPos=0;
int nbNeg=0;

pthread_mutex_t mutex;

struct metro{
    double Temp;
    struct grille * gri;
};

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

void * metropolis_hastings(void * args) {
    struct metro * temp = (struct metro *) args;
    double T = temp->Temp;
    int nb = NOMBRE_ITERATION_METROPOLIS;
    struct grille *grille = temp->gri;

    int i,j;
    int H_sigma = calculEnergieInitiale(grille);
    int Variation;

    if (NOMBRE_THREADS == 1) {
        // 1. Ouvrir le fichier de sauvegarde
        FILE *f = fopen("evolution.dat", "w");
        if (f == NULL) {
            printf("Erreur d'ouverture du fichier !\n");
            exit(1);
        }
        // 2. Sauvegarder la DIM et la grille initiale dans le fichier
        fprintf(f, "%d\n", grille->size * 2 + 1);

        for (int x = 0; x <  grille->size * 2 + 1; x++) {
            for (int y = 0; y <  grille->size * 2 + 1; y++) {
                fprintf(f, "%d ", grille->sigma[x * ( grille->size * 2 + 1) + y]);
            }
            fprintf(f, "\n");
        }
    }



    for (int n=0; n < nb; n ++ ) {
        // On doit choisir le couple (i,j) avec une proba de 1 / (2nb+1)**2
        // ce qui revient à tirer uniformément i avec une proba 1 / 2nb+1 et j avec proba 1/ 2nb+1

        i = ((double)rand() /  (RAND_MAX) ) * (grille->size); // nombre entre 0 et DIM-1
        j = ((double)rand() /  (RAND_MAX) ) * (grille->size);

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
            }
        }
        if (accepte == 1){
            H_sigma += Variation;

            if(NOMBRE_THREADS==1){
                // 3. Si le changement est conservé, on l'écrit dans le fichier
                    // On écrit juste les coordonnées du flip
                    FILE *f = fopen("evolution.dat", "a");
                    fprintf(f, "%d %d\n", i, j);
            }
        }
    }
    if (center_pos(grille,(grille->size-1)/2)==1) {
        pthread_mutex_lock(&mutex);
        nbPos++;
        printf("DIM : %2d,nbPos : %2d, nbNeg :%2d\n",grille->size*2+1,nbPos,nbNeg);
        pthread_mutex_unlock(&mutex);
    } else {
        pthread_mutex_lock(&mutex);
        nbNeg++;
        printf("DIM : %2d,nbPos : %2d, nbNeg :%2d\n",grille->size*2+1,nbPos,nbNeg);
        pthread_mutex_unlock(&mutex);
    }
    //print_grille(grille);
    clear_grille(&grille);
    free(args);
    return NULL;
}

// with_border = 1 => bordure positive
// with_border = -1 => bordure négative
// with_border = 0 => sans bordure

int main(int argc, char *argv[]) {
    printf("Début du main : \n");
    srand((unsigned int)time(NULL));

    printf("step1 :\n");
    int with_border = atoi(argv[1]);
    double T = atof(argv[2]);
    int N = atoi(argv[3]);

    printf("step2 :\n");

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    pthread_mutex_init(&mutex,NULL);
    pthread_t tid[NOMBRE_THREADS];

    printf("step3 - entrée dans la boucle while :\n");
    int i=0;

    while(i<NOMBRE_THREADS){
        // printf("i = %d\n", i);
        // initialisation d'une grille
        struct grille *p_grille;
        (with_border != 0) ? init_grille_border(&p_grille,N,with_border) : init_grille(&p_grille,N);

        // initialisation de la structure metro
        struct metro *temp = (struct metro *) malloc(sizeof(struct metro));
        temp->gri = p_grille;
        temp->Temp = T;
        pthread_create(&tid[i],&attr,metropolis_hastings,(void *)temp);
        i++;
    }
    pthread_exit(NULL);
    return 0;
}
