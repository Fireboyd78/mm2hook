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

bool bSDLDrawDebug = true;

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

AGEHook<0x4A3370>::Func<void> $asLinearCS_Update;

AGEHook<0x4415E0>::Func<void> $dgBangerInstance_Draw;

AGEHook<0x577210>::Func<void> $memSafeHeap_Init;

AGEHook<0x448090>::Func<void> $sdlCommon_UpdateLighting;

AGEHook<0x4AEDC0>::Func<void> $gfxImage_Scale;

/*
    TODO: Move VGL stuff to a separate file?
*/

AGEHook<0x4A1290>::Func<void> $asCullManager_Init;

// ==========================
// Pointer hooks
// ==========================

AGEHook<0x5C571C>::Type<float> obj_NoDrawThresh; // default: 300.0

AGEHook<0x5C6658>::Type<float> obj_VLowThresh;  // default: 200.0
AGEHook<0x5C665C>::Type<float> obj_LowThresh;   // default: 100.0
AGEHook<0x5C6660>::Type<float> obj_MedThresh;   // default: 40.0

AGEHook<0x5C5708>::Type<float> sdl_VLowThresh;  // default: 300.0
AGEHook<0x5C570C>::Type<float> sdl_LowThresh;   // default: 100.0
AGEHook<0x5C5710>::Type<float> sdl_MedThresh;   // default: 50.0

AGEHook<0x661738>::Type<asNode> ROOT;

AGEHook<0x6B167C>::Type<char[40]> cityName;
AGEHook<0x6B16A4>::Type<char[40]> cityName2;

AGEHook<0x62B068>::Type<int> timeOfDay;

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

        case VK_F10:
        {
            // toggle SDL draw debugging
            bSDLDrawDebug = !bSDLDrawDebug;
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

int numPedUpdateAttempts = 0;

class aiPathHandler {
public:
    void UpdatePedestrians(void) {
        numPedUpdateAttempts = 0;
        $aiPath_UpdatePedestrians(this);
    }

    static void Install() {
        InstallCallback("aiPath::UpdatePedestrians", "Limits the number of update attempts for pedestrians.",
            &UpdatePedestrians, {
                cbHook<CALL>(0x536FE0), // aiMap::Update
            }
        );
    }
};

class aiPedestrianHandler {
public:
    void Update(void) {
        if (numPedUpdateAttempts < 256) {
            ++numPedUpdateAttempts;
            $aiPedestrian_Update(this);
        }
    }

    static void Install() {
        InstallCallback("aiPedestrian::Update", "Limits the number of update attempts for a pedestrian.",
            &Update, {
                cbHook<CALL>(0x544191), // aiPath::UpdatePedestrians
            }
        );
    }
};

class aiPoliceForceHandler {
public:
    void Reset(void) {
        // reset number of cops pursuing player
        // fixes incorrect music bug
        vehPoliceCarAudio_iNumCopsPursuingPlayer = 0;

        $aiPoliceForce_Reset(this);
    }

    static void Install() {
        InstallCallback("aiPoliceForce::Reset", "Resets the number of cops pursuing the player upon reset.",
            &Reset, {
                cbHook<CALL>(0x536AAE),
                cbHook<CALL>(0x550ECA),
            }
        );
    }
};

class asCullManagerHandler {
public:
    void Init(int maxCullables, int maxCullables2D) {
        maxCullables = 1024;
        maxCullables2D = 256;

        LogFile::Format("[asCullManager::Init]: Increased Max Cullables to %d, %d\n", maxCullables, maxCullables2D);

        $asCullManager_Init(this, maxCullables, maxCullables2D);
    }

    static void Install() {
        InstallCallback("asCullManager::Init", "Increases max cullables.",
            &Init, {
                cbHook<CALL>(0x401D5C),
            }
        );
    }
};

// aaron wanted this so badly
int city_numRooms = 0;
int city_currentRoom = 0;

class cityLevelHandler {
public:
    void DrawRooms(const LPVOID viewport, unsigned int p2, LPVOID roomRecs, int numRooms)
    {
        city_numRooms = numRooms;
        city_currentRoom = 0;

        $cityLevel_DrawRooms(this, viewport, p2, roomRecs, numRooms);
    }

    // jumped to at the end of cityLevel::Update
    void PostUpdate() {
        // update the SDL lighting
        $sdlCommon_UpdateLighting();

        // update our shaded lighting
        // TODO: fix lighting quality not being taken into account (harder than it sounds)
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
    }

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

        obj_NoDrawThresh = objNoDrawThresh;
        obj_VLowThresh   = objVLowThresh;
        obj_LowThresh    = objLowThresh;
        obj_MedThresh    = objMedThresh;

        // By default, the game doesn't set these values based on the detail level
        // They are hardcoded to what is considered 'High' by default,
        // however this is now known as 'Medium' (lod = 1; see above)
        //
        // 'Medium' and below (default 'High') uses the defaults.
        float sdlVLowThresh = (lod > 1) ? (objVLowThresh + 100.0f) : 300.0f;
        float sdlLowThresh  = (lod > 1) ? (objLowThresh + 25.0f) : 100.0f;
        float sdlMedThresh  = (lod > 1) ? (objMedThresh + 10.0f) : 50.0f;

        sdl_VLowThresh = sdlVLowThresh;
        sdl_LowThresh  = sdlLowThresh;
        sdl_MedThresh  = sdlMedThresh;

        LogFile::Format("[cityLevel::SetObjectDetail]: '%s'\n"
                        " - OBJ { %.4f, %.4f, %.4f, %.4f }\n"
                        " - SDL { %.4f, %.4f, %.4f }\n",
                        lodLevelNames[lod],
                        objNoDrawThresh, objVLowThresh, objLowThresh, objMedThresh,
                        sdlVLowThresh, sdlLowThresh, sdlMedThresh);
    }

    static void Install() {
        InstallVTableHook("cityLevel::SetObjectDetail", &SetObjectDetail, {
            0x5B16E0
        });

        InstallCallback("cityLevel::DrawRooms", "Custom implementation to allow for getting the number of rooms.",
            &DrawRooms, {
                cbHook<CALL>(0x445798), // cityLevel::Draw
            }
        );

        InstallCallback("cityLevel::Update", "Adds PostUpdate handler.",
            &PostUpdate, {
                cbHook<JMP>(0x4452D0), // jump to PostUpdate at the very end
            }
        );
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

    static void Install() {
        // revert bridges/ferries to how they were in the betas
        InstallCallback("Bridge/Ferry: Cull",
            &Cull, {
                cbHook<CALL>(0x5780BC), // gizBridgeMgr::Cull
                cbHook<CALL>(0x5798F0), // gizFerryMgr::Cull
            }
        );

        InstallVTableHook("Bridge/Ferry: Draw", &Draw, {
            0x5B5FB8, // gizBridge::Draw
            0x5B61AC, // gizFerry::Draw
        });
    }
};

class gfxPipelineHandler {
public:
    static void gfxApplySettings(void) {
        auto gfxInterface   = &gfxInterfaces[gfxInterfaceChoice];

        auto deviceType = gfxInterface->DeviceType;

        enableHWTnL = (deviceType == gfxDeviceType::HardwareWithTnL);
        useBlade    = (useSoftware = (deviceType == gfxDeviceType::Software));

        useInterface = gfxInterfaceChoice;
    }

    static LRESULT APIENTRY gfxWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg)
        {
            case WM_ACTIVATEAPP:
            {
                if ((wParam == FALSE) && datArgParser::Get("nopause"))
                    return 0;
            } break;

            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                if (HandleKeyPress(wParam))
                    return 0;
            } break;
        }

        return $gfxPipeline_gfxWindowProc(hWnd, uMsg, wParam, lParam);
    }

    static void gfxWindowMove(bool isOpen) {
        HDC hDC = GetDC(NULL);
        int screenWidth  = GetDeviceCaps(hDC, HORZRES);
        int screenHeight = GetDeviceCaps(hDC, VERTRES);
        ReleaseDC(0, hDC);

        window_X = (screenWidth - window_iWidth) / 2;
        window_Y = (screenHeight - window_iHeight) / 2;

        // window may or may not be initialized yet
        if (isOpen) {
            MoveWindow(
                hWndMain,
                window_X,
                window_Y,
                window_iWidth,
                window_iHeight,
                0);
        }
    }

    static void gfxWindowUpdate(bool isOpen) {
        RECT rect;
        GetClientRect(hWndMain, &rect);

        MoveWindow(
            hWndMain,
            window_X,
            window_Y,
            (2 * window_iWidth - rect.right),
            (2 * window_iHeight - rect.bottom),
            isOpen /* repaint if open */);
    }

    static void SetRes(int width, int height, int cdepth, int zdepth, bool parseArgs) {
        if (datArgParser::Get("ref")) {
            useSoftware    = 1;
            useReference   = 1;
        } else if (datArgParser::Get("blade") || datArgParser::Get("bladed")) {
            useSoftware    = 1;
            useBlade       = 1;
        } else if (datArgParser::Get("swage")) {
            useSoftware    = 1;
            useAgeSoftware = 1;
        } else if (datArgParser::Get("sw")) {
            useSoftware    = 1;
        }

        if (datArgParser::Get("sysmem")) {
            useSysMem = 1;
        }
        if (datArgParser::Get("triple")) {
            tripleBuffer = 1;
        }

        if (datArgParser::Get("nomultitexture") || datArgParser::Get("nomt")) {
            useMultiTexture = 0;
        }
        if (datArgParser::Get("novblank") || datArgParser::Get("novsync")) {
            novblank = 1;
        }
        if (datArgParser::Get("nohwtnl")) {
            enableHWTnL = 0;
        }

        if (datArgParser::Get("primary")) {
            useInterface = 0;
        } else {
            datArgParser::Get("display", 0, &useInterface);
        }
        if (datArgParser::Get("single")) {
            pageFlip = 0;
        }

        if (datArgParser::Get("window") || datArgParser::Get("windowed")) {
            inWindow = 1;
        } else if (datArgParser::Get("fs") || datArgParser::Get("fullscreen")) {
            inWindow = 0;
        }

        int bitDepth = 0;
        if (datArgParser::Get("bpp", 0, &bitDepth) || datArgParser::Get("bitdepth", 0, &bitDepth)) {
            cdepth = bitDepth;
            zdepth = bitDepth;
        } else {
            datArgParser::Get("cdepth", 0, &cdepth);
            datArgParser::Get("zdepth", 0, &zdepth);
        }

        // We don't want to set the width/height if we are in a menu, it just fucks it up
        if (splashScreen != 0) {
            if (datArgParser::Get("max")) {
                HDC hDC = GetDC(NULL);
                width  = GetDeviceCaps(hDC, HORZRES);
                height = GetDeviceCaps(hDC, VERTRES);
                ReleaseDC(0, hDC);
            } else {
                datArgParser::Get("width",  0, &width);
                datArgParser::Get("height", 0, &height);
            }

            // datArgParser::Get("width",  0, &width);
            // datArgParser::Get("height", 0, &height);
        }

        useSysMem = useSoftware;

        window_iWidth  = width;
        window_iHeight = height;

        window_fWidth  = float(width);
        window_fHeight = float(height);

        window_ColorDepth = cdepth;
        window_ZDepth     = zdepth;

        gfxTexture::sm_Allow32 = (cdepth == 32);

        LogFile::Format("[gfxPipeline::SetRes]: 32-bit textures are%s allowed.\n", (gfxTexture::sm_Allow32) ? "" : "n't" );
        LogFile::Format("[gfxPipeline::SetRes]: %dx%dx%dx%d\n", width, height, cdepth, zdepth);

        if (lpDD)
        {
            if (inWindow)
            {
                gfxWindowMove(true);
                gfxWindowUpdate(true);
            } else {
                DDSURFACEDESC2 ddSurfaceDesc;

                ddSurfaceDesc.dwSize = 0x7C;

                if ((lpDD->GetDisplayMode(&ddSurfaceDesc) != DD_OK)
                    || (ddSurfaceDesc.dwWidth != window_iWidth)
                    || (ddSurfaceDesc.dwHeight != window_iHeight))
                {
                    if (lpDD->SetDisplayMode(
                        window_iWidth,
                        window_iHeight,
                        window_ColorDepth,
                        0,
                        0) != DD_OK) {
                        LogFile::WriteLine("[gfxPipeline::SetRes]: SHIT! Failed to set the display mode!");
                    }
                }
            }
        }

        ioMouse_InvWidth  = (1.0f / window_fWidth);
        ioMouse_InvHeight = (1.0f / window_fHeight);
    }

    static void gfxWindowCreate(LPCSTR lpWindowName) {
        if (hWndMain)
            return;

        if (lpWindowTitle)
            lpWindowName = lpWindowTitle;

        if (!ATOM_Class)
        {
            WNDCLASSA wc = {
                CS_HREDRAW | CS_VREDRAW,    /* style */
                gfxWindowProc,              /* lpfnWndProc */
                0,                          /* cbClsExtra */
                0,                          /* cbWndExtra */
                0,                          /* hInstance */
                LoadIconA(GetModuleHandleA(NULL), IconID ? IconID : IDI_APPLICATION),
                                            /* hIcon */
                LoadCursorA(0, IDC_ARROW),  /* hCursor */
                CreateSolidBrush(NULL),     /* hbrBackground */
                NULL,                       /* lpszMenuName */
                "gfxWindow",                /* lpszClassName */
            };

            ATOM_Class = RegisterClassA(&wc);
        }

        DWORD dwStyle = WS_POPUP;

        if (inWindow)
        {
            if (hWndParent)
            {
                dwStyle = WS_CHILD;
            }
            else if (hasBorder = !(datArgParser::Get("noborder") || datArgParser::Get("borderless")))
            {
                dwStyle |= (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
            }
        }
        else
        {
            dwStyle |= WS_SYSMENU;
        }

        // update the position
        gfxWindowMove(false);

        HWND hWND = CreateWindowExA(
            WS_EX_APPWINDOW,
            "gfxWindow",
            lpWindowName,
            dwStyle,
            window_X,
            window_Y,
            640,
            480,
            hWndParent,
            0,
            0,
            0);

        hWndMain = hWND;

        if (inWindow)
            gfxWindowUpdate(false);

        SetCursor(NULL);
        ShowCursor(FALSE);

        ShowWindow(hWND, TRUE);
        UpdateWindow(hWND);
        SetFocus(hWND);
    }

    static void gfxSetTexReduceSize(int) {
        static const int gfxReduceSizes[4] = {
            64,     // Low
            128,    // Medium
            256,    // High
            0,      // Very High (unlimited)
        };

        gfxTexReduceSize = gfxReduceSizes[*gfxTexQuality];
    }

    static void Install() {
        InstallPatch("Enables pointer in windowed mode.", { 0x90, 0x90 }, {
            0x4F136E,
        });

        InstallCallback("gfxPipeline::SetRes", "Custom implementation allowing for more control of the window.",
            &SetRes, {
                cbHook<JMP>(0x4A8CE0),
            }
        );

        InstallCallback("gfxPipeline::gfxWindowCreate", "Custom implementation allowing for more control of the window.",
            &gfxWindowCreate, {
                cbHook<JMP>(0x4A8A90),
            }
        );

        InstallCallback("gfxApplySettings", "Custom implementation allowing for more control of the graphical settings.",
            &gfxApplySettings, {
                cbHook<JMP>(0x4AC870),
            }
        );

        InstallCallback("gfxSetTexReduceSize", "Allows for unlimited texture sizes when texture quality is Very High.",
            &gfxSetTexReduceSize, {
                cbHook<JMP>(0x4B3020),
            }
        );

        InstallCallback("gfxLoadVideoDatabase", "Disables 'badvideo.txt' file.",
            &ReturnFalse, {
                cbHook<CALL>(0x4AC4F9),
            }
        );
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

    static void Install() {
        InstallCallback("memSafeHeap::Init", "Adds '-heapsize' parameter that takes a size in megabytes. Defaults to 128MB.",
            &Init, {
                cbHook<CALL>(0x4015DD),
            }
        );
    }
};

class gfxImageHandler {
public:
    void Scale(int width, int height) {
        width  = *window_iWidth;
        height = *window_iHeight;

        $gfxImage_Scale(this, width, height);
    }

    static void Install() {
        InstallCallback("gfxImage::Scale", "Fixes loading screen image scaling",
            &Scale, {
                cbHook<CALL>(0x401C75),
            }
        );
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

    static void Install() {
        // dashboard testing
        InstallCallback("mmDashView::Update", "Experimental testing.",
            &UpdateCS, {
                cbHook<CALL>(0x430F87), // replaces call to asLinearCS::Update
            }
        );
    }
};

class mmDirSndHandler {
public:
    static mmDirSnd* Init(int sampleRate, bool enableStero, int a4, float volume, LPCSTR deviceName, bool enable3D) {
        if (deviceName == '\0') {
            deviceName = "Primary Sound Driver";

            LogFile::WriteLine("[mmDirSnd::Init]: Using primary sound driver");
        }

        // TODO: Set sampling rate (see 0x519640 - int __thiscall AudManager::SetBitDepthAndSampleRate(int this, int bitDepth, int samplingRate))
        // TODO: Redo SetPrimaryBufferFormat to set sampleSize? (see 0x5A5860 -void __thiscall DirSnd::SetPrimaryBufferFormat(mmDirSnd *this, int sampleRate, bool allowStero))
        return mmDirSnd::Init(48000, enableStero, a4, volume, deviceName, enable3D);
    }

    static void Install() {
        InstallCallback("mmDirSnd::Init", "Fixes no sound issue on startup.",
            &Init, {
                cbHook<CALL>(0x51941D),
            }
        );
    }
};

class mmGameHandler {
public:
    void SendChatMessage(char *message) {
        if (isConsoleOpen) {
            MM2Lua::SendCommand(message);

            // return normal chatbox behavior
            isConsoleOpen = false;
        } else {
            LogFile::Format("Got chat message: %s\n", message);
        }
    }

    static void Install() {
        InstallPatch("Increases chat buffer size.", { 60 }, {
            0x4E68B5,
            0x4E68B9,
            0x50BBCF,
        });

        InstallCallback("mmGame::SendChatMessage", "Passes any chat messages to the handler.",
            &SendChatMessage, {
                cbHook<JMP>(0x414EB6),
            }
        );
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

    static void Install() {
        InstallCallback("mmGameMusicData::LoadAmbientSFX", "Allows for custom ambient effects in addon cities.",
            &LoadAmbientSFX, {
                cbHook<CALL>(0x433F93),
            }
        );
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

    static void Install() {
        InstallCallback("vehCarAudioContainer::SetSirenCSVName", "Allows for custom sirens in addon cities.",
            &SetSirenCSVName, {
                cbHook<CALL>(0x412783),
                cbHook<CALL>(0x412772),
            }
        );
    }
};

// HACK HACK HACK!
// Doesn't work well for subway stations :(
bool insideTunnel = false;

class sdlPage16Handler {
public:
    static sdlPage16 *page; // current sdl page
    static LPVOID attributePtr; // current attribute pointer

    // this MUST clean up the stack, hence the stdcall
    static void __stdcall SetAttributePointer(LPVOID lpBlock) {
        attributePtr = lpBlock;
    }

    void Draw(int p1, unsigned int p2) {
        page = reinterpret_cast<sdlPage16 *>(this);
        
        if (bSDLDrawDebug)
        {
            Draw_Impl(p1, p2);
        }
        else
        {
            page->Draw(p1, p2);
        }
        
        // not in a block anymore
        page = NULL;
        attributePtr = NULL; // lets vglHandler know we're not checking for SDL stuff

        // so hacky
        insideTunnel = false;

        // move to the next room
        ++city_currentRoom;
    }

    void Draw_Impl(int lod, unsigned int baseColor) {
        if (page->Attributes == nullptr)
            return;

        ushort* attributes = page->Attributes;

        Vector3 vecBuffer0[256];
        Vector3 vecBuffer1[256];
        float   sBuffer[256];

        int current_shaded_color = 0;
        int current_texture = 0;

        bool is_texture_null = false;

        while (true)
        {
            // temporary?
            attributePtr = attributes;

            ushort attribute = attributes[0];
            ++attributes;

            ushort type = (attribute & 0b1111000) >> 3;
            ushort vertex_count = (attribute & 0b111);

            ushort texture_id = (attributes[0] + (vertex_count << 8));

            if (vertex_count == 0)
            {
                vertex_count = attributes[0];
                ++attributes;
            }

            switch (type)
            {
                case 0:
                {
                    // if (current_texture)
                    if (false)
                    {
                        vglBindTexture(page->Textures[current_texture + 1]);

                        page->ArcMap(sBuffer, attributes, 4, vertex_count, 1);

                        if (attributes[0] != attributes[1])
                        {
                            {
                                vglBegin(DRAWMODE_TRIANGLESTRIP, vertex_count * 2);

                                for (int i = 0; i < vertex_count; ++i)
                                {
                                    ushort  attrib0 = attributes[(i * 4) + 0];
                                    ushort  attrib1 = attributes[(i * 4) + 1];
                                    Vector3 vertex0 = page->CodedVertices[attrib0];
                                    Vector3 vertex1 = page->CodedVertices[attrib1];

                                    vglTexCoord2f(sBuffer[i], 1.0f);
                                    vglVertex3f(vertex0.X, vertex0.Y + 0.15f, vertex0.Z);

                                    vglTexCoord2f(sBuffer[i], 0.0f);
                                    vglVertex3f(vertex1.X, vertex1.Y, vertex1.Z);
                                }

                                vglEnd();
                            }

                            vglCurrentColor = ((baseColor >> 1) & 0x7F7F7F) | 0xFF000000;

                            {
                                vglBegin(DRAWMODE_TRIANGLESTRIP, vertex_count * 2);

                                for (int i = 0; i < vertex_count; ++i)
                                {
                                    ushort  attrib1 = attributes[(i * 4) + 1];
                                    Vector3 vertex1 = page->CodedVertices[attrib1];

                                    vglTexCoord2f(sBuffer[i], 0.0f);

                                    vglVertex3f(vertex1.X, vertex1.Y, vertex1.Z);

                                    vglVertex3f(vertex1.X, vertex1.Y, vertex1.Z);
                                }

                                vglEnd();
                            }

                            vglCurrentColor = baseColor;
                        }

                        if (attributes[2] != attributes[3])
                        {
                            page->ArcMap(sBuffer, attributes + 2, 4, vertex_count, 1);

                            {
                                vglBegin(DRAWMODE_TRIANGLESTRIP, 2 * vertex_count);

                                for (int i = 0; i < vertex_count; ++i)
                                {
                                    ushort attrib2 = attributes[(i * 4) + 2];
                                    ushort attrib3 = attributes[(i * 4) + 3];
                                    Vector3 vertex2 = page->CodedVertices[attrib2];
                                    Vector3 vertex3 = page->CodedVertices[attrib3];

                                    vglTexCoord2f(sBuffer[i], 0.0f);
                                    vglVertex3f(vertex2.X, vertex2.Y, vertex2.Z);

                                    vglTexCoord2f(sBuffer[i], 1.0f);
                                    vglVertex3f(vertex3.X, vertex3.Y, vertex3.Z);
                                }

                                vglEnd();
                            }

                            vglCurrentColor = ((baseColor >> 1) & 0x7F7F7F) | 0xFF000000;

                            {
                                vglBegin(DRAWMODE_TRIANGLESTRIP, vertex_count * 2);

                                for (int i = 0; i < vertex_count; ++i)
                                {
                                    ushort attrib1  = attributes[(i * 4) + 2];
                                    Vector3 vertex1 = page->CodedVertices[attrib1];

                                    vglTexCoord2f(sBuffer[i], 0.0f);
                                    vglVertex3f(vertex1.X, vertex1.Y, vertex1.Z);

                                    vglVertex3f(vertex1.X, vertex1.Y + 0.15f, vertex1.Z);
                                }

                                vglEnd();
                            }

                            vglCurrentColor = baseColor;
                        }

                        page->ArcMap(sBuffer, attributes + 1, 4, vertex_count, 1);

                        vglBindTexture(page->Textures[current_texture]);

                        {
                            vglBegin(DRAWMODE_TRIANGLESTRIP, vertex_count * 2);

                            for (int i = 0; i < vertex_count; ++i)
                            {
                                ushort attrib1  = attributes[(i * 4) + 1];
                                ushort attrib2  = attributes[(i * 4) + 2];
                                Vector3 vertex1 = page->CodedVertices[attrib1];
                                Vector3 vertex2 = page->CodedVertices[attrib1];

                                vglTexCoord2f(sBuffer[i], 1.0f);
                                vglVertex3f(vertex1);

                                vglTexCoord2f(sBuffer[i], 0.0f);
                                Vector3 vertex_ =
                                {
                                    (vertex2.X - vertex1.X) * 0.5f + vertex1.X,
                                    (vertex2.Y - vertex1.Y) * 0.5f + vertex1.Y,
                                    (vertex2.Z - vertex1.Z) * 0.5f + vertex1.Z
                                };
                                vglVertex3f(vertex_.X, vertex_.Y, vertex_.Z);
                            }

                            vglEnd();
                        }

                        {
                            vglBegin(DRAWMODE_TRIANGLESTRIP, vertex_count * 2);

                            for (int i = 0; i < vertex_count; ++i)
                            {
                                ushort attrib1  = attributes[(i * 4) + 1];
                                ushort attrib2  = attributes[(i * 4) + 2];
                                Vector3 vertex1 = page->CodedVertices[attrib1];
                                Vector3 vertex2 = page->CodedVertices[attrib1];

                                vglTexCoord2f(sBuffer[i], 0.0f);
                                Vector3 vertex_ =
                                {
                                    (vertex2.X - vertex1.X) * 0.5f + vertex1.X,
                                    (vertex2.Y - vertex1.Y) * 0.5f + vertex1.Y,
                                    (vertex2.Z - vertex1.Z) * 0.5f + vertex1.Z
                                };
                                vglVertex3f(vertex_.X, vertex_.Y, vertex_.Z);

                                vglTexCoord2f(sBuffer[i], 1.0f);
                                vglVertex3f(vertex2);
                            }

                            vglEnd();
                        }
                    }

                    attributes += 4 * vertex_count;
                } break;

                case 1:
                {
                    if (!is_texture_null)
                    {
                        vglBindTexture(page->Textures[current_texture + 1]);

                        if ((vertex_count != 2) || (attributes[0] != attributes[1]) || (attributes[0] >= 2))
                        {
                            {
                                float deltaS = floor(page->CodedVertices[attributes[0]].X * 0.25f);
                                float deltaT = floor(page->CodedVertices[attributes[0]].Z * 0.25f);

                                vglBegin(DRAWMODE_TRIANGLESTRIP, 2 * vertex_count);

                                for (int i = 0; i < vertex_count; ++i)
                                {
                                    ushort attrib0  = attributes[(i * 2) + 0];
                                    ushort attrib1  = attributes[(i * 2) + 1];
                                    Vector3 vertex0 = page->CodedVertices[attrib0];
                                    Vector3 vertex1 = page->CodedVertices[attrib1];

                                    vglTexCoord2f(
                                        (vertex0.X * 0.25f) - deltaS,
                                        (vertex0.Z * 0.25f) - deltaT);

                                    vglVertex3f(vertex0.X, vertex0.Y, vertex0.Z);


                                    vglTexCoord2f(
                                        (vertex1.X * 0.25f) - deltaS,
                                        (vertex1.Z * 0.25f) - deltaT);

                                    vglVertex3f(vertex1.X, vertex1.Y, vertex1.Z);
                                }

                                vglEnd();
                            }
                        }
                    }

                    attributes += (vertex_count * 2);
                } break;

                case 2:
                {
                    attributes += (vertex_count * 2);
                } break;

                case 3:
                {
                    attributes += 4;
                } break;

                case 4:
                {
                    attributes += vertex_count;
                } break;

                case 5:
                {
                    attributes += (vertex_count + 2);
                } break;

                case 6:
                {
                    attributes += (vertex_count + 2);
                } break;

                case 7:
                {
                    current_shaded_color = attributes[0];
                    attributes += 4;
                } break;

                case 8:
                {
                    attributes += (vertex_count * 5) + 2;
                } break;

                case 9:
                {
                    attributes += 3;

                    if (vertex_count == 10)
                        attributes += 7;
                } break;

                case 10:
                {
                    current_texture = texture_id;

                    if ((current_texture >> 8) & 0b111)
                        ++attributes;
                } break;

                case 11:
                {
                    attributes += 6;
                } break;

                case 12:
                {
                    attributes += 1;

                    attributes += (vertex_count + 1);
                } break;

                default:
                {
                    Displayf("Invalid cmd %d (%d)", attribute, (attribute >> 3) & 0xF);
                } break;
            }

            if (attribute & 0b10000000)
                break;
        }
    }

    static void InvalidCmd(LPCSTR, int attr, int subtype)
    {
        char buf[256] = { NULL };
        int idx = 0;

        for (int i = 0; i < 16; i++)
            idx += sprintf(&buf[idx], "%02X ", *((byte*)attributePtr + i));

        Quitf("Invalid cmd %d (%d) : [%d / %d] : %x\ndump: %s", attr, subtype, city_currentRoom, city_numRooms, attributePtr, buf);
    }

    static void Install() {
        // even the slightest modification will f$!% this up, DO NOT TOUCH THIS
        InstallPatch({
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

        InstallCallback("sdlPage16::Draw", "SetAttributePointer implementation.",
            &SetAttributePointer, {
                cbHook<CALL>(0x448372), // 448371 + 1, after our 'push edi' instruction (SEE ABOVE)
            }
        );

        InstallCallback("sdlPage16::Draw", "Hooks a call to Quitf to print out more detailed information.",
            &InvalidCmd, {
                cbHook<CALL>(0x4507B3),
            }
        );

        InstallCallback("cityLevel::DrawRooms", "Intercepts a call to sdlPage16::Draw.",
            &Draw, {
                cbHook<CALL>(0x4459D2),
            }
        );
    }
};

sdlPage16 * sdlPage16Handler::page;
LPVOID sdlPage16Handler::attributePtr;

class vglHandler {
private:
    static unsigned int GetAdjustedColor(gfxDrawMode drawMode, unsigned int color) {
        if (sdlPage16Handler::page != NULL)
        {
            // fullbright
            static const ColorARGB sdlTunnelColor = { 255, 255, 255, 255 };

            // we can finally check for attributes!
            short *attr = (short*)sdlPage16Handler::attributePtr;

            int type = ((*attr >> 3) & 0xF);
            int subtype = (*attr & 0x7);

            /*
                Due to limitations in Angel's SDL format,
                this is the best way we can find "tunnels".
                
                It's not guaranteed to be 100% accurate,
                but it works pretty well for what we've got.
            */
            if (type == 9) {
                // flag location varies on subtype
                // there's a subtype 2 but it's never (?) used
                int flags = (subtype == 0) ? attr[2]
                    : (subtype == 3) ? attr[1] : 0;

                // do we have a roof?
                // note: junctions can't have curved roofs,
                // but this is a quicker way of doing things
                if ((flags & 0x8) || (flags & 0x100)) {
                    /* junction */
                    if (subtype == 0) {
                        int wallVisInfos = attr[1];
                        int unk = attr[5];

                        for (int i = 0; i < wallVisInfos; i++) {
                            int info = attr[6 + i];

                            if (info > 0) {
                                // found a wall!
                                // we're probably in a tunnel :)
                                insideTunnel = true;
                                break;
                            }
                        }
                    }
                    /* road */
                    else if (subtype == 3) {
                        // does tunnel have walls?
                        insideTunnel = (flags & 0x3) ? true : false;
                    }
                    /* unknown */
                    else {
                        // probably not a tunnel
                        insideTunnel = false;
                    }
                } else {
                    // tunnels usually have roofs...
                    // so we're probably not in a tunnel
                    insideTunnel = false;
                }
            }

            // use fullbright inside tunnels instead of the calculated colors below
            // will still preserve shading as well
            if (insideTunnel)
                return sdlPage16::GetShadedColor(color, sdlTunnelColor.color);
        }

        return sdlPage16::GetShadedColor(color, vglResultColor.color);
    }
public:
    static void vglBeginShaded(gfxDrawMode drawMode, int p1) {
        // Save current vgl color
        vglColor = *vglCurrentColor;

        vglCalculatedColor = GetAdjustedColor(drawMode, vglColor);
        vglCurrentColor = vglCalculatedColor;

        vglBegin(drawMode, p1);
    }

    static void vglEndShaded(void) {
        // restore color
        vglCurrentColor = vglColor;

        vglEnd();
    }

    static void Install() {
        LogFile::WriteLine(" - Installing shading fix...");

        auto_ptr vglBeginCB = &vglBeginShaded;
        auto_ptr vglEndCB = &vglEndShaded;

        LogFile::Format(" - vglBeginCB: %08X\n", vglBeginCB);
        LogFile::Format(" - vglEndCB: %08X\n", vglEndCB);

        // use a custom struct to make the process easier
        // this allows us to have an entry representing each "frame" (vglBegin/vglEnd)
        // and cuts down on the amount of time it takes to add a new entry
        struct vgl_pair {
            unsigned int begin; // vglBegin
            unsigned int end;   // vglEnd
        };

        std::initializer_list<vgl_pair> vglCBs = {
            /* ------------ sdlPage16::Draw ------------- */
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
            /* ------------------------------------------- */
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

            LogFile::Format("   - { vglBegin: %08X, vglEnd: %08X }\n", begin, end);
        }
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
        LARGE_INTEGER tStart, tEnd, tElapsed;
        LARGE_INTEGER tFrequency;

        QueryPerformanceFrequency(&tFrequency);

        Displayf("AutoDetect: GUID=%x, Description=%s, Name=%s", lpGUID, lpDriverDescription, lpDriverName);

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

            QueryPerformanceCounter(&tStart);
            lpDD->EnumDisplayModes(0, 0, gfxInterface, $ResCallback);
            lpDD->Release();
            QueryPerformanceCounter(&tEnd);

            tElapsed.QuadPart = (tEnd.QuadPart - tStart.QuadPart);
            tElapsed.QuadPart *= 1000000;
            tElapsed.QuadPart /= tFrequency.QuadPart;

            Displayf("  Enumerated display modes in %.4f ms", tElapsed.QuadPart * 0.001);

            lpDD = nullptr;

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

    static void Install() {
        InstallCallback("CreateGameMutex", "Adds '-nomutex' argument to allow multiple game processes.",
            &CreateGameMutex, {
                cbHook<CALL>(0x40128D),
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

        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

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

// make this clean up the stack since we'll be calling it a lot
void __stdcall InstallHandler(LPCSTR name, void (*installHandler)(void)) {
    LogFile::Format("Installing '%s' handler...\n", name);
    installHandler();
};

/*
    Assumes THandler is a class that implements a public,
    static method called 'Install' with no return type.
*/
template <class THandler>
inline void InstallHandler(LPCSTR name) {
    InstallHandler(name, &THandler::Install);
};

class HookSystemFramework
{
private:
    /*
        Installs all of the callbacks for MM2Hook.

        The most important ones are initialized at the top,
        but other than that there is no particular order.
    */
    static void InstallHandlers() {
        /*
            Initialize the really important handlers
        */
        InstallHandler<CallbackHandler>("Generic callbacks");
        InstallHandler<PrintHandler>("Print system");
        InstallHandler<TimeHandler>("Time manager");

        InstallHandler<gfxPipelineHandler>("gfxPipeline");
        InstallHandler<memSafeHeapHandler>("memSafeHeap");

        /*
            Initialize the rest of the handlers
            Order doesn't really matter, just whatever looks neat
        */

        InstallHandler<aiPathHandler>("aiPath");
        InstallHandler<aiPedestrianHandler>("aiPedestrian");

        InstallHandler<aiPoliceForceHandler>("aiPoliceForce");

        InstallHandler<asCullManagerHandler>("asCullManager");

        InstallHandler<cityLevelHandler>("cityLevel");

        InstallHandler<BridgeFerryHandler>("gizBridge/gizFerry");

        InstallHandler<mmDashViewHandler>("mmDashView");
        InstallHandler<mmDirSndHandler>("mmDirSnd");
        InstallHandler<mmGameHandler>("mmGame");
        InstallHandler<mmGameMusicDataHandler>("mmGameMusicData");

        InstallHandler<vehCarAudioContainerHandler>("vehCarAudioContainer");

        InstallHandler<sdlPage16Handler>("sdlPage16");
        InstallHandler<vglHandler>("VGL drawing");

        InstallHandler<gfxImageHandler>("gfxImage");
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

        /* Causes game crashes, needs fixing
        InstallPatch("dgPhysManager [Raise MAX_MOVERS]", { 64 }, {
            0x4683CE + 2, 0x468582 + 2,
            0x46869C + 2, 0x46872E + 2,
            0x468E16 + 1,
        });

        // from 0x12B0 to 0x24B0, since we're doubling the array size
        InstallPatch("dgPhysManager [Increase structure size]", { 0x24 }, {
            // mmGame::Init
            0x4129C7 + 2, // operator new
            0x412C52 + 3, 0x412C67 + 3,

            // dgPhysManager::dgPhysManager(void)
            0x46820C + 3, 0x468216 + 3,

            // dgPhysManager::DeclareMover(lvlInstance *, int, int)
            0x4683C8 + 3, 0x4683FD + 3, 0x468410 + 3, 0x468427 + 3,
            0x46843B + 3, 0x4684FC + 3, 0x468506 + 3,

            // dgPhysManager::NewMover(lvlInstance *, lvlInstance *)
            0x46857C + 3, 0x4685B0 + 3, 0x4685C8 + 3, 0x46865E + 3,

            // dgPhysManager::NewMover(lvlInstance *)
            0x468696 + 3, 0x4686DE + 3, 0x4686E5 + 3,

            // dgPhysManager::NewMover(lvlInstance *, lvlInstance *, lvlInstance *)
            0x468728 + 3, 0x46879D + 3, 0x4687EE + 3, 0x46882E + 3,
            0x468837 + 3,

            // dgPhysManager::IgnoreMover(lvlInstance *)
            0x468864 + 3,

            // dgPhysManager::Update(void)
            0x4688D5 + 3, 0x468934 + 3, 0x468948 + 3, 0x46896B + 3,
            0x468976 + 3, 0x468998 + 3, 0x4689DA + 3, 0x4689EF + 3,
            0x468A1B + 3, 0x468A4D + 3, 0x468A6F + 3, 0x468AAF + 3,
            0x468AD8 + 3, 0x468B1A + 3, 0x468B52 + 3, 0x468BE6 + 3,
            0x468C49 + 3, 0x468C8D + 3, 0x468CA6 + 3, 0x468CB6 + 3,
            0x468CDD + 3, 0x468D31 + 3, 0x468D51 + 3, 0x468D65 + 3,
            0x468DB1 + 3,

            // dgPhysManager::ResetTable(void)
            0x468E31 + 3,
        });
        */
    }
public:
    static void Initialize(int argc, char **argv) {
        InstallPatches();
        InstallHandlers();

        // Initialize the Lua engine
        MM2Lua::Initialize();

        LogFile::Write("Redirecting MM2 output...");

        datOutput::sm_Stream = Stream::Create("mm2.log", &logFileMethods);

        LogFile::WriteLine((datOutput::sm_Stream) ? "Done!" : "FAIL!");

        if (datArgParser::Get("age_debug") || datArgParser::Get("ageDebug"))
        {
            // AGE.log is a catch-all debug log
            // it will output _all_ debug to a file

            ageLogFile = fopen("AGE.log", "w+");
        } else {
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
    }

    static void Reset(bool restarting) {
        LogFile::Write("Hook reset request received: ");
        LogFile::WriteLine((restarting) ? "leaving GameLoop" : "entering GameLoop");

        if (restarting)
            MM2Lua::OnRestart();

        MM2Lua::Reset();
    }

    static void Start() {
        if (!gameClosing)
        {
            // GameLoop was restarted
            Reset(false);
        } else {
            LogFile::WriteLine("WTF: Hook startup request received, but the game is closing!");
        }
    }

    static void Stop() {
        if (gameClosing)
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

    static void Install() {
        LogFile::WriteLine("Installing framework...");

        __VtResumeSampling = &Start;
        __VtPauseSampling = &Stop;

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
