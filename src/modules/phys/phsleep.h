#pragma once
#include "phinertia.h"

namespace MM2
{
    // Forward declarations
    class phSleep;

    // External declarations


    // Class definitions
    enum class phSleepState : int
    {
        ASLEEP = 0x0,
        AWAKE = 0x1,
        UNK = 0x2,
    };


    class phSleep
    {
    private:
        int dword_00;
        int dword_04;
        int dword_08;
        Vector3 dword_0c;
        Vector3 LastAcceleration;
        phInertialCS* ICS;
        int Collider;
        int dword_2c;
        int dword_30;
        int AwakeFrameCount;
        int MaxAwakeFrames;
        float SomeMagTrigger;
        float AccelMagThreshold;
        float NextAwakeTime;
        phSleepState State;
        datCallback* AsleepCB;
    };
    ASSERT_SIZEOF(phSleep, 0x50);

    // Lua initialization

}