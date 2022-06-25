#include "feature_handlers.h"
#include "lua_drawable.h"
#include <imgui\impl\imgui_impl_age.h>
#include <imgui\impl\imgui_impl_win32.h>

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
    CreateHandler<cityTimeWeatherLightingHandler>("cityTimeWeatherLighting"),

    CreateHandler<BridgeFerryHandler>("gizBridge/gizFerry"),

    CreateHandler<gizFerryHandler>("gizFerry"),
    CreateHandler<gizParkedCarMgrHandler>("gizParkedCarMgr"),

    CreateHandler<mmHudMapFeatureHandler>("mmHudMapFeatureHandler"),
    CreateHandler<mmIconsHandler>("mmIconsHandler"),
    CreateHandler<mmDashViewHandler>("mmDashView"),
    CreateHandler<mmExternalViewHandler>("mmExternalView"),
    CreateHandler<mmDirSndHandler>("mmDirSnd"),
    CreateHandler<mmPlayerHandler>("mmPlayer"),
    CreateHandler<mmGameHandler>("mmGame"),
    CreateHandler<mmGameMusicDataHandler>("mmGameMusicData"),
    CreateHandler<Aud3DObjectManagerHandler>("Aud3DObjectManagerHandler"),
    CreateHandler<mmArrowHandler>("mmArrowHandler"),
    CreateHandler<mmSingleRaceHandler>("mmSingleRace"),
    CreateHandler<mmSingleBlitzHandler>("mmSingleBlitz"),
    CreateHandler<mmSingleCircuitHandler>("mmSingleCircuit"),
    CreateHandler<mmSingleStuntHandler>("mmSingleStunt"),
    CreateHandler<mmSingleRoamHandler>("mmSingleRoam"),

    CreateHandler<dgBangerInstanceHandler>("dgBangerInstance"),
    CreateHandler<ltLensFlareHandler>("ltLensFlare"),
    CreateHandler<ltLightHandler>("ltLight"),

    CreateHandler<vehCarHandler>("vehCar"),
    CreateHandler<vehCarAudioContainerHandler>("vehCarAudioContainer"),
    CreateHandler<vehPoliceCarAudioHandler>("vehPoliceCarAudio"),
    CreateHandler<vehBreakableMgrHandler>("vehBreakableMgr"),
    CreateHandler<vehCarModelFeatureHandler>("vehCarModel"),
    CreateHandler<vehCarSimHandler>("vehCarSim"),
    CreateHandler<vehWheelHandler>("vehWheel"),
    CreateHandler<vehTrailerFeatureHandler>("vehTrailer"),
    CreateHandler<vehTrailerInstanceFeatureHandler>("vehTrailerInstance"),
    CreateHandler<vehCableCarInstanceHandler>("vehCableCarInstance"),
    CreateHandler<vehSirenHandler>("vehSiren"),
    CreateHandler<vehEngineHandler>("vehEngine"),

    CreateHandler<Dialog_NewPlayerHandler>("New player dialog"),

    CreateHandler<lvlHandler>("Propulator"),
    CreateHandler<sdlPage16Handler>("sdlPage16"),
    CreateHandler<vglHandler>("VGL drawing"),

    CreateHandler<StreamHandler>("Stream"),
    CreateHandler<TextureVariantHandler>("Texture variants"),

    CreateHandler<mmCityListHandler>("mmCityList"),

    CreateHandler<PUMainHandler>("PUMain"),

    CreateHandler<fxTexelDamageHandler>("fxTexelDamage"),

    CreateHandler<pedestrianInstanceHandler>("pedestrianInstanceHandler"),

    CreateHandler<aiVehicleInstanceFeatureHandler>("aiVehicleInstance"),
    CreateHandler<aiPoliceOfficerFeatureHandler>("aiPoliceOfficer"),

    CreateHandler<MMDMusicManagerHandler>("MMDMusicManager"),

    CreateHandler<MenuManagerHandler>("MenuManager"),

    CreateHandler<camPovCSHandler>("camPovCS"),

    CreateHandler<luaDrawableHandler>("luaDrawableHandler")
};

// ==========================
// Pointer hooks
// ==========================

static float ped_LodThreshold = 1225.f;

hook::Type<float> obj_NoDrawThresh       ( 0x5C571C ); // default: 300.0

hook::Type<float> obj_VLowThresh         ( 0x5C6658 ); // default: 200.0
hook::Type<float> obj_LowThresh          ( 0x5C665C ); // default: 100.0
hook::Type<float> obj_MedThresh          ( 0x5C6660 ); // default: 40.0

hook::Type<float> sdl_VLowThresh         ( 0x5C5708 );  // default: 300.0
hook::Type<float> sdl_LowThresh          ( 0x5C570C );  // default: 100.0
hook::Type<float> sdl_MedThresh          ( 0x5C5710 );  // default: 50.0

/*
    asCullManagerHandler
*/

static ConfigValue<int> cfgMaxCullables     ("MaxCullables",            1024);
static ConfigValue<int> cfgMaxCullables2D   ("MaxCullables2D",          256);

void asCullManagerHandler::Init(int maxCullables, int maxCullables2D) {
    maxCullables = cfgMaxCullables;
    maxCullables2D = cfgMaxCullables2D;

    LogFile::Format("[asCullManager::Init]: Max Cullables = %d, %d\n", maxCullables, maxCullables2D);

    hook::Thunk<0x4A1290>::Call<void>(this, maxCullables, maxCullables2D);
}

void asCullManagerHandler::Install() {
    InstallCallback("asCullManager::Init", "Increases max cullables.",
        &Init, {
            cb::call(0x401D5C),
        }
    );
}

/*
    cityLevelHandler
*/

const double cosNum = 1.570796;

Vector3 addPitch(Vector3 *vec, float pitch) {
    pitch = (float)fmod(pitch, 3.14159);
    bool pitchIsZero = (pitch >= 0.0f);

    return {
        (float)((!pitchIsZero) ? fmaxf(vec->X, 0) * cos(pitch + cosNum) : 0.0f),
        (float)((!pitchIsZero) ? fmaxf(vec->Y, 0) * cos(pitch + cosNum) : 0.0f),
        (float)((!pitchIsZero) ? fmaxf(vec->Z, 0) * cos(pitch + cosNum) : 0.0f),
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

void cityLevelHandler::DrawRooms(const gfxViewport* viewport, unsigned int p2, LPVOID roomRecs, int numRooms)
{
    city_numRooms = numRooms;
    city_currentRoom = 0;

    auto level = reinterpret_cast<cityLevel*>(this);
    level->DrawRooms(viewport, p2, roomRecs, numRooms);
}

// jumped to at the end of cityLevel::Update
void cityLevelHandler::PostUpdate() {
    // update the SDL lighting
    sdlCommon::UpdateLighting();

    // update our shaded lighting
    // TODO: fix lighting quality not being taken into account (harder than it sounds)
    auto timeWeather = $::timeWeathers.ptr(cityLevel::timeOfDay);

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
        { 5000.0f, 200.0f, 150.0f, 100.0f }, // Very high (NEW)
    };

    if (lod == 3) {
        HookConfig::GetProperty("UltraLOD_NoDrawThreshold", lodLevels[lod][0]);
        HookConfig::GetProperty("UltraLOD_VeryLowThreshold", lodLevels[lod][1]);
        HookConfig::GetProperty("UltraLOD_LowThreshold", lodLevels[lod][2]);
        HookConfig::GetProperty("UltraLOD_MediumThreshold", lodLevels[lod][3]);
    }

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

    // By default the game doesn't set this based on the detail level
    float pedDrawThreshold = powf(HookConfig::GetProperty("PedestrianLod", 35.5f), 2) * (lod + 1);
    ped_LodThreshold = pedDrawThreshold;

    LogFile::Format("[cityLevel::SetObjectDetail]: '%s'\n"
        " - OBJ { %.4f, %.4f, %.4f, %.4f }\n"
        " - SDL { %.4f, %.4f, %.4f }\n"
        " - PED { %.4f }\n",
        lodLevelNames[lod],
        objNoDrawThresh, objVLowThresh, objLowThresh, objMedThresh,
        sdlVLowThresh, sdlLowThresh, sdlMedThresh,
        pedDrawThreshold);
}

void cityLevelHandler::Install() {
    InstallVTableHook("cityLevel::SetObjectDetail", &SetObjectDetail, {
        0x5B16E0
    });

    InstallCallback("cityLevel::DrawRooms", "Custom implementation to allow for getting the number of rooms.",
        &DrawRooms, {
            cb::call(0x445798), // cityLevel::Draw
        }
    );

    InstallCallback("cityLevel::Update", "Adds PostUpdate handler.",
        &PostUpdate, {
            cb::jmp(0x4452D0), // jump to PostUpdate at the very end
        }
    );

    // moves ped lod threshold to writable memory
    mem::write(0x54BC3D + 2, &ped_LodThreshold);
    mem::write(0x57B6CE + 2, &ped_LodThreshold);
}

/*
    cityTimeWeatherLightingHandler
*/

const int NUM_TIMEWEATHERS = 16;

// handled by TextureVariantHandler

struct TimeWeatherInfo {
    cityTimeWeatherLighting *data;

    bool ShowHeadlights;
    bool ShowLightGlows;

    float FlatColorIntensity;
    float WeatherFriction;

    char ReflectionMap[32];
    char ShadowMap[32];
    char GlowName[32];

    void Update() {
        data->ComputeAmbientLightLevels();
    }

    void FileIO(datParser &parser) {
        parser.AddValue("Headlights", &ShowHeadlights);
        parser.AddValue("LightGlows", &ShowLightGlows);

        parser.AddValue("FlatColorIntensity", &FlatColorIntensity);
        parser.AddValue("WeatherFriction", &WeatherFriction);

        parser.AddString("ReflectionMap", &ReflectionMap);
        parser.AddString("ShadowMap", &ShadowMap);

        parser.AddString("GlowName", &GlowName);
    }

    void SetDefaults(dgStatePack *statePack, int index) {
        data = &$::timeWeathers[index];

        ShowHeadlights = (statePack->TimeOfDay >= 2 || statePack->WeatherType == 2);
        ShowLightGlows = statePack->TimeOfDay == 3;

        FlatColorIntensity = (statePack->TimeOfDay == 3) ? 0.5f : 1.0f;

        WeatherFriction = (statePack->WeatherType == 3)
                            ? ((statePack->TimeOfDay == 3) ? 0.75f : 0.8f)
                            : 1.0f;

        strcpy_s(ReflectionMap, "refl_dc");
        strcpy_s(ShadowMap, (statePack->TimeOfDay == 3) ? "shadmap_nite" : "shadmap_day");
        strcpy_s(GlowName, "s_yel_glow");
    }

    void Apply() {
        static hook::Type<gfxTexture *> g_GlowTexture    = 0x62767C;
        static hook::Type<gfxTexture *> g_ReflectionMap  = 0x628914;

        static hook::Type<float> g_FlatColorIntensity    = 0x5C9DA0;
        static hook::Type<float> g_WeatherFriction       = 0x5CF6B8;

        aiMap::Instance->drawHeadlights = ShowHeadlights;
        vehCar::sm_DrawHeadlights = ShowHeadlights;

        g_FlatColorIntensity = FlatColorIntensity;
        g_WeatherFriction = WeatherFriction;

        if (MMSTATE->WeatherType == 3) {
            // jump to the part of mmGame::InitWeather that sets up birth rules
            hook::StaticThunk<0x4133D6>::Call<void>();
        }

        if (!useSoftware)
            g_ReflectionMap = gfxGetTexture(ReflectionMap);

        if (ShowLightGlows)
            g_GlowTexture = gfxGetTexture(GlowName);

        vglSetCloudMap(ShadowMap);
    }

    void ApplyFlatColor() {
        static hook::Type<float> g_FlatColorIntensity = 0x5C9DA0;

        g_FlatColorIntensity = FlatColorIntensity;
    }
};

static TimeWeatherInfo g_TimeWeathers[NUM_TIMEWEATHERS];
static TimeWeatherInfo *TimeWeather = nullptr;

static hook::Type<int> TimeWeatherIdx = 0x62B068;

// cannot be 'bool' or else EAX will be corrupted!
BOOL CanDrawNightTrafficGlows() {
    if (TimeWeather != nullptr)
        return TimeWeather->ShowLightGlows;

    return (dgStatePack::Instance->TimeOfDay >= 2);
}

void InitTimeWeathers() {
    TimeWeatherInfo *timeWeather = g_TimeWeathers;
    dgStatePack *statePack = dgStatePack::Instance;

    for (int i = 0; i < NUM_TIMEWEATHERS; i++) {
        timeWeather->SetDefaults(statePack, i);
        timeWeather++;
    }

    // reset for the iterator
    TimeWeather = nullptr;
}

void NextTimeWeather(char *buffer, const char *format, int idx) {
    sprintf(buffer, format, idx);

    if (TimeWeather != nullptr)
        TimeWeather->Update();

    // set the next TimeWeather for FileIO to reference
    TimeWeather = &g_TimeWeathers[idx];
}

void cityTimeWeatherLightingHandler::Reset() {
    TimeWeather->FlatColorIntensity = 1.0;
    TimeWeather->ApplyFlatColor();
}

void cityTimeWeatherLightingHandler::LoadCityTimeWeatherLighting() {
    InitTimeWeathers();

    // LoadCityTimeWeatherLighting
    hook::StaticThunk<0x443530>::Call<void>();

    TimeWeather = &g_TimeWeathers[TimeWeatherIdx];
    TimeWeather->Apply();
}

void cityTimeWeatherLightingHandler::FileIO(datParser &parser) {
    // cityTimeWeatherLighting::FileIO
    hook::Thunk<0x443440>::Call<void>(this, &parser);

    // apply to the active TimeWeatherInfo
    if (TimeWeather != nullptr)
        TimeWeather->FileIO(parser);
}

void cityTimeWeatherLightingHandler::Install() {
    InstallCallback("LoadCityTimeWeatherLighting", "Allows for more control over city lighting initialization.",
        &LoadCityTimeWeatherLighting, {
            cb::call(0x44425B), // cityLevel::Load
        }
    );

    InstallPatch({
        0xEB, 0x50, // jmp 444241
        0x90,       // nop
    }, {
        0x4441EF, // cityLevel::Load
    });

    InstallPatch("aiTrafficLightInstance::DrawGlow code injection", {
        0xE8, 0xCD, 0xCD, 0xCD, 0xCD,   // call <DEADCODE>
        0x90, 0x90,                     // nop(2)

        0x0F, 0xBF, 0x4E, 0x3C,         // movsx ecx, word ptr [esi+3Ch]
    }, {
        0x53CABC, // aiTrafficLightInstance::DrawGlow
    });

    InstallCallback(&CanDrawNightTrafficGlows, {
        cb::call(0x53CABC),
    }, "aiTrafficLightInstance::DrawGlow code implementation");

    /*
        LoadCityTimeWeatherLighting hooks
    */

    // use 'sprintf' like an iterator ;)
    InstallCallback(&NextTimeWeather, {
        cb::call(0x443564),
    }, "Custom iterator in LoadCityTimeWeatherLighting.");

    // inject our custom properties into the .lt file parser
    InstallCallback(&FileIO, {
        cb::call(0x443584),
    }, "Custom FileIO for cityTimeWeatherLighting.");
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

static bool g_bConsoleOpen = false;
bool sirenSoundState = false;

int HeadlightsToggleKey = 76;
int HazardLightsToggleKey = 189;
int LeftTurnSignalToggleKey = 188;
int RightTurnSignalToggleKey = 190;
int SirenLightsToggleKey = 75;
int SirenSoundsToggleKey = 74;
int FoglightsToggleKey = 77;

bool gfxPipelineHandler::HandleKeyPress(DWORD vKey)
{
    if (MM2Lua::IsEnabled())
    {
        // Inform Lua of any changes beforehand
        MM2Lua::OnKeyPress(vKey);
    }

    switch (vKey) {
        // '~'
        case VK_OEM_3:
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

        case VK_F8: {
            if (HookConfig::Read()) {
                LogFile::WriteLine("Configuration settings reloaded successfully.");
            }
        } return true;
    }

    if (vKey == HeadlightsToggleKey) {
        mmGameManager *mgr = mmGameManager::Instance;
        auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;

        if (gamePtr != NULL)
        {
            auto popup = gamePtr->getPopup();
            auto model = gamePtr->getPlayer()->getCar()->getModel();

            if (popup != NULL) {
                if (!popup->IsEnabled()) {
                    // toggle vehicle headlights
                    model->setHeadlightsState(!model->getHeadlightsState());
                }
            }
        }
        return true;
    }

    if (vKey == HazardLightsToggleKey) {
        mmGameManager *mgr = mmGameManager::Instance;
        auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;
        
        if (gamePtr != NULL)
        {
            auto popup = gamePtr->getPopup();
            auto model = gamePtr->getPlayer()->getCar()->getModel();

            if (popup != NULL) {
                if (!popup->IsEnabled()) {
                    // toggle hazard lights
                    model->setHazardLightsState(!model->getHazardLightsState());
                    model->setLeftSignalLightState(false);
                    model->setRightSignalLightState(false);
                }
            }
        }
        return true;
    }

    if (vKey == LeftTurnSignalToggleKey) {
        mmGameManager *mgr = mmGameManager::Instance;
        auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;

        if (gamePtr != NULL)
        {
            auto popup = gamePtr->getPopup();
            auto model = gamePtr->getPlayer()->getCar()->getModel();

            if (popup != NULL) {
                if (!popup->IsEnabled()) {
                    // toggle left signal
                    model->setLeftSignalLightState(!model->getLeftSignalLightState());
                    model->setHazardLightsState(false);
                    model->setRightSignalLightState(false);
                }
            }
        }
        return true;
    }

    if (vKey == RightTurnSignalToggleKey) {
        mmGameManager *mgr = mmGameManager::Instance;
        auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;

        if (gamePtr != NULL)
        {
            auto popup = gamePtr->getPopup();
            auto model = gamePtr->getPlayer()->getCar()->getModel();

            if (popup != NULL) {
                if (!popup->IsEnabled()) {
                    // toggle right signal
                    model->setRightSignalLightState(!model->getRightSignalLightState());
                    model->setHazardLightsState(false);
                    model->setLeftSignalLightState(false);
                }
            }
        }
        return true;
    }

    if (vKey == SirenLightsToggleKey) {
        mmGameManager *mgr = mmGameManager::Instance;
        auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;

        if (gamePtr != NULL)
        {
            auto popup = gamePtr->getPopup();
            auto car = gamePtr->getPlayer()->getCar();
            auto siren = car->getSiren();
            char *vehName = car->getCarDamage()->GetName();
            int flagsId = VehicleListPtr->GetVehicleInfo(vehName)->GetFlags();

            if (popup != NULL) {
                if (!popup->IsEnabled()) {
                    // toggle siren lights
                    if (siren != nullptr && siren->getHasLights() || flagsId == 8) {
                        siren->setActive(!siren->getActive());
                    }
                }
            }
        }
        return true;
    }

    if (vKey == SirenSoundsToggleKey) {
        mmGameManager *mgr = mmGameManager::Instance;
        auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;

        if (gamePtr != NULL)
        {
            auto popup = gamePtr->getPopup();
            auto audio = gamePtr->getPlayer()->getCar()->getAudio();
            auto policeAudio = audio->GetPoliceCarAudioPtr();

            if (popup != NULL) {
                if (!popup->IsEnabled()) {
                    if (policeAudio != nullptr) {
                        // toggle siren sounds
                        sirenSoundState = !sirenSoundState;

                        if (sirenSoundState)
                            policeAudio->StartSiren(0);
                        else
                            policeAudio->StopSiren();
                    }
                }
            }
        }
        return true;
    }

    if (vKey == FoglightsToggleKey) {
        mmGameManager *mgr = mmGameManager::Instance;
        auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;

        if (gamePtr != NULL)
        {
            auto popup = gamePtr->getPopup();
            auto model = gamePtr->getPlayer()->getCar()->getModel();

            if (popup != NULL) {
                if (!popup->IsEnabled()) {
                    // toggle foglights
                    model->setFoglightsState(!model->getFoglightsState());
                }
            }
        }
        return true;
    }

    return false;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT APIENTRY gfxPipelineHandler::gfxWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    //IMGUI
    ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

    //
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
    if (MMSTATE->NextState != 0) {
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
            cb::jmp(0x4A8CE0),
        }
    );

    InstallCallback("gfxPipeline::gfxWindowCreate", "Custom implementation allowing for more control of the window.",
        &gfxWindowCreate, {
            cb::jmp(0x4A8A90),
        }
    );

    InstallCallback("gfxApplySettings", "Custom implementation allowing for more control of the graphical settings.",
        &gfxApplySettings, {
            cb::jmp(0x4AC870),
        }
    );

    InstallCallback("gfxLoadVideoDatabase", "Disables 'badvideo.txt' file.",
        &ReturnFalse, {
            cb::call(0x4AC4F9),
        }
    );

    ConfigValue<int> cfgHeadlightsToggleKey("HeadlightsToggleKey", 76);
    ConfigValue<int> cfgHazardLightsToggleKey("HazardLightsToggleKey", 189);
    ConfigValue<int> cfgLeftTurnSignalToggleKey("LeftTurnSignalToggleKey", 188);
    ConfigValue<int> cfgRightTurnSignalToggleKey("RightTurnSignalToggleKey", 190);
    ConfigValue<int> cfgSirenLightsToggleKey("SirenLightsToggleKey", 75);
    ConfigValue<int> cfgSirenSoundsToggleKey("SirenSoundsToggleKey", 74);
    ConfigValue<int> cfgFoglightsToggleKey("FoglightsToggleKey", 77);

    HeadlightsToggleKey = cfgHeadlightsToggleKey.Get();
    HazardLightsToggleKey = cfgHazardLightsToggleKey.Get();
    LeftTurnSignalToggleKey = cfgLeftTurnSignalToggleKey.Get();
    RightTurnSignalToggleKey = cfgRightTurnSignalToggleKey.Get();
    SirenLightsToggleKey = cfgSirenLightsToggleKey.Get();
    SirenSoundsToggleKey = cfgSirenSoundsToggleKey.Get();
    FoglightsToggleKey = cfgFoglightsToggleKey.Get();
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
            cb::call(0x448372), // 448371 + 1, after our 'push edi' instruction (SEE ABOVE)
        }
    );

    InstallCallback("sdlPage16::Draw", "Hooks a call to Quitf to print out more detailed information.",
        &InvalidCmd, {
            cb::call(0x4507B3),
        }
    );

    InstallCallback("cityLevel::DrawRooms", "Intercepts a call to sdlPage16::Draw.",
        &Draw, {
            cb::call(0x4459D2),
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

        InstallCallback(vglBeginCB, cb::call(begin));
        InstallCallback(vglEndCB, cb::call(end));

        Installf("   - { vglBegin: %08X, vglEnd: %08X }\n", begin, end);
    }
}

/*
    Aud3DObjectManager
*/

void Aud3DObjectManagerHandler::InitAmbObjContainer(LPCSTR name) {
    string_buf<80> buffer("%sambientcontainer", MMSTATE->CityName);

    //don't continue if it doesn't exist
    if (!datAssetManager::Exists("aud\\ambient", buffer, "csv"))
        return;

    LPCSTR szAmbientSFX = buffer;
    LogFile::Format("AmbientContainer: %s\n", szAmbientSFX);

    //call original
    hook::Thunk<0x50F650>::Call<void>(this, szAmbientSFX);
}

void Aud3DObjectManagerHandler::Install() {
    InstallPatch("Allows for custom positional ambient effects in addon cities.", { 0x90, 0x90 }, {
        0x404059,
        });

    InstallCallback("mmPlayer::Init", "Allows for custom positional ambient effects in addon cities.",
        &InitAmbObjContainer, {
            cb::call(0x404082),
        }
    );
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
    HookConfig::GetProperty("DefaultCityAmbienceFile", defaultCityAmbienceFile, sizeof(defaultCityAmbienceFile));

    InstallCallback("mmGameMusicData::LoadAmbientSFX", "Allows for custom ambient effects in addon cities.",
        &LoadAmbientSFX, {
            cb::call(0x433F93),
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
    HookConfig::GetProperty("DefaultCitySirenFile", defaultCitySirenFile, sizeof(defaultCitySirenFile));

    InstallCallback("vehCarAudioContainer::SetSirenCSVName", "Allows for custom sirens in addon cities.",
        &SetSirenCSVName, {
            cb::call(0x412783),
            cb::call(0x412772),
        }
    );

    InstallCallback("vehCarAudioContainer::PlayHorn", "Adds horn sound for cop cars.",
        &vehCarAudioContainer::PlayHorn, {
            cb::call(0x43D4ED),
        }
    );

    InstallCallback("vehCarAudioContainer::StopHorn", "Adds horn sound for cop cars.",
        &vehCarAudioContainer::StopHorn, {
            cb::call(0x43D500),
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
    hook::Thunk<0x4D46F0>::Call<void>(this, a1, a2, basename, sirenCsvFile, a5);
}

void vehPoliceCarAudioHandler::Reset() {
    auto policeAudio = reinterpret_cast<vehPoliceCarAudio*>(this);
    policeAudio->StopSiren();

    //call original
    hook::Thunk<0x4D5290>::Call<void>(this);
}

void vehPoliceCarAudioHandler::Install() {
    InstallCallback("vehPoliceCarAudio::Init", "Allows vehicles to use their own custom sirens instead of default ones for each city.",
        &InitSirenAudio, {
            cb::call(0x4D44A3),
        }
    );

    InstallCallback("vehPoliceCarAudio::Reset", "Deactivates siren sounds upon reset.",
        &Reset, {
            cb::call(0x4D19D6),
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
            cb::call(0x45D70F),
        }
    );

    InstallCallback("lvlAiMap::SetRoad", "Allows for more detailed information when propulating roads.",
        &GetAIRoom, {
            cb::call(0x45D76E),
        }
    );

    if (datArgParser::Get("sdlEnumTest"))
    {
        InstallCallback("lvlSDL::Enumerate", "New enumerate function.",
            &EnumerateSDL, {
                cb::jmp(0x45BE50),
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
                cb::call(0x45BEF4),
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
    return hook::Thunk<0x577210>::Call<void>(this, memAllocator, heapSize, p3, p4, checkAlloc); //TODO: move to own class
}

void memSafeHeapHandler::Install() {
    cfgHeapSize.Get(g_heapSize);

    InstallCallback("memSafeHeap::Init", "Adds '-heapsize' parameter that takes a size in megabytes. Defaults to 128MB.",
        &Init, {
            cb::call(0x4015DD),
        }
    );
}

/*
    mmGameHandler
*/

hook::Type<float> bridgeSpeed(0x5DBFA4);
hook::Type<float> bridgeAngle(0x5DBFA8);

bool playerCanFly = false;
bool postalEnabled = false;
float boneScale = 1.f;

void mmGameHandler::SendChatMessage(char *message) {
    if (g_bConsoleOpen) {
        if (MM2Lua::IsEnabled())
            MM2Lua::SendCommand(message);

        // return normal chatbox behavior
        g_bConsoleOpen = false;
    }
    else {
        LogFile::Format("Got chat message: %s\n", message);

        //handle custom commands (TODO: some "RegisterCommand" thing?)
        if (!strcmp(message, "/freecam")) {
            mmGameManager *mgr = mmGameManager::Instance;
            auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;
            auto playerPtr = (gamePtr != NULL) ? gamePtr->getPlayer() : NULL;

            if (gamePtr != NULL && playerPtr != NULL)
            {
                auto playerPosition = playerPtr->getCar()->getModel()->GetPosition();
                auto freecam = playerPtr->getFreecam();

                //setcam and update view
                playerPtr->getCamView()->SetCam(freecam);
                freecam->UpdateView();

                //set freecam pos if >=100m away
                if (playerPosition.Dist(*freecam->getPosition()) >= 100.f) {
                    freecam->SetPosition(&playerPosition);
                }
            }
        }
        if (!strcmp(message, "/fuzz")) {
            mmGameManager *mgr = mmGameManager::Instance;
            auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;
            auto playerPtr = (gamePtr != NULL) ? gamePtr->getPlayer() : NULL;

            if (gamePtr != NULL && playerPtr != NULL)
            {
                auto hudmap = playerPtr->getHudmap();
                hudmap->SetShowAllCops(!hudmap->GetShowAllCops());
            }
        }
        if (!strcmp(message, "/grav")) {
            dgPhysManager::Gravity.set(dgPhysManager::Gravity.get() == -19.6f ? -9.8f : -19.6f);
        }
        if (!strcmp(message, "/slide")) {
            if (MMSTATE->WeatherType == 3 && MMSTATE->TimeOfDay == 3)
                vehWheel::WeatherFriction.set(vehWheel::WeatherFriction.get() == 0.75f ? -0.02f : 0.75f);
            else if (MMSTATE->WeatherType == 3 && MMSTATE->TimeOfDay != 3)
                vehWheel::WeatherFriction.set(vehWheel::WeatherFriction.get() == 0.8f ? -0.03f : 0.8f);
            else
                vehWheel::WeatherFriction.set(vehWheel::WeatherFriction.get() == 1.f ? -0.04f : 1.f);
        }
        if (!strcmp(message, "/bridge")) {
            bridgeSpeed.set(bridgeSpeed.get() == 0.05f ? 2.5f : 0.05f);
            bridgeAngle.set(bridgeAngle.get() == 0.471238941f ? 1.f : 0.471238941f);
        }
        if (!strcmp(message, "/fly")) {
            playerCanFly = !playerCanFly;
        }
        if (!strcmp(message, "/nodamage")) {
            mmGameManager *mgr = mmGameManager::Instance;
            auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;
            auto playerPtr = (gamePtr != NULL) ? gamePtr->getPlayer() : NULL;

            if (gamePtr != NULL && playerPtr != NULL)
            {
                auto carDamage = playerPtr->getCar()->getCarDamage();
                if (carDamage->getImpactThreshold() < 1001500.f)
                    carDamage->setImpactThreshold(carDamage->getImpactThreshold() + 1001500.f);
            }
        }
        if (!strcmp(message, "/damage")) {
            mmGameManager *mgr = mmGameManager::Instance;
            auto gamePtr = (mgr != NULL) ? mgr->getGame() : NULL;
            auto playerPtr = (gamePtr != NULL) ? gamePtr->getPlayer() : NULL;

            if (gamePtr != NULL && playerPtr != NULL)
            {
                auto carDamage = playerPtr->getCar()->getCarDamage();
                if (carDamage->getImpactThreshold() >= 1001500.f)
                    carDamage->setImpactThreshold(carDamage->getImpactThreshold() - 1001500.f);
            }
        }
        if (!strcmp(message, "/big")) {
            boneScale = boneScale == 1.f ? 5.f : 1.f;
        }
        if (!strcmp(message, "/tiny")) {
            boneScale = boneScale == 1.f ? 0.2f : 1.f;
        }
        if (!strcmp(message, "/postal")) {
            postalEnabled = !postalEnabled;
        }

        //send to dispatcher
        GameEventDispatcher::onChatMessage(message);
    }
}

void mmGameHandler::InitWeather(void) {
    // should've already been initialized, but juuuust in case...
    if (TimeWeather == nullptr) {
        Warningf("Couldn't initialize weather using TimeWeather, loading defaults...");
        get<mmGame>()->InitWeather();
    }
}

static ConfigValue<bool> cfgMm1StyleAutoReverse("MM1StyleAutoReverse", false);

void mmGameHandler::UpdateSteeringBrakes(void) {
    auto game = reinterpret_cast<mmGame*>(this);
    auto player = game->getPlayer();
    auto car = player->getCar();
    auto carsim = car->getCarSim();
    auto engine = carsim->getEngine();
    auto transmission = carsim->getTransmission();
    auto curDamage = car->getCarDamage()->getCurDamage();
    auto maxDamage = car->getCarDamage()->getMaxDamage();
    auto inst = mmReplayManager::Instance;

    void *gameInputPtr = *reinterpret_cast<void **>(0x6B1CF0); // pointer to mmInput

    int *vehCarPtr = *getPtr<int*>(player, 0x284);
    int reverse = *getPtr<int>(vehCarPtr, 0x304);
    int autoReverse = *getPtr<int>(gameInputPtr, 0x18C);
    int *pedalsSwapped = getPtr<int>(gameInputPtr, 0x1D4); // swaps throttle and brake inputs if true

    float throttleValue = *getPtr<float>(this, 0x40C);
    float brakesValue = *getPtr<float>(this, 0x68);
    float speedValue = *getPtr<float>(this, 0x6C);
    float speedMPH = carsim->getSpeedMPH();
    float brakes = inst->GetBrakes();
    float throttle = inst->GetThrottle();
    float steering = inst->GetSteering();
    float handbrakes = inst->GetHandBrakes();

    carsim->setBrake(brakes);
    carsim->setHandbrake(handbrakes);
    player->SetSteering(steering);

    if (NETMGR->SessionOpen && reverse >= 2) {
        if (throttle >= 0.f) {
            if (throttle > throttleValue)
                throttle = throttleValue;
            engine->setThrottleInput(throttle);
        }
        else {
            engine->setThrottleInput(0.f);
        }
    }
    else {
        engine->setThrottleInput(throttle);
    }

    if (transmission->IsAuto() && autoReverse) {
        if (reverse) {
            if (speedMPH <= speedValue && brakes >= brakesValue && throttle <= 0.1f) {
                *pedalsSwapped = true;
                transmission->SetReverse();
            }
        }
        else if (!reverse && *pedalsSwapped) {
            if (speedMPH <= speedValue && brakes >= brakesValue && throttle <= 0.1f) {
                *pedalsSwapped = false;
                transmission->SetForward();
            }
        }
        else if (!reverse && !*pedalsSwapped) {
            if (speedMPH <= speedValue && brakes >= brakesValue && throttle <= 0.1f) {
                *pedalsSwapped = true;
            }
        }
        // reset throttle and brake inputs when the vehicle is destroyed
        if (curDamage > maxDamage && *pedalsSwapped)
            *pedalsSwapped = false;
    }
}

static ConfigValue<bool> cfgGtaStyleHornSiren("GTAStyleHornSiren", false);
static ConfigValue<float> cfgHornSirenThreshold("HornSirenThreshold", 0.15f);
static float horn_lastPressTime = 0.f;
static float horn_lastReleaseTime = 0.f;
static float horn_holdTime = 0.f;
static float horn_sirenThreshold = 0.15f;
static bool lastHornButtonState = false;

char launchPropName[128] = "sp_mailbox_f";
float launchPropSpeed = 25.f;

void mmGameHandler::LaunchProp() {
    auto game = reinterpret_cast<mmGame*>(this);
    auto player = game->getPlayer();
    auto car = player->getCar();
    auto carInst = car->GetInst();

    Matrix34 matrix = Matrix34();
    gfxRenderState::sm_Camera->ToMatrix34(matrix);
    auto camPos = matrix.GetRow(3);
    auto camFwd = matrix.GetRow(2);
    matrix.SetRow(3, camPos - camFwd * 6.f);

    auto reqBanger = dgUnhitBangerInstance::RequestBanger(launchPropName, 0);
    reqBanger->Init(launchPropName, matrix, 0);

    // this is how car breakables work
    auto banger = dgBangerManager::GetInstance()->GetBanger();
    banger->SetBangerType(reqBanger->GetBangerType());
    banger->SetGeomIndex(reqBanger->GetGeomIndex());
    banger->SetVariant(reqBanger->GetVariant());
    banger->SetMatrix(matrix);
    lvlLevel::Singleton->MoveToRoom(banger, lvlLevel::Singleton->FindRoomId(camPos, 0));

    //make active and throw!
    auto active = banger->AttachEntity();
    if (active != nullptr)
    {
        auto carVel = carInst->GetVelocity();
        auto bangerICS = active->GetICS();
        auto launchedVel = carVel + (camFwd * bangerICS->GetMass() * launchPropSpeed);
        bangerICS->SetScaledVelocity(launchedVel * -1.f);
        bangerICS->SetAngularVelocity(launchedVel);
    }
}

void mmGameHandler::UpdateHorn(bool a1) {
    auto game = reinterpret_cast<mmGame*>(this);
    auto player = game->getPlayer();
    auto car = player->getCar();
    auto siren = car->getSiren();
    auto audio = car->getAudio();
    auto model = car->getModel();
    auto lightbar0 = model->getGenBreakableMgr()->Get(1);
    auto lightbar1 = model->getGenBreakableMgr()->Get(2);

    auto policeAudio = audio->GetPoliceCarAudioPtr();
    char* vehName = car->getCarDamage()->GetName();

    bool isSirenActive = siren->getActive();
    bool isVehiclePolice = audio->IsPolice(vehName);

    bool cancelHornInput = horn_holdTime < horn_sirenThreshold && isVehiclePolice;

    //button state updating
    bool buttonReleasedThisFrame = a1 != lastHornButtonState && !a1;
    bool buttonPressedThisFrame = a1 != lastHornButtonState && a1;
    if (buttonPressedThisFrame)
    {
        horn_lastPressTime = datTimeManager::ElapsedTime;

        if (postalEnabled)
            LaunchProp();
    }
    else if (buttonReleasedThisFrame)
    {
        horn_lastReleaseTime = datTimeManager::ElapsedTime;
        horn_holdTime = 0.f;
    }
    if (a1)
        horn_holdTime += datTimeManager::Seconds;
    lastHornButtonState = a1;

    //update police audio
    if (isVehiclePolice && siren != nullptr && !player->IsMaxDamaged()) {
        if ((lightbar0 == nullptr || (lightbar0 != nullptr && lightbar0->IsAttached)) ||
            (lightbar1 == nullptr || (lightbar1 != nullptr && lightbar1->IsAttached))) {
            if (buttonReleasedThisFrame && (horn_lastReleaseTime - horn_lastPressTime) < horn_sirenThreshold) {
                if (siren->getActive()) {
                    siren->setActive(false);
                    audio->StopSiren();
                }
                else
                {
                    siren->setActive(true);
                    audio->StartSiren();
                }
            }
        }
    }

    //update horn audio
    if (isVehiclePolice)
    {
        if (a1) {
            if (!cancelHornInput)
                audio->PlayHorn();
        }
        else {
            audio->StopHorn();
        }
    }
    else
    {
        if (a1) {
            audio->PlayHorn();
        }
        else
        {
            audio->StopHorn();
        }
    }
}

void mmGameHandler::Install() {
    HookConfig::GetProperty("LaunchPropName", launchPropName, sizeof(launchPropName));
    ConfigValue<float> cfgLaunchPropSpeed("LaunchPropSpeed", 25.f);
    launchPropSpeed = cfgLaunchPropSpeed.Get();

    InstallPatch("Increases chat buffer size.", { 60 }, {
        0x4E68B5,
        0x4E68B9,
        0x50BBCF,
    });

    InstallCallback("mmGame::SendChatMessage", "Passes any chat messages to the handler.",
        &SendChatMessage, {
            cb::jmp(0x414EB6),
        }
    );

    InstallCallback("mmGame::InitWeather", "Allows for more control over weather initialization.",
        &InitWeather, {
            cb::call(0x4131C0), // mmGame::Init
        }
    );

    //removes the "SpecialBound" (INST) spawn limitation (Flag 32), leaving only Standard flag
    /*
    InstallPatch("Enables spawning in INST rooms.", { 0x04 }, {
        0x413C1C,
        });
        */

    if (cfgMm1StyleAutoReverse.Get()) {
        InstallCallback("mmGame::UpdateSteeringBrakes", "Improves auto reverse system.",
            &UpdateSteeringBrakes, {
                cb::call(0x413EED),
                cb::call(0x413F29),
                cb::call(0x413F4C),
            }
        );
    }

    horn_sirenThreshold = cfgHornSirenThreshold.Get();
    if (cfgGtaStyleHornSiren.Get()) {
        InstallCallback("mmGame::UpdateHorn", "Implements GTA-style horn/siren",
            &UpdateHorn, {
                cb::call(0x413F22),
                cb::call(0x414691),
            }
        );
    }
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
            cb::call(0x51941D),
        }
    );
}

/*
    gizFerryHandler
*/

static ConfigValue<float> cfgFerrySpeedMultiplier ("FerrySpeedMultiplier", 5.0f);

void gizFerryHandler::SetSpeed(float value) {
    value *= cfgFerrySpeedMultiplier;

    hook::Thunk<0x579520>::Call<void>(this, value);
}

void gizFerryHandler::Install() {
    InstallCallback("gizFerry:SetSpeed", "Allows a speed modifier to be applied to ferry speeds.",
        &SetSpeed, {
            cb::call(0x579951), // gizFerryMgr::ApplyTuning
        }
    );

}

/*
    gizParkedCarMgrHandler
*/

static ConfigValue<bool> cfgDynamicParkedCarDensity("DynamicParkedCarDensity", true);

void gizParkedCarMgrHandler::EnumeratePath(LPCSTR a1, const Matrix34* a2, bool a3) {
    //only apply car scaling in cruise
    if (dgStatePack::Instance->GameMode == Cruise) {
        int oldRandomSeed = gRandSeed;
        float rand = hook::StaticThunk<0x4BBE30>::Call<float>();

        if (dgStatePack::Instance->TrafficDensity > rand) {
            gRandSeed = oldRandomSeed;
            hook::StaticThunk<0x579BD0>::Call<void>(a1, a2, a3); //gizParkedCarMgr_EnumeratePath
        }
    }
    else {
        hook::StaticThunk<0x579BD0>::Call<void>(a1, a2, a3); //gizParkedCarMgr_EnumeratePath
    }
}

void gizParkedCarMgrHandler::Install() {
    if (cfgDynamicParkedCarDensity) {
        InstallCallback("gizParkedCarMgr::Init", "Scales parked cars with traffic density.",
            &EnumeratePath, {
                cb::push(0x579B80),
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
    //call original
    hook::Thunk<0x4415E0>::Call<void>(this, lod);
}

void BridgeFerryHandler::Install() {
    // revert bridges/ferries to how they were in the betas
    InstallCallback("Bridge/Ferry: Cull", "Quick'n dirty fix for fullbright bridges/ferries.",
        &Cull, {
            cb::call(0x5780BC), // gizBridgeMgr::Cull
            cb::call(0x5798F0), // gizFerryMgr::Cull
        }
    );

    InstallVTableHook("Bridge/Ferry: Draw", &Draw, {
        0x5B5FB8, // gizBridge::Draw
        0x5B61AC, // gizFerry::Draw
        0x5B14C0,
        0x5B1538,
        0x5B15E4,
        0x5B5700,
        0x5B6100,
    });
}

/*
    mmHudMapFeatureHandler
*/
int hudMapColorStyle = 0;
int playerTriColor = 5;
int playerTriOutlineColor = 0;
int policeTriColor = 1;
int policeTriOutlineColor = 0;
int opponentTriColor = 7;
int opponentTriOutlineColor = 0;

static ConfigValue<int> cfgHudMapColorStyle          ("HudMapColorStyle",          0);
static ConfigValue<int> cfgPlayerTriColor            ("PlayerTriColor",            5);
static ConfigValue<int> cfgPlayerTriOutlineColor     ("PlayerTriOutlineColor",     0);
static ConfigValue<int> cfgPoliceTriColor            ("PoliceTriColor",            1);
static ConfigValue<int> cfgPoliceTriOutlineColor     ("PoliceTriOutlineColor",     0);
static ConfigValue<int> cfgOpponentTriColor          ("OpponentTriColor",          7);
static ConfigValue<int> cfgOpponentTriOutlineColor   ("OpponentTriOutlineColor",   0);

void mmHudMapFeatureHandler::DrawColoredTri(unsigned int color, const Matrix34 &matrix) {
    rglEnableDisable(RGL_DEPTH_TEST, false);
    Matrix44::Convert(gfxRenderState::sm_World, matrix);
    gfxRenderState::m_Touched |= 0x88;
    vglBindTexture(0);
    vglBegin(DRAWMODE_TRIANGLELIST, 0);
    vglCurrentColor = color;
    vglVertex(0.f, 0.f, -1.f);
    vglVertex(-0.69999999f, 0.f, 1.f);
    vglVertex(0.69999999f, 0.f, 1.f);
    vglEnd();
    rglEnableDisable(RGL_DEPTH_TEST, true);
}

void mmHudMapFeatureHandler::DrawWhiteTri(const Matrix34 &matrix) {
    rglEnableDisable(RGL_DEPTH_TEST, false);
    Matrix44::Convert(gfxRenderState::sm_World, matrix);
    gfxRenderState::m_Touched |= 0x88;
    vglBindTexture(0);
    vglBegin(DRAWMODE_TRIANGLELIST, 0);
    vglCurrentColor = 0xFFFFFFFF;
    vglVertex(0.f, 0.f, -1.f);
    vglVertex(-0.69999999f, 0.f, 1.f);
    vglVertex(0.69999999f, 0.f, 1.f);
    vglEnd();
    rglEnableDisable(RGL_DEPTH_TEST, true);
}

void mmHudMapFeatureHandler::DrawLightOrangeTri(const Matrix34 &matrix) {
    rglEnableDisable(RGL_DEPTH_TEST, false);
    Matrix44::Convert(gfxRenderState::sm_World, matrix);
    gfxRenderState::m_Touched |= 0x88;
    vglBindTexture(0);
    vglBegin(DRAWMODE_TRIANGLELIST, 0);
    vglCurrentColor = 0xFFFDBF72;
    vglVertex(0.f, 0.f, -1.f);
    vglVertex(-0.69999999f, 0.f, 1.f);
    vglVertex(0.69999999f, 0.f, 1.f);
    vglEnd();
    rglEnableDisable(RGL_DEPTH_TEST, true);
}

void mmHudMapFeatureHandler::DrawLightGreenTri(const Matrix34 &matrix) {
    rglEnableDisable(RGL_DEPTH_TEST, false);
    Matrix44::Convert(gfxRenderState::sm_World, matrix);
    gfxRenderState::m_Touched |= 0x88;
    vglBindTexture(0);
    vglBegin(DRAWMODE_TRIANGLELIST, 0);
    vglCurrentColor = 0xFFC0EC42;
    vglVertex(0.f, 0.f, -1.f);
    vglVertex(-0.69999999f, 0.f, 1.f);
    vglVertex(0.69999999f, 0.f, 1.f);
    vglEnd();
    rglEnableDisable(RGL_DEPTH_TEST, true);
}

hook::Type<unsigned int> HudmapIconColors(0x5C4740);
hook::Type<Vector3> YAXIS(0x6A3B28);
Matrix34 mtx;

void mmHudMapFeatureHandler::DrawIcon(int iconType, const Matrix34 &matrix) {
    auto hudmap = reinterpret_cast<mmHudMap*>(this);
    mtx.Set(matrix);

    mtx.SetRow(1, YAXIS);
    mtx.Normalize();

    mtx.m31 += 15.f;
    mtx.Scale(hudmap->GetIconScale());

    uint color = *HudmapIconColors[iconType];

    if (iconType >= 0)
        DrawColoredTri(color, mtx);
    if (iconType < 0)
        DrawWhiteTri(mtx);
}

void mmHudMapFeatureHandler::DrawNfsMwPlayerIcon(const Matrix34 &matrix) {
    auto hudmap = reinterpret_cast<mmHudMap*>(this);
    mtx.Set(matrix);

    mtx.SetRow(1, YAXIS);
    mtx.Normalize();

    mtx.m31 += 15.f;
    mtx.Scale(hudmap->GetIconScale());

    DrawLightOrangeTri(mtx);
}

void mmHudMapFeatureHandler::DrawNfsMwOpponentIcon(const Matrix34 &matrix) {
    auto hudmap = reinterpret_cast<mmHudMap*>(this);
    mtx.Set(matrix);

    mtx.SetRow(1, YAXIS);
    mtx.Normalize();

    mtx.m31 += 15.f;
    mtx.Scale(hudmap->GetIconScale());

    DrawLightGreenTri(mtx);
}

void mmHudMapFeatureHandler::DrawPlayer() {
    auto hudmap = reinterpret_cast<mmHudMap*>(this);
    auto player = hudmap->GetPlayer();
    auto car = player->getCar();
    auto audio = car->getAudio();
    auto siren = car->getSiren();
    char *vehName = car->getCarDamage()->GetName();
    bool elapsedTime1 = fmod(datTimeManager::ElapsedTime, 0.15f) > 0.1f;
    bool elapsedTime2 = fmod(datTimeManager::ElapsedTime, 0.125f) > 0.1f;
    bool elapsedTime3 = fmod(datTimeManager::ElapsedTime, 0.5f) > 0.25f;

    // draw triangle outline
    auto playerMtx = hudmap->GetPlayerMatrix();
    float scaleAmount = hudmap->GetIconScale() * 1.3f;
    float iconScale = hudmap->GetIconScale();
    hudmap->SetIconScale(scaleAmount);

    if (hudMapColorStyle == 0) {
        DrawIcon(0, *playerMtx);
        hudmap->SetIconScale(iconScale);
        DrawIcon(5, *playerMtx);
    }
    if (hudMapColorStyle == 1) {
        if (audio->IsPolice(vehName)) {
            DrawIcon(2, *playerMtx);
            hudmap->SetIconScale(iconScale);
            DrawIcon(1, *playerMtx);
        }
        else {
            DrawIcon(-1, *playerMtx);
            hudmap->SetIconScale(iconScale);
            DrawIcon(0, *playerMtx);
        }
    }
    if (hudMapColorStyle == 2) {
        DrawIcon(0, *playerMtx);
        hudmap->SetIconScale(iconScale);
        if (audio->IsPolice(vehName)) {
            DrawIcon(2, *playerMtx);
            if (siren != nullptr && siren->getActive()) {
                if (elapsedTime3)
                    DrawIcon(1, *playerMtx);
            }
        }
        else {
            DrawIcon(5, *playerMtx);
        }
    }
    if (hudMapColorStyle == 3) {
        DrawIcon(0, *playerMtx);
        hudmap->SetIconScale(iconScale);
        if (audio->IsPolice(vehName)) {
            if (siren != nullptr && siren->getActive()) {
                DrawIcon(2, *playerMtx);
                if (elapsedTime1)
                    DrawIcon(1, *playerMtx);
                if (elapsedTime2)
                    DrawIcon(-1, *playerMtx);
            }
            if (siren != nullptr && !siren->getActive()) {
                DrawIcon(-1, *playerMtx);
            }
        }
        else {
            DrawNfsMwPlayerIcon(*playerMtx);
        }
    }
    if (hudMapColorStyle == 4) {
        DrawIcon(0, *playerMtx);
        hudmap->SetIconScale(iconScale);
        if (audio->IsPolice(vehName)) {
            if (siren != nullptr && siren->getActive()) {
                DrawIcon(2, *playerMtx);
                if (elapsedTime1)
                    DrawIcon(1, *playerMtx);
                if (elapsedTime2)
                    DrawIcon(-1, *playerMtx);
            }
            if (siren != nullptr && !siren->getActive()) {
                DrawIcon(4, *playerMtx);
            }
        }
        else {
            DrawIcon(8, *playerMtx);
        }
    }
    if (hudMapColorStyle >= 5) {
        DrawIcon(playerTriOutlineColor, *playerMtx);
        hudmap->SetIconScale(iconScale);
        DrawIcon(playerTriColor, *playerMtx);
    }
}

void mmHudMapFeatureHandler::DrawCops() {
    auto hudmap = reinterpret_cast<mmHudMap*>(this);
    auto AIMAP = &aiMap::Instance;
    bool elapsedTime1 = fmod(datTimeManager::ElapsedTime, 0.15f) > 0.1f;
    bool elapsedTime2 = fmod(datTimeManager::ElapsedTime, 0.125f) > 0.1f;
    bool elapsedTime3 = fmod(datTimeManager::ElapsedTime, 0.5f) > 0.25f;
    BOOL ShowAllCops = hudmap->GetShowAllCops();

    for (int i = 0; i < AIMAP->numCops; i++) {
        auto police = AIMAP->Police(i);
        auto policeMtx = police->getCar()->getCarSim()->getICS()->GetMatrix();
        short policeState = police->getPoliceState();

        // check if the cop in pursuit
        if (policeState || ShowAllCops) {
            // draw triangle outline
            float scaleAmount = hudmap->GetIconScale() * 1.3f;
            float iconScale = hudmap->GetIconScale();
            hudmap->SetIconScale(scaleAmount);

            if (hudMapColorStyle == 0) {
                DrawIcon(0, *policeMtx);
                hudmap->SetIconScale(iconScale);
                DrawIcon(1, *policeMtx);
            }
            if (hudMapColorStyle == 1) {
                DrawIcon(2, *policeMtx);
                hudmap->SetIconScale(iconScale);
                DrawIcon(1, *policeMtx);
            }
            if (hudMapColorStyle == 2) {
                DrawIcon(0, *policeMtx);
                hudmap->SetIconScale(iconScale);
                DrawIcon(2, *policeMtx);
                if (elapsedTime3)
                    DrawIcon(1, *policeMtx);
                if (policeState == 12 || policeState == 0)
                    DrawIcon(2, *policeMtx);
            }
            if (hudMapColorStyle == 3) {
                DrawIcon(0, *policeMtx);
                hudmap->SetIconScale(iconScale);
                DrawIcon(2, *policeMtx);
                if (elapsedTime1)
                    DrawIcon(1, *policeMtx);
                if (elapsedTime2)
                    DrawIcon(-1, *policeMtx);
                if (policeState == 12 || policeState == 0)
                    DrawIcon(-1, *policeMtx);
            }
            if (hudMapColorStyle == 4) {
                DrawIcon(0, *policeMtx);
                hudmap->SetIconScale(iconScale);
                DrawIcon(2, *policeMtx);
                if (elapsedTime1)
                    DrawIcon(1, *policeMtx);
                if (elapsedTime2)
                    DrawIcon(-1, *policeMtx);
                if (policeState == 12 || policeState == 0)
                    DrawIcon(4, *policeMtx);
            }
            if (hudMapColorStyle >= 5) {
                DrawIcon(policeTriOutlineColor, *policeMtx);
                hudmap->SetIconScale(iconScale);
                DrawIcon(policeTriColor, *policeMtx);
            }
        }
    }
}

void mmHudMapFeatureHandler::DrawOpponents() {
    auto hudmap = reinterpret_cast<mmHudMap*>(this);
    auto AIMAP = &aiMap::Instance;

    for (int i = 0; i < hudmap->GetNumOpponents(); i++) {
        auto oppIconInfo = hudmap->GetOppIconInfo(i);
        auto opponentMtx = oppIconInfo->Matrix;

        if (oppIconInfo->Enabled && opponentMtx) {
            // draw triangle outline
            float scaleAmount = hudmap->GetIconScale() * 1.3f;
            float iconScale = hudmap->GetIconScale();
            hudmap->SetIconScale(scaleAmount);

            // check if we're in multiplayer
            if (MMSTATE->unk_EC) {
                DrawIcon(0, *opponentMtx);
                hudmap->SetIconScale(iconScale);
                DrawIcon(i + 2, *opponentMtx);
            } 
            else {
                auto opponent = AIMAP->Opponent(i);
                auto car = opponent->getCar();
                auto curDamage = car->getCarDamage()->getCurDamage();
                auto maxDamage = car->getCarDamage()->getMaxDamage();

                if (curDamage <= maxDamage) {
                    if (hudMapColorStyle == 0) {
                        DrawIcon(0, *opponentMtx);
                        hudmap->SetIconScale(iconScale);
                        DrawIcon(7, *opponentMtx);
                    }
                    if (hudMapColorStyle == 1) {
                        DrawIcon(0, *opponentMtx);
                        hudmap->SetIconScale(iconScale);
                        DrawIcon(i + 2, *opponentMtx);
                    }
                    if (hudMapColorStyle == 2) {
                        DrawIcon(0, *opponentMtx);
                        hudmap->SetIconScale(iconScale);
                        DrawIcon(3, *opponentMtx);
                    }
                    if (hudMapColorStyle == 3) {
                        DrawIcon(0, *opponentMtx);
                        hudmap->SetIconScale(iconScale);
                        DrawNfsMwOpponentIcon(*opponentMtx);
                    }
                    if (hudMapColorStyle == 4) {
                        DrawIcon(0, *opponentMtx);
                        hudmap->SetIconScale(iconScale);
                        DrawIcon(6, *opponentMtx);
                    }
                    if (hudMapColorStyle >= 5) {
                        DrawIcon(opponentTriOutlineColor, *opponentMtx);
                        hudmap->SetIconScale(iconScale);
                        DrawIcon(opponentTriColor, *opponentMtx);
                    }
                }
                else {
                    DrawIcon(0, *opponentMtx);
                    hudmap->SetIconScale(iconScale);
                    DrawIcon(16, *opponentMtx);
                }
            }
        }
    }
}

void mmHudMapFeatureHandler::Install() {
    hudMapColorStyle = cfgHudMapColorStyle.Get();
    playerTriColor = cfgPlayerTriColor.Get();
    playerTriOutlineColor = cfgPlayerTriOutlineColor.Get();
    policeTriColor = cfgPoliceTriColor.Get();
    policeTriOutlineColor = cfgPoliceTriOutlineColor.Get();
    opponentTriColor = cfgOpponentTriColor.Get();
    opponentTriOutlineColor = cfgOpponentTriOutlineColor.Get();

    InstallCallback("mmHudMap::DrawPlayer",
        &DrawPlayer, {
            cb::call(0x42F527),
        }
    );

    InstallCallback("mmHudMap::DrawCops",
        &DrawCops, {
            cb::call(0x42F519),
        }
    );

    InstallCallback("mmHudMap::DrawOpponents",
        &DrawOpponents, {
            cb::call(0x42F520),
        }
    );
}

/*
    mmIconsHandler
*/
int opponentIconStyle = 0;
int opponentIconColor = 5;
int blitzIconColor = 6;
bool blitzIconMultiColor = false;
bool opponentIconTransparency = false;
bool blitzIconTransparency = false;

static ConfigValue<bool> cfgBlitzIconMultiColor        ("BlitzIconMultiColor",         false);
static ConfigValue<bool> cfgOpponentIconTransparency   ("OpponentIconTransparency",    false);
static ConfigValue<bool> cfgBlitzIconTransparency      ("BlitzIconTransparency",       false);
static ConfigValue<int> cfgOpponentIconStyle           ("OpponentIconStyle",           0);
static ConfigValue<int> cfgOpponentIconColor           ("OpponentIconColor",           5);
static ConfigValue<int> cfgBlitzIconColor              ("BlitzIconColor",              6);

unsigned int IconColors[8] = {
    0xFF0000EF, // Blue
    0xFF00EF00, // Green
    0xFFEF0000, // Red
    0xFFFFFF00, // Yellow
    0xFFFF5A00, // Orange
    0xFFB400FF, // Purple
    0xFF00FFFF, // Aqua
    0xFFFF0390, // Pink
};

unsigned int SemiTransIconColors[8] = {
    0x800000EF, // Blue
    0x8000EF00, // Green
    0x80EF0000, // Red
    0x80FFFF00, // Yellow
    0x80FF5A00, // Orange
    0x80B400FF, // Purple
    0x8000FFFF, // Aqua
    0x80FF0390, // Pink
};

void mmIconsHandler::RegisterOpponents(OppIconInfo *icons, int count, void *a3) {
    for (int i = 0; i < count; i++) {
        if (opponentIconTransparency) {
            if (opponentIconStyle == 0) {
                icons[i].Color = SemiTransIconColors[5];
            }
            if (opponentIconStyle == 1) {
                icons[i].Color = SemiTransIconColors[i];
            }
            if (opponentIconStyle == 2) {
                icons[i].Color = SemiTransIconColors[1];
            }
            if (opponentIconStyle == 3) {
                icons[i].Color = 0x80C0EC42; // semi-transparent Light Green
            }
            if (opponentIconStyle == 4) {
                icons[i].Color = SemiTransIconColors[4];
            }
            if (opponentIconStyle >= 5) {
                icons[i].Color = SemiTransIconColors[opponentIconColor];
            }
        }
        else {
            if (opponentIconStyle == 0) {
                icons[i].Color = IconColors[5];
            }
            if (opponentIconStyle == 1) {
                icons[i].Color = IconColors[i];
            }
            if (opponentIconStyle == 2) {
                icons[i].Color = IconColors[1];
            }
            if (opponentIconStyle == 3) {
                icons[i].Color = 0xFFC0EC42; // Light Green
            }
            if (opponentIconStyle == 4) {
                icons[i].Color = IconColors[4];
            }
            if (opponentIconStyle >= 5) {
                icons[i].Color = IconColors[opponentIconColor];
            }
        }
    }

    //call original
    hook::Thunk<0x4322F0>::Call<void>(this, icons, count, a3);
}

void mmIconsHandler::RegisterOpponentsBlitz(OppIconInfo *icons, int count, void *a3) {
    for (int i = 0; i < count; i++) {
        if (blitzIconTransparency) {
            if (blitzIconMultiColor) {
                icons[i].Color = SemiTransIconColors[i];
            }
            if (!blitzIconMultiColor) {
                icons[i].Color = SemiTransIconColors[blitzIconColor];
            }
        }
        if (!blitzIconTransparency) {
            if (blitzIconMultiColor) {
                icons[i].Color = IconColors[i];
            }
            if (!blitzIconMultiColor) {
                icons[i].Color = IconColors[blitzIconColor];
            }
        }
    }

    //call original
    hook::Thunk<0x4322F0>::Call<void>(this, icons, count, a3);
}

void mmIconsHandler::Install() {
    opponentIconStyle = cfgOpponentIconStyle.Get();
    opponentIconColor = cfgOpponentIconColor.Get();
    opponentIconTransparency = cfgOpponentIconTransparency.Get();
    blitzIconColor = cfgBlitzIconColor.Get();
    blitzIconMultiColor = cfgBlitzIconMultiColor.Get();
    blitzIconTransparency = cfgBlitzIconTransparency.Get();

    InstallCallback("mmIcons::RegisterOpponents",
        &RegisterOpponents, {
            cb::call(0x412C40), // mmGame::Init
        }
    );

    InstallCallback("mmIcons::RegisterOpponents [mmSingleBlitz]",
        &RegisterOpponentsBlitz, {
            cb::call(0x41B065), // mmSingleBlitz::Init
        }
    );
}

/*
    mmDashViewHandler
*/

static Matrix34 sm_DashOffset;

static ConfigValue<bool> cfgEnableHeadBobbing       ("EnableHeadBobbing",           true);

static ConfigValue<float> cfgHeadBobOffsetScaleY    ("HeadBobOffsetScaleY",         0.0125f);
static ConfigValue<float> cfgHeadBobOffsetScaleZ    ("HeadBobOffsetScaleZ",         0.0125f);

static ConfigValue<float> cfgHeadBobSteeringFactor  ("HeadBobSteeringFactor",       0.001875f);

static ConfigValue<float> cfgHeadBobSteeringSpeedFactor
                                                    ("HeadBobSteeringSpeedFactor",  1.125f);

static ConfigValue<float> cfgHeadBobVelocityScaleY  ("HeadBobVelocityScaleY",       0.00075f);
static ConfigValue<float> cfgHeadBobVelocityScaleZ  ("HeadBobVelocityScaleZ",       0.00725f);

static ConfigValue<float> cfgHeadBobMultiplierY     ("HeadBobMultiplierY",          1.0f);
static ConfigValue<float> cfgHeadBobMultiplierZ     ("HeadBobMultiplierZ",          1.0f);

void mmDashViewHandler::UpdateCS() {
    auto dashCam = getPtr<Matrix34>(this, 0x18);
    auto player = *getPtr<mmPlayer *>(this, 0x80);

    auto car = player->getCar();
    auto carModel = car->getModel();

    if (cfgEnableHeadBobbing.Get())
    {
        sm_DashOffset = carModel->GetMatrix(&sm_DashOffset);

        auto steering = *getPtr<float>(player, 0x2264);
        auto wheelFact = *getPtr<float>(this, 0x400);

        Vector3 velocity = carModel->GetVelocity();

        auto velY = (velocity.Y * cfgHeadBobVelocityScaleY);
        auto velZ = (velocity.Z - (velocity.Y + velocity.X)) * -cfgHeadBobVelocityScaleZ;

        auto bodyRoll = -(steering * wheelFact) * (cfgHeadBobSteeringFactor * (cfgHeadBobSteeringSpeedFactor * velZ));

        auto headBobY = ((sm_DashOffset.m31 - dashCam->m31) * -cfgHeadBobOffsetScaleY) + velY + bodyRoll;
        auto headBobZ = ((sm_DashOffset.m32 - dashCam->m32) * -cfgHeadBobOffsetScaleZ) * velZ;

        dashCam->m31 += (headBobY * cfgHeadBobMultiplierY);
        dashCam->m32 += (headBobZ * cfgHeadBobMultiplierZ);
    }

    if (vehCarModel::WheelWobble && car->getCarSim()->OnGround())
    {
        Vector3 vec;

        float rotation = (fabs(car->getCarSim()->getWheel(1)->getRotationRate()) * datTimeManager::Seconds - 1.5707964f) * 0.31830987f;

        float rotationAmount = fmaxf(0.f, fminf(rotation, 1.f));
        float invRotationAmount = 1.f - rotationAmount;

        vec.X = dashCam->m20 + dashCam->m00;
        vec.Y = dashCam->m21 + dashCam->m01;
        vec.Z = dashCam->m22 + dashCam->m02;

        auto carDamage = car->getCarDamage();
        float damagePercent = (carDamage->getCurDamage() - carDamage->getMedDamage()) / (carDamage->getMaxDamage() - carDamage->getMedDamage());

        damagePercent = fmaxf(0.f, fminf(damagePercent, 1.f));

        float wobbleAngle = sin(car->getCarSim()->getWheel(1)->getAccumulatedRotation()) * damagePercent * invRotationAmount * 0.005f;

        if (!vehCarModel::MM1StyleWobble)
        {
            auto wheelWobbleLimit = car->getCarSim()->getWheel(1)->getWobbleLimit();
            auto wheelWobbleAmount = car->getCarSim()->getWheel(1)->getWobbleAmount();

            wobbleAngle = sin(car->getCarSim()->getWheel(1)->getAccumulatedRotation()) * damagePercent * wheelWobbleLimit * wheelWobbleAmount * 0.05f;
        }

        dashCam->Rotate(vec, wobbleAngle);
    }

    hook::Thunk<0x4A3370>::Call<void>(this);
}

void mmDashViewHandler::FileIO(datParser* parser) {
    //call original FileIO
    hook::Thunk<0x4315D0>::Call<void>(this, parser);

    //add missing things
    parser->AddValue("MaxSpeed", getPtr<float>(this, 0x5D0), 1);
    parser->AddValue("MaxRPM", getPtr<float>(this, 0x5D4), 1);
}

static hook::Type<int> m_TouchedMask(0x685780);
static hook::Type<int[2]> m_TouchedMasks(0x5CD604);
Matrix34 dashMatrix = Matrix34();
modShader *dGearShader;
modShader *pGearShader;

void mmDashViewHandler::Init(char* vehName, mmPlayer* player) {
    //call original
    hook::Thunk<0x430890>::Call<void>(this, vehName, player);

    auto dashView = reinterpret_cast<mmDashView*>(this);

    if (!dashView->field_604)
        return;

    auto model = dashView->Player->getCar()->getModel();
    int geomId = model->GetGeomIndex();
    auto geomTableEntry = lvlInstance::GetGeomTableEntry(geomId);
    int shaderCount = geomTableEntry->numShaders;
    auto shaderSet = *dashView->ShaderSet;

    dGearShader = new modShader[shaderCount];
    pGearShader = new modShader[shaderCount];

    for (int i = 0; i < shaderCount; i++)
    {
        dGearShader[i].Texture = shaderSet[i].Texture;
        dGearShader[i].Material = shaderSet[i].Material;

        pGearShader[i].Texture = shaderSet[i].Texture;
        pGearShader[i].Material = shaderSet[i].Material;
    }

    for (int i = 0; i < dashView->GearIndicatorModStatic->PacketCount; i++)
    {
        int shaderIndex = dashView->GearIndicatorModStatic->ShaderIndices[i];
        modShader* dShader = &dGearShader[shaderIndex];
        dShader->PreLoad();

        if (dShader->Texture != nullptr)
        {
            char gearTextureName[128];

            strcpy_s(gearTextureName, dShader->Texture->Name);

            char* r = strrchr(gearTextureName, 'r');
            char* R = strrchr(gearTextureName, 'R');

            if (r != NULL && !_strcmpi(r, "r"))
            {
                strncpy(r, "d", 1);

                gfxTexture* gearTexture = gfxGetTexture(gearTextureName, 1);

                if (gearTexture != nullptr)
                    dShader->Texture = gearTexture;
                else
                    dShader->Texture = gfxGetTexture("gear_d", 1);
            }
            else if (R != NULL && !_strcmpi(R, "R"))
            {
                strncpy(R, "D", 1);

                gfxTexture* gearTexture = gfxGetTexture(gearTextureName, 1);

                if (gearTexture != nullptr)
                    dShader->Texture = gearTexture;
                else
                    dShader->Texture = gfxGetTexture("gear_d", 1);
            }
            else {
                dShader->Texture = gfxGetTexture("d", 1);
            }
        }

        modShader* pShader = &pGearShader[shaderIndex];
        pShader->PreLoad();

        if (pShader->Texture != nullptr)
        {
            char gearTextureName[128];

            strcpy_s(gearTextureName, pShader->Texture->Name);

            char* r = strrchr(gearTextureName, 'r');
            char* R = strrchr(gearTextureName, 'R');

            if (r != NULL && !_strcmpi(r, "r")) {
                strncpy(r, "p", 1);

                gfxTexture* gearTexture = gfxGetTexture(gearTextureName, 1);

                if (gearTexture != nullptr)
                    pShader->Texture = gearTexture;
                else
                    pShader->Texture = gfxGetTexture("gear_p", 1);
            }
            else if (R != NULL && !_strcmpi(R, "R")) {
                strncpy(R, "P", 1);

                gfxTexture* gearTexture = gfxGetTexture(gearTextureName, 1);

                if (gearTexture != nullptr)
                    pShader->Texture = gearTexture;
                else
                    pShader->Texture = gfxGetTexture("gear_p", 1);
            }
            else {
                pShader->Texture = gfxGetTexture("p", 1);
            }
        }
    }
}

void mmDashViewHandler::Cull() {
    auto dashView = reinterpret_cast<mmDashView*>(this);
    int shaderCount = dashView->Player->getCar()->getModel()->GetGeomTablePtr()->numShaders;

    if (dashView->field_604) {
        if (*getPtr<float>(dashView->Player, 0x1D6C) == 0.f) {
            RadialGauge::bDebugPivot = dashView->PivotDebug;
            byte zEnable = RSTATE->Data.ZEnable;
            bool lighting = RSTATE->Data.Lighting;
            bool alphaEnable = RSTATE->Data.AlphaEnable;
            bool zWriteEnable = RSTATE->Data.ZWriteEnable;

            m_TouchedMask = m_TouchedMasks[0];
            if (RSTATE->Data.Lighting)
            {
                RSTATE->Data.Lighting = 0;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
            if (RSTATE->Data.ZEnable)
            {
                RSTATE->Data.ZEnable = 0;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
            if (RSTATE->Data.ZWriteEnable)
            {
                RSTATE->Data.ZWriteEnable = 0;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
            if (!RSTATE->Data.AlphaEnable)
            {
                RSTATE->Data.AlphaEnable = 1;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }

            float nearView = gfxCurrentViewport->getNear();
            if (!(gfxCurrentViewport->getField_128() & 1)) {
                gfxCurrentViewport->Perspective(
                    gfxCurrentViewport->getFov(),
                    gfxCurrentViewport->getAspect(),
                    0.0099999998f,
                    gfxCurrentViewport->getFar());
            }

            //draw dash
            if (dashView->DashModStatic != nullptr && dashView->field_6d8) {
                Matrix44::Convert(gfxRenderState::sm_World, dashView->field_408.field_48);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;
                dashView->DashModStatic->Draw(*dashView->ShaderSet);
            }
            
            //draw roof
            if (dashView->RoofModStatic != nullptr) {
                Matrix44::Convert(gfxRenderState::sm_World, dashView->field_508.field_48);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;
                dashView->RoofModStatic->Draw(*dashView->ShaderSet);
            }

            //draw gear indicator
            if (dashView->GearIndicatorModStatic != nullptr) {
                auto carsim = dashView->Player->getCar()->getCarSim();
                auto transmission = carsim->getTransmission();
                float trottleInput = carsim->getEngine()->getThrottleInput();
                float speedMPH = carsim->getSpeedMPH();
                int gear = transmission->getGear();

                dashMatrix.Set(dashView->field_408.field_48);
                float gearOffsetX = dashMatrix.m00 * dashView->GearPivotOffset.X + dashMatrix.m10 * dashView->GearPivotOffset.Y + dashMatrix.m20 * dashView->GearPivotOffset.Z;
                float gearOffsetY = dashMatrix.m01 * dashView->GearPivotOffset.X + dashMatrix.m11 * dashView->GearPivotOffset.Y + dashMatrix.m21 * dashView->GearPivotOffset.Z;
                float gearOffsetZ = dashMatrix.m02 * dashView->GearPivotOffset.X + dashMatrix.m12 * dashView->GearPivotOffset.Y + dashMatrix.m22 * dashView->GearPivotOffset.Z;
                dashMatrix.m30 += gearOffsetX;
                dashMatrix.m31 += gearOffsetY;
                dashMatrix.m32 += gearOffsetZ;

                Matrix44::Convert(gfxRenderState::sm_World, dashMatrix);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

                if (transmission->IsAuto()) {
                    if (gear == 0) {
                        dashView->GearIndicatorModStatic->Draw(dashView->ShaderSet[gear]);      // R
                    }
                    else if (trottleInput <= 0.f && speedMPH < 1.f) {
                        dashView->GearIndicatorModStatic->Draw(pGearShader);                    // P
                    }
                    else if (gear == 1) {
                        dashView->GearIndicatorModStatic->Draw(dashView->ShaderSet[gear]);      // N
                    }
                    else
                        dashView->GearIndicatorModStatic->Draw(dGearShader);                    // D
                }
                else {
                    dashView->GearIndicatorModStatic->Draw(dashView->ShaderSet[gear]);
                }
            }

            if (dashView->PivotDebug)
                rglDrawAxis(0.1f, dashMatrix);

            //draw speed needle
            dashView->SpeedGauge.Offset.X = dashView->SpeedOffset.X;
            dashView->SpeedGauge.Offset.Y = dashView->SpeedOffset.Y;
            dashView->SpeedGauge.Offset.Z = dashView->SpeedOffset.Z;
            dashView->SpeedGauge.field_d8 = dashView->SpeedPivotOffset.X;
            dashView->SpeedGauge.field_dc = dashView->SpeedPivotOffset.Y;
            dashView->SpeedGauge.field_e0 = dashView->SpeedPivotOffset.Z;
            dashView->SpeedGauge.Cull(&dashView->field_408.field_48);

            //draw RPM needle
            dashView->RPMGauge.Offset.X = dashView->TachOffset.X;
            dashView->RPMGauge.Offset.Y = dashView->TachOffset.Y;
            dashView->RPMGauge.Offset.Z = dashView->TachOffset.Z;
            dashView->RPMGauge.field_d8 = dashView->TachPivotOffset.X;
            dashView->RPMGauge.field_dc = dashView->TachPivotOffset.Y;
            dashView->RPMGauge.field_e0 = dashView->TachPivotOffset.Z;
            dashView->RPMGauge.Cull(&dashView->field_408.field_48);

            //draw damage needle
            dashView->DamageGauge.Offset.X = dashView->DmgOffset.X;
            dashView->DamageGauge.Offset.Y = dashView->DmgOffset.Y;
            dashView->DamageGauge.Offset.Z = dashView->DmgOffset.Z;
            dashView->DamageGauge.field_d8 = dashView->DmgPivotOffset.X;
            dashView->DamageGauge.field_dc = dashView->DmgPivotOffset.Y;
            dashView->DamageGauge.field_e0 = dashView->DmgPivotOffset.Z;
            dashView->DamageGauge.Cull(&dashView->field_408.field_48);

            //draw extra dash
            if (dashView->DashExtraModStatic != nullptr) {
                Matrix44::Convert(gfxRenderState::sm_World, dashView->field_408.field_48);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;
                dashView->DashExtraModStatic->Draw(*dashView->ShaderSet);
            }

            //draw steering wheel
            if (dashView->WheelModStatic != nullptr) {
                auto dWord_6a4 = ((Matrix34*)&dashView->field_6a4);
                dWord_6a4->Identity();

                float wheelPivotOffsetX = dashView->field_588 + dashView->WheelPivotOffset.X;
                float wheelPivotOffsetY = dashView->field_58c + dashView->WheelPivotOffset.Y;
                float wheelPivotOffsetZ = dashView->field_590 + dashView->WheelPivotOffset.Z;

                dWord_6a4->RotateZ(-(*getPtr<float>(dashView->Player, 0x2264) * dashView->WheelFact));

                float wheelOffsetX = wheelPivotOffsetZ * dashView->field_6bc + wheelPivotOffsetY * dashView->field_6b0 + wheelPivotOffsetX * dashView->field_6a4;
                float wheelOffsetY = wheelPivotOffsetZ * dashView->field_6c0 + wheelPivotOffsetY * dashView->field_6b4 + wheelPivotOffsetX * dashView->field_6a8;
                float wheelOffsetZ = wheelPivotOffsetZ * dashView->field_6c4 + wheelPivotOffsetY * dashView->field_6b8 + wheelPivotOffsetX * dashView->field_6ac;

                float wheelPosDifX = wheelPivotOffsetX - wheelOffsetX;
                float wheelPosDifY = wheelPivotOffsetY - wheelOffsetY;
                float wheelPosDifZ = wheelPivotOffsetZ - wheelOffsetZ;

                dashView->field_6c8 += wheelPosDifX;
                dashView->field_6cc += wheelPosDifY;
                dashView->field_6d0 += wheelPosDifZ;

                dWord_6a4->Dot(dashView->field_408.field_48);

                wheelPosDifX = dashView->field_408.field_48.m20 * dashView->WheelPos.Z + dashView->field_408.field_48.m10 * dashView->WheelPos.Y + dashView->field_408.field_48.m00 * dashView->WheelPos.X;
                wheelPosDifY = dashView->field_408.field_48.m21 * dashView->WheelPos.Z + dashView->field_408.field_48.m11 * dashView->WheelPos.Y + dashView->field_408.field_48.m01 * dashView->WheelPos.X;
                wheelPosDifZ = dashView->field_408.field_48.m22 * dashView->WheelPos.Z + dashView->field_408.field_48.m12 * dashView->WheelPos.Y + dashView->field_408.field_48.m02 * dashView->WheelPos.X;

                dWord_6a4->m30 += wheelPosDifX;
                dWord_6a4->m31 += wheelPosDifY;
                dWord_6a4->m32 += wheelPosDifZ;

                Matrix44::Convert(gfxRenderState::sm_World, *dWord_6a4);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;
                dashView->WheelModStatic->Draw(*dashView->ShaderSet);
            }

            if (!(gfxCurrentViewport->getField_128() & 1)) {
                gfxCurrentViewport->Perspective(
                    gfxCurrentViewport->getFov(),
                    gfxCurrentViewport->getAspect(),
                    nearView,
                    gfxCurrentViewport->getFar());
            }

            if (RSTATE->Data.AlphaEnable != alphaEnable)
            {
                RSTATE->Data.AlphaEnable = alphaEnable;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
            if (RSTATE->Data.ZWriteEnable != zWriteEnable)
            {
                RSTATE->Data.ZWriteEnable = zWriteEnable;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
            if (RSTATE->Data.ZEnable != zEnable)
            {
                RSTATE->Data.ZEnable = zEnable;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
            m_TouchedMask = m_TouchedMasks[lighting];
            if (RSTATE->Data.Lighting != lighting)
            {
                RSTATE->Data.Lighting = lighting;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
        }
    }
}

void mmDashViewHandler::Install() {
    if (cfgEnableHeadBobbing.Get() || vehCarModel::WheelWobble) {
        InstallCallback("mmDashView::Update", "Allows for a custom implementation of head-bobbing and wheel wobbling in dashboards.",
            &UpdateCS, {
                cb::call(0x430F87), // replaces call to asLinearCS::Update
            }
        );
    }

    InstallCallback("mmDashView::Init", "Adds the unused D and P gears",
        &Init, {
            cb::call(0x42D60B),
        }
    );

    // rv6 stuff
    InstallVTableHook("mmDashView::FileIO", &FileIO, {
        0x5B0D90
    });

    InstallVTableHook("mmDashView::Cull", &Cull, {
        0x5B0D7C
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
    return hook::StaticThunk<0x4C98D0>::Call<Stream *>(filename, handle, fileMethods);
}

void StreamHandler::Install()
{
    if (cfgUseModsFolder) {
        InstallCallback("Stream::Open", "Allows for files to be overridden using a mods folder.",
            &Open, {
                cb::jmp(0x4C99C0), // Stream::Open(const char *, bool)
            }
        );
    }
}

/*
    TextureVariantHandler
*/
struct variant_info {
    const char *suffix;
    bool canDesaturate;
};
std::vector<variant_info> variant_infos;
bool desaturateDefaultTextures = false;

static gfxImage * (*DefaultLoadImage)(const char *, bool);
static gfxImage * (*DefaultPrepareImage)(gfxImage*, const char *, bool);

hook::Type<bool> EnableTextureVariantHandler(0x6276EC);
hook::Type<bool> AllowDesaturatedTextureVariants(0x6276ED);


std::vector<std::string> split(std::string str, std::string token) {
    std::vector<std::string>result;
    while (str.size()) {
        int index = str.find(token);
        if (index != std::string::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0)result.push_back(str);
        }
        else {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

void TextureVariantHandler::Reset() {
    variant_infos.clear();
    desaturateDefaultTextures = false;
}

//load LT file, and do a variant prepass
void TextureVariantHandler::InitVariantData() {
    variant_infos.clear();
    auto parser = datParser("OwO");

    //records
    char textureVariants[100];
    char textureLuminances[100];
    int defaultSaturated = (dgStatePack::Instance->TimeOfDay != 3) ? 1 : 0;
    parser.AddString("TextureVariants", &textureVariants);
    parser.AddString("TextureLuminances", &textureLuminances);
    parser.AddValue("DefaultLuminance", &defaultSaturated);

    //load file
    int fileId = (dgStatePack::Instance->TimeOfDay * 4) + dgStatePack::Instance->WeatherType;
    string_buf<16> buffer("td%02d", fileId);
    LPCSTR tdExtension = buffer;

    Displayf("Attempting to load texture definition file %s", tdExtension);
    bool loadResult = parser.Load("city", MMSTATE->CityName, tdExtension);

    //deal with loaded values
    desaturateDefaultTextures = defaultSaturated == 0;

    auto tVarStd = std::string(textureVariants);
    auto tLumStd = std::string(textureLuminances);
    auto tVarVec = split(tVarStd, "|");
    auto tLumVec = split(tLumStd, "|");

    if (loadResult && tVarStd.length() > 0) {
        for (size_t i = 0; i < tVarVec.size(); i++) {
            auto vInfo = variant_info();

            auto suffix = tVarVec[i];
            auto suffix_copy = new char[suffix.length() + 1];
            memcpy(suffix_copy, suffix.c_str(), suffix.length() + 1);
            vInfo.suffix = suffix_copy;

            //the luminence array may be a different size or missing altogether
            if (i < tLumVec.size()) {
                auto canDesat = tLumVec[i];
                vInfo.canDesaturate = atof(canDesat.c_str()) < 0.5;
            }
            else {
                vInfo.canDesaturate = false;
            }

            Warningf("Pushing a new variant with suffix %s and desaturate %s", vInfo.suffix, vInfo.canDesaturate ? "true" : "false");
            variant_infos.push_back(vInfo);
        }
    }

    //add defaults
    if (dgStatePack::Instance->WeatherType == 3) {
        auto rVariant = variant_info();
        rVariant.suffix = "fa";
        rVariant.canDesaturate = dgStatePack::Instance->TimeOfDay == 3;
        variant_infos.push_back(rVariant);
    }
    if (dgStatePack::Instance->TimeOfDay == 3) {
        auto nVariant = variant_info();
        nVariant.suffix = "ni";
        nVariant.canDesaturate = false;
        variant_infos.push_back(nVariant);
    }

    //call vehCarAudioContainer::InitStatics, which we hooked
    hook::StaticThunk<0x4D0FF0>::Call<void>();
}

static void Desaturate(gfxImage* result) {
    for (gfxImage *image = result; image != nullptr; image = image->Next) {
        // DesaturateTextureVariant
        hook::StaticThunk<0x442FB0>::Call<void>(image);
    }
}

static bool TextureVariantExists(const char *textureName, const char*variant) {
    string_buf<128> textureVariant("%s_%s", textureName, variant);
    bool exists = datAssetManager::Exists("texture", textureVariant, "tex");
    if (exists)
        return true;
    exists = datAssetManager::Exists("texture", textureVariant, "tga");
    if (exists)
        return true;
    exists = datAssetManager::Exists("texture", textureVariant, "bmp");
    return exists;
}

static bool TryLoadTexVariant(const char *textureName, const char *variant, bool mipmaps, gfxImage **pgfxImage)
{
    string_buf<64> textureVariant("%s_%s", textureName, variant);
    gfxImage *variantTex = DefaultLoadImage(textureVariant, mipmaps);

    if (variantTex != nullptr) {
        //Warningf("[LoadTextureVariant]: Using '%s' variant for texture '%s' (buffer is %s)", variant, textureName, (LPCSTR)textureVariant);
        *pgfxImage = variantTex;
        return true;
    }

    return false;
}

gfxImage * TextureVariantHandler::LoadTextureVariant(const char *textureName, bool mipmaps)
{
    if (EnableTextureVariantHandler)
    {
        gfxImage *variantTex = nullptr;
        for (size_t i = 0; i < variant_infos.size(); i++) {
            if (TryLoadTexVariant(textureName, variant_infos[i].suffix, mipmaps, &variantTex))
                return variantTex;
        }
    }

    auto defaultTex = DefaultLoadImage(textureName, mipmaps);
    return defaultTex;
}

gfxImage * TextureVariantHandler::PrepareTextureVariant(gfxImage* image, const char *textureName, bool mipmaps)
{
    if (EnableTextureVariantHandler)
    {
        //check if this variant is handled manually
        for (size_t i = 0; i < variant_infos.size(); i++) {
            if (TextureVariantExists(textureName, variant_infos[i].suffix)) {
                if (variant_infos[i].canDesaturate)
                    Desaturate(image);

                return image;
            }
        }
    }

    //no variant
    if (desaturateDefaultTextures) {
        Desaturate(image);
    }
    return image;
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
    InstallCallback(InitVariantData, {
        cb::call(0x412746),
        }, "Installs the texture variant init function."
    );

    InstallCallback(InstallTextureVariantHandler, {
            cb::call(0x401599),
        }, "Installs new texture variant handler."
    );

    // handled by cityTimeWeatherLightingHandler
    // leaving this here in case something goes wrong
    //--if (UseNightTexturesInEvening)
    //--{
    //--    LogFile::WriteLine("Installing evening patches...");
    //--
    //--    // aiTrafficLightInstance::DrawGlow
    //--    InstallPatch({ 1 }, {
    //--        0x53CABC + 3
    //--    });
    //--
    //--    /*
    //--        mmGame::InitWeather patches
    //--    */
    //--
    //--    // minimum time of day for night checks
    //--    InstallPatch({ 2 }, {
    //--        0x41338E + 2,
    //--        0x4133BD + 2,
    //--    });
    //--
    //--    // jnz -> jb
    //--    InstallPatch({ 0x72 }, {
    //--        0x41339D,
    //--    });
    //--
    //--    // jz -> jge
    //--    InstallPatch({ 0x7D }, {
    //--        0x4133CA,
    //--    });
    //--}
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

ConfigValue<bool> cfgInstantReplay ("InstantReplay", true);

void PUMainHandler::Install() {
    if (cfgInstantReplay) {
        InstallCallback("PUMain::ctor", "Overrides button placement for the pause menu.",
            &PUMenuHook::AddPauseButton, {
                cb::call(0x50A6AE),
                cb::call(0x50A712),
                cb::call(0x50A776),
                cb::call(0x50A7D0),
            }
        );

        InstallPatch("Add replay button to main menu", { 0x3C }, {
            0x505EC3 + 2, // MainMenu::MainMenu(int)
        });
    }
}

/*
    mmPlayerHandler
*/
static ConfigValue<bool> cfgEnableOutOfMapFix("OutOfMapFix", true);
static ConfigValue<bool> cfgEnableMissingDashboardFix("MissingDashboardFix", true);
bool enableOutOfMapFixCached = true;
bool enableMissingDashboardFixCached = true;

void mmPlayerHandler::Zoink() {
    Warningf("Player is out of the world, teleporting!");

    //get required vars
    auto player = reinterpret_cast<mmPlayer*>(this);
    auto car = player->getCar();
    auto carPos = car->getModel()->GetPosition();

    // tell the player "That didn't happen!"
    player->getHUD()->SetMessage(AngelReadString(29), 3.f, 0);

    //if we're in CNR, drop the gold!
    if (dgStatePack::Instance->GameMode == dgGameMode::CnR) {
        auto game = mmGameManager::Instance->getGame();
        hook::Thunk<0x425460>::ThisCall<void>(game); // mmMultiCR::DropThruCityHandler
    }

    // if the aimap doesn't exist, reset back to spawn
    auto AIMAP = &aiMap::Instance;
    if (AIMAP == NULL) {
        car->Reset();
        return;
    }

    //search for an intersection to teleport to
    float shortestDistance = 99999;
    int closestIntersection = -1;

    for (int is = 0; is < AIMAP->numIntersections; is++) {
        auto intersection = AIMAP->intersections[is];

        // avoid dummy intersections
        if (intersection->pathCount == 0)
            continue;

        // check roads to see if this is a valid spawn point
        // valid == (!freeway && !alley)
        bool isInvalid = false;
        for (int i = 0; i < intersection->pathCount; i++) {
            auto path = intersection->paths[i];
            ushort pathFlags = path->getFlags();

            if (pathFlags & Freeway || pathFlags & Alley) {
                isInvalid = true;
            }
        }

        if (isInvalid)
            continue;

        // this is a valid intersection
        float pDist = intersection->center.Dist(carPos);
        if (pDist < shortestDistance) {
            shortestDistance = pDist;
            closestIntersection = is;
        }
    }

    // move player to the closest intersection if we can
    auto carsim = car->getCarSim();
    if (closestIntersection >= 0) {
        auto oldResetPos = carsim->getResetPosition();

        // set to closest intersection
        carsim->SetResetPos(&AIMAP->intersections[closestIntersection]->center);

        // reset vehicle
        car->Reset();

        // set back
        carsim->SetResetPos(&oldResetPos);
    }
    else
    {
        // reset vehicle to original spawn
        // no intersection found to teleport to
        car->Reset();
    }

}

static ConfigValue<int> cfgBustedTarget("BustedTarget", 3);
static ConfigValue<float> cfgBustedMaxSpeed("BustedMaxSpeed", 20.f);
static ConfigValue<float> cfgBustedTimeout("BustedTimeout", 4.f);
static ConfigValue<float> cfgEscapeTimeout("EscapeTimeout", 20.f);
static ConfigValue<bool> cfgNfsMwStyleEscape("NFSMWStyleEscape", false);
int bustedTarget = 3;
float bustedMaxSpeed = 20.f;
float bustedTimeout = 4.f;
float bustedTimer = 0.f;
float oppBustedTimer = 0.f;
float resetTimer = 0.f;
float cooldownTimer = 0.f;
float cooldownTimeout = 20.f;
float escapeTimer = 0.f;
bool enableBustedTimer = false;
bool invertBustedTimer = false;
bool enableOppBustedTimer = false;
bool invertOppBustedTimer = false;
bool enableResetTimer = false;
bool nfsmwStyleEscape = false;
bool playerInPursuit = false;
bool playerInCooldown = false;
bool enableEscapeTimer = false;
bool invertEscapeTimer = false;

int mmPlayerHandler::GetClosestCop() {
    auto player = reinterpret_cast<mmPlayer*>(this);
    auto playerPos = player->getCar()->getModel()->GetPosition();
    auto AIMAP = &aiMap::Instance;

    int closestCopId = 0;

    for (int i = 0; i < AIMAP->numCops; i++)
    {
        auto police = AIMAP->Police(i);
        auto police2 = AIMAP->Police(closestCopId);

        float policeDist = police->getVehiclePhysics()->getCar()->getModel()->GetPosition().Dist(playerPos);
        float police2Dist = police2->getVehiclePhysics()->getCar()->getModel()->GetPosition().Dist(playerPos);

        if (police2Dist > policeDist)
            closestCopId = i;
    }

    return closestCopId;
}

void mmPlayerHandler::BustPlayer() {
    auto player = reinterpret_cast<mmPlayer*>(this);
    auto carsim = player->getCar()->getCarSim();
    auto playerPos = player->getCar()->getModel()->GetPosition();
    auto AIMAP = &aiMap::Instance;
    auto mgr = *mmGameManager::Instance;
    int numCops = vehPoliceCarAudio::iNumCopsPursuingPlayer;

    if (numCops)
        playerInPursuit = true;

    else
        playerInPursuit = false;

    if (enableBustedTimer)
        bustedTimer += datTimeManager::Seconds;

    if (invertBustedTimer && bustedTimer > 0.f)
        bustedTimer -= datTimeManager::Seconds * 1.5f;

    for (int i = 0; i < AIMAP->numCops; i++)
    {
        auto police = AIMAP->Police(i);
        auto car = police->getVehiclePhysics()->getCar();
        auto policeAud = car->getAudio()->GetPoliceCarAudioPtr();
        auto policePos = car->getModel()->GetPosition();
        auto police2 = AIMAP->Police(GetClosestCop());
        auto copCar = police2->getVehiclePhysics()->getCar();
        auto copCarSim = copCar->getCarSim();
        auto curDamage = copCar->getCarDamage()->getCurDamage();
        auto maxDamage = copCar->getCarDamage()->getMaxDamage();
        auto police2Pos = copCar->getModel()->GetPosition();

        if (vehPoliceCarAudio::iNumCopsPursuingPlayer == 0 && bustedTimer > 0.f) {
            if (lvlLevel::Singleton->GetRoomInfo(copCar->getModel()->GetRoomId())->Flags & Water) {
                if (lvlLevel::Singleton->GetWaterLevel(copCar->getModel()->GetRoomId()) > copCarSim->getWorldMatrix()->m31) {
                    enableBustedTimer = false;
                    bustedTimer = 0.f;
                    enableResetTimer = false;
                    resetTimer = 0.f;
                }
            }
            if (curDamage > maxDamage) {
                enableBustedTimer = false;
                bustedTimer = 0.f;
                enableResetTimer = false;
                resetTimer = 0.f;
            }
        }

        if (*getPtr<int>(player->getCar(), 0xEC) != 0 && !player->IsMaxDamaged() || *getPtr<int>(player, 0x2258))
            continue;

        if (bustedTimer <= bustedTimeout) {
            if (police2->InPersuit() && !police2->Destroyed() && police2->getFollowedCar() == player->getCar())
            {
                lvlSegment segment;
                lvlIntersection intersection;

                police2Pos.Y += 1.f;

                segment.Set(playerPos, police2Pos, 0, nullptr);

                bool PlayerNotInRange = dgPhysManager::Instance->Collide(segment, &intersection, 0, nullptr, SpecialBound, 0);

                if (playerPos.Dist(police2Pos) <= 15.f && carsim->getSpeedMPH() <= bustedMaxSpeed && !PlayerNotInRange) {
                    enableBustedTimer = true;
                    invertBustedTimer = false;
                }
                else {
                    enableBustedTimer = false;
                    invertBustedTimer = true;
                    if (bustedTimer < 0.f)
                        bustedTimer = 0.f;
                }
            }
            else if (police->InPersuit() && !police->Destroyed() && police->getFollowedCar() == player->getCar())
            {
                lvlSegment segment;
                lvlIntersection intersection;

                policePos.Y += 1.f;

                segment.Set(playerPos, policePos, 0, nullptr);

                bool PlayerNotInRange = dgPhysManager::Instance->Collide(segment, &intersection, 0, nullptr, SpecialBound, 0);

                if (playerPos.Dist(policePos) <= 15.f && carsim->getSpeedMPH() <= bustedMaxSpeed && !PlayerNotInRange) {
                    enableBustedTimer = true;
                    invertBustedTimer = false;
                }
                else {
                    enableBustedTimer = false;
                    invertBustedTimer = true;
                    if (bustedTimer < 0.f)
                        bustedTimer = 0.f;
                }
            }
        }
        
        if (police->InPersuit() && !police->Destroyed() && police->getFollowedCar() == player->getCar()) {
            if (bustedTimer > bustedTimeout) {
                auto soundBase = *getPtr<AudSoundBase*>(mgr->getGame(), 0x8C);
                if (!soundBase->IsPlaying()) {
                    int i = irand() % 20 + 1;
                    if (MMSTATE->GameMode == Cruise)
                        soundBase->SetSoundHandleIndex(i + 1);
                    if (MMSTATE->GameMode == Checkpoint)
                        soundBase->SetSoundHandleIndex(i + 6);
                    if (MMSTATE->GameMode == Circuit)
                        soundBase->SetSoundHandleIndex(i + 5);
                    if (MMSTATE->GameMode == Blitz || MMSTATE->GameMode == CrashCourse)
                        soundBase->SetSoundHandleIndex(i + 7);
                    soundBase->PlayOnce(-1.f, -1.f);
                }
                if (policeAud != nullptr) {
                    policeAud->StopSiren();
                }
                player->getHUD()->SetMessage("Busted!", 4.f, 0);
                AIMAP->policeForce->UnRegisterCop(police->getCar(), police->getFollowedCar());
                police->setPoliceState(12);
                police->setState(3);
                vehPoliceCarAudio::iNumCopsPursuingPlayer = 0;
                enableBustedTimer = false;
                enableResetTimer = true;
                playerInPursuit = false;
            }
        }
    }
}

int mmPlayerHandler::GetClosestOpp() {
    auto player = reinterpret_cast<mmPlayer*>(this);
    auto playerPos = player->getCar()->getModel()->GetPosition();
    auto AIMAP = &aiMap::Instance;

    int closestOppId = 0;

    for (int i = 0; i < AIMAP->numOpponents; i++)
    {
        auto opponent = AIMAP->Opponent(i);
        auto opponent2 = AIMAP->Opponent(closestOppId);

        float opponentDist = opponent->getCar()->getModel()->GetPosition().Dist(playerPos);
        float opponent2Dist = opponent2->getCar()->getModel()->GetPosition().Dist(playerPos);

        if (opponent2Dist > opponentDist)
            closestOppId = i;
    }

    return closestOppId;
}

void mmPlayerHandler::BustOpp() {
    auto player = reinterpret_cast<mmPlayer*>(this);
    auto AIMAP = &aiMap::Instance;

    if (AIMAP->numOpponents == 0)
        return;

    auto car = player->getCar();
    auto audio = car->getAudio();
    auto siren = car->getSiren();
    auto playerPos = car->getModel()->GetPosition();
    auto opponent = AIMAP->Opponent(GetClosestOpp());
    auto oppCar = opponent->getCar();
    auto carsim = oppCar->getCarSim();
    auto opponentPos = oppCar->getModel()->GetPosition();
    
    if (enableOppBustedTimer)
        oppBustedTimer += datTimeManager::Seconds;

    if (invertOppBustedTimer && oppBustedTimer > 0.f)
        oppBustedTimer -= datTimeManager::Seconds * 1.5f;

    if (*getPtr<int>(oppCar, 0xEC) == 0) {
        if (opponent->getState() != 3) {
            if (opponentPos.Dist(playerPos) <= 15.f && carsim->getSpeedMPH() <= bustedMaxSpeed) {
                enableOppBustedTimer = true;
                invertOppBustedTimer = false;
                if (oppBustedTimer > bustedTimeout) {
                    opponent->setState(3);
                    siren->setActive(false);
                    audio->StopSiren();
                    enableOppBustedTimer = false;
                    invertOppBustedTimer = false;
                    oppBustedTimer = 0.f;
                }
            }
            else {
                enableOppBustedTimer = false;
                invertOppBustedTimer = true;
                if (oppBustedTimer < 0.f)
                    oppBustedTimer = 0.f;
            }
        }
    }
}

void mmPlayerHandler::Cooldown() {
    auto player = reinterpret_cast<mmPlayer*>(this);
    auto playerPos = player->getCar()->getModel()->GetPosition();
    auto AIMAP = &aiMap::Instance;

    if (enableEscapeTimer)
        escapeTimer += datTimeManager::Seconds;

    if (invertEscapeTimer && escapeTimer > 0.f)
        escapeTimer -= datTimeManager::Seconds;

    if (escapeTimer > 4.f) {
        escapeTimer = 4.f;
        playerInCooldown = true;
    }

    else {
        playerInCooldown = false;
    }

    if (playerInCooldown)
        cooldownTimer += datTimeManager::Seconds;

    for (int i = 0; i < AIMAP->numCops; i++)
    {
        auto police = AIMAP->Police(i);
        auto police2 = AIMAP->Police(GetClosestCop());

        if (police2->InPersuit() && !police2->Destroyed() && police2->getFollowedCar() == player->getCar())
        {
            lvlSegment segment;
            lvlIntersection intersection;

            Vector3 police2Pos = police2->getCar()->getModel()->GetPosition();

            police2Pos.Y += 1.f;

            segment.Set(playerPos, police2Pos, 0, nullptr);

            bool PlayerNotInRange = dgPhysManager::Instance->Collide(segment, &intersection, 0, nullptr, SpecialBound, 0);

            if (PlayerNotInRange)
                police2->ApprehendPerpetrator();

            if (police2->getFollowCarDistance() > AIMAP->raceData->getCopChaseDistance() || PlayerNotInRange)
            {
                enableEscapeTimer = true;
                invertEscapeTimer = false;
            }
            else if (police2->getFollowCarDistance() <= (AIMAP->raceData->getCopChaseDistance() * 0.5f)) {
                enableEscapeTimer = false;
                invertEscapeTimer = true;
                if (escapeTimer < 0.f)
                    escapeTimer = 0.f;
                if (cooldownTimer > 0.f)
                    cooldownTimer = 0.f;
            }
        }
        else if (police->InPersuit() && !police->Destroyed() && police->getFollowedCar() == player->getCar())
        {
            lvlSegment segment;
            lvlIntersection intersection;

            Vector3 policePos = police->getCar()->getModel()->GetPosition();

            policePos.Y += 1.f;

            segment.Set(playerPos, policePos, 0, nullptr);

            bool PlayerNotInRange = dgPhysManager::Instance->Collide(segment, &intersection, 0, nullptr, SpecialBound, 0);

            if (PlayerNotInRange)
                police->ApprehendPerpetrator();

            if (police->getFollowCarDistance() > AIMAP->raceData->getCopChaseDistance() || PlayerNotInRange)
            {
                enableEscapeTimer = true;
                invertEscapeTimer = false;
            }
            else if (police->getFollowCarDistance() <= (AIMAP->raceData->getCopChaseDistance() * 0.5f)) {
                enableEscapeTimer = false;
                invertEscapeTimer = true;
                if (escapeTimer < 0.f)
                    escapeTimer = 0.f;
                if (cooldownTimer > 0.f)
                    cooldownTimer = 0.f;
            }
        }

        if (cooldownTimer > cooldownTimeout)
        {
            if (police->InPersuit() && !police->Destroyed() && police->getFollowedCar() == player->getCar())
            {
                police->StopSiren();
                AIMAP->policeForce->UnRegisterCop(police->getCar(), police->getFollowedCar());
                police->setPoliceState(0);
                police->setState(3);
                vehPoliceCarAudio::iNumCopsPursuingPlayer = 0;
            }
            playerInCooldown = false;
            playerInPursuit = false;
        }
    }

    if (!playerInCooldown)
        cooldownTimer = 0.f;
}

void mmPlayerHandler::ControlTrailerJoint()
{
    auto player = reinterpret_cast<mmPlayer*>(this);
    auto trailer = player->getCar()->getTrailer();
    auto trailerJoint = trailer->getTrailerJoint();
    auto carHitchOffset = trailer->getCarHitchOffset();
    auto trailerHitchOffset = trailer->getTrailerHitchOffset();
    auto carMatrix = trailerJoint->getCarSim()->getICS()->GetMatrix();
    auto trailerMatrix = trailer->getICS()->GetMatrix();

    Vector3 carHitchPos;
    Vector3 trailerHitchPos;

    carHitchPos.Dot(carHitchOffset, *carMatrix);
    trailerHitchPos.Dot(trailerHitchOffset, *trailerMatrix);

    if (!trailerJoint->IsBroken())
    {
        if ((ioKeyboard::GetKeyState(DIK_LCONTROL) || ioKeyboard::GetKeyState(DIK_RCONTROL)) && ioKeyboard::GetKeyState(DIK_B))
        {
            trailerJoint->BreakJoint();
        }
    }

    if (carHitchPos.Dist(trailerHitchPos) <= 0.75f)
    {
        if ((ioKeyboard::GetKeyState(DIK_LCONTROL) || ioKeyboard::GetKeyState(DIK_RCONTROL)) && ioKeyboard::GetKeyState(DIK_N))
        {
            trailerJoint->UnbreakJoint();
        }
    }
}

static ConfigValue<bool> cfgMm1StyleFlipOver("MM1StyleFlipOver", false);
bool mm1StyleFlipOver = false;

void mmPlayerHandler::Update() {
    auto player = reinterpret_cast<mmPlayer*>(this);
    auto car = player->getCar();
    auto audio = car->getAudio();
    auto siren = car->getSiren();
    auto carsim = car->getCarSim();
    auto trailer = car->getTrailer();
    auto engine = carsim->getEngine();
    auto basename = player->getCar()->getCarDamage()->GetName();
    auto flagsId = VehicleListPtr->GetVehicleInfo(basename)->GetFlags();

    //check if we're out of the level
    int playerRoom = car->GetInst()->GetRoomId();
    if (playerRoom == 0 && enableOutOfMapFixCached) {
        Zoink();
    }

    //check if dashboard model is missing
    if (enableMissingDashboardFixCached) {
        if (!player->getHUD()->getDashView()->field_604) {
            if (MMSTATE->ShowDash) {
                player->getHUD()->DeactivateDash();
                player->getCamView()->SetCam(player->getPovCam());
            }
        }
    }

    if (bustedTarget != 0) {
        if (bustedTarget >= 2) {
            if (audio->IsPolice(basename) && flagsId == 8) {
                auto AIMAP = &aiMap::Instance;

                for (int i = 0; i < AIMAP->numOpponents; i++)
                {
                    short flags = AIMAP->Opponent(i)->getCar()->getModel()->GetFlags();

                    if (flags & INST_FLAG_8000) // if opponent hit me
                    {
                        siren->setActive(true);
                        audio->StartSiren();
                    }
                }

                if (siren != nullptr && siren->getActive())
                    BustOpp();
            }
        }

        if (bustedTarget == 1 || bustedTarget >= 3) {
            if (!audio->IsPolice(basename)) {
                BustPlayer();
                if (bustedTimer > bustedTimeout) {
                    carsim->setBrake(1.f);
                    engine->setThrottleInput(0.f);
                }
                if (nfsmwStyleEscape)
                    Cooldown();
            }

            if (enableResetTimer) {
                resetTimer += datTimeManager::Seconds;
                if (resetTimer > 4.f) {
                    mmGameManager *mgr = mmGameManager::Instance;
                    auto game = mgr->getGame();
                    auto soundBase = *getPtr<AudSoundBase*>(game, 0x8C);
                    if (MMSTATE->GameMode == Cruise) {
                        *getPtr<byte>(mmReplayManager::Instance, 0x19) = 1;
                        soundBase->SetSoundHandleIndex(1);
                        soundBase->PlayOnce(-1.f, -1.f);
                    }
                    else {
                        if (MMSTATE->GameMode == Checkpoint)
                            soundBase->SetSoundHandleIndex(6);
                        if (MMSTATE->GameMode == Circuit)
                            soundBase->SetSoundHandleIndex(5);
                        if (MMSTATE->GameMode == Blitz || MMSTATE->GameMode == CrashCourse) {
                            soundBase->SetSoundHandleIndex(7);
                            player->getHUD()->getTimer()->Stop();
                            player->getHUD()->getTimer2()->Stop();
                            player->getHUD()->getTimer3()->Stop();
                        }
                        soundBase->PlayOnce(-1.f, -1.f);
                        game->getPopup()->ProcessEscape(0);
                        player->getHUD()->StopTimers();
                        enableResetTimer = false;
                        resetTimer = 0.f;
                    }
                }
            }
        }
    }

    if (playerCanFly)
    {
        if (engine->getThrottleInput() > 0.f && carsim->getSpeed() < 50.f && !player->IsMaxDamaged())
        {
            carsim->getICS()->SetScaledVelocity(carsim->getICS()->GetScaledVelocity() * 1.03f);
        }
    }

    if (mm1StyleFlipOver && carsim->getWorldMatrix()->m11 <= 0.f) {
        car->getStuck()->setStuckTime(0.f);
    }

    if (trailer != nullptr) {
        ControlTrailerJoint();
    }

    //call original
    hook::Thunk<0x405760>::Call<void>(this);
}

void mmPlayerHandler::Reset() {
    // disable and reset timers
    enableBustedTimer = false;
    enableOppBustedTimer = false;
    enableResetTimer = false;
    enableEscapeTimer = false;
    playerInCooldown = false;
    playerInPursuit = false;
    bustedTimer = 0.f;
    oppBustedTimer = 0.f;
    resetTimer = 0.f;
    cooldownTimer = 0.f;
    escapeTimer = 0.f;

    // call original
    hook::Thunk<0x404A60>::Call<void>(this);
}

static ConfigValue<bool> cfgAmbientSoundsWithMusic ("AmbientSoundsWithMusic", true);
static ConfigValue<bool> cfgEnableModelVisibility ("ModelVisibility", false);

void mmPlayerHandler::Install() {
    enableOutOfMapFixCached = cfgEnableOutOfMapFix.Get();
    enableMissingDashboardFixCached = cfgEnableMissingDashboardFix.Get();
    bustedTarget = cfgBustedTarget.Get();
    bustedMaxSpeed = cfgBustedMaxSpeed.Get();
    bustedTimeout = cfgBustedTimeout.Get();
    cooldownTimeout = cfgEscapeTimeout.Get();
    mm1StyleFlipOver = cfgMm1StyleFlipOver.Get();
    nfsmwStyleEscape = cfgNfsMwStyleEscape.Get();

    InstallVTableHook("mmPlayer::Update",
        &Update, {
            0x5B03BC
        }
    );

    if (cfgAmbientSoundsWithMusic) {
        InstallPatch("Enables positional ambient sounds with music.", { 0x90, 0x90 }, {
            0x404044,
        });
    }

    InstallVTableHook("mmPlayer::Reset",
        &Reset, {
            0x5B03C0,
        }
    );

    if (cfgEnableModelVisibility.Get()) {
        InstallPatch("Enables vehicle's body visibility from the dashboard and bumper camera.", { 0x90, 0x90, 0x90 }, {
            0x405ED4,
        });
    }

    if (cfgMm1StyleFlipOver.Get()) {
        //fix collision detection
        InstallPatch({ 0x8B, 0x81, 0xF4, 0x0, 0x0, 0x0 }, {
            0x40493F, // mmPlayer::UpdateHOG
        });
    }
}

/*
    mmCityListHandler
*/

void mmCityListHandler::Load(char* cinfoName) {
    CityListPtr->Load("sf.cinfo");
    CityListPtr->Load("london.cinfo");
}

void mmCityListHandler::Install() {
    InstallCallback("mmCityList::LoadAll", "Makes London the 2nd city in the list.",
        &mmCityListHandler::Load, {
            cb::call(0x5244FE),
        }
    );
}

/*
    Dialog_NewPlayerHandler
*/

void Dialog_NewPlayerHandler::Install() {
    InstallPatch("Enables tooltips in the new player dialog.", { 0x90, 0x90, 0x90 }, {
        0x4FD823,
    });
}

/*
    ltLensFlareHandler
*/

static ConfigValue<bool> cfgEnableLensFlare("LensFlare", false);

void ltLensFlareHandler::Install() {
    if (cfgEnableLensFlare.Get()) {
        InstallPatch("Enables lens flares in a semi broken fashion.", { 0x90, 0x90, 0x90, 0x90, 0x90 }, {
            0x59C1EC,
        });
    }
}

/*
    ltLightHandler
*/

gfxTexture* RedGlowTexture;
gfxTexture* HeadlightTexture;
gfxTexture* SirenTexture;
gfxTexture* AmbientTexture;

ltLight* ltLightHandler::Constructor()
{
    auto light = reinterpret_cast<ltLight*>(this);
    light->Default();

    if (!ltLight::GlowTexture)
        ltLight::GlowTexture = gfxGetTexture("lt_glow", true);

    light->HighlightTexture = gfxGetTexture("lt_highlight", true);

    if (!RedGlowTexture)
        RedGlowTexture = gfxGetTexture("s_red_glow", true);

    if (!HeadlightTexture)
        HeadlightTexture = gfxGetTexture("lt_glow_headlight", true);

    if (!SirenTexture)
        SirenTexture = gfxGetTexture("lt_glow_siren", true);

    if (!AmbientTexture)
        AmbientTexture = gfxGetTexture("lt_glow_ambient", true);

    return light;
}

void ltLightHandler::ShutdownLights()
{
    if (ltLight::GlowTexture)
    {
        gfxFreeTexture((gfxTexture*)ltLight::GlowTexture);
        ltLight::GlowTexture = NULL;
    }

    if (RedGlowTexture)
    {
        gfxFreeTexture(RedGlowTexture);
        RedGlowTexture = NULL;
    }

    if (HeadlightTexture)
    {
        gfxFreeTexture(HeadlightTexture);
        HeadlightTexture = NULL;
    }

    if (SirenTexture)
    {
        gfxFreeTexture(SirenTexture);
        SirenTexture = NULL;
    }

    if (AmbientTexture)
    {
        gfxFreeTexture(AmbientTexture);
        AmbientTexture = NULL;
    }
}

void ltLightHandler::Install()
{
    InstallCallback("ltLight::ltLight", "Loads custom lt glow textures.",
        &Constructor, {
            cb::push(0x4CD56C),
            cb::push(0x4D66C4),
            cb::call(0x553D16),
        }
    );

    InstallCallback("ltLight::ShutdownLights", "Frees custom lt glow textures.",
        &ShutdownLights, {
            cb::call(0x4CCFB6),
            cb::call(0x4D6670),
            cb::call(0x553C7D),
        }
    );
}

/*
    mmSingleRaceHandler
*/

void mmSingleRaceHandler::QueueCopVoice(float a1) {
    //play damage out voice
    auto rsPtr = AudManager::Instance->GetRaceSpeechPtr();
    if (rsPtr != nullptr)
        rsPtr->PlayDamagePenalty();
}

void mmSingleRaceHandler::SetPriority(int a1) {
    auto soundBase = reinterpret_cast<AudSoundBase*>(this);
    soundBase->Load("arrest", 6, false);
    soundBase->SetVolume(1.f);

    for (int i = 1; i <= 20; i++)
    {
        string_buf<128> buffer("ACOPAPP%02d%s", i, MMSTATE->CityName);
        soundBase->Load(buffer, i + 6, false);
        soundBase->SetVolume(1.f);
    }

    soundBase->SetPriority(a1);
}

void mmSingleRaceHandler::Install() {
    InstallCallback("mmSingleRace::UpdateGame", "Plays damage out voices in checkpoint race.",
        &QueueCopVoice, {
            cb::call(0x41E9EF),
        }
    );

    InstallCallback("mmSingleRace::InitGameObjects", "Implements arrest and cop speech wav sounds in checkpoint race.",
        &SetPriority, {
            cb::call(0x41E48F),
        }
    );

    InstallPatch("Skips Aud3DObjectManager check, since we aren't using it.", { 0x90, 0x90 }, {
        0x41E9E8,
    });

    //increase num wav sounds
    InstallPatch({ 27 }, {
        0x41E393 + 1,
    });
}

/*
    mmSingleBlitzHandler
*/

void mmSingleBlitzHandler::SetPriority(int a1) {
    auto soundBase = reinterpret_cast<AudSoundBase*>(this);
    soundBase->Load("arrest", 7, false);
    soundBase->SetVolume(1.f);

    for (int i = 1; i <= 20; i++)
    {
        string_buf<128> buffer("ACOPAPP%02d%s", i, MMSTATE->CityName);
        soundBase->Load(buffer, i + 7, false);
        soundBase->SetVolume(1.f);
    }

    soundBase->SetPriority(a1);
}

void mmSingleBlitzHandler::Install() {
    InstallCallback("mmSingleBlitz::InitGameObjects", "Implements arrest and cop speech wav sounds in blitz race.",
        &SetPriority, {
            cb::call(0x41B30A),
        }
    );

    //increase num wav sounds
    InstallPatch({ 28 }, {
        0x41B1EE + 1,
    });
}

/*
    mmSingleCircuitHandler
*/

void mmSingleCircuitHandler::SetPriority(int a1) {
    auto soundBase = reinterpret_cast<AudSoundBase*>(this);
    soundBase->Load("arrest", 5, false);
    soundBase->SetVolume(1.f);

    for (int i = 1; i <= 20; i++)
    {
        string_buf<128> buffer("ACOPAPP%02d%s", i, MMSTATE->CityName);
        soundBase->Load(buffer, i + 5, false);
        soundBase->SetVolume(1.f);
    }

    soundBase->SetPriority(a1);
}

void mmSingleCircuitHandler::Install() {
    InstallCallback("mmSingleCircuit::InitGameObjects", "Implements arrest and cop speech wav sounds in circuit race.",
        &SetPriority, {
            cb::call(0x41C9EF),
        }
    );

    //increase num wav sounds
    InstallPatch({ 26 }, {
        0x41C917 + 1,
    });
}

/*
    mmSingleStuntHandler
*/

void mmSingleStuntHandler::SetPriority(int a1) {
    auto soundBase = reinterpret_cast<AudSoundBase*>(this);
    soundBase->Load("arrest", 7, false);
    soundBase->SetVolume(1.f);

    for (int i = 1; i <= 20; i++)
    {
        string_buf<128> buffer("ACOPAPP%02d%s", i, MMSTATE->CityName);
        soundBase->Load(buffer, i + 7, false);
        soundBase->SetVolume(1.f);
    }

    soundBase->SetPriority(a1);
}

void mmSingleStuntHandler::Install() {
    InstallCallback("mmSingleStunt::InitGameObjects", "Implements arrest and cop speech wav sounds in crash course.",
        &SetPriority, {
            cb::call(0x4166EE),
        }
    );

    //increase num wav sounds
    InstallPatch({ 28 }, {
        0x4165DA + 1,
    });
}

/*
    mmSingleRoamHandler
*/

static ConfigValue<bool> cfgEscapeDeepWater("EscapeDeepWater", true);
static ConfigValue<bool> cfgResetToNearestLocation("ResetToNearestLocation", false);

void mmSingleRoamHandler::ResetToNearestLocation() {
    auto singleRoam = reinterpret_cast<mmSingleRoam*>(this);
    auto player = singleRoam->getPlayer();
    auto car = player->getCar();
    auto carsim = car->getCarSim();
    auto carPos = car->getModel()->GetPosition();

    // if the aimap doesn't exist, reset back to spawn
    auto AIMAP = &aiMap::Instance;
    if (AIMAP == NULL) {
        player->Reset();
        return;
    }

    //search for an intersection to teleport to
    float shortestDistance = 99999;
    int closestIntersection = -1;

    for (int is = 0; is < AIMAP->numIntersections; is++) {
        auto intersection = AIMAP->intersections[is];

        // avoid dummy intersections
        if (intersection->pathCount == 0)
            continue;

        // check roads to see if this is a valid spawn point
        // valid == (!freeway && !alley)
        bool isInvalid = false;
        for (int i = 0; i < intersection->pathCount; i++) {
            auto path = intersection->paths[i];
            ushort pathFlags = path->getFlags();

            if (pathFlags & Freeway || pathFlags & Alley) {
                isInvalid = true;
            }
        }

        if (isInvalid)
            continue;

        // this is a valid intersection
        float pDist = intersection->center.Dist(carPos);
        if (pDist < shortestDistance) {
            shortestDistance = pDist;
            closestIntersection = is;
        }
    }

    // move player to the closest intersection if we can
    if (closestIntersection >= 0) {
        auto oldResetPos = carsim->getResetPosition();

        // set to closest intersection
        carsim->SetResetPos(&AIMAP->intersections[closestIntersection]->center);

        // reset vehicle
        player->Reset();

        // set back
        carsim->SetResetPos(&oldResetPos);
    }
    else
    {
        // reset vehicle to original spawn
        // no intersection found to teleport to
        player->Reset();
    }
}

void mmSingleRoamHandler::EscapeDeepWater() {
    auto singleRoam = reinterpret_cast<mmSingleRoam*>(this);
    auto player = singleRoam->getPlayer();
    auto car = player->getCar();
    auto carsim = car->getCarSim();
    auto carPos = car->getModel()->GetPosition();
    auto level = *lvlLevel::Singleton;

    if (level->GetRoomInfo(car->getModel()->GetRoomId())->Flags & Water) {
        if (cfgResetToNearestLocation.Get()) {
            ResetToNearestLocation();
        }
        else {
            player->Reset();
        }
    }
    else {
        //update splash cam
        *getPtr<int>(player, 0x2344) = 0;

        //recheck if player went back into water
        *getPtr<int>(player, 0x2348) = 0;

        //enable camera controls
        *getPtr<byte>(player, 0xE59) = 0;

        car->getSplash()->setActive(false);
        player->getCamView()->SetCam(player->getCurrentCameraPtr());

        if (MMSTATE->ShowDash || *getPtr<int>(player, 0x880)) {
            MMSTATE->ViewMode = 1;
            *getPtr<int>(player, 0xE48) = 1;
            player->getCamView()->SetCam(player->getDashCam());
            player->getHUD()->ActivateDash();
        }

        else if (!player->IsPOV()) {
            player->getHUD()->DeactivateDash();
        }

        if (!MMSTATE->ShowDash) {
            player->getHUD()->getDashView()->Deactivate();
        }

        player->SetWideFOV(MMSTATE->UseWideFOV);
    }
}

void mmSingleRoamHandler::HitWaterHandler() {
    auto singleRoam = reinterpret_cast<mmSingleRoam*>(this);
    auto player = singleRoam->getPlayer();

    if (cfgEscapeDeepWater.Get()) {
        EscapeDeepWater();
    }
    else if (cfgResetToNearestLocation.Get()) {
        ResetToNearestLocation();
    }
    else {
        player->Reset();
    }
}

void mmSingleRoamHandler::SetPriority(int a1) {
    auto soundBase = reinterpret_cast<AudSoundBase*>(this);
    soundBase->Load("arrest", 1, false);
    soundBase->SetVolume(1.f);

    for (int i = 1; i <= 20; i++)
    {
        string_buf<128> buffer("ACOPAPP%02d%s", i, MMSTATE->CityName);
        soundBase->Load(buffer, i + 1, false);
        soundBase->SetVolume(1.f);
    }

    soundBase->SetPriority(a1);
}

void mmSingleRoamHandler::Install() {
    InstallVTableHook("mmSingleRoam::HitWaterHandler",
        &HitWaterHandler, {
            0x5B0828
        }
    );

    InstallCallback("mmSingleRoam::InitGameObjects", "Implements arrest and cop speech wav sounds in cruise mode.",
        &SetPriority, {
            cb::call(0x41FBE6),
        }
    );

    //increase num wav sounds
    InstallPatch({ 22 }, {
        0x41FBB0 + 1,
    });
}

/*
    dgBangerInstanceHandler
*/

static ConfigValue<int> cfgMM1StyleShadows("MM1StyleShadows", 0);
static ConfigValue<bool> cfgShadowTransparency("ShadowTransparency", true);

void dgBangerInstanceHandler::DrawShadow() {
    auto inst = reinterpret_cast<dgUnhitYBangerInstance*>(this);
    auto data = inst->GetData();
    auto timeWeather = $::timeWeathers.ptr(cityLevel::timeOfDay);

    if (MMSTATE->TimeOfDay == 3 || MMSTATE->WeatherType != 0 ||
        lvlLevel::Singleton->GetRoomInfo(inst->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean))
        return;

    //get our geometry id
    int geomSetId = inst->GetGeomIndex();
    int geomSetIdOffset = geomSetId - 1;

    //get shaders
    auto shaders = lvlInstance::GetGeomTableEntry(geomSetIdOffset)->pShaders[inst->GetVariant()];

    //get model
    modStatic* model = lvlInstance::GetGeomTableEntry(geomSetIdOffset)->GetHighestLOD();

    if (model != nullptr)
    {
        int srcBlend = (&RSTATE->Data)->SrcBlend;
        if ((&RSTATE->Data)->SrcBlend != 0)
        {
            (&RSTATE->Data)->SrcBlend = 0;
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
        }

        int destBlend = (&RSTATE->Data)->DestBlend;
        if (cfgShadowTransparency.Get())
        {
            if ((&RSTATE->Data)->DestBlend != 9)
            {
                (&RSTATE->Data)->DestBlend = 9;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
        }
        else {
            if ((&RSTATE->Data)->DestBlend != 0)
            {
                (&RSTATE->Data)->DestBlend = 0;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
        }

        if (!strcmp(data->GetName(), "sp_jumptrailer_f"))
        {
            Matrix34 shadowMatrix;
            Matrix34 dummyMatrix;

            if (lvlInstance::ComputeShadowMatrix(&shadowMatrix, inst->GetRoomId(), &inst->GetMatrix(&dummyMatrix)))
            {
                shadowMatrix.m01 = 0.f;

                shadowMatrix.m10 = cos(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);
                shadowMatrix.m11 = 0.f;
                shadowMatrix.m12 = sin(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);

                shadowMatrix.m30 += shadowMatrix.m10 * 2.4f;
                shadowMatrix.m31 -= data->getCG().Y;
                shadowMatrix.m32 += shadowMatrix.m12 * 2.4f;

                Matrix44::Convert(gfxRenderState::sm_World, shadowMatrix);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

                model->Draw(shaders);
            }
        }
        else {
            Matrix34 shadowMatrix;
            Matrix34 dummyMatrix;

            if (lvlInstance::ComputeShadowMatrix(&shadowMatrix, inst->GetRoomId(), &inst->GetMatrix(&dummyMatrix)))
            {
                float posDiffY = inst->GetMatrix(&dummyMatrix).m31 - shadowMatrix.m31;

                shadowMatrix.m10 = cos(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);
                shadowMatrix.m11 = 0.f;
                shadowMatrix.m12 = sin(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);

                shadowMatrix.m30 += shadowMatrix.m10 * posDiffY;
                shadowMatrix.m32 += shadowMatrix.m12 * posDiffY;

                Matrix44::Convert(gfxRenderState::sm_World, shadowMatrix);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

                model->Draw(shaders);
            }
        }

        if ((&RSTATE->Data)->DestBlend != destBlend)
        {
            (&RSTATE->Data)->DestBlend = destBlend;
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
        }

        if ((&RSTATE->Data)->SrcBlend != srcBlend)
        {
            (&RSTATE->Data)->SrcBlend = srcBlend;
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
        }
    }
}

static ConfigValue<int> cfgLightClippingFix("LightClippingFix", 0);

void dgBangerInstanceHandler::DrawGlow()
{
    auto inst = reinterpret_cast<dgBangerInstance*>(this);

    //prepare glow texture
    dgBangerData* data = inst->GetData();
    gfxTexture* lastTexture = dgBangerInstance::DefaultGlowTexture;
    bool swappedTexture = false;

    if (!strcmp(data->GetName(), "sp_light_red_f") && lastTexture != NULL) {
        swappedTexture = true;
        dgBangerInstance::DefaultGlowTexture = RedGlowTexture;
    }

    //draw glows
    ltLight::DrawGlowBegin();
    if ((inst->GetFlags() & INST_FLAG_1) != 0)
    {
        Matrix34 matrix;
        Matrix34 bangerMatrix = inst->GetMatrix(&matrix);
        Vector3 camPosition = *(Vector3*)&gfxRenderState::sm_Camera->m30;

        if (dgBangerInstance::DefaultGlowTexture != nullptr && data != nullptr)
        {
            for (int i = 0; i < data->getNumGlows(); i++)
            {
                Vector3 glowOffset = data->getGlowOffset(i);
                Vector3 lightPos;

                lightPos.Dot(glowOffset, bangerMatrix);

                if (cfgLightClippingFix.Get() >= 3)
                {
                    Vector3 vec;

                    vec.Set(lightPos);
                    vec.Subtract(camPosition);

                    float invGlowScale = -ltLight::GlowScale;
                    lightPos.AddScaled(vec, invGlowScale * 0.25f);
                }

                vglBeginBatch();
                rglWorldIdentity();
                vglBindTexture(dgBangerInstance::DefaultGlowTexture);
                tglDrawParticle(lightPos, 1.515f, Vector4(1.f, 1.f, 1.f, 1.f));
                vglEndBatch();
            }
        }
    }
    ltLight::DrawGlowEnd();

    //reset glow texture
    if (swappedTexture) {
        dgBangerInstance::DefaultGlowTexture = lastTexture;
    }
}

bool dgBangerInstanceHandler::BeginGeom(const char* a1, const char* a2, int a3)
{
    //We hook this to set flag 64 (shadow)
    auto inst = reinterpret_cast<lvlInstance*>(this);
    inst->SetFlag(64);

    //Call original
    return inst->BeginGeom(a1, a2, a3);
}

void dgBangerInstanceHandler::Install()
{
    if (cfgMM1StyleShadows.Get() >= 1) {
        InstallVTableHook("dgBangerInstance::DrawShadow",
            &DrawShadow, {
                0x5B14C4,
                0x5B153C,
                0x5B15E8,
                0x5B1658,
                0x5B54DC,
                0x5B5688,
                0x5B5704,
                0x5B57C8,
                0x5B5FBC,
                0x5B6104,
                0x5B61B0
            }
        );

        InstallCallback("dgBangerInstance::Init", "Hook BeginGeom to set instance shadowing flag.",
            &BeginGeom, {
                cb::call(0x53C7DE), // aiTrafficLightInstance::Init
                cb::call(0x441C86), // dgUnhitBangerInstance::Init
            }
        );

        // increases the maximum distance from 5 to 10 meters to enable shadow rendering for very tall props 
        InstallPatch({ 0xD8, 0x25, 0x28, 0x4, 0x5B, 0x0 }, {
            0x464501
        });
    }

    // makes banger glows double sided
    InstallVTableHook("dgBangerInstance::DrawGlow",
        &DrawGlow, {
            0x5B14CC,
            0x5B1544,
            0x5B15F0,
            0x5B570C,
            0x5B57D0,
            0x5B5FC4,
            0x5B610C,
            0x5B61B8
        }
    );

    InstallCallback("aiTrafficLightInstance::DrawGlow", "Make traffic light banger lights double sided.",
        &DrawGlow, {
            cb::call(0x53CCFD),
        }
    );
}

/*
    vehCarHandler
*/

static ConfigValue<bool> cfgVehicleDebug("VehicleDebug", "vehicleDebug", false);
static ConfigValue<bool> cfgEnableWaterSplashSound("WaterSplashSound", true);
static ConfigValue<bool> cfgEnableExplosionSound("ExplosionSound", true);
bool enableWaterSplashSoundCached = true;
bool enableExplosionSoundCached = true;

void vehCarHandler::InitCar(LPCSTR vehName, int a2, int a3, bool a4, bool a5) {
    Displayf("Initializing vehicle (\"%s\", %d, %d, %s, %s)", vehName, a2, a3, bool_str(a4), bool_str(a5));
    get<vehCar>()->Init(vehName, a2, a3, a4, a5);
}

const phBound * vehCarHandler::GetModelBound(int a1) {
    auto result = hook::Thunk<0x4648C0>::Call<const phBound *>(this, a1);

    if (result == NULL)
        Errorf(">>> COULD NOT RETRIEVE VEHICLE BOUND (%d) !!! <<<", a1);

    return result;
}

void vehCarHandler::InitCarAudio(LPCSTR vehName, int vehType) {
    auto car = reinterpret_cast<vehCar*>(this);
    int flagsId = VehicleListPtr->GetVehicleInfo(vehName)->GetFlags();

    // debug if enabled
    if (cfgVehicleDebug.Get()) {
        Displayf("Loading vehicle audio (\"%s\", %d)", vehName, vehType);
    }

    //Automatic vehtypes system
    bool vehicleHasSiren = false;
    if (car->getSiren() != nullptr) {
        vehicleHasSiren = car->getSiren()->getHasLights() && car->getSiren()->getLightCount() > 0;
    }

    if (vehicleHasSiren || flagsId == 8 && !vehCarAudioContainer::IsPolice(vehName)) {
        Displayf("%s has a lightbar, but is not in the vehtypes file. Adding it.", vehName);
        string_buf<128> sirenBuffer("%s,ENDOFDATA", vehName);
        vehCarAudioContainer::RegisterPoliceNames(NULL, (LPCSTR)sirenBuffer);
    }

    string_buf<128> semiDataName("%s_semidata", vehName);
    bool semiDataExists = datAssetManager::Exists("aud\\cardata\\shared", (LPCSTR)semiDataName, "csv");
    if (semiDataExists && !vehCarAudioContainer::IsSemiOrBus(vehName)) {
        Displayf("%s has semidata, but is not in the vehtypes file. Adding it.", vehName);
        string_buf<128> semiBuffer("%s,ENDOFDATA", vehName);
        vehCarAudioContainer::RegisterSemiNames(NULL, (LPCSTR)semiBuffer);
    }

    //pass back to original function
    car->InitAudio(vehName, vehType);
}

void vehCarHandler::MM1StyleTransmission() {
    auto car = reinterpret_cast<vehCar*>(this);
    auto carsim = car->getCarSim();
    auto engine = carsim->getEngine();
    auto drivetrain = carsim->getDrivetrain();
    auto transmission = carsim->getTransmission();
    auto curDamage = car->getCarDamage()->getCurDamage();
    auto maxDamage = car->getCarDamage()->getMaxDamage();

    void *gameInputPtr = *reinterpret_cast<void**>(0x6B1CF0);
    int *pedalsSwapped = getPtr<int>(gameInputPtr, 0x1D4);

    if (curDamage <= maxDamage) {
        if (transmission->IsAuto()) {
            if (carsim->getSpeedMPH() >= 1.f && carsim->OnGround()) {
                if (engine->getThrottleInput() < 0.1f && transmission->getGear() != 1)
                    engine->setThrottleInput(0.1f);
            }
            // activate Handbrake if car goes under 1mph (P gear)
            if (carsim->getSpeedMPH() < 1.f && engine->getThrottleInput() < 0.1f) {
                carsim->setHandbrake(1.f);
                if (transmission->getGear() == 0 && carsim->getBrake() < 0.1f) {
                    *pedalsSwapped = false;
                    transmission->SetForward();
                }
            }
        }
        else {
            if (carsim->getBrake() < 0.1f && carsim->getHandbrake() < 0.1f) {
                if (engine->getThrottleInput() < 0.1f && transmission->getGear() != 1)
                    engine->setThrottleInput(0.1f);
            }
        }
    }
    // setting up this case for crash course
    // fixes ai cops and opponents have no brakes if they're damaged out
    if (curDamage > maxDamage) {
        carsim->setBrake(1.f);
    }

    // attach drivetrain to avoid engine stuttering
    if (transmission->getGear() != 1) {
        drivetrain->Attach();
    }
}

void vehCarHandler::Splash() {
    auto car = reinterpret_cast<vehCar*>(this);
    float vehicleMph = car->getModel()->GetVelocity().Mag() * 2.23694f;

    //trigger ColliderId 22 with velocity of vehicleMph
    auto impactAud = car->getAudio()->GetAudImpactPtr();

    if (impactAud != nullptr)
        impactAud->Play(vehicleMph, 22);
}

void vehCarHandler::PlayExplosion() {
    auto car = reinterpret_cast<vehCar*>(this);
    auto policeAudio = car->getAudio()->GetPoliceCarAudioPtr();
    auto explosionSound = *getPtr<AudSoundBase*>(policeAudio, 0x138);
    if (explosionSound != nullptr) {
        if (!explosionSound->IsPlaying())
            explosionSound->PlayOnce(-1.f, -1.f);
    }
}

void vehCarHandler::Update() {
    auto car = reinterpret_cast<vehCar*>(this);
    auto siren = car->getSiren();
    auto audio = car->getAudio();
    auto model = car->getModel();
    auto lightbar0 = model->getGenBreakableMgr()->Get(1);
    auto lightbar1 = model->getGenBreakableMgr()->Get(2);
    auto level = *lvlLevel::Singleton;

    if ((lightbar0 != nullptr && !lightbar0->IsAttached) ||
        (lightbar1 != nullptr && !lightbar1->IsAttached)) {
        if (siren != nullptr && siren->getActive()) {
            siren->setActive(false);
            audio->StopSiren();
        }
    }

    if (vehCarModel::MM1StyleTransmission) {
        vehCarHandler::MM1StyleTransmission();
    }

    //play splash sound if we just hit the water
    if (enableWaterSplashSoundCached) {
        bool splashState = car->getSplash()->isActive();
        if (splashState && car->getCarSim()->getSpeedMPH() > 3.f
            && level->GetRoomInfo(model->GetRoomId())->Flags & static_cast<int>(RoomFlags::Water)
            && level->GetWaterLevel(model->GetRoomId()) > model->GetPosition().Y) {
            Splash();
        }
    }

    //check if we're damaged out
    if (enableExplosionSoundCached) {
        auto curDamage = car->getCarDamage()->getCurDamage();
        auto maxDamage = car->getCarDamage()->getMaxDamage();
        if (curDamage > maxDamage) {
            //turn off engine
            audio->SilenceEngine(1);
            car->getCarSim()->getEngine()->setCurrentTorque(0.f);
            //play explosion sound if siren is activated
            if (siren != nullptr && siren->getActive()) {
                siren->setActive(false);
                audio->StopSiren();

                if (audio->IsPlayer())
                    PlayExplosion();
                else
                    audio->GetPoliceCarAudioPtr()->PlayExplosion();
            }
        }
        if (curDamage <= maxDamage) {
            audio->SilenceEngine(0);
        }
    }

    // call original
    hook::Thunk<0x42C690>::Call<void>(this);
}

void vehCarHandler::Reset() {
    auto car = reinterpret_cast<vehCar*>(this);
    auto model = car->getModel();

    model->setHeadlightsState(vehCar::sm_DrawHeadlights);
    model->setHazardLightsState(false);
    model->setLeftSignalLightState(false);
    model->setRightSignalLightState(false);
    model->setFoglightsState(MMSTATE->WeatherType == 2);

    car->Reset();
}

void vehCarHandler::Install(void) {
    enableWaterSplashSoundCached = cfgEnableWaterSplashSound.Get();
    enableExplosionSoundCached = cfgEnableExplosionSound.Get();
    InstallCallback("vehCar::InitAudio", "Enables debugging for vehicle initialization, and automatic vehtypes handling.",
        &InitCarAudio, {
            cb::call(0x55943A), // aiVehiclePhysics::Init
            cb::call(0x404090), // mmPlayer::Init
            cb::call(0x43C540), // mmNetObject::Init
        }
    );

    if (cfgVehicleDebug) {
        InstallCallback("vehCar::InitAudio", "Enables debugging for vehicle initialization.",
            &InitCarAudio, {
                cb::call(0x55943A), // aiVehiclePhysics::Init
                cb::call(0x404090), // mmPlayer::Init
                cb::call(0x43C540), // mmNetObject::Init
            }
        );

        InstallVTableHook("vehCarModel::GetBound",
            &GetModelBound, {
                0x5B2D14
            }
        );
    }

    InstallVTableHook("vehCar::Update",
        &Update, {
            0x5B0BB8,
        }
    );

    InstallCallback("vehCar::Reset", "Use our reset for vehicle initialization.",
        &Reset, {
            cb::call(0x404B87), // mmPlayer::Reset
            cb::call(0x413502), // mmGame::InitOtherPlayers
            cb::call(0x413631), // mmGame::CollideAIOpponents
            cb::call(0x4165BC), // mmSingleStunt::InitGameObjects
            cb::call(0x41B1CE), // mmSingleBlitz::InitGameObjects
            cb::call(0x41C8F6), // mmSingleCircuit::InitGameObjects
            cb::call(0x41E372), // mmSingleRace::InitGameObjects
            cb::call(0x41FB93), // mmSingleRoam::InitGameObjects
            cb::call(0x41FC81), // mmSingleRoam::InitOtherPlayers
            cb::call(0x420365), // mmMultiBlitz::InitGameObjects
            cb::call(0x420647), // mmMultiBlitz::InitNetworkPlayers
            cb::call(0x420780), // mmMultiBlitz::InitNetworkPlayers
            cb::call(0x421F8E), // mmMultiCircuit::InitGameObjects
            cb::call(0x422237), // mmMultiCircuit::InitNetworkPlayers
            cb::call(0x422370), // mmMultiCircuit::InitNetworkPlayers
            cb::call(0x423CE9), // mmMultiCR::InitGameObjects
            cb::call(0x4241F2), // mmMultiCR::InitNetworkPlayers
            cb::call(0x4277FB), // mmMultiRoam::InitGameObjects
            cb::call(0x4279E6), // mmMultiRoam::InitNetworkPlayers
            cb::call(0x427B43), // mmMultiRoam::InitNetworkPlayers
            cb::call(0x4285E9), // mmMultiRace::InitGameObjects
            cb::call(0x42887C), // mmMultiRace::InitNetworkPlayers
            cb::call(0x4289D0), // mmMultiRace::InitNetworkPlayers
            cb::call(0x43981D), // mmGameMulti::SystemMessageCB
            cb::call(0x439C75), // mmGameMulti::GameMessageCB
            cb::call(0x439E23), // mmGameMulti::GameMessageCB
            cb::call(0x43A63F), // mmGameMulti::Reset
            cb::call(0x5597C6), // aiVehiclePhysics::Reset
        }
    );

    if (vehCarModel::MM1StyleTransmission) {
        // deactivate auto Handbrake system
        InstallPatch({ 0xD8, 0x1D, 0x3C, 0x4, 0x5B, 0x0 }, {
            0x405C81,
        });
    }
}

/*
    vehBreakableMgrHandler
*/

static ConfigValue<bool> cfgBreakReflections("ReflectionsOnBreakables", true);

void vehBreakableMgrHandler::Install() {
    vehBreakableMgr::EnableReflections = cfgBreakReflections.Get();
    InstallCallback("vehBreakableMgr::Draw", "Draws reflections on breakables.",
        &vehBreakableMgr::Draw, {
            cb::call(0x4CE1B7), // vehCarModel::Draw
            cb::call(0x552220), // aiVehicleInstance::Draw
        }
    );
}

/*
    vehCarModelFeatureHandler
*/

void vehCarModelFeatureHandler::Draw(int a1) {
    auto model = reinterpret_cast<vehCarModel*>(this);
    model->vehCarModel::Draw(a1);
}

void vehCarModelFeatureHandler::DrawShadow() {
    auto model = reinterpret_cast<vehCarModel*>(this);
    auto carMatrix = model->getCarSim()->getWorldMatrix();
    auto roomId = model->GetRoomId();

    if (model->GetFlags() & INST_FLAG_200)
    {
        //get our geometry id
        int geomSetId = model->GetGeomIndex();
        int geomSetIdOffset = geomSetId - 1;

        //get shaders
        auto mainGeomEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset);
        auto shaders = mainGeomEntry->pShaders[model->getVariant()];

        //get time weather
        auto timeWeather = $::timeWeathers.ptr(cityLevel::timeOfDay);

        //get model
        modStatic* shadow = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 1)->GetHighLOD();

        if (shadow != nullptr)
        {
            Matrix34 shadowMatrix;

            if (lvlInstance::ComputeShadowMatrix(&shadowMatrix, roomId, carMatrix))
            {
                RSTATE->SetBlendSet(0, 0x80);

                Matrix44::Convert(gfxRenderState::sm_World, shadowMatrix);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

                shadow->Draw(shaders);
            }
        }

        if (MMSTATE->TimeOfDay == 3 || MMSTATE->WeatherType != 0 ||
            lvlLevel::Singleton->GetRoomInfo(model->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean))
            return;

        modStatic* body = lvlInstance::GetGeomTableEntry(geomSetIdOffset)->GetHighLOD();

        if (body != nullptr)
        {
            int srcBlend = (&RSTATE->Data)->SrcBlend;
            if ((&RSTATE->Data)->SrcBlend != 0)
            {
                (&RSTATE->Data)->SrcBlend = 0;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }

            int destBlend = (&RSTATE->Data)->DestBlend;
            if (cfgShadowTransparency.Get())
            {
                if ((&RSTATE->Data)->DestBlend != 9)
                {
                    (&RSTATE->Data)->DestBlend = 9;
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
                }
            }
            else {
                if ((&RSTATE->Data)->DestBlend != 0)
                {
                    (&RSTATE->Data)->DestBlend = 0;
                    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
                }
            }

            Matrix34 shadowMatrix;

            if (lvlInstance::ComputeShadowMatrix(&shadowMatrix, roomId, carMatrix))
            {
                float posDiffY = carMatrix->m31 - shadowMatrix.m31;

                shadowMatrix.m10 = cos(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);
                shadowMatrix.m11 = 0.f;
                shadowMatrix.m12 = sin(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);

                shadowMatrix.m30 += shadowMatrix.m10 * posDiffY;
                shadowMatrix.m32 += shadowMatrix.m12 * posDiffY;

                Matrix44::Convert(gfxRenderState::sm_World, shadowMatrix);
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

                body->Draw(shaders);
            }

            if ((&RSTATE->Data)->DestBlend != destBlend)
            {
                (&RSTATE->Data)->DestBlend = destBlend;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }

            if ((&RSTATE->Data)->SrcBlend != srcBlend)
            {
                (&RSTATE->Data)->SrcBlend = srcBlend;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
        }
    }
}

void vehCarModelFeatureHandler::DrawGlow() {
    auto model = reinterpret_cast<vehCarModel*>(this);
    if (!model->GetVisible())
        return;

    //get our geometry id
    int geomSetId = model->GetGeomIndex();
    int geomSetIdOffset = geomSetId - 1;

    //get shaders
    auto mainGeomEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset);
    auto shaders = mainGeomEntry->pShaders[model->getVariant()];

    //get car stuff we use to determine what to darw
    auto car = model->getCar();
    auto carsim = model->getCarSim();
    auto siren = car->getSiren();
    auto curDamage = car->getCarDamage()->getCurDamage();
    auto maxDamage = car->getCarDamage()->getMaxDamage();
    int gear = carsim->getTransmission()->getGear();
    if (curDamage > maxDamage && vehCarModel::MWStyleTotaledCar)
        return;

    //setup renderer
    Matrix44::Convert(gfxRenderState::sm_World, model->getCarMatrix());
    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

    //get signals
    modStatic* slight0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 5)->GetHighestLOD();
    modStatic* slight1 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 6)->GetHighestLOD();

    //get taillight signals
    modStatic* tslight0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 101)->GetHighestLOD();
    modStatic* tslight1 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 102)->GetHighestLOD();

    //check signal clock
    float randValue = (irand2(model) % 8) * 0.001f;
    bool drawSignal = fmod(datTimeManager::ElapsedTime, 1.f + randValue) > 0.5f + randValue;

    //draw stuff!
    if (drawSignal) {
        if (model->getLeftSignalLightState() || model->getHazardLightsState()) {
            if (slight0 != nullptr)
                slight0->Draw(shaders);
            if (tslight0 != nullptr)
                tslight0->Draw(shaders);
        }
        if (model->getRightSignalLightState() || model->getHazardLightsState()) {
            if (slight1 != nullptr)
                slight1->Draw(shaders);
            if (tslight1 != nullptr)
                tslight1->Draw(shaders);
        }
    }

    //draw taillight signals
    if (!model->getLeftSignalLightState() && !model->getHazardLightsState()) {
        if (tslight0 != nullptr) {
            //draw brake copy
            if (carsim->getBrake() > 0.1)
                tslight0->Draw(shaders);
            //draw headlight copy
            if (model->getHeadlightsState())
                tslight0->Draw(shaders);
        }
    }
    if (!model->getRightSignalLightState() && !model->getHazardLightsState()) {
        if (tslight1 != nullptr) {
            //draw brake copy
            if (carsim->getBrake() > 0.1)
                tslight1->Draw(shaders);
            //draw headlight copy
            if (model->getHeadlightsState())
                tslight1->Draw(shaders);
        }
    }

    //only draw rear lights if the electrics allow it
    if (model->getEnabledElectrics(0) || model->getEnabledElectrics(1))
    {
        //draw tlight
        modStatic* tlight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 3)->GetHighestLOD();
        if (tlight != nullptr) {
            //draw brake copy
            if (carsim->getBrake() > 0.1)
                tlight->Draw(shaders);
            //draw headlight copy
            if (model->getHeadlightsState())
                tlight->Draw(shaders);
        }

        //draw blight
        modStatic* blight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 7)->GetHighestLOD();
        if (blight != nullptr) {
            //draw brake copy
            if (carsim->getBrake() > 0.1)
                blight->Draw(shaders);
        }

        //draw rlight
        modStatic* rlight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 4)->GetHighestLOD();
        if (rlight != nullptr && gear == 0) {
            rlight->Draw(shaders);
        }
    }

    //get headlight and sirens
    modStatic* hlight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 2)->GetHighestLOD();
    modStatic* siren0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 9)->GetHighestLOD();
    modStatic* siren1 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 10)->GetHighestLOD();

    //draw headlights
    if (vehCarModel::HeadlightType == 0 || vehCarModel::HeadlightType == 2) {
        //MM2 headlights
        if (vehCarModel::EnableHeadlightFlashing && siren != nullptr && siren->getActive())
        {
            DrawHeadlights(true);
            DrawExtraHeadlights(true);
        }
        else if (model->getHeadlightsState())
        {
            DrawHeadlights(false);
            DrawExtraHeadlights(false);
        }
        if (model->getFoglightsState())
        {
            DrawFoglights();
        }
    }
    if (vehCarModel::HeadlightType == 1 || vehCarModel::HeadlightType == 2) {
        //MM1 headlights
        Matrix44::Convert(gfxRenderState::sm_World, model->getCarMatrix());
        gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

        if (model->getEnabledElectrics(2) || model->getEnabledElectrics(3))
        {
            if (hlight != nullptr)
            {
                if (model->getHeadlightsState())
                {
                    hlight->Draw(shaders);
                }
            }
        }
        if (model->getFoglightsState())
        {
            for (int i = 0; i < 4; i++)
            {
                //get foglight
                modStatic* flight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 103 + i)->GetHighestLOD();

                //get breakable foglight
                auto breaklt = model->getGenBreakableMgr()->Get(3 + i);

                if (flight != nullptr)
                {
                    if (breaklt == nullptr || breaklt->IsAttached)
                    {
                        flight->Draw(shaders);
                    }
                }
            }
        }
    }

    //draw sirens
    if (vehCarModel::SirenType == 0 || vehCarModel::SirenType == 2) {
        //MM2 siren
        if (siren != nullptr && siren->getHasLights() && siren->getActive())
        {
            DrawSiren(model->getCarMatrix());
        }
    }
    if (vehCarModel::SirenType == 1 || vehCarModel::SirenType == 2) {
        //MM1 siren
        Matrix44::Convert(gfxRenderState::sm_World, model->getCarMatrix());
        gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

        if (siren != nullptr && siren->getActive()) {
            bool drawLEDSiren = fmod(datTimeManager::ElapsedTime, 0.1f + randValue) > 0.05f + randValue;

            if (!vehCarModel::EnableLEDSiren || drawLEDSiren) {
                int sirenStage = fmod(datTimeManager::ElapsedTime, 2 * (vehCarModel::SirenCycle + randValue)) >= (vehCarModel::SirenCycle + randValue) ? 1 : 0;
                if (sirenStage == 0 && siren0 != nullptr) {
                    siren0->Draw(shaders);
                }
                else if (sirenStage == 1 && siren1 != nullptr) {
                    siren1->Draw(shaders);
                }
            }
        }
    }
}

void vehCarModelFeatureHandler::DrawHeadlights(bool rotate)
{
    auto model = reinterpret_cast<vehCarModel*>(this);

    if (model->getHeadlight(0) == nullptr)
        return;

    bool bothLightsBroken = !(model->getEnabledElectrics(2) || model->getEnabledElectrics(3));
    if (bothLightsBroken)
        return;

    //prepare glow texture
    gfxTexture* lastTexture = ltLight::GlowTexture;
    bool swappedTexture = false;

    //swap texture if it exists
    if (lastTexture != NULL && HeadlightTexture != NULL) {
        swappedTexture = true;
        ltLight::GlowTexture = HeadlightTexture;
    }

    auto carMatrix = model->getCarMatrix();

    float rotationAmount = vehCarModel::HeadlightFlashingSpeed;

    Vector3 camPosition = *(Vector3*)&gfxRenderState::sm_Camera->m30;

    ltLight::DrawGlowBegin();
    for (int i = 0; i < 2; i++)
    {
        bool isHeadlightBroken = !(model->getEnabledElectrics(i + 2));
        if (isHeadlightBroken)
            continue;

        auto light = model->getHeadlight(i);
        auto lightPos = model->getHeadlightPosition(i);

        light->Position.Dot(lightPos, carMatrix);

        if (rotate)
        {
            light->Direction.RotateY(datTimeManager::Seconds * rotationAmount);
            rotationAmount *= -1.f;
        }
        else
        {
            light->Direction = Vector3(-carMatrix.m20, -carMatrix.m21, -carMatrix.m22);
        }

        if (cfgLightClippingFix.Get() == 1 || cfgLightClippingFix.Get() >= 4)
        {
            Vector3 vec;
            lvlSegment segment;
            lvlIntersection intersection;

            vec.Set(light->Position);
            vec.Subtract(camPosition);

            float invGlowScale = -ltLight::GlowScale;

            segment.Set(light->Position, camPosition, 0, nullptr);

            if (Collide(segment, &intersection, 0, nullptr, SpecialBound | Warp, 0, light->Position))
            {
                invGlowScale = 0.f;
            }

            light->Position.AddScaled(vec, invGlowScale);
        }

        light->DrawGlow(camPosition);
    }
    ltLight::DrawGlowEnd();

    //reset glow texture
    if (swappedTexture) {
        ltLight::GlowTexture = lastTexture;
    }
}

void vehCarModelFeatureHandler::DrawExtraHeadlights(bool rotate)
{
    auto model = reinterpret_cast<vehCarModel*>(this);

    int geomSetId = model->GetGeomIndex();
    int geomSetIdOffset = geomSetId - 1;

    auto headlight2 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 73);
    auto headlight3 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 74);

    if (headlight2->GetHighLOD() == nullptr || headlight3->GetHighLOD() == nullptr)
        return;

    bool bothLightsBroken = !(model->getEnabledElectrics(2) || model->getEnabledElectrics(3));
    if (bothLightsBroken)
        return;

    //prepare glow texture
    gfxTexture* lastTexture = ltLight::GlowTexture;
    bool swappedTexture = false;

    //swap texture if it exists
    if (lastTexture != NULL && HeadlightTexture != NULL) {
        swappedTexture = true;
        ltLight::GlowTexture = HeadlightTexture;
    }

    auto carMatrix = model->getCarMatrix();

    float rotationAmount = vehCarModel::HeadlightFlashingSpeed;

    Vector3 camPosition = *(Vector3*)&gfxRenderState::sm_Camera->m30;

    ltLight::DrawGlowBegin();
    for (int i = 0; i < 2; i++)
    {
        bool isHeadlightBroken = !(model->getEnabledElectrics(i + 2));
        if (isHeadlightBroken)
            continue;

        auto light = model->getExtraHeadlight(i);
        auto lightPos = model->getExtraHeadlightPosition(i);

        light->Position.Dot(lightPos, carMatrix);

        if (rotate)
        {
            light->Direction.RotateY(datTimeManager::Seconds * rotationAmount);
            rotationAmount *= -1.f;
        }
        else
        {
            light->Direction = Vector3(-carMatrix.m20, -carMatrix.m21, -carMatrix.m22);
        }

        if (cfgLightClippingFix.Get() == 1 || cfgLightClippingFix.Get() >= 4)
        {
            Vector3 vec;
            lvlSegment segment;
            lvlIntersection intersection;

            vec.Set(light->Position);
            vec.Subtract(camPosition);

            float invGlowScale = -ltLight::GlowScale;

            segment.Set(light->Position, camPosition, 0, nullptr);

            if (Collide(segment, &intersection, 0, nullptr, SpecialBound | Warp, 0, light->Position))
            {
                invGlowScale = 0.f;
            }

            light->Position.AddScaled(vec, invGlowScale);
        }

        light->DrawGlow(camPosition);
    }
    ltLight::DrawGlowEnd();

    //reset glow texture
    if (swappedTexture) {
        ltLight::GlowTexture = lastTexture;
    }
}

void vehCarModelFeatureHandler::DrawFoglights()
{
    auto model = reinterpret_cast<vehCarModel*>(this);

    int geomSetId = model->GetGeomIndex();
    int geomSetIdOffset = geomSetId - 1;

    auto foglight0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 75);
    if (foglight0->GetHighLOD() == nullptr)
        return;

    //prepare glow texture
    gfxTexture* lastTexture = ltLight::GlowTexture;
    bool swappedTexture = false;

    //swap texture if it exists
    if (lastTexture != NULL && HeadlightTexture != NULL) {
        swappedTexture = true;
        ltLight::GlowTexture = HeadlightTexture;
    }

    auto carMatrix = model->getCarMatrix();

    Vector3 camPosition = *(Vector3*)&gfxRenderState::sm_Camera->m30;

    ltLight::DrawGlowBegin();
    for (int i = 0; i < 4; i++)
    {
        auto breaklt = model->getGenBreakableMgr()->Get(i + 3);

        if (breaklt == nullptr || breaklt->IsAttached)
        {
            auto light = model->getFoglight(i);
            auto lightPos = model->getFoglightPosition(i);

            light->Position.Dot(lightPos, carMatrix);

            light->Direction = Vector3(-carMatrix.m20, -carMatrix.m21, -carMatrix.m22);

            if (cfgLightClippingFix.Get() == 1 || cfgLightClippingFix.Get() >= 4)
            {
                Vector3 vec;
                lvlSegment segment;
                lvlIntersection intersection;

                vec.Set(light->Position);
                vec.Subtract(camPosition);

                float invGlowScale = -ltLight::GlowScale;

                segment.Set(light->Position, camPosition, 0, nullptr);

                if (Collide(segment, &intersection, 0, nullptr, SpecialBound | Warp, 0, light->Position))
                {
                    invGlowScale = 0.f;
                }

                light->Position.AddScaled(vec, invGlowScale);
            }

            light->DrawGlow(camPosition);
        }
    }
    ltLight::DrawGlowEnd();

    //reset glow texture
    if (swappedTexture) {
        ltLight::GlowTexture = lastTexture;
    }
}

void vehCarModelFeatureHandler::DrawSiren(const Matrix34& carMatrix)
{
    auto model = reinterpret_cast<vehCarModel*>(this);
    auto siren = model->getCar()->getSiren();

    if (siren->getLight(0) == nullptr)
        return;

    //prepare glow texture
    gfxTexture* lastTexture = ltLight::GlowTexture;
    bool swappedTexture = false;

    //swap texture if it exists
    if (lastTexture != NULL && SirenTexture != NULL) {
        swappedTexture = true;
        ltLight::GlowTexture = SirenTexture;
    }

    Vector3 camPosition = *(Vector3*)&gfxRenderState::sm_Camera->m30;

    ltLight::DrawGlowBegin();
    for (int i = 0; i < siren->getLightCount(); i++)
    {
        auto light = siren->getLight(i);
        auto lightPos = siren->getLightPosition(i);

        light->Position.Dot(lightPos, carMatrix);

        if (cfgLightClippingFix.Get() == 1 || cfgLightClippingFix.Get() >= 4)
        {
            Vector3 vec;
            lvlSegment segment;
            lvlIntersection intersection;

            vec.Set(light->Position);
            vec.Subtract(camPosition);

            float invGlowScale = -ltLight::GlowScale;

            segment.Set(light->Position, camPosition, 0, nullptr);

            if (Collide(segment, &intersection, 0, nullptr, SpecialBound | Warp, 0, light->Position))
            {
                invGlowScale = 0.f;
            }

            light->Position.AddScaled(vec, invGlowScale);
        }

        light->DrawGlow(camPosition);
    }
    ltLight::DrawGlowEnd();

    siren->getLensFlare()->DrawBegin();
    for (int i = 0; i < siren->getLightCount(); i++)
    {
        auto light = siren->getLight(i);
        auto lightPos = light->Position;
        auto color = light->Color;

        float intensity = light->ComputeIntensity(camPosition, 0.05f);
        siren->getLensFlare()->Draw(lightPos, color, intensity);
    }
    siren->getLensFlare()->DrawEnd();

    //reset glow texture
    if (swappedTexture) {
        ltLight::GlowTexture = lastTexture;
    }
}

bool vehCarModelFeatureHandler::Collide(lvlSegment& segment, lvlIntersection* intersection, int roomId, lvlInstance* ignoreInstance, ushort instanceFlags, int collideFlags, const Vector3& lightPos)
{
    lvlSegmentInfo segmentInfo;
    memset(&segmentInfo, 0, sizeof(segmentInfo));
    if (!segment.SegmentInfo)
    {
        segment.SegmentInfo = &segmentInfo;
        segmentInfo.StartRoomId = roomId;
        segmentInfo.EndRoomId = roomId;
    }

    auto level = *lvlLevel::Singleton;

    int startRoomId = level->FindRoomId(segment.StartPos, segment.SegmentInfo->StartRoomId);

    segment.SegmentInfo->StartRoomId = startRoomId;

    int endRoomId = level->FindRoomId(segment.EndPos, segment.SegmentInfo->EndRoomId);

    segment.SegmentInfo->EndRoomId = endRoomId;

    intersection->IntersectionPoint.NormalizedDistance = 2.0;

    intersection->Poly = 0;

    auto bound = level->GetBound();

    bool collide = false;

    Vector3 camPosition = *(Vector3*)&gfxRenderState::sm_Camera->m30;

    // gather ground and building bounds
    if (lightPos.Dist(camPosition) < 100.f)
        collide = bound->CollideProbe(
            segment,
            intersection,
            intersection->IntersectionPoint.NormalizedDistance);

    auto startRoomInfo = level->GetRoomInfo(startRoomId);
    if ((startRoomInfo->InstanceFlags & instanceFlags) != 0) // collide in start room
    {
        for (lvlInstance* i = startRoomInfo->FirstInstance; i; i = i->GetNext())
        {
            if ((i->GetFlags() & instanceFlags) != 0)
            {
                if ((i->GetFlags() & collideFlags) == 0 && i != ignoreInstance)
                {
                    collide |= dgPhysManager::Instance->CollideProbe(segment, intersection, i);
                }
            }
        }
    }

    if (endRoomId != startRoomId) // collide in end room
    {
        auto endRoomInfo = level->GetRoomInfo(endRoomId);
        if ((endRoomInfo->InstanceFlags & instanceFlags) != 0)
        {
            for (lvlInstance* i = endRoomInfo->FirstInstance; i; i = i->GetNext())
            {
                if ((i->GetFlags() & instanceFlags) != 0)
                {
                    if ((i->GetFlags() & collideFlags) == 0 && i != ignoreInstance)
                    {
                        collide |= dgPhysManager::Instance->CollideProbe(segment, intersection, i);
                    }
                }
            }
        }
    }

    auto roomInfo = level->GetRoomInfo(startRoomId);  // sf warp hack
    if ((roomInfo->Flags & Warp) != 0)
    {
        int roomId;
        lvlRoomInfo* roomInfo2;
        switch (startRoomId)
        {
        case 411:
            roomId = 102;
            break;
        case 412:
            roomId = 122;
            break;
        case 423:
            roomId = 96;
            break;
        case 625:
            roomId = 1;
            break;
        default:
            return collide;
        }
        roomInfo2 = level->GetRoomInfo(roomId);
        if ((roomInfo2->InstanceFlags & instanceFlags) != 0)
        {
            for (lvlInstance* i = roomInfo2->FirstInstance; i; i = i->GetNext())
            {
                if ((i->GetFlags() & instanceFlags) != 0)
                {
                    if ((i->GetFlags() & collideFlags) == 0 && i != ignoreInstance)
                    {
                        collide |= dgPhysManager::Instance->CollideProbe(segment, intersection, i);
                    }
                }
            }
        }
    }
    return collide;
}

void vehCarModelFeatureHandler::EjectOneShot() {
    auto model = reinterpret_cast<vehCarModel*>(this);
    model->vehCarModel::EjectOneshot();
}

void vehCarModelFeatureHandler::Install() {
    InstallPatch({ 0x60, 0x1 }, {
        0x42BB6E + 1, // Change size of vehCarModel on allocation
    });

    InstallPatch({ 0x60, 0x1 }, {
        0x4CDFE0 + 1, // Change size of vehCarModel on SizeOf
    });

    InstallCallback("vehCarModel::Init", "Use rewritten vehCarModel init.",
        &vehCarModel::Init, {
            cb::call(0x42BE86),
        }
    );

    InstallCallback("vehCarModel::EjectOneShot", "add more mechanical breakables.",
        &EjectOneShot, {
            cb::call(0x4CAE16),
        }
    );

    InstallVTableHook("vehCarModel::Draw",
        &Draw, {
            0x5B2CDC,
        }
    );

    if (cfgMM1StyleShadows.Get() >= 3) {
        InstallVTableHook("vehCarModel::DrawShadow",
            &DrawShadow, {
                0x5B2CE0,
            }
        );
    }

    InstallVTableHook("vehCarModel::DrawGlow",
        &DrawGlow, {
            0x5B2CE8
        }
    );

    ConfigValue<bool> cfgEnableSpinningWheels("EnableSpinningWheels", true);
    ConfigValue<bool> cfgPartReflections("ReflectionsOnCarParts", false);
    ConfigValue<bool> cfgHeadlightFlashing("EnableHeadlightFlashing", true);
    ConfigValue<bool> cfgEnableLEDSiren("EnableLEDSiren", false);
    ConfigValue<bool> cfgNfsMwStyleTotaledCar("NFSMWStyleTotaledCar", false);
    ConfigValue<bool> cfgBreakableRenderTweak("BreakableRenderTweak", false);
    ConfigValue<bool> cfgMM1StyleTransmission("MM1StyleTransmission", false);
    ConfigValue<bool> cfgWheelWobble("PhysicalWheelWobble", true);
    ConfigValue<bool> cfgMM1StyleWobble("MM1StyleWobble", false);
    ConfigValue<int> cfgSirenStyle("SirenStyle", 0);
    ConfigValue<int> cfgHeadlightStyle("HeadlightStyle", 0);
    ConfigValue<float> cfgSirenCycleRate("SirenCycle", 0.25f);
    ConfigValue<float> cfgHeadlightFlashingSpeed("HeadlightFlashingSpeed", 42.411503f);

    vehCarModel::EnableSpinningWheels = cfgEnableSpinningWheels.Get();
    vehCarModel::EnableHeadlightFlashing = cfgHeadlightFlashing.Get();
    vehCarModel::EnableLEDSiren = cfgEnableLEDSiren.Get();
    vehCarModel::SirenType = cfgSirenStyle.Get();
    vehCarModel::HeadlightType = cfgHeadlightStyle.Get();
    vehCarModel::SirenCycle = cfgSirenCycleRate.Get();
    vehCarModel::MWStyleTotaledCar = cfgNfsMwStyleTotaledCar.Get();
    vehCarModel::HeadlightFlashingSpeed = cfgHeadlightFlashingSpeed.Get();

    vehCarModel::PartReflections = cfgPartReflections.Get();
    vehCarModel::WheelReflections = vehCarModel::PartReflections;

    vehCarModel::MM1StyleTransmission = cfgMM1StyleTransmission.Get();
    vehCarModel::BreakableRenderTweak = cfgBreakableRenderTweak.Get();

    vehCarModel::WheelWobble = cfgWheelWobble.Get();
    vehCarModel::MM1StyleWobble = cfgMM1StyleWobble.Get();
}

/*
    vehWheelHandler
*/

void vehWheelHandler::Update()
{
    //call original
    hook::Thunk<0x4D34E0>::Call<void>(this);
}

void vehWheelHandler::Install()
{
    InstallCallback("vehWheel::Init", "Use rewritten vehWheel init.",
        &vehWheel::Init, {
            cb::call(0x4CBC0A),
            cb::call(0x4CBC41),
            cb::call(0x4CBC7D),
            cb::call(0x4CBCBB),
            cb::call(0x4D74B8),
            cb::call(0x4D74E9),
            cb::call(0x4D751A),
            cb::call(0x4D754B),
        }
    );

    InstallVTableHook("vehWheel::Update",
        &Update, {
            0x5B2DD4,
        }
    );
}

/*
    fxTexelDamageHandler
*/

static ConfigValue<bool> cfgMm1StyleDamage("MM1StyleDamage", false);

void fxTexelDamageHandler::Install()
{
    InstallCallback("fxTexelDamage::ApplyDamage", "",
        &fxTexelDamage::ApplyDamage, {
            cb::call(0x4CAE46),
        }
    );

    InstallCallback("fxTexelDamage::Init", "",
        &fxTexelDamage::Init, {
            cb::call(0x4CD492)
        }
    );

    InstallCallback("fxTexelDamage::Reset", "",
        &fxTexelDamage::Reset, {
            cb::call(0x4CE018)
        }
    );

    InstallCallback("fxTexelDamage::Kill", "",
        &fxTexelDamage::Kill, {
            cb::jmp(0x591CC0)
        }
    );

    if (!cfgMm1StyleDamage.Get())
        return;

    InstallPatch({ 0x8B, 0x55, 0x0 }, {
        0x59242D,
    });

    InstallPatch({ 0x8B, 0x55, 0x0 }, {
        0x59246C,
    });

    InstallPatch({ 0x8B, 0x55, 0x0 }, {
        0x5924A9,
    });

    InstallPatch({ 0x8B, 0x55, 0x0 }, {
        0x5924E5,
    });

    InstallPatch({ 0x8B, 0x55, 0x0 }, {
        0x592525,
    });

    InstallPatch({ 0x8B, 0x55, 0x0 }, {
        0x5925F7,
    });

    InstallPatch({ 0x8B, 0x45, 0x0 }, {
        0x592879,
    });

    //set TexelDamageRadius value to 0.5f
    InstallPatch({ 0xD9, 0x5, 0x60, 0x3, 0x5B, 0x0 }, {
        0x4CAE35, // vehCarDamage::Update
    });
}

/*
    pedestrianInstanceHandler
*/

static ConfigValue<bool> cfgRagdolls("Ragdolls", true);

void pedestrianInstanceHandler::aiMapClean()
{
    //clean aimap
    hook::Thunk<0x534C10>::Call<void>(this);

    //destroy pedRagdollMgr
    delete pedRagdollMgr::Instance;
}

void pedestrianInstanceHandler::aiMapInit(char * a1, char * a2, char * a3, const dgStatePack * a4, int a5, vehCar * a6, bool a7)
{
    //init aimap
    hook::Thunk<0x534FC0>::Call<void>(this, a1, a2, a3, a4, a5, a6, a7);

    //init pedRagdollMgr
    pedRagdollMgr::Instance = new pedRagdollMgr();

    char* values[2] = { "pedmodel_man", "pedmodel_woman" };
    pedRagdollMgr::Instance->Init(2, values);

    aiMap::Instance->AddChild(pedRagdollMgr::Instance);
}

phBound * pedestrianInstanceHandler::GetBound(int a1)
{
    return pedRagdollMgr::Instance->getBoundBox();
}

bool pedestrianInstanceHandler::IsCollidable() {
    return pedRagdollMgr::Instance->UnusedActive();
}

void pedestrianInstanceHandler::Init(char* a1, int a2, int a3) {
    //call original
    hook::Thunk<0x54B0D0>::Call<void>(this, a1, a2, a3);

    auto inst = reinterpret_cast<aiPedestrian*>(this)->getInstance();
    inst->SetFlag(64);
}

void pedestrianInstanceHandler::DrawRagdoll() {
    auto inst = reinterpret_cast<aiPedestrianInstance*>(this);

    //matrices
    Matrix44 pedestrianMatrixList[32]; //bone matrices

    //get pedActive
    auto active = reinterpret_cast<pedActive*>(inst->GetEntity());

    //set matrix
    gfxRenderState::sm_World->Identity();
    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

    //attach skeleton
    auto ragdollSkel = active->getSkeleton();
    ragdollSkel->Attach(&pedestrianMatrixList[0]);

    //get animationInstance and draw it
    auto animationInstance = inst->getAnimationInstance();
    auto anim = animationInstance->getAnimation();
    anim->pModel->Draw(&pedestrianMatrixList[0], anim->ppShaders[animationInstance->getVariant()], 0xFFFFFFFF);
}

void pedestrianInstanceHandler::Draw(int a1) {
    auto inst = reinterpret_cast<aiPedestrianInstance*>(this);

    //if we have no ragdoll, call the original function
    if (inst->GetEntity() == nullptr) {
        Matrix34 dummyMatrix;

        auto pedMatrix = inst->GetMatrix(&dummyMatrix);
        pedMatrix.Scale(boneScale);

        Matrix44::Convert(gfxRenderState::sm_World, pedMatrix);
        gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

        inst->getAnimationInstance()->Draw(a1 == 3);
        *getPtr<int>(this, 0x28) = a1 + 1;
    }
    else {
        this->DrawRagdoll();
    }
}

void pedestrianInstanceHandler::DrawShadow() {
    auto inst = reinterpret_cast<aiPedestrianInstance*>(this);
    auto animationInstance = inst->getAnimationInstance();
    auto anim = animationInstance->getAnimation();
    auto timeWeather = $::timeWeathers.ptr(cityLevel::timeOfDay);

    if (MMSTATE->TimeOfDay == 3 || MMSTATE->WeatherType != 0 ||
        lvlLevel::Singleton->GetRoomInfo(inst->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean))
        return;

    int srcBlend = (&RSTATE->Data)->SrcBlend;
    if ((&RSTATE->Data)->SrcBlend != 0)
    {
        (&RSTATE->Data)->SrcBlend = 0;
        gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
    }

    int destBlend = (&RSTATE->Data)->DestBlend;
    if (cfgShadowTransparency.Get())
    {
        if ((&RSTATE->Data)->DestBlend != 9)
        {
            (&RSTATE->Data)->DestBlend = 9;
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
        }
    }
    else {
        if ((&RSTATE->Data)->DestBlend != 0)
        {
            (&RSTATE->Data)->DestBlend = 0;
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
        }
    }

    Matrix34 shadowMatrix;
    Matrix34 dummyMatrix;

    if (lvlInstance::ComputeShadowMatrix(&shadowMatrix, inst->GetRoomId(), &inst->GetMatrix(&dummyMatrix)))
    {
        if (inst->GetEntity() == nullptr)
        {
            float posDiffY = inst->GetMatrix(&dummyMatrix).m31 - shadowMatrix.m31;

            shadowMatrix.m10 = cos(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);
            shadowMatrix.m11 = 0.f;
            shadowMatrix.m12 = sin(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);

            shadowMatrix.m30 += shadowMatrix.m10 * posDiffY;
            shadowMatrix.m32 += shadowMatrix.m12 * posDiffY;

            shadowMatrix.Scale(boneScale);

            Matrix44::Convert(gfxRenderState::sm_World, shadowMatrix);
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;
            animationInstance->Draw(true);
        }
        else {
            auto active = reinterpret_cast<pedActive*>(inst->GetEntity());

            Matrix44 pedestrianMatrixList[32];

            gfxRenderState::sm_World->Identity();

            gfxRenderState::sm_World->m10 = cos(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);
            gfxRenderState::sm_World->m11 = 0.f;
            gfxRenderState::sm_World->m12 = sin(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);

            gfxRenderState::sm_World->m30 -= gfxRenderState::sm_World->m10 * shadowMatrix.m31;
            gfxRenderState::sm_World->m31 = shadowMatrix.m31;
            gfxRenderState::sm_World->m32 -= gfxRenderState::sm_World->m12 * shadowMatrix.m31;

            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

            active->getSkeleton()->Attach(&pedestrianMatrixList[0]);

            anim->pModel->Draw(&pedestrianMatrixList[0], anim->ppShaders[animationInstance->getVariant()], 0xFFFFFFFF);
        }
    }

    if ((&RSTATE->Data)->DestBlend != destBlend)
    {
        (&RSTATE->Data)->DestBlend = destBlend;
        gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
    }

    if ((&RSTATE->Data)->SrcBlend != srcBlend)
    {
        (&RSTATE->Data)->SrcBlend = srcBlend;
        gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
    }
}

void pedestrianInstanceHandler::Install()
{
    if (cfgMM1StyleShadows.Get() >= 2) {
        InstallVTableHook("aiPedestrianInstance::DrawShadow",
            &DrawShadow, {
                0x5B6320
            }
        );

        InstallCallback("aiPedestrian::Init", "Set flag id to 64 to render pedestrian shadows",
            &Init, {
                cb::call(0x53584F), // aiMap::Init
                cb::call(0x5358CC), // aiMap::Init
            }
        );
    }

    //are ragdolls enabled
    if (!cfgRagdolls.Get())
        return;

    InstallCallback("aiMap::Init", "aiMap initialization hook for ragdoll manager",
        &aiMapInit, {
            cb::call(0x412B98), // mmGame::Init
        }
    );

    InstallCallback("aiMap::Clean", "aiMap clean hook for ragdoll manager",
        &aiMapClean, {
            cb::call(0x413A44), // mmGame::Init
        }
    );

    InstallVTableHook("aiPedestrianInstance::Draw",
        &Draw, {
            0x5B631C
        }
    );
    InstallVTableHook("aiPedestrianInstance::GetBound",
        &GetBound, {
            0x5B6354
        }
    );

    InstallVTableHook("aiPedestrianInstance::IsCollidable",
        &IsCollidable , {
            0x5B6340
        }
    );
}

/*
    aiVehicleInstanceFeatureHandler
*/

bool aiVehicleInstanceFeatureHandler::InitVehicleGeom(const char* basename, const char* bodyGeom, int bodyFlags)
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);

    if (inst->BeginGeom(basename, bodyGeom, bodyFlags))
    {
        gfxForceLVERTEX = true;
        inst->AddGeom(basename, "SHADOW", 0);
        inst->AddGeom(basename, "HLIGHT", 0);
        inst->AddGeom(basename, "TLIGHT", 0);
        inst->AddGeom(basename, "SLIGHT0", 0);
        inst->AddGeom(basename, "SLIGHT1", 0);
        inst->AddGeom(basename, "BLIGHT", 0); //NEW MM2HOOK OBJECT
        gfxForceLVERTEX = false;

        inst->AddGeom(basename, "WHL0", 4);
        inst->AddGeom(basename, "WHL1", 4);
        inst->AddGeom(basename, "WHL2", 4);
        inst->AddGeom(basename, "WHL3", 4);

        inst->AddGeom(basename, "break0", 2);
        inst->AddGeom(basename, "break1", 2);
        inst->AddGeom(basename, "break2", 2);
        inst->AddGeom(basename, "break3", 2);

        //NEW MM2HOOK OBJECTS
        inst->AddGeom(basename, "break01", 2);
        inst->AddGeom(basename, "break12", 2);
        inst->AddGeom(basename, "break23", 2);
        inst->AddGeom(basename, "break03", 2);
        //NEW MM2HOOK OBJECTS

        gfxForceLVERTEX = true;
        inst->AddGeom(basename, "headlight0", 0);
        inst->AddGeom(basename, "headlight1", 0);
        gfxForceLVERTEX = false;

        inst->AddGeom(basename, "WHL4", 0);
        inst->AddGeom(basename, "WHL5", 0);

        //NEW MM2HOOK OBJECTS
        inst->AddGeom(basename, "PLIGHTON", 0);
        inst->AddGeom(basename, "PLIGHTOFF", 0);

        inst->AddGeom(basename, "SWHL0", 0);
        inst->AddGeom(basename, "SWHL1", 0);
        inst->AddGeom(basename, "SWHL2", 0);
        inst->AddGeom(basename, "SWHL3", 0);
        inst->AddGeom(basename, "SWHL4", 0);
        inst->AddGeom(basename, "SWHL5", 0);

        gfxForceLVERTEX = true;
        inst->AddGeom(basename, "TSLIGHT0", 0);
        inst->AddGeom(basename, "TSLIGHT1", 0);

        inst->AddGeom(basename, "FLIGHT0", 0);
        inst->AddGeom(basename, "FLIGHT1", 0);
        inst->AddGeom(basename, "FLIGHT2", 0);
        inst->AddGeom(basename, "FLIGHT3", 0);
        gfxForceLVERTEX = false;

        inst->AddGeom(basename, "breaklt0", 0);
        inst->AddGeom(basename, "breaklt1", 0);
        inst->AddGeom(basename, "breaklt2", 0);
        inst->AddGeom(basename, "breaklt3", 0);

        inst->EndGeom();
    }

    // return false so the game doesn't try loading the geometry again (we already took care of it above)
    return false;
}

bool aiVehicleInstanceFeatureHandler::InitBreakable(const char* basename, const char* breakableName, int geomId, int flags)
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);

    if (inst->GetGeomIndex() == 0 || lvlInstance::GetGeomTableEntry(geomId + (inst->GetGeomIndex() - 1))->GetHighLOD() == nullptr)
        return false;

    if (inst->getBreakableMgr() == nullptr)
    {
        inst->setBreakableMgr(new vehBreakableMgr());
        inst->getBreakableMgr()->Init(&inst->getSpline()->GetMatrix());
    }

    auto breakable = lvlInstance::GetGeomTableEntry(geomId + (inst->GetGeomIndex() - 1));
    inst->getBreakableMgr()->Add(basename, breakableName, breakable->LOD, geomId + inst->GetGeomIndex(), flags);

    return true;
}

void aiVehicleInstanceFeatureHandler::InitDefaultBreakables(const char* basename, const char* breakableName, int geomId)
{
    InitBreakable(basename, breakableName, geomId + 1, 0);
}

void aiVehicleInstanceFeatureHandler::InitAdditionalBreakables(const char* basename, const char* breakableName, int geomId)
{
    InitBreakable(basename, breakableName, geomId + 1, 0);
    InitBreakable(basename, "break01", 15, 0);
    InitBreakable(basename, "break12", 16, 0);
    InitBreakable(basename, "break23", 17, 0);
    InitBreakable(basename, "break03", 18, 0);
    InitBreakable(basename, "breaklt0", 37, 1);
    InitBreakable(basename, "breaklt1", 38, 2);
    InitBreakable(basename, "breaklt2", 39, 3);
    InitBreakable(basename, "breaklt3", 40, 4);
}

void aiVehicleInstanceFeatureHandler::DrawPart(modStatic* model, const Matrix34& matrix, modShader* shaders, bool reflected)
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);

    //setup renderer
    Matrix44::Convert(gfxRenderState::sm_World, matrix);
    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

    //draw model
    model->Draw(shaders);

    //draw reflections
    if (reflected && !(lvlLevel::Singleton->GetRoomInfo(inst->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean)))
    {
        float reflectionIntensity = 1.f;
        auto reflectionMap = lvlLevel::Singleton->GetEnvMap(inst->GetRoomId(), inst->GetPosition(), &reflectionIntensity);
        if (reflectionMap != nullptr)
        {
            modShader::BeginEnvMap(reflectionMap, matrix);
            model->DrawEnvMapped(shaders, reflectionMap, reflectionIntensity);
            modShader::EndEnvMap();
        }
    }
}

void aiVehicleInstanceFeatureHandler::Reset()
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);
    auto breakableMgr = inst->getBreakableMgr();

    inst->setSignalState(0);

    if (breakableMgr != nullptr)
        breakableMgr->Reset();
}

void aiVehicleInstanceFeatureHandler::Draw(int lod)
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);
    auto data = inst->GetData();
    auto spline = inst->getSpline();
    auto carMatrix = spline->GetMatrix();

    //get our geometry id
    int geomSetId = inst->GetGeomIndex();
    int geomSetIdOffset = geomSetId - 1;

    //get shaders
    auto mainGeomEntry = lvlInstance::GetGeomTableEntry(geomSetIdOffset);
    auto shaders = mainGeomEntry->pShaders[inst->getVariant()];

    //draw the body
    auto bodyModel = mainGeomEntry->GetLOD(lod);
    if (bodyModel != nullptr)
        DrawPart(bodyModel, carMatrix, shaders, lod == 3);

    //draw breakables
    inst->getBreakableMgr()->Draw(carMatrix, shaders, 3);

    //draw wheels (only in H LOD)
    if (lod == 3)
    {
        if (inst->GetEntity() != nullptr)
        {
            auto vehicleActive = (aiVehicleActive*)inst->GetEntity();

            for (int i = 0; i < 4; i++)
            {
                modStatic* wheelModel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 7 + i)->GetHighLOD();
                if (wheelModel != nullptr)
                    DrawPart(wheelModel, vehicleActive->getWheel(i)->getMatrix(), shaders, vehCarModel::PartReflections);
            }

            modStatic* whl4Model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 21)->GetHighLOD();
            if (whl4Model != nullptr)
            {
                Matrix34 whl4Matrix;

                whl4Matrix.Identity();
                whl4Matrix.SetRow(3, data->GetWheelPosition(4));
                whl4Matrix.Dot(carMatrix);

                DrawPart(whl4Model, whl4Matrix, shaders, vehCarModel::PartReflections);
            }

            modStatic* whl5Model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 22)->GetHighLOD();
            if (whl5Model != nullptr)
            {
                Matrix34 whl5Matrix;

                whl5Matrix.Identity();
                whl5Matrix.SetRow(3, data->GetWheelPosition(5));
                whl5Matrix.Dot(carMatrix);

                DrawPart(whl5Model, whl5Matrix, shaders, vehCarModel::PartReflections);
            }
        }
        else {
            for (int i = 0; i < 4; i++)
            {
                Matrix34 wheelMatrix;

                wheelMatrix.Identity();
                wheelMatrix.SetRow(3, data->GetWheelPosition(i));
                wheelMatrix.MakeRotateX(-spline->getWheelRotation());
                wheelMatrix.Dot(carMatrix);

                modStatic* wheelModel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 7 + i)->GetHighLOD();
                if (wheelModel != nullptr)
                {
                    modStatic* sWheelModel = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 25 + i)->GetHighLOD();
                    if (spline->getCurSpeed() > 20.f && sWheelModel != nullptr && vehCarModel::EnableSpinningWheels)
                    {
                        DrawPart(sWheelModel, wheelMatrix, shaders, vehCarModel::PartReflections);
                    }
                    else {
                        DrawPart(wheelModel, wheelMatrix, shaders, vehCarModel::PartReflections);
                    }
                }
            }

            modStatic* whl4Model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 21)->GetHighLOD();
            if (whl4Model != nullptr)
            {
                Matrix34 whl4Matrix;

                whl4Matrix.Identity();
                whl4Matrix.SetRow(3, data->GetWheelPosition(4));
                whl4Matrix.MakeRotateX(-spline->getWheelRotation());
                whl4Matrix.Dot(carMatrix);

                modStatic* swhl4Model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 29)->GetHighLOD();
                if (spline->getCurSpeed() > 20.f && swhl4Model != nullptr && vehCarModel::EnableSpinningWheels)
                {
                    DrawPart(swhl4Model, whl4Matrix, shaders, vehCarModel::PartReflections);
                }
                else {
                    DrawPart(whl4Model, whl4Matrix, shaders, vehCarModel::PartReflections);
                }
            }

            modStatic* whl5Model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 22)->GetHighLOD();
            if (whl5Model != nullptr)
            {
                Matrix34 whl5Matrix;

                whl5Matrix.Identity();
                whl5Matrix.SetRow(3, data->GetWheelPosition(5));
                whl5Matrix.MakeRotateX(-spline->getWheelRotation());
                whl5Matrix.Dot(carMatrix);

                modStatic* swhl5Model = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 30)->GetHighLOD();
                if (spline->getCurSpeed() > 20.f && swhl5Model != nullptr && vehCarModel::EnableSpinningWheels)
                {
                    DrawPart(swhl5Model, whl5Matrix, shaders, vehCarModel::PartReflections);
                }
                else {
                    DrawPart(whl5Model, whl5Matrix, shaders, vehCarModel::PartReflections);
                }
            }
        }
    }

    //draw pop-up headlights
    if (lod >= 2)
    {
        modStatic* plighton = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 23)->GetHighLOD();
        if (plighton != nullptr)
        {
            if (aiMap::Instance->drawHeadlights)
                DrawPart(plighton, carMatrix, shaders, vehCarModel::PartReflections);
        }

        modStatic* plightoff = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 24)->GetHighLOD();
        if (plightoff != nullptr)
        {
            if (!aiMap::Instance->drawHeadlights)
                DrawPart(plightoff, carMatrix, shaders, vehCarModel::PartReflections);
        }
    }

    inst->setLOD(lod + 1);
}

void aiVehicleInstanceFeatureHandler::DrawShadow()
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);

    //get our geometry id
    int geomSetId = inst->GetGeomIndex();
    int geomSetIdOffset = geomSetId - 1;

    auto vehicleMatrix = inst->getSpline()->GetMatrix();
    auto timeWeather = $::timeWeathers.ptr(cityLevel::timeOfDay);

    //get shaders
    auto shaders = lvlInstance::GetGeomTableEntry(geomSetIdOffset)->pShaders[inst->getVariant()];

    //get shadow
    modStatic* shadow = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 1)->GetHighLOD();

    if (shadow != nullptr)
    {
        Matrix34 shadowMatrix;

        if (lvlInstance::ComputeShadowMatrix(&shadowMatrix, inst->GetRoomId(), &vehicleMatrix))
        {
            RSTATE->SetBlendSet(0, 0x80);

            Matrix44::Convert(gfxRenderState::sm_World, shadowMatrix);
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

            shadow->Draw(shaders);
        }
    }

    if (MMSTATE->TimeOfDay == 3 || MMSTATE->WeatherType != 0 || cfgMM1StyleShadows.Get() < 4 ||
        lvlLevel::Singleton->GetRoomInfo(inst->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean))
        return;

    modStatic* body = lvlInstance::GetGeomTableEntry(geomSetIdOffset)->GetHighLOD();

    if (body != nullptr)
    {
        int srcBlend = (&RSTATE->Data)->SrcBlend;
        if ((&RSTATE->Data)->SrcBlend != 0)
        {
            (&RSTATE->Data)->SrcBlend = 0;
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
        }

        int destBlend = (&RSTATE->Data)->DestBlend;
        if (cfgShadowTransparency.Get())
        {
            if ((&RSTATE->Data)->DestBlend != 9)
            {
                (&RSTATE->Data)->DestBlend = 9;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
        }
        else {
            if ((&RSTATE->Data)->DestBlend != 0)
            {
                (&RSTATE->Data)->DestBlend = 0;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
        }

        Matrix34 shadowMatrix;

        if (lvlInstance::ComputeShadowMatrix(&shadowMatrix, inst->GetRoomId(), &vehicleMatrix))
        {
            float posDiffY = vehicleMatrix.m31 - shadowMatrix.m31;

            shadowMatrix.m10 = cos(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);
            shadowMatrix.m11 = 0.f;
            shadowMatrix.m12 = sin(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);

            shadowMatrix.m30 += shadowMatrix.m10 * posDiffY;
            shadowMatrix.m32 += shadowMatrix.m12 * posDiffY;

            Matrix44::Convert(gfxRenderState::sm_World, shadowMatrix);
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

            body->Draw(shaders);
        }

        if ((&RSTATE->Data)->DestBlend != destBlend)
        {
            (&RSTATE->Data)->DestBlend = destBlend;
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
        }

        if ((&RSTATE->Data)->SrcBlend != srcBlend)
        {
            (&RSTATE->Data)->SrcBlend = srcBlend;
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
        }
    }
}

void aiVehicleInstanceFeatureHandler::DrawGlow()
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);
    auto spline = inst->getSpline();
    auto carMatrix = spline->GetMatrix();

    //setup renderer
    Matrix44::Convert(gfxRenderState::sm_World, carMatrix);
    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

    //get our geometry id
    int geomSetId = inst->GetGeomIndex();
    int geomSetIdOffset = geomSetId - 1;

    //get shaders
    auto shaders = lvlInstance::GetGeomTableEntry(geomSetIdOffset)->pShaders[inst->getVariant()];

    //get lights
    modStatic* hlight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 2)->GetHighestLOD();
    modStatic* tlight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 3)->GetHighestLOD();
    modStatic* slight0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 4)->GetHighestLOD();
    modStatic* slight1 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 5)->GetHighestLOD();
    modStatic* blight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 6)->GetHighestLOD();
    modStatic* tslight0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 31)->GetHighestLOD();
    modStatic* tslight1 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 32)->GetHighestLOD();

    //draw brake light
    if (blight != nullptr)
    {
        if (spline->getRailSet()->getCurAccelFactor() < 0.f || spline->getCurSpeed() == 0.f)
            blight->Draw(shaders);
    }

    //draw taillight
    if (tlight != nullptr)
    {
        //draw brake copy
        if (spline->getRailSet()->getCurAccelFactor() < 0.f || spline->getCurSpeed() == 0.f)
            tlight->Draw(shaders);
        //draw headlight copy
        if (aiMap::Instance->drawHeadlights)
            tlight->Draw(shaders);
    }

    //draw left signal
    if (inst->getSignalState() & 1)
    {
        if ((aiVehicleManager::SignalClock + inst->getSignalFrequency()) & 8)
        {
            if (slight0 != nullptr)
                slight0->Draw(shaders);
            if (tslight0 != nullptr)
                tslight0->Draw(shaders);
        }
    }
    else {
        if (tslight0 != nullptr) {
            //draw brake copy
            if (spline->getRailSet()->getCurAccelFactor() < 0.f || spline->getCurSpeed() == 0.f)
                tslight0->Draw(shaders);
            //draw headlight copy
            if (aiMap::Instance->drawHeadlights)
                tslight0->Draw(shaders);
        }
    }

    //draw right signal
    if (inst->getSignalState() & 2)
    {
        if ((aiVehicleManager::SignalClock + inst->getSignalFrequency()) & 8)
        {
            if (slight1 != nullptr)
                slight1->Draw(shaders);
            if (tslight1 != nullptr)
                tslight1->Draw(shaders);
        }
    }
    else {
        if (tslight1 != nullptr) {
            //draw brake copy
            if (spline->getRailSet()->getCurAccelFactor() < 0.f || spline->getCurSpeed() == 0.f)
                tslight1->Draw(shaders);
            //draw headlight copy
            if (aiMap::Instance->drawHeadlights)
                tslight1->Draw(shaders);
        }
    }

    //draw headlights
    if (aiVehicleInstance::AmbientHeadlightStyle == 0 || aiVehicleInstance::AmbientHeadlightStyle == 2)
    {
        //MM2 headlights
        if (aiMap::Instance->drawHeadlights)
        {
            DrawHeadlights();
        }
    }
    if (aiVehicleInstance::AmbientHeadlightStyle == 1 || aiVehicleInstance::AmbientHeadlightStyle == 2)
    {
        //MM1 headlights
        Matrix44::Convert(gfxRenderState::sm_World, carMatrix);
        gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

        if (hlight != nullptr && aiMap::Instance->drawHeadlights)
            hlight->Draw(shaders);

        if (MMSTATE->WeatherType == 2)
        {
            for (int i = 0; i < 4; i++)
            {
                modStatic* flight = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 33 + i)->GetHighestLOD();
                if (flight != nullptr)
                {
                    auto breaklt = inst->getBreakableMgr()->Get(i + 1);
                    if (breaklt == nullptr || breaklt->IsAttached)
                    {
                        flight->Draw(shaders);
                    }
                }
            }
        }
    }
}

void aiVehicleInstanceFeatureHandler::DrawHeadlights()
{
    auto inst = reinterpret_cast<aiVehicleInstance*>(this);
    auto carMatrix = inst->getSpline()->GetMatrix();

    int geomSetId = inst->GetGeomIndex();
    int geomSetIdOffset = geomSetId - 1;

    Vector3 camPosition = *(Vector3*)&gfxRenderState::sm_Camera->m30;

    //prepare glow texture
    gfxTexture* lastTexture = ltLight::GlowTexture;
    bool swappedTexture = false;

    //swap texture if it exists
    if (lastTexture != NULL && AmbientTexture != NULL) {
        swappedTexture = true;
        ltLight::GlowTexture = AmbientTexture;
    }

    ltLight::DrawGlowBegin();

    auto light = aiVehicleManager::Instance->getSharedLight();
    auto lightPos = inst->getHeadlightPosition();

    light->Direction = Vector3(-carMatrix.m20, -carMatrix.m21, -carMatrix.m22);

    modStatic* headlight0 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 19)->GetHighLOD();
    if (headlight0 != nullptr)
    {
        light->Position.Dot(lightPos, carMatrix);

        Vector3 vec;

        vec.Set(light->Position);
        vec.Subtract(camPosition);

        float invGlowScale = -ltLight::GlowScale;

        if (cfgLightClippingFix.Get() == 2 || cfgLightClippingFix.Get() >= 4)
        {
            lvlSegment segment;
            lvlIntersection intersection;

            segment.Set(light->Position, camPosition, 0, nullptr);

            light->Position.AddScaled(vec, invGlowScale * 0.1f);

            if (Collide(segment, &intersection, 0, nullptr, SpecialBound | Warp, 0))
            {
                invGlowScale = 0.f;
            }

            light->Position.AddScaled(vec, invGlowScale * 0.25f);
        }
        else
        {
            light->Position.AddScaled(vec, invGlowScale * 0.1f);
        }

        light->DrawGlow(camPosition);
    }

    modStatic* headlight1 = lvlInstance::GetGeomTableEntry(geomSetIdOffset + 20)->GetHighLOD();
    if (headlight1 != nullptr)
    {
        lightPos.X *= -1.f;
        light->Position.Dot(lightPos, carMatrix);

        Vector3 vec;

        vec.Set(light->Position);
        vec.Subtract(camPosition);

        float invGlowScale = -ltLight::GlowScale;

        if (cfgLightClippingFix.Get() == 2 || cfgLightClippingFix.Get() >= 4)
        {
            lvlSegment segment;
            lvlIntersection intersection;

            segment.Set(light->Position, camPosition, 0, nullptr);

            light->Position.AddScaled(vec, invGlowScale * 0.1f);

            if (Collide(segment, &intersection, 0, nullptr, SpecialBound | Warp, 0))
            {
                invGlowScale = 0.f;
            }

            light->Position.AddScaled(vec, invGlowScale * 0.25f);
        }
        else
        {
            light->Position.AddScaled(vec, invGlowScale * 0.1f);
        }

        light->DrawGlow(camPosition);
    }

    ltLight::DrawGlowEnd();

    //reset glow texture
    if (swappedTexture) {
        ltLight::GlowTexture = lastTexture;
    }
}

bool aiVehicleInstanceFeatureHandler::Collide(lvlSegment& segment, lvlIntersection* intersection, int roomId, lvlInstance* ignoreInstance, ushort instanceFlags, int collideFlags)
{
    lvlSegmentInfo segmentInfo;
    memset(&segmentInfo, 0, sizeof(segmentInfo));
    if (!segment.SegmentInfo)
    {
        segment.SegmentInfo = &segmentInfo;
        segmentInfo.StartRoomId = roomId;
        segmentInfo.EndRoomId = roomId;
    }

    auto level = *lvlLevel::Singleton;

    int startRoomId = level->FindRoomId(segment.StartPos, segment.SegmentInfo->StartRoomId);

    segment.SegmentInfo->StartRoomId = startRoomId;

    int endRoomId = level->FindRoomId(segment.EndPos, segment.SegmentInfo->EndRoomId);

    segment.SegmentInfo->EndRoomId = endRoomId;

    intersection->IntersectionPoint.NormalizedDistance = 2.0;

    intersection->Poly = 0;

    bool collide = false;

    auto startRoomInfo = level->GetRoomInfo(startRoomId);
    if ((startRoomInfo->InstanceFlags & instanceFlags) != 0) // collide in start room
    {
        for (lvlInstance* i = startRoomInfo->FirstInstance; i; i = i->GetNext())
        {
            if ((i->GetFlags() & instanceFlags) != 0)
            {
                if ((i->GetFlags() & collideFlags) == 0 && i != ignoreInstance)
                {
                    collide |= dgPhysManager::Instance->CollideProbe(segment, intersection, i);
                }
            }
        }
    }

    if (endRoomId != startRoomId) // collide in end room
    {
        auto endRoomInfo = level->GetRoomInfo(endRoomId);
        if ((endRoomInfo->InstanceFlags & instanceFlags) != 0)
        {
            for (lvlInstance* i = endRoomInfo->FirstInstance; i; i = i->GetNext())
            {
                if ((i->GetFlags() & instanceFlags) != 0)
                {
                    if ((i->GetFlags() & collideFlags) == 0 && i != ignoreInstance)
                    {
                        collide |= dgPhysManager::Instance->CollideProbe(segment, intersection, i);
                    }
                }
            }
        }
    }

    auto roomInfo = level->GetRoomInfo(startRoomId);  // sf warp hack
    if ((roomInfo->Flags & Warp) != 0)
    {
        int roomId;
        lvlRoomInfo* roomInfo2;
        switch (startRoomId)
        {
        case 411:
            roomId = 102;
            break;
        case 412:
            roomId = 122;
            break;
        case 423:
            roomId = 96;
            break;
        case 625:
            roomId = 1;
            break;
        default:
            return collide;
        }
        roomInfo2 = level->GetRoomInfo(roomId);
        if ((roomInfo2->InstanceFlags & instanceFlags) != 0)
        {
            for (lvlInstance* i = roomInfo2->FirstInstance; i; i = i->GetNext())
            {
                if ((i->GetFlags() & instanceFlags) != 0)
                {
                    if ((i->GetFlags() & collideFlags) == 0 && i != ignoreInstance)
                    {
                        collide |= dgPhysManager::Instance->CollideProbe(segment, intersection, i);
                    }
                }
            }
        }
    }
    return collide;
}

void aiVehicleInstanceFeatureHandler::Install()
{
    InstallCallback("aiVehicleInstance::aiVehicleInstance", "Adds more geometries.",
        &InitVehicleGeom, {
            cb::call(0x551E17),
        }
    );

    InstallCallback("aiVehicleInstance::aiVehicleInstance", "Clamps default breakable geom ids.",
        &InitDefaultBreakables, {
            cb::call(0x551F70),
            cb::call(0x551F7F),
            cb::call(0x551F8E),
        }
    );

    InstallCallback("aiVehicleInstance::aiVehicleInstance", "Adds more breakables.",
        &InitAdditionalBreakables, {
            cb::call(0x551F9D),
        }
    );

    InstallVTableHook("aiVehicleInstance::Reset",
        &Reset, {
            0x5B590C,
        }
    );

    InstallVTableHook("aiVehicleInstance::Draw",
        &Draw, {
            0x5B5938,
        }
    );

    InstallVTableHook("aiVehicleInstance::DrawShadow",
        &DrawShadow, {
            0x5B593C,
        }
    );

    InstallVTableHook("aiVehicleInstance::DrawGlow",
        &DrawGlow, {
            0x5B5944,
        }
    );

    //clamp headlight0 geom id to 19 (14 + 5)
    InstallPatch({ 0x39, 0x98, 0x24, 0x19, 0x63, 0x0 }, {
        0x551FCE,
    });

    ConfigValue<int> cfgAmbientHeadlightStyle("AmbientHeadlightStyle", 0);

    aiVehicleInstance::AmbientHeadlightStyle = cfgAmbientHeadlightStyle.Get();
}

/*
    vehTrailerFeatureHandler
*/

void vehTrailerFeatureHandler::Update() {
    //call original
    hook::Thunk<0x4D7B00>::Call<void>(this);
}

void vehTrailerFeatureHandler::Reset() {
    //call original
    hook::Thunk<0x4D79C0>::Call<void>(this);
}

void vehTrailerFeatureHandler::Install() {
    InstallCallback("vehTrailer::Init", "Reads TWHL4/5 MTX files",
        &vehTrailer::Init, {
            cb::call(0x42C023),
        }
    );

    InstallVTableHook("vehTrailer::Update",
        &Update, {
            0x5B2F64,
        }
    );

    InstallVTableHook("vehTrailer::Reset",
        &Reset, {
            0x5B2F34,
        }
    );

    // Disable Angel's trailer break key
    InstallPatch({ 0xEB }, {
        0x5932E2 // dgTrailerJoint::Update
    });
}

/*
    vehTrailerInstanceFeatureHandler
*/
Matrix34 trailerMatrix = Matrix34();

void vehTrailerInstanceFeatureHandler::DrawPartReflections(modStatic* a1, const Matrix34& a2, modShader* a3) {
    hook::Type<gfxTexture*> g_ReflectionMap = 0x628914;
    bool isSoftware = *(bool*)0x6830D4;

    //setup renderer
    Matrix44::Convert(gfxRenderState::sm_World, a2);
    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

    //draw trailer
    a1->Draw(a3);

    //draw reflections
    auto state = &MMSTATE;
    if (g_ReflectionMap != nullptr && !isSoftware && state->EnableReflections) {
        modShader::BeginEnvMap(g_ReflectionMap, a2);
        a1->DrawEnvMapped(a3, g_ReflectionMap, 1.0f);
        modShader::EndEnvMap();
    }
}

void vehTrailerInstanceFeatureHandler::DrawPart(int a1, int a2, const Matrix34& a3, modShader* a4) {
    auto inst = reinterpret_cast<vehTrailerInstance*>(this);
    auto geomID = inst->GetGeomIndex() - 1;
    auto geomSet = lvlInstance::GetGeomTableEntry(geomID);

    //setup renderer
    Matrix44::Convert(gfxRenderState::sm_World, a3);
    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

    //get part
    modStatic* part = *getPtr<modStatic*>((geomSet + a2), a1 * 4);

    if (part != nullptr) {
        if (vehCarModel::PartReflections && a1 == 3 &&
            !(lvlLevel::Singleton->GetRoomInfo(inst->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean)))
            DrawPartReflections(part, a3, a4);
        else
            part->Draw(a4);
    }
}

void vehTrailerInstanceFeatureHandler::Draw(int a1) {
    auto inst = reinterpret_cast<vehTrailerInstance*>(this);
    auto geomID = inst->GetGeomIndex() - 1;
    auto geomSet = lvlInstance::GetGeomTableEntry(geomID);
    auto trailer = inst->getTrailer();
    auto trailerMtx = inst->GetMatrix(&trailerMatrix);
    auto twhl0 = trailer->getWheel(0);
    auto twhl1 = trailer->getWheel(1);
    auto twhl2 = trailer->getWheel(2);
    auto twhl3 = trailer->getWheel(3);

    //get our shader set
    int shaderSet = *getPtr<int>(this, 24);
    auto shaders = geomSet->pShaders[shaderSet];

    //get spinning wheels
    modStatic* tswhl0 = *getPtr<modStatic*>((geomSet + 17), a1 * 4);
    modStatic* tswhl1 = *getPtr<modStatic*>((geomSet + 18), a1 * 4);
    modStatic* tswhl2 = *getPtr<modStatic*>((geomSet + 19), a1 * 4);
    modStatic* tswhl3 = *getPtr<modStatic*>((geomSet + 20), a1 * 4);
    modStatic* tswhl4 = *getPtr<modStatic*>((geomSet + 21), a1 * 4);
    modStatic* tswhl5 = *getPtr<modStatic*>((geomSet + 22), a1 * 4);

    vehWheel* wheels[4] = { twhl0, twhl1, twhl2, twhl3 };
    modStatic* sWhlGeometries[4] = { tswhl0, tswhl1, tswhl2, tswhl3 };
    int sWhlIds[4] = { 17, 18, 19, 20 };
    int whlIds[4] = { 3, 4, 5, 6 };

    //draw trailer
    DrawPart(a1, 0, trailerMtx, shaders);

    //draw (s)whl0-4
    for (int i = 0; i < 4; i++) {
        auto wheel = wheels[i];
        if (fabs(wheel->getRotationRate()) > 26.f && sWhlGeometries[i] != nullptr && vehCarModel::EnableSpinningWheels)
        {
            DrawPart(a1, sWhlIds[i], wheel->getMatrix(), shaders);
        }
        else {
            DrawPart(a1, whlIds[i], wheel->getMatrix(), shaders);
        }
    }

    if (fabs(twhl2->getRotationRate()) > 26.f && tswhl4 != nullptr && vehCarModel::EnableSpinningWheels)
    {
        DrawTwhl4(a1, 21, twhl2->getMatrix(), shaders);
    }
    else {
        DrawTwhl4(a1, 15, twhl2->getMatrix(), shaders);
    }

    if (fabs(twhl3->getRotationRate()) > 26.f && tswhl5 != nullptr && vehCarModel::EnableSpinningWheels)
    {
        DrawTwhl5(a1, 22, twhl3->getMatrix(), shaders);
    }
    else {
        DrawTwhl5(a1, 16, twhl3->getMatrix(), shaders);
    }
}

void vehTrailerInstanceFeatureHandler::DrawTwhl4(int a1, int a2, Matrix34& a3, modShader* a4) {
    auto inst = reinterpret_cast<vehTrailerInstance*>(this);
    auto trailer = inst->getTrailer();
    auto carsim = trailer->getTrailerJoint()->getCarSim();

    a3.Set(trailer->getWheel(2)->getMatrix());
    auto trailerMtx = inst->GetMatrix(&trailerMatrix);

    float offsetX = carsim->TrailerBackBackLeftWheelPosDiff.Y * trailerMtx.m10 + carsim->TrailerBackBackLeftWheelPosDiff.Z * trailerMtx.m20 + carsim->TrailerBackBackLeftWheelPosDiff.X * trailerMtx.m00;
    float offsetY = carsim->TrailerBackBackLeftWheelPosDiff.Y * trailerMtx.m11 + carsim->TrailerBackBackLeftWheelPosDiff.Z * trailerMtx.m21 + carsim->TrailerBackBackLeftWheelPosDiff.X * trailerMtx.m01;
    float offsetZ = carsim->TrailerBackBackLeftWheelPosDiff.Y * trailerMtx.m12 + carsim->TrailerBackBackLeftWheelPosDiff.Z * trailerMtx.m22 + carsim->TrailerBackBackLeftWheelPosDiff.X * trailerMtx.m02;
    a3.m30 += offsetX;
    a3.m31 += offsetY;
    a3.m32 += offsetZ;

    DrawPart(a1, a2, a3, a4);
}

void vehTrailerInstanceFeatureHandler::DrawTwhl5(int a1, int a2, Matrix34& a3, modShader* a4) {
    auto inst = reinterpret_cast<vehTrailerInstance*>(this);
    auto trailer = inst->getTrailer();
    auto carsim = trailer->getTrailerJoint()->getCarSim();

    a3.Set(trailer->getWheel(3)->getMatrix());
    auto trailerMtx = inst->GetMatrix(&trailerMatrix);

    float offsetX = carsim->TrailerBackBackRightWheelPosDiff.Y * trailerMtx.m10 + carsim->TrailerBackBackRightWheelPosDiff.Z * trailerMtx.m20 + carsim->TrailerBackBackRightWheelPosDiff.X * trailerMtx.m00;
    float offsetY = carsim->TrailerBackBackRightWheelPosDiff.Y * trailerMtx.m11 + carsim->TrailerBackBackRightWheelPosDiff.Z * trailerMtx.m21 + carsim->TrailerBackBackRightWheelPosDiff.X * trailerMtx.m01;
    float offsetZ = carsim->TrailerBackBackRightWheelPosDiff.Y * trailerMtx.m12 + carsim->TrailerBackBackRightWheelPosDiff.Z * trailerMtx.m22 + carsim->TrailerBackBackRightWheelPosDiff.X * trailerMtx.m02;
    a3.m30 += offsetX;
    a3.m31 += offsetY;
    a3.m32 += offsetZ;

    DrawPart(a1, a2, a3, a4);
}

void vehTrailerInstanceFeatureHandler::DrawShadow() {
    auto inst = reinterpret_cast<vehTrailerInstance*>(this);
    auto trailerMtx = inst->GetMatrix(&trailerMatrix);
    int geomSet = inst->GetGeomIndex() - 1;

    //get our shader set
    int shaderSet = *getPtr<int>(this, 24);
    auto shaders = lvlInstance::GetGeomTableEntry(geomSet)->pShaders[shaderSet];

    //get time weather
    auto timeWeather = $::timeWeathers.ptr(cityLevel::timeOfDay);

    //get model
    modStatic* shadow = lvlInstance::GetGeomTableEntry(geomSet + 1)->GetHighLOD();

    if (shadow != nullptr)
    {
        Matrix34 shadowMatrix;

        if (lvlInstance::ComputeShadowMatrix(&shadowMatrix, inst->GetRoomId(), &trailerMtx))
        {
            RSTATE->SetBlendSet(0, 0x80);

            Matrix44::Convert(gfxRenderState::sm_World, shadowMatrix);
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

            shadow->Draw(shaders);
        }
    }

    if (MMSTATE->TimeOfDay == 3 || MMSTATE->WeatherType != 0 ||
        lvlLevel::Singleton->GetRoomInfo(inst->GetRoomId())->Flags & static_cast<int>(RoomFlags::Subterranean))
        return;

    modStatic* trailer = lvlInstance::GetGeomTableEntry(geomSet)->GetHighLOD();

    if (trailer != nullptr)
    {
        int srcBlend = (&RSTATE->Data)->SrcBlend;
        if ((&RSTATE->Data)->SrcBlend != 0)
        {
            (&RSTATE->Data)->SrcBlend = 0;
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
        }

        int destBlend = (&RSTATE->Data)->DestBlend;
        if (cfgShadowTransparency.Get())
        {
            if ((&RSTATE->Data)->DestBlend != 9)
            {
                (&RSTATE->Data)->DestBlend = 9;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
        }
        else {
            if ((&RSTATE->Data)->DestBlend != 0)
            {
                (&RSTATE->Data)->DestBlend = 0;
                gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
            }
        }

        Matrix34 shadowMatrix;

        if (lvlInstance::ComputeShadowMatrix(&shadowMatrix, inst->GetRoomId(), &trailerMtx))
        {
            float posDiffY = trailerMtx.m31 - shadowMatrix.m31;

            shadowMatrix.m10 = cos(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);
            shadowMatrix.m11 = 0.f;
            shadowMatrix.m12 = sin(timeWeather->KeyHeading) * cos(timeWeather->KeyPitch);

            shadowMatrix.m30 += shadowMatrix.m10 * posDiffY;
            shadowMatrix.m32 += shadowMatrix.m12 * posDiffY;

            Matrix44::Convert(gfxRenderState::sm_World, shadowMatrix);
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

            trailer->Draw(shaders);
        }

        if ((&RSTATE->Data)->DestBlend != destBlend)
        {
            (&RSTATE->Data)->DestBlend = destBlend;
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
        }

        if ((&RSTATE->Data)->SrcBlend != srcBlend)
        {
            (&RSTATE->Data)->SrcBlend = srcBlend;
            gfxRenderState::m_Touched = gfxRenderState::m_Touched | 1;
        }
    }
}

void vehTrailerInstanceFeatureHandler::DrawGlow() {
    auto inst = reinterpret_cast<vehTrailerInstance*>(this);
    //don't draw trailer lights if it's broken
    if (inst->getTrailer()->getTrailerJoint()->IsBroken())
        return;

    //get vars
    auto trailer = inst->getTrailer();
    auto carsim = trailer->getTrailerJoint()->getCarSim();
    auto model = reinterpret_cast<vehCarModel*>(carsim->getInstance());
    float brakeInput = carsim->getBrake();
    int gear = carsim->getTransmission()->getGear();
    int geomSet = inst->GetGeomIndex() - 1;

    //setup renderer
    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;
    inst->GetMatrix(&trailerMatrix);
    Matrix44::Convert(gfxRenderState::sm_World, trailerMatrix);

    //get our shader set
    int shaderSet = *getPtr<int>(this, 24);
    auto shaders = lvlInstance::GetGeomTableEntry(geomSet)->pShaders[shaderSet];

    //get lights
    modStatic* tlight = lvlInstance::GetGeomTableEntry(geomSet + 2)->GetHighestLOD();
    modStatic* rlight = lvlInstance::GetGeomTableEntry(geomSet + 8)->GetHighestLOD();
    modStatic* blight = lvlInstance::GetGeomTableEntry(geomSet + 9)->GetHighestLOD();
    modStatic* hlight = lvlInstance::GetGeomTableEntry(geomSet + 10)->GetHighestLOD();
    modStatic* slight0 = lvlInstance::GetGeomTableEntry(geomSet + 11)->GetHighestLOD();
    modStatic* slight1 = lvlInstance::GetGeomTableEntry(geomSet + 12)->GetHighestLOD();
    modStatic* siren0 = lvlInstance::GetGeomTableEntry(geomSet + 13)->GetHighestLOD();
    modStatic* siren1 = lvlInstance::GetGeomTableEntry(geomSet + 14)->GetHighestLOD();
    modStatic* tslight0 = lvlInstance::GetGeomTableEntry(geomSet + 23)->GetHighestLOD();
    modStatic* tslight1 = lvlInstance::GetGeomTableEntry(geomSet + 24)->GetHighestLOD();
    modStatic* flight = lvlInstance::GetGeomTableEntry(geomSet + 25)->GetHighestLOD();

    //draw rlight
    if (rlight != nullptr && gear == 0) {
        rlight->Draw(shaders);
    }

    //draw blight
    if (blight != nullptr && brakeInput > 0.1) {
        blight->Draw(shaders);
    }

    //draw tlight
    if (tlight != nullptr) {
        //draw night copy
        if (model->getHeadlightsState())
            tlight->Draw(shaders);

        //draw brake input copy
        if (brakeInput > 0.1) {
            tlight->Draw(shaders);
        }
    }

    //draw hlight
    if (hlight != nullptr) {
        if (model->getHeadlightsState())
            hlight->Draw(shaders);
    }

    //check signal clock
    float randValue = (irand2(model) % 8) * 0.001f;
    bool drawSignal = fmod(datTimeManager::ElapsedTime, 1.f + randValue) > 0.5f + randValue;

    //draw stuff!
    if (drawSignal) {
        if (model->getLeftSignalLightState() || model->getHazardLightsState()) {
            if (slight0 != nullptr)
                slight0->Draw(shaders);
            if (tslight0 != nullptr)
                tslight0->Draw(shaders);
        }
        if (model->getRightSignalLightState() || model->getHazardLightsState()) {
            if (slight1 != nullptr)
                slight1->Draw(shaders);
            if (tslight1 != nullptr)
                tslight1->Draw(shaders);
        }
    }

    //draw taillight signals
    if (!model->getLeftSignalLightState() && !model->getHazardLightsState()) {
        if (tslight0 != nullptr) {
            //draw night copy
            if (model->getHeadlightsState())
                tslight0->Draw(shaders);

            //draw brake input copy
            if (brakeInput > 0.1) {
                tslight0->Draw(shaders);
            }
        }
    }
    if (!model->getRightSignalLightState() && !model->getHazardLightsState()) {
        if (tslight1 != nullptr) {
            //draw night copy
            if (model->getHeadlightsState())
                tslight1->Draw(shaders);

            //draw brake input copy
            if (brakeInput > 0.1) {
                tslight1->Draw(shaders);
            }
        }
    }

    //draw siren
    if (model->getCar()->getSiren()->getActive()) {
        bool drawLEDSiren = fmod(datTimeManager::ElapsedTime, 0.1f + randValue) > 0.05f + randValue;

        if (!vehCarModel::EnableLEDSiren || drawLEDSiren) {
            int sirenStage = fmod(datTimeManager::ElapsedTime, 2 * (vehCarModel::SirenCycle + randValue)) >= (vehCarModel::SirenCycle + randValue) ? 1 : 0;
            if (sirenStage == 0 && siren0 != nullptr) {
                siren0->Draw(shaders);
            }
            else if (sirenStage == 1 && siren1 != nullptr) {
                siren1->Draw(shaders);
            }
        }
    }

    //draw flight
    if (flight != nullptr) {
        if (model->getFoglightsState())
            flight->Draw(shaders);
    }
}

void vehTrailerInstanceFeatureHandler::Install() {
    InstallCallback("vehTrailerInstance::Init", "Use rewritten vehTrailerInstance init.",
        &vehTrailerInstance::Init, {
            cb::call(0x4D7350),
        }
    );

    InstallVTableHook("vehTrailerInstance::Draw",
        &Draw, {
            0x5B2FB0,
        }
    );

    if (cfgMM1StyleShadows.Get() >= 3) {
        InstallVTableHook("vehTrailerInstance::DrawShadow",
            &DrawShadow, {
                0x5B2FB4,
            }
        );
    }

    // adds custom light rendering, which adds proper brake lights,
    // reverse lights, signal lights, siren lights and night lights
    InstallVTableHook("vehTrailerInstance::DrawGlow",
        &DrawGlow, {
            0x5B2FBC,
        }
    );
}

/*
    vehCarSimHandler
*/

void vehCarSimHandler::Install()
{
    InstallPatch({ 0x90 }, {
        0x42BB4B + 1, // Change size of vehCarSim on allocation
    });

    InstallCallback("vehCarSim::Init", "Use our own init function.",
        &vehCarSim::Init, {
            cb::call(0x403C21),
            cb::call(0x42BE75),
            cb::call(0x43C573),
            cb::call(0x43C6DF),
        }
    );
}

/*
    vehCableCarInstanceHandler
*/

Matrix34 cableCarMatrix = Matrix34();

void vehCableCarInstanceHandler::DrawShadow()
{
    //get vars
    auto inst = reinterpret_cast<lvlInstance*>(this);
    int geomSet = inst->GetGeomIndex() - 1;

    //get our shader set
    int shaderSet = 0;
    auto shaders = lvlInstance::GetGeomTableEntry(geomSet)->pShaders[shaderSet];

    //
    Matrix34 shadowMatrix;
    Matrix34 dummyMatrix;

    if (lvlInstance::ComputeShadowMatrix(&shadowMatrix, inst->GetRoomId(), &inst->GetMatrix(&dummyMatrix)))
    {
        //setup renderer
        gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;
        Matrix44::Convert(gfxRenderState::sm_World, shadowMatrix);

        //draw shadow
        modStatic* shadow = lvlInstance::GetGeomTableEntry(geomSet + 1)->GetHighestLOD();
        if (shadow != nullptr)
        {
            shadow->Draw(shaders);
        }
    }
}

void vehCableCarInstanceHandler::DrawGlow()
{
    //We only draw a headlight glow here. Bail immediately if we can
    if (!aiMap::Instance->drawHeadlights)
        return;

    //get vars
    auto inst = reinterpret_cast<lvlInstance*>(this);
    int geomSet = inst->GetGeomIndex() - 1;

    //setup renderer
    gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;
    Matrix34 instMtx = inst->GetMatrix(&cableCarMatrix);
    memcpy(&cableCarMatrix, &instMtx, sizeof(Matrix34));

    Matrix44::Convert(gfxRenderState::sm_World, cableCarMatrix);

    //get our shader set
    int shaderSet = 0;
    auto shaders = lvlInstance::GetGeomTableEntry(geomSet)->pShaders[shaderSet];

    //get lights
    modStatic* hlight = lvlInstance::GetGeomTableEntry(geomSet + 2)->GetHighestLOD();
    if (hlight != nullptr)
    {
        hlight->Draw(shaders);
    }
}

bool vehCableCarInstanceHandler::BeginGeom(const char* a1, const char* a2, int a3)
{
    //We hook this to set flag 64 (shadow)
    auto inst = reinterpret_cast<lvlInstance*>(this);
    inst->SetFlag(64);

    //Call original
    return inst->BeginGeom(a1, a2, a3);
}

void vehCableCarInstanceHandler::Install()
{
    InstallVTableHook("vehCableCarInstanceHandler::DrawGlow",
        &DrawGlow, {
            0x5B5690,
        }
    );

    InstallVTableHook("vehCableCarInstanceHandler::DrawShadow",
        &DrawShadow, {
            0x5B5688,
        }
    );

    InstallCallback("aiCableCar::Init", "Hook BeginGeom to set instance shadowing flag.",
        &BeginGeom, {
            cb::call(0x53F8AA),
        }
    );
}

/*
    mmArrowHandler
*/

static ConfigValue<bool> cfgHudArrowStyles("EnableHudArrowStyles", true);

void mmArrowHandler::SetShape(LPCSTR modelName, LPCSTR dirName, bool useLVertex, Vector3* a4) {
    auto form = reinterpret_cast<asMeshSetForm*>(this);

    if (dgStatePack::Instance->GameMode == Blitz)
        form->SetShape("hudarrow_blitz01", "geometry", 0);
    else if (dgStatePack::Instance->GameMode == CrashCourse)
        form->SetShape("hudarrow_cc01", "geometry", 0);
    else
        form->SetShape("hudarrow01", "geometry", 0);
}

void mmArrowHandler::Update() {
    if (MMSTATE->GameMode == Cruise)
        return;

    //call original
    hook::Thunk<0x42E7F0>::Call<void>(this);
}

void mmArrowHandler::Install()
{
    if (cfgHudArrowStyles.Get()) {
        InstallCallback("mmArrow::mmArrow", "Enables the unused hud arrows for blitz and crash course game modes.",
            &SetShape, {
                cb::call(0x42E6BA),
            }
        );
    }

    InstallVTableHook("mmArrow::Update",
        &Update, {
            0x5B0CB4,
        }
    );
}

/*
    vehSirenHandler
*/

vehSiren* vehSirenHandler::OperatorNew(uint size) {
    return hook::StaticThunk<0x577360>::Call<vehSiren*>(sizeof(vehSiren));
}

void vehSirenHandler::Reset() {
    auto siren = reinterpret_cast<vehSiren*>(this);
    siren->setActive(false);
}

void vehSirenHandler::Update() {
    auto siren = reinterpret_cast<vehSiren*>(this);

    float rotationAmount = vehSiren::SirenRotationSpeed;

    if (siren->getLight(0) != nullptr && siren->getActive())
    {
        for (int i = 0; i < siren->getLightCount(); i++)
        {
            siren->getLight(i)->Direction.RotateY(datTimeManager::Seconds * siren->getRotationRate() * rotationAmount);
        }
    }
}

void vehSirenHandler::Install() {
    InstallCallback("vehCar::Init", "Use our new size of vehSiren on vehicle initialization.",
        &OperatorNew, {
            cb::call(0x42BE30),
        }
    );

    InstallCallback("vehSiren::Reset", "Deactivates siren lights upon reset.",
        &Reset, {
            cb::call(0x42C3A6),
        }
    );

    InstallCallback("vehSiren::Update", "Use our vehSiren update.",
        &Update, {
            cb::call(0x42C920),
        }
    );

    //increase ltLightPool size to hold 24 siren lights
    InstallPatch({ 0x68, 0xB4, 0xA, 0x0, 0x0 }, {
        0x4D66A4,
    });

    //count to 24
    InstallPatch({ 0x6A, 0x18 }, {
        0x4D66CC,
    });

    //set ltLight count to 24
    InstallPatch({ 0xC7, 0x0, 0x18, 0x0, 0x0, 0x0 }, {
        0x4D66D1,
    });

    ConfigValue<float> cfgSirenRotationSpeed("SirenRotationSpeed", 3.1415927f);
    ConfigValue<float> cfgSirenRotationStyle("SirenRotationStyle", 1.5707964f);

    vehSiren::SirenRotationSpeed = cfgSirenRotationSpeed.Get();
    vehSiren::SirenRotationStyle = cfgSirenRotationStyle.Get();
}

/*
    aiPoliceOfficerFeatureHandler
*/

aiVehicle* findVehicle(vehCar *car) {
    auto AIMAP = &aiMap::Instance;

    // check players
    for (int i = 0; i < 4; i++) {
        auto player = &AIMAP->players[i];

        if (player->getCar() == car)
            return player;
    }

    // check opponents
    for (int i = 0; i < AIMAP->numOpponents; i++) {
        auto opponent = AIMAP->opponents[i];

        if (opponent->getCar() == car)
            return opponent;
    }

    // not found :(
    return nullptr;
}

float getSpeedLimit(vehCar *car) {
    auto AIMAP = &aiMap::Instance;
    auto veh = findVehicle(car);

    if (veh != nullptr) {
        auto roadId = veh->CurrentRoadId();
        auto path = AIMAP->paths[roadId];

        return path->getSpeedLimit();
    }

    return aiPoliceOfficer::DefaultSpeedLimit;
}

BOOL aiPoliceOfficerFeatureHandler::TireScreeching(vehCar *car) {
    auto audio = car->getAudio();
    char* vehName = car->getCarDamage()->GetName();

    if (audio->IsSemiOrBus(vehName)) {
        auto semiAudio = audio->GetSemiCarAudioPtr();
        auto skidSound = semiAudio->getSurfaceAudio()->getSurfaceDatas()->getSkidWaveData()->getSkidSound();

        if (skidSound->IsPlaying())
            aiPoliceOfficer::SkidPlayTime += datTimeManager::Seconds;
        else
            aiPoliceOfficer::SkidPlayTime = 0.f;

        if (aiPoliceOfficer::SkidPlayTime > 4.f)
            return TRUE;
    }
    else {
        auto carAudio = audio->GetCarAudioPtr();
        auto skidSound = carAudio->getSurfaceAudio()->getSurfaceDatas()->getSkidWaveData()->getSkidSound();

        if (skidSound->IsPlaying())
            aiPoliceOfficer::SkidPlayTime += datTimeManager::Seconds;
        else
            aiPoliceOfficer::SkidPlayTime = 0.f;

        if (aiPoliceOfficer::SkidPlayTime > 4.f)
            return TRUE;
    }

    return FALSE;
}

BOOL aiPoliceOfficerFeatureHandler::HornSpamming(vehCar *car) {
    auto audio = car->getAudio();
    char* vehName = car->getCarDamage()->GetName();

    if (audio->IsSemiOrBus(vehName)) {
        auto semiAudio = audio->GetSemiCarAudioPtr();
        auto hornSound = semiAudio->getHornSound();

        if (hornSound->IsPlaying())
            aiPoliceOfficer::HornPlayTime += datTimeManager::Seconds;
        else
            aiPoliceOfficer::HornPlayTime = 0.f;

        if (aiPoliceOfficer::HornPlayTime > 3.f)
            return TRUE;
    }
    else {
        auto carAudio = audio->GetCarAudioPtr();
        auto hornSound = carAudio->getHornSound();

        if (hornSound->IsPlaying())
            aiPoliceOfficer::HornPlayTime += datTimeManager::Seconds;
        else
            aiPoliceOfficer::HornPlayTime = 0.f;

        if (aiPoliceOfficer::HornPlayTime > 3.f)
            return TRUE;
    }

    return FALSE;
}

BOOL aiPoliceOfficerFeatureHandler::Offroad(vehCar *car) {
    auto AIMAP = &aiMap::Instance;

    auto position = car->getModel()->GetPosition();
    auto roomId = car->getModel()->GetRoomId();

    short outId, outType;
    float outDist;

    AIMAP->MapComponent(position, &outId, &outType, roomId);

    aiMapComponentType component = (aiMapComponentType)outType;

    if (component == aiMapComponentType::Road)
    {
        auto path = AIMAP->paths[outId];

        if (path->IsPosOnRoad(&position, 0.f, &outDist) == 2) // 2 = Sidewalk
            return TRUE;

        // This case is for roads with grass
        for (int i = 0; i < 4; i++)
        {
            auto wheel = car->getCarSim()->getWheel(i);

            if (!strcmp(wheel->getCurrentPhysicsMaterial()->getName(), "grass"))
                return TRUE;
        }
    }
    if (component == aiMapComponentType::None || component == aiMapComponentType::Shortcut)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL aiPoliceOfficerFeatureHandler::IsPlayerDrivingMadly(vehCar *perpCar) {
    auto police = reinterpret_cast<aiPoliceOfficer*>(this);

    if (!police->IsPerpACop(perpCar)) {
        if (vehPoliceCarAudio::iNumCopsPursuingPlayer < aiPoliceOfficer::MaximumNumCops || aiPoliceOfficer::MaximumNumCops <= 0) {
            if (police->Fov(perpCar))
            {
                float speed = perpCar->getCarSim()->getSpeedMPH();
                float speedLimit = getSpeedLimit(perpCar) * 2.857142857142857f;

                if (speed > (speedLimit * aiPoliceOfficer::SpeedLimitTolerance)) {
                    LogFile::Printf(1, "PERP DETECTED!!! He's doing %.4f over the speed limit (~%.4fmph)!\n", (speed - speedLimit), speedLimit);
                    return TRUE;
                }
                if (police->Collision(perpCar)) {
                    LogFile::Printf(1, "PERP IS DOING DAMAGE TO PROPERTY!");
                    return TRUE;
                }
                if (aiPoliceOfficerFeatureHandler::Offroad(perpCar)) {
                    LogFile::Printf(1, "PERP IS GOING OFFROAD!");
                    return TRUE;
                }
            }
            if (police->HitMe(perpCar)) {
                LogFile::Printf(1, "OFFICER INVOLVED COLLISION WITH PERP!");
                return TRUE;
            }
            if (aiPoliceOfficerFeatureHandler::TireScreeching(perpCar)) {
                LogFile::Printf(1, "PERP IS SCREECHING THEIR TIRES!");
                return TRUE;
            }
            if (aiPoliceOfficerFeatureHandler::HornSpamming(perpCar)) {
                LogFile::Printf(1, "PERP IS SPAMMING HORN!");
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL aiPoliceOfficerFeatureHandler::IsOppDrivingMadly(vehCar *perpCar) {
    auto police = reinterpret_cast<aiPoliceOfficer*>(this);

    if (police->Fov(perpCar))
    {
        float speed = perpCar->getCarSim()->getSpeedMPH();
        float speedLimit = getSpeedLimit(perpCar) * 2.857142857142857f;

        if (speed > (speedLimit * aiPoliceOfficer::SpeedLimitTolerance)) {
            return TRUE;
        }
        if (police->Collision(perpCar)) {
            return TRUE;
        }
        if (aiPoliceOfficerFeatureHandler::Offroad(perpCar)) {
            return TRUE;
        }
    }
    if (police->HitMe(perpCar)) {
        return TRUE;
    }

    return FALSE;
}

void aiPoliceOfficerFeatureHandler::PerpEscapes(bool a1) {
    auto police = reinterpret_cast<aiPoliceOfficer*>(this);
    auto copCar = police->getCar();
    auto audio = copCar->getAudio();
    auto policeAudio = audio->GetPoliceCarAudioPtr();
    auto AIMAP = &aiMap::Instance;
    auto perpCar = police->getFollowedCar();

    police->StopSiren();

    if (policeAudio != nullptr && a1)
        policeAudio->PlayExplosion();

    AIMAP->policeForce->UnRegisterCop(copCar, perpCar);
    police->setPoliceState(0);
    police->setState(3);
}

void aiPoliceOfficerFeatureHandler::DetectPerpetrator() {
    auto police = reinterpret_cast<aiPoliceOfficer*>(this);
    auto copCar = police->getCar();
    auto AIMAP = &aiMap::Instance;
    auto player = AIMAP->lastPlayer;

    if (police->getPoliceState() != police->getLastPoliceState())
        police->setLastPoliceState(police->getPoliceState());

    if (player != nullptr)
    {
        float posDiffX = copCar->getModel()->GetPosition().X - player->getCar()->getModel()->GetPosition().X;
        float posDiffY = copCar->getModel()->GetPosition().Y - player->getCar()->getModel()->GetPosition().Y;
        float posDiffZ = copCar->getModel()->GetPosition().Z - player->getCar()->getModel()->GetPosition().Z;

        float dist = posDiffX * posDiffX + posDiffY * posDiffY + posDiffZ * posDiffZ;

        if (dist < 5625.f && bustedTimer <= bustedTimeout)
        {
            if (IsPlayerDrivingMadly(player->getCar()) && AIMAP->policeForce->RegisterPerp(copCar, player->getCar()) >= 0)
            {
                police->setFollowedCar(player->getCar());
                police->setState(0);
                police->setPoliceState(2);

                police->setFollowCarDistance(sqrt(posDiffX * posDiffX + posDiffY * posDiffY + posDiffZ * posDiffZ));

                short outId = police->getComponentId();
                short outType = police->getComponentType();

                AIMAP->MapComponent(player->getCar()->getModel()->GetPosition(), &outId, &outType, -1);
                police->FollowPerpetrator();
                return;
            }
        }
    }

    for (int i = 0; i < AIMAP->numOpponents; i++)
    {
        auto opponent = AIMAP->Opponent(i);

        float posDiffX = copCar->getModel()->GetPosition().X - opponent->getCar()->getModel()->GetPosition().X;
        float posDiffY = copCar->getModel()->GetPosition().Y - opponent->getCar()->getModel()->GetPosition().Y;
        float posDiffZ = copCar->getModel()->GetPosition().Z - opponent->getCar()->getModel()->GetPosition().Z;

        float dist = posDiffX * posDiffX + posDiffY * posDiffY + posDiffZ * posDiffZ;

        if (dist < 5625.f && opponent->getState() != 3)
        {
            if (IsOppDrivingMadly(opponent->getCar()) && AIMAP->policeForce->RegisterPerp(copCar, opponent->getCar()) >= 0)
            {
                police->setFollowedCar(opponent->getCar());
                police->setState(0);
                police->setPoliceState(2);

                police->setFollowCarDistance(sqrt(posDiffX * posDiffX + posDiffY * posDiffY + posDiffZ * posDiffZ));

                short outId = police->getComponentId();
                short outType = police->getComponentType();

                AIMAP->MapComponent(opponent->getCar()->getModel()->GetPosition(), &outId, &outType, -1);
                police->FollowPerpetrator();
                return;
            }
        }
    }
}

void aiPoliceOfficerFeatureHandler::Update() {
    auto police = reinterpret_cast<aiPoliceOfficer*>(this);
    auto vehPhysics = police->getVehiclePhysics();
    auto copCar = vehPhysics->getCar();
    auto trailer = copCar->getTrailer();
    auto carsim = copCar->getCarSim();
    auto level = *lvlLevel::Singleton;
    auto AIMAP = &aiMap::Instance;
    auto physMgr = *dgPhysManager::Instance;
    auto player = AIMAP->lastPlayer;
    auto perpCar = police->getFollowedCar();
    float curDist = 9999999.f;

    if (!police->Destroyed())
    {
        if (police->InPersuit())
        {
            short outId = police->getComponentId();
            short outType = police->getComponentType();

            AIMAP->MapComponent(
                perpCar->getModel()->GetPosition(),
                &outId,
                &outType,
                perpCar->getModel()->GetRoomId());

            float posDiffX = copCar->getModel()->GetPosition().X - perpCar->getModel()->GetPosition().X;
            float posDiffZ = copCar->getModel()->GetPosition().Z - perpCar->getModel()->GetPosition().Z;

            float dist = sqrt(posDiffX * posDiffX + posDiffZ * posDiffZ);

            police->setFollowCarDistance(dist);

            police->setPoliceState(AIMAP->policeForce->State(copCar, perpCar, dist));

            if (AIMAP->Player(0)->getCar()->getCarSim()->getTransmission()->getGear() == 0
                || police->getState() == 1
                || police->getFlagCount() == 0
                || perpCar->getCarSim()->getSpeed() < 10.f)
            {
                police->setPoliceState(2);
            }

            if (police->getPoliceState() == 2)
                police->FollowPerpetrator();
            else
                police->ApprehendPerpetrator();

            if (!nfsmwStyleEscape || !bustedTarget || !perpCar->IsPlayer()) {
                // if cop is far away from perp
                if (AIMAP->raceData->getCopChaseDistance() < police->getFollowCarDistance())
                {
                    PerpEscapes(false);
                    return;
                }
            }
        }
        else {
            DetectPerpetrator();
        }

        if (aiPoliceOfficer::FlyingCopFix)
        {
            if (police->getVehiclePhysics()->getThrottle() == 1.f && carsim->getSpeed() < 50.f && !playerInCooldown)
            {
                carsim->getICS()->SetVelocity(carsim->getICS()->GetVelocity() * 1.01f);
            }
        }
        else {
            if (police->getVehiclePhysics()->getThrottle() == 1.f && carsim->getSpeed() < 50.f)
            {
                carsim->getICS()->SetScaledVelocity(carsim->getICS()->GetScaledVelocity() * 1.03f);
            }
        }

        // if cop has fallen into water
        if (level->GetRoomInfo(copCar->getModel()->GetRoomId())->Flags & Water) {
            if (level->GetWaterLevel(copCar->getModel()->GetRoomId()) > copCar->getModel()->GetPosition().Y) {
                PerpEscapes(false);
                police->setPoliceState(12);
            }
        }
    }

    if (police->getPoliceState() != 1 || police->getApprehendState() != 7)
        vehPhysics->DriveRoute(0);
    else
        vehPhysics->Mirror(perpCar);

    // if cop is destroyed
    if (copCar->getCarDamage()->getCurDamage() > copCar->getCarDamage()->getMaxDamage())
    {
        PerpEscapes(true);
        police->setPoliceState(12);
    }

    if (!police->InPersuit())
        police->setState(3);

    if (playerInCooldown && vehPhysics->getThrottle() == 1.f && carsim->getSpeed() > 5.f)
        carsim->getEngine()->setThrottleInput(0.6f);

    if (player != nullptr)
    {
        float posDiffX = copCar->getModel()->GetPosition().X - player->getCar()->getModel()->GetPosition().X;
        float posDiffY = copCar->getModel()->GetPosition().Y - player->getCar()->getModel()->GetPosition().Y;
        float posDiffZ = copCar->getModel()->GetPosition().Z - player->getCar()->getModel()->GetPosition().Z;

        float dist = posDiffX * posDiffX + posDiffY * posDiffY + posDiffZ * posDiffZ;

        if (dist < curDist)
            curDist = dist;
    }

    if (curDist < 40000.f)
    {
        physMgr->DeclareMover(copCar->getModel(), 2, 0x1B);

        if (trailer != nullptr)
            physMgr->DeclareMover(trailer->getTrailerInstance(), 2, 0x1B);
    }

    if (police->InPersuit() && !police->Destroyed())
    {
        if (physMgr->OpponentOptimization)
        {
            physMgr->DeclareMover(copCar->getModel(), 2, 0x13);

            if (trailer != nullptr)
                physMgr->DeclareMover(trailer->getTrailerInstance(), 2, 0x13);
        }
        else {
            physMgr->DeclareMover(copCar->getModel(), 2, 0x1B);

            if (trailer != nullptr)
                physMgr->DeclareMover(trailer->getTrailerInstance(), 2, 0x1B);
        }
    }

    if (copCar->getModel()->GetPosition().Y < -200.f)
    {
        Warningf("Police Officer #%d, Has fallen through the geometry.", police->getId());
        police->Reset();
    }
}

void aiPoliceOfficerFeatureHandler::Install() {
    ConfigValue<bool> cfgPoliceAcademyFunding("PoliceAcademyFunding", true);
    ConfigValue<float> cfgDefaultSpeedLimit("DefaultSpeedLimit", 12.25f);
    ConfigValue<float> cfgSpeedLimitTolerance("SpeedLimitTolerance", 1.125f);
    ConfigValue<int> cfgMaximumCopsLimit("MaximumCopsLimit", 3);
    ConfigValue<bool> cfgFlyingCopFix("FlyingCopFix", true);

    aiPoliceOfficer::DefaultSpeedLimit = cfgDefaultSpeedLimit.Get();
    aiPoliceOfficer::SpeedLimitTolerance = cfgSpeedLimitTolerance.Get();
    aiPoliceOfficer::MaximumNumCops = cfgMaximumCopsLimit.Get();
    aiPoliceOfficer::FlyingCopFix = cfgFlyingCopFix.Get();

    if (cfgPoliceAcademyFunding.Get()) {
        InstallCallback("aiPoliceOfficer::Update", "Use rewritten aiPoliceOfficer update.",
            &Update, {
                cb::call(0x537265),
            }
        );
    }

    InstallCallback("aiPoliceOfficer::StartSiren", "Activates trailer siren.",
        &aiPoliceOfficer::StartSiren, {
            cb::call(0x53E429),
        }
    );

    InstallCallback("aiPoliceOfficer::StopSiren", "Deactivates trailer siren.",
        &aiPoliceOfficer::StopSiren, {
            cb::call(0x53DB45),
        }
    );
}

/*
    vehEngineHandler
*/

static ConfigValue<bool> cfgPhysicalEngineDamage("PhysicalEngineDamage", true);

float vehEngineHandler::CalcTorque(float a1) {
    auto engine = reinterpret_cast<vehEngine*>(this);

    auto carDamage = reinterpret_cast<vehCarModel*>(engine->getCarSim()->getInstance())->getCar()->getCarDamage();
    float damagePercent = (carDamage->getCurDamage() - carDamage->getMedDamage()) / (carDamage->getMaxDamage() - carDamage->getMedDamage());

    //clamp from 0-70%
    damagePercent = fmaxf(0.f, fminf(damagePercent, 0.7f));
    float invDamagePercent = 1.f - damagePercent;

    float torque = hook::Thunk<0x4D8ED0>::Call<float>(this, a1);
    return torque * invDamagePercent;
}

void vehEngineHandler::Install(void) {
    if (!cfgPhysicalEngineDamage.Get())
        return;

    InstallCallback("vehEngine::CalcTorque", "Overwrite torque calculation for engine damage",
        &CalcTorque, {
            cb::call(0x4D8F65),
        }
    );
}

/*
    mmExternalViewHandler
*/

gfxBitmap* PursuitBar;
gfxBitmap* BustedMeter;
gfxBitmap* BustedIcon;
gfxBitmap* EscapeMeter;
gfxBitmap* EscapeIcon;
gfxBitmap* BarMask;
gfxBitmap* CooldownBar;
gfxBitmap* CooldownIcon;
gfxBitmap* CooldownMeter;

void mmExternalViewHandler::Init(mmPlayer *player) {
    auto externalView = reinterpret_cast<mmExternalView*>(this);
    externalView->Init(player);

    PursuitBar = gfxGetBitmap("pursuit_bar", 0, 0);
    BustedMeter = gfxGetBitmap("busted_meter", 0, 0);
    BustedIcon = gfxGetBitmap("busted_icon", 0, 0);
    EscapeMeter = gfxGetBitmap("escape_meter", 0, 0);
    EscapeIcon = gfxGetBitmap("escape_icon", 0, 0);
    BarMask = gfxGetBitmap("bar_mask", 0, 0);
    CooldownBar = gfxGetBitmap("cooldown_bar", 0, 0);
    CooldownIcon = gfxGetBitmap("cooldown_icon", 0, 0);
    CooldownMeter = gfxGetBitmap("cooldown_meter", 0, 0);

    if (mmExternalView::EnableMM1StylePursuitBar) {
        PursuitBar = gfxGetBitmap("pursuit_bar_mm1", 0, 0);
        BustedMeter = gfxGetBitmap("busted_ticks", 0, 0);
        EscapeMeter = gfxGetBitmap("escape_ticks", 0, 0);
        CooldownBar = gfxGetBitmap("cooldown_bar_mm1", 0, 0);
        CooldownMeter = gfxGetBitmap("cooldown_ticks", 0, 0);
    }
}

void mmExternalViewHandler::ResChange(int width, int height) {
    auto externalView = reinterpret_cast<mmExternalView*>(this);
    auto linearGauge = externalView->getLinearGauge();
    auto slidingGauge = externalView->getSlidingGauge();
    auto gearIndicator = externalView->getGearIndicator();
    auto speedIndicator = externalView->getSpeedIndicator();

    auto player = externalView->getPlayer();
    auto carsim = externalView->getCarSim();

    int window_width = window_iWidth < 640;

    externalView->setDestX(0);
    externalView->setDestY(window_iHeight - (100 >> window_width));

    if (mmExternalView::EnableMM1StyleHud) {
        linearGauge->setDestX(8 >> window_width);
        linearGauge->setDestY(74 >> window_width);
    }
    else {
        linearGauge->setDestX(8 >> window_width);
        linearGauge->setDestY(41 >> window_width);
    }

    char* speedBitmapName = "speed_ticks_half";
    if (!window_width)
        speedBitmapName = "speed_ticks";

    linearGauge->Init(
        speedBitmapName,
        &*getPtr<float>(carsim, 0x2C4),
        &*getPtr<float>(carsim, 0x280),
        1, // 1 = Width, 0 = Height
        externalView);

    if (mmExternalView::EnableMM1StyleHud) {
        slidingGauge->setDestX(72 >> window_width);
        slidingGauge->setDestY(1 >> window_width);
    }
    else {
        slidingGauge->setDestX(8 >> window_width);
        slidingGauge->setDestY(88 >> window_width);
    }

    char* damageBitmapName = "damage_half";
    if (!window_width)
        damageBitmapName = "damage";

    slidingGauge->Init(
        damageBitmapName,
        &*getPtr<float>(*getPtr<vehCarDamage*>(player, 0xEC), 0x30),
        &*getPtr<float>(*getPtr<vehCarDamage*>(player, 0xEC), 0x34),
        1, // 1 = Width, 0 = Height
        externalView,
        linearGauge->getBitmapGauge()->Width);

    if (mmExternalView::EnableMM1StyleHud) {
        char* labelBitmapName = "speed_half";
        if (!window_width)
            labelBitmapName = "speed";

        slidingGauge->InitOverlay(labelBitmapName);
    }
    else {
        char* labelBitmapName = "damage_lable_half";
        if (!window_width)
            labelBitmapName = "damage_lable";

        slidingGauge->InitOverlay(labelBitmapName);
    }

    if (mmExternalView::EnableMM1StyleHud) {
        gearIndicator->setDestX(136 >> window_width);
        gearIndicator->setDestY(48 >> window_width);
    }
    else {
        gearIndicator->setDestX(16 >> window_width);
        gearIndicator->setDestY(46 >> window_width);
    }

    gearIndicator->Init(externalView, player);

    if (mmExternalView::EnableMM1StyleHud) {
        speedIndicator->setDestX(19 >> window_width);
        speedIndicator->setDestY(31 >> window_width);
    }
    else {
        speedIndicator->setDestX(19 >> window_width);
        speedIndicator->setDestY(-14 >> window_width);
    }

    speedIndicator->Init(externalView, carsim);

    //call asNode::ResChange
    hook::Thunk<0x4A0E30>::Call<void>(this, width, height);
}

void mmExternalViewHandler::Cull() {
    auto externalView = reinterpret_cast<mmExternalView*>(this);
    auto player = externalView->getPlayer();
    auto linearGauge = externalView->getLinearGauge();
    auto slidingGauge = externalView->getSlidingGauge();
    auto gearIndicator = externalView->getGearIndicator();
    auto speedIndicator = externalView->getSpeedIndicator();

    int window_width = window_iWidth;
    int window_height = window_iHeight;

    if (player->getCamView()->getUnk_18()) {
        if (MMSTATE->MapMode != 3 && MMSTATE->MapMode != 2) {
            int height = gfxCurrentViewport->getViewport()->dwY + gfxCurrentViewport->getViewport()->dwHeight;
            gfxPipeline::ClearRect(0, height, (int)(window_width * 0.3f), window_height - height, 0xFF000000);
        }
    }

    if (MMSTATE->InputDevice == 0 || mmExternalView::EnableMouseBar) {
        auto mouseBar = externalView->getMouseBar();
        auto mouseAr = externalView->getMouseAr();

        externalView->setField_40((mouseBar->Width >> 1) - 15);
        externalView->setField_44(mouseAr->Width >> 1);

        externalView->setField_30((window_width - mouseBar->Width) >> 1);
        externalView->setField_34(window_height - 2 * mouseBar->Height);

        externalView->setField_38((window_width >> 1) + (int)(externalView->getField_40() * *getPtr<float>(player, 0x2264)) - externalView->getField_44());
        externalView->setField_3c(externalView->getField_34() - 16);

        gfxPipeline::CopyBitmap(externalView->getField_38(), externalView->getField_3c(), mouseAr, 0, 0, mouseAr->Width, mouseAr->Height, true);
        gfxPipeline::CopyBitmap(externalView->getField_30(), externalView->getField_34(), mouseBar, 0, 0, mouseBar->Width, mouseBar->Height, true);
    }

    DrawSlidingGauge();
    DrawGearIndicator();
    DrawLinearGauge();
    DrawSpeedIndicator();

    if (bustedTarget == 1 || bustedTarget >= 3) {
        if (playerInPursuit && cooldownTimer == 0.f)
            DrawPursuitBar();
        if (playerInPursuit && playerInCooldown && cooldownTimer > 0.f)
            DrawCooldownBar();
    }
}

void mmExternalViewHandler::DrawSlidingGauge() {
    auto externalView = reinterpret_cast<mmExternalView*>(this);
    auto slidingGauge = externalView->getSlidingGauge();
    auto bitmapGauge = slidingGauge->getBitmapGauge();
    auto bitmapLabel = slidingGauge->getBitmapLabel();

    if (mmExternalView::EnableMM1StyleHud) {
        int window_width = window_iWidth < 640;

        auto carDamage = externalView->getPlayer()->getCar()->getCarDamage();

        float damagePercent = (carDamage->getCurDamage() - carDamage->getMedDamage()) / (carDamage->getMaxDamage() - carDamage->getMedDamage());

        bool elapsedTime = fmod(datTimeManager::ElapsedTime, 1.f) > 0.5f;

        if (bitmapLabel != nullptr)
            gfxPipeline::CopyBitmap(
                externalView->getDestX(),
                externalView->getDestY() - (11 >> window_width),
                bitmapLabel,
                0,
                0,
                bitmapLabel->Width,
                bitmapLabel->Height,
                true);

        if (damagePercent > 0.75f && elapsedTime)
            gfxPipeline::CopyBitmap(
                slidingGauge->getDestX() + externalView->getDestX(),
                slidingGauge->getDestY() + externalView->getDestY(),
                bitmapGauge,
                0,
                0,
                bitmapGauge->Width,
                bitmapGauge->Height,
                true);
    }
    else {
        int dimensionType = slidingGauge->getDimensionType();
        int bitmapDimension = slidingGauge->getBitmapDimension();
        int dimension = 0;

        if (dimensionType)
            dimension = bitmapGauge->Width;
        else
            dimension = bitmapGauge->Height;

        float damage = (*slidingGauge->getMinValue() / *slidingGauge->getMaxValue()) * (dimension - bitmapDimension);

        int actualValue = (int)damage;

        int result = (actualValue < (dimension - bitmapDimension)) ? actualValue : (dimension - bitmapDimension);

        if (dimensionType)
            gfxPipeline::CopyBitmap(
                slidingGauge->getDestX() + externalView->getDestX(),
                slidingGauge->getDestY() + externalView->getDestY(),
                bitmapGauge,
                result,
                0,
                bitmapDimension,
                bitmapGauge->Height,
                true);
        else
            gfxPipeline::CopyBitmap(
                slidingGauge->getDestX() + externalView->getDestX(),
                slidingGauge->getDestY() + externalView->getDestY(),
                bitmapGauge,
                0,
                result,
                bitmapGauge->Width,
                bitmapDimension,
                true);

        if (bitmapLabel != nullptr)
            gfxPipeline::CopyBitmap(
                slidingGauge->getDestX() + externalView->getDestX(),
                slidingGauge->getDestY() + externalView->getDestY(),
                bitmapLabel,
                0,
                0,
                bitmapLabel->Width,
                bitmapLabel->Height,
                true);
    }
}

void mmExternalViewHandler::DrawGearIndicator() {
    auto externalView = reinterpret_cast<mmExternalView*>(this);
    auto gearIndicator = externalView->getGearIndicator();
    auto player = gearIndicator->getPlayer();
    auto carsim = player->getCar()->getCarSim();
    auto transmission = carsim->getTransmission();
    float speedMPH = carsim->getSpeedMPH();
    float throttle = carsim->getEngine()->getThrottleInput();
    int gear = transmission->getGear();

    if (transmission->IsAuto()) {
        if (gear == 0) {
            gear = 9;  // R
        }
        else if (throttle <= 0.f && speedMPH < 1.f) {
            gear = 10; // P
        }
        else if (gear == 1) {
            gear = 0;  // N
        }
        else {
            gear = 11; // D
        }
    }
    else {
        if (gear == 0) {
            gear = 9;  // R
        }
        else if (gear == 1) {
            gear = 0;  // N
        }
        else {
            gear -= 1; // R N 1 2 3 4 5 6
        }
    }

    gfxPipeline::CopyBitmap(
        gearIndicator->getDestX() + gearIndicator->getExternalView()->getDestX(),
        gearIndicator->getDestY() + gearIndicator->getExternalView()->getDestY(),
        gearIndicator->getBitmapGears(gear),
        0,
        0,
        gearIndicator->getBitmapGears(gear)->Width,
        gearIndicator->getBitmapGears(gear)->Height,
        true);
}

void mmExternalViewHandler::DrawLinearGauge() {
    auto externalView = reinterpret_cast<mmExternalView*>(this);
    auto linearGauge = externalView->getLinearGauge();
    auto bitmapGauge = linearGauge->getBitmapGauge();
    auto bitmapLabel = linearGauge->getBitmapLabel();
    int dimensionType = linearGauge->getDimensionType();
    int dimension = 0;

    if (dimensionType)
        dimension = bitmapGauge->Width;
    else
        dimension = bitmapGauge->Height;

    float RPM = (*linearGauge->getMinValue() / *linearGauge->getMaxValue()) * dimension;

    int actualValue = (int)RPM;

    int result = (actualValue < dimension) ? actualValue : dimension;

    if (result) {
        if (dimensionType)
            gfxPipeline::CopyBitmap(
                linearGauge->getDestX() + externalView->getDestX(),
                linearGauge->getDestY() + externalView->getDestY(),
                bitmapGauge,
                0,
                0,
                result,
                bitmapGauge->Height,
                true);
        else
            gfxPipeline::CopyBitmap(
                linearGauge->getDestX() + externalView->getDestX(),
                linearGauge->getDestY() + externalView->getDestY(),
                bitmapGauge,
                0,
                0,
                bitmapGauge->Width,
                result,
                true);
    }

    if (bitmapLabel != nullptr)
        gfxPipeline::CopyBitmap(
            linearGauge->getDestX() + externalView->getDestX(),
            linearGauge->getDestY() + externalView->getDestY(),
            bitmapLabel,
            0,
            0,
            bitmapLabel->Width,
            bitmapLabel->Height,
            true);
}

void mmExternalViewHandler::DrawSpeedIndicator() {
    auto externalView = reinterpret_cast<mmExternalView*>(this);
    auto speedIndicator = externalView->getSpeedIndicator();
    auto carsim = speedIndicator->getCarSim();
    float speedMPH = carsim->getSpeedMPH();

    if (mmExternalView::SwitchFromMPH2KPH)
        speedMPH *= 1.609344f;

    int actualValue = (int)speedMPH;

    int result = (actualValue < 1000) ? actualValue : 999;

    int resultA = result / 100;
    int resultB = result % 100 / 10;
    int resultC = result % 10;

    if (resultA) {
        if (resultA < 10) {
            gfxPipeline::CopyBitmap(
                speedIndicator->getDestX() + externalView->getDestX(),
                speedIndicator->getDestY() + externalView->getDestY(),
                speedIndicator->getBitmapSpeeds(resultA),
                0,
                0,
                speedIndicator->getBitmapSpeeds(resultA)->Width,
                speedIndicator->getBitmapSpeeds(resultA)->Height,
                true);
        }
    }

    if (resultB || resultA) {
        gfxPipeline::CopyBitmap(
            speedIndicator->getDestX() + externalView->getDestX() + speedIndicator->getBitmapSpeeds(resultB)->Width + 1,
            speedIndicator->getDestY() + externalView->getDestY(),
            speedIndicator->getBitmapSpeeds(resultB),
            0,
            0,
            speedIndicator->getBitmapSpeeds(resultB)->Width,
            speedIndicator->getBitmapSpeeds(resultB)->Height,
            true);
    }

    gfxPipeline::CopyBitmap(
        speedIndicator->getDestX() + externalView->getDestX() + 2 * speedIndicator->getBitmapSpeeds(resultC)->Width + 1,
        speedIndicator->getDestY() + externalView->getDestY(),
        speedIndicator->getBitmapSpeeds(resultC),
        0,
        0,
        speedIndicator->getBitmapSpeeds(resultC)->Width,
        speedIndicator->getBitmapSpeeds(resultC)->Height,
        true);
}

void mmExternalViewHandler::DrawPursuitBar() {
    auto externalView = reinterpret_cast<mmExternalView*>(this);
    auto player = externalView->getPlayer();
    auto playerPos = player->getCar()->getModel()->GetPosition();
    auto AIMAP = &aiMap::Instance;

    if (PursuitBar == nullptr
        || BustedMeter == nullptr
        || BustedIcon == nullptr
        || EscapeMeter == nullptr
        || EscapeIcon == nullptr
        || BarMask == nullptr)
        return;

    int window_width = window_iWidth;
    int window_height = window_iHeight;

    int closestCopId = 0;

    float escapePercent = 0.f;

    for (int i = 0; i < AIMAP->numCops; i++)
    {
        auto police = AIMAP->Police(i);
        auto police2 = AIMAP->Police(closestCopId);

        float policeDist = police->getVehiclePhysics()->getCar()->getModel()->GetPosition().Dist(playerPos);
        float police2Dist = police2->getVehiclePhysics()->getCar()->getModel()->GetPosition().Dist(playerPos);

        if (police2Dist > policeDist)
            closestCopId = i;

        int pursuitBarDistX = (window_width - PursuitBar->Width) >> 1;

        if (nfsmwStyleEscape)
            escapePercent = EscapeMeter->Width * (escapeTimer / 4.f);

        else {
            if (*getPtr<WORD>(police, 0x977A) != 0 && *getPtr<WORD>(police, 0x977A) != 12 && *getPtr<vehCar*>(police, 0x9774) == player->getCar())
            {
                escapePercent = EscapeMeter->Width * (*getPtr<float>(police, 0x9794) / *getPtr<float>(AIMAP->raceData, 0x98));

                if (police2Dist <= policeDist && *getPtr<WORD>(police2, 0x977A) != 0 && *getPtr<WORD>(police2, 0x977A) != 12 && *getPtr<vehCar*>(police2, 0x9774) == player->getCar())
                {
                    escapePercent = EscapeMeter->Width * (*getPtr<float>(police2, 0x9794) / *getPtr<float>(AIMAP->raceData, 0x98));
                }
            }
        }

        if (mmExternalView::EnableMM1StylePursuitBar)
        {
            int pursuitBarDistY = (window_height - 2 * PursuitBar->Height);

            float bustedPercent = BustedMeter->Width - (BustedMeter->Width * (bustedTimer / bustedTimeout));

            gfxPipeline::CopyBitmap(pursuitBarDistX, pursuitBarDistY, PursuitBar, 0, 0, PursuitBar->Width, PursuitBar->Height, true);

            gfxPipeline::CopyBitmap(pursuitBarDistX + 8, pursuitBarDistY + 16, BustedMeter, 0, 0, BustedMeter->Width, BustedMeter->Height, true);

            gfxPipeline::CopyBitmap(pursuitBarDistX + 8, pursuitBarDistY + 16, BarMask, 0, 0, (int)bustedPercent, BustedMeter->Height, true);

            gfxPipeline::CopyBitmap(pursuitBarDistX + 152, pursuitBarDistY + 16, EscapeMeter, 0, 0, (int)escapePercent, EscapeMeter->Height, true);
        }
        else {
            int pursuitBarDistY = (window_height - 3 * PursuitBar->Height);

            float bustedPercent = BarMask->Width - (BarMask->Width * (bustedTimer / bustedTimeout));

            gfxPipeline::CopyBitmap(pursuitBarDistX, pursuitBarDistY, BarMask, 0, 0, BarMask->Width, BarMask->Height, true);

            gfxPipeline::CopyBitmap(pursuitBarDistX, pursuitBarDistY, BustedMeter, 0, 0, BustedMeter->Width, BustedMeter->Height, true);

            gfxPipeline::CopyBitmap(pursuitBarDistX, pursuitBarDistY, BarMask, 0, 0, (int)(bustedPercent * 0.5f), BarMask->Height, true);

            gfxPipeline::CopyBitmap(pursuitBarDistX + 128, pursuitBarDistY, EscapeMeter, 0, 0, (int)escapePercent, EscapeMeter->Height, true);

            gfxPipeline::CopyBitmap(pursuitBarDistX, pursuitBarDistY, PursuitBar, 0, 0, PursuitBar->Width, PursuitBar->Height, true);

            gfxPipeline::CopyBitmap(pursuitBarDistX - 13, pursuitBarDistY - 32, BustedIcon, 0, 0, BustedIcon->Width, BustedIcon->Height, true);

            gfxPipeline::CopyBitmap(pursuitBarDistX + 236, pursuitBarDistY - 32, EscapeIcon, 0, 0, EscapeIcon->Width, EscapeIcon->Height, true);
        }
    }
}

void mmExternalViewHandler::DrawCooldownBar() {
    if (CooldownBar == nullptr
        || CooldownMeter == nullptr
        || CooldownIcon == nullptr
        || BarMask == nullptr)
        return;

    int window_width = window_iWidth;
    int window_height = window_iHeight;

    int cooldownBarDistX = (window_width - CooldownBar->Width) >> 1;

    float cooldownPercent = CooldownMeter->Width * (cooldownTimer / cooldownTimeout);

    if (mmExternalView::EnableMM1StylePursuitBar)
    {
        int cooldownBarDistY = (window_height - 2 * CooldownBar->Height);

        gfxPipeline::CopyBitmap(cooldownBarDistX, cooldownBarDistY, CooldownBar, 0, 0, CooldownBar->Width, CooldownBar->Height, true);

        gfxPipeline::CopyBitmap(cooldownBarDistX + 8, cooldownBarDistY + 16, CooldownMeter, 0, 0, (int)cooldownPercent, CooldownMeter->Height, true);
    }
    else {
        int cooldownBarDistY = (window_height - 3 * CooldownBar->Height);

        gfxPipeline::CopyBitmap(cooldownBarDistX, cooldownBarDistY, BarMask, 0, 0, BarMask->Width, BarMask->Height, true);

        gfxPipeline::CopyBitmap(cooldownBarDistX, cooldownBarDistY, CooldownMeter, 0, 0, (int)cooldownPercent, CooldownMeter->Height, true);

        gfxPipeline::CopyBitmap(cooldownBarDistX, cooldownBarDistY, CooldownBar, 0, 0, CooldownBar->Width, CooldownBar->Height, true);

        gfxPipeline::CopyBitmap(cooldownBarDistX + 112, cooldownBarDistY - 32, CooldownIcon, 0, 0, CooldownIcon->Width, CooldownIcon->Height, true);
    }
}

void mmExternalViewHandler::Install() {
    InstallCallback("mmExternalView::Init", "Use our external view initialization",
        &Init, {
            cb::call(0x42D617),
        }
    );

    InstallVTableHook("mmExternalView::ResChange",
        &ResChange, {
            0x5B0DC0,
        }
    );

    InstallVTableHook("mmExternalView::Cull",
        &Cull, {
            0x5B0DB4,
        }
    );

    ConfigValue<bool> cfgMm1StyleHud("MM1StyleHud", false);
    ConfigValue<bool> cfgMm1StylePursuitBar("MM1StylePursuitBar", false);
    ConfigValue<bool> cfgEnableMouseBar("EnableMouseBar", false);
    ConfigValue<bool> cfgSwitchFromMPH2KPH("SwitchFromMPH2KPH", false);

    mmExternalView::EnableMM1StyleHud = cfgMm1StyleHud.Get();
    mmExternalView::EnableMM1StylePursuitBar = cfgMm1StylePursuitBar.Get();
    mmExternalView::EnableMouseBar = cfgEnableMouseBar.Get();
    mmExternalView::SwitchFromMPH2KPH = cfgSwitchFromMPH2KPH.Get();
}

/*
    MMDMusicManagerHandler
*/

void MMDMusicManagerHandler::MatchMusicToPlayerSpeed(float speed) {
    auto musicManager = reinterpret_cast<MMDMusicManager*>(this);
    auto musicObject = musicManager->GetDMusicObjectPtr();
    auto v2 = *getPtr<int>(this, 0x2C);
    auto v3 = *getPtr<int>(this, 0x30);
    auto v4 = *getPtr<int>(musicObject, 0x24);

    if (v4 != *getPtr<int>(this, 0x40) && v2 != 0xFFFFFFFF && *getPtr<int>(this, 0x38) != 0xFFFFFFFF)
    {
        if (speed <= 5.f && !playerInPursuit && !*getPtr<byte>(this, 0x50))
        {
            if (*getPtr<float>(this, 0xC) >= 5.f && v4 != v2 && v4 != v3 && v4 != *getPtr<int>(this, 0x34))
            {
                if (v4 == *getPtr<int>(this, 0x24))
                {
                    if (v3 != 0xFFFFFFFF)
                        musicObject->SegmentSwitch(v3, 0, 0x20);
                }
                else {
                    musicObject->SegmentSwitch(v2, 0, 0x20);
                }
            }
            *getPtr<float>(this, 0xC) += datTimeManager::Seconds;
            return;
        }
        if (*getPtr<float>(this, 0xC) == 0.f)
            return;

        if (v4 == v2)
        {
            if (v4 != v3)
            {
                musicObject->SegmentSwitch(*getPtr<int>(this, 0x38), 0, 0x20);
                *getPtr<float>(this, 0xC) = 0.f;
                return;
            }
        }
        else if (v4 != v3)
        {
            *getPtr<float>(this, 0xC) = 0.f;
            return;
        }
        musicObject->SegmentSwitch(*getPtr<int>(this, 0x3C), 0, 0x20);
        *getPtr<float>(this, 0xC) = 0.f;
        return;
    }
}

void MMDMusicManagerHandler::UpdateMusic(float speed, int numCops, bool isAirborne) {
    auto musicManager = reinterpret_cast<MMDMusicManager*>(this);
    auto musicObject = musicManager->GetDMusicObjectPtr();
    int regularMusic = *getPtr<int>(musicManager, 0x38);
    int copMusicIdle = *getPtr<int>(musicManager, 0x30);
    int copMusic = *getPtr<int>(musicManager, 0x24);

    if (nfsmwStyleEscape) {
        MatchMusicToPlayerSpeed(speed);

        if (numCops) {
            if (!*getPtr<int>(this, 0x4C) || !playerInCooldown)
                musicObject->SegmentSwitch(copMusic);
            else if (playerInCooldown)
                musicObject->SegmentSwitch(copMusicIdle, 0, 0x20);
        }
        else if (!numCops && *getPtr<int>(this, 0x4C))
            musicObject->SegmentSwitch(regularMusic);
    }
    else {
        musicManager->MatchMusicToPlayerSpeed(speed);

        if (numCops) {
            if (!*getPtr<int>(this, 0x4C))
                musicObject->SegmentSwitch(copMusic);
        }
        else if (!numCops && *getPtr<int>(this, 0x4C))
            musicObject->SegmentSwitch(regularMusic);
    }

    *getPtr<int>(this, 0x4C) = numCops;

    if (isAirborne)
    {
        if (!*getPtr<byte>(this, 0x51))
        {
            musicObject->PlayMotif(0, 0x880);
            *getPtr<byte>(this, 0x51) = isAirborne;
        }
    }
    else if (*getPtr<byte>(this, 0x51))
    {
        *getPtr<byte>(this, 0x51) = 0;
    }
}

void MMDMusicManagerHandler::Install() {
    InstallCallback("MMDMusicManager::UpdateMusic", "Plays idle cop music in cooldown.",
        &UpdateMusic, {
            cb::call(0x41426E),
        }
    );
}

/*
    MenuManagerHandler
*/

bool canPlayUIDriverSound = true;

void MenuManagerHandler::Update() {
    auto UISound = *getPtr<AudSoundBase*>(this, 0x148);

    if (UISound != nullptr) {
        int currentMenuID = *getPtr<int>(this, 0x138);
        if (currentMenuID == 1 && canPlayUIDriverSound) {
            UISound->SetSoundHandleIndex(0);
            if (!UISound->IsPlaying())
                UISound->PlayOnce(-1.f, -1.f);
            canPlayUIDriverSound = false;
        }
        else if (currentMenuID != 1) {
            canPlayUIDriverSound = true;
        }
    }

    //call original
    hook::Thunk<0x4E5410>::Call<void>(this);
}

void MenuManagerHandler::Install() {
    InstallVTableHook("MenuManager::Update",
        &Update, {
            0x5B3290,
        }
    );
}

/*
    camPovCSHandler
*/

void camPovCSHandler::UpdatePOV() {
    auto camPov = reinterpret_cast<camPovCS*>(this);
    auto matrix = camPov->GetMatrix();
    auto matrix2 = camPov->getMatrix2();
    auto reverseOffset = camPov->getReverseOffset();
    auto camPan = camPov->getCamPan();
    auto position2 = camPov->getPosition2();
    auto car = camPov->getCar();

    float angle = 0.f;
    float angle2 = 0.f;
    float pitch = camPov->getPitch();
    float yaw = camPov->getYaw();

    matrix2->Identity();

    if (camPan != -1)
        reverseOffset = camPov->getOffset();

    position2->X = reverseOffset.X;
    position2->Y = reverseOffset.Y;
    position2->Z = reverseOffset.Z;

    matrix2->Dot(*matrix);

    if (camPan == -1)
        angle = -pitch;
    else
        angle = pitch;

    if (angle != 0.f)
        matrix2->Rotate(*(Vector3*)&matrix2->m00, angle);

    if (camPan == -1)
        angle2 = 3.1415927f;
    else
        angle2 = camPov->getAngle();

    if (yaw != 0.f)
        angle2 = yaw;

    if (angle2 != 0.f)
        matrix2->Rotate(*(Vector3*)&matrix2->m10, angle2);

    if (vehCarModel::WheelWobble && car->getCarSim()->OnGround())
    {
        Vector3 vec;

        float rotation = (fabs(car->getCarSim()->getWheel(1)->getRotationRate()) * datTimeManager::Seconds - 1.5707964f) * 0.31830987f;

        float rotationAmount = fmaxf(0.f, fminf(rotation, 1.f));
        float invRotationAmount = 1.f - rotationAmount;

        vec.X = matrix2->m20 + matrix2->m00;
        vec.Y = matrix2->m21 + matrix2->m01;
        vec.Z = matrix2->m22 + matrix2->m02;

        auto carDamage = car->getCarDamage();
        float damagePercent = (carDamage->getCurDamage() - carDamage->getMedDamage()) / (carDamage->getMaxDamage() - carDamage->getMedDamage());

        damagePercent = fmaxf(0.f, fminf(damagePercent, 1.f));

        float wobbleAngle = sin(car->getCarSim()->getWheel(1)->getAccumulatedRotation()) * damagePercent * invRotationAmount * 0.005f;

        if (!vehCarModel::MM1StyleWobble)
        {
            auto wheelWobbleLimit = car->getCarSim()->getWheel(1)->getWobbleLimit();
            auto wheelWobbleAmount = car->getCarSim()->getWheel(1)->getWobbleAmount();

            wobbleAngle = sin(car->getCarSim()->getWheel(1)->getAccumulatedRotation()) * damagePercent * wheelWobbleLimit * wheelWobbleAmount * 0.05f;
        }

        matrix2->Rotate(vec, wobbleAngle);
    }

    camPov->approachIt();
}

void camPovCSHandler::Install() {
    InstallCallback("camPovCS::UpdatePOV", "Implements wobble cam feature.",
        &UpdatePOV, {
            cb::call(0x51D573),
        }
    );
}

#ifndef FEATURES_DECLARED
#define FEATURES_DECLARED
#endif
