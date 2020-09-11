#pragma once
#include <modules\ai.h>

#include "aiVehiclePhysics.h"

namespace MM2
{
    // Forward declarations
    class aiRouteRacer;

    // External declarations
    

    // Class definitions

    class aiRouteRacer : public aiVehiclePhysics {
    private:
        byte _buffer[0x24];
    public:
        aiRouteRacer(void)                                  DONOTCALL;
        aiRouteRacer(const aiRouteRacer &&)                 DONOTCALL;

        AGE_API int Finished()                              { return hook::Thunk<0x53D6E0>::Call<int>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiRouteRacer, aiVehiclePhysics>("aiRouteRacer")
                .addFunction("Finished", &Finished)
            .endClass();
        }
    };

    ASSERT_SIZEOF(aiRouteRacer, 0x9794);

    // Lua initialization

}