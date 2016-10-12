#include "main.h"

using namespace MM2;

LPFNDIRECTINPUTCREATE lpDICreate;

// Export as 'DirectInputCreateA/W' so we can hook into MM2
// (Apparently DirectInputCreateW gets called sometimes...)
#pragma comment(linker, "/EXPORT:DirectInputCreateA=_DirectInputCreate_Impl")
#pragma comment(linker, "/EXPORT:DirectInputCreateW=_DirectInputCreate_Impl")
HRESULT NAKED DirectInputCreate_Impl(HINSTANCE hinst, DWORD dwVersion, LPVOID *ppDI, LPUNKNOWN punkOuter) {
    _asm jmp dword ptr ds:lpDICreate
}

CMidtownMadness2 *pMM2;

MM2Version gameVersion;

bool isConsoleOpen = false;

// ==========================
// Game-related properties
// ==========================

/* AGE Debugging */

FILE *ageLogFile;

/* Heap fix */

int g_heapSize = 128;

/* PSDL shading fix */

const double cosNum = 1.570796;

UINT32 vglColor;
UINT32 vglCalculatedColor = 0xFFFFFFFF;

Vector3 vglAmbient;
Vector3 vglKeyColor;
Vector3 vglFill1Color;
Vector3 vglFill2Color;
Vector3 vglShadedColor;

COLOR_ARGB vglResultColor;

/* Dashboard experiment */

static Matrix34 sm_DashOffset;

// ==========================
// Function hooks
// ==========================

MM2FnHook<void> $CreateGameMutex                ( NULL, NULL, 0x402180 );

MM2FnHook<void> $dgBangerInstance_Draw          ( NULL, NULL, 0x4415E0 );

MM2FnHook<UINT32> $sdlPage16_GetShadedColor     ( NULL, NULL, 0x450880 );

MM2FnHook<void> $asLinearCS_Update              ( NULL, NULL, 0x4A3370 );

MM2FnHook<bool> $gfxAutoDetect                  ( NULL, NULL, 0x4ABE00 );

MM2FnHook<void> $setRes                         ( NULL, NULL, 0x4A8CE0 );

/*
    TODO: Move VGL stuff to a separate file?    
*/

MM2FnHook<void> $vglBegin                       ( NULL, NULL, 0x4A5500 );
MM2FnHook<void> $vglEnd                         ( NULL, NULL, 0x4A5A90 );

MM2FnHook<void> $memSafeHeap_Init               ( NULL, NULL, 0x577210 );

MM2FnHook<void> $DefaultPrintString             ( NULL, NULL, 0x4C9510 );
MM2FnHook<void> $DefaultPrinter                 ( NULL, NULL, 0x4C95F0 );

MM2RawFnHook<int (__stdcall *)(HMODULE, UINT, char *, int)>
                $MyLoadStringA                  ( NULL, NULL, 0x5346B0 );

MM2RawFnHook<WNDPROC> $gfxWindowProc            ( NULL, NULL, 0x4A88F0 );

MM2RawFnHook<LPD3DENUMDEVICESCALLBACK7> 
                $DeviceCallback                 ( NULL, NULL, 0x4AC3D0 );
MM2RawFnHook<LPDDENUMMODESCALLBACK2>
                $ResCallback                    ( NULL, NULL, 0x4AC6F0 );

MM2FnHook<void> $asCullManagerInit              ( NULL, NULL, 0x4A1290 );


// ==========================
// Pointer hooks
// ==========================

MM2PtrHook<cityTimeWeatherLighting> 
                TIMEWEATHER                     ( NULL, NULL, 0x6299A8 );

MM2PtrHook<int> timeOfDay                       ( NULL, NULL, 0x62B068 );

MM2PtrHook<char[40]> cityName                   ( NULL, NULL, 0x6B167C );
MM2PtrHook<char[40]> cityName2                  ( NULL, NULL, 0x6B16A4 );

MM2PtrHook<UINT32> vglCurrentColor              ( NULL, NULL, 0x661974 );

MM2PtrHook<asNode> ROOT                         ( NULL, NULL, 0x661738 );

MM2PtrHook<void (*)(LPCSTR)>
                $PrintString                    ( NULL, NULL, 0x5CECF0 );
MM2PtrHook<void (*)(int, LPCSTR, va_list)>
                $Printer                        ( NULL, NULL, 0x5CED24 );
MM2PtrHook<void(*)(void)>
                $FatalErrorHandler              ( NULL, NULL, 0x6A3D38 );

MM2PtrHook<LPDIRECTDRAWCREATEEX>
                $lpDirectDrawCreateEx           ( NULL, NULL, 0x684518 );

MM2PtrHook<IDirectDraw7 *> lpDD                 ( NULL, NULL, 0x6830A8 );
MM2PtrHook<IDirect3D7 *> lpD3D                  ( NULL, NULL, 0x6830AC );

MM2PtrHook<IDirectDrawSurface7 *> lpdsRend      ( NULL, NULL, 0x6830CC );

MM2PtrHook<gfxInterface> gfxInterfaces          ( NULL, NULL, 0x683130 );
MM2PtrHook<uint32_t> gfxInterfaceCount          ( NULL, NULL, 0x6844C0 );

MM2PtrHook<uint32_t> gfxMaxScreenWidth          ( NULL, NULL, 0x6844FC );
MM2PtrHook<uint32_t> gfxMaxScreenHeight         ( NULL, NULL, 0x6844D8 );

MM2PtrHook<HWND> hWndParent                     ( NULL, NULL, 0x682FA0 );
MM2PtrHook<HWND> hWndMain                       ( NULL, NULL, 0x6830B8 );

MM2PtrHook<LPCSTR> lpWindowTitle                ( NULL, NULL, 0x68311C );

MM2PtrHook<ATOM> ATOM_Class                     ( NULL, NULL, 0x6830F0 );
MM2PtrHook<LPCSTR> IconID                       ( NULL, NULL, 0x683108 );

MM2PtrHook<BOOL> inWindow                       ( NULL, NULL, 0x6830D0 );
MM2PtrHook<BOOL> isMaximized                    ( NULL, NULL, 0x6830D1 );
MM2PtrHook<BOOL> hasBorder                      ( NULL, NULL, 0x5CA3ED );

MM2PtrHook<DWORD> WndPosX                       ( NULL, NULL, 0x6830EC );
MM2PtrHook<DWORD> WndPosY                       ( NULL, NULL, 0x683110 );
MM2PtrHook<DWORD> WndWidth                      ( NULL, NULL, 0x683128 );
MM2PtrHook<DWORD> WndHeight                     ( NULL, NULL, 0x683100 );

/*
    !! THESE ARE ABSOLUTELY CRITICAL TO THE HOOK WORKING PROPERLY !!
*/

MM2PtrHook<void (*)(void)> $__VtResumeSampling  ( 0x5C86B8, 0x5DF710, 0x5E0CC4 );
MM2PtrHook<void (*)(void)> $__VtPauseSampling   ( 0x5C86C8, 0x5DF724, 0x5E0CD8 );
MM2PtrHook<BOOL> $gameClosing                   ( 0x667DEC, 0x6B0150, 0x6B1708 );

/*
    ===========================================================================
*/

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

            mmGameManager *mgr = mmGameManager::Instance();
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

// ==========================
// Generic functions
// ==========================

Vector3 addPitch(Vector3 *vec, float pitch) {
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
}

Vector3 intToColor(int value) {
    return {
        (float)((char)((value & 0xFF0000) >> 16) / 256.0),
        (float)((char)((value & 0xFF00) >> 8) / 256.0),
        (float)((char)((value & 0xFF)) / 256.0),
    };
}

// ==========================
// Specialized handlers
// ==========================
class asCullManagerHandler
{
public:
    void Init(int maxCullables, int maxCullables2D) {
        maxCullables = 1024;
        maxCullables2D = 256;

        LogFile::Format("[asCullManager::Init]: Increased Cullables to %d, %d\n", maxCullables, maxCullables2D);

        $asCullManagerInit(this, maxCullables, maxCullables2D);
    }
};

class BridgeFerryHandler
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

class gfxPipelineHandler
{
public:
    static LRESULT APIENTRY gfxWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                if (HandleKeyPress(wParam))
                    return 0;
            } break;

            case WM_ACTIVATEAPP:
            {
                if ((wParam == FALSE) && datArgParser::Get("nopause"))
                    return 0;
            } break;
        }
        return $gfxWindowProc(hWnd, uMsg, wParam, lParam);
    }

    static void SetRes(int width, int height, int cdepth, int zdepth, bool detectArgs)
    {
        LogFile::WriteLine("[gfxPipeline::SetRes]: Additional graphics params enabled.");

        $setRes(width, height, cdepth, zdepth, true);
    }

    static void gfxWindowCreate(LPCSTR lpWindowName)
    {
        if (hWndMain)
        {
            return;
        }

        if (lpWindowTitle)
        {
            lpWindowName = lpWindowTitle;
        }

        *hasBorder = !datArgParser::Get("noborder");

        if (!ATOM_Class)
        {
            WNDCLASSA wc = { NULL };

            wc.style = CS_HREDRAW | CS_VREDRAW;
            wc.lpfnWndProc = gfxPipelineHandler::gfxWindowProc;
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hInstance = 0;
            wc.hIcon = LoadIconA(GetModuleHandleA(NULL), IconID ? IconID : IDI_APPLICATION);
            wc.hCursor = LoadCursorA(0, IDC_ARROW);
            wc.hbrBackground = CreateSolidBrush(NULL);
            wc.lpszMenuName = NULL;
            wc.lpszClassName = "gfxWindow";

            *ATOM_Class = RegisterClassA(&wc);
        }

        HDC hDC = GetDC(0);
        DWORD screenWidth = GetDeviceCaps(hDC, HORZRES);
        DWORD screenHeight = GetDeviceCaps(hDC, VERTRES);
        ReleaseDC(0, hDC);

        if (WndPosX == -1)
        {
            *WndPosX = (screenWidth - WndWidth) / 2;
        }

        if (WndPosY == -1)
        {
            *WndPosY = (screenHeight - WndHeight) / 2;
        }

        DWORD dwStyle = NULL;

        if (inWindow)
        {
            if (hWndParent)
            {
                dwStyle = WS_CHILD;
            }
            else if (hasBorder)
            {
                dwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
            }
            else
            {
                dwStyle = WS_POPUP;
            }
        }
        else
        {
            dwStyle = WS_POPUP | WS_SYSMENU;
        }

        HWND hWND = CreateWindowExA(
            WS_EX_APPWINDOW,
            "gfxWindow",
            lpWindowName,
            dwStyle,
            WndPosX,
            WndPosY,
            640,
            480,
            hWndParent,
            0,
            0,
            0);

        *hWndMain = hWND;

        if (inWindow)
        {
            RECT rect;

            GetClientRect(hWND, &rect);

            MoveWindow(
                hWND,
                WndPosX,
                WndPosY,
                2 * WndWidth - rect.right,
                2 * WndWidth - rect.bottom,
                0);
        }

        SetCursor(NULL);
        ShowCursor(FALSE);

        ShowWindow(hWND, TRUE);
        UpdateWindow(hWND);
        SetFocus(hWND);
    }
};

class memSafeHeapHandler
{
public:
    void Init(void *memAllocator, unsigned int heapSize, bool p3, bool p4, bool checkAlloc) {
        datArgParser::Get("heapsize", 0, &g_heapSize);

        // fast way of expanding to the proper size
        // same as ((g_heapSize * 1024) * 1024)
        heapSize = (g_heapSize << 20);

        LogFile::Format("[memSafeHeap::Init]: Allocating %dMB heap (%d bytes)\n", g_heapSize, heapSize);
        return $memSafeHeap_Init(this, memAllocator, heapSize, p3, p4, checkAlloc);
    };
};

class mmDashViewHandler
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

class mmDirSndHandler
{
public:
    static mmDirSnd* Init(int sampleRate, bool enableStero, int a4, float volume, LPCSTR deviceName, bool enable3D) {
        // TODO: Load the device name from player config?
        if (*deviceName == '\0')
        {
            if (!datArgParser::Get("defaultsounddev", 0, &deviceName))
            {
                deviceName = "Primary Sound Driver";
            }

            LogFile::Format("mmDirSnd::Init - Default Device: %s\n", deviceName);
        }

        // TODO: Set sampling rate (see 0x519640 - int __thiscall AudManager::SetBitDepthAndSampleRate(int this, int bitDepth, int samplingRate))
        // TODO: Redo SetPrimaryBufferFormat to set sampleSize? (see 0x5A5860 -void __thiscall DirSnd::SetPrimaryBufferFormat(mmDirSnd *this, int sampleRate, bool allowStero))
        return mmDirSnd::Init(48000, enableStero, a4, volume, deviceName, enable3D);
    }
};

class vglHandler
{
private:
    static UINT32 CalculateShadedColor(UINT32 color) {
        auto timeWeather = &TIMEWEATHER[timeOfDay];

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

        vglResultColor.r = (byte)(vglShadedColor.X * 255.999);
        vglResultColor.g = (byte)(vglShadedColor.Y * 255.999);
        vglResultColor.b = (byte)(vglShadedColor.Z * 255.999);
        vglResultColor.a = (byte)255;

        return $sdlPage16_GetShadedColor(color, vglResultColor.color);
    }
public:
    static void vglBegin(int gfxMode, int p1) {
        // Save current vgl color
        vglColor = *vglCurrentColor;

        // calculate a nice shaded color ;)
        vglCalculatedColor = CalculateShadedColor(vglColor);
        *vglCurrentColor = vglCalculatedColor;

        $vglBegin(gfxMode, p1);
    }

    static void vglEnd(void) {
        // restore color
        *vglCurrentColor = vglColor;
        $vglEnd();
    }

    static void InstallCallbacks() {
        LogFile::WriteLine(" - Installing shading fix...");

        auto_ptr vglBeginCB = &vglBegin;
        auto_ptr vglEndCB = &vglEnd;

        // use a custom struct to make the process easier
        // this allows us to have an entry representing each "frame" (vglBegin/vglEnd)
        // and cuts down on the amount of time it takes to add a new entry
        struct vgl_cb {
            MM2AddressData begin_addrs; // vglBegin
            MM2AddressData end_addrs; // vglEnd
        };

        std::initializer_list<vgl_cb> vglCBs = {
            {{ NULL, NULL, 0x448424 }, { NULL, NULL, 0x4485D3 }}, 
            {{ NULL, NULL, 0x448697 }, { NULL, NULL, 0x448B82 }}, 
            {{ NULL, NULL, 0x448903 }, { NULL, NULL, 0x448D8C }}, 
            {{ NULL, NULL, 0x448BFD }, { NULL, NULL, 0x448FB7 }}, 
            {{ NULL, NULL, 0x448DE4 }, { NULL, NULL, 0x449219 }}, 
            {{ NULL, NULL, 0x44902A }, { NULL, NULL, 0x449480 }}, 
            {{ NULL, NULL, 0x4492A4 }, { NULL, NULL, 0x44963E }}, 
            {{ NULL, NULL, 0x4494C3 }, { NULL, NULL, 0x44983C }}, 
            {{ NULL, NULL, 0x4496A5 }, { NULL, NULL, 0x4499D4 }}, 
            {{ NULL, NULL, 0x44986B }, { NULL, NULL, 0x449BAA }}, 
            {{ NULL, NULL, 0x449A13 }, { NULL, NULL, 0x449D42 }}, 
            {{ NULL, NULL, 0x449BD9 }, { NULL, NULL, 0x449F5A }}, 
            {{ NULL, NULL, 0x449D82 }, { NULL, NULL, 0x44A146 }}, 
            {{ NULL, NULL, 0x449F67 }, { NULL, NULL, 0x44A3F8 }}, 
            {{ NULL, NULL, 0x44A21C }, { NULL, NULL, 0x44A5BF }}, 
            {{ NULL, NULL, 0x44A444 }, { NULL, NULL, 0x44A7C0 }}, 
            {{ NULL, NULL, 0x44A629 }, { NULL, NULL, 0x44A958 }}, 
            {{ NULL, NULL, 0x44A7EF }, { NULL, NULL, 0x44AB2E }}, 
            {{ NULL, NULL, 0x44A997 }, { NULL, NULL, 0x44ACC6 }}, 
            {{ NULL, NULL, 0x44AB5D }, { NULL, NULL, 0x44AEBC }}, 
            {{ NULL, NULL, 0x44AD06 }, { NULL, NULL, 0x44B083 }}, 
            {{ NULL, NULL, 0x44AECA }, { NULL, NULL, 0x44B23D }}, 
            {{ NULL, NULL, 0x44B0EC }, { NULL, NULL, 0x44B394 }}, 
            {{ NULL, NULL, 0x44B24B }, { NULL, NULL, 0x44B531 }}, 
            {{ NULL, NULL, 0x44B3B6 }, { NULL, NULL, 0x44B6E1 }}, 
            {{ NULL, NULL, 0x44B557 }, { NULL, NULL, 0x44B895 }}, 
            {{ NULL, NULL, 0x44B6F3 }, { NULL, NULL, 0x44BA7C }}, 
            {{ NULL, NULL, 0x44B8F1 }, { NULL, NULL, 0x44BC03 }}, 
            {{ NULL, NULL, 0x44BA8A }, { NULL, NULL, 0x44BE8E }}, 
            {{ NULL, NULL, 0x44BC29 }, { NULL, NULL, 0x44C118 }}, 
            {{ NULL, NULL, 0x44BE9C }, { NULL, NULL, 0x44C3EA }}, 
            {{ NULL, NULL, 0x44C136 }, { NULL, NULL, 0x44C638 }}, 
            {{ NULL, NULL, 0x44C40C }, { NULL, NULL, 0x44C77A }}, 
            {{ NULL, NULL, 0x44C64A }, { NULL, NULL, 0x44C989 }}, 
            {{ NULL, NULL, 0x44C7C0 }, { NULL, NULL, 0x44CC44 }}, 
            {{ NULL, NULL, 0x44CAD6 }, { NULL, NULL, 0x44CE63 }}, 
            {{ NULL, NULL, 0x44CCF5 }, { NULL, NULL, 0x44D04E }}, 
            {{ NULL, NULL, 0x44CF6D }, { NULL, NULL, 0x44D403 }}, 
            {{ NULL, NULL, 0x44D0D4 }, { NULL, NULL, 0x44D780 }}, 
            {{ NULL, NULL, 0x44D5F7 }, { NULL, NULL, 0x44D8E9 }}, 
            {{ NULL, NULL, 0x44D789 }, { NULL, NULL, 0x44E014 }}, 
            {{ NULL, NULL, 0x44DC55 }, { NULL, NULL, 0x44E131 }}, 
            {{ NULL, NULL, 0x44E050 }, { NULL, NULL, 0x44E22C }}, 
            {{ NULL, NULL, 0x44E14B }, { NULL, NULL, 0x44E661 }}, 
            {{ NULL, NULL, 0x44E2A3 }, { NULL, NULL, 0x44E785 }}, 
            {{ NULL, NULL, 0x44E69D }, { NULL, NULL, 0x44E886 }}, 
            {{ NULL, NULL, 0x44E79E }, { NULL, NULL, 0x44EB82 }}, 
            {{ NULL, NULL, 0x44EAA0 }, { NULL, NULL, 0x44EDC3 }}, 
            {{ NULL, NULL, 0x44EBA5 }, { NULL, NULL, 0x44F0B9 }}, 
            {{ NULL, NULL, 0x44EFD0 }, { NULL, NULL, 0x44F316 }}, 
            {{ NULL, NULL, 0x44F0DC }, { NULL, NULL, 0x44F64C }}, 
            {{ NULL, NULL, 0x44F588 }, { NULL, NULL, 0x44FB9D }}, 
            {{ NULL, NULL, 0x44F7E2 }, { NULL, NULL, 0x44FD30 }}, 
            {{ NULL, NULL, 0x44FC1E }, { NULL, NULL, 0x44FE4E }}, 
            {{ NULL, NULL, 0x44FDD4 }, { NULL, NULL, 0x44FFB3 }}, 
            {{ NULL, NULL, 0x44FF10 }, { NULL, NULL, 0x450162 }}, 
            {{ NULL, NULL, 0x450085 }, { NULL, NULL, 0x450390 }}, 
            {{ NULL, NULL, 0x450269 }, { NULL, NULL, 0x45078C }}, 
            // --------------------------------------------------
            {{ NULL, NULL, 0x443B9D }, { NULL, NULL, 0x443DCC }}, // dgRoadDecalInstance
            {{ NULL, NULL, 0x57AC4A }, { NULL, NULL, 0x57AD41 }}, // ped LODs
        };

        // mostly copied from InstallGameCallback
        std::size_t count = 0;

        for (auto cb : vglCBs)
        {
            auto begin = cb.begin_addrs[gameVersion];
            auto end = cb.end_addrs[gameVersion];

            LogFile::Format("   - { vglBegin: %08X => %08X, vglEnd: %08X => %08X } : ", begin, vglBeginCB, end, vglEndCB);

            if ((begin != NULL) && (end != NULL))
            {
                if (InstallCallbackHook(begin, vglBeginCB, true) && InstallCallbackHook(end, vglEndCB, true))
                {
                    LogFile::WriteLine("OK");
                    ++count;
                } else {
                    // let's hope this never happens...
                    LogFile::WriteLine("FAIL!");
                }

                continue;
            } else {
                LogFile::WriteLine("Not Supported");
            }
        }

        LogFile::Format("   - Installed %d / %d callbacks\n", count, vglCBs.size());
    }
};

// ==========================
// Callback handlers
// ==========================

class CallbackHandler {
public:
    static void ProgressRect(int x, int y, int width, int height, UINT color) {
        DDPIXELFORMAT ddPixelFormat = { NULL };
        DDBLTFX ddBltFx = { NULL };

        ddBltFx.dwSize = 0x64;
        ddPixelFormat.dwSize = 0x20;

        lpdsRend->GetPixelFormat(&ddPixelFormat);

        ddBltFx.dwFillColor = GetPixelFormatColor(&ddPixelFormat, color);

        RECT position = {
            x,
            y,
            x + width,
            y + height,
        };

        lpdsRend->Blt(&position, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddBltFx);
    };

    static void CreateGameMutex(LPCSTR lpName) {
        if (datArgParser::Get("nomutex")) {
            LogFile::WriteLine("Game mutex disabled.");
            return;
        }

        $CreateGameMutex(lpName);
    };

    bool LoadAmbientSFX(LPCSTR name) {
        LPCSTR szAmbientSFX = NULL;

        LPCSTR city = *cityName;

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
        return reinterpret_cast<mmGameMusicData *>(this)->LoadAmbientSFX(szAmbientSFX);
    };

    static void SetSirenCSVName(LPCSTR name) {
        char siren_name[80] = { NULL };

        sprintf(siren_name, "%spolicesiren", *cityName);

        bool useDefault = !(datAssetManager::Exists("aud\\cardata\\player", siren_name, "csv"));

        LPCSTR szSirenName = (useDefault) ? name : siren_name;

        LogFile::Format("SirenCSVName: %s\n", szSirenName);

        // pass to MM2
        vehCarAudioContainer::SetSirenCSVName(szSirenName);
    };

    static void ageDebug(int enabled, LPCSTR format, ...) {
        // this makes the game load up reeeeeally slow if enabled!
        if (ageLogFile)
        {
            char buffer[1024];

            va_list va;
            va_start(va, format);
            vsprintf(buffer, format, va);
            va_end(va);

            fputs(buffer, ageLogFile);
            fputs("\n", ageLogFile);
        }
    };

    static LPCSTR AngelReadString(UINT stringId) {
        static const LPCSTR STRING_UNKNOWN = "?? lang:%d ??";

        static HMODULE h_MMLANG = NULL;

        static LocString string_buffer[8];
        static int string_index = 0;

        LPCSTR str = NULL;

        L.getGlobal("GetLocaleString");
        L.push(stringId);

        if ((L.pcall(1, 1, 0) == LUA_OK) && !L.isNil(-1))
            str = L.toString(-1);

        // not found in Lua, let's look in MMLANG.DLL
        if (str == NULL)
        {
            auto locStr = (char *)&string_buffer[(string_index++ & 0x7)];

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

            str = locStr;
        }

        L.pop(1);
        return str;
    }

    static BOOL __stdcall AutoDetectCallback(GUID *lpGUID, 
                                             LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext)
    {
        Displayf("AutoDetect: GUID=%x, Description=%s, Name=%s", lpGUID, lpDriverDescription, lpDriverName);

        if ($lpDirectDrawCreateEx(lpGUID, (LPVOID*)&lpDD, IID_IDirectDraw7, nullptr) == DD_OK)
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

                *lpD3D = nullptr;
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

            *gfxMaxScreenWidth = 0;
            *gfxMaxScreenHeight = 0;

            lpDD->EnumDisplayModes(0, 0, gfxInterface, $ResCallback);
            lpDD->Release();

            *lpDD = nullptr;

            ++*gfxInterfaceCount;
        }

        return TRUE;
    }
};

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

class PrintHandler {
public:
    static void PrintString(LPCSTR message) {
        // TODO: redirect to a log file?
        $DefaultPrintString(message);
    };

    static void Print(int level, LPCSTR message, va_list va_args) {
        static short printer_types[] = {
            FOREGROUND_INTENSITY, // print
            FOREGROUND_INTENSITY, // message
            FOREGROUND_INTENSITY, // display
            FOREGROUND_YELLOW | FOREGROUND_INTENSITY, // warning
            FOREGROUND_RED | FOREGROUND_INTENSITY, // error
            FOREGROUND_RED | FOREGROUND_INTENSITY, // quit/abort
        };

        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        SetConsoleTextAttribute(hConsole, printer_types[level]);
        $DefaultPrinter(level, message, va_args);
        SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
    };

    static void FatalError() {
        // do something?
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

class HookSystemHandler
{
private:
    static void InstallCallbacks() {
        LogFile::WriteLine("Installing callbacks / virtual tables...");

        switch (gameVersion)
        {
            case MM2_BETA_1:
            case MM2_BETA_2:
            {
                // Disables time check on betas
                InstallGameCallback("TrialTimeExpired", &ReturnNullOrZero, {
                    CB_HOOK<CALL>({ 0x4011B0, 0x4012AC, NULL }),
                });
            } break;
            case MM2_RETAIL:
            {
                // mutex was introduced in retail
                InstallGameCallback("CreateGameMutex", &CallbackHandler::CreateGameMutex, {
                    CB_HOOK<CALL>({ NULL, NULL, 0x40128D }),
                });
            
                // revert bridges/ferries to how they were in the betas
                InstallGameCallback("Bridge/Ferry: Cull", &BridgeFerryHandler::Cull, {
                    CB_HOOK<CALL>({ NULL, NULL, 0x5780BC }), // gizBridgeMgr::Cull
                    CB_HOOK<CALL>({ NULL, NULL, 0x5798F0 }), // gizFerryMgr::Cull
                });

                InstallVTableHook("Bridge/Ferry: Draw", &BridgeFerryHandler::Draw, {
                    { NULL, NULL, 0x5B5FB8 }, // gizBridge::Draw
                    { NULL, NULL, 0x5B61AC }, // gizFerry::Draw
                });
            } break;
        }

        InstallGameCallback("ageDebug", &CallbackHandler::ageDebug, {
            CB_HOOK<JMP>({ NULL, NULL, 0x402630 }),
        });

        InstallGameCallback("ProgressRect [white loading bar fix]", &CallbackHandler::ProgressRect, {
            CB_HOOK<CALL>({ NULL, NULL, 0x401163 }),
            CB_HOOK<CALL>({ NULL, NULL, 0x4011CC }),
        });

        if (!datArgParser::Get("oldautodetect"))
        {
            // Hook into the original AutoDetect and replace it with our own version
            InstallGameCallback("AutoDetectCallback", &CallbackHandler::AutoDetectCallback, {
                CB_HOOK<JMP>({ NULL, NULL, 0x4AC030 }),
            });
        }

        InstallGameCallback("gfxPipeline::SetRes", &gfxPipelineHandler::SetRes, {
            CB_HOOK<CALL>({ NULL, NULL, 0x401482 }),
        });

        InstallGameCallback("gfxPipeline::gfxWindowCreate", &gfxPipelineHandler::gfxWindowCreate, {
            CB_HOOK<CALL>({ NULL, NULL, 0x4A94AA }),
        });

        InstallGameCallback("gfxLoadVideoDatabase [disable 'badvideo.txt']", &ReturnFalse, {
            CB_HOOK<CALL>({ NULL, NULL, 0x4AC4F9 }),
        });

        InstallGameCallback("mmDirSnd::Init", &mmDirSndHandler::Init, {
            CB_HOOK<CALL>({ NULL, NULL, 0x51941D }),
        });
    
        InstallGameCallback("memSafeHeap::Init [Heap fix]", &memSafeHeapHandler::Init, {
            CB_HOOK<CALL>({ NULL, NULL, 0x4015DD }),
        });

        InstallGameCallback("asCullManager::Init [Increase Max Cullables]", &asCullManagerHandler::Init, {
            CB_HOOK<CALL>({ NULL, NULL, 0x401D5C }),
        });

        // NOTE: Completely overrides the original AngelReadString (will check Lua first then DLL)
        InstallGameCallback("AngelReadString", &CallbackHandler::AngelReadString, {
            CB_HOOK<JMP>({ NULL, NULL, 0x534790 }),
        });

        InstallGameCallback("datTimeManager::Update", &TickHandler::Update, {
            CB_HOOK<CALL>({ NULL, NULL, 0x401A2F }),
        });

        InstallGameCallback("mmGame::SendChatMessage", &ChatHandler::Process, {
            CB_HOOK<JMP>({ NULL, NULL, 0x414EB6 }),
        });

        InstallGameCallback("mmGameMusicData::LoadAmbientSFX", &CallbackHandler::LoadAmbientSFX, {
            CB_HOOK<CALL>({ NULL, NULL, 0x433F93 }),
        });

        InstallGameCallback("vehCarAudioContainer::SetSirenCSVName", &CallbackHandler::SetSirenCSVName, {
            CB_HOOK<CALL>({ NULL, NULL, 0x412783 }),
            CB_HOOK<CALL>({ NULL, NULL, 0x412772 }),
        });

        // dashboard testing
        InstallGameCallback("mmDashView::Update [EXPERIMENTAL]", &mmDashViewHandler::UpdateCS, {
            CB_HOOK<CALL>({ NULL, NULL, 0x430F87 }), // replaces call to asLinearCS::Update
        });

        InstallGameCallback("zipFile::Init ['extraLen' spam fix]", &NullSub, {
            CB_HOOK<CALL>({ NULL, NULL, 0x5738EA }), // 'extraLen=%d'
        });

        // install shading fix (for PSDL, etc.)
        vglHandler::InstallCallbacks();
    }

    static void InstallPatches() {
        LogFile::WriteLine("Installing patches...");

        InstallGamePatch("Increase chat buffer size", { 60 }, {
            { NULL, NULL, 0x4E68B5 },
            { NULL, NULL, 0x4E68B9 },
            { NULL, NULL, 0x50BBCF },
        });

        InstallGamePatch("Increase cop limit", { 0x40 }, {
            { NULL, NULL, 0x55100B },
        });

        InstallGamePatch("Enables pointer in windowed mode", { 0x90, 0x90 }, {
            { NULL, NULL, 0x4F136E },
        });
    }

    static void InitializeLua() {
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
    static void Initialize(int argc, char **argv) {
        // Install callbacks/patches
        InstallCallbacks();
        InstallPatches();
        
        // Initialize the Lua engine
        InitializeLua();

        if (gameVersion == MM2_RETAIL)
        {
            // hook into the printer
            *$Printer           = &PrintHandler::Print;
            *$PrintString       = &PrintHandler::PrintString;
            *$FatalErrorHandler = &PrintHandler::FatalError;

            /* Won't write to the log file for some reason :(
            LogFile::Write("Redirecting MM2 output...");

            if (datOutput::OpenLog("mm2.log"))
            LogFile::Write("Done!\n");
            else
            LogFile::Write("FAIL!\n");
            */

            if (ageLogFile == NULL && datArgParser::Get("age_debug"))
            {
                ageLogFile = fopen("AGE.log", "w+");
            }
        }
    }

    static void Reset(bool restarting) {
        LogFile::Write("Hook reset request received: ");
        LogFile::WriteLine((restarting) ? "leaving GameLoop" : "entering GameLoop");

        if (restarting)
            MM2Lua::OnRestart();

        MM2Lua::Reset();
    }

    static void Start() {
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

    static void Stop() {
        if ($gameClosing)
        {
            LogFile::WriteLine("Hook shutdown request received.");

            LogFile::Close();
            L.close(); // release Lua

            if (ageLogFile)
            {
                fclose(ageLogFile);
            }
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
void InstallFramework() {
    LogFile::WriteLine("Installing framework...");

    *$__VtResumeSampling = &HookSystemHandler::Start;
    *$__VtPauseSampling = &HookSystemHandler::Stop;

    /*
        We'll hook into ArchInit (an empty function), 
        and use it to install our callbacks/patches.

        However, this time around, we can now use datArgParser
        to determine if a patch/callback should be installed or not,
        whereas before we needed to check after it was already hooked in.

        Basically, this method is a lot safer, and guarantees
        we'll have access to any arguments passed.

    */

    InstallGameCallback("ArchInit [Framework initialization]", &HookSystemHandler::Initialize, {
        CB_HOOK<CALL>({ NULL, NULL, 0x4023DB }),
    });

    /*
        IMPORTANT:
          Add any patches/callbacks here that must be initialized prior to the game's entry point.
          This should be used for very very advanced callbacks/patches only!
    */
};

//
// Initialize all the important stuff prior to MM2 starting up
// NOTE: We do not have access to datArgParser yet.
//
void Initialize(ageInfoLookup &gameInfo) {
    // initialize game manager
    pMM2 = new CMidtownMadness2(gameInfo.info);
    pMM2->Initialize();

    gameVersion = pMM2->GetVersion();

    if ((gameVersion != MM2_INVALID) && (gameVersion < MM2_NUM_VERSIONS)) {
        InstallFramework();
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

