CC = g++ 
CFLAGS = -std=c++20
SDLFLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -ggdb -O0
SRCDIR = Src/

all:
	$(CC) $(CFLAGS) -c $(SRCDIR)World.cpp 
	$(CC) $(CFLAGS) -c $(SRCDIR)Player.cpp
	$(CC) $(CFLAGS) -c $(SRCDIR)EntityManager.cpp
	$(CC) $(CFLAGS) -c $(SRCDIR)WindowRenderer.cpp -lSDL2_image -lSDL2_ttf
	$(CC) $(CFLAGS) -c $(SRCDIR)InputManager.cpp
	$(CC) $(CFLAGS) -c $(SRCDIR)Game.cpp
	$(CC) $(SRCDIR)Main.cpp -o Main.exe World.o Player.o EntityManager.o WindowRenderer.o InputManager.o Game.o $(SDLFLAGS)