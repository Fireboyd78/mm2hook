#pragma once
#include <mm2_common.h>

#include "input\keyboard.h"
#include "input\eventq.h"
#include "input\mouse.h"
#include "input\pad.h"
#include "input\joystick.h"

namespace MM2
{
    // Forward declarations


    // External declarations


    // Class definitions

    namespace $
    {
        namespace ioInput
        {
            declhook(0x4BA910, _Func<void>, Update);
        }
    }

    // Lua initialization

}