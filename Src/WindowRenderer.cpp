#include "WindowRenderer.h"

WindowRenderer::WindowRenderer(World &world, Player &player, int width, int height) : _World(world), _Player(player)
{
    ScreenWidth = width;
    ScreenHeight = height;

     TileLength = 30;

    HorizontalTileCount = ScreenWidth / TileLength;
    VerticalTileCount = ScreenHeight / TileLength;


    PlayerXOffset = ScreenWidth % TileLength / 2;
    PlayerYOffset = ScreenHeight % TileLength / 2;

    Discovered = new bool *[_World.Width];
    for (int i = 0; i < _World.Width; i++)
    {
        Discovered[i] = new bool[ScreenHeight];
        for (int j = 0; j < ScreenHeight; j++)
            Discovered[i][j] = false;
    }

    Debug = false;
    Running = true;
    Init_Display("Mining Game");
}
WindowRenderer::~WindowRenderer()
{
    if (Window)
        SDL_DestroyWindow(Window);
    if (Renderer)
        SDL_DestroyRenderer(Renderer);

    for (int i = 0; i < _World.Width; i++)
    {
        delete[] Discovered[i];
    }
    delete[] Discovered;
    for (int i = 0; i < 4; i++)
    {
        if (Textures[i])
        {
            SDL_DestroyTexture(Textures[i]);
        }
    }
}
void WindowRenderer::Init_Display(const char *windowTitle)
{
    Window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_ALLOW_HIGHDPI);
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

void WindowRenderer::Reveal()
{
    for (int i = 0; i < _World.Width; i++)
    {
        memset(Discovered[i], true, _World.Height);
    }
}
void WindowRenderer::Discover()
{
    int x = _Player.x, y = _Player.y;
    int xStart = x - 5, yStart = y - 5;
    int xEnd = x + 5, yEnd = y + 5;
    for (int i = xStart; i < xEnd; i++)
    {
        for (int j = yStart; j < yEnd; j++)
        {
            if (_World.IsInBounds(i, j) &&
                ((_World.IsInBounds(i - 1, j) && _World.tiles[i - 1][j] == AIR) ||
                 (_World.IsInBounds(i + 1, j) && _World.tiles[i + 1][j] == AIR) ||
                 (_World.IsInBounds(i, j - 1) && _World.tiles[i][j - 1] == AIR) ||
                 (_World.IsInBounds(i, j + 1) && _World.tiles[i][j + 1] == AIR)))
            {
                Discovered[i][j] = true;
            }
        }
    }
}

bool WindowRenderer::IsDiscovered(int x, int y)
{
    return Discovered[x][y];
}
void WindowRenderer::RenderFrame()
{
    TileXOffset = (_Player.x - (int)_Player.x) * TileLength;
    TileYOffset = (_Player.y - (int)_Player.y) * TileLength;

    ScreenXMin = (int)_Player.x - (HorizontalTileCount / 2);
    ScreenYMin = (int)_Player.y - (VerticalTileCount / 2);

    Discover();

    DrawWorld();
    DrawPlayer();
    DrawAndStoreSelectedTile();
    DebugInfo();

    SDL_RenderPresent(Renderer);
}

void WindowRenderer::ClearFrame()
{
    if (!SDL_RenderClear(Renderer))
        printf("RenderClear failed, %s\n", SDL_GetError());
    system("clear");
}

void WindowRenderer::ToggleDebug() { Debug = !Debug; }

void WindowRenderer::DrawWorld()
{
    SDL_Rect rect;
    rect.x = -TileXOffset - TileLength + PlayerXOffset;
    rect.y = -TileYOffset + PlayerYOffset;
    rect.w = TileLength;
    rect.h = TileLength;

    for (int i = ScreenXMin - 1; i < ScreenXMin + HorizontalTileCount + 2; i++)
    {
        for (int j = ScreenYMin; j < ScreenYMin + VerticalTileCount + 2; j++)
        {
            if (i < 0 || j < 0 || i > _World.Width - 1 || j > _World.Height - 1 || !Discovered[i][j])
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
        rect.y = -TileYOffset + PlayerYOffset;
    }
}

void WindowRenderer::DrawPlayer()
{
    SDL_Rect rect;
    rect.x = (ScreenWidth / 2) - (int)(_Player.HalfSize * TileLength);
    rect.y = (ScreenHeight / 2) - (int)(_Player.HalfSize * TileLength);
    rect.w = _Player.HalfSize * 2 * TileLength;
    rect.h = _Player.HalfSize * 2 * TileLength;

    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(Renderer, &rect);
}

void WindowRenderer::DebugInfo()
{
    if (Debug)
    {
        DrawPlayerCollisionBox();
        printf("(%f,%f) PLAYER\n", _Player.x, _Player.y);
        printf("DISTANCE FROM MOUSE AND PLAYER = %f\n", Utils::Distance(_Player.x + _Player.HalfSize, _Player.y + _Player.HalfSize, MouseWorldX, MouseWorldY));
        printf("(%d,%d) (%f,%f) MOUSE\n", MouseX, MouseY, MouseWorldX, MouseWorldY);
        printf("(%d,%d),(%d,%d) TILES COVERED\n", _Player.xStart, _Player.yStart, _Player.xEnd, _Player.yEnd);
        printf("SCORE: %d\n", _Player.Score);
        printf("\n");
        printf("TILELENGTH = %d\n", TileLength);
        printf("xTILECOUNT = %d, yTILECOUNT = %d\n", HorizontalTileCount, VerticalTileCount);
        printf("xOFF = %f, yOFF = %f\n", TileXOffset, TileYOffset);
        printf("xMIN = %d, yMIN = %d\n", ScreenXMin, ScreenYMin);
    }
}

void WindowRenderer::DrawAndStoreSelectedTile()
{
    float x = ((MouseX + TileXOffset - PlayerXOffset) / TileLength), y = ((MouseY + TileYOffset - PlayerYOffset) / TileLength);
    MouseWorldX = x + ScreenXMin;
    MouseWorldY = y + ScreenYMin;

    _Player.CanMine ? SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255) : SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);

    int rendX = (int)x * TileLength - TileXOffset + PlayerXOffset, rendY = (int)y * TileLength - TileYOffset + PlayerYOffset;

    if (_World.IsInBounds(MouseWorldX, MouseWorldY) && Discovered[(int)(MouseWorldX)][(int)(MouseWorldY)])
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
    float rendX0 = (xStart - ScreenXMin) * TileLength - TileXOffset + PlayerXOffset;
    float rendX1 = (xEnd + 1 - ScreenXMin) * TileLength - TileXOffset + PlayerXOffset;
    float rendY0 = (yStart - ScreenYMin) * TileLength - TileYOffset + PlayerYOffset;
    float rendY1 = (yEnd + 1 - ScreenYMin) * TileLength - TileYOffset + PlayerYOffset;

    SDL_RenderDrawLineF(Renderer, rendX0, rendY0, rendX1, rendY0);
    SDL_RenderDrawLineF(Renderer, rendX0, rendY0, rendX0, rendY1);
    SDL_RenderDrawLineF(Renderer, rendX1, rendY0, rendX1, rendY1);
    SDL_RenderDrawLineF(Renderer, rendX0, rendY1, rendX1, rendY1);
}

void WindowRenderer::DrawPlayerVector()
{
    // todo
}