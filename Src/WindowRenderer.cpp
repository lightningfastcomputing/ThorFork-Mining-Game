#include "WindowRenderer.h"

WindowRenderer::WindowRenderer(World &world, Player &player, int width, int height) : _World(world), _Player(player)
{
    Width = width;
    Height = height;

    MouseX = 0;
    MouseY = 0;

    MouseWorldX = -1;
    MouseWorldY = -1;

    TileLength = 60;

    PlayerXOffset = Width % TileLength / 2;
    PlayerYOffset = Height % TileLength / 2;

    Discovered = new bool *[_World.Width];
    for (int i = 0; i < _World.Width; i++)
    {
        Discovered[i] = new bool[Height];
        for (int j = 0; j < Height; j++)
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
    Window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, SDL_WINDOW_ALLOW_HIGHDPI);
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
    int horizontalTileCount = Width / TileLength;
    int verticalTileCount = Height / TileLength;

    xOffset = (_Player.x - (int)_Player.x) * TileLength;
    yOffset = (_Player.y - (int)_Player.y) * TileLength;

    int xMin = (int)_Player.x - (horizontalTileCount / 2);
    int yMin = (int)_Player.y - (verticalTileCount / 2);

    SDL_Rect rect;
    rect.x = -xOffset - TileLength + PlayerXOffset;
    rect.y = -yOffset + PlayerYOffset;
    rect.w = TileLength;
    rect.h = TileLength;

    Discover();

    for (int i = xMin - 1; i < xMin + horizontalTileCount + 2; i++)
    {
        for (int j = yMin; j < yMin + verticalTileCount + 2; j++)
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
        rect.y = -(int)yOffset;
    }

    // rect.x = horizontalTileCount / 2 * TileLength;
    // rect.y = verticalTileCount / 2 * TileLength;
    // rect.w = _Player.HalfSize * 2 * TileLength;
    // rect.h = _Player.HalfSize * 2 * TileLength;

    // drawing player square

    DrawPlayer();

    DrawAndStoreSelectedTile(xMin, yMin);

    if (Debug)
    {
        DrawPlayerCollisionBox(xMin, yMin);
        printf("(%f,%f) PLAYER\n", _Player.x, _Player.y);
        printf("DISTANCE FROM MOUSE AND PLAYER = %f\n", Utils::Distance(_Player.x + _Player.HalfSize, _Player.y + _Player.HalfSize, MouseWorldX, MouseWorldY));
        printf("(%d,%d) (%f,%f) MOUSE\n", MouseX, MouseY, MouseWorldX, MouseWorldY);
        printf("(%d,%d),(%d,%d) TILES COVERED\n", _Player.xStart, _Player.yStart, _Player.xEnd, _Player.yEnd);
        printf("SCORE: %d\n", _Player.Score);
        printf("\n");
        printf("TILELENGTH = %d\n", TileLength);
        printf("xTILECOUNT = %d, yTILECOUNT = %d\n", horizontalTileCount, verticalTileCount);
        printf("xOFF = %f, yOFF = %f\n", xOffset, yOffset);
        printf("xMIN = %d, yMIN = %d\n", xMin, yMin);
    }

    SDL_RenderPresent(Renderer);
}

void WindowRenderer::ClearFrame()
{
    SDL_RenderClear(Renderer);
    system("clear");
}

void WindowRenderer::ToggleDebug() { Debug = !Debug; }

void WindowRenderer::DrawPlayer()
{
    SDL_Rect rect;
    rect.x = (Width / 2) - static_cast<int>(_Player.HalfSize * TileLength);
    rect.y = (Height / 2) - static_cast<int>(_Player.HalfSize * TileLength);
    rect.w = _Player.HalfSize * 2 * TileLength;
    rect.h = _Player.HalfSize * 2 * TileLength;

    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(Renderer, &rect);
}

void WindowRenderer::DrawAndStoreSelectedTile(int minX, int minY)
{

    float x = ((MouseX + xOffset - PlayerXOffset) / TileLength), y = ((MouseY + yOffset - PlayerYOffset) / TileLength);
    MouseWorldX = x + minX;
    MouseWorldY = y + minY;

    if (_Player.CanMine)
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
    else
        SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);

    int rendX = (int)x * TileLength - xOffset + PlayerXOffset, rendY = (int)y * TileLength - yOffset + PlayerYOffset;

    if (_World.IsInBounds(MouseWorldX, MouseWorldY) && Discovered[static_cast<int>(MouseWorldX)][static_cast<int>(MouseWorldY)])
    {
        SDL_RenderDrawLine(Renderer, rendX, rendY, rendX + TileLength, rendY);
        SDL_RenderDrawLine(Renderer, rendX, rendY, rendX, rendY + TileLength);
        SDL_RenderDrawLine(Renderer, rendX + TileLength, rendY, rendX + TileLength, rendY + TileLength);
        SDL_RenderDrawLine(Renderer, rendX, rendY + TileLength, rendX + TileLength, rendY + TileLength);
    }
}

void WindowRenderer::DrawPlayerBoundingBox(int TileLength, int xRem)
{
}

void WindowRenderer::DrawPlayerCollisionBox(int minX, int minY)
{
    // float x = _Player.x, y = _Player.y, size = _Player.Size;
    int xStart = _Player.xStart;
    int yStart = _Player.yStart;
    int xEnd = _Player.xEnd;
    int yEnd = _Player.yEnd;

    SDL_SetRenderDrawColor(Renderer, 255, 20, 255, 255);
    float rendX0 = (xStart - minX) * TileLength - xOffset + PlayerXOffset;
    float rendX1 = (xEnd + 1 - minX) * TileLength - xOffset + PlayerXOffset;
    float rendY0 = (yStart - minY) * TileLength - yOffset + PlayerYOffset;
    float rendY1 = (yEnd + 1 - minY) * TileLength - yOffset + PlayerYOffset;
    SDL_RenderDrawLineF(Renderer, rendX0, rendY0, rendX1, rendY0);
    SDL_RenderDrawLineF(Renderer, rendX0, rendY0, rendX0, rendY1);
    SDL_RenderDrawLineF(Renderer, rendX1, rendY0, rendX1, rendY1);
    SDL_RenderDrawLineF(Renderer, rendX0, rendY1, rendX1, rendY1);
}

void WindowRenderer::DrawPlayerVector(int tileLength, int xRem)
{
}