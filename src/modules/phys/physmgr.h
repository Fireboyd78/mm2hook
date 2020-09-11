#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class dgPhysManager;

    // External declarations
    extern class lvlInstance;

    // Class definitions

    class dgPhysManager {
    public:
        static hook::Type<dgPhysManager *> Instance;
        static hook::Type<float> Gravity;

        void IgnoreMover(lvlInstance* instance)                  { hook::Thunk<0x468860>::Call<void>(this, instance); }
        void DeclareMover(lvlInstance* instance, int a2, int a3) { hook::Thunk<0x468370>::Call<void>(this, instance, a2, a3); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<dgPhysManager>("dgPhysManager")
                //statics
                .addStaticFunction("Instance", [] {return (dgPhysManager *)Instance; })

                //functions
                .addFunction("IgnoreMover", &IgnoreMover)
                .addFunction("DeclareMover", &DeclareMover)

                .endClass();
        }
    };

    // Lua initialization

}