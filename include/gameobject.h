#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "game.h"

typedef enum {TROU, FEU, FLAQUE, MUR, SORTIE} staticObjectType;
//Structure pour "GameObject" (joueur, caisse, élément interactif)
typedef struct GameObject {
    float xposition;
    float yposition;
    
    SDL_Texture* objectTexture;
    SDL_Rect srcRect; //srcRect pour le rectangle qui regarde sur la texture
    SDL_Rect destRect; //destRect pour le rendu ciblé (position & taille)

    double angle;
    double speed;

    int velocity_x;
    int velocity_y;

    SDL_bool isActive;
} GameObject;

typedef struct StaticObjects {
    SDL_Rect staticDestRect;
    staticObjectType type;
    SDL_bool isActive;
} StaticObjects;

typedef struct ObjectList {
    int numberObjects;
    int numberStaticObjects;
    GameObject* player;
    GameObject* objectslist;
    StaticObjects* staticObjectsList;
} ObjectList;

/**
 * @brief Initialiser les objets du jeu à partir d'un fichier
 * @param filename Chemin du fichier à charger
 * @param objects Pointeur/Adresse d'un ObjectList
 * @param game Instance de jeu
 * @return ObjectList chargée
*/
ObjectList initObjectListFromFile(const char* filename, ObjectList* objects, Game* game);

/**
 * @brief Mettre à jour les objets de jeu (coordonnées, )
 * @param objects 
*/
void updateObjectList(ObjectList* objects);

/**
 * @brief Afficher les différents objets de jeu
 * @param objects Liste d'objets à afficher
 * @param game Instance de jeu
*/
void renderObjectList(ObjectList* objects, Game* game);

/**
 * @brief Supprimer les éléments de la liste de jeu
 * @param objects Liste d'objets à supprimer
*/
void destroyObjectList(ObjectList* objects);

/**
 * @brief Tester si un objet dynamique va avoir une collision avec un élément statique
 * @param objects Liste d'objets où l'on effectuera les test
 * @param direction Dernière direction de l'objet (1: Haut; 2: Bas; 3: Gauche; 4: Droite)
 * @param game Instance de jeu pour mettre à jour les positions (glissade des caisses)
*/
void willHaveCollisionObjects(ObjectList *objects, int direction, Game* game);

/**
 * @brief Gérer les events post-déplacement (et events liés à la victoire/défaite)
 * @param objects Liste d'objet à tester
 * @param game Instance de jeu pour récupérer le temps restant
*/
void collisionEventObject(ObjectList* objects, Game* game);

/**
 * @brief Créer les collisions statiques (flaque, feu, trou et mur)
 * 
 * @param path Chemin de la texture où créer les collisions (découpage)
 * @param row Ligne où placer le rectangle (row * TILESIZE)
 * @param column Colonne où placer le rectangle (column * TILESIZE)
 * @param objects Liste d'objets où sauvegarder les rectangles de collisions
*/
void createStaticCollision(const char* path, int row, int column, ObjectList* objects);

#endif 