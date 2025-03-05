#include "WindowRenderer.h"

WindowRenderer::WindowRenderer(const World &world, const Player &player, int width, int height) : _World(world), _Player(player)
{
    WindowDimensions = {width, height};

    MouseWorld = {-1, -1};

    TileLength = 32; //keep this a power of 2 for quick division

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

    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);
    if (!Renderer)
    {
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        Running = false;
    }

    if (TTF_Init() < 0)
    {
        std::cerr << "TTF_Iinit() failed, " << TTF_GetError() << std::endl;
    }

    TextFont = TTF_OpenFont("Textures/Roboto.ttf", 20);
    if (!TextFont)
    {
        std::cerr << "TTF_OpenFont failed: " << SDL_GetError() << std::endl;
        Running = false;
    }

    Textures[AIR] = IMG_LoadTexture(Renderer, "Textures/grass.png");
    Textures[STONE] = IMG_LoadTexture(Renderer, "Textures/stone.png");
    Textures[GOLD] = IMG_LoadTexture(Renderer, "Textures/gold.png");
    Textures[EXPLOSIVE] = IMG_LoadTexture(Renderer, "Textures/explosive.png");

    for (int i = 0; i < 4; i++)
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

        for (int i = 0; i < 4; i++)
        {
            if (Textures[i])
            {
                SDL_DestroyTexture(Textures[i]);
            }
        }
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

bool WindowRenderer::IsDiscovered(int x, int y)
{
    return _World.IsInBounds(x, y) && _Player.DiscoveredTiles[x][y];
}

void WindowRenderer::RenderFrame()
{
    float x = _Player.Center.x, y = _Player.Center.y;
    TileOffset = {static_cast<int>((x - SDL_floorf(x)) * TileLength), static_cast<int>((y - SDL_floorf(y)) * TileLength)};
    MinCoordinates = {(int)x - (TileCounts.x / 2), (int)y - (TileCounts.y / 2)};
    MaxCoordinates = {MinCoordinates.x + TileCounts.x, MinCoordinates.y + TileCounts.y};

    DrawWorld();
    DrawPlayer();
    DrawAndStoreSelectedTile();
    DebugInfo();

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
            if (i < 0 || j < 0 || i >= _World.Width || j >= _World.Height || !_Player.DiscoveredTiles[i][j])
            {
                SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(Renderer, &rect);
            }
            else
            {
                int textureIdx = _World.tiles[i][j];
                SDL_RenderCopy(Renderer, Textures[textureIdx], nullptr, &rect);
            }
            rect.y += TileLength;
        }
        rect.x += TileLength;
        rect.y = -TileOffset.y;
    }
}

void WindowRenderer::DrawPlayer()
{
    SDL_FRect playerRect = _Player.BoundingBox;
    Vec2F halfDimensions = _Player.HalfDimensions;
    SDL_Rect displayRect;

    Vec2 windowCenter = {WindowDimensions.x >> 1, WindowDimensions.y >> 1};

    displayRect.x = (windowCenter.x - windowCenter.x%TileLength) - (int)(halfDimensions.x * TileLength);
    displayRect.y = (windowCenter.y - windowCenter.y%TileLength) - (int)(halfDimensions.y * TileLength);
    displayRect.w = playerRect.w * TileLength;
    displayRect.h = playerRect.h * TileLength;

    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(Renderer, &displayRect);
}

void WindowRenderer::DebugInfo()
{
    if (Debug)
    {
        DrawPlayerCollisionBox();

        SDL_Rect textRect = {(int)(WindowDimensions.x * 0.01f), (int)(WindowDimensions.y * 0.01f), 0, 0};

        std::string debugInfo =
            std::format("Screen Width, Height: ({},{})\n", WindowDimensions.x, WindowDimensions.y) +
            std::format("Player: ({:.2f},{:.2f})\n", _Player.Center.x, _Player.Center.y) +
            std::format("Score: {}\n", _Player.Score) + std::format("MouseScreen: ({},{})\n", MouseScreen.x, MouseScreen.y) +
            std::format("MouseWorld: ({},{})\n", MouseWorld.x, MouseWorld.y) +
            std::format("MinimumCoordinates: X={},Y={}\n", MinCoordinates.x, MinCoordinates.y) +
            std::format("MinimumCoordinates: X={},Y={}\n", MaxCoordinates.x, MaxCoordinates.y) +
            std::format("TileCounts: X={}, Y={}\n", TileCounts.x, TileCounts.y) +
            std::format("Offsets: X={}, Y={}\n", TileOffset.x, TileOffset.y);

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
}

void WindowRenderer::DrawAndStoreSelectedTile()
{
    int x = (int)((MouseScreen.x + TileOffset.x) / TileLength),
        y = (int)((MouseScreen.y + TileOffset.y) / TileLength);

    MouseWorld = {x + MinCoordinates.x, y + MinCoordinates.y};

    _Player.CanMine ? SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255) : SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);

    int rendX = (int)x * TileLength - TileOffset.x;
    int rendY = (int)y * TileLength - TileOffset.y;

    if (_World.IsInBounds(MouseWorld.x, MouseWorld.y) && _Player.DiscoveredTiles[(MouseWorld.x)][(MouseWorld.y)])
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
    int xStart = _Player.xStart, yStart = _Player.yStart, xEnd = _Player.xEnd, yEnd = _Player.yEnd;

    SDL_SetRenderDrawColor(Renderer, 255, 20, 255, 255);
    float rendX0 = (xStart - MinCoordinates.x) * TileLength - TileOffset.x;
    float rendX1 = (xEnd + 1 - MinCoordinates.x) * TileLength - TileOffset.x;
    float rendY0 = (yStart - MinCoordinates.y) * TileLength - TileOffset.y;
    float rendY1 = (yEnd + 1 - MinCoordinates.y) * TileLength - TileOffset.y;

    SDL_RenderDrawLineF(Renderer, rendX0, rendY0, rendX1, rendY0);
    SDL_RenderDrawLineF(Renderer, rendX0, rendY0, rendX0, rendY1);
    SDL_RenderDrawLineF(Renderer, rendX1, rendY0, rendX1, rendY1);
    SDL_RenderDrawLineF(Renderer, rendX0, rendY1, rendX1, rendY1);
}

void WindowRenderer::DrawPlayerVector()
{
    // todo
}