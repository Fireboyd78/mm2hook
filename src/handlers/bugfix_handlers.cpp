#include "bugfix_handlers.h"

using namespace MM2;

static init_handler g_bugfix_handlers[] = {
    CreateHandler<aiPathHandler>("aiPath"),
    CreateHandler<aiPedestrianHandler>("aiPedestrian"),
    CreateHandler<aiPoliceForceHandler>("aiPoliceForce"),

    CreateHandler<gfxImageHandler>("gfxImage"),

    CreateHandler<vehCarHandler>("vehCar"),
    CreateHandler<mmSpeedIndicatorHandler>("mmSpeedIndicator"),
};

int numPedUpdateAttempts = 0;

/*
    aiPathHandler
*/

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

void aiPedestrianHandler::Update(void) {
    if (numPedUpdateAttempts < 256) {
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

void aiPoliceForceHandler::Reset(void) {
    // reset number of cops pursuing player
    // fixes incorrect music bug
    vehPoliceCarAudio::iNumCopsPursuingPlayer = 0;

    $::aiPoliceForce::Reset(this);
}

BOOL aiPoliceForceHandler::IsPerpDrivingMadly(vehCar *perpCar) {
    const float speedLimit = 90.0f;

    if (ageHook::Thunk<0x53E2A0>::Call<BOOL>(this, perpCar)) {
        char *vehName = perpCar->getCarDamage()->GetName(); // we can't use vehCarSim because the game forces vpcop to vpmustang99...

        // ignore perp if they're a cop
        if (!ageHook::StaticThunk<0x4D1A70>::Call<bool>(vehName))
        {
            float speed = perpCar->getCarSim()->getSpeed() * 2.2360249f;

            if (speed > speedLimit) {
                LogFile::Format("PERP DETECTED!!! He's doing %.4f over the speed limit (%.4f)!\n", (speed - speedLimit), speedLimit);
                return TRUE;
            }
        }
    }
    
    if (ageHook::Thunk<0x53E390>::Call<BOOL>(this, perpCar)) {
        LogFile::WriteLine("OFFICER INVOLVED COLLISION WITH PERP!");
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

    // obviously doesn't belong in aiPoliceForceHandler, should either move it or make this a generic "PoliceHandler"
    InstallCallback("aiPoliceOfficer::DetectPerpetrator", "Experimenting with making cops a little smarter about chasing people.",
        &IsPerpDrivingMadly, {
            cbHook<CALL>(0x53E057), // aiPoliceOfficer::Fov
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
            cbHook<CALL>(0x401C75),
        }
    );
}

/*
    vehCarHandler
*/

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
    if (datArgParser::Get("vehicleDebug")) {
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
    mmSpeedIndicator
*/

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