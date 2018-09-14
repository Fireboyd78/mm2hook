#pragma once
#include "mm2_common.h"

#include <modules\creature.h>
#include <modules\level.h>
#include <modules\banger.h>
#include <modules\ped.h>

namespace MM2
{
    // External declarations
    extern class lvlInstance;

    template<>
    void luaAddModule<module_inst>(LuaState L) {
        luaBind<lvlInstance>(L);
    }
}