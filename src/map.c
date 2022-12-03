#include "../include/map.h"

Map initMapFromFile(const char* filename, Map *map, Game *game, ObjectList *objects)
{
    //Variables
    FILE* fichier = fopen(filename, "r");
    char buffer[101];
    char *sep = NULL;
    int count = 0;
    int width_tex = 0, height_tex = 0;

    //Initialisation à NULL
    map->grid = NULL;
    map->mapTexture = NULL;
    map->data = NULL;

    objects->staticObjectsList = NULL;
    
    //Renvoyez une erreur si le fichier ne peut pas être trouvé
    if(fichier == NULL)
    {
        getError("Fichier de jeu introuvable, impossible d'initialiser la carte.");
        closeGame(game);
    }
    //On parcours le fichier
    while (!feof(fichier))
    {
        if(fgets(buffer, 100, fichier) != NULL)
        {
            sep = strtok(buffer, ":");
        }
        if(sep != NULL)
        {
            //Comparaison du label des lignes
            if(strcmp("LARGEUR", sep) == 0)
            {
                sep = strtok(NULL, "\n");
                map->column = atoi(sep);
            }
            else if(strcmp("HAUTEUR", sep) == 0)
            {
                sep = strtok(NULL, "\n");
                map->row = atoi(sep);
            }
            else if(strcmp("TUILE", sep) == 0)
            {
                //Créer le nouvelle emplacement mémoire pour la texture
                map->nbTexture += 1;
                if(map->mapTexture == NULL)
                {
                    map->mapTexture = (MapTexture*) malloc(sizeof(MapTexture));
                }
                else
                {
                    map->mapTexture = (MapTexture*) realloc(map->mapTexture, sizeof(MapTexture) * map->nbTexture);
                }

                //Récupération des infos (numéro du tableau + image)
                sep = strtok(NULL, ",");
                count = atoi(sep);
                sep = strtok(NULL, "\n");
                strcpy(map->mapTexture[count].path, sep);
                map->mapTexture[count].texture = IMG_LoadTexture(game->renderer, sep);
            }
            else if(strcmp("TAB", sep) == 0)
            {
                //Initialisation de la carte
                map->grid = (int **) malloc(sizeof(int*) * map->row);
                map->data = (int *) malloc(sizeof(int) * map->row * map->column);

                //Liaison grille - données
                for (int i = 0; i < map->row; i++)
                {
                    *((map->grid)+i) = map->data+(i*map->column);
                }
                //Lecture du fichier
                for (int i = 0; i < map->row; i++)
                {
                    while(strcmp(fgets(buffer, 100, fichier), "\n") == 0);
                    sep = strtok(buffer, " ");
                    map->grid[i][0] = atoi(sep);
                    if(map->grid[i][0] != 0)
                        createStaticCollision(map->mapTexture[map->grid[i][0]].path, i, 0, objects);
                    for (int j = 1; j < map->column; j++)
                    {
                        sep = strtok(NULL, " ");
                        map->grid[i][j] = atoi(sep);
                        if(map->grid[i][j] != 0)
                            createStaticCollision(map->mapTexture[map->grid[i][j]].path, i, j, objects);
                    }
                }
            }
        }
    }

    //Coordonnées de base et taille des éléments à afficher
    map->destRect.x = 0;
    map->destRect.y = 0;
    map->destRect.w = TILESIZE;
    map->destRect.h = TILESIZE;

    //Redimensionner la fenêtre (temporaire)
    SDL_SetWindowSize(game->window,  map->column*TILESIZE, map->row*TILESIZE);

    //Vider le contenu utilisé
    fclose(fichier);
    
    return *map;
}

void renderMap(Map *map, Game *game)
{
    int tile = 0;
    if(map->grid != NULL && map->data != NULL)
    {
        for (int i = 0; i < map->row; i++)
        {
            map->destRect.y = i * TILESIZE;
            for (int j = 0; j < map->column; j++)
            {
                map->destRect.x = j * TILESIZE;
                tile = map->grid[i][j];
                if(SDL_RenderCopy(game->renderer, map->mapTexture[tile].texture, NULL, &(map->destRect)))
                {
                    getError("\0");
                    closeGame(game);
                }
            }
        }
    }
}

void destroyMap(Map *map)
{
    if(map->mapTexture != NULL && map->nbTexture > 0)
    {
        for (int i = 0; i < map->nbTexture; i++)
        {
            SDL_DestroyTexture(map->mapTexture[i].texture); 
        }
        free(map->mapTexture);
        map->mapTexture = NULL;
    }
}