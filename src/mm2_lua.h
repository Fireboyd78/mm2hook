#pragma once
#include "mm2_common.h"
#include "mm2_network.h"

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
    void OnGameEnd();
    void OnGameInit();
    void OnReset();
    void OnDisconnect();
    void OnSessionCreate(char *sessionName, char *sessionPassword, int sessionMaxPlayers, MM2::NETSESSION_DESC *sessionData);
    void OnSessionJoin(char *a2, GUID *a3, char *a4);

    void OnTick();
    void OnRestart();
    void OnShutdown();
    void OnKeyPress(DWORD vKey);

    void SendCommand(LPCSTR command);
}