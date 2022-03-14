#include <modules\ai.h>
using namespace MM2;

/*
    aiMap
*/
declfield(aiMap::Instance)(0x6B2E10);
declfield(aiMap::_fSubwayUpdate)(0x6B2DEC);
declfield(aiMap::_fCableCarUpdate)(0x6B2DF0);
declfield(aiMap::_fCTFOppUpdate)(0x6B2DF4);
declfield(aiMap::_fPedUpdate)(0x6B2DF8);
declfield(aiMap::_fAmbientUpdate)(0x6B2DFC);
declfield(aiMap::_fCopUpdate)(0x6B2E00);
declfield(aiMap::_nPedQty)(0x6B2E08);
declfield(aiMap::_fOppUpdate)(0x6B2FE4);
declfield(aiMap::_nAmbientQty)(0x6B2FE8);
declfield(aiMap::_fTotUpdate)(0x6B2FF4);

/*
    aiVehicleManager
*/
declfield(aiVehicleManager::Instance)(0x6B31AC);
declfield(aiVehicleManager::SignalClock)(0x6B31F4);

/*
    aiPoliceOfficer
*/
float aiPoliceOfficer::DefaultSpeedLimit = 12.25f;
float aiPoliceOfficer::SpeedLimitTolerance = 1.125f;
float aiPoliceOfficer::SkidPlayTime = 0.f;
float aiPoliceOfficer::HornPlayTime = 0.f;
bool aiPoliceOfficer::FlyingCopFix = true;
int aiPoliceOfficer::MaximumNumCops = 3;

/*
    aiVehicleInstance
*/
int aiVehicleInstance::AmbientHeadlightStyle = 0;