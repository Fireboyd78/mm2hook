#include "mm2_lua.h"

using namespace LuaIntf;
using namespace MM2;

extern LuaState L;
extern CMidtownMadness2 *pMM2;

bool isMainLuaLoaded = false;

void mm2L_error(LPCSTR message)
{
    MM2::Abortf("[Lua] Error -- %s", message);
}

void luaAddGlobals(lua_State * L)
{
    typedef void(__cdecl *printer_type)(LPCSTR);

    LuaBinding(L)
        .addFunction("Printf", (printer_type)&Printf)
        .addFunction("Messagef", (printer_type)&Messagef)
        .addFunction("Displayf", (printer_type)&Displayf)
        .addFunction("Warningf", (printer_type)&Warningf)
        .addFunction("Errorf", (printer_type)&Errorf)
        .addFunction("Quitf", (printer_type)&Quitf)
        .addFunction("Abortf", (printer_type)&Abortf)

        .addFunction("AngelReadString", &AngelReadString);
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

void luaSetGlobals()
{
    if (!isMainLuaLoaded)
        return;

    mmGameManager *gameMgr = mmGameManager::Instance();

    auto pGame = (gameMgr != NULL) ? gameMgr->getGame() : NULL;
    auto pPlayer = (pGame != NULL) ? pGame->getPlayer() : NULL;
    auto pHUD = (pPlayer != NULL) ? pPlayer->getHUD() : NULL;

    if (pHUD != NULL)
        Lua::setGlobal(L, "hud", pHUD);
}

LUAMOD_API int luaopen_MM2(lua_State * L)
{
    LogFile::Write(" - Registering MM2 library...");

    LuaRef mod = LuaRef::createTable(L);
    auto modL = mod.state();

    luaAddGlobals(modL);
    luaAddModule_LogFile(modL);

    LuaBinding(L)
        .beginClass<datOutput>("datOutput")
            .addStaticFunction("OpenLog", &datOutput::OpenLog)
            .addStaticFunction("CloseLog", &datOutput::CloseLog)
            .addStaticFunction("SetOutputMask", &datOutput::SetOutputMask)
        .endClass()

        .beginClass<mmHUD>("mmHUD")
            .addFunction("SetMessage", static_cast<void (mmHUD::*)(LPCSTR, float, int)>(&mmHUD::SetMessage))
            .addFunction("PostChatMessage", &mmHUD::PostChatMessage)
        .endClass()

        .beginClass<mmPopup>("mmPopup")
            .addFunction("IsEnabled", &mmPopup::IsEnabled)
            .addFunction("Lock", &mmPopup::Lock)
            .addFunction("Unlock", &mmPopup::Unlock)
            .addFunction("ProcessChat", &mmPopup::ProcessChat)
        .endClass()

        .beginClass<Stream>("Stream")
            .addFactory([](LPCSTR filename, bool createFile = false) {
                auto stream = (createFile) ? Stream::Create(filename) : Stream::Open(filename, false);
                return stream;
            }, LUA_ARGS(LPCSTR, _opt<bool>))
            .addStaticFunction("DumpOpenFiles", &Stream::DumpOpenFiles)

            .addStaticFunction("Open", static_cast<Stream * (*)(LPCSTR, bool)>(&Stream::Open))
            .addStaticFunction("Create", static_cast<Stream * (*)(LPCSTR)>(&Stream::Create))

            .addFunction("Read", &Stream::Read)
            .addFunction("Write", &Stream::Write)
            .addFunction("GetCh", &Stream::GetCh)
            .addFunction("PutCh", &Stream::PutCh)
            .addFunction("Seek", &Stream::Seek)
            .addFunction("Tell", &Stream::Tell)
            .addFunction("Close", &Stream::Close)
            .addFunction("Size", &Stream::Size)
            .addFunction("Flush", &Stream::Flush)
        .endClass()

        .beginClass<Vector2>("Vector2")
            .addFactory([](float x = 0.0, float y = 0.0) {
                auto vec = new Vector2;
                vec->X = x;
                vec->Y = y;
                return vec;
            }, LUA_ARGS(_opt<float>, _opt<float>))
            .addVariableRef("x", &Vector2::X)
            .addVariableRef("y", &Vector2::Y)
        .endClass()

        .beginClass<Vector3>("Vector3")
            .addFactory([](float x = 0.0, float y = 0.0, float z = 0.0) {
                auto vec = new Vector3;
                vec->X = x;
                vec->Y = y;
                vec->Z = z;
                return vec;
            }, LUA_ARGS(_opt<float>, _opt<float>, _opt<float>))
            .addVariableRef("x", &Vector3::X)
            .addVariableRef("y", &Vector3::Y)
            .addVariableRef("z", &Vector3::Z)
        .endClass()

        .beginClass<Vector4>("Vector4")
            .addFactory([](float x = 0.0, float y = 0.0, float z = 0.0, float w = 0.0) {
                auto vec = new Vector4;
                vec->X = x;
                vec->Y = y;
                vec->Z = z;
                vec->W = w;
                return vec;
            }, LUA_ARGS(_opt<float>, _opt<float>, _opt<float>, _opt<float>))
            .addVariableRef("x", &Vector4::X)
            .addVariableRef("y", &Vector4::Y)
            .addVariableRef("z", &Vector4::Z)
            .addVariableRef("w", &Vector4::W)
        .endClass();

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

    LogFile::WriteLine("Lua script reloaded.");

    /*
    ** Disabled until I can fix this on the main menu **
    auto hud = Lua::getGlobal<MM2::mmHUD *>(L, "hud");

    if (hud != NULL)
        hud->SetMessage("Lua script reloaded.", 3.5, 0);
    */
}

bool MM2Lua::IsLoaded()
{
    return isMainLuaLoaded;
}

void MM2Lua::Initialize() {
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

void MM2Lua::Reset()
{
    if (!isMainLuaLoaded)
        return;

    luaSetGlobals();
}

void MM2Lua::OnTick()
{
    if (isMainLuaLoaded)
    {
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
    if (!isMainLuaLoaded)
        return;

    L.getGlobal("restart");
    if (L.pcall(0, 0, 0) != LUA_OK)
        mm2L_error(L.toString(-1));
}

void MM2Lua::OnShutdown()
{
    if (!isMainLuaLoaded)
        return;

    L.getGlobal("shutdown");
    if (L.pcall(0, 0, 0) != LUA_OK)
        mm2L_error(L.toString(-1));
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
