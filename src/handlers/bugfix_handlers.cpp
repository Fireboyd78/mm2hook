#include "bugfix_handlers.h"

using namespace MM2;

int numPedUpdateAttempts = 0;

//--AIPATHHANDLER--
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



//--AIPEDESTRIANHANDLER--
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


//--AIPOLICEFORCEHANDLER--
void aiPoliceForceHandler::Reset(void) {
    // reset number of cops pursuing player
    // fixes incorrect music bug
    vehPoliceCarAudio::iNumCopsPursuingPlayer = 0;

    $::aiPoliceForce::Reset(this);
}

void aiPoliceForceHandler::Install() {
    InstallCallback("aiPoliceForce::Reset", "Resets the number of cops pursuing the player upon reset.",
        &Reset, {
            cbHook<CALL>(0x536AAE),
            cbHook<CALL>(0x550ECA),
        }
    );
}
\
//-GFXIMAGEHANDLER
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


