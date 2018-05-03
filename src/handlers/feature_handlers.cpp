#include "feature_handlers.h"
#include "..\logfile.h"

using namespace MM2;

static init_handler g_feature_handlers[] = {
    /*
        Initialize the really important handlers
    */

    CreateHandler<gfxPipelineHandler>("gfxPipeline"),
    CreateHandler<memSafeHeapHandler>("memSafeHeap"),

    CreateHandler<datCallbackExtensionHandler>("datCallback Extensions"),

    /*
        Initialize the rest of the handlers
        Order doesn't really matter, just whatever looks neat
    */

    CreateHandler<asCullManagerHandler>("asCullManager"),

    CreateHandler<cityLevelHandler>("cityLevel"),

    CreateHandler<BridgeFerryHandler>("gizBridge/gizFerry"),

    CreateHandler<gizFerryHandler>("gizFerry"),
    CreateHandler<gizParkedCarMgrHandler>("gizParkedCarMgr"),

    CreateHandler<mmDashViewHandler>("mmDashView"),
    CreateHandler<mmDirSndHandler>("mmDirSnd"),
    CreateHandler<mmGameHandler>("mmGame"),
    CreateHandler<mmGameMusicDataHandler>("mmGameMusicData"),

    CreateHandler<vehCarAudioContainerHandler>("vehCarAudioContainer"),
    CreateHandler<vehPoliceCarAudioHandler>("vehPoliceCarAudio"),

    CreateHandler<lvlHandler>("Propulator"),
    CreateHandler<sdlPage16Handler>("sdlPage16"),
    CreateHandler<vglHandler>("VGL drawing"),

    CreateHandler<StreamHandler>("Stream"),
    CreateHandler<TextureVariantHandler>("Texture variants"),

    CreateHandler<PUMainHandler>("PUMain"),
};

// ==========================
// Pointer hooks
// ==========================

ageHook::Type<float> obj_NoDrawThresh       ( 0x5C571C ); // default: 300.0
ageHook::Type<float> obj_VLowThresh         ( 0x5C6658 ); // default: 200.0
ageHook::Type<float> obj_LowThresh          ( 0x5C665C ); // default: 100.0
ageHook::Type<float> obj_MedThresh          ( 0x5C6660 ); // default: 40.0

ageHook::Type<float> sdl_VLowThresh         ( 0x5C5708 );  // default: 300.0
ageHook::Type<float> sdl_LowThresh          ( 0x5C570C );  // default: 100.0
ageHook::Type<float> sdl_MedThresh          ( 0x5C5710 );  // default: 50.0

ageHook::Type<int> timeOfDay                ( 0x62B068 );

ageHook::Type<int> vehCar_bHeadlights       ( 0x627518 );

/*
    asCullManagerHandler
*/

static ConfigValue<int> cfgMaxCullables     ("MaxCullables",            1024);
static ConfigValue<int> cfgMaxCullables2D   ("MaxCullables2D",          256);

void asCullManagerHandler::Init(int maxCullables, int maxCullables2D) {
    maxCullables = cfgMaxCullables;
    maxCullables2D = cfgMaxCullables2D;

    LogFile::Format("[asCullManager::Init]: Max Cullables = %d, %d\n", maxCullables, maxCullables2D);

    ageHook::Thunk<0x4A1290>::Call<void>(this, maxCullables, maxCullables2D);
}

void asCullManagerHandler::Install() {
    InstallCallback("asCullManager::Init", "Increases max cullables.",
        &Init, {
            cbHook<CALL>(0x401D5C),
        }
    );
}

/*
    cityLevelHandler
*/

const double cosNum = 1.570796;

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
        (float)((char)((value & 0xFF0000) >> 16) / 256.0f),
        (float)((char)((value & 0xFF00) >> 8) / 256.0f),
        (float)((char)((value & 0xFF)) / 256.0f),
    };
}

/* PSDL shading fix */

unsigned int vglColor;
unsigned int vglCalculatedColor = 0xFFFFFFFF;

Vector3 vglAmbient;
Vector3 vglKeyColor;
Vector3 vglFill1Color;
Vector3 vglFill2Color;
Vector3 vglShadedColor;

ColorARGB vglResultColor;

// aaron wanted this so badly
int city_numRooms = 0;
int city_currentRoom = 0;

void cityLevelHandler::DrawRooms(const LPVOID viewport, unsigned int p2, LPVOID roomRecs, int numRooms)
{
    city_numRooms = numRooms;
    city_currentRoom = 0;

    $::cityLevel::DrawRooms(this, viewport, p2, roomRecs, numRooms);
}

// jumped to at the end of cityLevel::Update
void cityLevelHandler::PostUpdate() {
    // update the SDL lighting
    sdlCommon::UpdateLighting();

    // update our shaded lighting
    // TODO: fix lighting quality not being taken into account (harder than it sounds)
    auto timeWeather = $::timeWeathers.ptr(timeOfDay);

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
void cityLevelHandler::SetObjectDetail(int lod) {
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
        { 600.0f, 300.0f, 175.0f, 90.0f }, // Very high (NEW)
    };

    // Using temporary variables so we don't need to constantly access pointers

    float objNoDrawThresh = lodLevels[lod][0]; // VL: <VLowThresh> - <NoDrawThresh>
    float objVLowThresh = lodLevels[lod][1]; // L: <LowThresh> - <VLowThresh>
    float objLowThresh = lodLevels[lod][2]; // M: <MedThresh> - <LowThresh>
    float objMedThresh = lodLevels[lod][3]; // H: 0.0 - <MedThresh>

    obj_NoDrawThresh = objNoDrawThresh;
    obj_VLowThresh = objVLowThresh;
    obj_LowThresh = objLowThresh;
    obj_MedThresh = objMedThresh;

    // By default, the game doesn't set these values based on the detail level
    // They are hardcoded to what is considered 'High' by default,
    // however this is now known as 'Medium' (lod = 1; see above)
    //
    // 'Medium' and below (default 'High') uses the defaults.
    float sdlVLowThresh = (lod > 1) ? (objVLowThresh + 100.0f) : 300.0f;
    float sdlLowThresh = (lod > 1) ? (objLowThresh + 25.0f) : 100.0f;
    float sdlMedThresh = (lod > 1) ? (objMedThresh + 10.0f) : 50.0f;

    sdl_VLowThresh = sdlVLowThresh;
    sdl_LowThresh = sdlLowThresh;
    sdl_MedThresh = sdlMedThresh;

    LogFile::Format("[cityLevel::SetObjectDetail]: '%s'\n"
        " - OBJ { %.4f, %.4f, %.4f, %.4f }\n"
        " - SDL { %.4f, %.4f, %.4f }\n",
        lodLevelNames[lod],
        objNoDrawThresh, objVLowThresh, objLowThresh, objMedThresh,
        sdlVLowThresh, sdlLowThresh, sdlMedThresh);
}

void cityLevelHandler::Install() {
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

/*
    gfxPipelineHandler
*/

void gfxPipelineHandler::gfxApplySettings(void) {
    gfxInterface *gfxInterface = gfxInterfaces[gfxInterfaceChoice];

    auto deviceType = gfxInterface->DeviceType;

    enableHWTnL = (deviceType == gfxDeviceType::HardwareWithTnL);
    useBlade = (useSoftware = (deviceType == gfxDeviceType::Software));

    useInterface = gfxInterfaceChoice;
}

bool gfxPipelineHandler::HandleKeyPress(DWORD vKey)
{
    if (MM2Lua::IsEnabled())
    {
        // Inform Lua of any changes beforehand
        MM2Lua::OnKeyPress(vKey);
    }

    switch (vKey) {
        // '~'
        case VK_OEM_2: case VK_OEM_3:
        // '`'
        case VK_OEM_8:
        {
            // tell the game to open a chat box,
            // and then use a local variable to check if it's open
            mmGameManager *mgr = mmGameManager::Instance;
            auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;

            if (gamePtr != NULL)
            {
                auto popup = gamePtr->getPopup();

                if (popup != NULL) {
                    // don't try opening it again if it's already open
                    if (popup->IsEnabled() && g_bConsoleOpen)
                        return true;

                    popup->ProcessChat();
                    g_bConsoleOpen = true;
                }
            }
        } return true;

        case VK_F7: {
            // TODO: make this a separate plugin
            // toggle vehicle headlights
            //vehCar_bHeadlights = !vehCar_bHeadlights;
        } return true;

        case VK_F8: {
            if (HookConfig::Read()) {
                LogFile::WriteLine("Configuration settings reloaded successfully.");
            }
        } return true;
    }

    return false;
}

LRESULT APIENTRY gfxPipelineHandler::gfxWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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

void gfxPipelineHandler::gfxWindowMove(bool isOpen) {
    HDC hDC = GetDC(NULL);
    int screenWidth = GetDeviceCaps(hDC, HORZRES);
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

void gfxPipelineHandler::gfxWindowUpdate(bool isOpen) {
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

void gfxPipelineHandler::SetRes(int width, int height, int cdepth, int zdepth, bool parseArgs) {
    if (datArgParser::Get("ref")) {
        useSoftware = 1;
        useReference = 1;
    }
    else if (datArgParser::Get("blade") || datArgParser::Get("bladed")) {
        useSoftware = 1;
        useBlade = 1;
    }
    else if (datArgParser::Get("swage")) {
        useSoftware = 1;
        useAgeSoftware = 1;
    }
    else if (datArgParser::Get("sw")) {
        useSoftware = 1;
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
    }
    else {
        datArgParser::Get("display", 0, &useInterface);
    }
    if (datArgParser::Get("single")) {
        pageFlip = 0;
    }

    if (datArgParser::Get("window") || datArgParser::Get("windowed")) {
        inWindow = 1;
    }
    else if (datArgParser::Get("fs") || datArgParser::Get("fullscreen")) {
        inWindow = 0;
    }

    int bitDepth = 0;
    if (datArgParser::Get("bpp", 0, &bitDepth) || datArgParser::Get("bitdepth", 0, &bitDepth)) {
        cdepth = bitDepth;
        zdepth = bitDepth;
    }
    else {
        datArgParser::Get("cdepth", 0, &cdepth);
        datArgParser::Get("zdepth", 0, &zdepth);
    }

    // We don't want to set the width/height if we are in a menu, it just fucks it up
    if (MMSTATE->SplashScreen != 0) {
        if (datArgParser::Get("max")) {
            HDC hDC = GetDC(NULL);
            width = GetDeviceCaps(hDC, HORZRES);
            height = GetDeviceCaps(hDC, VERTRES);
            ReleaseDC(0, hDC);
        }
        else {
            datArgParser::Get("width", 0, &width);
            datArgParser::Get("height", 0, &height);
        }

        // datArgParser::Get("width",  0, &width);
        // datArgParser::Get("height", 0, &height);
    }

    useSysMem = useSoftware;

    window_iWidth = width;
    window_iHeight = height;

    window_fWidth = float(width);
    window_fHeight = float(height);

    window_ColorDepth = cdepth;
    window_ZDepth = zdepth;

    gfxTexture::sm_Allow32 = (cdepth == 32);

    LogFile::Format("[gfxPipeline::SetRes]: 32-bit textures are%s allowed.\n", (gfxTexture::sm_Allow32) ? "" : "n't");
    LogFile::Format("[gfxPipeline::SetRes]: %dx%dx%dx%d\n", width, height, cdepth, zdepth);

    if (lpDD)
    {
        if (inWindow)
        {
            gfxWindowMove(true);
            gfxWindowUpdate(true);
        }
        else {
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

    ioMouse::InvWidth = (1.0f / window_fWidth);
    ioMouse::InvHeight = (1.0f / window_fHeight);
}

void gfxPipelineHandler::gfxWindowCreate(LPCSTR lpWindowName) {
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

void gfxPipelineHandler::gfxSetTexReduceSize(int) {
    static const int gfxReduceSizes[4] = {
        64,     // Low
        128,    // Medium
        256,    // High
        0,      // Very High (unlimited)
    };

    gfxTexReduceSize = gfxReduceSizes[*gfxTexQuality];
}

void gfxPipelineHandler::Install() {
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

/*
    sdlPage16Handler
*/

// HACK HACK HACK!
// Doesn't work well for subway stations :(
bool insideTunnel = false;

LPVOID sdlPage16Handler::blockPtr; // current block pointer
LPVOID sdlPage16Handler::attributePtr; // current attribute pointer

void sdlPage16Handler::SetAttributePointer(LPVOID lpBlock) {
    attributePtr = lpBlock;
}

void sdlPage16Handler::Draw(int p1, unsigned int p2) {
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

void sdlPage16Handler::InvalidCmd(LPCSTR, int attr, int subtype)
{
    char buf[256] = { NULL };
    int idx = 0;

    for (int i = 0; i < 16; i++)
        idx += sprintf(&buf[idx], "%02X ", *((byte*)attributePtr + i));

    Quitf("Invalid cmd %d (%d) : [%d / %d] : %x\ndump: %s", attr, subtype, city_currentRoom, city_numRooms, attributePtr, buf);
}

void sdlPage16Handler::Install() {
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

/*
    vglHandler
*/

unsigned int GetAdjustedColor(gfxDrawMode drawMode, unsigned int color) {
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
            }
            else {
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

void vglHandler::vglBeginShaded(gfxDrawMode drawMode, int p1) {
    // Save current vgl color
    vglColor = *vglCurrentColor;

    vglCalculatedColor = GetAdjustedColor(drawMode, vglColor);
    vglCurrentColor = vglCalculatedColor;

    vglBegin(drawMode, p1);
}

void vglHandler::vglEndShaded(void) {
    // restore color
    vglCurrentColor = vglColor;

    vglEnd();
}

void vglHandler::Install() {
    LogFile::WriteLine(" - Installing shading fix...");

    auto_ptr vglBeginCB = &vglBeginShaded;
    auto_ptr vglEndCB = &vglEndShaded;

    Installf(" - vglBeginCB: %08X\n", vglBeginCB);
    Installf(" - vglEndCB: %08X\n", vglEndCB);

    // use a custom struct to make the process easier
    // this allows us to have an entry representing each "frame" (vglBegin/vglEnd)
    // and cuts down on the amount of time it takes to add a new entry
    struct vgl_pair {
        unsigned int begin; // vglBegin
        unsigned int end;   // vglEnd
    };

    std::initializer_list<vgl_pair> vglCBs = {
        /* ------------ sdlPage16::Draw ------------- */
        { 0x448424, 0x4485D3 },{ 0x448697, 0x448B82 },{ 0x448903, 0x448D8C },{ 0x448BFD, 0x448FB7 },
        { 0x448DE4, 0x449219 },{ 0x44902A, 0x449480 },{ 0x4492A4, 0x44963E },{ 0x4494C3, 0x44983C },
        { 0x4496A5, 0x4499D4 },{ 0x44986B, 0x449BAA },{ 0x449A13, 0x449D42 },{ 0x449BD9, 0x449F5A },
        { 0x449D82, 0x44A146 },{ 0x449F67, 0x44A3F8 },{ 0x44A21C, 0x44A5BF },{ 0x44A444, 0x44A7C0 },
        { 0x44A629, 0x44A958 },{ 0x44A7EF, 0x44AB2E },{ 0x44A997, 0x44ACC6 },{ 0x44AB5D, 0x44AEBC },
        { 0x44AD06, 0x44B083 },{ 0x44AECA, 0x44B23D },{ 0x44B0EC, 0x44B394 },{ 0x44B24B, 0x44B531 },
        { 0x44B3B6, 0x44B6E1 },{ 0x44B557, 0x44B895 },{ 0x44B6F3, 0x44BA7C },{ 0x44B8F1, 0x44BC03 },
        { 0x44BA8A, 0x44BE8E },{ 0x44BC29, 0x44C118 },{ 0x44BE9C, 0x44C3EA },{ 0x44C136, 0x44C638 },
        { 0x44C40C, 0x44C77A },{ 0x44C64A, 0x44C989 },{ 0x44C7C0, 0x44CC44 },{ 0x44CAD6, 0x44CE63 },
        { 0x44CCF5, 0x44D04E },{ 0x44CF6D, 0x44D403 },{ 0x44D0D4, 0x44D780 },{ 0x44D5F7, 0x44D8E9 },
        { 0x44D789, 0x44E014 },{ 0x44DC55, 0x44E131 },{ 0x44E050, 0x44E22C },{ 0x44E14B, 0x44E661 },
        { 0x44E2A3, 0x44E785 },{ 0x44E69D, 0x44E886 },{ 0x44E79E, 0x44EB82 },{ 0x44EAA0, 0x44EDC3 },
        { 0x44EBA5, 0x44F0B9 },{ 0x44EFD0, 0x44F316 },{ 0x44F0DC, 0x44F64C },{ 0x44F588, 0x44FB9D },
        { 0x44F7E2, 0x44FD30 },{ 0x44FC1E, 0x44FE4E },{ 0x44FDD4, 0x44FFB3 },{ 0x44FF10, 0x450162 },
        { 0x450085, 0x450390 },{ 0x450269, 0x45078C },
        /* ------------------------------------------- */
        { 0x443B9D, 0x443DCC }, // dgRoadDecalInstance
        { 0x57AC4A, 0x57AD41 }, // ped LODs
    };

    // mostly copied from InstallCallback
    for (auto pair : vglCBs)
    {
        auto begin = pair.begin;
        auto end = pair.end;

        InstallCallback(vglBeginCB, { begin, CALL });
        InstallCallback(vglEndCB, { end,   CALL });

        Installf("   - { vglBegin: %08X, vglEnd: %08X }\n", begin, end);
    }
}

/*
    mmGameMusicDataHandler
*/

char defaultCityAmbienceFile[64] = "londonambience";

bool mmGameMusicDataHandler::LoadAmbientSFX(LPCSTR name) {
    string_buf<80> buffer("%sambience", MMSTATE->CityName);
    
    LPCSTR szAmbientSFX = (datAssetManager::Exists("aud\\dmusic\\csv_files", buffer, "csv")) ? buffer : defaultCityAmbienceFile;

    LogFile::Format("AmbientSFX: %s\n", szAmbientSFX);

    return reinterpret_cast<mmGameMusicData *>(this)->LoadAmbientSFX(szAmbientSFX);
}

void mmGameMusicDataHandler::Install() {
    HookConfig::GetProperty("DefaultCityAmbienceFile", defaultCityAmbienceFile);

    InstallCallback("mmGameMusicData::LoadAmbientSFX", "Allows for custom ambient effects in addon cities.",
        &LoadAmbientSFX, {
            cbHook<CALL>(0x433F93),
        }
    );
}


/*
    vehCarAudioContainerHandler
*/

char defaultCitySirenFile[64] = "sfpolicesiren";

void vehCarAudioContainerHandler::SetSirenCSVName(LPCSTR name) {
    string_buf<80> buffer("%spolicesiren", MMSTATE->CityName);
    
    LPCSTR szSirenName = (datAssetManager::Exists("aud\\cardata\\player", buffer, "csv")) ? buffer : defaultCitySirenFile;

    LogFile::Format("SirenCSVName: %s\n", szSirenName);

    vehCarAudioContainer::SetSirenCSVName(szSirenName);
}

void vehCarAudioContainerHandler::Install() {
    HookConfig::GetProperty("DefaultCitySirenFile", defaultCitySirenFile);

    InstallCallback("vehCarAudioContainer::SetSirenCSVName", "Allows for custom sirens in addon cities.",
        &SetSirenCSVName, {
            cbHook<CALL>(0x412783),
            cbHook<CALL>(0x412772),
        }
    );
}


/*
    vehPoliceCarAudioHandler
*/

void vehPoliceCarAudioHandler::InitSirenAudio(vehCarSim *a1, vehCarDamage *a2, LPCSTR basename, LPCSTR sirenCsvFile, bool a5) {
    string_buf<80> buffer("%s_siren", basename);

    // override the global city one if a custom siren exists
    if (datAssetManager::Exists("aud\\cardata\\player", buffer, "csv"))
        sirenCsvFile = buffer;

    // vehPoliceCarAudio::Init
    ageHook::Thunk<0x4D46F0>::Call<void>(this, a1, a2, basename, sirenCsvFile, a5);
}

void vehPoliceCarAudioHandler::Install() {
    InstallCallback("vehPoliceCarAudio::Init", "Allows vehicles to use their own custom sirens instead of default ones for each city.",
        &InitSirenAudio, {
            cbHook<CALL>(0x4D44A3),
        }
    );
}

/*
    datCallbackExtensionHandler
*/

void datCallbackExtensionHandler::Install() {
    InstallPatch("datCallback Fix 1", { 0x00, 0x00, 0x00, 0x40 }, { 0x4C7A5B + 2, 0x4C7AC8 + 2, 0x4C7B70 + 1, 0x4C7BA6 + 1 });
    InstallPatch("datCallback Fix 2", { 0x00, 0x00, 0x00, 0x80 }, { 0x4C7A90 + 2, 0x4C7AFB + 2, 0x4C7B7E + 1, 0x4C7BB4 + 1 });
    InstallPatch("datCallback Fix 3", { 0x00, 0x00, 0x00, 0xC0 }, { 0x4C7AB0 + 2, 0x4C7B2B + 2, 0x4C7B90 + 1, 0x4C7BC9 + 1, 0x4C7B61 + 1 });
    InstallPatch("datCallback Fix 4", { 0xFF, 0xFF, 0xFF, 0x3F }, { 0x4C7B5B + 2 });
    InstallPatch("datCallback Code Cave", { 0xFF, 0xE1 }, { 0x4C7BE3 });
}

/*
    lvlHandler
*/

// no error checking or resetting done
int lvl_aiRoad = 0;

int lvl_aiRoom = 0;
int lvl_aiRoomId = 0;

bool bRoadDebug = false;

static ConfigProperty cfgRoadDebug("RoadDebug", "roadDebug");

// generic handler for propulation stuff
void lvlHandler::SetAIRoad(const lvlSDL *lvlSDL, int road, bool p3) {
    lvl_aiRoad = road;

    if (bRoadDebug)
        Warningf("Propulating road %d", road);

    lvlAiMap::SetRoad(lvlSDL, road, p3);
};

int lvlHandler::GetAIRoom(int room) {
    lvl_aiRoom = lvlAiMap::GetRoom(room);
    lvl_aiRoomId = room;

    return lvl_aiRoom;
};

void lvlHandler::EnumerateSDL(int p1, SDLIteratorCB iter, void *context) {
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

void lvlHandler::InvalidCommand(int cmd, void *attrPtr, void *roadPtr) {
    int type = ((cmd >> 3) & 0xF);
    int subtype = (cmd & 0x7);

    // backtrack to the beginning of the attribute
    short *attr = (short*)attrPtr - ((subtype) ? 1 : 2);

    char buf[256] = { NULL };

    for (int i = 0, ii = 0; i < 16; i++)
        ii += sprintf(&buf[ii], "%02X ", *((byte*)attr + i));

    Quitf("Road %d / %d in room %d (%d / %d) has invalid command %d (%d) : %x\ndump: %s",
        lvl_aiRoad, lvlAiMap::GetNumRoads(), lvl_aiRoom, lvl_aiRoomId, lvlAiMap::GetNumRooms(), type, subtype, attr, buf);
};

void lvlHandler::Install() {
    cfgRoadDebug.Get(bRoadDebug);

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

    if (datArgParser::Get("sdlEnumTest"))
    {
        InstallCallback("lvlSDL::Enumerate", "New enumerate function.",
            &EnumerateSDL, {
                cbHook<JMP>(0x45BE50),
            }
        );
    }
    else
    {
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

        InstallCallback("lvlSDL::Enumerate", "Hooks a call to Quitf to print out more detailed information.",
            &InvalidCommand, {
                cbHook<CALL>(0x45BEF4),
            }
        );
    }   
}

/*
    memSafeHeapHandler
*/

int g_heapSize = 128;

static ConfigProperty cfgHeapSize("HeapSize", "heapsize");

void memSafeHeapHandler::Init(void *memAllocator, unsigned int heapSize, bool p3, bool p4, bool checkAlloc) {
    // fast way of expanding to the proper size
    // same as ((g_heapSize * 1024) * 1024)
    heapSize = (g_heapSize << 20);

    LogFile::Format("[memSafeHeap::Init]: Allocating %dMB heap (%d bytes)\n", g_heapSize, heapSize);
    return ageHook::Thunk<0x577210>::Call<void>(this, memAllocator, heapSize, p3, p4, checkAlloc); //TODO: move to own class
}

void memSafeHeapHandler::Install() {
    cfgHeapSize.Get(g_heapSize);

    InstallCallback("memSafeHeap::Init", "Adds '-heapsize' parameter that takes a size in megabytes. Defaults to 128MB.",
        &Init, {
            cbHook<CALL>(0x4015DD),
        }
    );
}

/*
    mmGameHandler
*/

void mmGameHandler::SendChatMessage(char *message) {
    if (g_bConsoleOpen) {
        if (MM2Lua::IsEnabled())
            MM2Lua::SendCommand(message);

        // return normal chatbox behavior
        g_bConsoleOpen = false;
    }
    else {
        LogFile::Format("Got chat message: %s\n", message);
    }
}

ageHook::Type<float> wheelFriction(0x5CF6B8);

void mmGameHandler::InitWeather(void) {
    // reset the wheel friction in case it got changed
    wheelFriction = 1.0f;
    get<mmGame>()->InitWeather();
}

void mmGameHandler::Install() {
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

    InstallCallback("mmGame::InitWeather", "Fixes a bug where the rainy weather effects do not get reset.",
        &InitWeather, {
            cbHook<CALL>(0x4131C0), // mmGame::Init
        }
    );
}

/*
    mmDirSndHandler
*/

mmDirSnd* mmDirSndHandler::Init(ulong sampleRate, bool enableStero, int p3, int volume, char *deviceName, short p6, short p7) {
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

void mmDirSndHandler::Install() {
    InstallCallback("mmDirSnd::Init", "Fixes no sound issue on startup.",
        &Init, {
            cbHook<CALL>(0x51941D),
        }
    );
}

/*
    gizFerryHandler
*/

void gizFerryHandler::SetSpeed(float value) {
    value *= cfgFerrySpeedMultiplier;

    ageHook::Thunk<0x579520>::Call<void>(this, value);
}

void gizFerryHandler::Install() {
    InstallCallback("gizFerry:SetSpeed", "Allows a speed modifier to be applied to ferry speeds.",
        &SetSpeed, {
            cbHook<CALL>(0x579951), // gizFerryMgr::ApplyTuning
        }
    );

}

/*
    gizParkedCarMgrHandler
*/

static ConfigValue<bool> cfgDynamicParkedCarDensity("DynamicParkedCarDensity", true);

void gizParkedCarMgrHandler::EnumeratePath(LPCSTR a1, const Matrix34* a2, bool a3) {
    int oldRandomSeed = gRandSeed;
    float rand = ageHook::StaticThunk<0x4BBE30>::Call<float>();

    if (dgStatePack::Instance->TrafficDensity > rand) {
        gRandSeed = oldRandomSeed;
        ageHook::StaticThunk<0x579BD0>::Call<void>(a1, a2, a3); //gizParkedCarMgr_EnumeratePath
    }
}

void gizParkedCarMgrHandler::Install() {
    if (cfgDynamicParkedCarDensity) {
        InstallCallback("gizParkedCarMgr::Init", "Scales parked cars with traffic density.",
            &EnumeratePath, {
                cbHook<PUSH>(0x579B80),
            }
        );
    }
}

/*
    BridgeFerryHandler
*/
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

void BridgeFerryHandler::Cull(int lod) {
    // TODO: Make this do something?
}

void BridgeFerryHandler::Draw(int lod) {
    reinterpret_cast<dgBangerInstance*>(this)->Draw(lod);
}

void BridgeFerryHandler::Install() {
    // revert bridges/ferries to how they were in the betas
    InstallCallback("Bridge/Ferry: Cull", "Quick'n dirty fix for fullbright bridges/ferries.",
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

/*
    mmDashViewHandler
*/

static Matrix34 sm_DashOffset;

static ConfigValue<bool> cfgEnableHeadBobbing       ("EnableHeadBobbing",       true);

static ConfigValue<float> cfgHeadBobOffsetScaleY    ("HeadBobOffsetScaleY",     0.0125f);
static ConfigValue<float> cfgHeadBobOffsetScaleZ    ("HeadBobOffsetScaleZ",     0.0125f);

static ConfigValue<float> cfgHeadBobSteeringScale   ("HeadBobSteeringScale",    0.001875f);

static ConfigValue<float> cfgHeadBobVelocityScaleY  ("HeadBobVelocityScaleY",   0.00075f);
static ConfigValue<float> cfgHeadBobVelocityScaleZ  ("HeadBobVelocityScaleZ",   0.00725f);

static ConfigValue<float> cfgHeadBobMultiplierY     ("HeadBobMultiplierY",      1.0f);
static ConfigValue<float> cfgHeadBobMultiplierZ     ("HeadBobMultiplierZ",      1.0f);

void mmDashViewHandler::UpdateCS() {
    auto dashCam = getPtr<Matrix34>(this, 0x18);
    auto player = *getPtr<mmPlayer *>(this, 0x80);

    auto car = player->getCar();
    auto carModel = car->getModel();

    sm_DashOffset = carModel->GetMatrix(sm_DashOffset);

    auto steering = *getPtr<float>(player, 0x2264);
    auto wheelFact = *getPtr<float>(this, 0x400);

    auto bodyRoll = -(steering * wheelFact) * cfgHeadBobSteeringScale;

    auto velocity = carModel->GetVelocity();

    auto velY = bodyRoll + (velocity->Y * cfgHeadBobVelocityScaleY);
    auto velZ = (velocity->Z - (velocity->Y + velocity->X)) * -cfgHeadBobVelocityScaleZ;

    auto headBobY = ((sm_DashOffset.m33 - dashCam->m33) * -cfgHeadBobOffsetScaleY) + velY;
    auto headBobZ = ((sm_DashOffset.m34 - dashCam->m34) * -cfgHeadBobOffsetScaleZ) * velZ;

    dashCam->m33 += (headBobY * cfgHeadBobMultiplierY);
    dashCam->m34 += (headBobZ * cfgHeadBobMultiplierZ);

    ageHook::Thunk<0x4A3370>::Call<void>(this);
}

void mmDashViewHandler::FileIO(datParser* parser) {
    //call original FileIO
    ageHook::Thunk<0x4315D0>::Call<void>(this, parser);

    //add missing things
    parser->AddValue("MaxSpeed", getPtr<float>(this, 0x5D0), 1);
    parser->AddValue("MaxRPM", getPtr<float>(this, 0x5D4), 1);
}

void mmDashViewHandler::Install() {
    if (cfgEnableHeadBobbing) {
        InstallCallback("mmDashView::Update", "Allows for a custom implementation of head-bobbing in dashboards.",
            &UpdateCS, {
                cbHook<CALL>(0x430F87), // replaces call to asLinearCS::Update
            }
        );
    }

    // rv6 stuff
    InstallVTableHook("mmDashView::FileIO", &FileIO, {
        0x5B0D90
    });
}

/*
    StreamHandler
*/

static ConfigValue<bool> cfgUseModsFolder("UseModsFolder", true);

Stream * StreamHandler::Open(const char *filename, bool readOnly)
{
    const coreFileMethods *fileMethods = (readOnly) ? Stream::sm_DefaultOpenMethods : Stream::sm_DefaultCreateMethods;

    string_buf<MAX_PATH> modFilename(".\\mods\\%s", filename);
    
    if (file_exists(modFilename))
    {
        LogFile::Format("[StreamHandler::Open]: Using '%s' file from mods directory (readOnly=%s)\n", filename, bool_str(readOnly));

        // don't let the name fool you, it's just non-zip file methods ;)
        fileMethods = Stream::sm_DefaultCreateMethods;
        
        // override filename with new path
        filename = modFilename;
    }

    int handle = fileMethods->open(filename, readOnly);

    if (handle == -1)
        return nullptr;

    // Stream::AllocStream
    return ageHook::StaticThunk<0x4C98D0>::Call<Stream *>(filename, handle, fileMethods);
}

void StreamHandler::Install()
{
    if (cfgUseModsFolder) {
        InstallCallback("Stream::Open", "Allows for files to be overridden using a mods folder.",
            &Open, {
                cbHook<JMP>(0x4C99C0), // Stream::Open(const char *, bool)
            }
        );
    }
}

/*
    TextureVariantHandler
*/

static gfxImage * (*DefaultLoadImage)(const char *, bool);
static gfxImage * (*DefaultPrepareImage)(const char *, bool);

ageHook::Type<bool> EnableTextureVariantHandler(0x6276EC);
ageHook::Type<bool> AllowDesaturatedTextureVariants(0x6276ED);

static bool TryLoadTexVariant(const char *textureName, const char *variant, bool mipmaps, gfxImage **pgfxImage)
{
    string_buf<64> textureVariant("%s_%s", textureName, variant);

    gfxImage *variantTex = DefaultLoadImage(textureVariant, mipmaps);

    if (variantTex != nullptr) {
        //Warningf("[LoadTextureVariant]: Using '%s' variant for texture '%s'", variant, textureName);
        *pgfxImage = variantTex;

        return true;
    }

    return false;
}

static bool UseNightTexturesInEvening = true;

struct variant_info {
    const char *suffix;

    int timeOfDay;
    int weather;

    bool canDesaturate;
} tex_variants[] = {
    /*
        texture variants, sorted by priority
    */
    { "nifa",   3,  3, false }, // rainy night
    { "fa",    -1,  3, true },  // rainy
    { "ni",     3, -1, false }, // night
    { NULL },
};

gfxImage * TextureVariantHandler::LoadTextureVariant(const char *textureName, bool mipmaps)
{
    if (EnableTextureVariantHandler)
    {
        int timeOfDay = dgStatePack::Instance->TimeOfDay;
        int weatherType = dgStatePack::Instance->WeatherType;

        bool useEveningHack = (UseNightTexturesInEvening && (timeOfDay == 2));

        if (useEveningHack)
            timeOfDay = 3;

        gfxImage *variantTex = nullptr;
        
        for (variant_info *variant = tex_variants; variant->suffix != nullptr; variant++)
        {
            if ((variant->timeOfDay == -1)
                || (variant->timeOfDay == timeOfDay))
            {
                if ((variant->weather == -1)
                    || (variant->weather == weatherType))
                {
                    AllowDesaturatedTextureVariants = variant->canDesaturate;

                    if (TryLoadTexVariant(textureName, variant->suffix, mipmaps, &variantTex))
                        return variantTex;
                }
            }
        }

        // desaturate for night-time if needed
        AllowDesaturatedTextureVariants = true;
    }

    return DefaultLoadImage(textureName, mipmaps);
}

gfxImage * TextureVariantHandler::PrepareTextureVariant(const char *textureName, bool mipmaps)
{
    gfxImage *result = DefaultPrepareImage(textureName, mipmaps);

    if (EnableTextureVariantHandler
        && AllowDesaturatedTextureVariants)
    {
        int timeOfDay = dgStatePack::Instance->TimeOfDay;

        if (UseNightTexturesInEvening && (timeOfDay == 2))
            timeOfDay = 3;

        if (timeOfDay == 3)
        {
            for (gfxImage *image = result; image != nullptr; image = image->Next) {
                // DesaturateTextureVariant
                ageHook::StaticThunk<0x442FB0>::Call<void>(image);
            }
        }
    }

    return result;
}

void TextureVariantHandler::InstallTextureVariantHandler()
{
    if (DefaultLoadImage == nullptr) {
        DefaultLoadImage = gfxLoadImage;
        gfxLoadImage = LoadTextureVariant;
    }

    if (DefaultPrepareImage == nullptr) {
        DefaultPrepareImage = gfxPrepareImage;
        gfxPrepareImage = PrepareTextureVariant;
    }
}

void TextureVariantHandler::Install()
{
    InstallCallback(InstallTextureVariantHandler, {
            cbHook<CALL>(0x401599),
        }, "Installs new texture variant handler."
    );

    HookConfig::GetProperty("NightTexturesInEvening", UseNightTexturesInEvening);

    if (UseNightTexturesInEvening)
    {
        LogFile::WriteLine("Installing evening patches...");

        // aiTrafficLightInstance::DrawGlow
        InstallPatch({ 1 }, {
            0x53CABC + 3
        });
        
        /*
            mmGame::InitWeather patches
        */

        // minimum time of day for night checks
        InstallPatch({ 2 }, {
            0x41338E + 2,
            0x4133BD + 2,
        });

        // jnz -> jb
        InstallPatch({ 0x72 }, {
            0x41339D,
        });

        // jz -> jge
        InstallPatch({ 0x7D }, {
            0x4133CA,
        });
    }
}

/*
    PUMainHandler
*/

static float PauseMenuAdjustment = 0.0f;
static float ReplayButtonOffsetY = 0.0f;

class PUMenuHook : public UIMenu {
public:
    UIButton * AddPauseButton(int id, LocString *text, float offsetX, float offsetY, float width, float height, int a7, int a8, datCallback cb, int a10) {
        offsetY += PauseMenuAdjustment;

        Warningf("**** PUMenuHook::AddPauseButton(%d, \"%s\", %.2f, %.2f, %.2f, %.2f, %d, %d, <callback:%08X>, %d) ****",
            id, text, offsetX, offsetY, width, height, a7, a8, cb.ptr(), a10);

        UIButton *result = this->AddButton(id, text, offsetX, offsetY, width, height, a7, a8, cb, a10);

        // 'Restart Race'
        if (id == 10) {
            // adding it here causes crashes,
            // so just reserve space for it and save its position
            PauseMenuAdjustment = 0.12f;
            ReplayButtonOffsetY = offsetY;
        }
        // 'Exit to Windows'
        else if (id == 14)
        {
            // our custom method keeps spacing consistent (no need to adjust offsets)
            UIButton *rplButton = this->AddPauseButton(
                16,
                (LocString *)AngelReadString(465),
                offsetX,
                ReplayButtonOffsetY,
                width,
                height,
                a7,
                a8,
                datCallback::NullCallback,
                0);

            // set the data
            setPtr(this, 0xBC, rplButton);

            if (datArgParser::Get("pudebug"))
            {
                // add 'DEBUG' button
                this->AddPauseButton(
                    15,
                    (LocString *)AngelReadString(454),
                    offsetX,
                    offsetY + 0.00425f,
                    width,
                    height,
                    a7,
                    2,
                    datCallback::NullCallback,
                    0);
            }

            // reset the adjustment value
            PauseMenuAdjustment = 0.0f;
        }

        return result;
    }
};

void PUMainHandler::Install() {
    if (cfgInstantReplay) {
        InstallCallback("PUMain::ctor", "Overrides button placement for the pause menu.",
            &PUMenuHook::AddPauseButton, {
                cbHook<CALL>(0x50A6AE),
                cbHook<CALL>(0x50A712),
                cbHook<CALL>(0x50A776),
                cbHook<CALL>(0x50A7D0),
            }
        );
    }
}

#ifndef FEATURES_DECLARED
#define FEATURES_DECLARED
#endif
