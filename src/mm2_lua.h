#pragma once
#include "mm2_common.h"

static ConfigValue<bool> cfgEnableLua   ("EnableLua", true);

namespace MM2Lua
{
    LuaState * GetState();

    bool IsEnabled();
    bool IsLoaded();

    void Initialize();
    void Reset();

    //events
    void OnChatMessage(char* message);

    void OnTick();
    void OnRestart();
    void OnShutdown();
    void OnKeyPress(DWORD vKey);

    void SendCommand(LPCSTR command);
}