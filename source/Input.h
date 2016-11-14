#ifndef _INPUT_H_
#define _INPUT_H_

#include "Global.h"

namespace Input
{
    enum ControllerType
    {
        CONTROLLER_NONE = 0,
        CONTROLLER_MOUSE = 1, CONTROLLER_16BUTANALOG = 2, CONTROLLER_GUN14 = 3,
        CONTROLLER_16BUT = 4, CONTROLLER_ANALOGJOY = 5, CONTROLLER_GUN34 = 6,
        CONTROLLER_ANALOG = 7, CONTROLLER_MULTITAP = 8
    };

    enum Button
    {
        BUTTON_DLEFT, BUTTON_DRIGHT, BUTTON_DUP, BUTTON_DDOWN,
        BUTTON_SQ, BUTTON_X, BUTTON_O, BUTTON_TRI,
        BUTTON_L1, BUTTON_L2, BUTTON_L3, BUTTON_R1, BUTTON_R2, BUTTON_R3,
        BUTTON_START, BUTTON_SELECT
    };

    struct StickState
    {
        u_char x;
        u_char y;
    };

    void Input::Initialize(int flags);
    void Input::Update();
    int Input::ButtonDown(Button btn, int playerIdx);
    int Input::ButtonPressed(Button btn, int playerIdx);
    StickState Input::GetLeftStick(int playerIdx);
    StickState Input::GetRightStick(int playerIdx);
    ControllerType Input::GetControllerType(int playerIdx);
}

#endif