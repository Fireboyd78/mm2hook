#include "bugfix_handlers.h"

using namespace MM2;

static init_handler g_bugfix_handlers[] = {
    CreateHandler<aiPathHandler>("aiPath"),
    CreateHandler<aiPedestrianHandler>("aiPedestrian"),
    CreateHandler<aiPoliceForceHandler>("aiPoliceForce"),

    CreateHandler<audManagerHandler>("audManager"),

    CreateHandler<gfxImageHandler>("gfxImage"),

    CreateHandler<mmBillInstanceHandler>("mmBillInstance"),

    CreateHandler<mmInterfaceHandler>("mmInterface"),
    CreateHandler<mmPopupHandler>("mmPopupHandler"),

    CreateHandler<vehCarHandler>("vehCar"),
    CreateHandler<vehCarAudioContainerBugfixHandler>("vehCarAudio bugfixes"),
    CreateHandler<vehCarModelHandler>("vehCarModel"),
    CreateHandler<mmSpeedIndicatorHandler>("mmSpeedIndicator"),
    CreateHandler<mmHudMapHandler>("mmHudMap"),
    CreateHandler<mmCDPlayerHandler>("mmCDPlayer"),

    CreateHandler<lvlSkyHandler>("lvlSkyHandler"),

    CreateHandler<cityLevelBugfixHandler>("cityLevelBugfixHandler"),

    CreateHandler<mmMultiCRHandler>("mmMultiCR"),

    CreateHandler<BugfixPatchHandler>("Bugfix patches"),

    CreateHandler<pedAnimationInstanceHandler>("pedAnimationInstance"),
};

/*
    aiPathHandler
*/

int numPedUpdateAttempts = 0;

void aiPathHandler::UpdatePedestrians(void) {
    numPedUpdateAttempts = 0;
    $::aiPath::UpdatePedestrians(this);
}

void aiPathHandler::Install() {
    InstallCallback("aiPath::UpdatePedestrians", "Limits the number of update attempts for pedestrians.",
        &UpdatePedestrians, {
            cbHook<CALL>(0x536FE0), // aiMap::Update
        }
    );
}

/*
    aiPedestrianHandler
*/

static ConfigValue<int> cfgMaxPedUpdateAttempts("MaxPedUpdateAttempts", 256);

void aiPedestrianHandler::Update(void) {
    if (numPedUpdateAttempts < cfgMaxPedUpdateAttempts) {
        ++numPedUpdateAttempts;
        $::aiPedestrian::Update(this);
    }
}

void aiPedestrianHandler::Install() {
    InstallCallback("aiPedestrian::Update", "Limits the number of update attempts for a pedestrian.",
        &Update, {
            cbHook<CALL>(0x544191), // aiPath::UpdatePedestrians
        }
    );
}

/*
    aiPoliceForceHandler
*/

static ConfigValue<bool> cfgPoliceAcademyFunding    ("PoliceAcademyFunding",    true);
static ConfigValue<float> cfgDefaultSpeedLimit      ("DefaultSpeedLimit",       12.25f);
static ConfigValue<float> cfgSpeedLimitTolerance    ("SpeedLimitTolerance",     1.125f);

void aiPoliceForceHandler::Reset(void) {
    // reset number of cops pursuing player
    // fixes incorrect music bug
    vehPoliceCarAudio::iNumCopsPursuingPlayer = 0;

    $::aiPoliceForce::Reset(this);
}

aiVehicle * findVehicle(vehCar *car) {
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

        return *getPtr<float>(path, 0x18);
    }

    return cfgDefaultSpeedLimit;
}

BOOL aiPoliceForceHandler::IsPerpDrivingMadly(vehCar *perpCar) {
    if (ageHook::Thunk<0x53E2A0>::Call<BOOL>(this, perpCar)) {
        char *vehName = perpCar->getCarDamage()->GetName(); // we can't use vehCarSim because the game forces vpcop to vpmustang99...

        // ignore perp if they're a cop
        if (!ageHook::StaticThunk<0x4D1A70>::Call<bool>(vehName))
        {
            float speed = perpCar->getCarSim()->getSpeed();
            float speedLimit = getSpeedLimit(perpCar) * 2.857142857142857f;

            if (speed > (speedLimit * cfgSpeedLimitTolerance)) {
                LogFile::Printf(1, "PERP DETECTED!!! He's doing %.4f over the speed limit (~%.4fmph)!\n", (speed - speedLimit), speedLimit);
                return TRUE;
            }
        }
    }
    
    if (ageHook::Thunk<0x53E390>::Call<BOOL>(this, perpCar)) {
        LogFile::Printf(1, "OFFICER INVOLVED COLLISION WITH PERP!");
        return TRUE;
    }

    return FALSE;
}

void aiPoliceForceHandler::Install() {
    InstallCallback("aiPoliceForce::Reset", "Resets the number of cops pursuing the player upon reset.",
        &Reset, {
            cbHook<CALL>(0x536AAE),
            cbHook<CALL>(0x550ECA),
        }
    );

    if (cfgPoliceAcademyFunding) {
        // obviously doesn't belong in aiPoliceForceHandler, should either move it or make this a generic "PoliceHandler"
        InstallCallback("aiPoliceOfficer::DetectPerpetrator", "Experimenting with making cops a little smarter about chasing people.",
            &IsPerpDrivingMadly, {
                cbHook<CALL>(0x53E057), // aiPoliceOfficer::Fov
            }
        );
    }
}

/*
    gfxImageHandler
*/

void gfxImageHandler::Scale(int width, int height) {
    width = *window_iWidth;
    height = *window_iHeight;

    reinterpret_cast<gfxImage*>(this)->Scale(width, height);
}

void gfxImageHandler::Install() {
    InstallCallback("gfxImage::Scale", "Fixes loading screen image scaling",
        &Scale, {
            cbHook<CALL>(0x401C75),
        }
    );
}

/*
    vehCarHandler
*/

static ConfigValue<bool> cfgVehicleDebug("VehicleDebug", "vehicleDebug", false);

void vehCarHandler::InitCar(LPCSTR vehName, int a2, int a3, bool a4, bool a5) {
    Displayf("Initializing vehicle (\"%s\", %d, %d, %s, %s)", vehName, a2, a3, bool_str(a4), bool_str(a5));
    get<vehCar>()->Init(vehName, a2, a3, a4, a5);
}

void vehCarHandler::InitCarAudio(LPCSTR name, int a2) {
    Displayf("Loading vehicle audio (\"%s\", %d)", name, a2);
    get<vehCar>()->InitAudio(name, a2);
}

const phBound * vehCarHandler::GetModelBound(int a1) {
    auto result = ageHook::Thunk<0x4648C0>::Call<const phBound *>(this, a1);

    if (result == NULL)
        Errorf(">>> COULD NOT RETRIEVE VEHICLE BOUND (%d) !!! <<<", a1);

    return result;
}

void vehCarHandler::Install(void) {
    if (cfgVehicleDebug) {
        InstallCallback("vehCar::Init", "Enables debugging for vehicle initialization.",
            &InitCar, {
                cbHook<CALL>(0x55942D), // aiVehiclePhysics::Init
                cbHook<CALL>(0x403BDD), // mmPlayer::Init
                cbHook<CALL>(0x43C536), // mmNetObject::Init
            }
        );

        InstallCallback("vehCar::InitAudio", "Enables debugging for vehicle initialization.",
            &InitCarAudio, {
                cbHook<CALL>(0x55943A), // aiVehiclePhysics::Init
                cbHook<CALL>(0x404090), // mmPlayer::Init
                cbHook<CALL>(0x43C540), // mmNetObject::Init
            }
        );

        InstallVTableHook("vehCarModel::GetBound",
            &GetModelBound, {
                0x5B2D14
            }
        );
    }
}

/*
    vehCarAudioContainerBugfixHandler
*/

void vehCarAudioContainerBugfixHandler::StartSiren() {
    auto policeAudio = get<vehCarAudioContainer>()->GetPoliceCarAudioPtr();

    if (policeAudio != nullptr) {
        // vehPoliceCarAudio::StartSiren
        ageHook::Thunk<0x4D4B20>::ThisCall<void>(policeAudio, 0);
    }
}

void vehCarAudioContainerBugfixHandler::StopSiren() {
    auto policeAudio = get<vehCarAudioContainer>()->GetPoliceCarAudioPtr();

    if (policeAudio != nullptr) {
        // vehPoliceCarAudio::StopSiren
        ageHook::Thunk<0x4D4C20>::ThisCall<void>(policeAudio);
    }
}

void vehCarAudioContainerBugfixHandler::Install() {
    InstallCallback("vehCarAudioContainer::StartSiren", "Fixes a crash caused by activating sirens on a vehicle with missing audio." ,
        &StartSiren, {
            cbHook<CALL>(0x4145FB), // mmGame::UpdateHorn
            cbHook<CALL>(0x43D533), // mmNetObject::PositionUpdate
        }
    );

    InstallCallback("vehCarAudioContainer::StopSiren", "Fixes a crash caused by deactivating sirens on a vehicle with missing audio." ,
        &StopSiren, {
            cbHook<CALL>(0x41460C), // mmGame::UpdateHorn
            cbHook<CALL>(0x43D562), // mmNetObject::PositionUpdate
        }
    );
}

/*
    vehCarModelHandler
*/

static ConfigValue<int> cfgMaxVehiclePaintjobs("MaxVehiclePaintjobs", 64);

//Fixes gizmo models in cars by initializing 64 variant slots instead of 10
void vehCarModelHandler::Install() {
    InstallPatch({ (byte)cfgMaxVehiclePaintjobs }, {
        0x4CD39E,
    });
}


/*
    mmBillInstanceHandler
*/

void mmBillInstanceHandler::Scale(float x, float y, float z) {
    ageHook::Thunk<0x4BE560>::Call<void>(this, x, y, -z);
}

void mmBillInstanceHandler::Install() {
    InstallCallback("mmBillInstance::Draw", "Fix inverted checkpoints",
        &Scale, {
            cbHook<CALL>(0x43F952), // mmBillInstance::Draw
        }
    );

    Installf("Installing fix for vertical billboarding of CnR checkpoints...");
    mem::nop(0x43F8FD, (6 * 3)); // 6 fld/fstp instructions (size: 3)
}

/*
    mmSpeedIndicator
*/

static ConfigValue<bool> cfgSpeedoUseUpperLimit("SpeedoUseUpperLimit", true);

// due to _ftol's non-standard calling convention (and because we patched the call),
// we have to make the compiler think there's an int in ECX (for __fastcall)
// otherwise, it'll try popping a float from the stack :/
int __fastcall Float2Long(int fValueNotAnInt) {
    // this compiles down to moving ECX into a stack variable
    // which then allows us to _properly_ convert the value
    float actualValue = *(float*)&fValueNotAnInt;
    int result = (int)actualValue;

    return (result < 1000) ? result : 999;
}

void mmSpeedIndicatorHandler::Install() {
    if (cfgSpeedoUseUpperLimit)
    {
        InstallCallback("mmSpeedIndicator::Draw", "Fixes graphical UI errors that occur when a vehicle travels too fast.",
            &Float2Long, {
                cbHook<CALL>(0x43F345),
            }
        );

        // change 'fld (...)' to 'mov ecx, (...)'
        // this way we can pass it to Float2Long
        InstallPatch({ 0x8B, 0x88 }, {
            0x43F33F,
        });
    }
}

/*
    cityLevelBugfixHandler
*/

ageHook::Type<bool> sm_EnablePVS(0x62B070);

Stream* cityLevelBugfixHandler::OpenPvsStream(const char * folder, const char * file, const char * extension, bool a4, bool a5) {
    //open stream
    auto stream = ageHook::StaticThunk<0x4C58C0>::Call<Stream*>(folder, file, extension, a4, a5);
    
    //stream will be NULL if the PVS doesn't exist
    if (!stream) {
        sm_EnablePVS = false;
    }

    //return original stream
    return stream;
}

void cityLevelBugfixHandler::Install() {
    InstallCallback("cityLevel::Load", "Disables PVS when it doesn't exist.",
        &OpenPvsStream, {
            cbHook<CALL>(0x4440E8), // cityLevel::Load
        }
    );
}

/*
    mmInterfaceHandler
*/

ageHook::Type<char[80]> currentPlayerVehicle (0x6B1B28);

void mmInterfaceHandler::PlayerResolveCars() {
    //call original
    ageHook::Thunk<0x40FE20>::Call<void>(this);

    //null out currentPlayerVehicle if this vehicle is missing
    //the code after PlayerResolveCars in PlayerSetState will then reset to vpbug
    if (!datAssetManager::Exists("tune", currentPlayerVehicle, "info")) {
        currentPlayerVehicle[0] = NULL;
    }
}

void mmInterfaceHandler::Install() {
    InstallCallback("mmInterface::PlayerSetState", "Fixes game crashes in the vehicle select menu when the selected vehicle is missing.",
        &PlayerResolveCars, {
            cbHook<CALL>(0x040E256),
        }
    );
}

/*
    lvlSkyHandler
*/

void lvlSkyHandler::Install() {
    //Overwrite SetMIPMapEnv call since it does nothing
    //and the game will crash without a .sky file when atetmpting to call it
    InstallPatch({ 0xEB, 0x0F }, {
        0x465226,
    });
}

/*
    BugfixPatchHandler
*/

void BugfixPatchHandler::Install() {
    InstallPatch("Fixes a bug where the HUD becomes permanently disabled after switching from a POV cam.", {
        0xEB, 0x3A, // jmp short loc_414959
        0x90,       // nop
    }, {
        0x41491D,   // mmGame::UpdateGameInput
    });

    InstallPatch("Fixes a bug where an access violation may occur during Cops 'n Robbers gameplay.", {
        0x51,       // push ecx
        0x89, 0xE0, // mov eax, esp
        0xDB, 0x00, // fild [eax]
        0x90, 0x90, // nop(2)
    }, {
        0x424982,   // mmMultiCR::ImpactCallback
    });
}

/*
    mmHudMapHandler
*/

bool mmHudMapHandler::CanActivateMap() {
    return *getPtr<byte>(this, 0x3D) != 1;   
}

void mmHudMapHandler::Activate() {
    if (!CanActivateMap())
        return;

    //forward to mmHudMap::Activate
    ageHook::Thunk<0x42EEE0>::Call<void>(this);
}

void mmHudMapHandler::SetMapMode(int mode) {
    if (!CanActivateMap())
        return;

    //forward to mmHudMap::SetMapMode
    ageHook::Thunk<0x42EF30>::Call<void>(this, mode);
}

void mmHudMapHandler::Install() {
    InstallCallback("mmHudMap::Activate", "Fixes crashes when attempting to activate a nonexistant hudmap.",
        &Activate, {
            cbHook<CALL>(0x42A306), // mmPopup::DisablePU
        }
    );
    InstallCallback("mmHudMap::SetMapMode", "Fixes crashes when attempting to activate a nonexistant hudmap.",
        &SetMapMode, {
            cbHook<CALL>(0x42EE98), // mmHudMap::Reset
            cbHook<CALL>(0x43204E), // mmViewMgr::SetViewSetting
        }
    );
}

/*
    mmPopupHandler
*/

static ConfigValue<bool> cfgChatMusicFix("ChatMusicFix", true);

// fix cd player persisting through popups
bool wasCdPlayerEnabled = false;

void mmPopupHandler::HudEnable() {
    auto cdPlayer = mmGameManager::Instance.get()->getGame()->getPlayer()->getHUD()->getCdPlayer();

    // if cd player is inactive, and it was active before
    // reactivate it
    if(wasCdPlayerEnabled && !cdPlayer->isActive())
      cdPlayer->Toggle();

    ageHook::Thunk<0x42D910>::Call<void>(this); // mmHUD::Enable
}

void mmPopupHandler::HudDisable(int a1) {
    auto cdPlayer = mmGameManager::Instance.get()->getGame()->getPlayer()->getHUD()->getCdPlayer();
    wasCdPlayerEnabled = cdPlayer->isActive();

    // hide the cd player if shown
    if (wasCdPlayerEnabled)
        cdPlayer->Toggle();

    ageHook::Thunk<0x42D970>::Call<void>(this, a1); // mmHUD::Disable
}

void mmPopupHandler::Install() {
    // Fixes chat music presisting after the chat box is closed
    if (cfgChatMusicFix) {
        InstallPatch({ 0x01 }, {
            0x42B558+1,
            0x42B537+1,
        });
    }

    // CD player fixes
    InstallCallback("mmPopup::DisablePU", "Shows the CD player on popup disable",
        &HudEnable, {
            cbHook<CALL>(0x42A2F5),
        }
    );

    InstallCallback("mmPopup::ShowResults", "Hides the CD player when popups are showing",
        &HudDisable, {
            cbHook<CALL>(0x42A65F),
            cbHook<CALL>(0x42A722),
            cbHook<CALL>(0x42A7EB),
            cbHook<CALL>(0x42A3BF),
        }
    );
}

/*
    mmMultiCRHandler
*/

bool mmMultiCRHandler::LoadMusic(char* a1, char* a2) {
    return ageHook::Thunk<0x433F40>::Call<bool>(this, "singlerace", a2);
}

void mmMultiCRHandler::Install() {
    InstallCallback("mmMultiCR::Init", "Fixes results screen crash due to incorrect music.",
        &LoadMusic, {
            cbHook<CALL>(0x4239CB),
        }
    );

    //changes VTable for music data from roam to race
    InstallPatch({ 0x10, 0x06, 0x5B}, {
        0x423715 + 6,
    });
}

/*
    audManagerHandler
*/

// reimplemented using similar logic found in MM1
// AudManager is aware of a Mixer, but it's never initialized
class MixerCTL {
private:
    float unk_04;
    float unk_08;
    float unk_0C;

    float unk_10;
    float unk_14;
    float unk_18;

    HWND window;

    int unk_20;
    int unk_24; // = 1?

    uint32_t mixer_id;

    float wave_balance;
    float cd_balance; // initialized but not used
public:
    explicit MixerCTL() {
        // override the vtable
        *reinterpret_cast<int *>(this) = 0x5B4E70;

        unk_04 = 1.0f;
        unk_08 = 1.0f;
        unk_0C = 1.0f;

        unk_10 = 1.0f;
        unk_14 = 1.0f;
        unk_18 = 1.0f;

        window = GetActiveWindow();

        unk_24 = 1;

        wave_balance = 0.0f;
        cd_balance = 0.0f;
    }

    ULONG Init(void) {
        return ageHook::Thunk<0x51C1F0>::Call<ulong>(this);
    }

    virtual ~MixerCTL() FORWARD_THUNK;
};

ASSERT_SIZEOF(MixerCTL, 0x34);

static MixerCTL * CreateMixerCTL() {
    auto result = new MixerCTL();

    // initialize the mixer
    result->Init();

    return result;
}

static ConfigValue<int> cfgAudioMaxSounds("AudioMaxSounds", 800);

void audManagerHandler::Init(int maxSounds, int a2, int a3, char *a4, short a5, short a6) {
    maxSounds = cfgAudioMaxSounds;

    LogFile::Printf(1, "[audManagerHandler::Init]: %d max sounds", maxSounds);

#ifdef USE_MIXER_STUFF
    // mixer already initialized?
    if (*getPtr<void *>(this, 0x34) == nullptr) {
        LogFile::Printf(1, "[audManagerHandler::Init]: Creating mixer...");

        auto mixer = CreateMixerCTL();

        setPtr<MixerCTL *>(this, 0x34, mixer);
    }
#endif

    ageHook::Thunk<0x519350>::Call<void>(this, maxSounds, a2, a3, a4, a5, a6);
}

void audManagerHandler::AssignCDVolume(float value) {
    // update mixer volume first
    SetMixerCDVolume(value);

    // to prevent CD volume acting as a "master volume" slider...
    // I have no idea why this is needed :/
    SetMixerWaveVolume(MMSTATE->SoundFXVolume);
    
    // AudManager::AssignCDVolume
    ageHook::Thunk<0x519A30>::Call<void>(this, value);
}

void audManagerHandler::SetupCDAudio(float balance) {
    SetMixerCDVolume(MMSTATE->MusicVolume);
    SetMixerWaveVolume(MMSTATE->SoundFXVolume);

    // AudManager::AssignCDBalance
    ageHook::Thunk<0x519880>::Call<void>(this, balance);
}

void audManagerHandler::SetMixerCDVolume(float value) {
    auto mixer = *getPtr<void *>(this, 0x34);

    if (mixer != nullptr) {
        LogFile::Printf(1, "[audManagerHandler::SetMixerVolume]: Setting mixer CD volume to %.2f", value);

        // MixerCTL::AssignCDVolume
        ageHook::Thunk<0x51C330>::Call<void>((MixerCTL *)mixer, value);
    }
}

void audManagerHandler::SetMixerWaveVolume(float value) {
    auto mixer = *getPtr<void *>(this, 0x34);

    if (mixer != nullptr) {
        LogFile::Printf(1, "[audManagerHandler::SetMixerVolume]: Setting mixer sound volume to %.2f", value);

        // MixerCTL::AssignWaveVolume
        ageHook::Thunk<0x51C310>::Call<void>((MixerCTL *)mixer, value);
    }
}

void audManagerHandler::Install() {
    int audHeapSize = HookConfig::GetProperty("AudioHeapSize", 32);

    LogFile::Printf(1, "Audio heap size: %dMB", audHeapSize);

    mem::write(0x51938D + 1, (int)(audHeapSize * 1000000));

    InstallCallback("AudManager::Init", "Allows the mixer control to be initialized along with the audio manager.",
        &Init, {
            cbHook<CALL>(0x401F1B),
        }
    );

#ifdef USE_MIXER_STUFF
    InstallCallback("AudManager::AssignCDVolume", "Properly sets mixer volume when changing CD volume.",
        &AssignCDVolume, {
            cbHook<CALL>(0x401F81), // InitAudioManager
            cbHook<CALL>(0x50C932), // PUAudioOptions::SetCDVolume
        }
    );

    InstallCallback("AudManager::AssignWaveVolume", "Properly sets mixer volume when changing sound volume.",
        &AssignCDVolume, {
            cbHook<CALL>(0x401F71), // InitAudioManager
            cbHook<CALL>(0x50C8FC), // PUAudioOptions::SetWaveVolume
        }
    );

    InstallCallback(&SetupCDAudio, "Allows the mixer volume to be updated when loading player config.", {
            cbHook<CALL>(0x525DC6), // mmPlayerConfig::SetAudio
        }
    );
#endif
}

/*
    pedAnimationInstanceHandler
*/

ageHook::Type<float> FrameFraction      = 0x6B4724;
ageHook::Type<int> FrameDelta           = 0x6B4720;

static float pedAnimFrameRate   = 30.0f;
static float pedAnimFPS         = (1000.0f / pedAnimFrameRate);

void pedAnimationInstanceHandler::PreUpdate(float seconds) {
    FrameFraction -= (seconds * -pedAnimFPS);

    float delta = floor(FrameFraction);

    FrameDelta = (int)delta;
    FrameFraction -= delta;
}

void pedAnimationInstanceHandler::Install() {
    InstallCallback("pedAnimationInstance::PreUpdate", "Allows for more precise control over pedestrian animations.",
        &PreUpdate, {
            cbHook<CALL>(0x54BF6A),
        }
    );
}

/*
    mmCDPlayerHandler
*/

void mmCDPlayerHandler::Install() {
    //nop out jump
    InstallPatch({ 0x90, 0x90 }, {
        0x42CEC4,
    });

    //force font to 24
    InstallPatch({ 0x18 }, {
        0x42CEC7,
    });
}