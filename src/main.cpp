#include "main.h"

LPFNDIRECTINPUTCREATE lpDICreate;

// Export as 'DirectInputCreateA/W' so we can hook into MM2
// (Apparently DirectInputCreateW gets called sometimes...)
#pragma comment(linker, "/EXPORT:DirectInputCreateA=_DirectInputCreateA_Impl")
#pragma comment(linker, "/EXPORT:DirectInputCreateW=_DirectInputCreateA_Impl")
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

struct FN_PTR {
    const LPVOID lpHandler;

    template<typename T>
    constexpr FN_PTR(T *test) : lpHandler((LPVOID)(*(DWORD*)&test)) {};

    template<class TT, typename T>
    constexpr FN_PTR(T(TT::*test)) : lpHandler((LPVOID)(*(DWORD*)&test)) {};

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

    FN_PTR cb_proc;
    struct {
        MM2AddressData addr_data;
        CB_HOOK_TYPE type;
    } cb_data[size];
};

template<int size = 1, int count = 1>
struct PATCH_INSTALL_INFO {
    static const int length = count;
    static const int buffer_size = size;

    char buffer[size];
    MM2AddressData addrData[count];
};

template<int count = 1>
struct VT_INSTALL_INFO {
    static const int length = count;

    FN_PTR dwHookAddr;
    MM2AddressData addrData[count];
};

#define INIT_CB_DATA(type, addr1, addr2, addr3) {{ addr1, addr2, addr3 }, type }

#define INIT_CB_CALL(addr1, addr2, addr3) INIT_CB_DATA(HOOK_CALL, addr1, addr2, addr3)
#define INIT_CB_JUMP(addr1, addr2, addr3) INIT_CB_DATA(HOOK_JMP, addr1, addr2, addr3)

template<int count>
inline bool InstallVTableHook(LPCSTR name, MM2Version gameVersion, const VT_INSTALL_INFO<count> &data) {
    return InstallVTableHook(name, gameVersion, (LPVOID)&data, data.length);
}

bool InstallVTableHook(LPCSTR name, MM2Version gameVersion, LPVOID lpData, int count) {
    LogFile::Format(" - Installing V-Table hook: '%s'...\n", name);

    auto info = (VT_INSTALL_INFO<>*)lpData;

    if (count == 0) {
        LogFile::WriteLine(" - ERROR! Data is empty!");
        return false;
    }

    int progress = 0;

    for (int i = 0; i < count; i++) {
        auto addr = info->addrData[i].addresses[gameVersion];

        LogFile::Format("  - [%d] %08X => %08X : ", (i + 1), addr, info->dwHookAddr);

        if (addr != NULL)
        {
            InstallVTHook(addr, info->dwHookAddr);
            LogFile::WriteLine("OK");

            progress++;
        } else LogFile::WriteLine("Not supported");
    }

    bool success = (progress > 0);
    return success;
}

template<int size, int count>
inline bool InstallGamePatch(LPCSTR name, MM2Version gameVersion, const PATCH_INSTALL_INFO<size, count> &data) {
    return InstallGamePatch(name, gameVersion, (LPVOID)&data, data.buffer_size, data.length);
}

bool InstallGamePatch(LPCSTR name, MM2Version gameVersion, LPVOID lpData, int buffer_size, int count) {
    LogFile::Format(" - Installing patch: '%s'...\n", name);

    auto info = (PATCH_INSTALL_INFO<>*)lpData;

    if (count == 0 || buffer_size == 0) {
        LogFile::WriteLine(" - ERROR! Data is empty!");
        return false;
    }

    int progress = 0;

    for (int i = 0; i < count; i++) {
        auto addr = info->addrData[i].addresses[gameVersion];

        LogFile::Format("  - [%d] %08X => %08X : ", (i + 1), addr, (BYTE*)&info->buffer);

        if (addr != NULL)
        {
            InstallPatch(addr, (BYTE*)&info->buffer, buffer_size);
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

MM2PtrHook<int> cityCurrentTime ( NULL, NULL, 0x62B068 );
MM2PtrHook<MM2::cityTimeWeatherLighting> 
                TIMEWEATHER     ( NULL, NULL, 0x6299A8 );

MM2PtrHook<UINT32> vglCurrentColor ( NULL, NULL, 0x661974 );

MM2FnHook<void> $vglBegin     ( NULL, NULL, 0x4A5500 );
MM2FnHook<void> $vglEnd       ( NULL, NULL, 0x4A5A90 );

MM2FnHook<UINT32> $sdlPage16_GetShadedColor ( NULL, NULL, 0x450880 );

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
    float p = (float)fmod(pitch, 3.14159);
    bool pitchIsZero = (pitch >= 0.0f);

    return {
        (float)((!pitchIsZero) ? vec->X * cos(pitch + cosNum) : 0.0f),
        (float)((!pitchIsZero) ? vec->Y * cos(pitch + cosNum) : 0.0f),
        (float)((!pitchIsZero) ? vec->Z * cos(pitch + cosNum) : 0.0f),
    };
}

float normalize(float value) {
    if (value >= 2.0f)
        value = 1.0f;

    return (value > 1.0f) ? (value - (value - 1.0f)) : value;
};

MM2::Vector3 intToColor(int value) {
    return {
        (float)((char)((value & 0xFF0000) >> 16) / 256.0),
        (float)((char)((value & 0xFF00) >> 8) / 256.0),
        (float)((char)((value & 0xFF)) / 256.0),
    };
}

UINT32 CalculateShadedColor(int color) {
    auto timeWeather = TIMEWEATHER[cityCurrentTime];

    vglKeyColor = addPitch(&timeWeather->KeyColor, timeWeather->KeyPitch);
    vglFill1Color = addPitch(&timeWeather->Fill1Color, timeWeather->Fill1Pitch);
    vglFill2Color = addPitch(&timeWeather->Fill2Color, timeWeather->Fill2Pitch);

    // convert the ambient to a vector3 for better accuracy
    vglAmbient = intToColor(timeWeather->Ambient);

    // compute le values
    vglShadedColor = {
        normalize((vglKeyColor.X + vglFill1Color.X + vglFill2Color.X) + vglAmbient.X),
        normalize((vglKeyColor.Y + vglFill1Color.Y + vglFill2Color.Y) + vglAmbient.Y),
        normalize((vglKeyColor.Z + vglFill1Color.Z + vglFill2Color.Z) + vglAmbient.Z),
    };

    vglResultColor.r = (char)(vglShadedColor.X * 255.999);
    vglResultColor.g = (char)(vglShadedColor.Y * 255.999);
    vglResultColor.b = (char)(vglShadedColor.Z * 255.999);

    return $sdlPage16_GetShadedColor(color, *(int*)&vglResultColor);
}

class GraphicsCallbackHandler
{
public:
    static void vglBegin(int gfxMode, int p1) {
        vglColor = vglCurrentColor;

        // calculate a nice shaded color ;)
        vglCalculatedColor = CalculateShadedColor(vglColor);
        vglCurrentColor.set(vglCalculatedColor);

        $vglBegin(gfxMode, p1);
    }

    static void vglEnd(void) {
        // restore color
        vglCurrentColor.set(vglColor);
        $vglEnd();
    }
};

MM2FnHook<void> $dgBangerInstance_Draw ( NULL, NULL, 0x4415E0 );

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

MM2FnHook<void> $asLinearCS_Update ( NULL, NULL, 0x4A3370);

static MM2::Matrix34 sm_DashOffset;

class mmDashViewCallbackHandler
{
public:
    void UpdateCS() {
        auto dashCam = getPtr<MM2::Matrix34>(this, 0x18);

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

MM2PtrHook<BOOL> gameClosing ( 0x667DEC, 0x6B0150, 0x6B1708 );

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

MM2PtrHook<DWORD> vtResumeSampling    ( 0x5C86B8, 0x5DF710, 0x5E0CC4 );
MM2PtrHook<DWORD> vtPauseSampling     ( 0x5C86C8, 0x5DF724, 0x5E0CD8 );

bool HookFramework(MM2Version gameVersion) {
    if (gameVersion == MM2_INVALID || gameVersion > MM2_NUM_VERSIONS)
        return false;

    vtResumeSampling.set((DWORD)&HookStartup);
    vtPauseSampling.set((DWORD)&HookShutdown);

    return true;
};

const PATCH_INSTALL_INFO<1, 3> chatSize_patch = {
    { 60 }, {
        { NULL, NULL, 0x4E68B5 },
        { NULL, NULL, 0x4E68B9 },
        { NULL, NULL, 0x50BBCF }
    }
};

void InstallPatches(MM2Version gameVersion) {
    LogFile::WriteLine("Installing patches...");

    InstallGamePatch("Increase chat buffer size", gameVersion, chatSize_patch);
};

/*
    Callbacks
*/

// Replaces a call to ArchInit (a null function) just before ExceptMain
// This is PERFECT for initializing everything before the game even starts!
const CB_INSTALL_INFO<1> archInit_CB = {
    &InitializeHook, {
        INIT_CB_CALL( NULL, NULL, 0x4023DB ),
    }
};

const CB_INSTALL_INFO<1> tick_CB = {
    &TickHandler::Update, {
        INIT_CB_CALL( NULL, NULL, 0x401A2F ),
    }
};

const CB_INSTALL_INFO<1> ageDebug_CB = {
    &CallbackHandler::ageDebug, {
        INIT_CB_JUMP( NULL, NULL, 0x402630 ),
    }
};

const CB_INSTALL_INFO<1> angelReadString_CB = {
    &CallbackHandler::AngelReadString, {
        // NOTE: Completely overrides AngelReadString!
        INIT_CB_JUMP( NULL, NULL, 0x534790 ),
    }
};

const CB_INSTALL_INFO<1> sendChatMsg_CB = {
    &ChatHandler::Process, {
        INIT_CB_JUMP( NULL, NULL, 0x414EB6 ),
    }
};

const CB_INSTALL_INFO<1> loadAmbientSFX_CB = {
    &CallbackHandler::LoadAmbientSFX, {
        INIT_CB_CALL( NULL, NULL, 0x433F93 ),
    }
};

const CB_INSTALL_INFO<2> sirenCSV_CB = {
    &CallbackHandler::SetSirenCSVName, {
        INIT_CB_CALL( NULL, NULL, 0x412783 ),
        INIT_CB_CALL( NULL, NULL, 0x412772 ),
    }
};

const CB_INSTALL_INFO<60> vglBegin_CB = {
    &GraphicsCallbackHandler::vglBegin, {
        INIT_CB_CALL( NULL, NULL, 0x448424 ), 
        INIT_CB_CALL( NULL, NULL, 0x448697 ), 
        INIT_CB_CALL( NULL, NULL, 0x448903 ), 
        INIT_CB_CALL( NULL, NULL, 0x448BFD ), 
        INIT_CB_CALL( NULL, NULL, 0x448DE4 ), 
        INIT_CB_CALL( NULL, NULL, 0x44902A ), 
        INIT_CB_CALL( NULL, NULL, 0x4492A4 ), 
        INIT_CB_CALL( NULL, NULL, 0x4494C3 ), 
        INIT_CB_CALL( NULL, NULL, 0x4496A5 ), 
        INIT_CB_CALL( NULL, NULL, 0x44986B ), 
        INIT_CB_CALL( NULL, NULL, 0x449A13 ), 
        INIT_CB_CALL( NULL, NULL, 0x449BD9 ), 
        INIT_CB_CALL( NULL, NULL, 0x449D82 ), 
        INIT_CB_CALL( NULL, NULL, 0x449F67 ), 
        INIT_CB_CALL( NULL, NULL, 0x44A21C ), 
        INIT_CB_CALL( NULL, NULL, 0x44A444 ), 
        INIT_CB_CALL( NULL, NULL, 0x44A629 ), 
        INIT_CB_CALL( NULL, NULL, 0x44A7EF ), 
        INIT_CB_CALL( NULL, NULL, 0x44A997 ), 
        INIT_CB_CALL( NULL, NULL, 0x44AB5D ), 
        INIT_CB_CALL( NULL, NULL, 0x44AD06 ), 
        INIT_CB_CALL( NULL, NULL, 0x44AECA ), 
        INIT_CB_CALL( NULL, NULL, 0x44B0EC ), 
        INIT_CB_CALL( NULL, NULL, 0x44B24B ), 
        INIT_CB_CALL( NULL, NULL, 0x44B3B6 ), 
        INIT_CB_CALL( NULL, NULL, 0x44B557 ), 
        INIT_CB_CALL( NULL, NULL, 0x44B6F3 ), 
        INIT_CB_CALL( NULL, NULL, 0x44B8F1 ), 
        INIT_CB_CALL( NULL, NULL, 0x44BA8A ), 
        INIT_CB_CALL( NULL, NULL, 0x44BC29 ), 
        INIT_CB_CALL( NULL, NULL, 0x44BE9C ), 
        INIT_CB_CALL( NULL, NULL, 0x44C136 ), 
        INIT_CB_CALL( NULL, NULL, 0x44C40C ), 
        INIT_CB_CALL( NULL, NULL, 0x44C64A ), 
        INIT_CB_CALL( NULL, NULL, 0x44C7C0 ), 
        INIT_CB_CALL( NULL, NULL, 0x44CAD6 ), 
        INIT_CB_CALL( NULL, NULL, 0x44CCF5 ), 
        INIT_CB_CALL( NULL, NULL, 0x44CF6D ), 
        INIT_CB_CALL( NULL, NULL, 0x44D0D4 ), 
        INIT_CB_CALL( NULL, NULL, 0x44D5F7 ), 
        INIT_CB_CALL( NULL, NULL, 0x44D789 ), 
        INIT_CB_CALL( NULL, NULL, 0x44DC55 ), 
        INIT_CB_CALL( NULL, NULL, 0x44E050 ), 
        INIT_CB_CALL( NULL, NULL, 0x44E14B ), 
        INIT_CB_CALL( NULL, NULL, 0x44E2A3 ), 
        INIT_CB_CALL( NULL, NULL, 0x44E69D ), 
        INIT_CB_CALL( NULL, NULL, 0x44E79E ), 
        INIT_CB_CALL( NULL, NULL, 0x44EAA0 ), 
        INIT_CB_CALL( NULL, NULL, 0x44EBA5 ), 
        INIT_CB_CALL( NULL, NULL, 0x44EFD0 ), 
        INIT_CB_CALL( NULL, NULL, 0x44F0DC ), 
        INIT_CB_CALL( NULL, NULL, 0x44F588 ), 
        INIT_CB_CALL( NULL, NULL, 0x44F7E2 ), 
        INIT_CB_CALL( NULL, NULL, 0x44FC1E ), 
        INIT_CB_CALL( NULL, NULL, 0x44FDD4 ), 
        INIT_CB_CALL( NULL, NULL, 0x44FF10 ), 
        INIT_CB_CALL( NULL, NULL, 0x450085 ), 
        INIT_CB_CALL( NULL, NULL, 0x450269 ), 
        // ------------------------------------
        INIT_CB_CALL( NULL, NULL, 0x443B9D ), // dgRoadDecalInstance
        INIT_CB_CALL( NULL, NULL, 0x57AC4A ), // ped LODs
    }
};

const CB_INSTALL_INFO<60> vglEnd_CB = {
    &GraphicsCallbackHandler::vglEnd, {
        INIT_CB_CALL( NULL, NULL, 0x4485D3 ),
        INIT_CB_CALL( NULL, NULL, 0x448B82 ),
        INIT_CB_CALL( NULL, NULL, 0x448D8C ),
        INIT_CB_CALL( NULL, NULL, 0x448FB7 ),
        INIT_CB_CALL( NULL, NULL, 0x449219 ),
        INIT_CB_CALL( NULL, NULL, 0x449480 ),
        INIT_CB_CALL( NULL, NULL, 0x44963E ),
        INIT_CB_CALL( NULL, NULL, 0x44983C ),
        INIT_CB_CALL( NULL, NULL, 0x4499D4 ),
        INIT_CB_CALL( NULL, NULL, 0x449BAA ),
        INIT_CB_CALL( NULL, NULL, 0x449D42 ),
        INIT_CB_CALL( NULL, NULL, 0x449F5A ),
        INIT_CB_CALL( NULL, NULL, 0x44A146 ),
        INIT_CB_CALL( NULL, NULL, 0x44A3F8 ),
        INIT_CB_CALL( NULL, NULL, 0x44A5BF ),
        INIT_CB_CALL( NULL, NULL, 0x44A7C0 ),
        INIT_CB_CALL( NULL, NULL, 0x44A958 ),
        INIT_CB_CALL( NULL, NULL, 0x44AB2E ),
        INIT_CB_CALL( NULL, NULL, 0x44ACC6 ),
        INIT_CB_CALL( NULL, NULL, 0x44AEBC ),
        INIT_CB_CALL( NULL, NULL, 0x44B083 ),
        INIT_CB_CALL( NULL, NULL, 0x44B23D ),
        INIT_CB_CALL( NULL, NULL, 0x44B394 ),
        INIT_CB_CALL( NULL, NULL, 0x44B531 ),
        INIT_CB_CALL( NULL, NULL, 0x44B6E1 ),
        INIT_CB_CALL( NULL, NULL, 0x44B895 ),
        INIT_CB_CALL( NULL, NULL, 0x44BA7C ),
        INIT_CB_CALL( NULL, NULL, 0x44BC03 ),
        INIT_CB_CALL( NULL, NULL, 0x44BE8E ),
        INIT_CB_CALL( NULL, NULL, 0x44C118 ),
        INIT_CB_CALL( NULL, NULL, 0x44C3EA ),
        INIT_CB_CALL( NULL, NULL, 0x44C638 ),
        INIT_CB_CALL( NULL, NULL, 0x44C77A ),
        INIT_CB_CALL( NULL, NULL, 0x44C989 ),
        INIT_CB_CALL( NULL, NULL, 0x44CC44 ),
        INIT_CB_CALL( NULL, NULL, 0x44CE63 ),
        INIT_CB_CALL( NULL, NULL, 0x44D04E ),
        INIT_CB_CALL( NULL, NULL, 0x44D403 ),
        INIT_CB_CALL( NULL, NULL, 0x44D780 ),
        INIT_CB_CALL( NULL, NULL, 0x44D8E9 ),
        INIT_CB_CALL( NULL, NULL, 0x44E014 ),
        INIT_CB_CALL( NULL, NULL, 0x44E131 ),
        INIT_CB_CALL( NULL, NULL, 0x44E22C ),
        INIT_CB_CALL( NULL, NULL, 0x44E661 ),
        INIT_CB_CALL( NULL, NULL, 0x44E785 ),
        INIT_CB_CALL( NULL, NULL, 0x44E886 ),
        INIT_CB_CALL( NULL, NULL, 0x44EB82 ),
        INIT_CB_CALL( NULL, NULL, 0x44EDC3 ),
        INIT_CB_CALL( NULL, NULL, 0x44F0B9 ),
        INIT_CB_CALL( NULL, NULL, 0x44F316 ),
        INIT_CB_CALL( NULL, NULL, 0x44F64C ),
        INIT_CB_CALL( NULL, NULL, 0x44FB9D ),
        INIT_CB_CALL( NULL, NULL, 0x44FD30 ),
        INIT_CB_CALL( NULL, NULL, 0x44FE4E ),
        INIT_CB_CALL( NULL, NULL, 0x44FFB3 ),
        INIT_CB_CALL( NULL, NULL, 0x450162 ),
        INIT_CB_CALL( NULL, NULL, 0x450390 ),
        INIT_CB_CALL( NULL, NULL, 0x45078C ),
        // ------------------------------------
        INIT_CB_CALL( NULL, NULL, 0x443DCC ), // road decals
        INIT_CB_CALL( NULL, NULL, 0x57AD41 ), // ped LODs
    }
};

const CB_INSTALL_INFO<2> bridgeFerryCull_CB = {
    &BridgeFerryCallbackHandler::Cull, {
        INIT_CB_CALL( NULL, NULL, 0x5780BC ), // gizBridgeMgr::Cull
        INIT_CB_CALL( NULL, NULL, 0x5798F0 ), // gizFerryMgr::Cull
    }
};

const CB_INSTALL_INFO<1> mmDashView_UpdateCS_CB = {
    &mmDashViewCallbackHandler::UpdateCS, {
        INIT_CB_CALL( NULL, NULL, 0x430F87 ), // replaces call to asLinearCS::Update
    }
};

/*
    Virtual Table hooks
*/

const VT_INSTALL_INFO<2> bridgeFerryDraw_VT = {
    &BridgeFerryCallbackHandler::Draw, {
        { NULL, NULL, 0x5B5FB8 }, // gizBridge::Draw
        { NULL, NULL, 0x5B61AC } // gizFerry::Draw
    }
};

void InstallCallbacks(MM2Version gameVersion) {
    LogFile::WriteLine("Installing callbacks / virtual tables...");

    switch (gameVersion)
    {
        case MM2_BETA_1:
        case MM2_BETA_2:
        {
            // Make sure the betas never expire
            const CB_INSTALL_INFO<1> trialTimeExpired_CB = {
                &ReturnNullOrZero, {
                    INIT_CB_CALL( 0x4011B0, 0x4012AC, NULL ),
                }
            };

            InstallGameCallback("TrialTimeExpired", gameVersion, trialTimeExpired_CB);
        } break;
    }

    InstallGameCallback("ArchInit", gameVersion, archInit_CB);
    InstallGameCallback("ageDebug", gameVersion, ageDebug_CB);

    // not supported for betas yet
    if (gameVersion == MM2_RETAIL) {
        InstallGameCallback("AngelReadString", gameVersion, angelReadString_CB);
    }

    InstallGameCallback("vglBegin", gameVersion, vglBegin_CB);
    InstallGameCallback("vglEnd", gameVersion, vglEnd_CB);

    InstallGameCallback("datTimeManager::Update", gameVersion, tick_CB);

    InstallGameCallback("mmGame::SendChatMessage", gameVersion, sendChatMsg_CB);
    InstallGameCallback("mmGameMusicData::LoadAmbientSFX", gameVersion, loadAmbientSFX_CB);

    InstallGameCallback("vehCarAudioContainer::SetSirenCSVName", gameVersion, sirenCSV_CB);

    // bridge/ferry fix for retail only
    if (gameVersion == MM2_RETAIL) {
        InstallGameCallback("Bridge/Ferry: Cull", gameVersion, bridgeFerryCull_CB);
        InstallVTableHook("Bridge/Ferry: Draw", gameVersion, bridgeFerryDraw_VT);
    }

    // dashboard testing
    InstallGameCallback("mmDashView::Update [EXPERIMENTAL]", gameVersion, mmDashView_UpdateCS_CB);
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

