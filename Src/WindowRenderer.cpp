#include "WindowRenderer.h"

WindowRenderer::WindowRenderer(const World &world, Player *player, std::vector<Entity *> &entities, int width, int height) 
: _World(world), _Player(player), _Entities(entities)
{
    WindowDimensions = {width, height};

    MouseWorld = {-1, -1};
    MouseRelativeDeltas = {0, 0};

    TileLength = 32; // keep this a power of 2 for quick division

    TileCounts = {WindowDimensions.x / TileLength, WindowDimensions.y / TileLength};

    Discovered = new bool*[_World.Width];
    for (int i = 0; i < _World.Width; i++)
    {
        Discovered[i] = new bool[_World.Height];
        std::memset(Discovered[i], false, _World.Height * sizeof(bool));
    }


    Fullscreen = false;
    Debug = false;
    GlobalView = false;
    RelativeCursorMode = false;

    Running = true;

    Init_Display("Mining Game");
}

void WindowRenderer::Init_Display(const char *windowTitle)
{
    Window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WindowDimensions.x, WindowDimensions.y, SDL_WINDOW_RESIZABLE);
    if (!Window)
    {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        Running = false;
    }
    SDL_SetWindowMinimumSize(Window, 640, 480);

    SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");

    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (Renderer == nullptr)
    {
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        Running = false;
    }

    if (TTF_Init() < 0)
    {
        std::cerr << "TTF_Iinit() failed, " << TTF_GetError() << std::endl;
    }

    TextFont = TTF_OpenFont("Textures/Roboto.ttf", 14);
    if (!TextFont)
    {
        std::cerr << "TTF_OpenFont failed: " << SDL_GetError() << std::endl;
        Running = false;
    }


    //set tileTextures
    TileTextures[TileType::AIR] = IMG_LoadTexture(Renderer, "Textures/grass.png");
    TileTextures[TileType::STONE] = IMG_LoadTexture(Renderer, "Textures/stone.png");
    TileTextures[TileType::DENSE_STONE] = IMG_LoadTexture(Renderer, "Textures/dense_rock.png");
    TileTextures[TileType::GOLD] = IMG_LoadTexture(Renderer, "Textures/gold.png");
    TileTextures[TileType::EXPLOSIVE] = IMG_LoadTexture(Renderer, "Textures/explosive.png");
    TileTextures[TileType::BARRIER] = IMG_LoadTexture(Renderer, "Textures/barrier.png");

    //set entityTextures
    EntityTextures[EntityType::PLAYER] = IMG_LoadTexture(Renderer, "Textures/barrier.png");
    EntityTextures[EntityType::DYNAMITE] = IMG_LoadTexture(Renderer, "Textures/explosive.png");


    for (int i = 0; i < TILETYPE_COUNT; i++)
    {
        if (!TileTextures[i])
        {
            printf("TileTextures not properly loaded: %s\n", SDL_GetError());
            Running = false;
        }
    }
}

WindowRenderer::~WindowRenderer()
{
    if (Window)
        SDL_DestroyWindow(Window);
    if (Renderer)
        SDL_DestroyRenderer(Renderer);
    if (TextFont)
        TTF_CloseFont(TextFont);

    for (int i = 0; i < TILETYPE_COUNT; i++)
    {
        if (TileTextures[i])
        {
            SDL_DestroyTexture(TileTextures[i]);
        }
    }

    for (int i = 0; i < _World.Width; i++)
    {
        delete[] Discovered[i];
    }
    delete[] Discovered;
}

void WindowRenderer::Update(Uint64 tickCount) {
    RadialDiscover();
    ClearFrame();
    RenderFrame();
}

void WindowRenderer::UpdateWindow()
{
    SDL_GetWindowSize(Window, &WindowDimensions.x, &WindowDimensions.y);

    TileCounts = {WindowDimensions.x / TileLength, WindowDimensions.y / TileLength};
}

void WindowRenderer::ToggleRelativeCursor()
{
    RelativeCursorMode = !RelativeCursorMode;
    SDL_bool toggle = (SDL_bool)RelativeCursorMode;
    if (SDL_SetRelativeMouseMode(toggle) < 0)
    {
        printf("SDL_SetRelativeMouseMode failed: %s", SDL_GetError());
    }
    else 
    {
        if (toggle)
        {
            LastMouseCoordinates = MouseScreen;
        }
        else
        {
            SDL_WarpMouseInWindow(Window, LastMouseCoordinates.x, LastMouseCoordinates.y);
        }
    }
}

void WindowRenderer::ToggleGlobalView() { GlobalView = !GlobalView; }

void WindowRenderer::ToggleDebug() { Debug = !Debug; }

void WindowRenderer::ToggleFullScreen()
{
    Fullscreen = !Fullscreen;
    Uint32 flag = Fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
    if (SDL_SetWindowFullscreen(Window, flag) < 0)
    {
        printf("SDL_SetWindowFullscreen failed: %s", SDL_GetError());
    }
}

void WindowRenderer::OutlineTile(int x, int y)
{
    Vec2 renderCoords = {(int)x + MinCoordinates.x, (int)y + MinCoordinates.y};
    int rendX = (int)renderCoords.x * TileLength - TileOffset.x;
    int rendY = (int)renderCoords.y * TileLength - TileOffset.y;

    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(Renderer, rendX, rendY, rendX + TileLength, rendY);
    SDL_RenderDrawLine(Renderer, rendX, rendY, rendX, rendY + TileLength);
    SDL_RenderDrawLine(Renderer, rendX + TileLength, rendY, rendX + TileLength, rendY + TileLength);
    SDL_RenderDrawLine(Renderer, rendX, rendY + TileLength, rendX + TileLength, rendY + TileLength);
}

void WindowRenderer::Reveal()
{
    for (int i = 0; i < _World.Width; i++)
    {
        memset(Discovered[i], true, _World.Height);
    }
}

bool WindowRenderer::IsDiscovered(int x, int y)
{
    return _World.IsInBounds(x, y) && Discovered[x][y];
}

void WindowRenderer::RenderFrame()
{
    float x = _Player->Center.x, y = _Player->Center.y;
    TileOffset = {(int)((x - SDL_floorf(x)) * TileLength), (int)((y - SDL_floorf(y)) * TileLength)};
    MinCoordinates = {(int)x - (TileCounts.x / 2), (int)y - (TileCounts.y / 2)};
    MaxCoordinates = {MinCoordinates.x + TileCounts.x, MinCoordinates.y + TileCounts.y};

    if (!GlobalView) 
    {

    DrawWorld();
    DrawEntities();
        if (RelativeCursorMode)
        {
            DrawPlayerReach();
            DrawCursor();
            DrawAndStoreSelectedTile();
        }
        if (Debug)
            DebugInfo();
    }
    else
    {
        GlobalDrawWorld();
    }

    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255); //undiscovered tiles are just black
    SDL_RenderPresent(Renderer);
}

void WindowRenderer::ClearFrame()
{
    if (SDL_RenderClear(Renderer) < 0)
    {
        printf("RenderClear failed, %s\n", SDL_GetError());
        Running = false;
    }
}

void WindowRenderer::GlobalDrawWorld()
{
    int globalTileLength = _World.Height < _World.Width ? WindowDimensions.x/_World.Width : WindowDimensions.y/_World.Height;

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = globalTileLength;
    rect.h = globalTileLength;

    for (int i = 0; i < _World.Width; i++)
    {
        for (int j = 0; j < _World.Height; j++)
        {
            if (Discovered[i][j])
            {
                int textureIdx = _World.tiles[i][j].TileType;
                if (SDL_RenderCopy(Renderer, TileTextures[textureIdx], nullptr, &rect) < 0)
                {
                    printf("SDL_RenderCopy Failed: %s", SDL_GetError());
                }
            }
            rect.y += globalTileLength;
        }
        rect.x += globalTileLength;
        rect.y = 0;
    }

    //draw little squares for the tiles the players are inhabiting
    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    for (Entity* p : _Entities)
    {
        rect.x = p->xStart * globalTileLength;
        rect.y = p->yStart * globalTileLength;
        rect.w = (p->xEnd - p->xStart) + 1 * globalTileLength;
        rect.h = (p->yEnd - p->yStart) + 1 * globalTileLength;
        SDL_RenderFillRect(Renderer, &rect);
    }
}

void WindowRenderer::DrawWorld()
{
    SDL_Rect rect;
    rect.x = -TileOffset.x;
    rect.y = -TileOffset.y;
    rect.w = TileLength;
    rect.h = TileLength;

    for (int i = MinCoordinates.x; i <= MaxCoordinates.x + 1; i++)
    {
        for (int j = MinCoordinates.y; j <= MaxCoordinates.y + 1; j++)
        {
            if (_World.IsInBounds(i, j) && Discovered[i][j])
            {
                int textureIdx = _World.tiles[i][j].TileType;
                SDL_RenderCopy(Renderer, TileTextures[textureIdx], nullptr, &rect);
            }
            rect.y += TileLength;
        }
        rect.x += TileLength;
        rect.y = -TileOffset.y;
    }
}

void WindowRenderer::DrawEntities()
{
    // Draw all players in the vector
    //todo: if player is not within view, dont draw
    for (Entity* p : _Entities) 
    {
        // Calculate position relative to current player
        int rendX = ((p->Center.x - MinCoordinates.x) * TileLength) - TileOffset.x + 1; //this is a terrible fix, find some other way to do this (+ 1)
        int rendY = ((p->Center.y - MinCoordinates.y) * TileLength) - TileOffset.y + 1;

        SDL_Rect displayRect;
        displayRect.x = rendX - (p->HalfDimensions.x * TileLength);
        displayRect.y = rendY - (p->HalfDimensions.y * TileLength);
        displayRect.w = p->BoundingBox.w * TileLength;
        displayRect.h = p->BoundingBox.h * TileLength;

        SDL_RenderCopy(Renderer, EntityTextures[p->type], nullptr, &displayRect);

    }
}

void WindowRenderer::DebugInfo()
{
        DrawPlayerCollisionBox();
        DrawPlayerVector();

        SDL_Rect textRect = {(int)(WindowDimensions.x * 0.01f), (int)(WindowDimensions.y * 0.01f), 0, 0};

        std::string debugInfo =
            std::format("Screen Width, Height: ({},{})\n", WindowDimensions.x, WindowDimensions.y) +
            std::format("Player: ({:.2f},{:.2f})\n", _Player->Center.x, _Player->Center.y) +
            std::format("Score: {}\n", _Player->Score) + std::format("MouseScreen: ({},{})\n", MouseScreen.x, MouseScreen.y) +
            std::format("MouseWorld: ({},{})\n", MouseWorld.x, MouseWorld.y) +
            std::format("   Tile Health: {}\n", (_World.IsInBounds(MouseWorld) && Discovered[MouseWorld.x][MouseWorld.y])
                                                    ? _World.tiles[MouseWorld.x][MouseWorld.y].Health
                                                    : -1) +
            std::format("PlayerTarget: ({:.2f},{:.2f})\n", _Player->Target.x, _Player->Target.y) +
            std::format("MinimumCoordinates: X={},Y={}\n", MinCoordinates.x, MinCoordinates.y) +
            std::format("MaximumCoordinates: X={},Y={}\n", MaxCoordinates.x, MaxCoordinates.y) +
            std::format("TileCounts: X={}, Y={}\n", TileCounts.x, TileCounts.y) +
            std::format("Offsets: X={}, Y={}\n", TileOffset.x, TileOffset.y) +
            std::format("TickCount: {}\n", TickCount);

        SDL_Surface *textSurface = TTF_RenderText_Solid_Wrapped(TextFont, debugInfo.c_str(), {255, 255, 255, 255}, 0);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(Renderer, textSurface);
        if (!textSurface)
        {
            std::cerr << "Unable to create text surface TTF_Error: " << TTF_GetError() << std::endl;
            Running = false;
        }

        textRect.w = textSurface->w;
        textRect.h = textSurface->h;

        SDL_RenderCopy(Renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
}

void WindowRenderer::DrawAndStoreSelectedTile()
{
    _Player->Target.x += (float)MouseRelativeDeltas.x/TileLength/2;
    _Player->Target.y += (float)MouseRelativeDeltas.y/TileLength/2;

    Vec2F targetVector = _Player->Target - _Player->Center;
    if (targetVector.Magnitude() > _Player->MiningRadius)
    {
        targetVector.Normalize();
        _Player->Target = _Player->Center + (targetVector * (_Player->MiningRadius - 0.001f));
    }
    MouseWorld = {(int)_Player->Target.x, (int)_Player->Target.y};

    _Player->CanMine ? SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255) : SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);


    int rendX = (int)(_Player->Target.x - MinCoordinates.x) * TileLength - TileOffset.x;
    int rendY = (int)(_Player->Target.y - MinCoordinates.y) * TileLength - TileOffset.y;

    if (_World.IsInBounds((int)MouseWorld.x, (int)MouseWorld.y) && Discovered[((int)MouseWorld.x)][((int)MouseWorld.y)])
    {
        SDL_RenderDrawLine(Renderer, rendX, rendY, rendX + TileLength, rendY);
        SDL_RenderDrawLine(Renderer, rendX, rendY, rendX, rendY + TileLength);
        SDL_RenderDrawLine(Renderer, rendX + TileLength, rendY, rendX + TileLength, rendY + TileLength);
        SDL_RenderDrawLine(Renderer, rendX, rendY + TileLength, rendX + TileLength, rendY + TileLength);
    }
}

void WindowRenderer::DrawPlayerReach()
{
    int rendX = ((_Player->Center.x - MinCoordinates.x) * TileLength) - TileOffset.x;
    int rendY = ((_Player->Center.y - MinCoordinates.y) * TileLength) - TileOffset.y;

    int divisions = 30;
    float dtheta = (2 * PI)/divisions;
    float theta = 0;

    int x1, y1;
    int x0 = rendX + (_Player->MiningRadius * TileLength) * SDL_cosf(theta);
    int y0 = rendY + (_Player->MiningRadius * TileLength) * SDL_sinf(theta);

    for (int i = 0; i < divisions; i++)
    {
        theta += dtheta;
        x1 = rendX + (_Player->MiningRadius * TileLength) * SDL_cosf(theta);
        y1 = rendY + (_Player->MiningRadius * TileLength) * SDL_sinf(theta);

        SDL_RenderDrawLine(Renderer, x0, y0, x1, y1);

        x0 = x1;
        y0 = y1;
    }
}

void WindowRenderer::DrawCursor()
{
    SDL_Rect rect;
    rect.x = (_Player->Target.x - MinCoordinates.x) * TileLength - TileOffset.x - TileLength/4;
    rect.y = (_Player->Target.y - MinCoordinates.y) * TileLength - TileOffset.y - TileLength/4;
    rect.w = TileLength/2;
    rect.h = TileLength/2;

    SDL_RenderCopy(Renderer, TileTextures[EXPLOSIVE], nullptr, &rect);
}

void WindowRenderer::RadialDiscover()
{
        int numRays = (int)(2 * PI * _Player->DiscoverRadius) + 1; // aproximation of how many rays are needed to get a clean discovery radius
        float dTheta = (2 * PI) / numRays;
        float theta = 0;

        for (int i = 0; i < numRays; i++)
        {
            Vec2F endPos = {_Player->Center.x + (_Player->DiscoverRadius * SDL_cosf(theta)), _Player->Center.y + (_Player->DiscoverRadius * SDL_sinf(theta))};

            int x = (int)_Player->Center.x;
            int y = (int)_Player->Center.y;
            int endX = (int)(endPos.x);
            int endY = (int)(endPos.y);

            float dx = endPos.x - _Player->Center.x;
            float dy = endPos.y - _Player->Center.y;

            int stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
            int stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

            float tDeltaX = (stepX != 0) ? SDL_fabsf(1.0f / dx) : 1e6;
            float tDeltaY = (stepY != 0) ? SDL_fabsf(1.0f / dy) : 1e6;

            float nextTileBoundaryX = (stepX > 0) ? (SDL_floorf(_Player->Center.x) + 1.0f) : (SDL_floorf(_Player->Center.x));
            float nextTileBoundaryY = (stepY > 0) ? (SDL_floorf(_Player->Center.y) + 1.0f) : (SDL_floorf(_Player->Center.y));

            float tMaxX = (stepX != 0) ? SDL_fabsf((nextTileBoundaryX - _Player->Center.x) / dx) : 1e6;
            float tMaxY = (stepY != 0) ? SDL_fabsf((nextTileBoundaryY - _Player->Center.y) / dy) : 1e6;

            while (x != endX || y != endY) 
            {
                if (_World.IsInBounds(x,y))
                    Discovered[x][y] = true;
                if (!_World.tiles[x][y].Passable) 
                    break;

                if (tMaxX < tMaxY) 
                {
                    tMaxX += tDeltaX;
                    x += stepX;
                } 
                else 
                {
                    tMaxY += tDeltaY;
                    y += stepY;
                }
            }
            theta += dTheta;
        }
}

void WindowRenderer::DrawPlayerBoundingBox()
{
    // todo
}

void WindowRenderer::DrawPlayerCollisionBox()
{
    int xStart = _Player->xStart, yStart = _Player->yStart, xEnd = _Player->xEnd, yEnd = _Player->yEnd;

    SDL_SetRenderDrawColor(Renderer, 255, 20, 255, 255);
    int rendX0 = (xStart - MinCoordinates.x) * TileLength - TileOffset.x;
    int rendX1 = (xEnd + 1 - MinCoordinates.x) * TileLength - TileOffset.x;
    int rendY0 = (yStart - MinCoordinates.y) * TileLength - TileOffset.y;
    int rendY1 = (yEnd + 1 - MinCoordinates.y) * TileLength - TileOffset.y;

    SDL_RenderDrawLine(Renderer, rendX0, rendY0, rendX1, rendY0);
    SDL_RenderDrawLine(Renderer, rendX0, rendY0, rendX0, rendY1);
    SDL_RenderDrawLine(Renderer, rendX1, rendY0, rendX1, rendY1);
    SDL_RenderDrawLine(Renderer, rendX0, rendY1, rendX1, rendY1);
}

void WindowRenderer::DrawPlayerVector()
{
    int rendX0 = ((_Player->Center.x - MinCoordinates.x) * TileLength) - TileOffset.x;
    int rendY0 = ((_Player->Center.y - MinCoordinates.y) * TileLength) - TileOffset.y;
    int rendX1 = ((_Player->Target.x - MinCoordinates.x) * TileLength) - TileOffset.x;
    int rendY1 = ((_Player->Target.y - MinCoordinates.y) * TileLength) - TileOffset.y;

    SDL_RenderDrawLine(Renderer, rendX0, rendY0, rendX1, rendY1);
}

