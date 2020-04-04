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
    extern class aiVehicleSpline;
    extern class aiVehicleAmbient;

    template<>
    void luaAddModule<module_ai>(LuaState L) {
        luaBind<aiPath>(L);
        luaBind<aiIntersection>(L);
        luaBind<aiMap>(L);
        luaBind<aiMapStats>(L);
        luaBind<aiObstacle>(L);
        luaBind<aiVehicle>(L);
        luaBind<aiVehicleSpline>(L);
        luaBind<aiVehicleAmbient>(L);
        luaBind<lvlAiMap>(L);
        luaBind<lvlAiRoad>(L);
        luaBind<aiVehicleInstance>(L);
        luaBind<aiVehicleData>(L);
        luaBind<aiVehicleManager>(L);
    }
}