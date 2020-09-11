#include "lua_drawable.h"

using namespace MM2;

/*
    luaDrawableHandler
*/

int luaDrawableHandler::callbacksCount1 = 0;
int luaDrawableHandler::callbacksCount0 = 0;
luaCallback luaDrawableHandler::LuaCallbacks[256];

void luaDrawableHandler::RegisterCallback(LuaRef self, LuaRef function, int phase)
{
    //registering a callback during pause phase is not allowed
    if (ROOT->IsPaused())
        return;

    //verify
    if (!function.isFunction()) {
        Errorf("RegisterLuaDrawable: function input is not function type.");
        return;
    }

    //
    int count = (phase == 1) ? luaDrawableHandler::callbacksCount1 : luaDrawableHandler::callbacksCount0;

    //setup 
    auto callback = &LuaCallbacks[(phase * 128) + count];
    callback->function = function;
    callback->self = self;

    //increase counter
    if (phase == 1) {
        callbacksCount1++;
    }
    else {
        callbacksCount0++;
    }
}

void luaDrawableHandler::CallCallbacks(int phase)
{
    //call original
    hook::Thunk<0x465630>::Call<void>(this, phase);

    //call lua callbacks
    int callbackOffset = phase * 128;
    int callbackCount = (phase == 0) ? callbacksCount0 : callbacksCount1;

    if (callbackCount > 128) {
        Errorf("luaDrawHandler: TOO MANY CALLBACKS! ");
        callbackCount = 128;
    }

    for (int i = callbackOffset; i < callbackOffset + callbackCount; i++) {
        auto callback = LuaCallbacks[i];
        if (callback.function.isValid() && callback.self.isValid()) {
            callback.function.call(callback.self);
        }
    }
}

void luaDrawableHandler::ResetCallbacks()
{
    if (ROOT->IsPaused())
        return;

    //call original resetcallbacks
    hook::Thunk<0x465680>::Call<void>(this);

    //reset our callbacks
    callbacksCount0 = 0;
    callbacksCount1 = 0;
}

void luaDrawableHandler::Install()
{
    InstallCallback("lvlLevel::CallCallbacks", "Hook for lua callbacks",
        &CallCallbacks, {
            cb::hook<CALL>(0x446217),
            cb::hook<CALL>(0x445D6F),
        }
    );

    InstallCallback("lvlLevel::ResetCallbacks", "Allows for control over when to clear callbacks.",
        &ResetCallbacks, {
            cb::hook<JMP>(0x465460),
        }
    );
}