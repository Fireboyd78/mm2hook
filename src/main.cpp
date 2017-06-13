#include "main.h"

using namespace MM2;

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
HOOK_SET(0x4A3370, _Func<void>::ThisCall, $asLinearCS_Update);
HOOK_SET(0x4415E0, _Func<void>::ThisCall, $dgBangerInstance_Draw);
HOOK_SET(0x577210, _Func<void>::ThisCall, $memSafeHeap_Init);
HOOK_SET(0x4AEDC0, _Func<void>::ThisCall, $gfxImage_Scale);

/*
    TODO: Move VGL stuff to a separate file?
*/

HOOK_SET(0x4A1290, _Func<void>::ThisCall, $asCullManager_Init);

// ==========================
// Pointer hooks
// ==========================

HOOK_SET(0x5C571C, _Type<float>, obj_NoDrawThresh); // default: 300.0
HOOK_SET(0x5C6658, _Type<float>, obj_VLowThresh);  // default: 200.0
HOOK_SET(0x5C665C, _Type<float>, obj_LowThresh);   // default: 100.0
HOOK_SET(0x5C6660, _Type<float>, obj_MedThresh);   // default: 40.0

HOOK_SET(0x5C5708, _Type<float>, sdl_VLowThresh);  // default: 300.0
HOOK_SET(0x5C570C, _Type<float>, sdl_LowThresh);   // default: 100.0
HOOK_SET(0x5C5710, _Type<float>, sdl_MedThresh);   // default: 50.0

HOOK_SET(0x661738, _Type<asNode>, ROOT);

HOOK_SET(0x6B167C, _Type<char[40]>, cityName);
HOOK_SET(0x6B16A4, _Type<char[40]>, cityName2);

HOOK_SET(0x62B068, _Type<int>, timeOfDay);

HOOK_SET(0x627518, _Type<int>, vehCar_bHeadlights);

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
            auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;

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

        case VK_F7:
        {
            // TODO: make this a separate plugin
            // toggle vehicle headlights
            vehCar_bHeadlights = !vehCar_bHeadlights;
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
        vehPoliceCarAudio::iNumCopsPursuingPlayer = 0;

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

        $::cityLevel::DrawRooms(this, viewport, p2, roomRecs, numRooms);
    }

    // jumped to at the end of cityLevel::Update
    void PostUpdate() {
        // update the SDL lighting
        sdlCommon::UpdateLighting();

        // update our shaded lighting
        // TODO: fix lighting quality not being taken into account (harder than it sounds)
        auto timeWeather = *$::timeWeathers + timeOfDay;

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
        // TODO: Make this do something?
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
        if (gameState != 0) {
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

        ioMouse::InvWidth  = (1.0f / window_fWidth);
        ioMouse::InvHeight = (1.0f / window_fHeight);
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

class datCallbackExtensionHandler {
public:
	static void Install() {
		 InstallPatch("datCallback Fix 1", { 0x00, 0x00, 0x00, 0x40 }, { 0x4C7A5B + 2, 0x4C7AC8 + 2, 0x4C7B70 + 1, 0x4C7BA6 + 1 });
		 InstallPatch("datCallback Fix 2", { 0x00, 0x00, 0x00, 0x80 }, { 0x4C7A90 + 2, 0x4C7AFB + 2, 0x4C7B7E + 1, 0x4C7BB4 + 1 });
		 InstallPatch("datCallback Fix 3", { 0x00, 0x00, 0x00, 0xC0 }, { 0x4C7AB0 + 2, 0x4C7B2B + 2, 0x4C7B90 + 1, 0x4C7BC9 + 1, 0x4C7B61 + 1 });
		 InstallPatch("datCallback Fix 4", { 0xFF, 0xFF, 0xFF, 0x3F }, { 0x4C7B5B + 2 });
		 InstallPatch("datCallback Code Cave", { 0xFF, 0xE1 }, { 0x4C7BE3 });
	}
};

class mmDirSndHandler {
public:
    static mmDirSnd* Init(ulong sampleRate, bool enableStero, int p3, int volume, char *deviceName, short p6, short p7) {
        if (*deviceName == '\0') {
            deviceName = "Primary Sound Driver";

            LogFile::WriteLine("[mmDirSnd::Init]: Using primary sound driver");
        }

        /*
            TODO:
            
            - Set sampling rate (see: AudManager::SetBitDepthAndSampleRate(int bitDepth, ulong samplingRate))
            - Redo SetPrimaryBufferFormat to set sampleSize? (see: DirSnd::SetPrimaryBufferFormat(ulong sampleRate, bool allowStero))
        */
        return mmDirSnd::Init(48000, enableStero, p3, volume, deviceName, p6, p7);
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

        LPCSTR szAmbientSFX = (datAssetManager::Exists("aud\\dmusic\\csv_files", buffer, "csv")) ? buffer : "sfambience";

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

        LPCSTR szSirenName = (datAssetManager::Exists("aud\\cardata\\player", buffer, "csv")) ? buffer : "sfpolicesiren";

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

// oh look, more stuff aaron wanted .-.
HOOK_SET(0x45DA00, _Func<int>, $lvlAiMap_GetRoom);
HOOK_SET(0x45D720, _Func<void>, $lvlAiMap_SetRoad); // lvlAiMap::SetRoad(class lvlSDL const *, int, bool)
HOOK_SET(0x45D860, _Func<uint>, $lvlAiMap_GetNumRoads);
HOOK_SET(0x45DA50, _Func<int>, $lvlAiMap_GetNumRooms);

// no error checking or resetting done
int lvl_aiRoad = 0;

int lvl_aiRoom = 0;
int lvl_aiRoomId = 0;

// generic handler for propulation stuff
class lvlHandler {
public:
    static void SetAIRoad(const void *lvlSDL, int road, bool p3) {
        lvl_aiRoad = road;

        Warningf("Propulating road %d", road);
        $lvlAiMap_SetRoad(lvlSDL, road, p3);
    };

    static int GetAIRoom(int room) {
        lvl_aiRoom = $lvlAiMap_GetRoom(room);
        lvl_aiRoomId = room;

        return lvl_aiRoom;
    };

    void EnumerateSDL(int p1, void (*iter)(const void *, int, int, int, const ushort *, void *), void *context) {
        int *page = (*getPtr<int**>(this, 0x54))[p1];

        ushort *attributes = *getPtr<ushort*>(page, 0x14);
        ushort attribute;

        do
        {
            attribute = attributes[0];
            ++attributes;

            ushort type = ((attribute >> 3) & 0xF);
            ushort subtype = (attribute & 0x7);

            ushort vertex_count = subtype;

            if (subtype == 0)
            {
                vertex_count = attributes[0];
                ++attributes;
            }

        } while (!(attribute & 0x80));
    }

    static void InvalidCommand(int cmd, void *attrPtr, void *roadPtr) {
        int type = ((cmd >> 3) & 0xF);
        int subtype = (cmd & 0x7);

        // backtrack to the beginning of the attribute
        short *attr = (short*)attrPtr - ((subtype) ? 1 : 2);

        char buf[256] = { NULL };
        
        for (int i = 0, ii = 0; i < 16; i++)
            ii += sprintf(&buf[ii], "%02X ", *((byte*)attr + i));

        Quitf("Road %d / %d in room %d (%d / %d) has invalid command %d (%d) : %x\ndump: %s",
            lvl_aiRoad, $lvlAiMap_GetNumRoads(), lvl_aiRoom, lvl_aiRoomId, $lvlAiMap_GetNumRooms(), type, subtype, attr, buf);
    };

    static void Install() {
        // patches the Quitf call in lvlSDL::Enumerate
        InstallPatch({
            0x8B, 0x45 , 0x10,  // mov eax, [ebp+arg_8]
            0x50,               // push eax     ; roadPtr
            0x57,               // push edi     ; attrPtr
            0x53,               // push ebx     ; cmd
            
            // 0x45BEEE
            0x90,
            0x90, 0x90, 0x90, 0x90, 0x90,
        }, {
            0x45BEE8,
        });

        // invalid command fix
        InstallPatch({
            0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
        }, {
            0x45BE84,
        });

        InstallCallback("lvlAiMap::SetRoad", "Allows for more detailed information when propulating roads.",
            &SetAIRoad, {
                cbHook<CALL>(0x45D70F),
            }
        );

        InstallCallback("lvlAiMap::SetRoad", "Allows for more detailed information when propulating roads.",
            &GetAIRoom, {
                cbHook<CALL>(0x45D76E),
            }
        );

        InstallCallback("lvlSDL::Enumerate", "Hooks a call to Quitf to print out more detailed information.",
            &InvalidCommand, {
                cbHook<CALL>(0x45BEF4),
            }
        );

        /*
        InstallCallback("lvlSDL::Enumerate", "New enumerate function.",
            &EnumerateSDL, {
                cbHook<JMP>(0x45BE50),
            }
        );
        */
    }
};

// HACK HACK HACK!
// Doesn't work well for subway stations :(
bool insideTunnel = false;

class sdlPage16Handler {
public:
    static LPVOID blockPtr; // current block pointer
    static LPVOID attributePtr; // current attribute pointer

    // this MUST clean up the stack, hence the stdcall
    static void __stdcall SetAttributePointer(LPVOID lpBlock) {
        attributePtr = lpBlock;
    }

    void Draw(int p1, unsigned int p2) {
        blockPtr = this;
        reinterpret_cast<sdlPage16*>(this)->Draw(p1, p2);

        // not in a block anymore
        blockPtr = NULL;
        attributePtr = NULL; // lets vglHandler know we're not checking for SDL stuff

        // so hacky
        insideTunnel = false;

        // move to the next room
        ++city_currentRoom;
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

LPVOID sdlPage16Handler::blockPtr;
LPVOID sdlPage16Handler::attributePtr;

class vglHandler {
private:
    static unsigned int GetAdjustedColor(gfxDrawMode drawMode, unsigned int color) {
        if (sdlPage16Handler::blockPtr != NULL)
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

        // don't print certain errors unless specified
        if (!datArgParser::Get("physDebug")) {
            InstallCallback("dgPhysManager::Collide[X]", "Disables physics collision error debugging (use '-physDebug' to enable).",
                &NullSub, {
                    cbHook<CALL>(0x469A20), // ; 'CollideInstances: Attempting to collide instances without bounds'
                    cbHook<CALL>(0x4692C5), // ; 'dgPhysManager::CollideProbe : instance has no bound'
                    cbHook<CALL>(0x469B24), // ; 'dgPhysManager::CollideTerrain - entry in room 0'
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
        InstallHandler<StackHandler>("Stack information");

        InstallHandler<gfxPipelineHandler>("gfxPipeline");
        InstallHandler<memSafeHeapHandler>("memSafeHeap");
        
        InstallHandler<datCallbackExtensionHandler>("datCallback Extensions");

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

        InstallHandler<lvlHandler>("Propulator");
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
