#include "WindowRenderer.h"

WindowRenderer::WindowRenderer(const World &world, Player *player, std::vector<Player *> &players, int width, int height) 
: _World(world), _Player(player), _Players(players)
{
    WindowDimensions = {width, height};

    MouseWorld = {-1, -1};

    TileLength = 32; // keep this a power of 2 for quick division

    TileCounts = {WindowDimensions.x / TileLength, WindowDimensions.y / TileLength};

    Fullscreen = false;
    Debug = false;
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
    if (!Renderer)
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

    Textures[TileType::AIR] = IMG_LoadTexture(Renderer, "Textures/grass.png");
    Textures[TileType::STONE] = IMG_LoadTexture(Renderer, "Textures/stone.png");
    Textures[TileType::GOLD] = IMG_LoadTexture(Renderer, "Textures/gold.png");
    Textures[TileType::EXPLOSIVE] = IMG_LoadTexture(Renderer, "Textures/explosive.png");
    Textures[TileType::BARRIER] = IMG_LoadTexture(Renderer, "Textures/stone.png");

    for (int i = 0; i < TILETYPE_COUNT; i++)
    {
        if (!Textures[i])
        {
            printf("Textures not properly loaded: %s\n", SDL_GetError());
            IMG_Quit();
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

        for (int i = 0; i < TILETYPE_COUNT; i++)
        {
            if (Textures[i])
            {
                SDL_DestroyTexture(Textures[i]);
            }
        }
}

void WindowRenderer::Update(Uint64 tickCount) {
    this->TickCount = tickCount;
    this->ClearFrame();
    this->RenderFrame();
}

void WindowRenderer::UpdateWindow()
{
    SDL_GetWindowSize(Window, &WindowDimensions.x, &WindowDimensions.y);

    TileCounts = {WindowDimensions.x / TileLength, WindowDimensions.y / TileLength};
}

void WindowRenderer::ToggleDebug() { Debug = !Debug; }

void WindowRenderer::ToggleFullScreen()
{
    Fullscreen = !Fullscreen;
    Uint32 flag = Fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
    SDL_SetWindowFullscreen(Window, flag);
}

void WindowRenderer::OutlineTile(int x, int y)
{
    Vec2 renderCoords = {(int)x + MinCoordinates.x, (int)y + MinCoordinates.y};
    int rendX = (int)renderCoords.x * TileLength - TileOffset.x;
    int rendY = (int)renderCoords.y * TileLength - TileOffset.y;

    SDL_RenderDrawLine(Renderer, rendX, rendY, rendX + TileLength, rendY);
    SDL_RenderDrawLine(Renderer, rendX, rendY, rendX, rendY + TileLength);
    SDL_RenderDrawLine(Renderer, rendX + TileLength, rendY, rendX + TileLength, rendY + TileLength);
    SDL_RenderDrawLine(Renderer, rendX, rendY + TileLength, rendX + TileLength, rendY + TileLength);
}

bool WindowRenderer::IsDiscovered(int x, int y)
{
    return _World.IsInBounds(x, y) && _Player->DiscoveredTiles[x][y];
}

void WindowRenderer::RenderFrame()
{
    float x = _Player->Center.x, y = _Player->Center.y;
    TileOffset = {(int)((x - SDL_floorf(x)) * TileLength), (int)((y - SDL_floorf(y)) * TileLength)};
    MinCoordinates = {(int)x - (TileCounts.x / 2), (int)y - (TileCounts.y / 2)};
    MaxCoordinates = {MinCoordinates.x + TileCounts.x, MinCoordinates.y + TileCounts.y};

    DrawWorld();
    DrawPlayers();
    DrawAndStoreSelectedTile();

    if (Debug)
        DebugInfo();

    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255); //undiscovered tiles are just black
    SDL_RenderPresent(Renderer);
}

void WindowRenderer::ClearFrame()
{
    if (SDL_RenderClear(Renderer) < 0)
    {
        printf("RenderClear failed, %s\n", SDL_GetError());
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
            if (_World.IsInBounds(i, j) && _Player->DiscoveredTiles[i][j])
            {
                int textureIdx = _World.tiles[i][j].TileType;
                SDL_RenderCopy(Renderer, Textures[textureIdx], nullptr, &rect);
            }
            rect.y += TileLength;
        }
        rect.x += TileLength;
        rect.y = -TileOffset.y;
    }
}

void WindowRenderer::DrawPlayers()
{
    // Draw all players in the vector
    //todo: if player is not within view, dont draw
    for (Player* p : _Players) 
    {
        // Calculate position relative to current player
        int rendX = ((p->Center.x - MinCoordinates.x) * TileLength) - TileOffset.x + 1; //this is a terrible fix, find some other way to do this
        int rendY = ((p->Center.y - MinCoordinates.y) * TileLength) - TileOffset.y + 1;

        // Create rectangle for the player
        SDL_Rect displayRect;
        displayRect.x = rendX - (p->HalfDimensions.x * TileLength);
        displayRect.y = rendY - (p->HalfDimensions.y * TileLength);
        displayRect.w = p->BoundingBox.w * TileLength;
        displayRect.h = p->BoundingBox.h * TileLength;

        // Set different colors for different players
        SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255); // White for current player
        SDL_RenderFillRect(Renderer, &displayRect);

        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        for (int i = p->xStart; i <= p->xEnd; i++)
        {
            for (int j = p->yStart; j <= p->yEnd; j++)
            {
                if (_World.IsInBounds(i, j) && !_Player->DiscoveredTiles[i][j])
                {
                    displayRect.x = (i - MinCoordinates.x) * TileLength - TileOffset.x;
                    displayRect.y = (j - MinCoordinates.y) * TileLength - TileOffset.y;
                    displayRect.w = TileLength;
                    displayRect.h = TileLength;
                    SDL_RenderFillRect(Renderer, &displayRect);
                }
            }
        }
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
            std::format("   Tile Health: {}\n", (_World.IsInBounds(MouseWorld) && _Player->DiscoveredTiles[MouseWorld.x][MouseWorld.y])
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
    float x = (float)(MouseScreen.x + TileOffset.x) / TileLength;
    float y = (float)(MouseScreen.y + TileOffset.y) / TileLength;

    MouseWorld = {(int)x + MinCoordinates.x, (int)y + MinCoordinates.y};
    _Player->Target = {x + MinCoordinates.x, y + MinCoordinates.y};

    _Player->CanMine ? SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255) : SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);

    int rendX = (int)x * TileLength - TileOffset.x;
    int rendY = (int)y * TileLength - TileOffset.y;

    if (_World.IsInBounds((int)MouseWorld.x, (int)MouseWorld.y) && _Player->DiscoveredTiles[((int)MouseWorld.x)][((int)MouseWorld.y)])
    {
        SDL_RenderDrawLine(Renderer, rendX, rendY, rendX + TileLength, rendY);
        SDL_RenderDrawLine(Renderer, rendX, rendY, rendX, rendY + TileLength);
        SDL_RenderDrawLine(Renderer, rendX + TileLength, rendY, rendX + TileLength, rendY + TileLength);
        SDL_RenderDrawLine(Renderer, rendX, rendY + TileLength, rendX + TileLength, rendY + TileLength);
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

