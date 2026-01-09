%%writefile ising_save.cu

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curand_kernel.h>

// Macro de diagnostic d'erreur
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true) {
   if (code != cudaSuccess) {
      fprintf(stderr,"GPU Error: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

#define N_SIM 16384     // Réduit pour éviter le timeout
#define SIMS_PER_BATCH 8192  // Nombre de simulations par lancement de kernel
#define THREADS_PER_BLOCK 256
#define NbFor 250000000    // Réduit pour test initial
#define N 100            // Taille de grille plus raisonnable pour test
#define DIM (2*N+1)
#define POS 1
#define NEG -1
#define T 3.0f         // Température critique réelle de l'Ising 2D

__device__ void voisin_grille(int *spins, int i, int j) {
    spins[i * DIM + j] *= -1;
}

// Initialisation utilisant l'état RNG déjà créé
__device__ void init_grille(int *spins, curandState *state) {
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            spins[i * DIM + j] = (curand(state) & 1) ? POS : NEG;
        }
    }
}

// Initialisation avec les bords fixés à une valeur spécifique
// val_bord doit être POS ou NEG
__device__ void init_grille_bords_fixes(int *spins, curandState *state, int val_bord) {
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            int idx = i * DIM + j;

            // Vérification si on est sur un bord
            if (i == 0 || i == DIM - 1 || j == 0 || j == DIM - 1) {
                spins[idx] = val_bord;
            }
            else {
                // L'intérieur reste aléatoire (ou vous pouvez le mettre à 0)
                spins[idx] = (curand(state) & 1) ? POS : NEG;
            }
        }
    }
}

__device__ int calculVariationVoisin(int *spins, int i, int j) {
    int s = spins[i * DIM + j]; // Spin après le flip
    int voisins = 0;
    if (i > 0)       voisins += spins[(i-1) * DIM + j];
    if (i < DIM - 1) voisins += spins[(i+1) * DIM + j];
    if (j > 0)       voisins += spins[i * DIM + (j-1)];
    if (j < DIM - 1) voisins += spins[i * DIM + (j+1)];
    // DeltaH = 2 * s_nouveau * somme_voisins
    return 2 * s * voisins;
}

// Modifiez le kernel pour fixer réellement les bords
__global__ void metropolis_hastings(int *d_spins, int *count_pos, int *count_neg, unsigned long long seed) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= N_SIM) return;

    int *spins = &d_spins[tid * DIM * DIM];
    curandState state;
    // Ajout du tid et du seed pour garantir l'unicité
    curand_init(seed + tid, tid, 0, &state);

    // 1. Initialisation forcée des bords à POS
    init_grille_bords_fixes(spins, &state, POS);

    for (int n = 0; n < NbFor; n++) {
        // CORRECTION : On ne tire que des indices entre 1 et DIM-2 (on ignore les bords)
        int i = 1 + (curand(&state) % (DIM - 2));
        int j = 1 + (curand(&state) % (DIM - 2));

        // Calcul de la variation sans modifier le spin tout de suite
        int s_old = spins[i * DIM + j];

        // Somme des voisins
        int voisins = spins[(i-1) * DIM + j] + spins[(i+1) * DIM + j] +
                      spins[i * DIM + (j-1)] + spins[i * DIM + (j+1)];

        // DeltaE = 2 * J * s_old * somme_voisins (avec J=1)
        int variation = 2 * s_old * voisins;

        if (variation <= 0 || curand_uniform(&state) < expf(-(float)variation / T)) {
            spins[i * DIM + j] = -s_old; // On accepte le flip
        }
    }

    if (spins[N * DIM + N] == POS) atomicAdd(count_pos, 1);
    else atomicAdd(count_neg, 1);
}

int main() {
    int *d_spins, *d_count_pos, *d_count_neg;
    int h_pos = 0, h_neg = 0;
    long long total_h_pos = 0, total_h_neg = 0;
    int batch_pos = 0, batch_neg = 0;

    size_t mem_size = (size_t)N_SIM * DIM * DIM * sizeof(int);
    printf("Allocation mémoire : %.2f MB\n", (float)mem_size / 1048576.0f);

    gpuErrchk(cudaMalloc(&d_spins, mem_size));
    gpuErrchk(cudaMalloc(&d_count_pos, sizeof(int)));
    gpuErrchk(cudaMalloc(&d_count_neg, sizeof(int)));

    gpuErrchk(cudaMemset(d_count_pos, 0, sizeof(int)));
    gpuErrchk(cudaMemset(d_count_neg, 0, sizeof(int)));

    int duration_minutes = 3;
    time_t start_time = time(NULL);
    int batch_count = 0;
    int blocks = (SIMS_PER_BATCH + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK;

    // NEW: Initialisation du fichier CSV
    const char* filename = "ising_data.csv";
    FILE *f = fopen(filename, "w");
    fprintf(f, "Batch,Time_sec,Batch_Pos,Batch_Neg,Total_Pos,Total_Neg,Expectation\n");
    fclose(f);

    printf("Début de la simulation intensive pour %d minutes...\n", duration_minutes);

    while (difftime(time(NULL), start_time) < duration_minutes * 60) {
        // On remet à zéro pour CHAQUE lot
        gpuErrchk(cudaMemset(d_count_pos, 0, sizeof(int)));
        gpuErrchk(cudaMemset(d_count_neg, 0, sizeof(int)));

        // On lance SIMS_PER_BATCH threads
        metropolis_hastings<<<blocks, THREADS_PER_BLOCK>>>(d_spins, d_count_pos, d_count_neg, (unsigned long long)time(NULL) + batch_count);

        cudaDeviceSynchronize();

        // On récupère le résultat du lot actuel
        gpuErrchk(cudaMemcpy(&batch_pos, d_count_pos, sizeof(int), cudaMemcpyDeviceToHost));
        gpuErrchk(cudaMemcpy(&batch_neg, d_count_neg, sizeof(int), cudaMemcpyDeviceToHost));

        // On AJOUTE au total global
        total_h_pos += batch_pos;
        total_h_neg += batch_neg;

        batch_count++;

        // Sauvegarde et affichage périodique
        if (batch_count % 5 == 0) {
            gpuErrchk(cudaMemcpy(&batch_pos, d_count_pos, sizeof(int), cudaMemcpyDeviceToHost));
            gpuErrchk(cudaMemcpy(&batch_neg, d_count_neg, sizeof(int), cudaMemcpyDeviceToHost));

            total_h_pos += batch_pos;
            total_h_neg += batch_neg;

            double elapsed = difftime(time(NULL), start_time);
            double expectation = (double)(total_h_pos - total_h_neg) / (total_h_pos + total_h_neg);

            // Écriture dans le CSV
            f = fopen(filename, "a");
            fprintf(f, "%d,%.1f,%d,%d,%lld,%lld,%f\n", batch_count, elapsed, batch_pos, batch_neg, total_h_pos, total_h_neg, expectation);
            fclose(f);

            printf("\rLot: %d | T: %.1fs | Espérance: %f", batch_count, elapsed, expectation);
            fflush(stdout);
        }
    }
    // Vérification d'erreurs au lancement
    gpuErrchk(cudaPeekAtLastError());
    gpuErrchk(cudaDeviceSynchronize());

    gpuErrchk(cudaMemcpy(&h_pos, d_count_pos, sizeof(int), cudaMemcpyDeviceToHost));
    gpuErrchk(cudaMemcpy(&h_neg, d_count_neg, sizeof(int), cudaMemcpyDeviceToHost));

    // AFFICHAGE FINAL CORRIGÉ
    long long total_sims = total_h_pos + total_h_neg;
    printf("\n--- Résultats Finaux ---\n");
    printf("Nombre total de simulations effectuées : %lld\n", total_sims);
    printf("Total Positifs : %lld, Total Négatifs : %lld\n", total_h_pos, total_h_neg);
    printf("Espérance globale du spin central : %f\n", (double)(total_h_pos - total_h_neg) / total_sims);

    cudaFree(d_spins); cudaFree(d_count_pos); cudaFree(d_count_neg);
    return 0;
}
