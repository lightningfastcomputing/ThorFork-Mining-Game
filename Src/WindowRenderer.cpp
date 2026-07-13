#include "WindowRenderer.h"

WindowRenderer::WindowRenderer(const World &world, Camera &camera, int width, int height)
    : _Camera(camera), _World(world)
{
    WindowDimensions = {width, height};

    Discovered = new bool *[_World.Width];
    for (int i = 0; i < _World.Width; i++)
    {
        Discovered[i] = new bool[_World.Height];
        std::memset(Discovered[i], false, _World.Height * sizeof(bool));
    }

    Fullscreen = false;
    Debug = false;
    GlobalView = false;

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

    // set tileTextures
    TileTextures[TileType::AIR] = IMG_LoadTexture(Renderer, "Textures/grass.png");
    TileTextures[TileType::STONE_FLOOR] = IMG_LoadTexture(Renderer, "Textures/stone_floor.png");
    TileTextures[TileType::STONE] = IMG_LoadTexture(Renderer, "Textures/stone.png");
    TileTextures[TileType::DENSE_STONE] = IMG_LoadTexture(Renderer, "Textures/dense_rock.png");
    TileTextures[TileType::GOLD] = IMG_LoadTexture(Renderer, "Textures/gold.png");
    TileTextures[TileType::BARRIER] = IMG_LoadTexture(Renderer, "Textures/barrier.png");

    // set entityTextures
    EntityTextures[EntityType::PLAYER] = IMG_LoadTexture(Renderer, "Textures/barrier.png");
    EntityTextures[EntityType::MINECART_TRACK] = IMG_LoadTexture(Renderer, "Textures/MinecartTrack.png");
    EntityTextures[EntityType::DYNAMITE] = IMG_LoadTexture(Renderer, "Textures/explosive.png");
    EntityTextures[EntityType::MINECART] = IMG_LoadTexture(Renderer, "Textures/Minecart.png");

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

void WindowRenderer::Update(Uint64 tickCount)
{
    TickCount = tickCount;
    if (TickCount % 5 == 0)
        RadialDiscover();
    ClearFrame();
    RenderFrame();
}

void WindowRenderer::UpdateWindow()
{
    SDL_GetWindowSize(Window, &WindowDimensions.x, &WindowDimensions.y);

    _Camera.TileCounts = WindowDimensions.ToVec2F() / _Camera.TileLength;
    _Camera.WindowDim = WindowDimensions;
}

void WindowRenderer::ToggleGlobalView() { GlobalView = !GlobalView; }

void WindowRenderer::ToggleDebug() { Debug = !Debug; }

void WindowRenderer::ToggleCursorMode()
{
    SDL_bool toggle = (SDL_bool)_Camera._Player->InteractMode;
    if (SDL_SetRelativeMouseMode(toggle) < 0)
        printf("SDL_SetRelativeMouseMode failed: %s", SDL_GetError());
    else
        SDL_WarpMouseInWindow(Window, _Camera.MouseCoords.x, _Camera.MouseCoords.y);
}

void WindowRenderer::ToggleFullScreen()
{
    Fullscreen = !Fullscreen;
    Uint32 flag = Fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
    if (SDL_SetWindowFullscreen(Window, flag) < 0)
    {
        printf("SDL_SetWindowFullscreen failed: %s", SDL_GetError());
    }
}

void WindowRenderer::Outline(float x, float y, float w, float h)
{
    Vec2F rend = _Camera.WorldToScreen({x, y});
    w *= _Camera.TileLength;
    h *= _Camera.TileLength;

    SDL_RenderDrawLineF(Renderer, rend.x, rend.y, rend.x + w, rend.y);
    SDL_RenderDrawLineF(Renderer, rend.x, rend.y, rend.x, rend.y + h);
    SDL_RenderDrawLineF(Renderer, rend.x + w, rend.y, rend.x + w, rend.y + h);
    SDL_RenderDrawLineF(Renderer, rend.x, rend.y + h, rend.x + w, rend.y + h);
}

void WindowRenderer::Encircle(Vec2F pos, float radius)
{
    Vec2F rend = _Camera.WorldToScreen(pos);

    float dtheta = (2 * PI) / CIRCLE_SEGMENTS;
    float theta = 0;

    Vec2F p0 = rend + (Vec2F(radius * SDL_cosf(theta), radius * SDL_sinf(theta)) * _Camera.TileLength);

    for (int i = 0; i < CIRCLE_SEGMENTS; i++)
    {
        theta += dtheta;

        Vec2F p1 = rend + (Vec2F(radius * SDL_cosf(theta), radius * SDL_sinf(theta)) * _Camera.TileLength);

        SDL_RenderDrawLineF(Renderer, p0.x, p0.y, p1.x, p1.y);

        p0 = p1;
    }
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
    return _World.InBounds({x, y}) && Discovered[x][y];
}

void WindowRenderer::RenderFrame()
{
    _Camera.Update();
    if (!GlobalView)
    {

        DrawWorld();
        DrawEntities();
        DrawHealthBar();
        if (_Camera._Player->InteractMode)
        {
            DrawPlayerReach();
            DrawCursor();
            HighlightTarget();
        }
        if (Debug)
            DebugInfo();
    }
    else
    {
        GlobalDrawWorld();
    }

    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255); // undiscovered tiles are just black
    DrawGameOver();

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
    int globalTileLength = _World.Height < _World.Width ? WindowDimensions.x / _World.Width : WindowDimensions.y / _World.Height;

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
                int textureIdx = _World.Tiles[i][j].TileType;
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

    // draw little squares for the tiles the players are inhabiting
    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    for (Entity *p : _World._Entities)
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
    Vec2F minCoord = _Camera.MinCoord;
    Vec2F maxCoord = _Camera.MaxCoord;

    Vec2F minCoordAdjusted = minCoord - (minCoord - minCoord.Trunc());
    Vec2F min = _Camera.WorldToScreen(minCoordAdjusted);
    SDL_FRect rect;
    rect.x = min.x;
    rect.y = min.y;
    rect.w = _Camera.TileLength;
    rect.h = _Camera.TileLength;

    for (int i = minCoord.x; i <= maxCoord.x; i++)
    {
        for (int j = minCoord.y; j <= maxCoord.y; j++)
        {
            if (_World.InBounds({i, j}) && Discovered[i][j])
            {
                int textureIdx = _World.Tiles[i][j].TileType;
                SDL_RenderCopyF(Renderer, TileTextures[textureIdx], nullptr, &rect);
            }
            rect.y += _Camera.TileLength;
        }
        rect.x += _Camera.TileLength;
        rect.y = min.y;
    }
}

void WindowRenderer::DrawEntities()
{
    // todo: if player is not within view, dont draw
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);

    for (Entity *e : _World._Entities)
    {
        Vec2F rend = _Camera.WorldToScreen(e->Position);

        SDL_FRect displayRect;
        displayRect.x = rend.x;
        displayRect.y = rend.y;
        displayRect.w = e->Dimensions.x * _Camera.TileLength;
        displayRect.h = e->Dimensions.y * _Camera.TileLength;

        if (e->Type == BLASTLING)
        {
            SDL_SetRenderDrawColor(Renderer, 40, 220, 70, 255);
            SDL_RenderFillRectF(Renderer, &displayRect);

            SDL_SetRenderDrawColor(Renderer, 10, 80, 20, 255);
            SDL_RenderDrawRectF(Renderer, &displayRect);
            continue;
        }

        SDL_Texture *texture;
        if (e->Type == CHUNK)
        {
            Chunk *c = static_cast<Chunk *>(e);
            texture = TileTextures[c->Material];
            SDL_RenderCopyF(Renderer, texture, nullptr, &displayRect);
            Outline(e->Position.x, e->Position.y, e->Dimensions.x, e->Dimensions.y);
        }
        else
        {
            texture = EntityTextures[e->Type];
            SDL_RenderCopyF(Renderer, texture, nullptr, &displayRect);
        }
    }
}

void WindowRenderer::DrawHealthBar()
{
    Player *player = _Camera._Player;

    const float x = 20.0f;
    const float y = 20.0f;
    const float totalWidth = 240.0f;
    const float height = 28.0f;
    const float gap = 5.0f;

    const float segmentWidth =
        (totalWidth - gap * (Player::MaxHealth - 1)) /
        Player::MaxHealth;

    SDL_FRect background = {
        x - 5.0f,
        y - 5.0f,
        totalWidth + 10.0f,
        height + 10.0f
    };

    SDL_SetRenderDrawColor(Renderer, 15, 15, 15, 255);
    SDL_RenderFillRectF(Renderer, &background);

    for (int i = 0; i < Player::MaxHealth; i++)
    {
        SDL_FRect segment = {
            x + i * (segmentWidth + gap),
            y,
            segmentWidth,
            height
        };

        if (i < player->Health)
        {
            SDL_SetRenderDrawColor(Renderer, 220, 45, 45, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(Renderer, 65, 65, 65, 255);
        }

        SDL_RenderFillRectF(Renderer, &segment);

        SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
        SDL_RenderDrawRectF(Renderer, &segment);
    }
}

void WindowRenderer::DrawGameOver()
{
    Player *player = _Camera._Player;

    if (player->Health > 0)
        return;

    // Darken the entire game view.
    SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 210);

    SDL_FRect overlay = {
        0.0f,
        0.0f,
        static_cast<float>(WindowDimensions.x),
        static_cast<float>(WindowDimensions.y)
    };

    SDL_RenderFillRectF(Renderer, &overlay);

    const char *title = "GAME OVER";
    SDL_Color titleColor = {220, 45, 45, 255};

    SDL_Surface *titleSurface =
        TTF_RenderText_Solid(TextFont, title, titleColor);

    if (!titleSurface)
        return;

    SDL_Texture *titleTexture =
        SDL_CreateTextureFromSurface(Renderer, titleSurface);

    if (!titleTexture)
    {
        SDL_FreeSurface(titleSurface);
        return;
    }

    const float titleScale = 2.5f;

    SDL_FRect titleRect = {
        (WindowDimensions.x -
         titleSurface->w * titleScale) / 2.0f,

        (WindowDimensions.y -
         titleSurface->h * titleScale) / 2.0f,

        titleSurface->w * titleScale,
        titleSurface->h * titleScale
    };

    SDL_RenderCopyF(
        Renderer,
        titleTexture,
        nullptr,
        &titleRect
    );

    SDL_DestroyTexture(titleTexture);
    SDL_FreeSurface(titleSurface);

    const char *subtitle = "Three explosions took you out.";
    SDL_Color subtitleColor = {255, 255, 255, 255};

    SDL_Surface *subtitleSurface =
        TTF_RenderText_Solid(
            TextFont,
            subtitle,
            subtitleColor
        );

    if (!subtitleSurface)
        return;

    SDL_Texture *subtitleTexture =
        SDL_CreateTextureFromSurface(
            Renderer,
            subtitleSurface
        );

    if (!subtitleTexture)
    {
        SDL_FreeSurface(subtitleSurface);
        return;
    }

    SDL_FRect subtitleRect = {
        (WindowDimensions.x - subtitleSurface->w) / 2.0f,
        titleRect.y + titleRect.h + 25.0f,
        static_cast<float>(subtitleSurface->w),
        static_cast<float>(subtitleSurface->h)
    };

    SDL_RenderCopyF(
        Renderer,
        subtitleTexture,
        nullptr,
        &subtitleRect
    );

    SDL_DestroyTexture(subtitleTexture);
    SDL_FreeSurface(subtitleSurface);
}

void WindowRenderer::DebugInfo()
{
    DrawPlayerCollisionBox();
    DrawPlayerVector();
    DrawPlayerBoundingBox();

    SDL_FRect textRect = {WindowDimensions.x * 0.01f, WindowDimensions.y * 0.01f, 0, 0};

    Player *player = _Camera._Player;
    int targetX = (int)player->Target.x;
    int targetY = (int)player->Target.y;

    std::string debugInfo = std::format(
        "Screen Width, Height: ({},{})\n"
        "Player: ({:.2f},{:.2f})\n"
        "Velocity: ({:.2f},{:.2f})\n"
        "Score: {}\n"
        "MouseScreen: ({},{})\n"
        "PlayerTarget: ({:.2f},{:.2f})\n"
        "MinCoord: ({:.2f},{:.2f})\n"
        "MaxCoord: ({:.2f},{:.2f})\n"
        "TileCounts: X={:.2f}, Y={:.2f}\n",
        WindowDimensions.x, WindowDimensions.y,
        player->Position.x, player->Position.y,
        player->Velocity.x, player->Velocity.y,
        player->Score,
        _Camera.MouseCoords.x, _Camera.MouseCoords.y,
        player->Target.x, player->Target.y,
        _Camera.MinCoord.x, _Camera.MinCoord.y,
        _Camera.MaxCoord.x, _Camera.MaxCoord.y,
        _Camera.TileCounts.x, _Camera.TileCounts.y);  

    if (player->SelectedEntity)
    {
        debugInfo += std::format("Selected Entity: {:p}\n", static_cast<void *>(player->SelectedEntity));
        debugInfo += _Camera._Player->SelectedEntity->DebugInfo();
    }

    debugInfo += std::format("Entities: {}\n", _World._Entities.size()) +
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

    SDL_RenderCopyF(Renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void WindowRenderer::HighlightTarget()
{
    Player *player = _Camera._Player;

    Vec2F lengths = {-1, -1};
    Vec2F coords = player->Target.ToVec2().ToVec2F();

    bool renderTarget = true;
    Entity *e = player->EquippedEntity; // equipped item gets highlighted, first priority
    if (e)
    {
        Entity *mc = _World.FindEntity(player->Target, MINECART);
        if (mc)
        {
            SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);
            Outline(mc->Position.x, mc->Position.y, mc->Dimensions.x, mc->Dimensions.y);
        }
    }
    if (!e) // if not equipped, then the one that's pointed at with the cursor
    {
        e = player->SelectedEntity;
    }
    if (e)
    {
        lengths = e->Dimensions;
        coords = e->Position;
        if (e->Type == EntityType::DYNAMITE)
        {
            Explosive *expl = static_cast<Explosive *>(e);
            SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
            Encircle(expl->Center, expl->ExplosionRadius);
        }
    }
    else
    {
        Vec2 tileCoords = player->Target.ToVec2();
        if (_World.InBounds(tileCoords) && IsDiscovered(tileCoords.x, tileCoords.y) && !_World.Tiles[tileCoords.x][tileCoords.y].Passable)
        {
            lengths = {1, 1};
            coords = player->Target.Floor();
        }
        else
        {
            renderTarget = false;
        }
    }

    player->CanMine ? SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 255) : SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);

    if (renderTarget)
    {
        Outline(coords.x, coords.y, lengths.x, lengths.y);
    }
}

void WindowRenderer::DrawPlayerReach()
{
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
    Encircle(_Camera.Position, _Camera._Player->MiningRadius);
}

void WindowRenderer::DrawCursor()
{
    int tileLength = _Camera.TileLength;
    Vec2F pos = _Camera.WorldToScreen(_Camera._Player->Target);
    pos.x -= tileLength / 8;
    pos.y -= tileLength / 8;

    SDL_FRect rect;
    rect.x = pos.x;
    rect.y = pos.y;
    rect.w = tileLength / 4;
    rect.h = tileLength / 4;

    SDL_RenderCopyF(Renderer, TileTextures[DENSE_STONE], nullptr, &rect);
}

void WindowRenderer::RadialDiscover()
{
    float radius = _Camera._Player->DiscoverRadius;
    int numRays = (int)(2 * PI * radius) + 1; // aproximation of how many rays are needed to get a clean discovery radius
    float dTheta = (2 * PI) / numRays;
    float theta = 0;

    for (int i = 0; i < numRays; i++)
    {
        Vec2F endPos = {_Camera.Position.x + (radius * SDL_cosf(theta)), _Camera.Position.y + (radius * SDL_sinf(theta))};

        int x = (int)_Camera.Position.x;
        int y = (int)_Camera.Position.y;
        int endX = (int)(endPos.x);
        int endY = (int)(endPos.y);

        float dx = endPos.x - _Camera.Position.x;
        float dy = endPos.y - _Camera.Position.y;

        int stepX = (dx > 0) ? 1 : (dx < 0) ? -1
                                            : 0;
        int stepY = (dy > 0) ? 1 : (dy < 0) ? -1
                                            : 0;

        float tDeltaX = (stepX != 0) ? SDL_fabsf(1.0f / dx) : 1e6;
        float tDeltaY = (stepY != 0) ? SDL_fabsf(1.0f / dy) : 1e6;

        float nextTileBoundaryX = (stepX > 0) ? (SDL_floorf(_Camera.Position.x) + 1.0f) : (SDL_floorf(_Camera.Position.x));
        float nextTileBoundaryY = (stepY > 0) ? (SDL_floorf(_Camera.Position.y) + 1.0f) : (SDL_floorf(_Camera.Position.y));

        float tMaxX = (stepX != 0) ? SDL_fabsf((nextTileBoundaryX - _Camera.Position.x) / dx) : 1e6;
        float tMaxY = (stepY != 0) ? SDL_fabsf((nextTileBoundaryY - _Camera.Position.y) / dy) : 1e6;

        while (x != endX || y != endY)
        {
            Discovered[x][y] = true;
            if (!_World.Tiles[x][y].Passable)
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
    Vec2F pos = _Camera._Player->Position, dim = _Camera._Player->Dimensions;
    Outline(pos.x, pos.y, dim.x, dim.y);
}

void WindowRenderer::DrawPlayerCollisionBox()
{
    int xS = _Camera._Player->xStart,
        yS = _Camera._Player->yStart,
        xE = _Camera._Player->xEnd,
        yE = _Camera._Player->yEnd;
    SDL_SetRenderDrawColor(Renderer, 255, 20, 255, 255);
    Outline(xS, yS, (xE - xS + 1), (yE - yS + 1));
}

void WindowRenderer::DrawPlayerVector()
{
    Vec2F rend0 = _Camera.WorldToScreen(_Camera.Position);
    Vec2F rend1 = _Camera.WorldToScreen(_Camera._Player->Target);

    SDL_RenderDrawLineF(Renderer, rend0.x, rend0.y, rend1.x, rend1.y);
}
