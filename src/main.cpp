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

template<int size, int count>
inline bool InstallGamePatch(LPCSTR name, MM2Version gameVersion, const PATCH_INSTALL_INFO<size, count> &data) {
    return InstallGamePatch(name, gameVersion, (LPVOID)&data, data.buffer_size, data.length);
}

bool InstallGamePatch(LPCSTR name, MM2Version gameVersion, LPVOID lpData, int buffer_size, int count) {
    LogFile::Format(" - Installing patch: '%s'...\n", name);

    auto *info = (PATCH_INSTALL_INFO<>*)lpData;

    if (count == 0 || buffer_size == 0) {
        LogFile::WriteLine(" - ERROR! Data is empty!");
        return false;
    }

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

template<int size>
inline bool InstallGameCallback(LPCSTR cb_name, MM2Version gameVersion, const CB_INSTALL_INFO<size> &cb) {
    return InstallGameCallback(cb_name, gameVersion, (LPVOID)&cb, cb.length);
}

bool InstallGameCallback(LPCSTR cb_name, MM2Version gameVersion, LPVOID lpCallback, int count) {
    LogFile::Format(" - Installing callback: '%s'...\n", cb_name);

    if (count == 0) {
        LogFile::WriteLine(" - ERROR! Address data is empty!");
        return false;
    }

    auto cb_info = (CB_INSTALL_INFO<>*)lpCallback;

    const DWORD cb = cb_info->cb_proc;
    int progress = 0;

    for (int i = 0; i < count; i++) {
        auto data = cb_info->cb_data[i];
        auto addr = data.addr_data.addresses[gameVersion];

        LogFile::Format("  - [%d/%d]: %08X => %08X : ", (i + 1), count, addr, cb);

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

bool isWindowSubclassed = false;
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
};

MM2PtrHook<int> cityCurrentTime INIT_DATA(NULL, NULL, 0x62B068);
MM2PtrHook<LPVOID> TIMEWEATHER INIT_DATA(NULL, NULL, 0x6299A8);

MM2PtrHook<UINT32*> sdlLightTable INIT_DATA(NULL, NULL, 0x62B0A0);

MM2FnHook<UINT32> sdlPage16__GetShadedColor INIT_DATA(NULL, NULL, 0x450880);

MM2FnHook<void> vglBegin INIT_DATA(NULL, NULL, 0x4A5500);
MM2FnHook<void> vglEnd   INIT_DATA(NULL, NULL, 0x4A5A90);
MM2PtrHook<UINT32> vglCurrentColor INIT_DATA(NULL, NULL, 0x661974);

MM2::cityTimeWeatherLighting * getTimeWeather(int time) {
    auto addr = (DWORD)TIMEWEATHER.ptr() + (time * sizeof(MM2::cityTimeWeatherLighting));
    return (MM2::cityTimeWeatherLighting *)addr;
};

MM2::cityTimeWeatherLighting * getCurrentTimeWeather(void) {
    return getTimeWeather(cityCurrentTime);
};

UINT32 vglColor;
UINT32 vglCalculatedColor = 0xFFFFFFFF;

MM2::Vector3 vglAmbient;
MM2::Vector3 vglKeyColor;
MM2::Vector3 vglFill1Color;
MM2::Vector3 vglFill2Color;
MM2::Vector3 vglShadedColor;

// IMPORTANT: ARGB as 4 bytes!
struct {
    char b;
    char g;
    char r;
    char a;
} vglResultColor = {
    (char)0,
    (char)0,
    (char)0,
    (char)255,
};

const double cosNum = 1.570796;

MM2::Vector3 addPitch(MM2::Vector3 *vec, float pitch) {
    bool pitchIsZero = (pitch >= 0.0f);

    return {
        (float)((!pitchIsZero) ? vec->X * cos(pitch + cosNum) : 0.0f),
        (float)((!pitchIsZero) ? vec->Y * cos(pitch + cosNum) : 0.0f),
        (float)((!pitchIsZero) ? vec->Z * cos(pitch + cosNum) : 0.0f),
    };
}

void normalize(float *value) {
    if (*value >= 2.0f)
        *value = 1.0f;
    if (*value > 1.0f)
        *value = (*value - (*value - 1.0f));
};

void normalizeVector(MM2::Vector3 *vec) {
    normalize(&vec->X);
    normalize(&vec->Y);
    normalize(&vec->Z);
}

UINT32 CalculateShading(MM2::cityTimeWeatherLighting *timeWeather) {
    auto ambientColor = timeWeather->Ambient;

    // convert the ambient to a vector3 for better accuracy
    vglAmbient = {
        (float)((char)((ambientColor & 0xFF0000) >> 16) / 256.0),
        (float)((char)((ambientColor & 0xFF00) >> 8) / 256.0),
        (float)((char)((ambientColor & 0xFF)) / 256.0),
    };

    vglKeyColor = addPitch(&timeWeather->KeyColor, timeWeather->KeyPitch);
    vglFill1Color = addPitch(&timeWeather->Fill1Color, timeWeather->Fill1Pitch);
    vglFill2Color = addPitch(&timeWeather->Fill2Color, timeWeather->Fill2Pitch);

    // compute le values
    vglShadedColor = {
        ((vglKeyColor.X + vglFill1Color.X + vglFill2Color.X) + vglAmbient.X),
        ((vglKeyColor.Y + vglFill1Color.Y + vglFill2Color.Y) + vglAmbient.Y),
        ((vglKeyColor.Z + vglFill1Color.Z + vglFill2Color.Z) + vglAmbient.Z),
    };

    normalizeVector(&vglShadedColor);

    vglResultColor.r = (char)(vglShadedColor.X * 255.999);
    vglResultColor.g = (char)(vglShadedColor.Y * 255.999);
    vglResultColor.b = (char)(vglShadedColor.Z * 255.999);

    return *(int*)&vglResultColor;
}

class GraphicsCallbackHandler
{
public:
    static void vglSDLBegin(int gfxMode, int p1) {
        vglColor = vglCurrentColor;
        vglCalculatedColor = CalculateShading(getCurrentTimeWeather());

        // shade shit bruh
        vglCalculatedColor = sdlPage16__GetShadedColor(vglColor, vglCalculatedColor);

        // hijack current color
        vglCurrentColor.set_ptr(vglCalculatedColor);
        vglBegin(gfxMode, p1);
    }

    static void vglSDLEnd(void) {
        // restore color
        vglCurrentColor.set_ptr(vglColor);
        vglEnd();
    }
};

/*
    ===========================================================================
*/

void InitializeLua() {
    // Guaranteed to be loaded before anything vital is called (e.g. AngelReadString)
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

void InitializeHook(int, char**) {
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
        if (!isWindowSubclassed)
        {
            LogFile::Write("Subclassing window...");
            LogFile::WriteLine(SubclassGameWindow(pMM2->GetMainWindowHwnd(), WndProcNew, &hProcOld) ? "Done!" : "FAIL!");

            isWindowSubclassed = true;
        }
        
        // GameLoop was restarted
        ResetHook(false);
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

    InstallGamePatch("Increase chat buffer size", gameVersion, chatSize_patch);
};

// Replaces a call to ArchInit (a null function) just before ExceptMain
// This is PERFECT for initializing everything before the game even starts!
const CB_INSTALL_INFO<1> archInit_CB {
    &InitializeHook,
    {
        {{ NULL, NULL, 0x4023DB }, HOOK_CALL }
    }
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

const CB_INSTALL_INFO<1> angelReadString_CB{
    &CallbackHandler::AngelReadString,
    {
        // NOTE: Completely overrides AngelReadString!
        {{ NULL, NULL, 0x534790 }, HOOK_JMP }
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

const CB_INSTALL_INFO<64> vglSDLBegin_CB{
    &GraphicsCallbackHandler::vglSDLBegin,
    {
        {{ NULL, NULL, 0x448424 }, HOOK_CALL },
        {{ NULL, NULL, 0x448697 }, HOOK_CALL },
        {{ NULL, NULL, 0x448903 }, HOOK_CALL },
        {{ NULL, NULL, 0x448BFD }, HOOK_CALL },
        {{ NULL, NULL, 0x448DE4 }, HOOK_CALL },
        {{ NULL, NULL, 0x44902A }, HOOK_CALL },
        {{ NULL, NULL, 0x4492A4 }, HOOK_CALL },
        {{ NULL, NULL, 0x4494C3 }, HOOK_CALL },
        {{ NULL, NULL, 0x4496A5 }, HOOK_CALL },
        {{ NULL, NULL, 0x44986B }, HOOK_CALL },
        {{ NULL, NULL, 0x449A13 }, HOOK_CALL },
        {{ NULL, NULL, 0x449BD9 }, HOOK_CALL },
        {{ NULL, NULL, 0x449D82 }, HOOK_CALL },
        {{ NULL, NULL, 0x449F67 }, HOOK_CALL },
        {{ NULL, NULL, 0x44A21C }, HOOK_CALL },
        {{ NULL, NULL, 0x44A444 }, HOOK_CALL },
        {{ NULL, NULL, 0x44A629 }, HOOK_CALL },
        {{ NULL, NULL, 0x44A7EF }, HOOK_CALL },
        {{ NULL, NULL, 0x44A997 }, HOOK_CALL },
        {{ NULL, NULL, 0x44AB5D }, HOOK_CALL },
        {{ NULL, NULL, 0x44AD06 }, HOOK_CALL },
        {{ NULL, NULL, 0x44AECA }, HOOK_CALL },
        {{ NULL, NULL, 0x44B0EC }, HOOK_CALL },
        {{ NULL, NULL, 0x44B24B }, HOOK_CALL },
        {{ NULL, NULL, 0x44B3B6 }, HOOK_CALL },
        {{ NULL, NULL, 0x44B557 }, HOOK_CALL },
        {{ NULL, NULL, 0x44B6F3 }, HOOK_CALL },
        {{ NULL, NULL, 0x44B8F1 }, HOOK_CALL },
        {{ NULL, NULL, 0x44BA8A }, HOOK_CALL },
        {{ NULL, NULL, 0x44BC29 }, HOOK_CALL },
        {{ NULL, NULL, 0x44BE9C }, HOOK_CALL },
        {{ NULL, NULL, 0x44C136 }, HOOK_CALL },
        {{ NULL, NULL, 0x44C40C }, HOOK_CALL },
        {{ NULL, NULL, 0x44C64A }, HOOK_CALL },
        {{ NULL, NULL, 0x44C7C0 }, HOOK_CALL },
        {{ NULL, NULL, 0x44CAD6 }, HOOK_CALL },
        {{ NULL, NULL, 0x44CCF5 }, HOOK_CALL },
        {{ NULL, NULL, 0x44CF6D }, HOOK_CALL },
        {{ NULL, NULL, 0x44D0D4 }, HOOK_CALL },
        {{ NULL, NULL, 0x44D5F7 }, HOOK_CALL },
        {{ NULL, NULL, 0x44D789 }, HOOK_CALL },
        {{ NULL, NULL, 0x44DC55 }, HOOK_CALL },
        {{ NULL, NULL, 0x44E050 }, HOOK_CALL },
        {{ NULL, NULL, 0x44E14B }, HOOK_CALL },
        {{ NULL, NULL, 0x44E2A3 }, HOOK_CALL },
        {{ NULL, NULL, 0x44E69D }, HOOK_CALL },
        {{ NULL, NULL, 0x44E79E }, HOOK_CALL },
        {{ NULL, NULL, 0x44EAA0 }, HOOK_CALL },
        {{ NULL, NULL, 0x44EBA5 }, HOOK_CALL },
        {{ NULL, NULL, 0x44EFD0 }, HOOK_CALL },
        {{ NULL, NULL, 0x44F0DC }, HOOK_CALL },
        {{ NULL, NULL, 0x44F588 }, HOOK_CALL },
        {{ NULL, NULL, 0x44F7E2 }, HOOK_CALL },
        {{ NULL, NULL, 0x44FC1E }, HOOK_CALL },
        {{ NULL, NULL, 0x44FDD4 }, HOOK_CALL },
        {{ NULL, NULL, 0x44FF10 }, HOOK_CALL },
        {{ NULL, NULL, 0x450085 }, HOOK_CALL },
        {{ NULL, NULL, 0x450269 }, HOOK_CALL },
        // ------------------------------------
        {{ NULL, NULL, 0x443B9D }, HOOK_CALL }, // dgRoadDecalInstance
        {{ NULL, NULL, 0x57AC4A }, HOOK_CALL }, // ped LODs

        // some multiplayer object
        {{ NULL, NULL, 0x43DA66 }, HOOK_CALL },
        {{ NULL, NULL, 0x43DAB9 }, HOOK_CALL },
        {{ NULL, NULL, 0x43DB2F }, HOOK_CALL },
        {{ NULL, NULL, 0x43DBD6 }, HOOK_CALL },
        // ----------------------------------
    }
};

const CB_INSTALL_INFO<64> vglSDLEnd_CB{
    &GraphicsCallbackHandler::vglSDLEnd,
    {
        {{ NULL, NULL, 0x4485D3 }, HOOK_CALL },
        {{ NULL, NULL, 0x448B82 }, HOOK_CALL },
        {{ NULL, NULL, 0x448D8C }, HOOK_CALL },
        {{ NULL, NULL, 0x448FB7 }, HOOK_CALL },
        {{ NULL, NULL, 0x449219 }, HOOK_CALL },
        {{ NULL, NULL, 0x449480 }, HOOK_CALL },
        {{ NULL, NULL, 0x44963E }, HOOK_CALL },
        {{ NULL, NULL, 0x44983C }, HOOK_CALL },
        {{ NULL, NULL, 0x4499D4 }, HOOK_CALL },
        {{ NULL, NULL, 0x449BAA }, HOOK_CALL },
        {{ NULL, NULL, 0x449D42 }, HOOK_CALL },
        {{ NULL, NULL, 0x449F5A }, HOOK_CALL },
        {{ NULL, NULL, 0x44A146 }, HOOK_CALL },
        {{ NULL, NULL, 0x44A3F8 }, HOOK_CALL },
        {{ NULL, NULL, 0x44A5BF }, HOOK_CALL },
        {{ NULL, NULL, 0x44A7C0 }, HOOK_CALL },
        {{ NULL, NULL, 0x44A958 }, HOOK_CALL },
        {{ NULL, NULL, 0x44AB2E }, HOOK_CALL },
        {{ NULL, NULL, 0x44ACC6 }, HOOK_CALL },
        {{ NULL, NULL, 0x44AEBC }, HOOK_CALL },
        {{ NULL, NULL, 0x44B083 }, HOOK_CALL },
        {{ NULL, NULL, 0x44B23D }, HOOK_CALL },
        {{ NULL, NULL, 0x44B394 }, HOOK_CALL },
        {{ NULL, NULL, 0x44B531 }, HOOK_CALL },
        {{ NULL, NULL, 0x44B6E1 }, HOOK_CALL },
        {{ NULL, NULL, 0x44B895 }, HOOK_CALL },
        {{ NULL, NULL, 0x44BA7C }, HOOK_CALL },
        {{ NULL, NULL, 0x44BC03 }, HOOK_CALL },
        {{ NULL, NULL, 0x44BE8E }, HOOK_CALL },
        {{ NULL, NULL, 0x44C118 }, HOOK_CALL },
        {{ NULL, NULL, 0x44C3EA }, HOOK_CALL },
        {{ NULL, NULL, 0x44C638 }, HOOK_CALL },
        {{ NULL, NULL, 0x44C77A }, HOOK_CALL },
        {{ NULL, NULL, 0x44C989 }, HOOK_CALL },
        {{ NULL, NULL, 0x44CC44 }, HOOK_CALL },
        {{ NULL, NULL, 0x44CE63 }, HOOK_CALL },
        {{ NULL, NULL, 0x44D04E }, HOOK_CALL },
        {{ NULL, NULL, 0x44D403 }, HOOK_CALL },
        {{ NULL, NULL, 0x44D780 }, HOOK_CALL },
        {{ NULL, NULL, 0x44D8E9 }, HOOK_CALL },
        {{ NULL, NULL, 0x44E014 }, HOOK_CALL },
        {{ NULL, NULL, 0x44E131 }, HOOK_CALL },
        {{ NULL, NULL, 0x44E22C }, HOOK_CALL },
        {{ NULL, NULL, 0x44E661 }, HOOK_CALL },
        {{ NULL, NULL, 0x44E785 }, HOOK_CALL },
        {{ NULL, NULL, 0x44E886 }, HOOK_CALL },
        {{ NULL, NULL, 0x44EB82 }, HOOK_CALL },
        {{ NULL, NULL, 0x44EDC3 }, HOOK_CALL },
        {{ NULL, NULL, 0x44F0B9 }, HOOK_CALL },
        {{ NULL, NULL, 0x44F316 }, HOOK_CALL },
        {{ NULL, NULL, 0x44F64C }, HOOK_CALL },
        {{ NULL, NULL, 0x44FB9D }, HOOK_CALL },
        {{ NULL, NULL, 0x44FD30 }, HOOK_CALL },
        {{ NULL, NULL, 0x44FE4E }, HOOK_CALL },
        {{ NULL, NULL, 0x44FFB3 }, HOOK_CALL },
        {{ NULL, NULL, 0x450162 }, HOOK_CALL },
        {{ NULL, NULL, 0x450390 }, HOOK_CALL },
        {{ NULL, NULL, 0x45078C }, HOOK_CALL },
        // ------------------------------------
        {{ NULL, NULL, 0x443DCC }, HOOK_CALL }, // road decals
        {{ NULL, NULL, 0x57AD41 }, HOOK_CALL }, // ped LODs

        // some multiplayer object
        {{ NULL, NULL, 0x43DAB0 }, HOOK_CALL },
        {{ NULL, NULL, 0x43DB03 }, HOOK_CALL },
        {{ NULL, NULL, 0x43DBB7 }, HOOK_CALL },
        {{ NULL, NULL, 0x43DC61 }, HOOK_CALL },
        // ----------------------------------
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

            InstallGameCallback("TrialTimeExpired", gameVersion, trialTimeExpired_CB);
        } break;
    }

    InstallGameCallback("ArchInit", gameVersion, archInit_CB);
    InstallGameCallback("Tick", gameVersion, tick_CB);
    InstallGameCallback("ageDebug", gameVersion, ageDebug_CB);

    // not supported for betas yet
    if (gameVersion == MM2_RETAIL)
    {
        InstallGameCallback("AngelReadString", gameVersion, angelReadString_CB);
    }

    InstallGameCallback("LoadAmbientSFX", gameVersion, loadAmbientSFX_CB);
    InstallGameCallback("SendChatMessage", gameVersion, sendChatMsg_CB);
    InstallGameCallback("SetSirenCSVName", gameVersion, sirenCSV_CB);
    InstallGameCallback("vglBegin (PSDL)", gameVersion, vglSDLBegin_CB);
    InstallGameCallback("vglEnd (PSDL)", gameVersion, vglSDLEnd_CB);
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

