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

/*
    Game patching functions
*/

enum CB_HOOK_TYPE {
    HOOK_JMP,
    HOOK_CALL
};

struct CB_HANDLER {
    const LPVOID lpHandler;

    template<typename T>
    constexpr CB_HANDLER(T *test) : lpHandler((LPVOID)(*(DWORD*)&test)) {};

    template<class TT, typename T>
    constexpr CB_HANDLER(T(TT::*test)) : lpHandler((LPVOID)(*(DWORD*)&test)) {};

    constexpr operator LPVOID() const {
        return lpHandler;
    };

    constexpr operator DWORD() const {
        return reinterpret_cast<DWORD>(lpHandler);
    };
};

template<int size = 1>
struct CB_INSTALL_INFO {
    static const int length = size;

    CB_HANDLER cb_proc;
    struct {
        MM2AddressData addr_data;
        CB_HOOK_TYPE type;
    } cb_data[size];
};

template<int size = 1, int count = 1>
struct PATCH_INSTALL_INFO {
    static const int length = count;
    static const int buffer_size = size;

    const char buffer[size];
    MM2AddressData addrData[count];
};

bool InstallGamePatch(LPCSTR name, MM2Version gameVersion, LPVOID lpData) {
    LogFile::Format(" - Installing patch: '%s'...\n", name);

    auto *info = (PATCH_INSTALL_INFO<>*)lpData;

    if (info->length == 0 || info->buffer_size == 0) {
        LogFile::WriteLine(" - ERROR! Data is empty!");
        return false;
    }

    int count = info->length;
    int progress = 0;

    for (int i = 0; i < count; i++) {
        auto addr = info->addrData[i].addresses[gameVersion];

        LogFile::Format("  - [%d] %08X => %08X : ", (i + 1), addr, (BYTE*)info->buffer);

        if (addr != NULL)
        {
            InstallPatch(addr, (BYTE*)info->buffer, info->length);
            LogFile::WriteLine("OK");

            progress++;
        } else LogFile::WriteLine("Not supported");
    }

    bool success = (progress > 0);
    return success;
}

bool InstallGameCallback(LPCSTR cb_name, MM2Version gameVersion, LPVOID lpCallback) {
    LogFile::Format(" - Installing callback: '%s'...\n", cb_name);

    auto *cb_info = (CB_INSTALL_INFO<>*)lpCallback;

    if (cb_info->length == 0) {
        LogFile::WriteLine(" - ERROR! Address data is empty!");
        return false;
    }

    const DWORD cb = cb_info->cb_proc;
    int count = cb_info->length;
    int progress = 0;

    for (int i = 0; i < count; i++) {
        auto data = cb_info->cb_data[i];
        auto addr = data.addr_data.addresses[gameVersion];

        LogFile::Format("  - [%d]: %08X => %08X : ", (i + 1), addr, cb);

        if (addr != NULL && InstallCallbackHook(addr, cb, (data.type == HOOK_CALL)))
        {
            LogFile::WriteLine("OK");
            progress++;
        } else LogFile::WriteLine("Not supported");
    }

    bool success = (progress > 0);
    return success;
}

/*
    ===========================================================================
*/

bool isHookSetup = false;
bool isConsoleOpen = false;

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
    // Inform Lua of any changes beforehand
    MM2Lua::OnKeyPress(vKey);

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
        MM2Lua::OnTick();

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
            MM2Lua::SendCommand(message);

            // return normal chatbox behavior
            isConsoleOpen = false;
        } else {
            LogFile::Format("Got chat message: %s\n", message);
        }
    }
};

FILE *ageLog;

LPCSTR ageLogFile = "AGE.log";
bool ageDebugEnabled = false;
char ageDebug_buffer[1024] = { NULL };

class CallbackHandler {
public:
    bool LoadAmbientSFX(LPCSTR name) {
        LPCSTR szAmbientSFX = NULL;

        auto city = (LPCSTR)MM2::szCityName;

        if ((_strcmpi(city, "sf") == 0) && (_strcmpi(city, "london") == 0))
        {
            char ambientSFX[80] = { NULL };

            sprintf(ambientSFX, "%sambience", city);

            bool exists = !(MM2::datAssetManager::Exists("aud\\dmusic\\csv_files", ambientSFX, "csv"));

            // default to 'sfambience' instead of 'londonambience'
            szAmbientSFX = (exists) ? ambientSFX : "sfambience";
        }
        else
        {
            szAmbientSFX = name;
        }

        LogFile::Format("AmbientSFX: %s\n", szAmbientSFX);

        // pass to MM2
        return ((MM2::mmGameMusicData*)this)->LoadAmbientSFX(szAmbientSFX);
    };

    static void SetSirenCSVName(LPCSTR name) {
        char siren_name[80] = { NULL };

        sprintf(siren_name, "%spolicesiren", (LPCSTR)MM2::szCityName);

        bool useDefault = !(MM2::datAssetManager::Exists("aud\\cardata\\player", siren_name, "csv"));

        LPCSTR szSirenName = (useDefault) ? name : siren_name;

        LogFile::Format("SirenCSVName: %s\n", szSirenName);

        // pass to MM2
        MM2::vehCarAudioContainer::SetSirenCSVName(szSirenName);
    };

    static void ageDebug(int enabled, LPCSTR format, ...) {
        // this makes the game load up reeeeeally slow if enabled!
        if (ageDebugEnabled || (ageDebugEnabled = MM2::datArgParser::Get("age_debug")))
        {
            va_list va;
            va_start(va, format);
            vsprintf(ageDebug_buffer, format, va);
            va_end(va);

            // 'LogFile::WriteLine' seems to be causing crashes upon exiting?
            if (ageLog == NULL)
            {
                FILE *logFile = fopen(ageLogFile, "w");
                
                fputs("--<< A.G.E. Log file >>--\n\n", logFile);
                fclose(logFile);
            }

            ageLog = fopen(ageLogFile, "a+");

            fputs(ageDebug_buffer, ageLog);
            fputs("\n", ageLog);

            fclose(ageLog);
        }
    };
};
/*
    ===========================================================================
*/

void InitializeLua() {
    if (pMM2->GetGameVersion() == MM2_RETAIL)
    {
        MM2Lua::Initialize();
    } else
    {
        // no lua support for betas yet
        MessageBox(NULL, "NOTE: This game version does not currently support Lua scripting.", "MM2Hook", MB_OK | MB_ICONINFORMATION);
        return;
    }
}

void SetupHook() {
    // initialize the Lua engine
    InitializeLua();
    
    if (pMM2->GetGameVersion() == MM2_RETAIL)
    {
        LogFile::Write("Setting MM2 output log file...");

        if (MM2::datOutput::OpenLog("mm2.log"))
            LogFile::Write("Done!\n");
        else
            LogFile::Write("FAIL!\n");
    }

    LogFile::Write("Subclassing window...");
    LogFile::WriteLine(SubclassGameWindow(pMM2->GetMainWindowHwnd(), WndProcNew, &hProcOld) ? "Done!" : "FAIL!");

    isHookSetup = true;
}

void ResetHook(bool restarting) {
    LogFile::Write("Hook reset request received: ");
    LogFile::WriteLine((restarting) ? "leaving GameLoop" : "entering GameLoop");

    if (restarting)
        MM2Lua::OnRestart();

    MM2Lua::Reset();
}

MM2PtrHook<BOOL> gameClosing INIT_DATA( 0x667DEC, 0x6B0150, 0x6B1708 );

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
        }
    } else {
        LogFile::WriteLine("WTF: Hook startup request received, but the game is closing!");
    }
};

// replaces VtPauseSampling
void HookShutdown() {
    if (gameClosing) {
        LogFile::WriteLine("Hook shutdown request received.");

        LogFile::Close();
        L.close(); // release Lua
    } else {
        // GameLoop is restarting
        ResetHook(true);
    }
};

MM2PtrHook<DWORD> vtResumeSampling    INIT_DATA( 0x5C86B8, 0x5DF710, 0x5E0CC4 );
MM2PtrHook<DWORD> vtPauseSampling     INIT_DATA( 0x5C86C8, 0x5DF724, 0x5E0CD8 );

bool HookFramework(MM2Version gameVersion) {
    if (gameVersion == MM2_INVALID || gameVersion > MM2_NUM_VERSIONS)
        return false;

    //const DWORD vtResumeSampling = MM2AddressData { 0x5C86B8, 0x5DF710, 0x5E0CC4 }.addresses[gameVersion];
    //const DWORD vtPauseSampling = MM2AddressData { 0x5C86C8, 0x5DF724, 0x5E0CD8 }.addresses[gameVersion];
    //
    //*(DWORD*)vtResumeSampling = (DWORD)&HookStartup;
    //*(DWORD*)vtPauseSampling = (DWORD)&HookShutdown;

    vtResumeSampling.set_ptr((DWORD)&HookStartup);
    vtPauseSampling.set_ptr((DWORD)&HookShutdown);

    return true;
};

const PATCH_INSTALL_INFO<1, 3> chatSize_patch{
    { 80 }, {
        { NULL, NULL, 0x4E68B5 },
        { NULL, NULL, 0x4E68B9 },
        { NULL, NULL, 0x50BBCF }
    }
};

void InstallPatches(MM2Version gameVersion) {
    LogFile::WriteLine("Installing patches...");

    InstallGamePatch("Increase chat buffer size", gameVersion, (LPVOID)&chatSize_patch);
};

const CB_INSTALL_INFO<1> tick_CB {
    &TickHandler::Update,
    {
        {{ NULL, NULL, 0x401A2F }, HOOK_CALL }
    }
};

const CB_INSTALL_INFO<1> ageDebug_CB {
    &CallbackHandler::ageDebug,
    {
        {{ NULL, NULL, 0x402630 }, HOOK_JMP }
    }
};

const CB_INSTALL_INFO<1> sendChatMsg_CB {
    &ChatHandler::Process,
    {
        {{ NULL, NULL, 0x414EB6 }, HOOK_JMP }
    }
};

const CB_INSTALL_INFO<1> loadAmbientSFX_CB {
    &CallbackHandler::LoadAmbientSFX,
    {
        {{ NULL, NULL, 0x433F93 }, HOOK_CALL }
    }
};

const CB_INSTALL_INFO<2> sirenCSV_CB {
    &CallbackHandler::SetSirenCSVName,
    {
        {{ NULL, NULL, 0x412783 }, HOOK_CALL },
        {{ NULL, NULL, 0x412772 }, HOOK_CALL },
    }
};

void InstallCallbacks(MM2Version gameVersion) {
    LogFile::WriteLine("Installing callbacks...");

    switch (gameVersion)
    {
        case MM2_BETA_1:
        case MM2_BETA_2:
        {
            // Make sure the betas never expire
            const CB_INSTALL_INFO<1> trialTimeExpired_CB {
                &ReturnNullOrZero,
                {
                    {{ 0x4011B0, 0x4012AC, NULL }, HOOK_CALL }
                }
            };

            InstallGameCallback("TrialTimeExpired", gameVersion, (LPVOID)&trialTimeExpired_CB);
        } break;
    }

    InstallGameCallback("Tick", gameVersion, (LPVOID)&tick_CB);
    InstallGameCallback("ageDebug", gameVersion, (LPVOID)&ageDebug_CB);

    InstallGameCallback("LoadAmbientSFX", gameVersion, (LPVOID)&loadAmbientSFX_CB);
    InstallGameCallback("SendChatMessage", gameVersion, (LPVOID)&sendChatMsg_CB);
    InstallGameCallback("SetSirenCSVName", gameVersion, (LPVOID)&sirenCSV_CB);
};

//
// Initialize all the important stuff prior to MM2 starting up
//
void Initialize(MM2Version gameVersion) {
    // first hook into the framework
    LogFile::Write("Hooking into the framework...");
    LogFile::WriteLine(HookFramework(gameVersion) ? "Done!" : "FAIL!");
    
    InstallCallbacks(gameVersion);
    InstallPatches(gameVersion);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
        case DLL_PROCESS_ATTACH:
        {
            debug("Initializing MM2Hook...");

            // Initialize the log file
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

                    if (gameInfo.version == MM2_BETA_2_PETITE)
                    {
                        MessageBox(NULL, "Sorry, this version of Beta 2 was compressed with PeTite -- you'll need an unpacked version.\n\nOtherwise, please remove MM2Hook to launch the game.", "MM2Hook", MB_OK | MB_ICONERROR);
                    }
                    else
                    {
                        MessageBox(NULL, "Sorry, this version of MM2 is unsupported. Please remove MM2Hook to launch the game.", "MM2Hook", MB_OK | MB_ICONERROR);
                    }

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

