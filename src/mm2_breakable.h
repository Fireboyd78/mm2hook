#pragma once
#include "mm2_common.h"
#include "mm2_static2.h"

namespace MM2
{
    class vehBreakable {
    public:
        AGE_API vehBreakable(modStatic * * a1, class Matrix34 * a2, int a3, int a4, int a5)
                                                            { ageHook::Thunk<0x4D8530>::Call<void>(this); }
        
        AGE_API void Add(vehBreakable * a1)                 { ageHook::Thunk<0x4D8BE0>::Call<void>(this, a1); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehBreakable>("vehBreakable")
                .addConstructor(LUA_ARGS(modStatic * *, Matrix34 *, int, int, int))
                .addFunction("Add", &Add)
            .endClass();
        }
    private:
        byte _buffer[0x4C];
    };

    class vehBreakableMgr {
    public:
        AGE_API vehBreakableMgr()                           { ageHook::Thunk<0x4D8580>::Call<void>(this); }
        AGE_API ~vehBreakableMgr()                          { ageHook::Thunk<0x4D85B0>::Call<void>(this); }

        AGE_API void Add(char const * a1, char const * a2, class modStatic * * a3, int a4, int a5)     
                                                            { ageHook::Thunk<0x4D86A0 >::Call<void>(this, a1, a2, a3, a4, a5); }
        AGE_API void Create(char const * a1, char const * a2, class modStatic * * a3, int a4, int a5)
                                                            { ageHook::Thunk<0x4D8600 >::Call<void>(this, a1, a2, a3, a4, a5); }
        AGE_API void Eject(vehBreakable* a1, int a2)        { ageHook::Thunk<0x4D8940>::Call<void>(this, a1, a2); }
        AGE_API void EjectAll(int a1)                       { ageHook::Thunk<0x4D8BB0>::Call<void>(this, a1); }
        AGE_API vehBreakable* Get(int a1)                   { return ageHook::Thunk<0x4D86F0>::Call<vehBreakable*>(this, a1); }

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
    private:
        byte _buffer[0x20];
    };
}