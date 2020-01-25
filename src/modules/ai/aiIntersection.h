#pragma once
#include <modules\ai.h>

#include "aiPath.h"
#include "aiVehicle.h"
#include "aiTrafficLight.h"
#include "aiVehicle.h"

namespace MM2
{
    // Forward declarations
    class aiIntersection;

    // External declarations


    // Class definitions

    class aiIntersection {
    public:
        aiPath * *paths;
        int16_t pathCount;
        aiVehicle *prevVeh;
        aiVehicle *nextVeh;
        uint16_t roomId;
        uint16_t id;
        Vector3 center;
        aiTrafficLightSet *trafficLights;
        aiObstacle *vehicles;
        aiObstacle *bangers;
    private:
        //helpers
        inline int getPathCount() {
            return pathCount;
        }

        inline aiPath* getPath(int id) {
            if (id >= pathCount)
                return nullptr;
            return paths[id];
        }

        inline int getId() {
            return id;
        }

        inline int getRoomId() {
            return roomId;
        }

        inline Vector3 getCenter() {
            return center;
        }
    public:
        aiIntersection(void)                                DONOTCALL;
        aiIntersection(const aiIntersection &&)             DONOTCALL;

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<aiIntersection>("aiIntersection")
                .addFunction("GetPath", &getPath)
                .addPropertyReadOnly("Id", &getId)
                .addPropertyReadOnly("RoomId", &getRoomId)
                .addPropertyReadOnly("Center", &getCenter)
                .addPropertyReadOnly("NumPaths", &getPathCount)
                .endClass();
        }
    };

    ASSERT_SIZEOF(aiIntersection, 0x2C);

    // Lua initialization

}