#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

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

class aiPathHandler {
public:
    void UpdatePedestrians();

    static void Install();
};

class vehCarModelHandler {
public:
    static void Install();
};

class vehCarHandler : handler_t {
public:
    void InitCar(LPCSTR vehName, int a2, int a3, bool a4, bool a5);
    void InitCarAudio(LPCSTR name, int a2);
    void InitBound(LPCSTR name, bool a2);
    const MM2::phBound * GetModelBound(int a1);

    static void Install(void);
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

class mmSpeedIndicatorHandler
{
public:
    static void Install();
};

class cityLevelBugfixHandler
{
public:
    void Update();
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
    void PreUpdate(float seconds);

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

class vehTrailerInstanceHandler {
public :
    void DrawGlow();
    void AddGeomHook(const char* pkgName, const char* name, int flags);
    static void Install();
};

class vehPoliceCarAudioBugfixHandler {
public:
    static void Install();
};

class asMeshCardInfoHandler {
public:
    static void Install();
};