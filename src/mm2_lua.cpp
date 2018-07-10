#include "mm2.h"
#include "mm2_lua.h"
#include "mm2_network.h"

using namespace LuaIntf;
using namespace MM2;

LuaState L;

bool isMainLuaLoaded = false;

void mm2L_error(LPCSTR message)
{
    MM2::Abortf("[Lua] Error -- %s", message);
}

void luaAddModule_LogFile(lua_State * L)
{
    LuaBinding(L)
        .beginModule("LogFile")
            .addFunction("AppendLine", &LogFile::AppendLine)
            .addFunction("Write", &LogFile::Write)
            .addFunction("WriteLine", &LogFile::WriteLine)
        .endModule();
}

// these need to be here due to header file placement
// maybe this should be fixed...
void luaAddModule_Vector(lua_State *L)
{
    luaBind<Vector2>(L);
    luaBind<Vector3>(L);
    luaBind<Vector4>(L);
    luaBind<Matrix34>(L);
    luaBind<Matrix44>(L);
}

void luaSetGlobals()
{
    LogFile::Write("Updating Lua globals...");

    mmGameManager *gameMgr = mmGameManager::Instance;

    auto pGame = (gameMgr != NULL) ? gameMgr->getGame() : NULL;
    auto pPlayer = (pGame != NULL) ? pGame->getPlayer() : NULL;
    auto pHUD = (pPlayer != NULL) ? pPlayer->getHUD() : NULL;

    if (pHUD != NULL)
        Lua::setGlobal(L, "HUD", pHUD);
    if (pGame != NULL)
        Lua::setGlobal(L, "Game", pGame);
    if (pPlayer != NULL)
        Lua::setGlobal(L, "Player", pPlayer);

    LogFile::WriteLine("Done!");
}

LUAMOD_API int luaopen_MM2(lua_State *L)
{
    LogFile::Write(" - Registering MM2 library...");

    LuaRef mod = LuaRef::createTable(L);
    auto modL = mod.state();

    luaAddModule_LogFile(modL);
    luaAddModule_Vector(modL);

    // register all Lua modules
    // empty modules will be safely ignored
    luaAddModule<module_ai>(modL);
    luaAddModule<module_audio>(modL);
    luaAddModule<module_base>(modL);
    luaAddModule<module_bound>(modL);
    luaAddModule<module_breakable>(modL);
    luaAddModule<module_city>(modL);
    luaAddModule<module_common>(modL);
    luaAddModule<module_creature>(modL);
    luaAddModule<module_data>(modL);
    luaAddModule<module_game>(modL);
    luaAddModule<module_gfx>(modL);
    luaAddModule<module_input>(modL);
    luaAddModule<module_inst>(modL);
    luaAddModule<module_level>(modL);
    luaAddModule<module_model>(modL);
    luaAddModule<module_network>(modL);
    luaAddModule<module_phys>(modL);
    luaAddModule<module_rgl>(modL);
    luaAddModule<module_stream>(modL);
    luaAddModule<module_ui>(modL);
    luaAddModule<module_vehicle>(modL);

    mod.pushToStack();

    LogFile::WriteLine("Done!");

    return 1;
}

void LoadMainScript() {
    //char lua_file[MAX_PATH];
    //MM2::datAssetManager::FullPath(lua_file, sizeof(lua_file), "lua", "main", "lua");

    LPCSTR lua_file = ".\\lua\\main.lua";

    if (file_exists(lua_file))
    {
        int status = L.loadFile(lua_file) || L.pcall(0, 0, 0);

        if (isMainLuaLoaded = (status == LUA_OK))
        {
            LogFile::Format(" - Loaded script file: %s\n", lua_file);

            // this is kinda important, no?
            luaSetGlobals();

            L.getGlobal("init");
            if (L.pcall(0, 0, 0) == LUA_OK)
                return;
        }

        mm2L_error(L.toString(-1));
    }
    else
    {
        LogFile::Format(" - COULD NOT FIND FILE '%s'!!!\n", lua_file);
    }
}

void ReloadScript()
{
    // try reloading Lua
    LogFile::WriteLine("Reloading main script...");
    LoadMainScript();

    mmGameManager *mgr = mmGameManager::Instance;
    auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;

    if (gamePtr != NULL)
    {
        auto hud = Lua::getGlobal<MM2::mmHUD *>(L, "hud");

        if (hud != NULL)
            hud->SetMessage("Lua script reloaded.", 3.5, 0);
    }
    else
    {
        LogFile::WriteLine("Lua script reloaded.\n");
    }
}

LuaState * MM2Lua::GetState() {
    return &L;
}

bool MM2Lua::IsEnabled()
{
    return cfgEnableLua;
}

bool MM2Lua::IsLoaded()
{
    return cfgEnableLua && isMainLuaLoaded;
}

void MM2Lua::Initialize() {
    if (cfgEnableLua) {
        if (isMainLuaLoaded)
            mm2L_error("Tried to initialize the Lua engine twice!");

        LogFile::WriteLine("Initializing Lua...");

        L = LuaState::newState();

        L.openLibs();
        L.require("MM2", luaopen_MM2);
        L.pop();

        LogFile::WriteLine("Loading main script...");
        LoadMainScript();
    }
}

void MM2Lua::Reset()
{
    luaSetGlobals();
}

void MM2Lua::OnChatMessage(char* message) {
    LuaRef func(L, "onChatMessage");
    if (func != NULL)
        func.call(message);
}

void MM2Lua::OnGameEnd() {
    LuaRef func(L, "onGameEnd");
    if (func != NULL)
        func.call();
}

void MM2Lua::OnGameInit() {
    LuaRef func(L, "onGameInit");
    if (func != NULL)
        func.call();
}

void MM2Lua::OnSessionCreate(char * sessionName, char * sessionPassword, int sessionMaxPlayers, NETSESSION_DESC * sessionData)
{
    LuaRef func(L, "onSessionCreate");
    if (func != NULL)
        func.call(sessionName, sessionPassword, sessionMaxPlayers, sessionData);
}

void MM2Lua::OnSessionJoin(char * a2, GUID * a3, char * a4)
{
    LuaRef func(L, "onSessionJoin");
    if (func != NULL)
        func.call(a2, a3, a4);
}

void MM2Lua::OnDisconnect() {
    LuaRef func(L, "onDisconnect");
    if (func != NULL)
        func.call();
}

void MM2Lua::OnReset() {
    LuaRef func(L, "onReset");
    if (func != NULL)
        func.call();
}

void MM2Lua::OnTick()
{
    if (isMainLuaLoaded) {
        // don't do any errors since this is called EVERY tick
        // testing should be done in the lua script (if needed)
        L.getGlobal("tick");
        L.pcall(0, 0, 0);
    }

    // reset lastKey
    Lua::setGlobal(L, "lastKey", -1);
}

void MM2Lua::OnRestart()
{
    if (isMainLuaLoaded) {
        L.getGlobal("restart");
        if (L.pcall(0, 0, 0) != LUA_OK)
            mm2L_error(L.toString(-1));
    }
}

void MM2Lua::OnShutdown()
{
    if (isMainLuaLoaded) {
        L.getGlobal("shutdown");
        if (L.pcall(0, 0, 0) != LUA_OK)
            mm2L_error(L.toString(-1));
    }

    L.close();
}

void MM2Lua::OnKeyPress(DWORD vKey)
{
    Lua::setGlobal(L, "lastKey", vKey);

    switch (vKey)
    {
        case VK_F5:
        {
            ReloadScript();
        } break;
    }
}

void MM2Lua::SendCommand(LPCSTR command)
{
    LogFile::Format("> [Lua]: %s\n", command);
    luaL_dostring(L, command);
}