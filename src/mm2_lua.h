#pragma once
#include "common.h"
#include "mm2.h"

namespace MM2Lua
{
    bool IsLoaded();

    void Initialize();
    void Reset();

    void OnTick();
    void OnRestart();
    void OnShutdown();
    void OnKeyPress(DWORD vKey);

    void SendCommand(LPCSTR command);
}