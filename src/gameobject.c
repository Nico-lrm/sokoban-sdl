#include "../include/gameobject.h"

ObjectList initObjectListFromFile(const char* filename, ObjectList *objects, Game *game)
{
    //Variables
    FILE *fichier = fopen(filename, "r");
    char buffer[101];
    char *sep = NULL;
    int count = 0;
    int width = 0, height = 0;

    //Initialisation à NULL des objets et du joueur
    objects->objectslist = NULL;
    objects->player = NULL;
    objects->numberObjects = 0;

    //Si le fichier n'est pas trouver, on renvoie une erreur et on quitte le jeu
    if(fichier == NULL)
    {
        getError("Fichier de jeu introuvable, impossible d'initialiser les objets.");
        closeGame(game);
    }

    //Parcours du fichier
    while(!feof(fichier))
    {
        if(fgets(buffer, 100, fichier) != NULL)
        {
            sep = strtok(buffer, ":");
        }
        if(sep != NULL)
        {
            if(strcmp("OBJET", sep) == 0)
            {   
                objects->numberObjects += 1;
                if(objects->objectslist == NULL)
                {
                    objects->objectslist = malloc(sizeof(GameObject) * objects->numberObjects);
                }
                else
                {
                    objects->objectslist = realloc(objects->objectslist, sizeof(GameObject) * objects->numberObjects);
                }

                sep = strtok(NULL, ",");
                count = atoi(sep);
                sep = strtok(NULL, ",");
                objects->objectslist[count].objectTexture = IMG_LoadTexture(game->renderer, sep);
                if(SDL_QueryTexture(objects->objectslist[count].objectTexture, NULL, NULL, &width, &height) != 0)
                {
                    getError("\0");
                    closeGame(game);
                }
                sep = strtok(NULL, ",");
                objects->objectslist[count].xposition = atoi(sep);
                sep = strtok(NULL, "\n");
                objects->objectslist[count].yposition = atoi(sep);

                //Taille de l'objet
                objects->objectslist[count].destRect.w = (TILESIZE - ((TILESIZE - (TILESIZE/4))/2));
                objects->objectslist[count].destRect.h = (TILESIZE - ((TILESIZE - (TILESIZE/4))/2));

                objects->objectslist[count].speed = 0;
                objects->objectslist[count].angle = 0.;
                objects->objectslist[count].isActive = SDL_TRUE;
            }
            else if(strcmp("JOUEUR", sep) == 0)
            {
                objects->player = malloc(sizeof(GameObject));
                sep = strtok(NULL, ",");
                (objects->player)->objectTexture = IMG_LoadTexture(game->renderer, sep);

                //Coordonnées <x,y> du joueur
                sep = strtok(NULL, ",");
                (objects->player)->xposition = atoi(sep);
                sep = strtok(NULL, "\n");
                (objects->player)->yposition = atoi(sep);

                //Taille du joueur = Taille de la texture de carte/2
                (objects->player)->destRect.w = (TILESIZE/2);
                (objects->player)->destRect.h = (TILESIZE/2);

                objects->player->speed = 2.;
                objects->player->angle = 0.;

                objects->player->velocity_x = 0;
                objects->player->velocity_y = 0;
                objects->objectslist->isActive = SDL_TRUE;
            }
        }
    }

    //On libère les éléments à présent inutile et on ferme le fichier
    fclose(fichier);

    return *objects;
}

void updateObjectList(ObjectList *objects)
{   
    //Si les différents objets sont initialisés, on peut mettre à jour leur positions
    if(objects->numberObjects > 0)
    {
        for (int i = 0; i < objects->numberObjects; i++)
        {
            objects->objectslist[i].destRect.x = objects->objectslist[i].xposition;
            objects->objectslist[i].destRect.y = objects->objectslist[i].yposition;
        }
    }
    if(objects->player != NULL)
    {
        objects->player->destRect.x = objects->player->xposition;
        objects->player->destRect.y = objects->player->yposition;
    }
}

void renderObjectList(ObjectList *objects, Game *game)
{
    //Si les différents objets sont initialisés, on peut les afficher
    if(objects->numberObjects > 0)
    {
        for (int i = 0; i < objects->numberObjects; i++)
        {
            if(SDL_RenderCopy(game->renderer, objects->objectslist[i].objectTexture, NULL, &(objects->objectslist[i].destRect)) < 0)
            {
                getError("\0");
                closeGame(game);
            }
        }
    }
    if(objects->player != NULL)
    {
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if(SDL_RenderCopyEx(game->renderer, objects->player->objectTexture, NULL, &(objects->player->destRect), objects->player->angle, NULL, flip) < 0)
        {
            getError("\0");
            closeGame(game);
        }
    }

}

void destroyObjectList(ObjectList *objects)
{
    if(objects->numberObjects > 0)
    {
        for (int i = 0; i < objects->numberObjects; i++)
        {
            if(objects->objectslist[i].objectTexture != NULL)
            {
                SDL_DestroyTexture(objects->objectslist[i].objectTexture);
            }
        }
        free(objects->objectslist);
        objects->objectslist = NULL;
        objects->numberObjects = 0;
    }
    if(objects->player != NULL)
    {
        if(objects->player->objectTexture != NULL)
        {
            SDL_DestroyTexture(objects->player->objectTexture);
        }
        free(objects->player);
        objects->player = NULL;
    }
    if(objects->numberStaticObjects > 0)
    {
        free(objects->staticObjectsList);
        objects->staticObjectsList = NULL;
        objects->numberStaticObjects = 0;
    }
}

//Gestion des collisions pendant le déplacement
void willHaveCollisionObjects(ObjectList *objects, int direction, Game* game)
{
    SDL_Rect tempRectPlayer;
    tempRectPlayer.x = objects->player->xposition;
    tempRectPlayer.y = objects->player->yposition;
    tempRectPlayer.w = objects->player->destRect.w;
    tempRectPlayer.h = objects->player->destRect.h;

    SDL_Rect tempRectObject;

    //Collision Joueur-Caisse et Caisse-Mur
    for (int i = 0; i < objects->numberObjects; i++)
    {
        tempRectObject.x = objects->objectslist[i].xposition;
        tempRectObject.y = objects->objectslist[i].yposition;
        tempRectObject.w = objects->objectslist[i].destRect.w;
        tempRectObject.h = objects->objectslist[i].destRect.h;
        if(SDL_HasIntersection(&tempRectPlayer, &(objects->objectslist[i].destRect)) == SDL_TRUE && objects->objectslist[i].isActive == SDL_TRUE)
        {
            switch (direction)
            {
                case 1:
                    objects->objectslist[i].yposition -= objects->player->speed;
                    tempRectObject.y -= objects->player->speed;
                    break;
                case 2:
                    objects->objectslist[i].yposition += objects->player->speed;
                    tempRectObject.y += objects->player->speed;
                    break;
                case 3:
                    objects->objectslist[i].xposition -= objects->player->speed;
                    tempRectObject.x -= objects->player->speed;
                    break;
                case 4:
                    objects->objectslist[i].xposition += objects->player->speed;
                    tempRectObject.x += objects->player->speed;
                    break;            
            }

            for (int j = 0; j < objects->numberStaticObjects; j++)
            {
                if(SDL_HasIntersection(&tempRectObject, &(objects->staticObjectsList[j].staticDestRect)) == SDL_TRUE && objects->staticObjectsList[j].type == MUR)
                {   
                    objects->objectslist[i].yposition = objects->objectslist[i].destRect.y;
                    objects->player->yposition = objects->player->destRect.y;
                    objects->objectslist[i].xposition = objects->objectslist[i].destRect.x;
                    objects->player->xposition = objects->player->destRect.x;
                }
            }

            //Collision Caisse-Flaque
            for (int j = 0; j < objects->numberStaticObjects; j++)
            {
                if(objects->staticObjectsList[j].type == FLAQUE)
                {
                    if(SDL_HasIntersection(&tempRectObject, &(objects->staticObjectsList[j].staticDestRect)) == SDL_TRUE)
                    {
                        objects->player->isActive = SDL_FALSE;
                        objects->player->velocity_x = 0;
                        objects->player->velocity_y = 0;
                        int isSlipping = 1;
                        while(isSlipping == 1)
                        {
                            tempRectObject.y = objects->objectslist[i].destRect.y;
                            tempRectObject.x = objects->objectslist[i].destRect.x;
                            for (int k = 0; k < objects->numberStaticObjects; k++)
                            {
                                if(SDL_HasIntersection(&tempRectObject, &(objects->staticObjectsList[k].staticDestRect)) == SDL_TRUE && objects->staticObjectsList[k].type != FLAQUE)
                                {
                                    isSlipping = 0;
                                }
                            }
                            switch (direction)
                            {
                                case 1:
                                    tempRectObject.y -= objects->player->speed;
                                    break;
                                case 2:
                                    tempRectObject.y += objects->player->speed;
                                    break;
                                case 3:
                                    tempRectObject.x -= objects->player->speed;
                                    break;
                                case 4:
                                    tempRectObject.x += objects->player->speed;
                                    break;            
                            }
                            objects->objectslist[i].xposition = tempRectObject.x;
                            objects->objectslist[i].yposition = tempRectObject.y;       

                            //Mise à jour
                            updateGame(game);
                            renderGame(game);
                        }
                    }
                    for (int k = 0; k < objects->numberStaticObjects; k++)
                    {
                        if(SDL_HasIntersection(&(objects->objectslist[i].destRect), &(objects->staticObjectsList[k].staticDestRect)) == SDL_TRUE && objects->staticObjectsList[k].type != FLAQUE && objects->staticObjectsList[k].type != TROU && objects->staticObjectsList[k].type != FEU)
                        {
                            switch (direction)
                            {
                                case 1:
                                    tempRectObject.y += objects->player->speed;
                                    break;
                                case 2:
                                    tempRectObject.y -= objects->player->speed;
                                    break;
                                case 3:
                                    tempRectObject.x += objects->player->speed;
                                    break;
                                case 4:
                                    tempRectObject.x -= objects->player->speed;
                                    break;            
                            }
                            objects->objectslist[i].xposition = tempRectObject.x;
                            objects->objectslist[i].yposition = tempRectObject.y;       

                            //Mise à jour
                            updateGame(game);
                            renderGame(game);
                        }
                    } 
                    objects->player->isActive = SDL_TRUE; 
                }  
            }
            
        }
    }

    //Collision Joueur-Flaque
    for(int i = 0; i < objects->numberStaticObjects; i++)
    {
        if(objects->staticObjectsList[i].type == FLAQUE)
        {
            if(SDL_HasIntersection(&tempRectPlayer, &(objects->staticObjectsList[i].staticDestRect)) == SDL_TRUE)
            {
                objects->player->isActive = SDL_FALSE;
                objects->player->velocity_x = 0;
                objects->player->velocity_y = 0;
                int isSlipping = 1;
                while(isSlipping == 1)
                {
                    tempRectPlayer.y = objects->player->destRect.y;
                    tempRectPlayer.x = objects->player->destRect.x;
                    for (int k = 0; k < objects->numberStaticObjects; k++)
                    {
                        if(SDL_HasIntersection(&tempRectPlayer, &(objects->staticObjectsList[k].staticDestRect)) == SDL_TRUE && objects->staticObjectsList[k].type != FLAQUE && objects->staticObjectsList[k].isActive == SDL_TRUE)
                        {
                            isSlipping = 0;
                        }
                    }
                    for (int k = 0; k < objects->numberObjects; k++)
                    {
                        if(SDL_HasIntersection(&tempRectPlayer, &(objects->objectslist[k].destRect)) == SDL_TRUE && objects->objectslist[k].isActive == SDL_TRUE)
                        {
                            isSlipping = 0;
                        }
                    }
                    switch (direction)
                    {
                        case 1:
                            tempRectPlayer.y -= objects->player->speed;
                            break;
                        case 2:
                            tempRectPlayer.y += objects->player->speed;
                            break;
                        case 3:
                            tempRectPlayer.x -= objects->player->speed;
                            break;
                        case 4:
                            tempRectPlayer.x += objects->player->speed;
                            break;            
                    }
                    objects->player->xposition = tempRectPlayer.x;
                    objects->player->yposition = tempRectPlayer.y;       

                    //Mise à jour
                    updateGame(game);
                    renderGame(game);
                }
                objects->player->isActive = SDL_TRUE;
                switch (direction)
                {
                    case 1:
                        tempRectPlayer.y += objects->player->speed*2;
                        break;
                    case 2:
                        tempRectPlayer.y -= objects->player->speed*2;
                        break;
                    case 3:
                        tempRectPlayer.x += objects->player->speed*2;
                        break;
                    case 4:
                        tempRectPlayer.x -= objects->player->speed*2;
                        break;            
                }
                objects->player->xposition = tempRectPlayer.x;
                objects->player->yposition = tempRectPlayer.y;       

                //Mise à jour
                updateGame(game);
            }
        }
    }

    //Collision Joueur-Mur
    for(int i = 0; i < objects->numberStaticObjects; i++)
    {
        if(objects->staticObjectsList[i].type == MUR)
        {
            if(SDL_HasIntersection(&tempRectPlayer, &(objects->staticObjectsList[i].staticDestRect)) == SDL_TRUE && objects->staticObjectsList[i].type == MUR)
            {
                objects->player->yposition = objects->player->destRect.y;
                objects->player->xposition = objects->player->destRect.x;
            }
        }
    }
}

//Gestion des events post-déplacement
void collisionEventObject(ObjectList* objects, Game* game)
{
    //Si le temps imparti est écoulé
    if(difftime(mktime(&(game->fin)), mktime(&(game->debut))) == 0)
    {
            objects->player->xposition = 0;
            objects->player->yposition = 0;
            objects->player->speed = 0;
            SDL_Delay(250);
            game->isDead = SDL_TRUE;
    }
    //Game Over & Victoire
    for (int i = 0; i < objects->numberStaticObjects; i++)
    {
        if(SDL_HasIntersection(&(objects->player->destRect),&(objects->staticObjectsList[i].staticDestRect)) == SDL_TRUE && objects->staticObjectsList[i].isActive == SDL_TRUE && (objects->staticObjectsList[i].type == TROU || objects->staticObjectsList[i].type == FEU))
        {   
            objects->player->xposition = 0;
            objects->player->yposition = 0;
            objects->player->speed = 0;
            SDL_Delay(250);
            game->isDead = SDL_TRUE;
        }
        else if(SDL_HasIntersection(&(objects->player->destRect),&(objects->staticObjectsList[i].staticDestRect)) == SDL_TRUE && objects->staticObjectsList[i].isActive == SDL_TRUE && objects->staticObjectsList[i].type == SORTIE)
        {
            objects->player->xposition = 0;
            objects->player->yposition = 0;
            objects->player->speed = 0;
            SDL_Delay(250);
            game->isWinner = SDL_TRUE;
        }
    }

    //Comparaison entre caisse-trou & caisse-flamme
    for (int i = 0; i < objects->numberObjects; i++)
    {
        for (int j = 0; j < objects->numberStaticObjects; j++)
        {
            if(SDL_HasIntersection(&(objects->objectslist[i].destRect), &(objects->staticObjectsList[j].staticDestRect)) == SDL_TRUE && objects->staticObjectsList[j].isActive == SDL_TRUE && objects->objectslist[i].isActive == SDL_TRUE && objects->staticObjectsList[j].type == TROU)
            {
                objects->objectslist[i].xposition = objects->staticObjectsList[j].staticDestRect.x;
                objects->objectslist[i].yposition = objects->staticObjectsList[j].staticDestRect.y;

                objects->objectslist[i].destRect.w = objects->staticObjectsList[j].staticDestRect.w;
                objects->objectslist[i].destRect.h = objects->staticObjectsList[j].staticDestRect.h;

                objects->objectslist[i].isActive = SDL_FALSE;
                objects->staticObjectsList[j].isActive = SDL_FALSE;
            }
            else if(SDL_HasIntersection(&(objects->objectslist[i].destRect), &(objects->staticObjectsList[j].staticDestRect)) == SDL_TRUE && objects->staticObjectsList[j].isActive == SDL_TRUE && objects->objectslist[i].isActive == SDL_TRUE && objects->staticObjectsList[j].type == FEU)
            {
                objects->objectslist[i].xposition = 0;
                objects->objectslist[i].yposition = 0;

                objects->objectslist[i].destRect.w = 0;
                objects->objectslist[i].destRect.h = 0;
                objects->objectslist[i].isActive = SDL_FALSE;
            }
        }   
    }
}


//Création des murs et statiques avec redécoupage du rectangle (pour s'adapter correctement au mur)
void createStaticCollision(const char* path, int row, int column, ObjectList* objects)
{
    objects->numberStaticObjects++;

    if(objects->staticObjectsList == NULL)
        objects->staticObjectsList = malloc(sizeof(StaticObjects));
    else 
        objects->staticObjectsList = realloc(objects->staticObjectsList, sizeof(StaticObjects) * objects->numberStaticObjects);

    //On défini en premier le type du collider a MUR
    objects->staticObjectsList[objects->numberStaticObjects-1].type = MUR;
    objects->staticObjectsList[objects->numberStaticObjects-1].isActive = SDL_TRUE;

    //Coins
    if(strcmp("assets/img/coin_bd.png", path) == 0)
    {
        objects->numberStaticObjects++;
        objects->staticObjectsList = realloc(objects->staticObjectsList, sizeof(StaticObjects) * objects->numberStaticObjects);
        objects->staticObjectsList[objects->numberStaticObjects-1].type = MUR;
        objects->staticObjectsList[objects->numberStaticObjects-1].isActive = SDL_TRUE;
        //Rectangle 1 (large)
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = (TILESIZE*row)+(TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column;

        //Rectangle 2 (fin)
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.w = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.h = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.x = (TILESIZE*column)+(TILESIZE/4);
    }
    else if(strcmp("assets/img/coin_bg.png", path) == 0)
    {
        objects->numberStaticObjects++;
        objects->staticObjectsList = realloc(objects->staticObjectsList, sizeof(StaticObjects) * objects->numberStaticObjects);
        objects->staticObjectsList[objects->numberStaticObjects-1].type = MUR;
        objects->staticObjectsList[objects->numberStaticObjects-1].isActive = SDL_TRUE;
        //Rectangle 1 (large)
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = (TILESIZE*row)+(TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column;

        //Rectangle 2 (fin)
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.w = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.h = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.x = (TILESIZE*column);
    }
    else if(strcmp("assets/img/coin_hd.png", path) == 0)
    {
        objects->numberStaticObjects++;
        objects->staticObjectsList = realloc(objects->staticObjectsList, sizeof(StaticObjects) * objects->numberStaticObjects);
        objects->staticObjectsList[objects->numberStaticObjects-1].type = MUR;
        objects->staticObjectsList[objects->numberStaticObjects-1].isActive = SDL_TRUE;
        //Rectangle 1 (large)
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column;

        //Rectangle 2 (fin)
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.w = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.h = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.y = (TILESIZE*row)+(TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.x = (TILESIZE*column)+(TILESIZE/4);
    }

    else if(strcmp("assets/img/coin_hg.png", path) == 0)
    {
        objects->numberStaticObjects++;
        objects->staticObjectsList = realloc(objects->staticObjectsList, sizeof(StaticObjects) * objects->numberStaticObjects);
        objects->staticObjectsList[objects->numberStaticObjects-1].type = MUR;
        objects->staticObjectsList[objects->numberStaticObjects-1].isActive = SDL_TRUE;
        //Rectangle 1 (large)
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column;

        //Rectangle 2 (fin)
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.w = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.h = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.y = TILESIZE*row+(TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.x = TILESIZE*column;
    }


    //Angles
    else if(strcmp("assets/img/mur_angle_db.png", path) == 0)
    {
        objects->numberStaticObjects++;
        objects->staticObjectsList = realloc(objects->staticObjectsList, sizeof(StaticObjects) * objects->numberStaticObjects);
        objects->staticObjectsList[objects->numberStaticObjects-1].type = MUR;
        objects->staticObjectsList[objects->numberStaticObjects-1].isActive = SDL_TRUE;

        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE - ((TILESIZE - (TILESIZE/4))/2));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = (TILESIZE*row)+((TILESIZE - (TILESIZE/4))/2);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = (TILESIZE*column)+((TILESIZE - (TILESIZE/4))/2);

        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.w = ((TILESIZE - (TILESIZE/4))/2);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.h = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.y = (TILESIZE*row)+((TILESIZE - (TILESIZE/4))/2);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.x = (TILESIZE*column)+(TILESIZE - ((TILESIZE - (TILESIZE/4))/2));
    }
    else if(strcmp("assets/img/mur_angle_dh.png", path) == 0)
    {
        objects->numberStaticObjects++;
        objects->staticObjectsList = realloc(objects->staticObjectsList, sizeof(StaticObjects) * objects->numberStaticObjects);
        objects->staticObjectsList[objects->numberStaticObjects-1].type = MUR;
        objects->staticObjectsList[objects->numberStaticObjects-1].isActive = SDL_TRUE;

        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = ((TILESIZE - (TILESIZE/4))/2);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = (TILESIZE*column)+((TILESIZE - (TILESIZE/4))/2);

        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.w = (TILESIZE - ((TILESIZE - (TILESIZE/4))/2));
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.h = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.y = (TILESIZE*row)+((TILESIZE - (TILESIZE/4))/2);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.x = (TILESIZE*column)+((TILESIZE - (TILESIZE/4))/2);
    }

    else if(strcmp("assets/img/mur_angle_gb.png", path) == 0)
    {
        objects->numberStaticObjects++;
        objects->staticObjectsList = realloc(objects->staticObjectsList, sizeof(StaticObjects) * objects->numberStaticObjects);
        objects->staticObjectsList[objects->numberStaticObjects-1].type = MUR;
        objects->staticObjectsList[objects->numberStaticObjects-1].isActive = SDL_TRUE;
    
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = (TILESIZE - ((TILESIZE - (TILESIZE/4))/2));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = (TILESIZE*row)+((TILESIZE - (TILESIZE/4))/2);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column;

        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.w = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.h = ((TILESIZE - (TILESIZE/4))/2);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.y = (TILESIZE*row)+(TILESIZE - ((TILESIZE - (TILESIZE/4))/2));
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.x = (TILESIZE*column)+(TILESIZE/4);
    }

    else if(strcmp("assets/img/mur_angle_gh.png", path) == 0)
    {
        objects->numberStaticObjects++;
        objects->staticObjectsList = realloc(objects->staticObjectsList, sizeof(StaticObjects) * objects->numberStaticObjects);
        objects->staticObjectsList[objects->numberStaticObjects-1].type = MUR;
        objects->staticObjectsList[objects->numberStaticObjects-1].isActive = SDL_TRUE;
    
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE - ((TILESIZE - (TILESIZE/4))/2));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column+((TILESIZE - (TILESIZE/4))/2);

        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.w = ((TILESIZE - (TILESIZE/4))/2);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.h = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.y = (TILESIZE*row)+((TILESIZE - (TILESIZE/4))/2);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.x = (TILESIZE*column);
    }

    //Murs standards
    else if(strcmp("assets/img/mur_bas.png", path) == 0)
    {
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE/2)+((TILESIZE - (TILESIZE/4))/2/2);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row+((TILESIZE/2)-((TILESIZE - (TILESIZE/4))/2/2));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column;
    }

    else if(strcmp("assets/img/mur_droit.png", path) == 0)
    {
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column+(TILESIZE/4);
    }

    else if(strcmp("assets/img/mur_gauche.png", path) == 0)
    {
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column;
    }

    else if(strcmp("assets/img/mur_haut.png", path) == 0)
    {
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE/2)+((TILESIZE - (TILESIZE/4))/2/2);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column;
    }

    //Murs fins droits
    else if(strcmp("assets/img/mur_gd.png", path) == 0)
    {
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = (TILESIZE*row)+((TILESIZE - (TILESIZE/4))/2);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column;
    }

    else if(strcmp("assets/img/mur_hb.png", path) == 0)
    {
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = (TILESIZE*column)+((TILESIZE - (TILESIZE/4))/2);
    }


    //Sorties
    else if(strcmp("assets/img/sortie_droite.png", path) == 0)
    {
        objects->staticObjectsList[objects->numberStaticObjects-1].type = SORTIE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = (TILESIZE - (TILESIZE/4))/2;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column+(TILESIZE/4)+(TILESIZE/2);
    }

    //Mur avec départ de mur fin
    else if(strcmp("assets/img/mid_bas.png", path) == 0)
    {
        objects->numberStaticObjects++;
        objects->staticObjectsList = realloc(objects->staticObjectsList, sizeof(StaticObjects) * objects->numberStaticObjects);
        objects->staticObjectsList[objects->numberStaticObjects-1].type = MUR;
        objects->staticObjectsList[objects->numberStaticObjects-1].isActive = SDL_TRUE;
        //Mur large
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = (TILESIZE*row)+(TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column;

        //Depart de mur
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.w = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.h = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.x = TILESIZE*column+((TILESIZE - (TILESIZE/4))/2);
    }

    else if(strcmp("assets/img/mid_gauche.png", path) == 0)
    {
        objects->numberStaticObjects++;
        objects->staticObjectsList = realloc(objects->staticObjectsList, sizeof(StaticObjects) * objects->numberStaticObjects);
        objects->staticObjectsList[objects->numberStaticObjects-1].type = MUR;
        objects->staticObjectsList[objects->numberStaticObjects-1].isActive = SDL_TRUE;
        //Mur large
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column;

        //Depart de mur
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.w = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.h = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.y = TILESIZE*row+((TILESIZE - (TILESIZE/4))/2);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.x = TILESIZE*column+(TILESIZE - (TILESIZE/4));
    }

    else if(strcmp("assets/img/mid_haut.png", path) == 0)
    {
        objects->numberStaticObjects++;
        objects->staticObjectsList = realloc(objects->staticObjectsList, sizeof(StaticObjects) * objects->numberStaticObjects);
        objects->staticObjectsList[objects->numberStaticObjects-1].type = MUR;
        objects->staticObjectsList[objects->numberStaticObjects-1].isActive = SDL_TRUE;
        //Mur large
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column;

        //Depart de mur
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.w = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.h = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.y = TILESIZE*row+(TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.x = TILESIZE*column+28;
    }

    else if(strcmp("assets/img/mid_droit.png", path) == 0)
    {
        objects->numberStaticObjects++;
        objects->staticObjectsList = realloc(objects->staticObjectsList, sizeof(StaticObjects) * objects->numberStaticObjects);
        objects->staticObjectsList[objects->numberStaticObjects-1].isActive = SDL_TRUE;
        //Mur large 
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column+(TILESIZE/4);

        //Depart de mur
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.w = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.h = (TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.y = TILESIZE*row+((TILESIZE - (TILESIZE/4))/2);
        objects->staticObjectsList[objects->numberStaticObjects-2].staticDestRect.x = TILESIZE*column;
    }

    //Objets statique
    else if(strcmp("assets/img/trou.png", path) == 0)
    {
        objects->staticObjectsList[objects->numberStaticObjects-1].type = TROU;

        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE - (TILESIZE/4));
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row+(TILESIZE/8);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column+(TILESIZE/8);
    }

    else if(strcmp("assets/img/flaque.png", path) == 0)
    {
        objects->staticObjectsList[objects->numberStaticObjects-1].type = FLAQUE;

        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = TILESIZE;
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = (TILESIZE/2);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row+(TILESIZE/4);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column;
    }

    else if(strcmp("assets/img/feu.png", path) == 0)
    {
        objects->staticObjectsList[objects->numberStaticObjects-1].type = FEU;

        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.w = TILESIZE - (((TILESIZE - (TILESIZE/4))/2)/2);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.h = TILESIZE - (((TILESIZE - (TILESIZE/4))/2)/2);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.y = TILESIZE*row+((TILESIZE - (TILESIZE - (((TILESIZE - (TILESIZE/4))/2)/2)))/2);
        objects->staticObjectsList[objects->numberStaticObjects-1].staticDestRect.x = TILESIZE*column+((TILESIZE - (TILESIZE - (((TILESIZE - (TILESIZE/4))/2)/2)))/2);
    }
}