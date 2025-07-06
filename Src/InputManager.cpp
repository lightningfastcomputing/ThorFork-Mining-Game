#include "InputManager.h"

InputManager::InputManager(World &world, Player *player, std::vector<Player *> &players, WindowRenderer &renderer, SoundManager &soundManager) : _World(world),
                                                                                                                                                 _Player(player), _Players(players), _Renderer(renderer), _SoundManager(soundManager)
{
    Keys = SDL_GetKeyboardState(NULL);
    Running = true;

    MovementInputs[UP] = {SDL_SCANCODE_W, 0, 0, nullptr};
    MovementInputs[DOWN] = {SDL_SCANCODE_S, 0, 0, nullptr};
    MovementInputs[LEFT] = {SDL_SCANCODE_A, 0, 0, nullptr};
    MovementInputs[RIGHT] = {SDL_SCANCODE_D, 0, 0, nullptr};

    MouseInputs = {250, 250, 0, 0};

    ActionInputs[REVEAL] = {SDL_SCANCODE_TAB, 500, 0, [this]()
                            {
                                this->_Renderer.Reveal();
                            }};
    ActionInputs[TOGGLE_DEBUG] = {SDL_SCANCODE_F1,
                                  500,
                                  0,
                                  [this]()
                                  { this->_Renderer.ToggleDebug(); }};

    ActionInputs[TOGGLE_FULLSCREEN] = {SDL_SCANCODE_F11,
                                       1000,
                                       0,
                                       [this]()
                                       { this->_Renderer.ToggleFullScreen(); }};

    ActionInputs[EXIT] = {SDL_SCANCODE_ESCAPE,
                          100,
                          0,
                          [this]()
                          {
                              // std::cout << "Exiting..." << std::endl;
                              this->Running = false;
                          }};

    ActionInputs[SWITCH_PLAYER_0] = {SDL_SCANCODE_1,
                                     100,
                                     0,
                                     [this]()
                                     {
                                         this->_Player = _Players[0];
                                         this->_Renderer._Player = _Players[0];
                                     }};
}

InputManager::~InputManager()
{
}

void InputManager::Update(Uint64 tickCount)
{
    SDL_PumpEvents();
    ValidatePlayerState();
    HandleMouseInput();

    HandleMovement();

    PollAndUpdate(REVEAL);
    PollAndUpdate(TOGGLE_DEBUG);
    PollAndUpdate(TOGGLE_FULLSCREEN);
    PollAndUpdate(EXIT);
    PollAndUpdate(SWITCH_PLAYER_0);
}

void InputManager::HandleMouseInput()
{
    int mouseState = SDL_GetMouseState(&(_Renderer.MouseScreen.x), &(_Renderer.MouseScreen.y));
    int selectedX = (int)_Renderer.MouseWorld.x, selectedY = (int)_Renderer.MouseWorld.y;
    Uint64 now = SDL_GetTicks64();

    if (mouseState & SDL_BUTTON(1) && (now - MouseInputs.LeftLastTimePressed) > MouseInputs.LeftCooldown)
    {
        if (_Player->CanMine && _Renderer.IsDiscovered(selectedX, selectedY))
        {
            _World.MineTile(selectedX, selectedY, 1, *_Player);
            _SoundManager.PlaySound(Sound::PICKAXE_STRIKE);
            MouseInputs.LeftLastTimePressed = now;
        }
    }
    else if (mouseState & SDL_BUTTON(3) && (now - MouseInputs.RightLastTimePressed) > MouseInputs.RightCooldown)
    {
        if (_Player->CanMine && _Renderer.IsDiscovered(selectedX, selectedY))
        {
            _World.ChangeTile(selectedX, selectedY, TileType::EXPLOSIVE);
            MouseInputs.RightLastTimePressed = now;
        }
    }
}

void InputManager::HandleMovement()
{
    int up = MovementInputs[UP].ScanCode, down = MovementInputs[DOWN].ScanCode,
        left = MovementInputs[LEFT].ScanCode, right = MovementInputs[RIGHT].ScanCode;

    bool horizontalInput = Keys[left] || Keys[right];
    bool verticalInput = Keys[up] || Keys[down];

    direction dir = NONE;
    if (Keys[up] && !horizontalInput)
        dir = NORTH;
    else if (Keys[down] && !horizontalInput)
        dir = SOUTH;
    else if (Keys[left] && !verticalInput)
        dir = WEST;
    else if (Keys[right] && !verticalInput)
        dir = EAST;
    else if (Keys[up] && Keys[left])
        dir = NORTHWEST;
    else if (Keys[up] && Keys[right])
        dir = NORTHEAST;
    else if (Keys[down] && Keys[left])
        dir = SOUTHWEST;
    else if (Keys[down] && Keys[right])
        dir = SOUTHEAST;
    _Player->UpdateVelocity(dir);
}

inline void InputManager::PollAndUpdate(int actionIndex)
{
    Input &input = ActionInputs[actionIndex];
    Uint64 now = SDL_GetTicks64();
    if (Keys[input.ScanCode] && (now - input.LastTimePressed) >= input.MillisecondCooldown)
    {
        input.Action();
        input.LastTimePressed = now;
    }
}

void InputManager::ValidatePlayerState() 
{
        Vec2F targetVector = _Player->Target - _Player->Center;
        if (targetVector.Magnitude() > _Player->MiningRadius)
        {
            _Player->CanMine = false;
        }
        else
        {
            int x = (int)_Player->Center.x, y = (int)_Player->Center.y, endX = (int)_Player->Target.x, endY = (int)_Player->Target.y;

            float dx = _Player->Target.x - _Player->Center.x;
            float dy = _Player->Target.y - _Player->Center.y;

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
                if (x == (int)_Player->Target.x && y == (int)_Player->Target.y)
                {
                    _Player->CanMine = true;
                    break;
                }
                else if (!_World.IsInBounds(x,y) || !_World.tiles[x][y].Passable) 
                {
                    _Player->CanMine = false;
                    break;
                }
            }
        }
}
