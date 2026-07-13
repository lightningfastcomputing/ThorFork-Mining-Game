# ThorFork Mining Game

A Windows-tested fork and expansion of **Otto Pfeff's Mining-Game**, a destructible-terrain mining sandbox written in C++ using SDL2.

> Original project and core engine by [ottopfeff](https://github.com/ottopfeff/Mining-Game)  
> Windows stabilization and Deep Systems expansion by [lightningfastcomputing](https://github.com/lightningfastcomputing)

## Current Status

The `deep-systems-edition` branch contains the first stabilization pass.

The game now builds and launches successfully on a current Windows 10/11 system using MSYS2 MinGW64.

### Confirmed Working

- Native C++20 compilation
- SDL2 rendering
- Texture loading
- Procedural world generation
- Player and entity initialization
- SDL audio startup
- WAV loading and conversion
- Debug builds using GDB
- Local Windows deployment

## Windows Startup Fixes

The original audio initialization caused immediate segmentation faults on a modern MSYS2/SDL2 environment.

The following issues were corrected:

- Fixed incorrect `SDL_BuildAudioCVT()` result handling
- Initialized the SDL audio output format before WAV conversion
- Opened the audio device before loading sounds
- Zero-initialized audio callback channel state
- Initialized WAV pointers to `nullptr`
- Added bounds and null checks to `PlaySound()`
- Added SDL audio locking around channel assignment
- Initialized failed WAV buffers safely
- Replaced incorrect `SDL_CloseAudioDevice(1)` usage with `SDL_CloseAudio()`
- Added clear audio initialization error output

One observed crash exposed an uninitialized callback source pointer:

```text
0xbaadf00dbaadf00d
```

This was resolved by zero-initializing `UserData` before the SDL audio thread begins.

## Build Requirements

Use the **MSYS2 MinGW x64** terminal.

Required packages:

```bash
pacman -S --needed \
  git \
  make \
  mingw-w64-x86_64-gcc \
  mingw-w64-x86_64-SDL2 \
  mingw-w64-x86_64-SDL2_image \
  mingw-w64-x86_64-SDL2_ttf \
  mingw-w64-x86_64-gdb
```

## Clone

```bash
git clone https://github.com/lightningfastcomputing/ThorFork-Mining-Game.git
cd ThorFork-Mining-Game
git checkout deep-systems-edition
```

## Build and Run

A serial build is currently recommended:

```bash
make clean-native
make -j1
./Main.exe
```

Debug build:

```bash
make clean-native
make debug -j1
gdb ./Main.exe
```

Inside GDB:

```gdb
run
bt
bt full
```

## Why `-j1`?

A highly parallel build occasionally produced:

```text
g++: Bad address
```

The failed compiler process left a missing object file, after which the linker attempted to continue.

Serial compilation has been reliable:

```bash
make -j1
```

## Original Game Features

The existing engine includes work around:

- Destructible tile-based terrain
- Mining and raycasting
- Dynamite and cascading explosions
- Tile health, hardness, and blast resistance
- Rock chunks
- Minecarts and tracks
- Entity inheritance
- Collision bitmasks
- Render batching
- Sound mixing
- Priority-queued world actions
- Procedural world generation
- Resizable and fullscreen rendering
- In-game debug information

## Deep Systems Edition Roadmap

This fork aims to turn the original prototype into a more systems-heavy mining sandbox while preserving the original engine and design direction.

### Phase 1 — Stabilization and Instrumentation

- Safe deferred entity removal
- Fix multi-dynamite double-free behavior
- Improve audio resource ownership
- Add an F3 runtime diagnostics overlay
- Add collision-box and raycast visualization
- Add chunk-border visualization
- Add frame-time and entity-count metrics

### Phase 2 — Procedural World Systems

- Seeded deterministic generation
- Dedicated world generator
- Cave layers and underground biomes
- Ore veins and fault lines
- Structures and abandoned mine shafts
- Chunk-based generation and streaming
- Persistent modified terrain

### Phase 3 — Dynamic Lighting

- Tile-based light propagation
- Torches
- Explosion flashes
- Glowing ore and crystals
- Lava light
- Local lighting recalculation after terrain destruction

### Phase 4 — Structural Simulation

- Tile support values
- Unstable terrain
- Cave-ins after mining and explosions
- Falling rock entities
- Debris, dust, and impact damage
- Event-driven support recalculation

### Phase 5 — Fluids

- Water and lava cells
- Downward and lateral flow
- Active-cell update queues
- Water/lava reactions
- Explosion displacement
- Pumps and pipes

### Phase 6 — Minecart Logistics

- Cargo carts
- Passenger carts
- Explosive carts
- Junctions and switches
- Powered rails
- Loaders and unloaders
- Routing and track signals
- Automated ore transport

### Phase 7 — Creatures and AI

- Minecart Goblin
- Cave creatures
- Weighted A* pathfinding
- Burrowing movement
- Sound-reactive AI
- Terrain-aware navigation

### Phase 8 — Persistence

- Versioned save format
- World seed storage
- Player state
- Modified chunks
- Entities and minecart systems
- Fluid and lighting state
- Save migration support

## Intended Architecture

```text
Engine/
  Renderer
  Audio
  Input
  Resources
  Serialization

World/
  Chunk
  Tile
  Generator
  Lighting
  Fluids
  StructuralSupport

Entities/
  Player
  Minecart
  Explosive
  Creature
  Item

Gameplay/
  Mining
  Inventory
  Crafting
  Logistics

Debug/
  Overlay
  Profiler
  Visualization
```

The long-term goal is for systems to communicate through events rather than tightly coupling every subsystem:

```text
Explosion
  -> terrain damage
  -> lighting update
  -> support recalculation
  -> falling debris
  -> fluid activation
  -> AI sound event
  -> minecart damage
```

## Known Upstream Issues

The original project documents several unfinished areas, including:

- Crashes involving clustered dynamite
- Possible double-free behavior
- Collision jitter
- Entity tunneling
- Primitive collision resolution
- Incomplete cave generation
- Incomplete cave-ins
- Experimental minecart behavior

These are part of the planned stabilization work.

## Attribution

This repository is a fork, not a rewrite.

### Original work

- Core game engine
- Terrain and mining systems
- Entity architecture
- Rendering
- Physics and collision foundation
- Minecarts
- Explosives
- Original assets and project concept

Created by **Otto Pfefferkorn**.

### Fork work

- Windows deployment testing
- GDB crash analysis
- SDL audio stabilization
- Build documentation
- Defensive runtime checks
- Deep Systems Edition architecture and future expansions

Developed by **Thor / lightningfastcomputing**.

## Branches

- `main` — synchronized fork of the original upstream project
- `deep-systems-edition` — stabilized Windows build and active expansion branch

## Upstream

Original repository:

```text
https://github.com/ottopfeff/Mining-Game
```

This fork:

```text
https://github.com/lightningfastcomputing/ThorFork-Mining-Game
```

## License

No explicit license was visible in the upstream repository at the time of this fork.

All original ownership remains with the respective author. Until a license is provided, treat the code as publicly viewable source rather than automatically reusable under a standard open-source license.
