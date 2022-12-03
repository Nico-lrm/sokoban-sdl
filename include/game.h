#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define TILESIZE 64
#define ROW 12
#define COLUMN 16
#define SCREEN_WIDTH COLUMN*TILESIZE //64*16
#define SCREEN_HEIGHT ROW*TILESIZE  //64*12

//Structure pour instance de jeu
typedef struct Game {
    SDL_bool isPlaying;
    SDL_bool isDead;
    SDL_bool isWinner;
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;

    struct tm debut;
    struct tm fin;
} Game;

//Structure pour limiter les FPS
typedef struct FPS {
    int targetFPS;
    double frameDelay; //Délai souhaité (targetFPS/1000)
    int frameStart; //Premier tick
    int frameTime;
} FPS; 

/**
 * @brief Initialise la fenêtre, le rendu et lance la boucle de jeu
 * @param title Le titre de la fenêtre
 * @param width La taille de la fenêtre en largeur
 * @param height La taille de la fenêtre en hauteur
 * @param game L'instance du jeu où on initialise les paramètres
*/
void initGame(const char *title, const int width, const int height, Game* game);

/**
 * @brief Renvoie le valeur booléenne du jeu : s'il est en cours ou non
 * @param game Le jeu où l'on va regarder s'il est lancé ou non
 * @return SDL_bool Valeur booléenne de boucle du jeu
*/
SDL_bool runningGame(Game* game);

/**
 * @brief Récupérer les inputs lorsque le jeu est lancé
 * @param game Le jeu où on gère les évènements
*/
void handleInputGame(Game* game);

/**
 * @brief Mise à jour des éléments du jeu et mise a jour du jeu lui même, on appelle les fonctions de mise à jour des objets dans celle-ci
*/
void updateGame(Game* game);

/**
 * @brief Gérer les évènements du jeu
*/
void handleEventsGame();

/**
 * @brief Affichage des éléments du jeu, on appelle les fonctions de mise à jour des objets dans celle-ci
 * @param game Le jeu à afficher
*/
void renderGame(Game* game);

/**
 * @brief Afficher un écran avec du texte et un fond noir
 * @param game Instance de jeu où récupérer la police et le renderer
 * @param text Texte à afficher
 * @param r Valeur de la couleur rouge du texte (0-255)
 * @param g Valeur de la couleur verte du texte (0-255)
 * @param b Valeur de la couleur bleu du texte (0-255)
*/
void renderGameText(Game* game, const char* text, const int r, const int g, const int b);

/**
 * @brief Mise à jour et affichage du timer
 * @param game Instance de jeu où récupérer les informations
*/
void renderTimer(Game *game);

/**
 * @brief Fermer la fenêtre et fermer la SDL
 * @param game Le jeu à fermer (contient la fenêtre et le rendu)
*/
void closeGame(Game* game);

/**
 * @brief Récupérer l'erreur et l'enregistrer dans "error/errorlogs.txt"
 * @param error_msg Si Erreur lié a la SDL, le message doit être égale à "\0", sinon écrire le message voulu à la place
 */
void getError(const char* error_msg);

#endif