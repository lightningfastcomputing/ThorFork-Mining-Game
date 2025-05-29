#include "InputManager.h"

InputManager::InputManager(World &world, Player *player, std::vector<Player*> &players, WindowRenderer &renderer, SoundManager &soundManager) : _World(world),
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
                                { for (int i = 0; i < _World.Width; i++) 
                                    {
                                    memset(_Player->DiscoveredTiles[i], true, _World.Height * sizeof(bool));
                                    }
                                }
                            };
    ActionInputs[TOGGLE_DEBUG] = {SDL_SCANCODE_F1,
                                  500,
                                  0,
                                  [this]()
                                  { this->_Renderer.ToggleDebug(); }
                                };

    ActionInputs[TOGGLE_FULLSCREEN] = {SDL_SCANCODE_F11,
                                       1000,
                                       0,
                                       [this]()
                                       { this->_Renderer.ToggleFullScreen(); }
                                    };

    ActionInputs[EXIT] = {SDL_SCANCODE_ESCAPE,
                          100,
                          0,
                          [this]()
                          { 
                            std::cout << "Exiting..." << std::endl;
                            this->Running = false; 
                          }
                        };

    ActionInputs[SWITCH_PLAYER_0] = {SDL_SCANCODE_1,
                                    100,
                                    0,
                                    [this]()
                                    {
                                        this->_Player = _Players[0]; 
                                        this->_Renderer._Player = _Players[0];
                                    }
                                };

    ActionInputs[SWITCH_PLAYER_1] = {SDL_SCANCODE_2,
                                    100,
                                    0,
                                    [this]()
                                    { 
                                        this->_Player = _Players[1];
                                        this->_Renderer._Player = _Players[1];
                                    }
                                };
    ActionInputs[SWITCH_PLAYER_2] = {SDL_SCANCODE_3,
                                    100,
                                    0,
                                    [this]()
                                    { 
                                        this->_Player = _Players[2];
                                        this->_Renderer._Player = _Players[2];
                                    }
                                };
    ActionInputs[SWITCH_PLAYER_3] = {SDL_SCANCODE_4,
                                    100,
                                    0,
                                    [this]()
                                    { 
                                        this->_Player = _Players[3];
                                        this->_Renderer._Player = _Players[3];
                                    }
                                };
    ActionInputs[SWITCH_PLAYER_4] = {SDL_SCANCODE_5,
                                    100,
                                    0,
                                    [this]()
                                    { 
                                        this->_Player = _Players[4];
                                        this->_Renderer._Player = _Players[4];
                                    }
                                };
}

InputManager::~InputManager()
{
}

void InputManager::Update(Uint64 tickCount)
{
    SDL_PumpEvents();

    HandleMouseInput();

    HandleMovement();

    PollAndUpdate(REVEAL);
    PollAndUpdate(TOGGLE_DEBUG);
    PollAndUpdate(TOGGLE_FULLSCREEN);
    PollAndUpdate(EXIT);
    PollAndUpdate(SWITCH_PLAYER_0);
    PollAndUpdate(SWITCH_PLAYER_1);
    PollAndUpdate(SWITCH_PLAYER_2);
    PollAndUpdate(SWITCH_PLAYER_3);
    PollAndUpdate(SWITCH_PLAYER_4);
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

void InputManager::PollAndUpdate(int actionIndex)
{
    Input &input = ActionInputs[actionIndex];
    Uint64 now = SDL_GetTicks64();
    if (Keys[input.ScanCode] && (now - input.LastTimePressed) >= input.MillisecondCooldown)
    {
        input.Action();
        input.LastTimePressed = now;
    }
}

