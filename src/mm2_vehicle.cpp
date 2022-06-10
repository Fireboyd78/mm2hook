#include "mm2_vehicle.h"
using namespace MM2;

/*
    vehPoliceCarAudio
*/
declfield(vehPoliceCarAudio::iNumCopsPursuingPlayer)(0x6AFFE4);

/*
    vehCar
*/
declfield(vehCar::sm_DrawHeadlights)(0x627518);

/*
    vehCarDamage
*/
declfield(vehCarDamage::EngineSmokeRule)(0x6AFED4);

/*
    vehWheel
*/
declfield(vehWheel::WeatherFriction)(0x5CF6B8);

/*
    vehCarModel
*/
bool vehCarModel::EnableSpinningWheels = true;
bool vehCarModel::EnableHeadlightFlashing = true;
bool vehCarModel::EnableLEDSiren = false;
bool vehCarModel::MWStyleTotaledCar = false;
bool vehCarModel::PartReflections = false;
bool vehCarModel::WheelReflections = false;
bool vehCarModel::MM1StyleTransmission = true;
bool vehCarModel::BreakableRenderTweak = false;
bool vehCarModel::WheelWobble = true;
bool vehCarModel::MM1StyleWobble = false;
int vehCarModel::SirenType = 0;
int vehCarModel::HeadlightType = 0;
float vehCarModel::SirenCycle = 0.25f;
float vehCarModel::HeadlightFlashingSpeed = 42.411503f;

/*
    vehBreakableMgr
*/
bool vehBreakableMgr::EnableReflections = false;

/*
    vehSiren
*/
float vehSiren::SirenRotationSpeed = 3.1415927f;
float vehSiren::SirenRotationStyle = 1.5707964f;