#include "bugfix_handlers.h"

using namespace MM2;

static init_handler g_bugfix_handlers[] = {
    CreateHandler<aiPathHandler>("aiPath"),
    CreateHandler<aiPedestrianHandler>("aiPedestrian"),
    CreateHandler<aiPoliceForceHandler>("aiPoliceForce"),
    CreateHandler<aiPoliceOfficerHandler>("aiPoliceOfficer"),
    CreateHandler<aiVehicleAmbientHandler>("aiVehicleAmbient"),
    CreateHandler<aiVehicleInstanceHandler>("aiVehicleInstance"),
    CreateHandler<aiGoalAvoidPlayerHandler>("aiGoalAvoidPlayer"),
    CreateHandler<aiRouteRacerHandler>("aiRouteRacer"),

    CreateHandler<asMeshCardInfoHandler>("asMeshCardInfo"),

    CreateHandler<audManagerHandler>("audManager"),

    CreateHandler<gfxImageHandler>("gfxImage"),

    CreateHandler<mmBillInstanceHandler>("mmBillInstance"),

    CreateHandler<mmInterfaceHandler>("mmInterface"),
    CreateHandler<mmPopupHandler>("mmPopupHandler"),

    CreateHandler<vehCarBugfixHandler>("vehCar"),
    CreateHandler<vehCarAudioHandler>("vehCarAudio"),
    CreateHandler<vehCarAudioContainerBugfixHandler>("vehCarAudioContainer bugfixes"),
    CreateHandler<vehCarModelHandler>("vehCarModel"),
    CreateHandler<vehTrailerHandler>("vehTrailer"),
    CreateHandler<vehTrailerInstanceHandler>("vehTrailerInstance"),
    CreateHandler<vehPoliceCarAudioBugfixHandler>("vehPoliceCarAudio"),
    CreateHandler<vehSemiCarAudioBugfixHandler>("vehSemiCarAudio"),
    CreateHandler<mmDashViewBugfixHandler>("mmDashViewBugfix"),
    CreateHandler<mmPlayerBugfixHandler>("mmPlayer"),
    CreateHandler<mmGearIndicatorHandler>("mmGearIndicator"),
    CreateHandler<mmSpeedIndicatorHandler>("mmSpeedIndicator"),
    CreateHandler<mmHudMapHandler>("mmHudMap"),
    CreateHandler<mmCDPlayerHandler>("mmCDPlayer"),
    CreateHandler<mmMirrorHandler>("mmMirror"),
    CreateHandler<lvlSkyHandler>("lvlSkyHandler"),

    CreateHandler<modShaderHandler>("modShader"),

    CreateHandler<cityLevelBugfixHandler>("cityLevelBugfixHandler"),

    CreateHandler<mpConsistencyHandler>("mpConsistencyHandler"),
    CreateHandler<mmMultiCRHandler>("mmMultiCR"),

    CreateHandler<BugfixPatchHandler>("Bugfix patches"),

    CreateHandler<pedAnimationInstanceHandler>("pedAnimationInstance"),
    CreateHandler<fxShardManagerBugfixHandler>("fxShardManager")
    //CreateHandler<phBoundBugfixHandler>("phBound"),
};

/*
    aiPathHandler
*/

int numPedUpdateAttempts = 0;

void aiPathHandler::UpdatePedestrians(void) {
    auto path = reinterpret_cast<aiPath*>(this);
    numPedUpdateAttempts = 0;

    path->UpdatePedestrians();
}

void aiPathHandler::Install() {
    InstallCallback("aiPath::UpdatePedestrians", "Limits the number of update attempts for pedestrians.",
        &UpdatePedestrians, {
            cb::call(0x536FE0), // aiMap::Update
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
            cb::call(0x544191), // aiPath::UpdatePedestrians
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
    char *vehName = perpCar->getCarDamage()->GetName(); // we can't use vehCarSim because the game forces vpcop to vpmustang99...

    // ignore perp if they're a cop
    if (!hook::StaticThunk<0x4D1A70>::Call<bool>(vehName)) {
        if (hook::Thunk<0x53E2A0>::Call<BOOL>(this, perpCar))
        {
            float speed = perpCar->getCarSim()->getSpeedMPH();
            float speedLimit = getSpeedLimit(perpCar) * 2.857142857142857f;

            if (speed > (speedLimit * cfgSpeedLimitTolerance)) {
                LogFile::Printf(1, "PERP DETECTED!!! He's doing %.4f over the speed limit (~%.4fmph)!\n", (speed - speedLimit), speedLimit);
                return TRUE;
            }
            if (hook::Thunk<0x53E370>::Call<BOOL>(this, perpCar)) {
                LogFile::Printf(1, "PERP IS DOING DAMAGE TO PROPERTY!");
                return TRUE;
            }
        }
        if (hook::Thunk<0x53E390>::Call<BOOL>(this, perpCar)) {
            LogFile::Printf(1, "OFFICER INVOLVED COLLISION WITH PERP!");
            return TRUE;
        }
    }

    return FALSE;
}

void aiPoliceForceHandler::Install() {
    InstallCallback("aiPoliceForce::Reset", "Resets the number of cops pursuing the player upon reset.",
        &Reset, {
            cb::call(0x536AAE),
            cb::call(0x550ECA),
        }
    );

    if (cfgPoliceAcademyFunding) {
        // obviously doesn't belong in aiPoliceForceHandler, should either move it or make this a generic "PoliceHandler"
        InstallCallback("aiPoliceOfficer::DetectPerpetrator", "Experimenting with making cops a little smarter about chasing people.",
            &IsPerpDrivingMadly, {
                cb::call(0x53E057), // aiPoliceOfficer::Fov
            }
        );
    }

    // fix aiPoliceOfficer::Collision
    InstallPatch({ 0x8B, 0x91, 0xF4, 0x00, 0x00, 0x00 }, {
        0x53E37E,
    });
}

/*
    aiPoliceOfficerHandler
*/

void aiPoliceOfficerHandler::PerpEscapes(bool a1) {
    // stop siren lights when perp escapes
    $::aiPoliceOfficer::StopSiren(this);

    $::aiPoliceOfficer::PerpEscapes(this, a1);
}

void aiPoliceOfficerHandler::Install() {
    InstallCallback("aiPoliceOfficer::PerpEscapes", "Fixes ai police siren lights being active if perp escaped.",
        &PerpEscapes, {
            cb::call(0x53DD1F),
            cb::call(0x53DE83),
            cb::call(0x53DE9F),
            cb::call(0x53DEC1),
        }
    );
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
            cb::call(0x401C75),
        }
    );
}

/*
    vehCarBugfixHandler
*/
static ConfigValue<bool> cfgMm1StyleTransmission("MM1StyleTransmission", false);

void vehCarBugfixHandler::Update() {
    auto car = reinterpret_cast<vehCar*>(this);
    auto carsim = car->getCarSim();
    auto engine = carsim->getEngine();
    auto drivetrain = carsim->getDrivetrain();
    auto transmission = carsim->getTransmission();
    auto curDamage = car->getCarDamage()->getCurDamage();
    auto maxDamage = car->getCarDamage()->getMaxDamage();

    if (curDamage < maxDamage) {
        if (transmission->IsAuto()) {
            if (carsim->getSpeedMPH() >= 1.f) {
                if (engine->getThrottleInput() < 0.1f && transmission->getGear() != 1)
                    engine->setThrottleInput(0.1f);
            }
            // activate Handbrake if car goes under 1mph (P gear)
            if (carsim->getSpeedMPH() < 1.f && engine->getThrottleInput() < 0.1f) {
                carsim->setHandbrake(1.f);
            }
        }
        if (!transmission->IsAuto()) {
            if (carsim->getBrake() < 0.1f && carsim->getHandbrake() < 0.1f) {
                if (engine->getThrottleInput() < 0.1f && transmission->getGear() != 1)
                    engine->setThrottleInput(0.1f);
            }
        }
    }
    // setting up this case for crash course
    // fixes ai cops and opponents have no brakes if they're damaged out
    if (curDamage >= maxDamage) {
        carsim->setBrake(1.f);
    }

    // attach drive train if we just hit throttle
    // fixes the short delay that happens before the car moves
    if (engine->getThrottleInput() > 0.f) {
        drivetrain->Attach();
    }

    // call original
    hook::Thunk<0x42C690>::Call<void>(this);
}

void vehCarBugfixHandler::Install() {
    if (cfgMm1StyleTransmission.Get()) {
        InstallVTableHook("vehCar::Update",
            &Update, {
                0x5B0BB8,
            });

        // deactivate auto Handbrake system
        InstallPatch({ 0xD8, 0x1D, 0x3C, 0x04, 0x5B, 0x00 }, {
            0x405C81,
        });
    }
}

/*
    vehCarAudioHandler
*/
static ConfigValue<float> cfgAirborneTimerThresh("AirborneTimerThreshold", 1.1);
static ConfigValue<float> cfgAirborneSpeedThresh("AirborneSpeedThreshold", 45.0);
static ConfigValue<bool> cfgEnableAirborneCheck("TweakableAirborneCheck", false);

float carAirborneTimer = 0.0f;
float carAirborneThreshold = 1.1f;
float carAirborneSpeedThreshold = 45.0;

bool vehCarAudioHandler::IsAirBorne() {
    return carAirborneTimer > carAirborneThreshold;
}

void vehCarAudioHandler::Update() {
    float elapsedTime = datTimeManager::Seconds;
    auto carAudio = reinterpret_cast<vehCarAudio*>(this);
    auto carSim = carAudio->getCarSim();
    
    // grab only forward/sideways velocity, ignore vertical
    auto vehicleVelo = carSim->getInstance()->GetVelocity();
    float vehicleMph = sqrtf((vehicleVelo->X * vehicleVelo->X) + (vehicleVelo->Z * vehicleVelo->Z)) * 2.23694f;

    //update timer
    if (carSim->OnGround() || carSim->BottomedOut() || vehicleMph < carAirborneSpeedThreshold) 
    {
        carAirborneTimer = 0.0f;
    }
    else  
    {
        carAirborneTimer += elapsedTime;
    }

    // call original
    hook::Thunk<0x4DC320>::Call<void>(this);
}

void vehCarAudioHandler::Reset() {
    carAirborneTimer = 0.0f;

    // call original
    hook::Thunk<0x4DBE30>::Call<void>(this);
}

void vehCarAudioHandler::Install() {
    if (!cfgEnableAirborneCheck.Get())
        return;

    carAirborneSpeedThreshold = cfgAirborneSpeedThresh.Get();
    carAirborneThreshold = cfgAirborneTimerThresh.Get();

    InstallCallback("vehCarAudio::IsAirBorne", "Better method of vehicle airborne checking.",
        &IsAirBorne, {
            cb::jmp(0x4D16F9),
            cb::jmp(0x4D1705),
            cb::jmp(0x4D1711),
            cb::jmp(0x4D171F),
        }
    );

    InstallCallback("vehCarAudio::Reset", "Better method of vehicle airborne checking.",
        &Reset, {
            cb::call(0x4D19CA),
        }
    );

    InstallVTableHook("vehCarAudio::Update",
        &Update, {
            0x5B319C
        }
    );
}

/*
    vehCarAudioContainerBugfixHandler
*/

void vehCarAudioContainerBugfixHandler::StartSiren() {
    auto policeAudio = get<vehCarAudioContainer>()->GetPoliceCarAudioPtr();

    if (policeAudio != nullptr) {
        // vehPoliceCarAudio::StartSiren
        hook::Thunk<0x4D4B20>::ThisCall<void>(policeAudio, 0);
    }
}

void vehCarAudioContainerBugfixHandler::StopSiren() {
    auto policeAudio = get<vehCarAudioContainer>()->GetPoliceCarAudioPtr();

    if (policeAudio != nullptr) {
        // vehPoliceCarAudio::StopSiren
        hook::Thunk<0x4D4C20>::ThisCall<void>(policeAudio);
    }
}

void vehCarAudioContainerBugfixHandler::Install() {
    InstallCallback("vehCarAudioContainer::StartSiren", "Fixes a crash caused by activating sirens on a vehicle with missing audio." ,
        &StartSiren, {
            cb::call(0x4145FB), // mmGame::UpdateHorn
            cb::call(0x43D533), // mmNetObject::PositionUpdate
        }
    );

    InstallCallback("vehCarAudioContainer::StopSiren", "Fixes a crash caused by deactivating sirens on a vehicle with missing audio." ,
        &StopSiren, {
            cb::call(0x41460C), // mmGame::UpdateHorn
            cb::call(0x43D562), // mmNetObject::PositionUpdate
        }
    );
}

/*
    vehCarModelHandler
*/

static ConfigValue<int> cfgMaxVehiclePaintjobs("MaxVehiclePaintjobs", 64);

// Fixes gizmo models in cars by initializing 64 variant slots instead of 10
void vehCarModelHandler::Install() {
    InstallPatch({ (byte)cfgMaxVehiclePaintjobs }, {
        0x4CD39E,
    });
}


/*
    mmBillInstanceHandler
*/

void mmBillInstanceHandler::Scale(float x, float y, float z) {
    hook::Thunk<0x4BE560>::Call<void>(this, x, y, -z);
}

void mmBillInstanceHandler::Install() {
    InstallCallback("mmBillInstance::Draw", "Fix inverted checkpoints",
        &Scale, {
            cb::call(0x43F952), // mmBillInstance::Draw
        }
    );

    Installf("Installing fix for vertical billboarding of CnR checkpoints...");
    mem::nop(0x43F8FD, (6 * 3)); // 6 fld/fstp instructions (size: 3)
}

/*
    mmGearIndicatorHandler
*/

void mmGearIndicatorHandler::Draw() {
    int *gearIndicator = *getPtr<int*>(this, 0x3C);
    int *vehCarPtr = *getPtr<int*>(gearIndicator, 0x284);
    int reverse = *getPtr<int>(vehCarPtr, 0x304);
    int autoTransmission = *getPtr<int>(vehCarPtr, 0x300);
    int gearID = -1;
    float speedMPH = *getPtr<float>(vehCarPtr, 0x24C);
    float throttle = *getPtr<float>(vehCarPtr, 0x2BC);

    if (autoTransmission) {
        if (throttle > 0.f || speedMPH >= 1.f) {
            if (!reverse)
                gearID = 9;
            else if (reverse == 1)
                gearID = 10;
            else
                gearID = 11;
        }
        else {
            gearID = 0;
        }
    }
    if (!autoTransmission) {
        if (!reverse)
            gearID = 9;
        else if (reverse == 1)
            gearID = 10;
        else
            gearID = reverse - 1;
    }

    // get gfxPipeline::CopyBitmap stuff
    int v1 = *getPtr<int>(this, 0x30);
    int v2 = *getPtr<int>(this, 0x34);
    int *v3 = *getPtr<int*>(this, 0x38);
    int v4 = *getPtr<int>(v3, 0x20);
    int v5 = *getPtr<int>(v3, 0x24);
    auto *bitmap = *getPtr<gfxBitmap*>(this, gearID * 4);
    int width = bitmap->Width;
    int height = bitmap ->Height;
    
    gfxPipeline::CopyBitmap(
        v1 + v4,     // destX
        v2 + v5,     // destY
        bitmap,      // bitmap
        0,           // srcX
        0,           // srcY
        width,       // width
        height,      // height
        true);       // srcColorKey
}

void mmGearIndicatorHandler::Install() {
    if (cfgMm1StyleTransmission.Get()) {
        InstallCallback("mmGearIndicatorHandler::Draw", "Adds the unused P gear indicator to the HUD.",
            &Draw, {
                cb::call(0x431B26),
            }
        );
    }

    InstallPatch("Replace N letter with P", {
        0xB8, 0xD4, 0x52, 0x5C, 0x00,    // mov eax, offset aDigitac_gear_p
        0x75, 0x05,                      // jnz short loc_43F132
        0xB8, 0xE8, 0x52, 0x5C, 0x00,    // mov eax, offset aDigitac_gear_0
    }, {
        0x43F126 // mmGearIndicator::Init
    });

    InstallPatch("Replace P letter with N", {
        0xB8, 0x64, 0x52, 0x5C, 0x00,    // mov eax, offset aDigitac_gear_n
        0x75, 0x05,                      // jnz short loc_43F1AF
        0xB8, 0x78, 0x52, 0x5C, 0x00,    // mov eax, offset aDigitac_gear_3
    }, {
        0x43F1A3 // mmGearIndicator::Init
    });
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
                cb::call(0x43F345),
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

hook::Type<asParticles*> sm_RainParticles(0x62770C);
hook::Type<bool> sm_EnablePVS(0x62B070);
bool cityLevelBugfixHandler::IsMirrorDrawing = false;

Stream* cityLevelBugfixHandler::OpenPvsStream(const char * folder, const char * file, const char * extension, bool a4, bool a5) {
    //open stream
    auto stream = hook::StaticThunk<0x4C58C0>::Call<Stream*>(folder, file, extension, a4, a5);
    
    //stream will be NULL if the PVS doesn't exist
    if (!stream) {
        sm_EnablePVS = false;
    }

    //return original stream
    return stream;
}

void cityLevelBugfixHandler::UpdateRainParticles() {
    asParticles* rainParticles = (asParticles*)sm_RainParticles;

    // set position if appropriate
    if (!IsMirrorDrawing) {
        Vector4 dotWith = Vector4(0.0, 10.0, -10.0, 1.0);
        
        Vector4 newParticlePosition = Vector4(0, 0, 0, 0);
        newParticlePosition.Dot(dotWith, *(Matrix44*)gfxRenderState::sm_Camera);

        rainParticles->pBirthRule->Position.X = newParticlePosition.X;
        rainParticles->pBirthRule->Position.Y = newParticlePosition.Y;
        rainParticles->pBirthRule->Position.Z = newParticlePosition.Z;
    }
    
    // render particles
    rainParticles->Cull();
}

void cityLevelBugfixHandler::Install() {
    InstallCallback("cityLevel::Load", "Disables PVS when it doesn't exist.",
        &OpenPvsStream, {
            cb::call(0x4440E8), // cityLevel::Load
        }
    );

    InstallCallback("lvlLevel::Draw", "Allows for control over when to update rain particle position.",
        &UpdateRainParticles, {
            cb::call(0x4462B7),
        }
    );
    mem::nop(0x4462BA + 0x02, 76); // nop out the rest of the rain update, since we're replacing it
}

/*
    mmMirrorHandler
*/

void mmMirrorHandler::Cull() {
    cityLevelBugfixHandler::IsMirrorDrawing = true;
    hook::Thunk<0x42B8C0>::Call<void>(this); // call original
    cityLevelBugfixHandler::IsMirrorDrawing = false;
}

void mmMirrorHandler::Install() {
    InstallVTableHook("mmMirror::Cull",
        &Cull, {
            0x5B0B80 ,
        }
    );
}

/*
    mmInterfaceHandler
*/

hook::Type<char[80]> currentPlayerVehicle (0x6B1B28);

void mmInterfaceHandler::PlayerResolveCars() {
    //call original
    hook::Thunk<0x40FE20>::Call<void>(this);

    //null out currentPlayerVehicle if this vehicle is missing
    //the code after PlayerResolveCars in PlayerSetState will then reset to vpbug
    if (!datAssetManager::Exists("tune", currentPlayerVehicle, "info")) {
        currentPlayerVehicle[0] = NULL;
    }
}

void mmInterfaceHandler::Install() {
    InstallCallback("mmInterface::PlayerSetState", "Fixes game crashes in the vehicle select menu when the selected vehicle is missing.",
        &PlayerResolveCars, {
            cb::call(0x040E256),
        }
    );
}

/*
    lvlSkyHandler
*/

void lvlSkyHandler::ResetRot() {
    float* skyRot = (float*)(0x628728 + 0x18); //cityLevel::sm_Sky + 0x18
    *skyRot = 0.0f;
}

void lvlSkyHandler::Install() {
    //Overwrite SetMIPMapEnv call since it does nothing
    //and the game will crash without a .sky file when atetmpting to call it
    InstallPatch({ 0xEB, 0x0F }, {
        0x465226,
    });

    InstallCallback("mmGame::Reset", "Fixes sky rotation on city reset.",
        &ResetRot, {
            cb::jmp(0x413DE3),
        }
    );
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

    InstallPatch("Fixes a bug where makes the cops fly away.", {
        0xEB, 0x41, // jmp short loc_53DCEA
    }, {
        0x53DCA7,   // aiPoliceOfficer::Update
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
    hook::Thunk<0x42EEE0>::Call<void>(this);
}

void mmHudMapHandler::SetMapMode(int mode) {
    if (!CanActivateMap())
        return;

    //forward to mmHudMap::SetMapMode
    hook::Thunk<0x42EF30>::Call<void>(this, mode);
}

void mmHudMapHandler::Install() {
    InstallCallback("mmHudMap::Activate", "Fixes crashes when attempting to activate a nonexistant hudmap.",
        &Activate, {
            cb::call(0x42A306), // mmPopup::DisablePU
        }
    );
    InstallCallback("mmHudMap::SetMapMode", "Fixes crashes when attempting to activate a nonexistant hudmap.",
        &SetMapMode, {
            cb::call(0x42EE98), // mmHudMap::Reset
            cb::call(0x43204E), // mmViewMgr::SetViewSetting
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

    hook::Thunk<0x42D910>::Call<void>(this); // mmHUD::Enable
}

void mmPopupHandler::HudDisable(int a1) {
    auto cdPlayer = mmGameManager::Instance.get()->getGame()->getPlayer()->getHUD()->getCdPlayer();
    wasCdPlayerEnabled = cdPlayer->isActive();

    // hide the cd player if shown
    if (wasCdPlayerEnabled)
        cdPlayer->Toggle();

    hook::Thunk<0x42D970>::Call<void>(this, a1); // mmHUD::Disable
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
            cb::call(0x42A2F5),
        }
    );

    InstallCallback("mmPopup::ShowResults", "Hides the CD player when popups are showing",
        &HudDisable, {
            cb::call(0x42A65F),
            cb::call(0x42A722),
            cb::call(0x42A7EB),
            cb::call(0x42A3BF),
        }
    );
}

/*
    mmMultiCRHandler
*/

bool mmMultiCRHandler::LoadMusic(char* a1, char* a2) {
    return hook::Thunk<0x433F40>::Call<bool>(this, "singlerace", a2);
}

void mmMultiCRHandler::Install() {
    InstallCallback("mmMultiCR::Init", "Fixes results screen crash due to incorrect music.",
        &LoadMusic, {
            cb::call(0x4239CB),
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
        return hook::Thunk<0x51C1F0>::Call<ulong>(this);
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

    hook::Thunk<0x519350>::Call<void>(this, maxSounds, a2, a3, a4, a5, a6);
}

void audManagerHandler::AssignCDVolume(float value) {
    // update mixer volume first
    SetMixerCDVolume(value);

    // to prevent CD volume acting as a "master volume" slider...
    // I have no idea why this is needed :/
    SetMixerWaveVolume(MMSTATE->SoundFXVolume);
    
    // AudManager::AssignCDVolume
    hook::Thunk<0x519A30>::Call<void>(this, value);
}

void audManagerHandler::SetupCDAudio(float balance) {
    SetMixerCDVolume(MMSTATE->MusicVolume);
    SetMixerWaveVolume(MMSTATE->SoundFXVolume);

    // AudManager::AssignCDBalance
    hook::Thunk<0x519880>::Call<void>(this, balance);
}

void audManagerHandler::SetMixerCDVolume(float value) {
    auto mixer = *getPtr<void *>(this, 0x34);

    if (mixer != nullptr) {
        LogFile::Printf(1, "[audManagerHandler::SetMixerVolume]: Setting mixer CD volume to %.2f", value);

        // MixerCTL::AssignCDVolume
        hook::Thunk<0x51C330>::Call<void>((MixerCTL *)mixer, value);
    }
}

void audManagerHandler::SetMixerWaveVolume(float value) {
    auto mixer = *getPtr<void *>(this, 0x34);

    if (mixer != nullptr) {
        LogFile::Printf(1, "[audManagerHandler::SetMixerVolume]: Setting mixer sound volume to %.2f", value);

        // MixerCTL::AssignWaveVolume
        hook::Thunk<0x51C310>::Call<void>((MixerCTL *)mixer, value);
    }
}

bool audManagerHandler::MinInstall() {
    return !datAssetManager::Exists("aud\\aud11\\al2", "AL2Pre01", "11k.wav");
}

void audManagerHandler::Install() {
    int audHeapSize = HookConfig::GetProperty("AudioHeapSize", 32);

    LogFile::Printf(1, "Audio heap size: %dMB", audHeapSize);

    mem::write(0x51938D + 1, (int)(audHeapSize * 1000000));

    InstallCallback("AudManager::Init", "Allows the mixer control to be initialized along with the audio manager.",
        &Init, {
            cb::call(0x401F1B),
        }
    );

#ifdef USE_MIXER_STUFF
    InstallCallback("AudManager::AssignCDVolume", "Properly sets mixer volume when changing CD volume.",
        &AssignCDVolume, {
            cb::call(0x401F81), // InitAudioManager
            cb::call(0x50C932), // PUAudioOptions::SetCDVolume
        }
    );

    InstallCallback("AudManager::AssignWaveVolume", "Properly sets mixer volume when changing sound volume.",
        &AssignCDVolume, {
            cb::call(0x401F71), // InitAudioManager
            cb::call(0x50C8FC), // PUAudioOptions::SetWaveVolume
        }
    );

    InstallCallback(&SetupCDAudio, "Allows the mixer volume to be updated when loading player config.", {
            cb::call(0x525DC6), // mmPlayerConfig::SetAudio
        }
    );
#endif

    InstallCallback("mmRaceSpeech::LoadCityInfo", "Non registry dependent minimum install check.",
        &MinInstall, {
            cb::call(0x51AA2C),
        }
    );
}

/*
    pedAnimationInstanceHandler
*/

hook::Type<float> FrameFraction      = 0x6B4724;
hook::Type<int> FrameDelta           = 0x6B4720;

static float pedAnimFrameRate   = 30.0f;
static float pedAnimFPS         = (1000.0f / pedAnimFrameRate);

void pedAnimationInstanceHandler::PreUpdate(float seconds) {
    FrameFraction -= (seconds * -pedAnimFPS);

    float delta = floorf(FrameFraction);

    FrameDelta = (int)delta;
    FrameFraction -= delta;
}

void pedAnimationInstanceHandler::Install() {
    InstallCallback("pedAnimationInstance::PreUpdate", "Allows for more precise control over pedestrian animations.",
        &PreUpdate, {
            cb::call(0x54BF6A),
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

/*
    aiVehicleInstanceHandler
*/

void aiVehicleAmbientHandler::Install() {
    // fixes traffic vehicles using a different color
    // than their respective breakables
    InstallPatch({ 0x90, 0x90, 0x90, 0x90, 0x90 }, {
        0x5513E2,
    });
    
}

/*
    mpConsistencyHandler
*/

// Makes multiplayer AIMAP behavior match singleplayer
void mpConsistencyHandler::Install() {
    // removes mmGame.EnableAIMAP check
    InstallPatch({ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }, {
        0x412ACB,
    });

    // removes mmMultiRoam second AIMAP
    InstallPatch({ 0xEB }, {
        0x427661,
    });

    // removes mmMultiBlitz second AIMAP
    InstallPatch({ 0xEB }, {
        0x420084,
    });

    // removes mmMultiRace second AIMAP
    InstallPatch({ 0xEB }, {
        0x4283A8,
    });

    // removes mmMultiCircuit second AIMAP
    InstallPatch({ 0xEB }, {
        0x421CE4,
    });

    // removes mmMultiCR second AIMAP
    InstallPatch({ 0xEB }, {
        0x4238BE,
    });
}

/*
    vehTrailerHandler
*/

void vehTrailerHandler::Install()
{
    // removes Angels improperly rendered lights
    Installf("Removes default.dgTrailerJoint check preventing trailers from loading properly.");
    mem::nop(0x4D7DCC, 19); // nop out default.dgTrailerJoint load attempt
}

/*
    vehTrailerInstanceHandler
*/

void vehTrailerInstanceHandler::Install()
{
    // removes Angels improperly rendered lights
    InstallPatch({ 0xEB }, {
        0x4D7FBF,
    });
}

/*
    vehPoliceCarAudioBugfixHandler
*/

void vehPoliceCarAudioBugfixHandler::Install() {
    // fixes infinite explosion sounds
    InstallPatch({ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }, {
        0x4D4C28,
    });
}

/*
    asMeshCardInfoHandler
*/

void asMeshCardInfoHandler::Install()
{
    InstallCallback("asMeshCardInfo::Draw", "Scales particles correctly based on current cull mode.",
        &asMeshCardInfo::Draw, {
            cb::jmp(0x461770),
        }
    );
}

/*
    aiVehicleInstanceHandler
*/

void aiVehicleInstanceHandler::Reset()
{
    *getPtr<byte>(this, 0x1A) = 0;
    hook::Thunk<0x552100>::Call<void>(this); // call original
}

void aiVehicleInstanceHandler::Install()
{
    // fixes four ways persisting after level reset
    InstallVTableHook("aiVehicleInstance::DrawGlow",
        &Reset, {
            0x5B590C,
        }
    );
}

/*
    aiGoalAvoidPlayerHandler
*/

void aiGoalAvoidPlayerHandler::Install() {
    // fixes traffic reactions when they avoid the player
    InstallPatch({ 0x90, 0x90, 0x90 }, {
        0x56B235,
    });
}

/*
    aiRouteRacerHandler
*/

void aiRouteRacerHandler::Update() {
    auto opponent = reinterpret_cast<aiRouteRacer*>(this);

    if (opponent->Finished())
        *getPtr<int>(this, 0x27C) = 3;

    //call original
    hook::Thunk<0x53D3B0>::Call<void>(this);
}

void aiRouteRacerHandler::Install() {
    InstallCallback("aiRouteRacer::Update", "Fixes opponents fight each other for their spots at the finish line",
        &Update, {
            cb::call(0x53705B), // aiMap::Update
        }
    );
}

/*
    modShaderHandler
*/

static ConfigValue<bool> cfgMm1StyleRefl("MM1StyleReflections", false);

float lastFogStart;
float lastFogEnd;

void modShaderHandler::BeginEnvMap(gfxTexture * a1, const Matrix34 * a2)
{
    // Set fog distance so it's not blended with reflections
    lastFogStart = (&RSTATE->Data)->FogStart;
    lastFogEnd = (&RSTATE->Data)->FogEnd;
    (&RSTATE->Data)->FogStart = 9999;
    (&RSTATE->Data)->FogEnd = 10000;
        
    hook::StaticThunk<0x4A41B0>::Call<void>(a1, a2); //call original
}

void modShaderHandler::EndEnvMap()
{
    // Restore last fog settings
    (&RSTATE->Data)->FogStart = lastFogStart;
    (&RSTATE->Data)->FogEnd = lastFogEnd;

    hook::StaticThunk<0x4A4420>::Call<void>(); //call original
}

void modShaderHandler::Install()
{
    InstallCallback("modShader::BeginEnvMap", "Turns off fog while drawing reflections.",
        &BeginEnvMap, {
            cb::call(0x4CE1F5),
            cb::call(0x5341DD),
            cb::call(0x552252),
        }
    );

    InstallCallback("modShader::EndEnvMap", "Turns off fog while drawing reflections.",
        &EndEnvMap, {
            cb::call(0x4CE228),
            cb::call(0x534202),
            cb::call(0x55226B),
        }
    );

    if (cfgMm1StyleRefl.Get()) {
        // changes the way reflections are rendered, similar to MM1
        InstallPatch({ 0x03 }, {
            (0x4A4243 + 0x03),
        });
    }
}

/*
    mmDashViewBugfixHandler
*/

void mmDashViewBugfixHandler::Init(char *basename, mmPlayer *a2)
{
    //check if dashboard model is missing
    string_buf<80> buffer("%s_dash", basename);
    if (!datAssetManager::Exists("geometry", buffer, "pkg"))
        basename = "vpcaddie"; //set vpcaddie's dash as a default dashboard

    //call original
    hook::Thunk<0x430890>::Call<void>(this, basename, a2);
}

void mmDashViewBugfixHandler::Install()
{
    if (cfgMissingDashboardFix.Get()) {
        InstallCallback("mmDashView::Init", "Fixes missing dashboard",
            &Init, {
                cb::call(0x42D60B),
            }
        );
    }
}

/*
    mmPlayerBugfixHandler
*/

void mmPlayerBugfixHandler::Ctor()
{
    //clean up our memory
    memset(this, 0x00, 0x23A4);

    //call ctor original
    hook::Thunk<0x4033D0>::Call<void>(this);
}

void mmPlayerBugfixHandler::Install()
{
    InstallCallback("mmPlayer::mmPlayer", "Clean up player memory on ctor.",
        &Ctor, {
            cb::call(0x415D79),
            cb::call(0x41AE89),
            cb::call(0x41C6E9),
            cb::call(0x41E169),
            cb::call(0x41FAD9),
            cb::call(0x420169),
            cb::call(0x421E09),
            cb::call(0x423A2E),
            cb::call(0x427739),
            cb::call(0x428469),
        }
    );
}

/* 
    phBoundBugfixHandler
*/
void phBoundBugfixHandler::CalculateSphereFromBoundingBox()
{
    //ptrs
    float*  centerXPtr = getPtr<float>(this, 0x28);
    float*  centerYPtr = getPtr<float>(this, 0x2C);
    float*  centerZPtr = getPtr<float>(this, 0x30);
    float*  bboxMinXPtr = getPtr<float>(this, 0x0C);
    float*  bboxMinYPtr = getPtr<float>(this, 0x10);
    float*  bboxMinZPtr = getPtr<float>(this, 0x14);
    float*  bboxMaxXPtr = getPtr<float>(this, 0x18);
    float*  bboxMaxYPtr = getPtr<float>(this, 0x1C);
    float*  bboxMaxZPtr = getPtr<float>(this, 0x20);

    //set center
    *centerXPtr = (*bboxMaxXPtr - *bboxMinXPtr) * 0.5f + *bboxMinXPtr;
    *centerYPtr = (*bboxMaxYPtr - *bboxMinYPtr) * 0.5f + *bboxMinYPtr;
    *centerZPtr = (*bboxMaxZPtr - *bboxMinZPtr) * 0.5f + *bboxMinZPtr;

    //set offset flag
    if (*centerXPtr != 0.0f || *centerYPtr != 0.0f || *centerZPtr != 0.0f) {
        BOOL* offsetFlagPtr = getPtr<BOOL>(this, 0x24);
        *offsetFlagPtr = TRUE;
    }

    //calculate sphere
    float sizeX = fmaxf(fabsf(*centerXPtr - *bboxMaxXPtr), fabsf(*centerXPtr - *bboxMinXPtr));
    float sizeY = fmaxf(fabsf(*centerYPtr - *bboxMaxYPtr), fabsf(*centerYPtr - *bboxMinYPtr));
    float sizeZ = fmaxf(fabsf(*centerZPtr - *bboxMaxZPtr), fabsf(*centerZPtr - *bboxMinZPtr));

    //set original size multiplied by the value needed to *fully enclose* the bound in the worst case situations
    float* boundingSpherePtr = getPtr<float>(this, 0x34);
    *boundingSpherePtr = sqrtf(sizeZ * sizeZ + sizeY * sizeY + sizeX * sizeX) * 1.412f;
}

void phBoundBugfixHandler::Install()
{
    if (cfgBoundSphereCalculation) {
        InstallCallback("phBound::CalculateSphereFromBoundingBox", "Fix bound sphere calculations causing bad collisions.",
            &CalculateSphereFromBoundingBox, {
                cb::call(0x480CCC),
                cb::call(0x4842BA),
                cb::call(0x486C31),
                cb::call(0x48712C),
            }
        );
    }
}


/*
    fxShardManagerBugfixHandler
*/
void fxShardManagerBugfixHandler::Install()
{
    InstallCallback("fxShardManager::Drraw", "Fix crashes with cars with low material count.",
        &fxShardManager::Draw, {
            cb::jmp(0x4602D0)
        }
    );
}

/*
    vehSemiCarAudioBugfixHandler
*/
bool canPlayAirBlowSound = false;
void vehSemiCarAudioBugfixHandler::UpdateAirBlow() 
{
    auto carsim = *getPtr<vehCarSim*>(this, 0x118);
    
    //only do this sound for player vehicles
    //if done on all vehicles, cheaters in multiplayer
    //will just absolutely SPAM the airbrake sound
    auto player = mmGameManager::Instance.get()->getPlayerSafe();
    if (player == nullptr)
        return;
    if (player->getCar()->getCarSim() != carsim)
        return;

    float speed = carsim->getSpeedMPH();
    bool isBraking = carsim->getBrake() > 0.1f;

    //if we're going >5mph, set the sound flag
    if (speed > 5.f) {
        canPlayAirBlowSound = true;
    }

    //if we're going <1mph, play the sound
    if (speed < 1.f) {
        if (isBraking && canPlayAirBlowSound) {
            auto soundBase = *getPtr<AudSoundBase*>(this, 0x13C);
            if (soundBase != nullptr)
                soundBase->PlayOnce(-1.f, -1.f);
        }
        canPlayAirBlowSound = false;
    }
}

void vehSemiCarAudioBugfixHandler::Init(MM2::vehCarSim * carsim, MM2::vehCarDamage * cardamage, char * basename, bool a5, bool a6, bool a7)
{
    //set some things in the semi audio class
    *getPtr<int>(this, 0x138) = 0;
    *getPtr<int>(this, 0x13C) = 0;
    *getPtr<float>(this, 0x130) = 0.87f;
    *getPtr<float>(this, 0x134) = 0.9f;
    *getPtr<int>(this, 0x140) = 0xFFFFFFFF;
    *getPtr<int>(this, 0x144) = 0xFFFFFFFF;

    //call vehSemiCarAudio::Load, to load airbrake
    string_buf<128> semiDataName("%s_semidata", basename);
    bool semiDataExists = datAssetManager::Exists("aud\\cardata\\shared", (LPCSTR)semiDataName, "csv");

    //if custom semi data exists, load that. Otherwise load default.
    if (semiDataExists) {
        Warningf("Loading custom semi data: %s", (LPCSTR)semiDataName);
        hook::Thunk<0x4DCD70>::Call<void>(this, (LPCSTR)semiDataName);
    }
    else {
        Warningf("Loading DEFAULT semi data, can't find custom semi data: %s", (LPCSTR)semiDataName);
        hook::Thunk<0x4DCD70>::Call<void>(this, "semidata");
    }
    
    //call back to vehCarAudio::Init
    hook::Thunk<0x4DB900>::Call<void>(this, carsim, cardamage, basename, a5, a6, a7);
}

void vehSemiCarAudioBugfixHandler::SetNon3DParams() 
{
    bool isStereo = AudManagerBase::Instance.get()->getIsStereo();
    auto airBrakeSound = *getPtr<AudSoundBase*>(this, 0x13C);
    auto reverseSound = *getPtr<AudSoundBase*>(this, 0x138);
    float airBrakeVolume = *getPtr<float>(this, 0x134);
    float reverseVolume = *getPtr<float>(this, 0x130);  
    float pan = *getPtr<float>(this, 0x7C);

    //setup reverse sound
    if (reverseSound != nullptr) {
        reverseSound->SetVolume(reverseVolume);
        reverseSound->SetFrequency(1.0f);
        if (isStereo)
            reverseSound->SetPan(pan, -1);
    }

    //setup air brake
    if (airBrakeSound != nullptr) {
        airBrakeSound->SetVolume(airBrakeVolume);
        airBrakeSound->SetFrequency(1.0f);
        if (isStereo)
            airBrakeSound->SetPan(pan, -1);
    }
 
    //call back to vehCarAudio::SetNon3DParams
    hook::Thunk<0x4DC240>::Call<void>(this);
}

void vehSemiCarAudioBugfixHandler::Install()
{
    if (!cfgAirbrakeFix.Get())
        return;

    InstallCallback("vehSemiCarAudio::Init", "Allow custom sounds for air brake audio.",
        &Init, {
            cb::call(0x4DC99A)
        }
    );

    InstallCallback("vehSemiCarAudio::Init", "Fix semi air brake audio.",
        &UpdateAirBlow, {
            cb::call(0x4DCB61),
            cb::call(0x4DCB3C)
        }
    );

    InstallVTableHook("vehSemiCarAudio::SetNon3DParams",
        &SetNon3DParams, {
            0x5B31C4
        }
    );
}
