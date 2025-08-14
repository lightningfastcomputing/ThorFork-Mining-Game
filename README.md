# Mining-Game
Extract the gold! Drill through the stone! Blow things up with dynamite!
Terrain Destruction Game using SDL2, written in C++

# Demo Videos:
https://www.youtube.com/@ottopfeff

# TODO

unify world and entitymanager to facilitate interactions between

implement rock chunks

# Ideas

FAULT LINES (my most genius idea) (insanely complicated probably)

add complex entities using an array of bounding boxes 

Tie view and tilesize to player size (keep tilesize a power of 2)

Fix entity tunneling through world tiles (for now, just make the player slow)

Investigate rendering with RenderDoc

Improve line-of-sight mining with two rays

Breaking stone/gold releases a bunch of chunks that have to be picked up for transport

Cave generation

Caveins

Minecarts and tracks

Add chunking for world to facilitate online multiplayer

Add basic multiplayer

# Past Tasks

Fix jitter in renderer (not using float render functions probably)

implement collision bitmasks

Fix Explosions: should reach further

Improve tiles (add health, more variance, better worldgen), fix scoring:

    * tiles have health, explosion resistance, hardness, etc
    
    * worldgen: differentiate between overworld and underground, different walkable floortypes, veins of ore 

Add sounds for mining, walking, ambience, explosions

add more entities and improve physics engine to deal with them: entity class that player, monsters, etc inherit from

fix logic for line-of-sight

Use raycasting to determine block to be mined

Fix rendering again (wide players force x-level gen to be off)

Fix Makefile

Fix performance issue with cascading explosives (use priority queue)

Utilize render batching

Make window resizeable/fullscreen

Add debug info to screen instead of just using console

Fix the discover function for the renderer (use rays or something)

Either standardize Vec2F/Vec2 coordinates or just use float coordinates, Vec2 could come in handly for raycasting later though

Update bounding boxes and physics to allow rectangular player figures

When physics are improved, replace bounding boxes with SDL_RECTs to take advantage of the library's utilities

Add a queue to the world class that sequences actions done to it (cascading explosions, falling rocks, etc)

Update renderer types so params arent needed (make better names or document)

Update renderer and coordinates to properly display player in the exact middle

Add button debouncing

Update collider for any size (square) object












