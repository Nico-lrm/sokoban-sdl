#include "../include/game.h"

int main(int argc, char **argv)
{
    //Structure pour limiter la boucle de jeu (et les images) à 60
    FPS fpsLimiter = {60, 1000/fpsLimiter.targetFPS, 0, 0};

    //"Instance" de jeu
    Game game;
    game.isPlaying = SDL_FALSE;
    game.isDead = SDL_FALSE;
    game.isWinner = SDL_FALSE;
    game.font = NULL;
    game.window = NULL;
    game.renderer = NULL;
    game.font = NULL;

    //Gestion du temps restant
    time_t now;
    time(&now);

    game.debut = *localtime(&now);
    game.fin = game.debut;
    game.fin.tm_min += 1;
    game.fin.tm_sec += 45;

    
    //Lancement de la SDL et création de la fenêtre et du rendu
    initGame("Sokoscape", SCREEN_WIDTH, SCREEN_HEIGHT, &game);

    

    //Boucle de jeu
    while(runningGame(&game))
    {
        //On récupère les ticks de départ
        fpsLimiter.frameStart = SDL_GetTicks();

        handleInputGame(&game);
        updateGame(&game);
        handleEventsGame(&game);
        if(game.isDead == SDL_TRUE)
            renderGameText(&game, "Game Over", 255, 0, 0);
        else if(game.isWinner == SDL_TRUE)
            renderGameText(&game, "Winner !", 0, 255, 0);
        else 
            renderGame(&game);

        //On récupère les ticks a chaque boucle
        fpsLimiter.frameTime = SDL_GetTicks() - fpsLimiter.frameStart;

        //On vérifie si la différence entre les ticks de départ et d'arrivée est inférieur au délai de l'écran
        if (fpsLimiter.frameDelay > fpsLimiter.frameTime)
        {
            //Si oui, on créer un délai pour limiter le nombre d'image
            SDL_Delay(fpsLimiter.frameDelay - fpsLimiter.frameTime);
        }
    }
    
    //Quitter le jeu et fermer la SDL
    closeGame(&game);
    return 0;
}