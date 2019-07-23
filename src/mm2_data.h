#pragma once
#include "mm2_common.h"
#include "mm2_stream.h"

#include <modules\core.h>
#include <modules\data.h>

namespace MM2
{
    // External declarations
    extern class datOutput;
    extern class datTimeManager;

    template<>
    void luaAddModule<module_data>(LuaState L) {
        luaBind<datOutput>(L);
        luaBind<datTimeManager>(L);
    }
}