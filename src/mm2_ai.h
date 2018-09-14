#pragma once
#include "mm2_common.h"

#include <modules\ai.h>
#include <modules\city.h>

namespace MM2
{
    //External declarations
    extern class asNode;
    extern class Stream;
    extern class aiMap;
    extern class lvlAiMap;
    extern class lvlAiRoad;

    template<>
    void luaAddModule<module_ai>(LuaState L) {
        luaBind<aiMap>(L);
        luaBind<lvlAiMap>(L);
        luaBind<lvlAiRoad>(L);
    }
}