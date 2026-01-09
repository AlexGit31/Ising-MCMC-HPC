import matplotlib.animation as animation
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.colors import ListedColormap

# --- Configuration ---
FILENAME = "evolution.dat"
INTERVAL = 1  # Vitesse de l'animation en ms (plus petit = plus rapide)
SKIP_STEPS = (
    1000  # Pour accélérer : on ne met à jour l'image que tous les X changements
)
# Si votre simulation a 100 millions d'étapes, mettez SKIP_STEPS à 1000 ou 10000 !


def load_data():
    with open(FILENAME, "r") as f:
        # Lire la dimension
        dim = int(f.readline())

        # Lire la grille initiale
        initial_grid = []
        for _ in range(dim):
            line = list(map(int, f.readline().split()))
            initial_grid.append(line)

        # Lire tous les changements
        changes = []
        for line in f:
            if line.strip():
                parts = list(map(int, line.split()))
                if len(parts) == 2:
                    changes.append((parts[0], parts[1]))

    return dim, np.array(initial_grid), changes


# Chargement des données
print("Chargement des données...")
dim, grid, changes = load_data()
print(f"Dimension: {dim}x{dim}")
print(f"Nombre total de changements acceptés: {len(changes)}")

# --- Visualisation ---

fig, ax = plt.subplots()

# Création de la colormap : -1 = Rouge, +1 = Vert
# On mappe les valeurs [-1, 1].
cmap = ListedColormap(["red", "green"])
mat = ax.imshow(grid, cmap=cmap, vmin=-1, vmax=1)
plt.colorbar(mat, ticks=[-1, 1], label="Spin")
ax.set_title("Modèle d'Ising - Évolution")


# Fonction d'animation
def update(frame_idx):
    # On applique un paquet de changements d'un coup pour aller plus vite
    start = frame_idx * SKIP_STEPS
    end = start + SKIP_STEPS

    # Sécurité pour ne pas dépasser la liste
    current_changes = changes[start:end]

    for i, j in current_changes:
        grid[i, j] *= -1  # On inverse le spin

    mat.set_data(grid)
    return [mat]


# Calcul du nombre de frames nécessaires
num_frames = len(changes) // SKIP_STEPS + 1

print("Génération de l'animation...")
ani = animation.FuncAnimation(
    fig, update, frames=num_frames, interval=INTERVAL, blit=True, repeat=False
)

plt.show()

# Pour sauvegarder en MP4 (nécessite ffmpeg) décommentez la ligne ci-dessous :
# ani.save("ising_video.mp4", writer="ffmpeg", fps=30)
