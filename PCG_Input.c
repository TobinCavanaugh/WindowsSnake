//
// Created by tobin on 5/3/2023.
//


#include "PCG_Input.h"


PCG_Point PCG_GetMousePos(HWND *window) {
    POINT p;
    if (GetCursorPos(&p)) {
        ScreenToClient(*window, &p);
    }

    PCG_Point pcgPoint = {
            .x = p.x,
            .y = p.y,
    };

    return pcgPoint;
}

int controllerSupport = 1;

void ToggleControllerSupport(int newState) {
    controllerSupport = newState;
}

//Initialize it to 255 values, this isnt strictly needed but it gives me cushion if we expand and add more keys in the future
char currentKeystates[0xFF] = {0};
char oldKeystates[0xFF] = {0};

Controller currentController;
Controller oldControllerState;

KeyEvent *array_KeyEvents;
int keyEventCount = 0;

HWND _currentWindow;

void HandleKeySubscriptions(int index, KeyEventType type) {
    //Get Key
    for (int j = 0; j < keyEventCount; j++) {
        if (array_KeyEvents[j].key == index && array_KeyEvents[j].type == type) {
            if (array_KeyEvents[j].window == NULL || *array_KeyEvents[j].window == _currentWindow) {
                array_KeyEvents[j].Function();
//
//                if (array_KeyEvents[j].key >= (char) 0xEE) {
//                    printf("%d | ", j);
//                }
            }
        }
    }
}

XINPUT_STATE controllerState;

//TODO support for multiple controllers ??!??


void PCG_InitInput() {
    ZeroMemory(&controllerState, sizeof(XINPUT_STATE));
}

void AssignControllerValues(XINPUT_STATE state, Controller *c) {
    c->DPad_Up = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
    c->DPad_Down = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
    c->DPad_Left = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
    c->DPad_Right = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
    c->LeftStick_X = state.Gamepad.sThumbLX / 32767.0f;
    c->LeftStick_Y = state.Gamepad.sThumbLY / 32767.0f;
    c->RightStick_X = state.Gamepad.sThumbRX / 32767.0f;
    c->RightStick_Y = state.Gamepad.sThumbRY / 32767.0f;
    c->LeftTrigger = state.Gamepad.bLeftTrigger;
    c->RightTrigger = state.Gamepad.bRightTrigger;
    c->North_Button = (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
    c->South_Button = (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
    c->East_Button = (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
    c->West_Button = (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
    c->LeftBumper = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
    c->RightBumper = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
    c->Back_Button = (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
    c->Start_Button = (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
}

float deltaTime = 0;
float timer = 0;
float vibrationTime = 0;

/// Call this at the beginning of your renderloop
void PCG_InputFrameStart() {

    //This is for vibration
    if (timer < FLT_MAX - deltaTime) {
        timer += deltaTime;
    }

    if (timer > vibrationTime) {
        XINPUT_VIBRATION vibration = {0};
        XInputSetState(0, &vibration);
    }

    _currentWindow = GetForegroundWindow();

    //Update the controller values
    if (controllerSupport) {
        XInputGetState(0, &controllerState);
        AssignControllerValues(controllerState, &currentController);
    }

    //Loop through each possible key
    for (int i = 0; i < 0xFF; i++) {

        //Is that key currently pressed down?
        bool isKeyDown = 0;

        //This is for controller input
        if (i >= 0xEE) {

            //Getting the right button
            switch (i) {
                case CONTROLLER_DPAD_UP:
                    isKeyDown = currentController.DPad_Up;
                    break;
                case CONTROLLER_DPAD_DOWN:
                    isKeyDown = currentController.DPad_Down;
                    break;
                case CONTROLLER_DPAD_LEFT:
                    isKeyDown = currentController.DPad_Left;
                    break;
                case CONTROLLER_DPAD_RIGHT:
                    isKeyDown = currentController.DPad_Right;
                    break;
                case CONTROLLER_BUTTON_NORTH:
                    isKeyDown = currentController.North_Button;
                    break;
                case CONTROLLER_BUTTON_SOUTH:
                    isKeyDown = currentController.South_Button;
                    break;
                case CONTROLLER_BUTTON_EAST:
                    isKeyDown = currentController.East_Button;
                    break;
                case CONTROLLER_BUTTON_WEST:
                    isKeyDown = currentController.West_Button;
                    break;
                case CONTROLLER_LEFT_BUMPER:
                    isKeyDown = currentController.LeftBumper;
                    break;
                case CONTROLLER_RIGHT_BUMPER:
                    isKeyDown = currentController.RightBumper;
                    break;
                case CONTROLLER_BACK_BUTTON:
                    isKeyDown = currentController.Back_Button;
                    break;
                case CONTROLLER_START_BUTTON:
                    isKeyDown = currentController.Start_Button;
                    break;
                default:
                    isKeyDown = false;
            }

        } else {
            isKeyDown = GetAsyncKeyState(i) & 0x8000;
        }

        //The rest is for keyboard input

        //If it is...
        if (isKeyDown) {

            //It is for sure being held
            HandleKeySubscriptions(i, KE_Key);

            //If it switched from not being down to being down
            if (!currentKeystates[i]) {
                currentKeystates[i] = true;


                //Do the key down subscriptions
                HandleKeySubscriptions(i, KE_Down);
            }
        }
            //Key is not down
        else {
            //If it was just down
            if (currentKeystates[i]) {

                //Key up subscriptions
                currentKeystates[i] = false;
                HandleKeySubscriptions(i, KE_Up);
            }
        }
    }
}

///Call this at the end of your renderloop
void PCG_InputFrameEnd(float _deltaTime) {
    memcpy(oldKeystates, currentKeystates, sizeof(currentKeystates));
    oldControllerState = currentController;
    deltaTime = _deltaTime;
}

/// When the key is held down
/// Call with a KEY_{key} enum and the window you care about
/// \param key
/// \param window
/// \return
int PCG_GetKey(HWND *hwnd, int key) {
    return currentKeystates[key] && _currentWindow == *hwnd;
}

/// When the key is pressed, called once per press
/// Call with a KEY_{key} enum and the window you care about
/// \param hwnd
/// \param key
/// \return
int PCG_GetKeyDown(HWND *hwnd, int key) {

    if(hwnd == NULL){
        return (currentKeystates[key] && !oldKeystates[key]);
    }

    if (currentKeystates[key] && !oldKeystates[key]) {
        return _currentWindow == *hwnd;
    }

    return 0;
}

/// When the key is released
/// Call with a KEY_{key} enum and the window you care about
/// \param hwnd
/// \param key
/// \return
int PCG_GetKeyUp(HWND *hwnd, int key) {

    if(hwnd == NULL){
        return !currentKeystates[key] && oldKeystates[key];
    }

    if (!currentKeystates[key] && oldKeystates[key]) {
        return _currentWindow == *hwnd;
    }

    return 0;
}

/// Calls the KeyFunction when the type, key, and window states are fulfilled
/// \param key The key to be tested for
/// \param KeyFunction The method to be called
/// \param type The Key Event Type: KE_Down, KE_Up, KE_Key
/// \param win The window that this should work with
/// \returns Returns a keyevent struct pointer, this can be unsubscribed using PCG_UnregisterKeyEvent
KeyEvent *PCG_RegisterKeyEvent(char key, void (*KeyFunction)(void), KeyEventType type, HWND *win) {

    //Reallocate a new array with the increased size
    array_KeyEvents = realloc(array_KeyEvents, (keyEventCount + 1) * sizeof(KeyEvent));

    //Set the keyevent and increment the count

    array_KeyEvents[keyEventCount] = (KeyEvent) {key, type, win, KeyFunction};

    keyEventCount++;

    return &array_KeyEvents[keyEventCount - 1];
}

/// Unregister a key event
/// \param key
/// \param KeyFunction
/// \param type
void PCG_UnregisterKeyEvent(KeyEvent *event) {
    for (int i = 0; i < keyEventCount; i++) {
        //If they match
        if (&array_KeyEvents[i] == event) {

            // shift consecutive elements one to the left
            for (int j = i; j < keyEventCount - 1; j++) {
                array_KeyEvents[j] = array_KeyEvents[j + 1];
            }

            //Reallocate it
            array_KeyEvents = realloc(array_KeyEvents, sizeof(KeyEvent) * (keyEventCount - 1));
            keyEventCount--;
            return;
        }
    }
}

/// Toggles the cursor to the newstate
/// \param newState
void PCG_CursorToggle(int newState) {
    if (newState) {
        ShowCursor(TRUE);
    } else {
        ShowCursor(FALSE);
    }
}

Controller *PCG_GetControllerData() {
    return &currentController;
}

/// If the absolute value of the value is greater than or equal to the deadzone, we return it, otherwise we return 0
/// \param value
/// \param deadZone
/// \return
float DeadZone(float value, float deadZone) {
    if (fabs(value) >= deadZone) {
        return value;
    }

    return 0;
}

/// Get the left stick
/// \return
PCG_Point PCG_GetControllerStickLeft(HWND *hwnd, float deadZone) {
    if (*hwnd == _currentWindow) {

        float x = DeadZone(currentController.LeftStick_X, deadZone);
        float y = DeadZone(currentController.LeftStick_Y, deadZone);

        return (PCG_Point) {x, y};
    }

    return PCG_PointZero;
}

/// Get the right stick input
/// \return
PCG_Point PCG_GetControllerStickRight(HWND *hwnd, float deadZone) {
    if (*hwnd == _currentWindow) {

        float x = 0;
        if (fabsf(currentController.RightStick_X) >= deadZone) {
            x = currentController.RightStick_X;
        }

        float y = 0;
        if (fabsf(currentController.RightStick_Y) >= deadZone) {
            y = currentController.RightStick_Y;
        }

        return (PCG_Point) {x, y};
    }

    return PCG_PointZero;
}

int PCG_GetControllerButton(HWND *hwnd, int key) {
    if (*hwnd == _currentWindow) {
        switch (key) {
            case CONTROLLER_DPAD_UP:
                return currentController.DPad_Up;
            case CONTROLLER_DPAD_DOWN:
                return currentController.DPad_Down;
            case CONTROLLER_DPAD_LEFT:
                return currentController.DPad_Left;
            case CONTROLLER_DPAD_RIGHT:
                return currentController.DPad_Right;
            case CONTROLLER_BUTTON_NORTH:
                return currentController.North_Button;
            case CONTROLLER_BUTTON_SOUTH:
                return currentController.South_Button;
            case CONTROLLER_BUTTON_EAST:
                return currentController.East_Button;
            case CONTROLLER_BUTTON_WEST:
                return currentController.West_Button;
            case CONTROLLER_LEFT_BUMPER:
                return currentController.LeftBumper;
            case CONTROLLER_RIGHT_BUMPER:
                return currentController.RightBumper;
            case CONTROLLER_BACK_BUTTON:
                return currentController.Back_Button;
            case CONTROLLER_START_BUTTON:
                return currentController.Start_Button;
            default:
                return 0;
        }
        return 0;
    }

    return 0;
}


/// Y is the left, X is the right
/// \param hwnd
/// \param deadZone
/// \return

PCG_Point PCG_GetControllerTriggers(HWND *hwnd, float deadZone) {
    float l = DeadZone(currentController.LeftTrigger, deadZone);
    float r = DeadZone(currentController.RightTrigger, deadZone);

    return (PCG_Point) {l / 255.0f, r / 255.0f};
}

/// Values are between 0 and 1
/// \param hwnd
/// \param lMotor
/// \param rMotor
/// \param time
void PCG_ControllerVibrate(HWND *hwnd, float lMotor, float rMotor, float time) {

    if (*hwnd != _currentWindow) {
        return;
    }

    vibrationTime = time;
    timer = 0;

    XINPUT_VIBRATION vibration = {0};

    vibration.wLeftMotorSpeed = (WORD) (lMotor * 65535.0f);
    vibration.wRightMotorSpeed = (WORD) (rMotor * 65535.0f);

    XInputSetState(0, &vibration);
}

//Haptics