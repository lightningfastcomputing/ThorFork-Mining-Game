CC = g++
CFLAGS = -Wall -g
SDLFLAGS = -lSDL2main -lSDL2 -lSDL2_image -ggdb -O0
SRCDIR = Src/

all:
	$(CC) -c $(SRCDIR)World.cpp 
	$(CC) -c $(SRCDIR)Player.cpp
	$(CC) -c $(SRCDIR)WindowRenderer.cpp -lSDL2_image
	$(CC) -c $(SRCDIR)InputManager.cpp
	$(CC) -c $(SRCDIR)Game.cpp
	$(CC) $(SRCDIR)Main.cpp -o Main.exe World.o Player.o WindowRenderer.o InputManager.o Game.o $(SDLFLAGS)