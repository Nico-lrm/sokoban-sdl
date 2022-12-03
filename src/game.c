#include "../include/game.h"
#include "../include/gameobject.h"
#include "../include/map.h"

Map map;
ObjectList objects;

void initGame(const char *title, const int width, const int height, Game *game)
{
    //Initialisation des éléments de la SDL + fenêtre + rendu
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        getError("\0");
        return;
    }
    if(TTF_Init() != 0)
    {
        getError("\0");
        return;
    }
    game->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if(!(game->window))
    {
        getError("\0");
        return;
    }
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if(!(game->renderer))
    {
        getError("\0");
        return;
    }

    //Lancer la boucle et afficher le fond en noir
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    game->isPlaying = SDL_TRUE;

    //Initialiser les éléments du jeu
    map = initMapFromFile("assets/level/plateau.tab", &map, game, &objects);
    objects = initObjectListFromFile("assets/level/plateau.tab", &objects, game);
}

SDL_bool runningGame(Game *game)
{
    return game->isPlaying;
}

void handleInputGame(Game *game)
{
    if(objects.player->isActive == SDL_TRUE)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event) !=  0)
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    game->isPlaying = SDL_FALSE;
                break;
                case SDL_KEYDOWN:
                    //Switch sur les scancodes (compatibilité QWERTY & AZERTY)
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_ESCAPE:
                            game->isPlaying = SDL_FALSE;
                        break;
                        case SDL_SCANCODE_UP:
                        case SDL_SCANCODE_W:
                            objects.player->velocity_x = 0;
                            objects.player->velocity_y = -1;
                            objects.player->angle = 180;
                        break;
                        case SDL_SCANCODE_DOWN:
                        case SDL_SCANCODE_S:
                            objects.player->velocity_x = 0;
                            objects.player->velocity_y = 1;
                            objects.player->angle = 0;
                        break;
                        case SDL_SCANCODE_LEFT:
                        case SDL_SCANCODE_A:
                            objects.player->velocity_y = 0;
                            objects.player->velocity_x = -1;
                            objects.player->angle = 90;
                        break;
                        case SDL_SCANCODE_RIGHT:
                        case SDL_SCANCODE_D:
                            objects.player->velocity_y = 0;
                            objects.player->angle = 270;
                            objects.player->velocity_x = 1;
                        break;
                        default:
                        break;
                    }
                break;
                case SDL_KEYUP:
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_UP:
                        case SDL_SCANCODE_W:
                            objects.player->velocity_y = 0;
                        break;
                        case SDL_SCANCODE_DOWN:
                        case SDL_SCANCODE_S:
                            objects.player->velocity_y = 0;
                        break;
                        case SDL_SCANCODE_LEFT:
                        case SDL_SCANCODE_A:
                            objects.player->velocity_x = 0;
                        break;
                        case SDL_SCANCODE_RIGHT:
                        case SDL_SCANCODE_D:
                            objects.player->velocity_x = 0;
                        break;
                        default:
                        break;
                    }
                break;               
                default:
                break;
            }
        }
    }
}
void handleEventsGame(Game* game)
{
    collisionEventObject(&objects, game);
}

void updateGame(Game* game) 
{
    //On mets à jour la position du joueur en fonction de la vélocité (accélération) et de sa vitesse
    objects.player->yposition += objects.player->velocity_y * objects.player->speed;
    objects.player->xposition += objects.player->velocity_x * objects.player->speed;

    //Tester les collisions en fonction de la direction (vélocité d'un axe avec valeur -1 ou 1)
    if(objects.player->velocity_y == -1)
        willHaveCollisionObjects(&objects, 1, game);
    else if(objects.player->velocity_y == 1)
        willHaveCollisionObjects(&objects, 2, game);
    else if(objects.player->velocity_x == -1)
        willHaveCollisionObjects(&objects, 3, game);
    else if(objects.player->velocity_x == 1)
        willHaveCollisionObjects(&objects, 4, game);

    //Mise à jour des rectangles représentant les objets
    updateObjectList(&objects);
}

void renderGame(Game *game)
{
    //On nettoie d'abord l'écran de l'ancienne image
    if(SDL_RenderClear(game->renderer) < 0)
    {
        getError("\0");
        closeGame(game);
    }
    //On dessine les éléments à afficher
    renderMap(&map, game);
    renderTimer(game);
    renderObjectList(&objects, game);

    //On affiche l'image
    SDL_RenderPresent(game->renderer);
}

void renderTimer(Game *game)
{
    time_t redefine_time;
    time(&redefine_time);
    int seconds = 0;
    seconds = difftime(mktime(&(game->fin)), mktime(&(game->debut)));
    game->debut = *localtime(&redefine_time);

    char text[30];
    
    sprintf(text, "Temps restant : %d", seconds);

    SDL_Color color = { 255, 255, 255 };
    int width = 275;
    int height = 24;
    SDL_Rect rect = { SCREEN_WIDTH/2 - width/2, 15, width, height };

    game->font = TTF_OpenFont("assets/ttf/arcade_n.ttf", 25);

    SDL_Surface *surface = TTF_RenderText_Solid(game->font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);

    SDL_FreeSurface(surface);

    SDL_RenderCopy(game->renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(game->font);
}


void renderGameText(Game* game, const char* text, const int r, const int g, const int b)
{
    //On nettoie d'abord l'écran de l'ancienne image
    if(SDL_RenderClear(game->renderer) < 0)
    {
        getError("\0");
        closeGame(game);
    }
    SDL_Color color = { r, g, b };
    int width = 600;
    int height = 150;
    SDL_Rect rect = { (SCREEN_WIDTH/2)-(width/2), (SCREEN_HEIGHT/2)-(height/2), width, height };
    game->font = TTF_OpenFont("assets/ttf/arcade_n.ttf", 25);
    SDL_Surface *surface = TTF_RenderText_Solid(game->font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);

    SDL_FreeSurface(surface);
    SDL_RenderCopy(game->renderer, texture, NULL, &rect);
    SDL_RenderPresent(game->renderer);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(game->font);
}

void closeGame(Game *game)
{
    destroyObjectList(&objects);
    destroyMap(&map);
    
    //Destruction des éléments essentiels
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    TTF_Quit();
    SDL_Quit();
}

void getError(const char *error_msg)
{   
    //Fichier d'erreur
    FILE *fichier = fopen("error/errorlogs.txt", "a");
    if(fichier == NULL)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Erreur fichier 'errorlogs.txt'", "Erreur lors de la sauvegarde de l'erreur du programme: Impossible d'ouvrir le fichier 'errorlogs.txt'.", NULL);
        return;
    }
    else 
    {
        //Récupérer la date
        time_t t = time(NULL);
        struct tm *datetime = localtime(&t);

        //Ecriture dans le fichier
        fprintf(fichier, "### %02d-%02d-%04d @ %02d:%02d:%02d ###\n", datetime->tm_mday, datetime->tm_mon+1, datetime->tm_year+1900, datetime->tm_hour, datetime->tm_min, datetime->tm_sec);
        
        if(strcmp("\0", error_msg) != 0)
            fprintf(fichier, "ERREUR : %s\n\n", error_msg);
        else
            fprintf(fichier, "ERREUR SDL : %s\n\n", SDL_GetError());

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Erreur SDL", "Erreur lors de l'execution du programme: Consulter le fichier 'errorlogs.txt' dans le dossier error.", NULL);
        fclose(fichier);
    }
}