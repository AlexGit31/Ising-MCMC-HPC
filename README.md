# Simulation du Modèle d'Ising 2D : Algorithme de Metropolis-Hastings

Ce projet propose une étude numérique de la transition de phase dans le modèle d'Ising en deux dimensions. Il s'appuie sur l'algorithme de **Metropolis-Hastings** pour simuler l'évolution des spins et analyser la magnétisation en fonction de la température.

Le projet est implémenté avec deux approches de calcul haute performance :

1.  **CPU Multithreading** : Pour une exécution optimisée sur processeurs standards.
2.  **GPU CUDA** : Pour des simulations massives (jusqu'à 2 000 milliards de basculements).

---

📘 **Rapport LaTeX**
👉 [Lire le rapport en ligne](https://Alex31Git.github.io/nom-du-repo/Ising-MCMC-HPC/report.pdf)

## 📁 Structure du Projet

| Dossier / Fichier | Description                                                    |
| :---------------- | :------------------------------------------------------------- |
| `Optimiser/`      | Code C++ optimisé pour CPU (supporte le multithreading).       |
| `cuda/`           | Code source CUDA pour cartes graphiques NVIDIA.                |
| `plot/`           | Scripts de visualisation et de traçage des résultats.          |
| `compil.sh`       | Script d'automatisation de la compilation pour la version CPU. |

---

## 🛠️ Installation et Utilisation

### 1. Version CPU (Optimisée)

Le code CPU permet d'étudier le système sur des grilles de taille modérée avec une gestion efficace des ressources.

**Compilation :**

```bash
chmod +x compil.sh
./compil.sh

```

**Execution :**

```bash
# with_border = {1,-1,0} valeur pour les bordures d'une signes positif, négatif ou nulle.
# T température comprise entre 0 et +oo
# N dimension de la grille comme dans l'énoncé
./main with_border T N

```

### 2. Version Visuelle

```bash
./exec.sh
```

### 3. Version CUDA

- Utiliser sur google colab deux codes :
  - le fichier projetMAM.cu
  - le fichier exce.py
