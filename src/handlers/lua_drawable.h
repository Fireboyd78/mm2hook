#pragma once
#include <mm2_common.h>
#include <modules/node.h>

//helper to get callbacks from lua
class luaCallback
{
public:
    LuaRef self;
    LuaRef function;

    //lua
    static void BindLua(LuaState L) {
        LuaBinding(L).beginClass<luaCallback>("luaCallback")
            .addFactory([](LuaRef self, LuaRef function = LuaRef()) {
            auto callback = new luaCallback();
            callback->self = self;
            callback->function = function;
        }, LUA_ARGS(LuaRef, _opt<LuaRef>))
            .endClass();
    }
};

class luaDrawableHandler {
public:
    static luaCallback LuaCallbacks[256];
    static int callbacksCount1;
    static int callbacksCount0;

    static void RegisterCallback(LuaRef self, LuaRef function, int phase);

    void CallCallbacks(int phase);
    void ResetCallbacks();

    static void Install();
};