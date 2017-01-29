#include "Input.h"

#include <libpad.h>
#include <libetc.h>

namespace Input
{
    typedef struct SixteenButtonInput
    {
        u_short buttonState;
    } SixteenButtonInput;

    typedef struct AnalogInput
    {
        u_short buttonState;
        u_char rightStickX;
        u_char rightStickY;
        u_char leftStickX;
        u_char leftStickY;
    } AnalogInput;

    typedef struct ControlPortState
    {
        u_char errorStatus;
        int halfSize : 4;
        ControllerType terminalType : 4;
        union
        {
            // Don't care about anything else LUL
            SixteenButtonInput sixteenInput;
            AnalogInput analogInput;
        } state;
    } ControlPortState;

    typedef struct Input
    {
        int controllerStates[2];
        u_short previousButtonStates[2];
        ControlPortState controlPadStates[2];

        union
        {
            struct
            {
                u_char recvBufs[2][34];
            } rawBufs;
            ControlPortState controlPortStates[2];
        } rawInputData;
    } Input;

    static int gButtonMapping[] =
    {
        PADLleft, PADLright, PADLup, PADLdown,
        PADRleft, PADRdown, PADRright, PADRup,
        PADL1, PADL2, PADi, PADR1,
        PADR2, PADj, PADstart, PADselect
    };
    static Input gInput;

    u_char gPadActAlign[] = { 0x00, 0x01, 0xff, 0xff, 0xff, 0xff };
    u_char gPadAct[6] = { 0, 0, 0, 0, 0, 0 };

    void Initialize(int flags)
    {
        PadInitDirect(gInput.rawInputData.rawBufs.recvBufs[0], gInput.rawInputData.rawBufs.recvBufs[1]);
        PadSetAct(0x00, gPadAct, 6);
        PadSetActAlign(0x00, gPadActAlign);
        PadStartCom();
    }

    void Update()
    {
        int i;
        u_char* pByteSwap;
        u_char byteSwap;

        for (i = 0; i < 2; i++)
        {
            gInput.previousButtonStates[i] = gInput.controlPadStates[i].state.sixteenInput.buttonState;
            gInput.controllerStates[i] = PadGetState(i * 4);
            if (gInput.controllerStates[i] == PadStateDiscon)
                gInput.previousButtonStates[i] = 0;
            gInput.controlPadStates[i] = gInput.rawInputData.controlPortStates[i];

            // Seriously, Sony?
            pByteSwap = (u_char*)&gInput.controlPadStates[i].state.sixteenInput.buttonState;
            byteSwap = pByteSwap[1];
            pByteSwap[1] = ~pByteSwap[0];
            pByteSwap[0] = ~byteSwap;
        }
    }

    ControllerType GetControllerType(int playerIdx)
    {
        if (gInput.controllerStates[playerIdx] == PadStateDiscon
            || gInput.controlPadStates[playerIdx].errorStatus) return CONTROLLER_NONE;
        return gInput.controlPadStates[playerIdx].terminalType;
    }

    int ButtonDown(Button btn, int playerIdx)
    {
        return gInput.controlPadStates[playerIdx].state.sixteenInput.buttonState & gButtonMapping[btn]; 
    }

    int ButtonPressed(Button btn, int playerIdx)
    {
        int mapping = gButtonMapping[btn];
        return (gInput.controlPadStates[playerIdx].state.sixteenInput.buttonState & mapping)
            && !(gInput.previousButtonStates[playerIdx] & mapping);
    }

    StickState GetLeftStick(int playerIdx)
    {
        const AnalogInput* pAnalog = &gInput.controlPadStates[playerIdx].state.analogInput;
        StickState state = { pAnalog->leftStickX, pAnalog->leftStickY };
        return state;
    }

    StickState GetRightStick(int playerIdx)
    {
        const AnalogInput* pAnalog = &gInput.controlPadStates[playerIdx].state.analogInput;
        StickState state = { pAnalog->rightStickX, pAnalog->rightStickY };
        return state;
    }
}
