#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"
#include "..\modules\effects.h"

static ConfigValue<bool> cfgBoundSphereCalculation  ("BoundSphereCalculationFix", true);
static ConfigValue<bool> cfgAirbrakeFix             ("AirbrakeFix",               true);
static ConfigValue<bool> cfgMissingDashboardFix     ("MissingDashboardFix",       true);

class gfxImageHandler
{
public:
    void Scale(int width, int height);

    static void Install();
};

class aiPedestrianHandler
{
public:
    void Update();

    static void Install();
};

class aiPoliceForceHandler {
public:
    void Reset();
    BOOL IsPerpDrivingMadly(MM2::vehCar *perpCar);

    static void Install();
};

class aiPoliceOfficerHandler {
public:
    void PerpEscapes(bool a1);

    static void Install();
};

class aiPathHandler {
public:
    void UpdatePedestrians();

    static void Install();
};

class vehCarBugfixHandler {
public:
    void Update();
    static void Install();
};

class vehCarModelHandler {
public:
    static void Install();
};

class vehCarAudioHandler {
public:
    bool IsAirBorne();
    void Update();
    void Reset();

    static void Install();
};

class vehCarAudioContainerBugfixHandler : handler_t {
public:
    void StartSiren();
    void StopSiren();

    static void Install(void);
};

class mmBillInstanceHandler
{
public:
    void Scale(float x, float y, float z);
    static void Install();
};

class mmGearIndicatorHandler
{
public:
    void Draw();
    static void Install();
};

class mmSpeedIndicatorHandler
{
public:
    static void Install();
};

class cityLevelBugfixHandler
{
public:
    static bool IsMirrorDrawing;
    static void UpdateRainParticles();
    static MM2::Stream* OpenPvsStream(const char * folder, const char * file, const char * extension, bool a4, bool a5);
    static void Install();
};

class mmInterfaceHandler {
public:
    void PlayerResolveCars();
    static void Install();
};

class lvlSkyHandler {
public:
    void ResetRot();
    static void Install();
};

class BugfixPatchHandler {
public:
    static void Install();
};

class mmHudMapHandler {
public:
    bool CanActivateMap();
    void Activate();
    void SetMapMode(int mode);
    static void Install();
};

class mmPopupHandler {
public:
    void HudEnable();
    void HudDisable(int a1);
    static void Install();
};

class mmMultiCRHandler {
public:
    bool LoadMusic(char* a1, char* a2);
    static void Install();
};

class audManagerHandler {
public:
    void Init(int, int, int, char *, short, short);
    
    void AssignCDVolume(float value);
    void SetupCDAudio(float);

    void SetMixerCDVolume(float value);
    void SetMixerWaveVolume(float value);

    bool MinInstall();

    static void Install();
};

class pedAnimationInstanceHandler {
public:
    void aiMapUpdate();

    static void PreUpdate(float seconds);

    static void Install();
};

class mmCDPlayerHandler {
public:
    static void Install();
};

class aiVehicleAmbientHandler {
public:
    static void Install();
};

class mpConsistencyHandler {
public:
    static void Install();
};

class vehTrailerHandler {
public:
    static void Install();
};

class vehTrailerInstanceHandler {
public:
    static void Install();
};

class vehPoliceCarAudioBugfixHandler {
public:
    static void Install();
};

class vehSemiCarAudioBugfixHandler {
public:
    void SetNon3DParams();
    void UpdateAirBlow();
    void UpdateReverse();
    void Init(MM2::vehCarSim* carsim, MM2::vehCarDamage* cardamage, char* basename, bool a5, bool a6, bool a7);
    static void Install();
};

class asMeshCardInfoHandler {
public:
    static void Install();
};

class aiVehicleInstanceHandler {
public:
    void Reset();
    static void Install();
};

class aiGoalAvoidPlayerHandler {
public:
    static void Install();
};

class aiRouteRacerHandler {
public:
    void Update();
    static void Install();
};

class modShaderHandler {
public:
    static void EndEnvMap();
    static void BeginEnvMap(MM2::gfxTexture* a1, const MM2::Matrix34* a2);
    static void Install();
};

class mmMirrorHandler {
public:
    void Cull();
    static void Install();
};

class mmDashViewBugfixHandler {
public:
    void Init(char *basename, MM2::mmPlayer *a2);
    static void Install();
};

class mmPlayerBugfixHandler {
public:
    void Ctor();
    static void Install();
};

class phBoundBugfixHandler {
public:
    void CalculateSphereFromBoundingBox();
    static void Install();
};

class fxShardManagerBugfixHandler {
public:
    static void Install();
};