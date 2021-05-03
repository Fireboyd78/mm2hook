#include "mm2.h"
#include "mm2_lua.h"
#include "mm2_network.h"
#include "luafilesystem/lfs.h"
#include <imgui\luabindings\imgui_lua_bindings.cpp>

using namespace LuaIntf;
using namespace MM2;

LuaState L;

bool isMainLuaLoaded = false;

void mm2L_error(LPCSTR message)
{
    MM2::Errorf("[Lua] Error -- %s", message);
}

template <class retType, typename... T>
retType tryCallFunction(LuaRef func, T&&... args)
{
    if (func.isValid()) {
        if (!func.isFunction()) {
            mm2L_error("Tried to call a LuaRef that's not a function.");
            return;
        }

        try 
        {
            return func.call<retType>(std::forward<T>(args)...);
        }
        catch (LuaException le) 
        {
            mm2L_error(le.what());
        }
    }
}


void tryCallFunction(LuaRef func)
{
    /*if (func.isValid()) {
        if (!func.isFunction()) {
            mm2L_error("Tried to call a LuaRef that's not a function.");
            return;
        }

        try
        {
            func.call();
        }
        catch (LuaException le)
        {
            mm2L_error(le.what());
        }
    }*/
    tryCallFunction<void>(func);
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

    Lua::setGlobal(L, "HUD", pHUD);
    Lua::setGlobal(L, "Game", pGame);
    Lua::setGlobal(L, "Player", pPlayer);
    Lua::setGlobal(L, "ROOT", &ROOT);

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
    // note that order matters. any beginExtendClass<>
    // which is missing a parent class will CTD before the game even starts!!
    luaAddModule<module_base>(modL);
    luaAddModule<module_inst>(modL);
    luaAddModule<module_ai>(modL);
    luaAddModule<module_audio>(modL);
    luaAddModule<module_bound>(modL);
    luaAddModule<module_breakable>(modL);
    luaAddModule<module_camera>(modL);
    luaAddModule<module_common>(modL);
    luaAddModule<module_creature>(modL);
    luaAddModule<module_data>(modL);
    luaAddModule<module_game>(modL);
    luaAddModule<module_gfx>(modL);
    luaAddModule<module_input>(modL);
    luaAddModule<module_particle>(modL);
    luaAddModule<module_level>(modL);
    luaAddModule<module_city>(modL);
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

            //call init
            LuaRef func(L, "init");
            tryCallFunction(func);
        }

        //mm2L_error(L.toString(-1));
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

    if (gamePtr != NULL && gamePtr->getPlayer() != NULL)
    {
        auto hud = gamePtr->getPlayer()->getHUD();
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
        L.require("lfs", luaopen_lfs);
        L.pop();

        ImguiBindLua(L);

        //set LFS path
        LogFile::WriteLine("Setting lfs path...");
        char execPath[MAX_PATH];
        GetModuleFileNameA(NULL, execPath, sizeof(execPath));
        PathRemoveFileSpecA(execPath);

        LuaRef(L, "lfs").get("chdir").call(execPath);

        //override print function
        SendCommand("print = function(x) Displayf(tostring(x)) end");

        //
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
    tryCallFunction<void>(func, message);
}

void MM2Lua::OnGameEnd() {
    LuaRef func(L, "onGameEnd");
    tryCallFunction(func);
}

void MM2Lua::OnGameInit() {
    LuaRef func(L, "onGameInit");
    tryCallFunction(func);
}

void MM2Lua::OnSessionCreate(char * sessionName, char * sessionPassword, int sessionMaxPlayers, NETSESSION_DESC * sessionData)
{
    LuaRef func(L, "onSessionCreate");
    tryCallFunction<void>(func, sessionName, sessionPassword, sessionMaxPlayers, sessionData);
}

void MM2Lua::OnSessionJoin(char * a2, GUID * a3, char * a4)
{
    LuaRef func(L, "onSessionJoin");
    tryCallFunction<void>(func, a2, a3, a4);
}

void MM2Lua::OnDisconnect() {
    LuaRef func(L, "onDisconnect");
    tryCallFunction(func);
}

void MM2Lua::OnReset() {
    LuaRef func(L, "onReset");
    tryCallFunction(func);
}

void MM2Lua::OnTick()
{
    if (isMainLuaLoaded) {
        LuaRef tickFunction(L, "tick");
        tryCallFunction(tickFunction);
    }

    // reset lastKey
    Lua::setGlobal(L, "lastKey", -1);
}

void MM2Lua::OnRestart()
{
    if (isMainLuaLoaded) {
        LuaRef func(L, "restart");
        tryCallFunction(func);
    }
}

void MM2Lua::OnShutdown()
{
    if (isMainLuaLoaded) {
        LuaRef func(L, "shutdown");
        tryCallFunction(func);
    }

    L.close();
}

void MM2Lua::OnRenderUi()
{
    LuaRef func(L, "onRenderUi");
    tryCallFunction(func);
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
    try 
    {
        Lua::exec(L, command);
    }
    catch (LuaException le) 
    {
        mm2L_error(le.what());
    }
}