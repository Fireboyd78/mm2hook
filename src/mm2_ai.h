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
        //LogFile::WriteLine("-- aiMap");
        luaBind<aiMap>(L);
        //LogFile::WriteLine("-- aiObstacle");
        luaBind<aiObstacle>(L);
        //LogFile::WriteLine("-- aiVehicle");
        luaBind<aiVehicle>(L);
        //LogFile::WriteLine("-- aiVehicleSpline");
        luaBind<aiVehicleSpline>(L);
        //LogFile::WriteLine("-- aiVehicleAmbient");
        luaBind<aiVehicleAmbient>(L);
        //LogFile::WriteLine("-- lvlAiMap");
        luaBind<lvlAiMap>(L);
        //LogFile::WriteLine("-- lvlAiRoad");
        luaBind<lvlAiRoad>(L);
        //LogFile::WriteLine("-- aiVehicleInstance");
        luaBind<aiVehicleInstance>(L);
        //LogFile::WriteLine("-- aiVehicleData");
        luaBind<aiVehicleData>(L);
    }
}