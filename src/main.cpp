#include "main.h"

using namespace MM2;

LPFNDIRECTINPUTCREATE lpDICreate;

// Export as 'DirectInputCreateA/W' so we can hook into MM2
// (Apparently DirectInputCreateW gets called sometimes...)
#pragma comment(linker, "/EXPORT:DirectInputCreateA=_DirectInputCreate_Impl")
#pragma comment(linker, "/EXPORT:DirectInputCreateW=_DirectInputCreate_Impl")
HRESULT NAKED DirectInputCreate_Impl(HINSTANCE hinst, DWORD dwVersion, LPVOID *ppDI, LPUNKNOWN punkOuter) {
    JMP_PTR(lpDICreate);
}

CMidtownMadness2 *pMM2;

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

unsigned int vglColor;
unsigned int vglCalculatedColor = 0xFFFFFFFF;

Vector3 vglAmbient;
Vector3 vglKeyColor;
Vector3 vglFill1Color;
Vector3 vglFill2Color;
Vector3 vglShadedColor;

ColorARGB vglResultColor;

/* Dashboard experiment */

static Matrix34 sm_DashOffset;

// ==========================
// Function hooks
// ==========================

AGEHook<0x402180>::Func<void> $CreateGameMutex;

AGEHook<0x4C9510>::Func<void> $DefaultPrintString;
AGEHook<0x4C95F0>::Func<void> $DefaultPrinter;

AGEHook<0x4A3370>::Func<void> $asLinearCS_Update;

AGEHook<0x4415E0>::Func<void> $dgBangerInstance_Draw;

AGEHook<0x4ABE00>::Func<bool> $gfxAutoDetect;
AGEHook<0x4A8CE0>::Func<void> $gfxPipeline_SetRes;

AGEHook<0x577210>::Func<void> $memSafeHeap_Init;

AGEHook<0x5346B0>::Func<int>::StdCall $MyLoadStringA;

AGEHook<0x448330>::Func<void> $sdlPage16_Draw;
AGEHook<0x450880>::Func<unsigned int> $sdlPage16_GetShadedColor;

/*
    TODO: Move VGL stuff to a separate file?
*/

AGEHook<0x4A5500>::Func<void> $vglBegin;
AGEHook<0x4A5A90>::Func<void> $vglEnd;

AGEHook<0x4A88F0>::Func<WNDPROC> $gfxPipeline_gfxWindowProc;

AGEHook<0x4AC3D0>::Func<LPD3DENUMDEVICESCALLBACK7> $DeviceCallback;
AGEHook<0x4AC6F0>::Func<LPDDENUMMODESCALLBACK2> $ResCallback;

AGEHook<0x4A1290>::Func<void> $asCullManagerInit;
AGEHook<0x443E50>::Func<void> $cityLevelSetObjectDetail;

// ==========================
// Pointer hooks
// ==========================

AGEHook<0x6A3D40>::Type<Stream *> datOutputStream;

AGEHook<0x5C571C>::Type<float> obj_NoDrawThresh; // default: 300.0

AGEHook<0x5C6658>::Type<float> obj_VLowThresh;  // default: 200.0
AGEHook<0x5C665C>::Type<float> obj_LowThresh;   // default: 100.0
AGEHook<0x5C6660>::Type<float> obj_MedThresh;   // default: 40.0

AGEHook<0x5C5708>::Type<float> sdl_VLowThresh;  // default: 300.0
AGEHook<0x5C570C>::Type<float> sdl_LowThresh;   // default: 100.0
AGEHook<0x5C5710>::Type<float> sdl_MedThresh;   // default: 50.0

AGEHook<0x6299A8>::Type<cityTimeWeatherLighting[16]> timeWeathers;

AGEHook<0x62B068>::Type<int> timeOfDay;

AGEHook<0x6B167C>::Type<char[40]> cityName;
AGEHook<0x6B16A4>::Type<char[40]> cityName2;

AGEHook<0x661974>::Type<unsigned int> vglCurrentColor;

AGEHook<0x661738>::Type<asNode> ROOT;

AGEHook<0x5CECF0>::Type<void (*)(LPCSTR)> $PrintString;
AGEHook<0x5CED24>::Type<void (*)(int, LPCSTR, va_list)> $Printer;
AGEHook<0x6A3D38>::Type<void (*)(void)> $FatalErrorHandler;

AGEHook<0x684518>::Type<LPDIRECTDRAWCREATEEX> $lpDirectDrawCreateEx;

AGEHook<0x6830A8>::Type<IDirectDraw7 *> lpDD;
AGEHook<0x6830AC>::Type<IDirect3D7 *> lpD3D;
AGEHook<0x6830CC>::Type<IDirectDrawSurface7 *> lpdsRend;

AGEHook<0x683130>::Type<gfxInterface> gfxInterfaces;
AGEHook<0x6844C0>::Type<unsigned int> gfxInterfaceCount;

AGEHook<0x6844FC>::Type<unsigned int> gfxMaxScreenWidth;
AGEHook<0x6844D8>::Type<unsigned int> gfxMaxScreenHeight;

AGEHook<0x682FA0>::Type<HWND> hWndParent;
AGEHook<0x6830B8>::Type<HWND> hWndMain;

AGEHook<0x68311C>::Type<LPCSTR> lpWindowTitle;

AGEHook<0x6830F0>::Type<ATOM> ATOM_Class;
AGEHook<0x683108>::Type<LPCSTR> IconID;

AGEHook<0x6830D0>::Type<BOOL> inWindow;
AGEHook<0x6830D1>::Type<BOOL> isMaximized;
AGEHook<0x5CA3ED>::Type<BOOL> hasBorder;

AGEHook<0x6830EC>::Type<unsigned int> windowX;
AGEHook<0x683110>::Type<unsigned int> windowY;
AGEHook<0x683128>::Type<unsigned int> windowWidth;
AGEHook<0x683100>::Type<unsigned int> windowHeight;

AGEHook<0x5E0CC4>::Type<void (*)(void)> $__VtResumeSampling;
AGEHook<0x5E0CD8>::Type<void (*)(void)> $__VtPauseSampling;

AGEHook<0x6B1708>::Type<BOOL> $gameClosing;

/*
    ===========================================================================
*/

bool HandleKeyPress(DWORD vKey)
{
    // Inform Lua of any changes beforehand
    MM2Lua::OnKeyPress(vKey);

    switch (vKey) {
        // '~'
        case VK_OEM_2: case VK_OEM_3:
        // '`'
        case VK_OEM_8:
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
class asCullManagerHandler {
public:
    void Init(int maxCullables, int maxCullables2D) {
        maxCullables = 1024;
        maxCullables2D = 256;

        LogFile::Format("[asCullManager::Init]: Increased Max Cullables to %d, %d\n", maxCullables, maxCullables2D);

        $asCullManagerInit(this, maxCullables, maxCullables2D);
    }
};

class cityLevelHandler {
public:
    // TODO: Factor in 'Visibility' level somehow?
    void SetObjectDetail(int lod) {
        // Default MM2 values (leaving this here for reference)
        // static float lodLevels[4][4] = {
        //     { 200.0f, 150.0f,  70.0f, 20.0f }, // Low
        //     { 250.0f, 175.0f,  90.0f, 30.0f }, // Medium
        //     { 300.0f, 200.0f, 100.0f, 40.0f }, // High
        //     { 300.0f, 200.0f, 130.0f, 70.0f }, // Very high
        // };

        static const char *lodLevelNames[4] = {
            "Low",
            "Medium",
            "High",
            "Very High",
        };

        // 'Low' now uses 'Medium' values, 'Medium' uses 'High', etc.
        // 'Very high' now renders a bit farther than before, but not to an extreme.
        // Performance drops are to be expected until bugs can be ironed out.
        // Poor PVS optimization seems to be the reason why values were so low.
        static float lodLevels[4][4] = {
            { 250.0f, 175.0f,  90.0f, 30.0f }, // Low       (Default: Medium)
            { 300.0f, 200.0f, 100.0f, 40.0f }, // Medium    (Default: High)
            { 300.0f, 200.0f, 130.0f, 70.0f }, // High      (Default: Very High)
            { 640.0f, 480.0f, 160.0f, 80.0f }, // Very high (NEW)
        };

        // Using temporary variables so we don't need to constantly access pointers

        float objNoDrawThresh = lodLevels[lod][0]; // VL: <VLowThresh> - <NoDrawThresh>
        float objVLowThresh   = lodLevels[lod][1]; // L: <LowThresh> - <VLowThresh>
        float objLowThresh    = lodLevels[lod][2]; // M: <MedThresh> - <LowThresh>
        float objMedThresh    = lodLevels[lod][3]; // H: 0.0 - <MedThresh>

        *obj_NoDrawThresh = objNoDrawThresh;
        *obj_VLowThresh   = objVLowThresh;
        *obj_LowThresh    = objLowThresh;
        *obj_MedThresh    = objMedThresh;

        // By default, the game doesn't set these values based on the detail level
        // They are hardcoded to what is considered 'High' by default,
        // however this is now known as 'Medium' (lod = 1; see above)
        //
        // 'Medium' and below (default 'High') uses the defaults.
        float sdlVLowThresh = (lod > 1) ? (objVLowThresh + 100.0f) : 300.0f;
        float sdlLowThresh  = (lod > 1) ? (objLowThresh + 25.0f) : 100.0f;
        float sdlMedThresh  = (lod > 1) ? (objMedThresh + 10.0f) : 50.0f;

        *sdl_VLowThresh = sdlVLowThresh;
        *sdl_LowThresh  = sdlLowThresh;
        *sdl_MedThresh  = sdlMedThresh;

        LogFile::Format("[cityLevel::SetObjectDetail]: '%s'\n"
                        "    - OBJ { %.4f, %.4f, %.4f, %.4f }\n"
                        "    - SDL { %.4f, %.4f, %.4f }\n",
                        lodLevelNames[lod],
                        objNoDrawThresh, objVLowThresh, objLowThresh, objMedThresh,
                        sdlVLowThresh, sdlLowThresh, sdlMedThresh);
    }
};

/*
    By default, bridges are treated as "Cullables" instead of "Drawables" (MM2 is weird)

    Before the patch, Cull calls dgBangerInstance::Draw, and Draw does nothing.
    With the patch, these 2 functions are swapped around, so Draw calls dgBangerInstance::Draw, and Cull does nothing.

    Problem solved, right? Nope. Cull is called by gizBridgeMgr::Cull, where as Draw is called by cityLevel::DrawRooms.

    gizBridgeMgr has it's own maximum draw distance for bridges, so gizBridgeMgr::Cull draws them fine at range.
    But cityLevel::DrawRooms uses the prop lod distances, so the bridges end up disappearing a lot sooner.

    A "solution" is to manually set the LOD distance values (see cityLevel::SetObjectDetail).
    But that would cause everything to be drawn further, and decrase FPS.
    It also seems to create rendering artifacts when set too high.
*/
class BridgeFerryHandler {
public:
    void Cull(int lod) {
        // wtf
        //setPtr(this, 0x1B, (char)++lod);
    }

    void Draw(int lod) {
        $dgBangerInstance_Draw(this, lod);
    }
};

class gfxPipelineHandler {
public:
    static LRESULT APIENTRY gfxWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
        return $gfxPipeline_gfxWindowProc(hWnd, uMsg, wParam, lParam);
    }

    static void SetRes(int width, int height, int cdepth, int zdepth, bool detectArgs) {
        LogFile::WriteLine("[gfxPipeline::SetRes]: Additional graphics params enabled.");

        $gfxPipeline_SetRes(width, height, cdepth, zdepth, true);
    }

    static void gfxWindowCreate(LPCSTR lpWindowName) {
        if (hWndMain)
            return;

        if (lpWindowTitle)
            lpWindowName = lpWindowTitle;

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
        int screenWidth  = GetDeviceCaps(hDC, HORZRES);
        int screenHeight = GetDeviceCaps(hDC, VERTRES);
        ReleaseDC(0, hDC);

        if (windowX == -1)
            *windowX = (screenWidth - windowWidth) / 2;

        if (windowY == -1)
            *windowY = (screenHeight - windowHeight) / 2;

        DWORD dwStyle = NULL;

        if (inWindow)
        {
            if (hWndParent)
            {
                dwStyle = WS_CHILD;
            }
            else if (*hasBorder = !datArgParser::Get("noborder"))
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
            windowX,
            windowY,
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
                windowX,
                windowY,
                2 * windowWidth - rect.right,
                2 * windowHeight - rect.bottom,
                0);
        }

        SetCursor(NULL);
        ShowCursor(FALSE);

        ShowWindow(hWND, TRUE);
        UpdateWindow(hWND);
        SetFocus(hWND);
    }
};

class memSafeHeapHandler {
public:
    void Init(void *memAllocator, unsigned int heapSize, bool p3, bool p4, bool checkAlloc) {
        datArgParser::Get("heapsize", 0, &g_heapSize);

        // fast way of expanding to the proper size
        // same as ((g_heapSize * 1024) * 1024)
        heapSize = (g_heapSize << 20);

        LogFile::Format("[memSafeHeap::Init]: Allocating %dMB heap (%d bytes)\n", g_heapSize, heapSize);
        return $memSafeHeap_Init(this, memAllocator, heapSize, p3, p4, checkAlloc);
    }
};

class mmDashViewHandler {
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
    }
};

class mmDirSndHandler {
public:
    static mmDirSnd* Init(int sampleRate, bool enableStero, int a4, float volume, LPCSTR deviceName, bool enable3D) {
        if (*deviceName == '\0') {
            deviceName = "Primary Sound Driver";

            LogFile::WriteLine("[mmDirSnd::Init]: Using primary sound driver");
        }

        // TODO: Set sampling rate (see 0x519640 - int __thiscall AudManager::SetBitDepthAndSampleRate(int this, int bitDepth, int samplingRate))
        // TODO: Redo SetPrimaryBufferFormat to set sampleSize? (see 0x5A5860 -void __thiscall DirSnd::SetPrimaryBufferFormat(mmDirSnd *this, int sampleRate, bool allowStero))
        return mmDirSnd::Init(48000, enableStero, a4, volume, deviceName, enable3D);
    }
};

class mmGameMusicDataHandler {
public:
    bool LoadAmbientSFX(LPCSTR name) {
        char buffer[80];
        sprintf(buffer, "%sambience", *cityName);

        bool exists = datAssetManager::Exists("aud\\dmusic\\csv_files", buffer, "csv");
        LPCSTR szAmbientSFX = exists ? buffer : "sfambience";

        LogFile::Format("AmbientSFX: %s\n", szAmbientSFX);

        return reinterpret_cast<mmGameMusicData *>(this)->LoadAmbientSFX(szAmbientSFX);
    }
};

class vehCarAudioContainerHandler {
public:
    static void SetSirenCSVName(LPCSTR name) {
        char buffer[80];
        sprintf(buffer, "%spolicesiren", *cityName);

        bool exists = datAssetManager::Exists("aud\\cardata\\player", buffer, "csv");
        LPCSTR szSirenName = exists ? buffer : "sfpolicesiren";

        LogFile::Format("SirenCSVName: %s\n", szSirenName);

        vehCarAudioContainer::SetSirenCSVName(szSirenName);
    }
};

// HACK HACK HACK!
// Doesn't work well for subway stations :(
bool insideTunnel = false;

class sdlPage16Handler {
public:
    static LPVOID blockPtr; // current block pointer
    static LPVOID attributePtr; // current attribute pointer
public:
    // this MUST clean up the stack, hence the stdcall
    static void __stdcall SetBlockPointer(LPVOID lpBlock) {
        attributePtr = lpBlock;
    };

    void Draw(int p1, unsigned int p2) {
        blockPtr = this;
        $sdlPage16_Draw(this, p1, p2);

        // not in a block anymore
        blockPtr = NULL;
        attributePtr = NULL; // lets vglHandler know we're not checking for SDL stuff

        // so hacky
        insideTunnel = false;
    };
};

LPVOID sdlPage16Handler::blockPtr;
LPVOID sdlPage16Handler::attributePtr;

bool bVGLShadingDebug = false;

class vglHandler {
private:
    static unsigned int CalculateShadedColor(gfxDrawMode drawMode, unsigned int color) {
        if (sdlPage16Handler::blockPtr != NULL)
        {
            static ColorARGB sdlTunnelColors[] = {
                // debug colors
                { 0, 0, 255, 255 },
                { 0, 255, 0, 255 },
                { 255, 0, 0, 255 },

                // fullbright
                { 255, 255, 255, 255 },
            };

            // we can finally check for attributes...
            short *attr = (short*)sdlPage16Handler::attributePtr;

            int type = ((*attr >> 3) & 0xF);
            int subtype = *attr & 0x7;

            if (type == 9) {
                // junction ceiling (?)
                // may need verification
                if (subtype == 0){
                    int flags = attr[2];
                    int unk = attr[5];

                    if (!(flags & 0x4)) {
                        if (!(flags & 0x4000)) {
                            // not culled
                            insideTunnel = true;
                            return sdlTunnelColors[3].color;
                        } else {
                            // culled
                            insideTunnel = false;
                        }
                    }
                }
                if (subtype == 3) {
                    int flags = attr[1];

                    if ((flags > 10) && !(flags & 0x4)) {
                        if (!(flags & 0x4000)) {
                            // not culled
                            insideTunnel = true;
                            return sdlTunnelColors[3].color;
                        }
                    } else {
                        insideTunnel = false;
                    }
                }
            } else {
                // lol hack
                if (insideTunnel) {
                    return sdlTunnelColors[3].color;
                }
            }
        }

        if (bVGLShadingDebug) {
            // visualize what's being drawn
            static ColorARGB drawModeColors[] = {
                { 255, 0, 0, 255 }, // POINTLIST (RED)
                { 0, 255, 0, 255 }, // LINELIST (GREEN)
                { 0, 0, 255, 255 }, // LINESTRIP (BLUE)

                { 255, 255, 0, 255 }, // TRIANGLELIST (YELLOW)
                { 255, 0, 255, 255 }, // TRIANGLESTRIP (PINK)
                { 0, 255, 255, 255 }, // TRIANGLEFAN (TEAL)
            };

            return drawModeColors[drawMode - 1].color;
        } else {
            auto timeWeather = *timeWeathers + timeOfDay;

            vglKeyColor = addPitch(&timeWeather->KeyColor, timeWeather->KeyPitch);
            vglFill1Color = addPitch(&timeWeather->Fill1Color, timeWeather->Fill1Pitch);
            vglFill2Color = addPitch(&timeWeather->Fill2Color, timeWeather->Fill2Pitch);

            // convert the ambient to a vector3 for better accuracy
            vglAmbient = intToColor(timeWeather->Ambient);

            // compute le values
            vglShadedColor = {
                normalize(vglKeyColor.X + vglFill1Color.X + vglFill2Color.X + vglAmbient.X),
                normalize(vglKeyColor.Y + vglFill1Color.Y + vglFill2Color.Y + vglAmbient.Y),
                normalize(vglKeyColor.Z + vglFill1Color.Z + vglFill2Color.Z + vglAmbient.Z),
            };

            vglResultColor.r = byte(vglShadedColor.X * 255.999f);
            vglResultColor.g = byte(vglShadedColor.Y * 255.999f);
            vglResultColor.b = byte(vglShadedColor.Z * 255.999f);
            vglResultColor.a = 255;

            return $sdlPage16_GetShadedColor(color, vglResultColor.color);
        }
    }
public:
    static void vglBegin(gfxDrawMode drawMode, int p1) {
        // Save current vgl color
        vglColor = *vglCurrentColor;

        // calculate a nice shaded color ;)
        vglCalculatedColor = CalculateShadedColor(drawMode, vglColor);
        *vglCurrentColor = vglCalculatedColor;

        $vglBegin(drawMode, p1);
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
        struct vgl_pair {
            unsigned int begin; // vglBegin
            unsigned int end;   // vglEnd
        };

        std::initializer_list<vgl_pair> vglCBs = {
            { 0x448424, 0x4485D3 }, { 0x448697, 0x448B82 }, { 0x448903, 0x448D8C }, { 0x448BFD, 0x448FB7 },
            { 0x448DE4, 0x449219 }, { 0x44902A, 0x449480 }, { 0x4492A4, 0x44963E }, { 0x4494C3, 0x44983C },
            { 0x4496A5, 0x4499D4 }, { 0x44986B, 0x449BAA }, { 0x449A13, 0x449D42 }, { 0x449BD9, 0x449F5A },
            { 0x449D82, 0x44A146 }, { 0x449F67, 0x44A3F8 }, { 0x44A21C, 0x44A5BF }, { 0x44A444, 0x44A7C0 },
            { 0x44A629, 0x44A958 }, { 0x44A7EF, 0x44AB2E }, { 0x44A997, 0x44ACC6 }, { 0x44AB5D, 0x44AEBC },
            { 0x44AD06, 0x44B083 }, { 0x44AECA, 0x44B23D }, { 0x44B0EC, 0x44B394 }, { 0x44B24B, 0x44B531 },
            { 0x44B3B6, 0x44B6E1 }, { 0x44B557, 0x44B895 }, { 0x44B6F3, 0x44BA7C }, { 0x44B8F1, 0x44BC03 },
            { 0x44BA8A, 0x44BE8E }, { 0x44BC29, 0x44C118 }, { 0x44BE9C, 0x44C3EA }, { 0x44C136, 0x44C638 },
            { 0x44C40C, 0x44C77A }, { 0x44C64A, 0x44C989 }, { 0x44C7C0, 0x44CC44 }, { 0x44CAD6, 0x44CE63 },
            { 0x44CCF5, 0x44D04E }, { 0x44CF6D, 0x44D403 }, { 0x44D0D4, 0x44D780 }, { 0x44D5F7, 0x44D8E9 },
            { 0x44D789, 0x44E014 }, { 0x44DC55, 0x44E131 }, { 0x44E050, 0x44E22C }, { 0x44E14B, 0x44E661 },
            { 0x44E2A3, 0x44E785 }, { 0x44E69D, 0x44E886 }, { 0x44E79E, 0x44EB82 }, { 0x44EAA0, 0x44EDC3 },
            { 0x44EBA5, 0x44F0B9 }, { 0x44EFD0, 0x44F316 }, { 0x44F0DC, 0x44F64C }, { 0x44F588, 0x44FB9D },
            { 0x44F7E2, 0x44FD30 }, { 0x44FC1E, 0x44FE4E }, { 0x44FDD4, 0x44FFB3 }, { 0x44FF10, 0x450162 },
            { 0x450085, 0x450390 }, { 0x450269, 0x45078C },
            // ---------------------
            { 0x443B9D, 0x443DCC }, // dgRoadDecalInstance
            { 0x57AC4A, 0x57AD41 }, // ped LODs
        };

        // mostly copied from InstallCallback
        for (auto pair : vglCBs)
        {
            auto begin = pair.begin;
            auto end   = pair.end;

            InstallCallback(vglBeginCB, { begin, CALL });
            InstallCallback(vglEndCB,   { end,   CALL });

            LogFile::Format("   - { vglBegin: %08X => %08X, vglEnd: %08X => %08X }\n", begin, vglBeginCB, end, vglEndCB);
        }

        LogFile::Format("   - Installed %d callbacks\n", vglCBs.size());
    }
};

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

    static void ageDebug(bool enabled, const char* format, ...) {
        if (ageLogFile)
        {
            va_list va;

            va_start(va, format);
            vfprintf(ageLogFile, format, va);
            va_end(va);

            fputc('\n', ageLogFile);
            // fflush(ageLogFile);
        }
    }

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

        L.pop(1);

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

    static bool isVehInfoFile(const char *filename) {
        auto ext = strrchr(filename, '.');
        return (ext != NULL) ? (_strcmpi(ext, ".info") == 0) : false;
    }

    static bool isCityInfoFile(const char *filename) {
        auto ext = strrchr(filename, '.');
        return (ext != NULL) ? (_strcmpi(ext, ".cinfo") == 0) : false;
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

        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        SetConsoleTextAttribute(hConsole, printer_types[level]);
        $DefaultPrinter(level, message, va_args);
        SetConsoleTextAttribute(hConsole, TEXTCOLOR_LIGHTGRAY);
    }

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

        InstallCallback("CreateGameMutex", &CallbackHandler::CreateGameMutex, {
            cbHook<CALL>(0x40128D),
        });

        // revert bridges/ferries to how they were in the betas
        InstallCallback("Bridge/Ferry: Cull", &BridgeFerryHandler::Cull, {
            cbHook<CALL>(0x5780BC), // gizBridgeMgr::Cull
            cbHook<CALL>(0x5798F0), // gizFerryMgr::Cull
        });

        InstallVTableHook("Bridge/Ferry: Draw", &BridgeFerryHandler::Draw, {
            0x5B5FB8, // gizBridge::Draw
            0x5B61AC, // gizFerry::Draw
        });

        InstallVTableHook("cityLevel::SetObjectDetail", &cityLevelHandler::SetObjectDetail, {
            0x5B16E0
        });

        InstallCallback("ageDebug", &CallbackHandler::ageDebug, {
            cbHook<JMP>(0x402630),
        });

        InstallCallback("ProgressRect [white loading bar fix]", &CallbackHandler::ProgressRect, {
            cbHook<CALL>(0x401163),
            cbHook<CALL>(0x4011CC),
        });

        if (!datArgParser::Get("oldautodetect"))
        {
            // cbHook into the original AutoDetect and replace it with our own version
            InstallCallback("AutoDetectCallback", &CallbackHandler::AutoDetectCallback, {
                cbHook<JMP>(0x4AC030),
            });
        }

        InstallCallback("isVehInfoFile [fix random crashes]", &CallbackHandler::isVehInfoFile, {
            cbHook<CALL>(0x5248E1),
        });

        InstallCallback("isCityInfoFile [fix random crashes]", &CallbackHandler::isCityInfoFile, {
            cbHook<CALL>(0x5244CF),
        });

        InstallCallback("gfxPipeline::SetRes", &gfxPipelineHandler::SetRes, {
            cbHook<CALL>(0x401482),
        });

        InstallCallback("gfxPipeline::gfxWindowCreate", &gfxPipelineHandler::gfxWindowCreate, {
            cbHook<CALL>(0x4A94AA),
        });

        InstallCallback("gfxLoadVideoDatabase [disable 'badvideo.txt']", &ReturnFalse, {
            cbHook<CALL>(0x4AC4F9),
        });

        InstallCallback("mmDirSnd::Init", &mmDirSndHandler::Init, {
            cbHook<CALL>(0x51941D),
        });

        InstallCallback("memSafeHeap::Init [Heap fix]", &memSafeHeapHandler::Init, {
            cbHook<CALL>(0x4015DD),
        });

        InstallCallback("asCullManager::Init [Increase Max Cullables]", &asCullManagerHandler::Init, {
            cbHook<CALL>(0x401D5C),
        });

        // NOTE: Completely overrides the original AngelReadString (will check Lua first then DLL)
        InstallCallback("AngelReadString", &CallbackHandler::AngelReadString, {
            cbHook<JMP>(0x534790),
        });

        InstallCallback("datTimeManager::Update", &TickHandler::Update, {
            cbHook<CALL>(0x401A2F),
        });

        InstallCallback("mmGame::SendChatMessage", &ChatHandler::Process, {
            cbHook<JMP>(0x414EB6),
        });

        InstallCallback("mmGameMusicData::LoadAmbientSFX", &mmGameMusicDataHandler::LoadAmbientSFX, {
            cbHook<CALL>(0x433F93),
        });

        InstallCallback("vehCarAudioContainer::SetSirenCSVName", &vehCarAudioContainerHandler::SetSirenCSVName, {
            cbHook<CALL>(0x412783),
            cbHook<CALL>(0x412772),
        });

        // dashboard testing
        InstallCallback("mmDashView::Update [EXPERIMENTAL]", &mmDashViewHandler::UpdateCS, {
            cbHook<CALL>(0x430F87), // replaces call to asLinearCS::Update
        });

        InstallCallback("zipFile::Init ['extraLen' spam fix]", &NullSub, {
            cbHook<CALL>(0x5738EA), // 'extraLen=%d'
        });

        // install shading fix (for PSDL, etc.)
        vglHandler::InstallCallbacks();

        InstallCallback("cityLevel::DrawRooms [sdlPage16::Draw call hook]", &sdlPage16Handler::Draw, {
            cbHook<CALL>(0x4459D2),
        });

        InstallCallback("sdlPage16::Draw [SetBlockPointer implementation]", &sdlPage16Handler::SetBlockPointer, {
            cbHook<CALL>(0x448372), // 448371 + 1, after our 'push edi' instruction (SEE BELOW)
        });
    }

    static void InstallPatches() {
        LogFile::WriteLine("Installing patches...");

        InstallPatch("Increase chat buffer size", { 60 }, {
            0x4E68B5,
            0x4E68B9,
            0x50BBCF,
        });

        InstallPatch("Increase cop limit", { 64 }, {
            0x55100B,
        });

        InstallPatch("Enable pointer in windowed mode", { 0x90, 0x90 }, {
            0x4F136E,
        });

        // even the slightest modification will f$!% this up, DO NOT TOUCH THIS
        InstallPatch("Free up some space in sdlPage16::Draw", {
            0x57,                               // push edi
            0xE8, 0xCD, 0xCD, 0xCD, 0xCD,       // call <...> !!! (WILL BE INITIALIZED AS A CALLBACK) !!!
            0x53,                               // push ebx
            0x0F, 0xB7, 0x0F,                   // movzx ecx, word ptr [edi]
            0x0F, 0xB7, 0x57, 0x02,             // movzx edx, word ptr [edi+2]
            0x89, 0x8D, 0xE8, 0xFE, 0xFF, 0xFF, // mov [ebp-118], ecx
            0x83, 0xC7, 0x02,                   // add edi, 2
            0x8B, 0xC1,                         // mov eax, ecx
            0x83, 0xE0, 0x07,                   // and eax, 07
            0x8B, 0xD8,                         // mov ebx, eax
            0x75, 0x06,                         // jnz short subtype_not_zero

            0x0F, 0xB7, 0x1F,                   // movzx ebx, word ptr [edi]
            0x83, 0xC7, 0x02,                   // add edi, 2

            // subtype_not_zero:
            0x89, 0x5D, 0xFC,                   // mov [ebp-04], ebx
            0xC1, 0xE0, 0x08,                   // shl eax, 8
            0x09, 0xD0,                         // or eax, edx
            0x5B,                               // pop ebx

            0x90, 0x90, 0x90, 0x90,             // nop out the rest
        }, {
            0x448371,
        });
    }
public:
    static void Initialize(int argc, char **argv) {
        InstallPatches();
        InstallCallbacks();

        // Initialize the Lua engine
        MM2Lua::Initialize();

        // hook into the printer
        *$Printer = &PrintHandler::Print;
        *$PrintString = &PrintHandler::PrintString;
        *$FatalErrorHandler = &PrintHandler::FatalError;

        LogFile::Write("Redirecting MM2 output...");

        *datOutputStream = Stream::Create("mm2.log", &logFileMethods);

        LogFile::WriteLine((*datOutputStream) ? "Done!" : "FAIL!");

        if (ageLogFile == NULL && datArgParser::Get("age_debug"))
        {
            ageLogFile = fopen("AGE.log", "w+");
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
        } else {
            LogFile::WriteLine("WTF: Hook startup request received, but the game is closing!");
        }
    }

    static void Stop() {
        if ($gameClosing)
        {
            LogFile::WriteLine("Hook shutdown request received.");

            LogFile::Close();
            L.close(); // release Lua

            // close datOutput log
            datOutput::CloseLog();

            if (ageLogFile)
                fclose(ageLogFile);
        } else {
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

    InstallCallback("ArchInit [Framework initialization]", &HookSystemHandler::Initialize, {
        cbHook<CALL>(0x4023DB),
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

    InstallFramework();
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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
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
                } else {
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
                } else {
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
