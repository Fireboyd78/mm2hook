#pragma once
#include "mm2_common.h"

static ConfigValue<bool> cfgEnableLua   ("EnableLua", true);

namespace MM2Lua
{
    bool IsEnabled();
    bool IsLoaded();

    void Initialize();
    void Reset();

    void OnTick();
    void OnRestart();
    void OnShutdown();
    void OnKeyPress(DWORD vKey);

    void SendCommand(LPCSTR command);
}