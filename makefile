LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lm

.PHONY:  all clean 

all: main map game gameobject
	gcc -o bin/prog game.o gameobject.o main.o map.o $(LIBS)

main:
	gcc -c src/main.c include/game.h

map: 
	gcc -c src/map.c include/map.h

game: 
	gcc -c src/game.c include/game.h include/gameobject.h include/map.h

gameobject:
	gcc -c src/gameobject.c include/gameobject.h

clean:
	rm -f include/*.gch
	rm -f *.o