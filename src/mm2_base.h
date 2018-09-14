#pragma once
#include "mm2_common.h"
#include "mm2_lua.h"

#include <modules\node.h>

namespace MM2
{
    // External declarations
    extern class asNode;

    template<>
    void luaAddModule<module_base>(LuaState L) {
        luaBind<asNode>(L, "asNode");
    }
}