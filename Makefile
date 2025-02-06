CC = g++
CFLAGS = -Wall -g
SDLFLAGS = -I ./src/include -L ./src/lib -lSDL2main -lSDL2 -ggdb -O0

all:
	$(CC) -c World.cpp 
	$(CC) -c Player.cpp
	$(CC) -c WindowRenderer.cpp
	$(CC) -c InputManager.cpp
	$(CC) Main.cpp -o Main.exe World.o Player.o WindowRenderer.o InputManager.o $(SDLFLAGS)