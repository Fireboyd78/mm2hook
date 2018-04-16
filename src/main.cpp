#include "handlers\bugfix_handlers.h"
#include "handlers\feature_handlers.h"
#include "main.h"

#include <discord-presence.h>

using namespace MM2;

// ==========================
// Game-related properties
// ==========================

static int cur_seed = 0;

/* AGE Debugging */
FILE *ageLogFile;

// ==========================
// Pointer hooks
// ==========================
ageHook::Type<asNode> ROOT                              ( 0x661738 );

/*
    ===========================================================================
*/

/*
    Custom coreFileMethods implementation for log files
*/
class coreLogFile {
public:
    static int Open(LPCSTR filename, bool) {
        return (int)LogFileStream::Open(filename);
    }

    static int Create(LPCSTR filename) {
        return (int)LogFileStream::Create(filename);
    }

    static int Read(int, LPVOID, int) {
        return 0;
    };

    static int Write(int handle, const LPVOID buffer, int length) {
        if (length > 0)
        {
            // ensure the buffer is clean
            char buf[4096] = { NULL };
            strncpy(buf, (LPCSTR)buffer, length);

            reinterpret_cast<LogFileStream *>(handle)->Write(buf);
        }
        return length;
    }

    static int Close(int handle) {
        reinterpret_cast<LogFileStream *>(handle)->Close();
        return 0;
    }

    static int Flush(int handle) {
        reinterpret_cast<LogFileStream *>(handle)->Flush(true);
        return 0;
    }
};

const coreFileMethods logFileMethods = {
    &coreLogFile::Open,
    &coreLogFile::Create,
    &coreLogFile::Read,
    &coreLogFile::Write,
    NULL,
    &coreLogFile::Close,
    NULL,
    NULL,
    &coreLogFile::Flush,
};

// ==========================
// Callback handlers
// ==========================

class CallbackHandler {
public:
    static void ProgressRect(int x, int y, int width, int height, UINT color) {

        DDPIXELFORMAT ddPixelFormat = { sizeof(ddPixelFormat) };
        lpdsRend->GetPixelFormat(&ddPixelFormat);

        DDBLTFX ddBltFx = { sizeof(ddBltFx) };
        ddBltFx.dwFillColor = GetPixelFormatColor(&ddPixelFormat, color);

        RECT position = {
            x,
            y,
            x + width,
            y + height,
        };

        lpdsRend->Blt(&position, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddBltFx);
    }

    static void CreateGameMutex(LPCSTR lpName) {
        if (datArgParser::Get("nomutex")) {
            LogFile::WriteLine("Game mutex disabled.");
        } else {
            $CreateGameMutex(lpName);
        }
    }

    static void ageDebug(int debug, const char* format, ...) {
        va_list va;

        if (ageLogFile)
        {
            // print to AGE.log if user specified -ageDebug

            va_start(va, format);
            vfprintf(ageLogFile, format, va);
            va_end(va);

            fputc('\n', ageLogFile);
            // fflush(ageLogFile);
        } else if (debug) {
            va_start(va, format);

            // treat as Messagef
            Printer(1, format, va);

            va_end(va);
        }
    }

    static LocString * AngelReadString(UINT stringId) {
        static const LPCSTR STRING_UNKNOWN = "?? lang:%d ??";

        static HMODULE h_MMLANG = NULL;

        static LocString string_buffer[8];
        static int string_index = 0;

        LPCSTR str = NULL;

        L.getGlobal("GetLocaleString");
        L.push(stringId);

        if ((L.pcall(1, 1, 0) == LUA_OK) && !L.isNil(-1))
            str = L.toString(-1);

        L.pop(1);

        auto locStr = &string_buffer[(string_index++ & 0x7)];

        // not found in Lua, let's look in MMLANG.DLL
        if (str == NULL)
        {
            // revert to MMLANG.DLL
            if (h_MMLANG == NULL)
            {
                if ((h_MMLANG = LoadLibrary("MMLANG.DLL")) == NULL)
                {
                    MessageBox(NULL, "MMLANG.DLL not found.", "Midtown Madness 2", MB_ICONHAND);
                    ExitProcess(0);
                }
            }

            if ($MyLoadStringA(h_MMLANG, stringId, locStr, sizeof(LocString)) == 0)
            {
                // string wasn't in Lua or DLL, return an unknown string
                // e.g. "?? lang:123 ??"
                sprintf((char *)locStr, STRING_UNKNOWN, stringId);
            }
        } else {
            strcpy(locStr->buffer, str);
        }

        return locStr;
    }

    static BOOL __stdcall AutoDetectCallback(GUID *lpGUID,
                                             LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext)
    {
        stopwatch timer;

        Displayf("AutoDetect: GUID=%x, Description=%s, Name=%s", lpGUID, lpDriverDescription, lpDriverName);

        timer.start();

        if (lpDirectDrawCreateEx(lpGUID, (LPVOID*)&lpDD, IID_IDirectDraw7, nullptr) == DD_OK)
        {
            gfxInterface *gfxInterface = &gfxInterfaces[gfxInterfaceCount];

            strcpy (gfxInterface->Name, lpDriverDescription);

            gfxInterface->DeviceCaps = 1;
            gfxInterface->AcceptableDepths = gfxDepthFlags::Depth32;

            DDDEVICEIDENTIFIER2 ddDeviceIdentifier = { NULL };

            if (lpDD->GetDeviceIdentifier(&ddDeviceIdentifier, 0) == DD_OK)
            {
                gfxInterface->VendorID  = ddDeviceIdentifier.dwVendorId;
                gfxInterface->DeviceID  = ddDeviceIdentifier.dwDeviceId;
                gfxInterface->GUID      = ddDeviceIdentifier.guidDeviceIdentifier;
            }

            if (lpDD->QueryInterface(IID_IDirect3D7, (LPVOID*)&lpD3D) == DD_OK)
            {
                lpD3D->EnumDevices($DeviceCallback, gfxInterface);
                lpD3D->Release();

                lpD3D = nullptr;
            }

            gfxInterface->DeviceType        = gfxDeviceType::HardwareWithTnL;

            gfxInterface->ResolutionCount   = 0;
            gfxInterface->ResolutionChoice  = 0;

            DWORD availableMemory = 0x40000000; // 1GB = 1024 * 1024 * 1024

            DDSCAPS2 ddsCaps = { NULL };

            ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;

            if (lpDD->GetAvailableVidMem(&ddsCaps, &availableMemory, NULL) != DD_OK)
                Warningf("  Couldn't get video memory, using default");

            Displayf("  Total video memory: %dMB", (availableMemory >> 20));

            gfxInterface->AvailableMemory = availableMemory;

            gfxMaxScreenWidth = 0;
            gfxMaxScreenHeight = 0;

            stopwatch enumTimer;

            enumTimer.start();
            lpDD->EnumDisplayModes(0, 0, gfxInterface, $ResCallback);
            lpDD->Release();
            enumTimer.stop();

            Displayf("  Enumerated display modes in %.4f ms", enumTimer.elapsedMilliseconds());

            lpDD = nullptr;

            ++*gfxInterfaceCount;
        }

        timer.stop();

        Displayf("  Finished in %.4f ms", timer.elapsedMilliseconds());

        return TRUE;
    }

    static bool isVehInfoFile(const char *filename) {
        auto ext = strrchr(filename, '.');
        return (ext != NULL) ? (_strcmpi(ext, ".info") == 0) : false;
    }

    static bool isCityInfoFile(const char *filename) {
        auto ext = strrchr(filename, '.');
        return (ext != NULL) ? (_strcmpi(ext, ".cinfo") == 0) : false;
    }

    bool ParseStateArgs(void) {
        if (datArgParser::Get("allrewards"))
            MMSTATE->UnlockRewards = true;

        return true;
    }

    void GenerateRandomSeed() {
        cur_seed = stopwatch::ticks();
    }

    void ResetRandomSeed() {
        // make sure the seed was generated at least one time
        if (cur_seed == 0)
            GenerateRandomSeed();

        gRandSeed = cur_seed;
    }

    static void Install() {
        InstallCallback("CreateGameMutex", "Adds '-nomutex' argument to allow multiple game processes.",
            &CreateGameMutex, {
                cbHook<CALL>(0x40128D),
            }
        );

        InstallCallback("CheckGlobalMemory", "Disables check for available memory.",
            &NullSub, {
                cbHook<CALL>(0x401295),
            }
        );

        InstallCallback("ageDebug", "Verbose debug logger.",
            &ageDebug, {
                cbHook<JMP>(0x402630),
            }
        );

        InstallCallback("ProgressRect", "Fixes white loading bar in 32-bit display mode.",
            &ProgressRect, {
                cbHook<CALL>(0x401163),
                cbHook<CALL>(0x4011CC),
            }
        );

        if (!datArgParser::Get("oldautodetect"))
        {
            InstallCallback("ComputeCpuSpeed", "Removes the CPU speed calculation for the old auto detect method and improves startup times.",
                &ReturnNullOrZero, {
                    cbHook<CALL>(0x401208),
                }
            );

            // cbHook into the original AutoDetect and replace it with our own version
            InstallCallback("AutoDetectCallback", "Replaces the default AutoDetect method with a much faster one.",
                &AutoDetectCallback, {
                    cbHook<JMP>(0x4AC030),
                }
            );
        }

        InstallCallback("isVehInfoFile", "Fixes random crashes.",
            &isVehInfoFile, {
                cbHook<CALL>(0x5248E1),
            }
        );

        InstallCallback("isCityInfoFile", "Fixes random crashes.",
            &isCityInfoFile, {
                cbHook<CALL>(0x5244CF),
            }
        );

        // NOTE: Completely overrides the original AngelReadString (will check Lua first then DLL)
        InstallCallback("AngelReadString", "Adds support for Lua-based locale. Uses MMLANG.DLL on Lua failure.",
            &AngelReadString, {
                cbHook<JMP>(0x534790),
            }
        );

        InstallCallback("zipFile::Init", "Fixes 'extraLen' spam in the console/log.",
            &NullSub, {
                cbHook<CALL>(0x5738EA),
            }
        );

        // don't print certain errors unless specified
        if (!datArgParser::Get("physDebug")) {
            InstallCallback(&NullSub, {
                    cbHook<CALL>(0x469A20), // ; 'CollideInstances: Attempting to collide instances without bounds'
                    cbHook<CALL>(0x4692C5), // ; 'dgPhysManager::CollideProbe : instance has no bound'
                    cbHook<CALL>(0x469B24), // ; 'dgPhysManager::CollideTerrain - entry in room 0'
                }, "Disables physics collision error debugging (use '-physDebug' to enable)."
            );
        }
        
        InstallCallback(&ParseStateArgs, {
            cbHook<CALL>(0x4013A4)
        }, "State pack argument parsing.");

        if (datArgParser::Get("seed", 0, &cur_seed))
        {
            InstallCallback("ResetRandomSeed", "Resets the random seed to a user-specified one.",
                &ResetRandomSeed, {
                    cbHook<CALL>(0x4068F0), // mmReplayManager::ctor
                    cbHook<CALL>(0x406993), // mmReplayManager::Reset
                    cbHook<CALL>(0x444B79), // cityLevel::Load
                    cbHook<CALL>(0x536A68), // aiMap::Reset
                }
            );
        }
        else if (datArgParser::Get("randy"))
        {
            InstallCallback("GenerateRandomSeed", "Generates a new random seed instead of resetting it to a fixed value.",
                &GenerateRandomSeed, {
                    cbHook<CALL>(0x4068F0), // mmReplayManager::ctor
                    cbHook<CALL>(0x444B79), // cityLevel::Load
                }
            );

            InstallCallback("ResetRandomSeed", "Resets the random seed to one we previously generated.",
                &ResetRandomSeed, {
                    cbHook<CALL>(0x406993), // mmReplayManager::Reset
                    cbHook<CALL>(0x536A68), // aiMap::Reset
                }
            );
        }
    }
};

class TimeHandler {
public:
    static void Reset(void) {
        // TODO: reset tick stuff
    }

    static void Update(void) {
        MM2Lua::OnTick();

        // pass control back to MM2
        datTimeManager::Update();
    }

    static void Install() {
        InstallCallback("datTimeManager::Update", "Intercepts the call to update each tick.",
            &Update, {
                cbHook<CALL>(0x401A2F),
            }
        );
    }
};

class PrintHandler {
public:
    static void PrintString(LPCSTR message) {
        $DefaultPrintString(message);
    }

    static void Print(int level, LPCSTR message, va_list va_args) {
        static short printer_types[] = {
            TEXTCOLOR_DARKGRAY, // print
            TEXTCOLOR_DARKGRAY, // message
            TEXTCOLOR_DARKGRAY, // display
            TEXTCOLOR_YELLOW, // warning
            TEXTCOLOR_LIGHTRED, // error
            TEXTCOLOR_LIGHTRED, // quit/abort
        };

        HANDLE hConsole = ConsoleLog::GetOutputHandle();

        SetConsoleTextAttribute(hConsole, printer_types[level]);
        $DefaultPrinter(level, message, va_args);
        SetConsoleTextAttribute(hConsole, TEXTCOLOR_LIGHTGRAY);
    }

    static void FatalError() {
        // do something?
    }

    static void Install() {
        Printer = &Print;
        StringPrinter = &PrintString;
        FatalErrorHandler = &FatalError;
    }
};

class StackHandler {
public:
    static void GetAddressName(char *buffer, LPCSTR, int address) {
        /*
            TODO: Retrieve symbols from MM2Hook?
        */

        sprintf(buffer, "%08x (Unknown)", address);
    }

    static void GetAddressName(char *buffer, LPCSTR, int address, char *fnSymbol, int offset) {
        char fnName[1024] = { NULL };

        // no error checking (for now?)
        UnDecorateSymbolName(fnSymbol, fnName, sizeof(fnName),
            UNDNAME_COMPLETE
            | UNDNAME_NO_FUNCTION_RETURNS
            | UNDNAME_NO_ALLOCATION_MODEL
            | UNDNAME_NO_ALLOCATION_LANGUAGE
            | UNDNAME_NO_ACCESS_SPECIFIERS
            | UNDNAME_NO_THROW_SIGNATURES
            | UNDNAME_NO_MEMBER_TYPE
            | UNDNAME_NO_RETURN_UDT_MODEL
        );

        sprintf(buffer, "%08x (\"%s\"+%x)", address, fnName, offset);
    }

    static void Install() {
        InstallCallback("datStack::LookupAddress", "Allows for more detailed address information.",
            static_cast<void (*)(char*, LPCSTR, int, char*, int)>(&GetAddressName), {
                cbHook<CALL>(0x4C74DD), // sprintf
            }
        );

        InstallCallback("datStack::LookupAddress", "Allows for more detailed information of unknown symbols.",
            static_cast<void(*)(char*, LPCSTR, int)>(&GetAddressName), {
                cbHook<CALL>(0x4C74B9), // sprintf
            }
        );
    }
};

class HookSystemFramework
{
private:
    /*
        Installs all of the callbacks for MM2Hook.
    */
    static void InstallHandlers() {
        /*
            Initialize the important handlers first
        */

        InstallHandler<CallbackHandler>("Generic callbacks");
        InstallHandler<PrintHandler>("Print system");
        InstallHandler<TimeHandler>("Time manager");
        InstallHandler<StackHandler>("Stack information");

        InstallHandler<discordHandler>("Discord Rich Presence");

        /*
            Now install everything else
        */

        init_base::RunAll();
    }

    static void InstallPatches() {
        InstallPatch("Increase cop limit", { 64 }, {
            0x55100B,
        });

        InstallPatch("Use all parked cars", { 4 }, {
            0x579BE1,
        });

        InstallPatch("Fix crash for missing images", { 0xEB /* jnz -> jmp */ }, {
            0x4B329B, // gfxGetBitmap
        });

        InstallPatch("Disable lock check", { 0x65 /* jnz 40130D */ }, {
            0x4012A7, // Main
        });

        InstallPatch("Add replay button to main menu", { 0x3C }, {
            0x505EC3+2, // MainMenu::MainMenu(int)
        });

        InstallPatch("Fixes being kicked in multiplayer when losing focus", { 0xB8, 0x00, 0x00, 0x00, 0x00, 0xC3 /* mov eax, 0 -> ret */ }, {
            0x4390F0,   //mmGameMulti::LostCallback
        });
    }
public:
    static void Initialize(int argc, char **argv) {
        InstallPatches();
        InstallHandlers();

        // Initialize the Lua engine
        MM2Lua::Initialize();

        if (!datOutput::OpenLog("mm2.log", &logFileMethods)) {
            LogFile::WriteLine("Failed to initialize MM2 log!");
        }

        if (datArgParser::Get("age_debug") || datArgParser::Get("ageDebug"))
        {
            // AGE.log is a catch-all debug log
            // it will output _all_ debug to a file

            ageLogFile = fopen("AGE.log", "w+");
        } else {
            int logLevel = 0;

            // limit the amount of logging if specified
            // otherwise, everything will be captured
            //   1 = Printf, Messagef, Displayf
            //   2 = Warningf
            //   3 = Errorf
            if (datArgParser::Get("loglevel", 0, &logLevel) || datArgParser::Get("nolog")) {
                int outputMask = 0;

                if (logLevel >= 1)
                    outputMask |= 2;
                if (logLevel >= 2)
                    outputMask |= 4;
                if (logLevel >= 3)
                    outputMask |= 8;

                datOutput::SetOutputMask(outputMask);
            }

            // these will output to the console and mm2.log if specified

            if (datArgParser::Get("gfxDebug"))
                gfxDebug = 1;
            if (datArgParser::Get("audDebug"))
                audDebug = 1;
            if (datArgParser::Get("joyDebug"))
                joyDebug = 1;
            if (datArgParser::Get("assetDebug"))
                assetDebug = 1;
        }

        if (datArgParser::Get("noconsole")) {
            ConsoleLog::Close();
        }
    }

    static void Reset(bool restarting) {
        LogFile::Write("Hook reset request received: ");
        LogFile::WriteLine((restarting) ? "leaving GameLoop" : "entering GameLoop");

        if (restarting)
            MM2Lua::OnRestart();

        MM2Lua::Reset();
    }

    // TODO: fix this horrible logic
    static void Update(bool parsedStateArgs) {
        Reset(false);

        // GameLoop
        ageHook::StaticThunk<0x401A00>::Call<void>(parsedStateArgs);

        Reset(true);
    }

    static void Shutdown() {
        LogFile::WriteLine("Hook shutdown request received.");

        discordHandler::Release();

        // gfxPipeline::EndGfx2D
        ageHook::StaticThunk<0x4AAA10>::Call<void>();

        // we can now safely close everything else
        LogFile::Close();
        L.close(); // release Lua

        // close datOutput log
        datOutput::CloseLog();

        if (ageLogFile)
            fclose(ageLogFile);

        ConsoleLog::Close();
    }

    static void Install() {
        LogFile::WriteLine("Installing framework...");

        InstallCallback(
            &Update, {
                cbHook<CALL>(0x401989), // MainPhase
            }, "GameLoop hook" );

        InstallCallback(
            &Shutdown, {
                cbHook<CALL>(0x40161B) // Main
            }, "Shutdown hook");

        /*
            We'll hook into ArchInit (an empty function),
            and use it to install our callbacks/patches.

            However, this time around, we can now use datArgParser
            to determine if a patch/callback should be installed or not,
            whereas before we needed to check after it was already hooked in.

            Basically, this method is a lot safer, and guarantees
            we'll have access to any arguments passed.
        */

        InstallCallback("ArchInit", "Allows the hook to initialize before the game starts.",
            &Initialize, {
                cbHook<CALL>(0x4023DB),
            }
        );

        /*
            IMPORTANT:
            Add any patches/callbacks here that must be initialized prior to the game's entry point.
            This should be used for very very advanced callbacks/patches only!
        */
    }
};

/*
    ===========================================================================
*/
//
// Initialize all the important stuff prior to MM2 starting up.
// NOTE: We do not have access to datArgParser yet.
//
void Initialize(ageInfoLookup &gameInfo) {
    // initialize game manager
    pMM2 = new CMidtownMadness2(gameInfo.info);
    pMM2->Initialize();

    // install the framework
    HookSystemFramework::Install();
}

bool IsGameSupported(ageInfoLookup &gameInfo) {
    LogFile::WriteLine("Checking for known MM2 versions...");

    if (CMidtownMadness2::GetGameInfo(gameInfo))
    {
        LogFile::Format(" - Detected game version %d\n", gameInfo.info.engineVersion);
        return gameInfo.isSupported;
    } else {
        LogFile::WriteLine("Unknown module detected! Terminating...");
        MessageBox(NULL, "Unknown module! MM2Hook will now terminate the process.", "MM2Hook", MB_OK | MB_ICONERROR);

        ExitProcess(EXIT_FAILURE);
    }

    return false;
}

bool getPathSpec(char *path, char *dest, int destLen) {
    char ch;
    int idx = 0, len = 0;

    if ((path != NULL) && (dest != NULL))
    {
        while ((ch = path[idx++]) != NULL) {
            if (ch == '\\')
                len = idx;
        }

        if (len < destLen) {
            strncpy(dest, path, len);
            return true;
        }
    }

    return false;
}

static char mm2_path[MAX_PATH]{ NULL };

void initPath(void) {
    char dir[MAX_PATH]{ NULL };
    auto len = GetModuleFileName(NULL, dir, MAX_PATH);

    if (getPathSpec(dir, mm2_path, len)) {
        SetCurrentDirectory(mm2_path);
    } else {
        GetCurrentDirectory(MAX_PATH, mm2_path);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call)
	{
        case DLL_PROCESS_ATTACH:
        {
            ConsoleLog::Initialize();
            ConsoleLog::SetTitle("MM2Hook Console");

            debug("Initializing MM2Hook...");

            // setup the current directory
            initPath();

            // Initialize the log file
            LogFile::Initialize("mm2hook.log", "--<< MM2Hook log file >>--\n");
            LogFile::Format("Working directory is '%s'\n", mm2_path);

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
                } else {
                    LogFile::WriteLine("Failed to inject into the game process.");
                    MessageBox(NULL, "Could not inject into the game process. Unknown errors may occur.", "MM2Hook", MB_OK | MB_ICONWARNING);
                }
            }
            else
            {
                LogFile::WriteLine("Unsupported game version -- terminating...");
                MessageBox(NULL, "Sorry, this version of MM2 is unsupported. Please remove MM2Hook to launch the game.", "MM2Hook", MB_OK | MB_ICONERROR);

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