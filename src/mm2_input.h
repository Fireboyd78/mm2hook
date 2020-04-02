#pragma once
#include "mm2_common.h"

#include <modules\input.h>

namespace MM2
{
    template<>
    void luaAddModule<module_input>(LuaState L) {
        luaBind<ioKeyboard>(L);
    }
}