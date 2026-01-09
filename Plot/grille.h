#define N 100
#define DIM 2*N+1
#define POS 1
#define NEG -1

struct grille {
    int id;
    int sigma[DIM][DIM];
    int energie;
};

// constructeur de la matrice aléatoirement

extern void init_grille(struct grille **,int id);

extern void init_grille_border(struct grille **,int id, int PosOrNeg);

// destructeur

extern void clear_grille(struct grille **);

// assigne l'énergie une fois calculé

extern void init_energy_grille(struct grille *,int);

// modifier la grille par un voisin obtenu par ces coordonnées

extern void voisin_grille(struct grille **, int i, int j);

extern void print_grille(struct grille *);

extern int center_pos(struct grille *);

