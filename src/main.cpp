#include "main.h"

using namespace MM2;

LPFNDIRECTINPUTCREATE lpDICreate;

// Export as 'DirectInputCreateA/W' so we can hook into MM2
#pragma comment(linker, "/EXPORT:DirectInputCreateA=_DirectInputCreateA_Impl")
HRESULT NAKED DirectInputCreateA_Impl(HINSTANCE hinst, DWORD dwVersion, LPVOID *ppDI, LPUNKNOWN punkOuter)
{
    _asm jmp DWORD PTR ds:lpDICreate
}

CMidtownMadness2 *pMM2;

MM2Version gameVersion;

// ==========================
// Game-related properties
// ==========================

/* AGE Debugging */
FILE* ageLogFile = NULL;

/* PSDL shading fix */


/* Dashboard experiment */

static Matrix34 sm_DashOffset;

// ==========================
// Function hooks
// ==========================

MM2FnHook<void> $CreateGameMutex                ( NULL, NULL, 0x402180 );
MM2FnHook<void> $dgBangerInstance_Draw          ( NULL, NULL, 0x4415E0 );
MM2FnHook<void> $asLinearCS_Update              ( NULL, NULL, 0x4A3370 );
MM2FnHook<void> $memSafeHeap_Init               ( NULL, NULL, 0x577210 );
MM2FnHook<void> $DefaultPrintString             ( NULL, NULL, 0x4C9510 );
MM2FnHook<void> $DefaultPrinter                 ( NULL, NULL, 0x4C95F0 );
MM2FnHook<void> $asCullManagerInit              ( NULL, NULL, 0x4A1290 );

// ==========================
// Pointer hooks
// ==========================

MM2PtrHook<asNode> ROOT                         ( NULL, NULL, 0x661738 );

MM2PtrHook<void (*)(LPCSTR)>
                $PrintString                    ( NULL, NULL, 0x5CECF0 );
MM2PtrHook<void (*)(int, LPCSTR, va_list)>
                $Printer                        ( NULL, NULL, 0x5CED24 );
MM2PtrHook<void(*)(void)>
                $FatalErrorHandler              ( NULL, NULL, 0x6A3D38 );


/*
    !! THESE ARE ABSOLUTELY CRITICAL TO THE HOOK WORKING PROPERLY !!
*/

MM2PtrHook<void (*)(void)> $__VtResumeSampling  ( 0x5C86B8, 0x5DF710, 0x5E0CC4 );
MM2PtrHook<void (*)(void)> $__VtPauseSampling   ( 0x5C86C8, 0x5DF724, 0x5E0CD8 );
MM2PtrHook<BOOL> $gameClosing                   ( 0x667DEC, 0x6B0150, 0x6B1708 );

/*
    ===========================================================================
*/

// ==========================
// Callback handlers
// ==========================

#define FOREGROUND_PINK (FOREGROUND_BLUE | FOREGROUND_RED)
#define FOREGROUND_TEAL (FOREGROUND_BLUE | FOREGROUND_GREEN)
#define FOREGROUND_YELLOW (FOREGROUND_RED | FOREGROUND_GREEN)
#define FOREGROUND_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

#define BACKGROUND_PINK (BACKGROUND_BLUE | BACKGROUND_RED)
#define BACKGROUND_TEAL (BACKGROUND_BLUE | BACKGROUND_GREEN)
#define BACKGROUND_YELLOW (BACKGROUND_RED | BACKGROUND_GREEN)
#define BACKGROUND_WHITE (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)

short printer_types[] = {
    FOREGROUND_INTENSITY, // print
    FOREGROUND_INTENSITY, // message
    FOREGROUND_INTENSITY, // display
    FOREGROUND_YELLOW | FOREGROUND_INTENSITY, // warning
    FOREGROUND_RED | FOREGROUND_INTENSITY, // error
    FOREGROUND_RED | FOREGROUND_INTENSITY, // quit/abort
};

class PrinterHandler {
public:
    static void PrintString(LPCSTR message) {
        // TODO: redirect to a log file?
        $DefaultPrintString(message);
    };

    static void Print(int level, LPCSTR message, va_list va_args) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        SetConsoleTextAttribute(hConsole, printer_types[level]);
        $DefaultPrinter(level, message, va_args);
        SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
    };

    static void FatalErrorHandler()
    {
        system("PAUSE");
    }
};

class TickHandler {
public:
    static void Reset(void) {
        // TODO: reset tick stuff
    }

    static void Update(void) {
        MM2Lua::OnTick();

        // pass control back to MM2
        datTimeManager::Update();
    }
};

class CallbackHandler {
public:


    static void CreateGameMutex(LPCSTR lpName) {
        if (datArgParser::Get("nomutex")) {
            LogFile::WriteLine("Game mutex disabled.");
            return;
        }

        $CreateGameMutex(lpName);
    };

    bool LoadAmbientSFX(LPCSTR name) {
        LPCSTR szAmbientSFX = NULL;

        auto city = (LPCSTR)szCityName;

        if ((_strcmpi(city, "sf") == 0) && (_strcmpi(city, "london") == 0))
        {
            char ambientSFX[80] = { NULL };

            sprintf(ambientSFX, "%sambience", city);

            bool exists = !(datAssetManager::Exists("aud\\dmusic\\csv_files", ambientSFX, "csv"));

            // default to 'sfambience' instead of 'londonambience'
            szAmbientSFX = (exists) ? ambientSFX : "sfambience";
        }
        else
        {
            szAmbientSFX = name;
        }

        LogFile::Format("AmbientSFX: %s\n", szAmbientSFX);

        // pass to MM2
        return ((mmGameMusicData*)this)->LoadAmbientSFX(szAmbientSFX);
    };

    static void SetSirenCSVName(LPCSTR name) {
        char siren_name[80] = { NULL };

        sprintf(siren_name, "%spolicesiren", (LPCSTR)szCityName);

        bool useDefault = !(datAssetManager::Exists("aud\\cardata\\player", siren_name, "csv"));

        LPCSTR szSirenName = (useDefault) ? name : siren_name;

        LogFile::Format("SirenCSVName: %s\n", szSirenName);

        // pass to MM2
        vehCarAudioContainer::SetSirenCSVName(szSirenName);
    };

    static void ageDebug(bool enabled, LPCSTR format, ...)
    {
        if (ageLogFile)
        {
            fputs(FormatTime("%T: "), ageLogFile);

            char buffer[1024];

            va_list va;
            va_start(va, format);
            vsprintf(buffer, format, va);
            va_end(va);

            fputs(buffer, ageLogFile);
            fputc('\n', ageLogFile);

            fflush(ageLogFile);
        }
    };

    static LPCSTR AngelReadString(UINT stringId) {
        // currently this overrides the old AngelReadString function
        // and provides no backwards compat. for MMLANG.DLL
        auto str = "";
        L.getGlobal("GetLocaleString");
        L.push(stringId);
        if (L.pcall(1, 1, 0) == LUA_OK)
        {
            str = L.toString(-1);
        }
        else
        {
            LogFile::Format("[LUA] ERROR: %s\n", stringId, L.toString(-1));
            str = "???";
        }   
        L.pop(1);
        return str;
    }

    static mmDirSnd* mmDirSndInit(int sampleRate, bool enableStero, int a4, float volume, LPCSTR deviceName, bool enable3D)
    {
        // TODO: Load the device name from player config?
        if (*deviceName == '\0')
        {
            datArgParser::Get("defaultsounddev", 0, &deviceName);
        }

        LogFile::Format("mmDirSnd::Init - Using Device: %s\n", deviceName);

        // TODO: Set sampling rate (see 0x519640 - int __thiscall AudManager::SetBitDepthAndSampleRate(int this, int bitDepth, int samplingRate))
        // TODO: Redo SetPrimaryBufferFormat to set sampleSize? (see 0x5A5860 -void __thiscall DirSnd::SetPrimaryBufferFormat(mmDirSnd *this, int sampleRate, bool allowStero))
        return $mmDirSndInit(48000, enableStero, a4, volume, deviceName, enable3D);
    }
};

class BridgeFerryCallbackHandler
{
public:
    void Cull(int lod) {
        // wtf
        //setPtr(this, 0x1B, (char)++lod);
    }

    void Draw(int lod) {
        $dgBangerInstance_Draw(this, lod);
    }
};

class mmDashViewCallbackHandler
{
public:
    void UpdateCS() {
        auto dashCam = getPtr<Matrix34>(this, 0x18);

        // apply an offset (mainly doing this from CheatEngine, etc.)
        dashCam->m11 += sm_DashOffset.m11;
        dashCam->m12 += sm_DashOffset.m12;
        dashCam->m13 += sm_DashOffset.m13;
        dashCam->m14 += sm_DashOffset.m14;

        dashCam->m21 += sm_DashOffset.m21;
        dashCam->m22 += sm_DashOffset.m22;
        dashCam->m23 += sm_DashOffset.m23;
        dashCam->m24 += sm_DashOffset.m24;

        dashCam->m31 += sm_DashOffset.m31;
        dashCam->m32 += sm_DashOffset.m32;
        dashCam->m33 += sm_DashOffset.m33;
        dashCam->m34 += sm_DashOffset.m34;

        $asLinearCS_Update(this);
    };
};

class memSafeHeapCallbackHandler
{
public:
    void Init(void *memAllocator, unsigned int heapSize, bool p3, bool p4, bool checkAlloc)
    {        
        int newHeapSize = 128;

        datArgParser::Get("heapsize", 0, &newHeapSize);

        LogFile::Format("memSafeHeap::Init -- Allocating %dMB heap\n", newHeapSize);

        newHeapSize <<= 20; // Same as *= (1024 * 1024)

        return $memSafeHeap_Init(this, memAllocator, newHeapSize, p3, p4, checkAlloc);
    };
};

class asCullManager
{
public:
    void Init(int maxCullables, int maxCullables2D)
    {
        maxCullables = 1024;
        maxCullables2D = 256;

        LogFile::Format("asCullManager::Init - Increased Cullables to %i, %i\n", maxCullables, maxCullables2D);

        $asCullManagerInit(this, maxCullables, maxCullables2D);
    }
};


class HookSystemHandler
{
private:
    static void InitializeLua()
    {
        // Guaranteed to be loaded before anything vital is called (e.g. AngelReadString)
        if (gameVersion == MM2_RETAIL)
        {
            MM2Lua::Initialize();
        }
        else
        {
            // no lua support for betas yet
            MessageBox(NULL, "NOTE: This game version does not currently support Lua scripting.", "MM2Hook", MB_OK | MB_ICONINFORMATION);
            return;
        }
    }
public:
    static void Initialize(int argv, char **argc)
    {
        // initialize the Lua engine
        InitializeLua();

        if (gameVersion == MM2_RETAIL)
        {
            // hook into the printer
            *$Printer           = &PrinterHandler::Print;
            *$PrintString       = &PrinterHandler::PrintString;
            *$FatalErrorHandler = &PrinterHandler::FatalErrorHandler;

            /* Won't write to the log file for some reason :(
            LogFile::Write("Redirecting MM2 output...");

            if (datOutput::OpenLog("mm2.log"))
            LogFile::Write("Done!\n");
            else
            LogFile::Write("FAIL!\n");
            */

            if (ageLogFile == NULL && datArgParser::Get("ageDebug"))
            {
                ageLogFile = fopen("AGE.log", "w+");
            }
        }
    }

    static void Reset(bool restarting)
    {
        LogFile::Write("Hook reset request received: ");
        LogFile::WriteLine(restarting ? "Leaving GameLoop" : "Entering GameLoop");

        if (restarting)
            MM2Lua::OnRestart();

        MM2Lua::Reset();
    }

    static void Start()
    {
        if (!$gameClosing)
        {
            // GameLoop was restarted
            Reset(false);
        }
        else
        {
            LogFile::WriteLine("WTF: Hook startup request received, but the game is closing!");
        }
    }

    static void Stop()
    {
        if ($gameClosing)
        {
            LogFile::WriteLine("Hook shutdown request received.");

            LogFile::Close();
            L.close(); // release Lua

            fclose(ageLogFile);
        }
        else
        {
            // GameLoop is restarting
            Reset(true);
        }
    }
};

/*
    ===========================================================================
*/

bool InitializeFramework(MM2Version gameVersion) {
    LogFile::Write("Hooking into the framework...");

    if (gameVersion == MM2_INVALID || gameVersion > MM2_NUM_VERSIONS)
    {
        LogFile::WriteLine("FAIL!");
        return false;
    }

    *$__VtResumeSampling = &HookSystemHandler::Start;
    *$__VtPauseSampling = &HookSystemHandler::Stop;

    LogFile::WriteLine("Done!");
    return true;
};

void InstallPatches(MM2Version gameVersion) {

    LogFile::WriteLine("Installing patches...");

    InstallGamePatch("Increase chat buffer size", gameVersion, { 60 },
    {
        { NULL, NULL, 0x4E68B5 },
        { NULL, NULL, 0x4E68B9 },
        { NULL, NULL, 0x50BBCF },
    });

    InstallGamePatch ("Increase cop limit", gameVersion, { 0x40 },
    {
        { NULL, NULL, 0x55100B },
    });

    InstallGamePatch("Enables pointer in windowed mode", gameVersion, { 0x90, 0x90 },
    {
        { NULL, NULL, 0x4F136E },
    });

    //InstallGamePatch("Set Sample Rate", gameVersion, { 32 },
    //{
    //    { NULL, NULL, 0x5A58A0 }
    //});
}

void InstallCallbacks(MM2Version gameVersion) {
    LogFile::WriteLine("Installing callbacks / virtual tables...");

    switch (gameVersion)
    {
        case MM2_BETA_1:
        case MM2_BETA_2:
        {
            // Disables time check on betas
            InstallGameCallback("TrialTimeExpired", gameVersion, &ReturnNullOrZero, HOOK_CALL,
            {
                { 0x4011B0, 0x4012AC, NULL },
            });
        } break;
        case MM2_RETAIL:
        {
            // mutex was introduced in retail
            InstallGameCallback("CreateGameMutex", gameVersion, &CallbackHandler::CreateGameMutex, HOOK_CALL,
            {
                { NULL, NULL, 0x40128D },
            });
            
            // revert bridges/ferries to how they were in the betas
            InstallGameCallback("Bridge/Ferry: Cull", gameVersion, &BridgeFerryCallbackHandler::Cull, HOOK_CALL,
            {
                { NULL, NULL, 0x5780BC }, // gizBridgeMgr::Cull
                { NULL, NULL, 0x5798F0 }, // gizFerryMgr::Cull
            });

            InstallVTableHook("Bridge/Ferry: Draw", gameVersion, &BridgeFerryCallbackHandler::Draw,
            {
                { NULL, NULL, 0x5B5FB8 }, // gizBridge::Draw
                { NULL, NULL, 0x5B61AC }, // gizFerry::Draw
            });
        } break;
    }

    // Replaces a call to ArchInit (a null function) just before ExceptMain
    // This is PERFECT for initializing everything before the game even starts!
    //
    // NOTE: Arguments passed to the game can be retrieved using datArgParser,
    // since it gets initialized just before ArchInit.
    InstallGameCallback("ArchInit", gameVersion, &HookSystemHandler::Initialize, HOOK_CALL,
    {         
        { NULL, NULL, 0x4023DB },
    });

    InstallGameCallback("ageDebug", gameVersion, &CallbackHandler::ageDebug, HOOK_JMP,
    {
        { NULL, NULL, 0x402630 },
    });

    InstallGameCallback("ProgressRect [white loading bar fix]", gameVersion, &gfxCallback::ProgressRect, HOOK_CALL,
    {
        { NULL, NULL, 0x401163 },
        { NULL, NULL, 0x4011CC },
    });

    InstallGameCallback("gfxPipeline::SetRes", gameVersion, &gfxCallback::setRes, HOOK_CALL,
    {
        { NULL, NULL, 0x401482 },
    });

    InstallGameCallback("AutoDetectCallback", gameVersion, &gfxCallback::AutoDetectCallback, HOOK_JMP,
    {
        { NULL, NULL, 0x4AC030 },
    });

    InstallGameCallback("gfxWindowCreate", gameVersion, &gfxCallback::gfxWindowCreate, HOOK_CALL,
    {
        { NULL, NULL, 0x4A94AA },
    });    

    InstallGameCallback("mmDirSnd::Init", gameVersion, &CallbackHandler::mmDirSndInit, HOOK_CALL,
    {
        { NULL, NULL, 0x51941D },
    });

    InstallGameCallback("gfxLoadVideoDatabase [disable 'badvideo.txt']", gameVersion, &ReturnFalse, HOOK_CALL,
    {
        { NULL, NULL, 0x4AC4F9 },
    });
    
    InstallGameCallback("memSafeHeap::Init [Heap fix]", gameVersion, &memSafeHeapCallbackHandler::Init, HOOK_CALL,
    {
        { NULL, NULL, 0x4015DD },
    });

    InstallGameCallback("asCullManager::Init [Increase Max Cullables]", gameVersion, &asCullManager::Init, HOOK_CALL,
    {
        { NULL, NULL, 0x401D5C },
    });

    //// not supported for betas yet
    //if (gameVersion == MM2_RETAIL)
    //{
    //    // NOTE: Completely overrides AngelReadString!
    //    InstallGameCallback("AngelReadString", gameVersion, &CallbackHandler::AngelReadString, HOOK_JMP,
    //    {
    //        { NULL, NULL, 0x534790 },
    //    });
    //}

    InstallGameCallback("vglBegin", gameVersion, &gfxCallback::vglBegin, HOOK_CALL,
    {
        { NULL, NULL, 0x448424 }, { NULL, NULL, 0x448697 }, { NULL, NULL, 0x448903 }, { NULL, NULL, 0x448BFD },
        { NULL, NULL, 0x448DE4 }, { NULL, NULL, 0x44902A }, { NULL, NULL, 0x4492A4 }, { NULL, NULL, 0x4494C3 },
        { NULL, NULL, 0x4496A5 }, { NULL, NULL, 0x44986B }, { NULL, NULL, 0x449A13 }, { NULL, NULL, 0x449BD9 },
        { NULL, NULL, 0x449D82 }, { NULL, NULL, 0x449F67 }, { NULL, NULL, 0x44A21C }, { NULL, NULL, 0x44A444 },
        { NULL, NULL, 0x44A629 }, { NULL, NULL, 0x44A7EF }, { NULL, NULL, 0x44A997 }, { NULL, NULL, 0x44AB5D },
        { NULL, NULL, 0x44AD06 }, { NULL, NULL, 0x44AECA }, { NULL, NULL, 0x44B0EC }, { NULL, NULL, 0x44B24B },
        { NULL, NULL, 0x44B3B6 }, { NULL, NULL, 0x44B557 }, { NULL, NULL, 0x44B6F3 }, { NULL, NULL, 0x44B8F1 },
        { NULL, NULL, 0x44BA8A }, { NULL, NULL, 0x44BC29 }, { NULL, NULL, 0x44BE9C }, { NULL, NULL, 0x44C136 },
        { NULL, NULL, 0x44C40C }, { NULL, NULL, 0x44C64A }, { NULL, NULL, 0x44C7C0 }, { NULL, NULL, 0x44CAD6 },
        { NULL, NULL, 0x44CCF5 }, { NULL, NULL, 0x44CF6D }, { NULL, NULL, 0x44D0D4 }, { NULL, NULL, 0x44D5F7 },
        { NULL, NULL, 0x44D789 }, { NULL, NULL, 0x44DC55 }, { NULL, NULL, 0x44E050 }, { NULL, NULL, 0x44E14B },
        { NULL, NULL, 0x44E2A3 }, { NULL, NULL, 0x44E69D }, { NULL, NULL, 0x44E79E }, { NULL, NULL, 0x44EAA0 },
        { NULL, NULL, 0x44EBA5 }, { NULL, NULL, 0x44EFD0 }, { NULL, NULL, 0x44F0DC }, { NULL, NULL, 0x44F588 },
        { NULL, NULL, 0x44F7E2 }, { NULL, NULL, 0x44FC1E }, { NULL, NULL, 0x44FDD4 }, { NULL, NULL, 0x44FF10 },
        { NULL, NULL, 0x450085 }, { NULL, NULL, 0x450269 },
        // ------------------------------------
        { NULL, NULL, 0x443B9D }, // dgRoadDecalInstance
        { NULL, NULL, 0x57AC4A }, // ped LODs
    });

    InstallGameCallback("vglEnd", gameVersion, &gfxCallback::vglEnd, HOOK_CALL,
    {
        { NULL, NULL, 0x4485D3 }, { NULL, NULL, 0x448B82 }, { NULL, NULL, 0x448D8C }, { NULL, NULL, 0x448FB7 },
        { NULL, NULL, 0x449219 }, { NULL, NULL, 0x449480 }, { NULL, NULL, 0x44963E }, { NULL, NULL, 0x44983C },
        { NULL, NULL, 0x4499D4 }, { NULL, NULL, 0x449BAA }, { NULL, NULL, 0x449D42 }, { NULL, NULL, 0x449F5A },
        { NULL, NULL, 0x44A146 }, { NULL, NULL, 0x44A3F8 }, { NULL, NULL, 0x44A5BF }, { NULL, NULL, 0x44A7C0 },
        { NULL, NULL, 0x44A958 }, { NULL, NULL, 0x44AB2E }, { NULL, NULL, 0x44ACC6 }, { NULL, NULL, 0x44AEBC },
        { NULL, NULL, 0x44B083 }, { NULL, NULL, 0x44B23D }, { NULL, NULL, 0x44B394 }, { NULL, NULL, 0x44B531 },
        { NULL, NULL, 0x44B6E1 }, { NULL, NULL, 0x44B895 }, { NULL, NULL, 0x44BA7C }, { NULL, NULL, 0x44BC03 },
        { NULL, NULL, 0x44BE8E }, { NULL, NULL, 0x44C118 }, { NULL, NULL, 0x44C3EA }, { NULL, NULL, 0x44C638 },
        { NULL, NULL, 0x44C77A }, { NULL, NULL, 0x44C989 }, { NULL, NULL, 0x44CC44 }, { NULL, NULL, 0x44CE63 },
        { NULL, NULL, 0x44D04E }, { NULL, NULL, 0x44D403 }, { NULL, NULL, 0x44D780 }, { NULL, NULL, 0x44D8E9 },
        { NULL, NULL, 0x44E014 }, { NULL, NULL, 0x44E131 }, { NULL, NULL, 0x44E22C }, { NULL, NULL, 0x44E661 },
        { NULL, NULL, 0x44E785 }, { NULL, NULL, 0x44E886 }, { NULL, NULL, 0x44EB82 }, { NULL, NULL, 0x44EDC3 },
        { NULL, NULL, 0x44F0B9 }, { NULL, NULL, 0x44F316 }, { NULL, NULL, 0x44F64C }, { NULL, NULL, 0x44FB9D },
        { NULL, NULL, 0x44FD30 }, { NULL, NULL, 0x44FE4E }, { NULL, NULL, 0x44FFB3 }, { NULL, NULL, 0x450162 },
        { NULL, NULL, 0x450390 }, { NULL, NULL, 0x45078C },
        // ------------------------------------
        { NULL, NULL, 0x443DCC }, // road decals
        { NULL, NULL, 0x57AD41 }, // ped LODs
    });

    InstallGameCallback("datTimeManager::Update", gameVersion, &TickHandler::Update, HOOK_CALL,
    {
        { NULL, NULL, 0x401A2F },
    });

    InstallGameCallback("mmGame::SendChatMessage", gameVersion, &inputCallback::ChatHandler::Process, HOOK_JMP,
    {
        { NULL, NULL, 0x414EB6 },
    });

    InstallGameCallback("mmGameMusicData::LoadAmbientSFX", gameVersion, &CallbackHandler::LoadAmbientSFX, HOOK_CALL,
    {
        { NULL, NULL, 0x433F93 },
    });

    InstallGameCallback("vehCarAudioContainer::SetSirenCSVName", gameVersion, &CallbackHandler::SetSirenCSVName, HOOK_CALL,
    {
        { NULL, NULL, 0x412783 },
        { NULL, NULL, 0x412772 },
    });

    // dashboard testing
    InstallGameCallback("mmDashView::Update [EXPERIMENTAL]", gameVersion, &mmDashViewCallbackHandler::UpdateCS, HOOK_CALL,
    {
        { NULL, NULL, 0x430F87 }, // replaces call to asLinearCS::Update
    });

    InstallGameCallback("zipFile::Init ['extraLen' spam fix]", gameVersion, &NullSub, HOOK_CALL,
    {
        { NULL, NULL, 0x5738EA }, // 'extraLen=%d'
    });
};

//
// Initialize all the important stuff prior to MM2 starting up
//
void Initialize(ageInfoLookup &gameInfo) {
    // initialize game manager
    pMM2 = new CMidtownMadness2(gameInfo.info);
    pMM2->Initialize();

    // TODO: Remove dependency on 'gameVersion'?
    gameVersion = pMM2->GetVersion();

    if (InitializeFramework(gameVersion)) {
        InstallCallbacks(gameVersion);
        InstallPatches(gameVersion);
    } else {
        MessageBox(NULL, "MM2Hook was unable to initialize properly. The game will proceed normally.", "MM2Hook", MB_OK);
    }
}

bool IsGameSupported(ageInfoLookup &gameInfo) {
    LogFile::WriteLine("Checking for known MM2 versions...");

    if (CMidtownMadness2::GetGameInfo(gameInfo))
    {
        LogFile::Format(" - Detected game version %d\n", gameInfo.info.engineVersion);
        return gameInfo.isSupported;
    }
    else
    {
        LogFile::WriteLine("Unknown module detected! Terminating...");
        MessageBox(NULL, "Unknown module! MM2Hook will now terminate the process.", "MM2Hook", MB_OK | MB_ICONERROR);

        ExitProcess(EXIT_FAILURE);
    }

    return false;
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
            LogFile::WriteLine("Initializing...");

            HMODULE hDIModule = NULL;
            ageInfoLookup gameInfo;

            if (IsGameSupported(gameInfo))
            {
                if (LoadSystemLibrary("dinput.dll", &hDIModule) &&
                    GetHookProcAddress(hDIModule, "DirectInputCreateA", (FARPROC*)&lpDICreate))
                {
                    LogFile::WriteLine("Injected into the game process successfully.");

                    // initialize the hook
                    Initialize(gameInfo);
                }
                else
                {
                    LogFile::WriteLine("Failed to inject into the game process.");
                    MessageBox(NULL, "Could not inject into the game process. Unknown errors may occur.", "MM2Hook", MB_OK | MB_ICONWARNING);
                }
            }
            else
            {
                LogFile::WriteLine("Unsupported game version -- terminating...");

                if (gameInfo.info.gameVersion == MM2_BETA_2_PETITE)
                {
                    MessageBox(NULL, "Sorry, this version of Beta 2 was compressed with PeTite -- you'll need an unpacked version.\n\nOtherwise, please remove MM2Hook to launch the game.", "MM2Hook", MB_OK | MB_ICONERROR);
                } else
                {
                    MessageBox(NULL, "Sorry, this version of MM2 is unsupported. Please remove MM2Hook to launch the game.", "MM2Hook", MB_OK | MB_ICONERROR);
                }

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

