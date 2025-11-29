#include "InputManager.h"

InputManager::InputManager(World &world,
                           Player *player,
                           Camera &camera,
                           WindowRenderer &renderer,
                           SoundManager &SoundManager) : _World(world),
                                                         _Player(player),
                                                         _Camera(camera),
                                                         _Renderer(renderer),
                                                         _SoundManager(SoundManager)

{
    Keys = SDL_GetKeyboardState(NULL);
    Running = true;

    MovementInputs[UP] = {SDL_SCANCODE_W, 0, 0, nullptr};
    MovementInputs[DOWN] = {SDL_SCANCODE_S, 0, 0, nullptr};
    MovementInputs[LEFT] = {SDL_SCANCODE_A, 0, 0, nullptr};
    MovementInputs[RIGHT] = {SDL_SCANCODE_D, 0, 0, nullptr};

    MouseInputs.LeftCooldown = 500;
    MouseInputs.RightCooldown = 1000;

    ActionInputs[REVEAL] = {SDL_SCANCODE_F2, 500, 0, [this]()
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
                              this->Running = false;
                          }};

    ActionInputs[SWITCH_PLAYER_0] = {SDL_SCANCODE_1,
                                     100,
                                     0,
                                     [this]()
                                     {
                                         printf("teehee\n");
                                     }};
    ActionInputs[TOGGLE_CURSOR_MODE] = {SDL_SCANCODE_TAB,
                                        500,
                                        0,
                                        [this]()
                                        {
                                            this->_Player->InteractMode = !this->_Player->InteractMode;
                                            _Renderer.ToggleCursorMode();
                                        }};
    ActionInputs[TOGGLE_VIEW] = {SDL_SCANCODE_F,
                                 500,
                                 0,
                                 [this]()
                                 {
                                     this->_Renderer.ToggleGlobalView();
                                 }};
    ActionInputs[INTERACT] = {SDL_SCANCODE_E,
                              500,
                              0,
                              [this]()
                              {
                                  this->PlayerInteract();
                              }};
    ActionInputs[SPAWN_MINECART] = {SDL_SCANCODE_C,
                                    500,
                                    0,
                                    [this]()
                                    {
                                        this->_World.SpawnEntity(_Player->Target, MINECART);
                                    }};
    ActionInputs[SPAWN_TRACK] = {SDL_SCANCODE_T,
                                 50,
                                 0,
                                 [this]()
                                 {
                                     this->_World.SpawnEntity(_Player->Target, MINECART_TRACK);
                                 }};
    ActionInputs[MOUNT] = {SDL_SCANCODE_SPACE,
                           1000,
                           0,
                           [this]()
                           {
                               if (!_Player->Parent)
                               {
                                   Entity *e = this->_World.FindEntity(_Player->Target, MINECART);
                                   if (e)
                                   {
                                       Minecart *mc = static_cast<Minecart *>(e);
                                       mc->AddEntity(_Player);
                                   }
                               }
                               else
                               {
                                   Minecart *mc = static_cast<Minecart *>(_Player->Parent);
                                   mc->RemoveEntity(_Player);
                               }
                           }};
}

InputManager::~InputManager()
{
}

void InputManager::Update(Uint64 tickCount)
{
    SDL_PumpEvents();
    ValidatePlayerState();

    HandleMovement();
    HandleMouseInput();

    PollAndUpdate(REVEAL);
    PollAndUpdate(TOGGLE_DEBUG);
    PollAndUpdate(TOGGLE_FULLSCREEN);
    PollAndUpdate(EXIT);
    PollAndUpdate(SWITCH_PLAYER_0);
    PollAndUpdate(TOGGLE_CURSOR_MODE);
    PollAndUpdate(TOGGLE_VIEW);
    PollAndUpdate(INTERACT);
    PollAndUpdate(SPAWN_MINECART);
    PollAndUpdate(SPAWN_TRACK);
    PollAndUpdate(MOUNT);
}

void InputManager::HandleMouseInput()
{

    int mouseState;
    if (_Player->InteractMode)
    {
        mouseState = SDL_GetRelativeMouseState(&_Camera.MouseDeltas.x, &_Camera.MouseDeltas.y);
    }
    else
    {
        mouseState = SDL_GetMouseState(&_Camera.MouseCoords.x, &_Camera.MouseCoords.y);
    }

    _Camera.MouseCoords += _Camera.MouseDeltas/2;
    _Player->Target = _Camera.ScreenToWorld(_Camera.MouseCoords);
    //_Player->Target += _Camera.MouseDeltas.ToVec2F() / _Camera.TileLength / 2;


    Vec2F targetVector = _Player->Target - _Player->Center;
    if (targetVector.Magnitude() > _Player->MiningRadius)
    {
        targetVector.Normalize();
        _Player->Target = _Player->Center + targetVector * _Player->MiningRadius;
        _Camera.MouseCoords = _Camera.WorldToScreen(_Player->Target).ToVec2();
    }

    _Player->SelectedEntity = _World.FindEntity(_Player->Target, -1);

    Vec2 selected = _Player->Target.ToVec2();
    Uint64 now = SDL_GetTicks64();

    if (mouseState & SDL_BUTTON(1) && (now - MouseInputs.LeftLastTimePressed) > MouseInputs.LeftCooldown)
    {
        if (_Player->SelectedEntity)
        {
            _World.KillEntity(_Player->SelectedEntity);
        }
        else if (_Player->CanMine && _Renderer.IsDiscovered(selected.x, selected.y))
        {
            _World.MineTile(selected.x, selected.y, 5, true);
        }
        MouseInputs.LeftLastTimePressed = now;
    }
    else if (mouseState & SDL_BUTTON(3) && (now - MouseInputs.RightLastTimePressed) > MouseInputs.RightCooldown)
    {
        if (_Player->CanMine && _Renderer.IsDiscovered(selected.x, selected.y) && _World.Tiles[selected.x][selected.y].Passable)
        {
            _World.SpawnExplosive(_Player->Target.x, _Player->Target.y);
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
    _Player->UpdateAcceleration(dir);
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
    ValidateCanMine();
    if (_Player->EquippedEntity)
    {
        UpdateEquippedAcceleration();
    }
}

void InputManager::PlayerInteract()
{

    if (_Player->SelectedEntity && _Player->EquippedEntity != this->_Player && !_Player->EquippedEntity)
    {

        _Player->EquippedEntity = _Player->SelectedEntity;
        Minecart *mc = static_cast<Minecart *>(_World.FindEntity(_Player->Target, MINECART));
        if (mc && _Player->SelectedEntity != mc)
        {
            mc->RemoveEntity(_Player->EquippedEntity);
        }
        _Player->EquippedEntity->Killable = false;

        if (_Player->EquippedEntity->Type != MINECART)
        {
            _Player->EquippedEntity->RemoveCollision(MINECART);
            _Player->EquippedEntity->RemoveCollision(PLAYER);
        }
        lastDragCoeff = _Player->EquippedEntity->DragCoefficient;
        _Player->EquippedEntity->DragCoefficient = 0;
    }
    else if (_Player->EquippedEntity)
    {
        _Player->EquippedEntity->Killable = true;
        if (_Player->EquippedEntity->Type != MINECART)
        {
            _Player->EquippedEntity->AddCollision(MINECART);
            _Player->EquippedEntity->AddCollision(PLAYER);
        }

        Minecart *mc = static_cast<Minecart *>(_World.FindEntity(_Player->Target, MINECART));
        if (mc && _Player->EquippedEntity != mc)
        {
            mc->AddEntity(_Player->EquippedEntity);
        }
        _Player->EquippedEntity->DragCoefficient = lastDragCoeff;
        _Player->EquippedEntity = nullptr;
    }
}

void InputManager::UpdateEquippedAcceleration()
{
    Vec2F targetVector = _Player->Target - _Player->EquippedEntity->Center;
    _Player->EquippedEntity->Acceleration = targetVector;
}

void InputManager::ValidateCanMine()
{

    int x = (int)_Player->Center.x, y = (int)_Player->Center.y, endX = (int)_Player->Target.x, endY = (int)_Player->Target.y;

    float dx = _Player->Target.x - _Player->Center.x;
    float dy = _Player->Target.y - _Player->Center.y;

    int stepX = (dx > 0) ? 1 : (dx < 0) ? -1
                                        : 0;
    int stepY = (dy > 0) ? 1 : (dy < 0) ? -1
                                        : 0;

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
        else if (!_World.Tiles[x][y].Passable)
        {
            _Player->CanMine = false;
            break;
        }
    }
}
