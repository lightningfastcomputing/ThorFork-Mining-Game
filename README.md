# Mining-Game
Extract the gold! Drill through the stone! Blow things up with dynamite!
Terrain Destruction Game using SDL2, written in C++

# Demo:
https://www.youtube.com/watch?v=Xjp7c8JEtJ8 (out of date)


# This game was made on WSL2 Ubuntu
Compiling is described in the Makefile and requires gcc, gdb, and SDL2 packages
Cross-Platform building will be implemented later

Run these commands in the terminal to install:

sudo apt install gcc

sudo apt install gdb

sudo apt-get install libsdl2-dev

sudo apt-get install libsdl2-image-dev

# TODO
improve collision function on entitymanager to allow for objects larger than 1f size, and fix tunneling that happens with high enough speeds
add a queue to the world class that sequences actions done to it (cascading explosions, falling rocks, etc)

when physics are improved, replace bounding boxes with SDL_RECTs to take advantage of the library's utilities

either standardize Vec2F/Vec2 coordinates or just use float coordinates, Vec2 could come in handly for raycasting later though






