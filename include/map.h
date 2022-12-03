#ifndef MAP_H
#define MAP_H

#include "game.h"
#include "gameobject.h"

//Structure pour la carte
typedef struct MapTexture
{
    SDL_Texture* texture;
    char path[40];
} MapTexture;

typedef struct Map
{
    int column;
    int row;
    int nbTexture;

    MapTexture* mapTexture;
    int** grid; //Double pointeur pour la grille (lien ligne-colonne)
    int* data; //Pointeur simple pour valeur du tableau (valeur)

    SDL_Rect srcRect, destRect; //srcRect pour le rectangle qui regarde sur la texture, destRect pour le rendu ciblé (position & taille)
} Map;

/**
 * @brief Charger la carte et les éléments nécéssaire à son affichage
 * @param filename 
 * @param map 
 * @param game 
 * @return Map 
*/
Map initMapFromFile(const char* filename, Map *map, Game *game, ObjectList *objects);

/**
 * @brief Afficher la carte 
 * @param map Carte qui sera affichée
 * @param game Instance du jeu pour le rendu et la gestion des erreurs
*/
void renderMap(Map *map, Game *game);


/**
 * @brief Supprimer la carte - Supprime les textures liées à la carte
 * @param map Carte qui sera supprimée
*/
void destroyMap(Map *map);

/**
 * @brief Appel de la fonction SDL_RenderCopy() plus simple pour la carte
 * @param map Carte qui sera utilisé (récupérer les textures)
 * @param game Instance de jeu pour le rendu et la gestion des erreurs
 * @param tabIndex Index de la texture a afficher (tableau de texture de la carte)
*/
void renderCopyMap(Map *map, Game *game, int tabIndex);

#endif