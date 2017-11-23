#pragma once
#include "mm2_common.h"
#include "mm2_static2.h"

namespace MM2
{
    // Forward declarations
    class vehBreakable;
    class vehBreakableMgr;

    // External declarations
    extern class modStatic;
    
    class vehBreakable {
    private:
        /*0x00*/bool isAttached; // still attached to the parent?
        /*0x04*/int index; // index into the list of breakables

        /*0x08*/byte _buffer[0x40];

        /*0x48*/vehBreakable *next;
    public:
        AGE_API vehBreakable(modStatic **a1, Matrix34 *a2, int a3, int a4, int a5)
                                                            { ageHook::Thunk<0x4D8530>::Call<void>(this); }
        
        AGE_API void Add(vehBreakable *breakable)           { ageHook::Thunk<0x4D8BE0>::Call<void>(this, breakable); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehBreakable>("vehBreakable")
                .addFunction("Add", &Add)
            .endClass();
        }
    };

    class vehBreakableMgr {
    private:
        vehBreakable *first; // linked-list
        float maxImpact; // educated guess (default: 10000.0)
        Matrix34 *matrix; // origin?
        int variant; // always zero by default, used in Eject to set Banger variant

        // TODO: figure out what these do

        float p5; // (default: 4.0)
        float p6; // (default: 0.0)
        float p7; // (default: 2.0)
        float p8; // (default: 0.0)
    public:
        AGE_API vehBreakableMgr()                           { ageHook::Thunk<0x4D8580>::Call<void>(this); }
        AGE_API ~vehBreakableMgr()                          { ageHook::Thunk<0x4D85B0>::Call<void>(this); }

        AGE_API void Add(LPCSTR a1, LPCSTR a2, modStatic **a3, int a4, int a5)     
                                                            { ageHook::Thunk<0x4D86A0 >::Call<void>(this, a1, a2, a3, a4, a5); }
        AGE_API void Create(LPCSTR a1, LPCSTR a2, modStatic **a3, int a4, int a5)
                                                            { ageHook::Thunk<0x4D8600 >::Call<void>(this, a1, a2, a3, a4, a5); }
        AGE_API void Eject(vehBreakable *a1, int room)      { ageHook::Thunk<0x4D8940>::Call<void>(this, a1, room); }
        AGE_API void EjectAll(int room)                     { ageHook::Thunk<0x4D8BB0>::Call<void>(this, room); }
        AGE_API vehBreakable * Get(int index)               { return ageHook::Thunk<0x4D86F0>::Call<vehBreakable *>(this, index); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehBreakableMgr>("vehBreakableMgr")
                .addConstructor(LUA_ARGS())
                .addFunction("Add", &Add)
                .addFunction("Create", &Create)
                .addFunction("Eject", &Eject)
                .addFunction("EjectAll", &EjectAll)
                .addFunction("Get", &Get)
            .endClass();
        }
    };

    ASSERT_SIZEOF(vehBreakable, 0x4C);
    ASSERT_SIZEOF(vehBreakableMgr, 0x20);

    template<>
    void luaAddModule<module_breakable>(LuaState L) {
        luaBind<vehBreakable>(L);
        luaBind<vehBreakableMgr>(L);
    }
}