#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward delcarations
    class dgPhysManager;

    // External declarations
    extern class lvlInstance;

    // Class definitions

    class dgPhysManager {
    public:
        static ageHook::Type<dgPhysManager *> Instance;

        void DeclareMover(lvlInstance* instance, int a2, int a3) { ageHook::Thunk<0x468370>::Call<void>(this, instance, a2, a3); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<dgPhysManager>("dgPhysManager")
                //statics
                .addStaticFunction("Instance", [] {return (dgPhysManager *)Instance; })

                //functions
                .addFunction("DeclareMover", &DeclareMover)

                .endClass();
        }
    };

    // Lua initialization

}