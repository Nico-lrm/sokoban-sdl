# Sokoban SDL
Jeu du sokoban, développé entièrement en C.

## Dépendances
- SDL : https://www.libsdl.org/
- SDL_image : https://github.com/libsdl-org/SDL_image/releases
- SDL_ttf : https://github.com/libsdl-org/SDL_ttf/releases

## Fonctionnalités

- Lecture des paramètres du jeu depuis un fichier
- Chronomètrage de la partie
- Collision AABB
- Affichage de textures

## Installation

La démo d'installation du programme sera faite sous Linux, il est nécessaire d'avoir le compilateur GCC.  
Tout d'abord, vous devez installer les dépendances nécessaires au fonctionnement du jeu.

```bash
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
```

Une fois les dépendances installées, vous pouvez cloner le dossier et vous positionner dans le dossier.
```bash
git clone git@github.com:Nico-lrm/sokoban-sdl.git
cd sokoban-sdl-main
```

Enfin, il vous suffit de compiler le programme grâce à la commande "make" et de lancer l'exécutable
```bash
make
./bin/prog
```
## Screenshot
![Screenshot de l'écran de jeu](https://github.com/Nico-lrm/sokoban-sdl/blob/main/assets/example/sokoscape.png)
