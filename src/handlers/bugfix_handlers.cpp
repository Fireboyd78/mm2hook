#include "bugfix_handlers.h"

using namespace MM2;

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
    const float speedLimit = 70.0f;

    if (ageHook::Thunk<0x53E2A0>::Call<BOOL>(this, perpCar)) {
        float speed = perpCar->getCarSim()->getSpeed() * 2.2360249f;
        bool speeding = (speed > speedLimit);

        if (speeding)
            LogFile::Format("PERP DETECTED!!! He's doing %.4f over the speed limit (%.4f)!\n", (speed - speedLimit), speedLimit);

        return speeding;
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