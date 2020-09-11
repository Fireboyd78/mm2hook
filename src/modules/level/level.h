#pragma once
#include <modules\level.h>
#include <handlers\lua_drawable.h>

namespace MM2
{
    // Forward declarations
    class lvlLevel;

    // External declarations

    // Class definitions

    class lvlLevel {
    private:
        //lua drawables!
        inline void RegisterLuaDrawable(LuaRef self, LuaRef function, int phase = 1) {
            luaDrawableHandler::RegisterCallback(self, function, phase);
        }

    public:
        static hook::Type<lvlLevel> Singleton;

        AGE_API void MoveToRoom(lvlInstance *instance, int room) {
            hook::Thunk<0x465480>::Call<void>(this, instance, room);
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<lvlLevel>("lvlLevel")
                .addFunction("RegisterDrawable", &RegisterLuaDrawable, 
                    LUA_ARGS(LuaRef, LuaRef, _opt<int>)) //register a LuaCallback
                .addStaticProperty("Singleton", [] { return &Singleton; })
                .endClass();
        }
    };

    // Lua initialization

}