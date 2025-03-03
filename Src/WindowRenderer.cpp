#include "WindowRenderer.h"

WindowRenderer::WindowRenderer(const World &world, const Player &player, int width, int height) : _World(world), _Player(player)
{
    ScreenDimensions = {width, height};

    TileLength = 32;

    TileCounts = {ScreenDimensions.x / TileLength, ScreenDimensions.y / TileLength};
    ScreenRemainderOffset = {(ScreenDimensions.x % TileLength) / 2, (ScreenDimensions.y % TileLength) / 2};
    PlayerDimensionOffset = {_Player.HalfDimensions.x * TileLength, _Player.HalfDimensions.y * TileLength};

    Debug = false;
    Running = true;
    Init_Display("Mining Game");
}

void WindowRenderer::Init_Display(const char *windowTitle)
{
    Window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenDimensions.x, ScreenDimensions.y, SDL_WINDOW_ALLOW_HIGHDPI);
    if (!Window)
    {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        Running = false;
    }
    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_SOFTWARE);
    if (!Renderer)
    {
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        Running = false;
    }

    if (TTF_Init() < 0) {
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

bool WindowRenderer::IsDiscovered(int x, int y)
{
    return _World.IsInBounds(x, y) && _Player.DiscoveredTiles[x][y];
}
void WindowRenderer::RenderFrame()
{
    float x = _Player.BoundingBox.x, y = _Player.BoundingBox.y;
    TileOffset = {(x - (int)x) * TileLength, (y - (int)y) * TileLength};
    MinCoordinates = {(int)x - (TileCounts.x / 2), (int)y - (TileCounts.y / 2)};

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

void WindowRenderer::ToggleDebug() { Debug = !Debug; }

void WindowRenderer::DrawWorld()
{
    SDL_Rect rect;
    rect.x = -TileOffset.x - TileLength + ScreenRemainderOffset.x - PlayerDimensionOffset.x;
    rect.y = -TileOffset.y + ScreenRemainderOffset.y - PlayerDimensionOffset.y;
    rect.w = TileLength;
    rect.h = TileLength;

    for (int i = MinCoordinates.x - 1; i < MinCoordinates.x + TileCounts.x + 2; i++)
    {
        for (int j = MinCoordinates.y; j < MinCoordinates.y + TileCounts.y + 3; j++)
        {
            if (i < 0 || j < 0 || i > _World.Width - 1 || j > _World.Height - 1 || !_Player.DiscoveredTiles[i][j])
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
        rect.y = -TileOffset.y + ScreenRemainderOffset.y - PlayerDimensionOffset.y;
    }
}

void WindowRenderer::DrawPlayer()
{
    SDL_FRect playerRect = _Player.BoundingBox;
    Vec2F halfDimensions = _Player.HalfDimensions;
    SDL_Rect displayRect;

    displayRect.x = (ScreenDimensions.x / 2) - (int)(halfDimensions.x * TileLength);
    displayRect.y = (ScreenDimensions.y / 2) - (int)(halfDimensions.y * TileLength);
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

        SDL_Rect textRect = {(int)(ScreenDimensions.x * 0.01f), (int)(ScreenDimensions.y * 0.01f), 0, 0};

        std::string debugInfo = 
        std::format("Player: ({:.2f},{:.2f})\n", _Player.Center.x, _Player.Center.y)
        + std::format("Score: {}\n", _Player.Score)
        + std::format("MouseScreen: ({},{})\n", MouseScreen.x, MouseScreen.y)
        + std::format("MouseWorld: ({},{})\n", MouseWorld.x, MouseWorld.y)
        + std::format("MinimumCoordinates: X={},Y={})\n", MinCoordinates.x, MinCoordinates.y) 
        + std::format("TileCounts: X={}, Y={}\n", TileCounts.x, TileCounts.y)
        + std::format("Offsets: X={:.2f}, Y={:.2f}\n", TileOffset.x, TileOffset.y);

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
    int x = (int)((MouseScreen.x + TileOffset.x - ScreenRemainderOffset.x + PlayerDimensionOffset.x) / TileLength),
        y = (int)((MouseScreen.y + TileOffset.y - ScreenRemainderOffset.y + PlayerDimensionOffset.y) / TileLength);

    MouseWorld = {x + MinCoordinates.x, y + MinCoordinates.y};

    _Player.CanMine ? SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255) : SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);

    int rendX = (int)x * TileLength - TileOffset.x + ScreenRemainderOffset.x - PlayerDimensionOffset.x,
        rendY = (int)y * TileLength - TileOffset.y + ScreenRemainderOffset.y - PlayerDimensionOffset.y;

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
    float rendX0 = (xStart - MinCoordinates.x) * TileLength - TileOffset.x + ScreenRemainderOffset.x - PlayerDimensionOffset.x;
    float rendX1 = (xEnd + 1 - MinCoordinates.x) * TileLength - TileOffset.x + ScreenRemainderOffset.x - PlayerDimensionOffset.x;
    float rendY0 = (yStart - MinCoordinates.y) * TileLength - TileOffset.y + ScreenRemainderOffset.y - PlayerDimensionOffset.y;
    float rendY1 = (yEnd + 1 - MinCoordinates.y) * TileLength - TileOffset.y + ScreenRemainderOffset.y - PlayerDimensionOffset.y;

    SDL_RenderDrawLineF(Renderer, rendX0, rendY0, rendX1, rendY0);
    SDL_RenderDrawLineF(Renderer, rendX0, rendY0, rendX0, rendY1);
    SDL_RenderDrawLineF(Renderer, rendX1, rendY0, rendX1, rendY1);
    SDL_RenderDrawLineF(Renderer, rendX0, rendY1, rendX1, rendY1);
}

void WindowRenderer::DrawPlayerVector()
{
    // todo
}