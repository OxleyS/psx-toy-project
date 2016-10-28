#ifndef _INPUT_H_
#define _INPUT_H_

#include "Global.h"

typedef enum ControllerType
{
    CONTROLLER_NONE = 0,
    CONTROLLER_MOUSE = 1, CONTROLLER_16BUTANALOG = 2, CONTROLLER_GUN14 = 3,
    CONTROLLER_16BUT = 4, CONTROLLER_ANALOGJOY = 5, CONTROLLER_GUN34 = 6,
    CONTROLLER_ANALOG = 7, CONTROLLER_MULTITAP = 8
} ControllerType;

typedef enum Button
{
    BUTTON_DLEFT, BUTTON_DRIGHT, BUTTON_DUP, BUTTON_DDOWN,
    BUTTON_SQ, BUTTON_X, BUTTON_O, BUTTON_TRI,
    BUTTON_L1, BUTTON_L2, BUTTON_L3, BUTTON_R1, BUTTON_R2, BUTTON_R3,
    BUTTON_START, BUTTON_SELECT
} Button;

typedef struct StickState
{
    u_char x;
    u_char y;
} StickState;

void Input_Initialize(int flags);
void Input_Update();
int Input_ButtonDown(Button btn, int playerIdx);
int Input_ButtonPressed(Button btn, int playerIdx);
StickState Input_GetLeftStick(int playerIdx);
StickState Input_GetRightStick(int playerIdx);
ControllerType Input_GetControllerType(int playerIdx);

#endif