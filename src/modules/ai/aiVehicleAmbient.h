#pragma once
#include <modules\ai.h>

#include "aiVehicleSpline.h"

namespace MM2
{
    // Forward declarations
    class aiVehicleAmbient;

    // External declarations


    // Class definitions

    class aiVehicleAmbient : public aiVehicleSpline {
    private:
        byte _buffer[0x14];
    public:
        aiVehicleAmbient(void)                              DONOTCALL;
        aiVehicleAmbient(const aiVehicleAmbient &&)         DONOTCALL;

        void Update(void) override                          FORWARD_THUNK;
        void Reset(void) override                           FORWARD_THUNK;
        int Type(void) override                             FORWARD_THUNK;
        void DrawId(void) override                          FORWARD_THUNK;
        void ReplayDebug(void) override                     FORWARD_THUNK;

        void Impact(int a1) override                        FORWARD_THUNK;

        //members
        AGE_API void Dump()                                 { ageHook::Thunk<0x551A80>::Call<void>(this); }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiVehicleAmbient, aiVehicleSpline>("aiVehicleAmbient")
                .addFunction("Dump", &Dump)
                .endClass();
        }
    };

    ASSERT_SIZEOF(aiVehicleAmbient, 0x1A0);

    // Lua initialization

}