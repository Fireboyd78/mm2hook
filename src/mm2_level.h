#pragma once
#include "mm2_common.h"
#include "mm2_inst.h"

namespace MM2
{
    template<>
    void luaAddModule<module_level>(LuaState L) {
        luaBind<dgPathPoint>(L);
        luaBind<dgPath>(L);
        luaBind<dgPathSet>(L);
        luaBind<lvlLevel>(L);
    }
}