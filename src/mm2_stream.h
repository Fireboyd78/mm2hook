#pragma once
#include "mm2_common.h"

#include <modules\core.h>

namespace MM2
{
    // External declarations
    extern class Stream;

    template<>
    void luaAddModule<module_stream>(LuaState L) {
        luaBind<Stream>(L);
    }
}