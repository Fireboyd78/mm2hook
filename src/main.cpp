#include "main.h"

LPFNDIRECTINPUTCREATE lpDICreate;

// Export as 'DirectInputCreateA' so we can hook into MM2
#pragma comment(linker, "/EXPORT:DirectInputCreateA=_DirectInputCreateA_Impl")
HRESULT NAKED DirectInputCreateA_Impl(HINSTANCE hinst, DWORD dwVersion, LPVOID *ppDI, LPUNKNOWN punkOuter)
{
    _asm jmp DWORD PTR ds:lpDICreate
}

CMidtownMadness2 *pMM2;

WNDPROC hProcOld;
LRESULT APIENTRY WndProcNew(HWND, UINT, WPARAM, LPARAM);

void InstallVTHook(DWORD dwHookAddr, DWORD dwNewAddr) {
    DWORD flOldProtect;

    VirtualProtect((LPVOID)dwHookAddr, 4, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(DWORD*)dwHookAddr = dwNewAddr;
    VirtualProtect((LPVOID)dwHookAddr, 4, flOldProtect, &flOldProtect);
}

void InstallVTHook(DWORD dwHookAddr, DWORD dwNewAddr, LPDWORD lpdwOldAddr) {
    *lpdwOldAddr = *(DWORD*)dwHookAddr;
    InstallVTHook(dwHookAddr, dwNewAddr);
}

void InstallPatch(DWORD dwAddress, BYTE *patchData, DWORD dwSize) {
    DWORD flOldProtect;

    VirtualProtect((LPVOID)dwAddress, dwSize, PAGE_EXECUTE_READWRITE, &flOldProtect);
    memcpy((LPVOID)dwAddress, patchData, dwSize);
    VirtualProtect((LPVOID)dwAddress, dwSize, flOldProtect, &flOldProtect);
};

void SetJmpDest(LPVOID patch, DWORD jmpOffset, LPVOID jmpDest, LPDWORD jmpStorage) {
    *jmpStorage = (DWORD)jmpDest;
    *(DWORD*)((BYTE*)patch + jmpOffset) = (DWORD)jmpStorage;
};

// TODO: separate these into separate functions
bool InstallJmpHook(DWORD dwAddress, DWORD jmpDest, bool useCall = false) {
    const LPVOID lpAddr = (LPVOID)dwAddress;
    const DWORD dwSize = 5; // size of jmp instruction
    DWORD relOffset = dwAddress + dwSize;

    int relJmpDest = (int)(jmpDest - relOffset);

    // hopefully this is good enough error checking
    // otherwise we're going to have some serious problems!
    if ((relOffset + relJmpDest) != jmpDest)
        return false;

    DWORD flOldProtect;

    VirtualProtect(lpAddr, dwSize, PAGE_EXECUTE_READWRITE, &flOldProtect);

    *(BYTE*)lpAddr = (useCall) ? 0xE8 : 0xE9; // '[CALL/JMP] REL32'
    *(int*)((BYTE*)lpAddr + 1) = relJmpDest;

    VirtualProtect(lpAddr, dwSize, flOldProtect, &flOldProtect);
    return true;
};

void luaAddGlobals(lua_State* L)
{
    using namespace MM2;
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

void luaAddModule_LogFile(lua_State* L)
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
    using namespace MM2;

    mmGameManager *gameMgr = mmGameManager::Instance();

    auto pGame = (gameMgr != NULL) ? gameMgr->getGame() : NULL;
    auto pPlayer = (pGame != NULL) ? pGame->getPlayer() : NULL;
    auto pHUD = (pPlayer != NULL) ? pPlayer->getHUD() : NULL;

    if (pHUD != NULL)
        Lua::setGlobal(L, "hud", pHUD);
}

LUAMOD_API int luaopen_MM2(lua_State* L)
{
    using namespace MM2;

    LogFile::Write(" - Registering MM2 library...");

    LuaRef mod = LuaRef::createTable(L);

    luaAddGlobals(mod.state());
    luaAddModule_LogFile(mod.state());

    LuaBinding(L)
        .beginClass<datOutput>("datOutput")
            .addStaticFunction("OpenLog", &datOutput::OpenLog)
            .addStaticFunction("CloseLog", &datOutput::CloseLog)
            .addStaticFunction("SetOutputMask", &datOutput::SetOutputMask)
        .endClass()

        .beginClass<mmHUD>("mmHUD")
            .addFunction("SetMessage", &mmHUD::SetMessage, LUA_ARGS(LPCSTR, _opt<float>, _opt<int>))
            .addFunction("SetMessage", &mmHUD::SetMessage2, LUA_ARGS(LPCSTR))
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

            .addStaticFunction("Open", &Stream::Open)
            .addStaticFunction("Create", LUA_FN(Stream*, Stream::Create, LPCSTR))

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

void InitializeLua() {
    using namespace MM2;
    typedef void(__cdecl *printer_type)(LPCSTR);

    LogFile::WriteLine("Initializing Lua...");

    L = LuaState::newState();

    L.openLibs();
    L.require("MM2", luaopen_MM2);
    L.pop();
}

bool isHookSetup = false;
bool isConsoleOpen = false;
bool isMainLuaLoaded = false;

void LoadMainScript() {
    char lua_file[MAX_PATH];
    MM2::datAssetManager::FullPath(lua_file, sizeof(lua_file), "lua", "main", "lua");

    if (file_exists(lua_file))
    {
        int status = L.loadFile(lua_file) || L.pcall(0, 0, 0);

        if (isMainLuaLoaded = (status == LUA_OK))
        {
            LogFile::Format(" - Loaded script file: %s\n", lua_file);

            L.getGlobal("init");
            if (L.pcall(0, 0, 0) == LUA_OK)
                return;
        }
        MM2::Abortf("[Lua] Error -- %s", L.toString(-1));
    }
}

bool SubclassGameWindow(HWND gameWnd, WNDPROC pWndProcNew, WNDPROC *ppWndProcOld)
{
    if (gameWnd != NULL)
    {
        WNDPROC hProcOld = (WNDPROC)GetWindowLong(gameWnd, GWL_WNDPROC);

        *ppWndProcOld = hProcOld;

        if (hProcOld != NULL && SetWindowLong(gameWnd, GWL_WNDPROC, (LONG)pWndProcNew))
            return true;
    }
    return false;
}

bool HandleKeyPress(DWORD vKey)
{
    Lua::setGlobal(L, "lastKey", vKey);

    switch (vKey) {
        // '~'
        case VK_OEM_2:
        case VK_OEM_3:
        {
            // tell the game to open a chat box,
            // and then use a local variable to check if it's open

            MM2::mmGameManager *mgr = MM2::mmGameManager::Instance();
            auto gamePtr = mgr->getGame();

            if (gamePtr != NULL)
            {
                auto popup = gamePtr->getPopup();

                if (popup != NULL) {
                    // don't try opening it again if it's already open
                    if (popup->IsEnabled() && isConsoleOpen)
                        return true;

                    popup->ProcessChat();
                    isConsoleOpen = true;
                }
            }
        } return true;
        case VK_F5:
        {
            // try reloading Lua
            LogFile::WriteLine("Reloading main script...");
            LoadMainScript();

            auto hud = Lua::getGlobal<MM2::mmHUD *>(L, "hud");

            if (hud != NULL)
                hud->SetMessage("Lua script reloaded.", 3.5, 0);
        } return true;
    }
    return false;
}

LRESULT APIENTRY WndProcNew(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            if (HandleKeyPress((DWORD)wParam))
                return 0;
        } break;
    }
    return CallWindowProc(hProcOld, hWnd, uMsg, wParam, lParam);
}

class TickHandler {
public:
    static void Reset(void) {
        // TODO: reset tick stuff
    }

    static void Update(void) {
        if (isMainLuaLoaded)
        {
            // don't do any errors since this is called EVERY tick
            // testing should be done in the lua script (if needed)
            L.getGlobal("tick");
            L.pcall(0, 0, 0);
        }

        // reset lastKey
        Lua::setGlobal(L, "lastKey", -1);

        // pass control back to MM2
        MM2::datTimeManager::Update();
    }
};

// needed to put this in a class for proper stack cleanup
// not meant to be initialized directly!
class ChatHandler {
public:
    void Process(char *message) {
        if (isConsoleOpen) {
            LogFile::Format("> [Lua]: %s\n", message);

            luaL_dostring(L, message);

            // return normal chatbox behavior
            isConsoleOpen = false;
        } else {
            LogFile::Format("Got chat message: %s\n", message);
        }
    }
};

void SetupHook() {
    InitializeLua();

    LogFile::WriteLine("Loading main script...");
    LoadMainScript();

    LogFile::Write("Setting MM2 output log file...");

    if (MM2::datOutput::OpenLog("mm2.log"))
        LogFile::Write("Done!\n");
    else
        LogFile::Write("FAIL!\n");

    bool subclassWindow = true;

    if (subclassWindow)
    {
        LogFile::Write("Subclassing window...");

        if (SubclassGameWindow(pMM2->GetMainWindowHwnd(), WndProcNew, &hProcOld))
            LogFile::Write("Done!\n");
        else
            LogFile::Write("FAIL!\n");
    }

    isHookSetup = true;
}

void ResetHook(bool restarting) {
    LogFile::Write("Hook reset request received: ");
    LogFile::WriteLine((restarting) ? "leaving GameLoop" : "entering GameLoop");
    // TODO: reset stuff here?

    if (restarting && isMainLuaLoaded)
    {
        L.getGlobal("restart");
        if (L.pcall(0, 0, 0) != LUA_OK)
            MM2::Abortf("[Lua] Error -- %s", L.toString(-1));
    }
}

PtrHook<BOOL> gameClosing;

// replaces VtResumeSampling
void HookStartup() {
    if (!gameClosing) {
        if (!isHookSetup)
        {
            LogFile::WriteLine("Hook startup request received.");
            SetupHook();
        }
        else
        {
            // GameLoop was restarted
            ResetHook(false);
            luaSetGlobals();
        }
    } else {
        LogFile::WriteLine("WTF: Hook startup request received, but the game is closing!");
    }
};

// replaces VtPauseSampling
void HookShutdown() {
    if (gameClosing) {
        LogFile::WriteLine("Hook shutdown request received.");

        if (isMainLuaLoaded)
        {
            L.getGlobal("shutdown");
            if (L.pcall(0, 0, 0) != LUA_OK)
                MM2::Abortf("[Lua] Error -- %s", L.toString(-1));
        }

        LogFile::Close();
        L.close(); // release Lua
    } else {
        // GameLoop is restarting
        ResetHook(true);
    }
};

bool HookFramework(MM2Version gameVersion) {
    switch (gameVersion)
    {
        case MM2_RETAIL:
        {
            *(DWORD*)0x5E0CC4 = (DWORD)&HookStartup;
            *(DWORD*)0x5E0CD8 = (DWORD)&HookShutdown;

            gameClosing = 0x6B1708;
        } return true;
    }
    return false;
};

void InstallTickHandler(MM2Version gameVersion) {
    auto tickHandler = &TickHandler::Update;

    switch (gameVersion)
    {
        case MM2_RETAIL:
        {
            LogFile::Write("Installing tick handler...");
            LogFile::WriteLine((InstallJmpHook(0x401A2F, *(DWORD*)&tickHandler, true)) ? "Done!" : "FAIL!");
        } break;
    }
};

void InstallChatHandler(MM2Version gameVersion) {
    auto chatHandler = &ChatHandler::Process;

    switch (gameVersion)
    {
        case MM2_RETAIL:
        {
            // send mmGame::SendChatMessage to our handler instead of returning
            LogFile::Write("Installing chat handler...");

            BYTE patch[] = { 80 };

            // try increasing the label buffer sizes
            InstallPatch(0x50BBCF, patch, 1);
            InstallPatch(0x4E68B5, patch, 1);
            InstallPatch(0x4E68B9, patch, 1);

            LogFile::WriteLine((InstallJmpHook(0x414EB6, *(DWORD*)&chatHandler)) ? "Done!" : "FAIL!");
        } break;
    }
};

void InstallPatches(MM2Version gameVersion) {
    switch (gameVersion)
    {
        case MM2_RETAIL:
        {
            InstallTickHandler(gameVersion);
            InstallChatHandler(gameVersion);
        } break;
    }
};

//
// Initialize all the important stuff prior to MM2 starting up
//
void Initialize(MM2Version gameVersion) {
    // first hook into the framework
    LogFile::Write("Hooking into the framework...");

    if (HookFramework(gameVersion)) {
        LogFile::Write("Done!\n");
    } else {
        LogFile::Write("FAIL!\n");
    }

    InstallPatches(gameVersion);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
        case DLL_PROCESS_ATTACH:
        {
            // Initialize the log file
            debug("Initializing MM2Hook...");
            LogFile::Initialize("mm2hook.log", "--<< MM2Hook log file >>--\n");
            LogFile::WriteLine("MM2Hook initialized.");

            HMODULE hDIModule = NULL;
            AGEGameInfo gameInfo;

            LogFile::WriteLine("Checking for known MM2 versions...");

            if (CMidtownMadness2::GetGameInfo(gameInfo))
            {
                LogFile::Format(" - Detected game version %d\n", gameInfo.age_version);

                if (gameInfo.isSupported)
                {
                    if (LoadSystemLibrary("dinput.dll", &hDIModule) &&
                        GetHookProcAddress(hDIModule, "DirectInputCreateA", (FARPROC*)&lpDICreate))
                    {
                        // initialize game manager
                        pMM2 = new CMidtownMadness2(&gameInfo);

                        LogFile::WriteLine("Successfully injected into MM2!");
                        Initialize(pMM2->GetGameVersion());
                    }
                    else
                    {
                        LogFile::WriteLine("Failed to inject into MM2!");
                    }
                }
                else
                {
                    LogFile::WriteLine("Unsupported game version! Terminating...");
                    MessageBox(NULL, "Sorry, this version of MM2 is unsupported. Please remove MM2Hook to launch the game.", "MM2Hook", MB_OK | MB_ICONERROR);

                    ExitProcess(EXIT_FAILURE);
                }
            }
            else
            {
                LogFile::WriteLine("Unknown module detected! Terminating...");
                MessageBox(NULL, "Unknown module! MM2Hook will now terminate the process.", "MM2Hook", MB_OK | MB_ICONERROR);

                ExitProcess(EXIT_FAILURE);
            }
        } break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
        break;
	}
	return TRUE;
}

