#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

#include <vector>

#include "..\events\dispatcher.h"

class asCullManagerHandler {
public:
    void Init(int maxCullables, int maxCullables2D);

    static void Install();
};

class cityLevelHandler {
public:
    void DrawRooms(const MM2::gfxViewport* viewport, unsigned int p2, LPVOID roomRecs, int numRooms);
    void PostUpdate();
    void SetObjectDetail(int lod);

    static void Install();
};

class cityTimeWeatherLightingHandler {
public:
    static void Reset();

    static void LoadCityTimeWeatherLighting();

    void FileIO(MM2::datParser &parser);

    static void Install();
};

class gfxPipelineHandler {
public:
    static bool HandleKeyPress(DWORD vKey);

    static void gfxApplySettings();
    static LRESULT APIENTRY gfxWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static void gfxWindowMove(bool isOpen);
    static void gfxWindowUpdate(bool isOpen);
    static void SetRes(int width, int height, int cdepth, int zdepth, bool parseArgs);
    static void gfxWindowCreate(LPCSTR lpWindowName);
    static void gfxSetTexReduceSize(int);

    static void Install();
};

class vglHandler {
public:
    static void vglBeginShaded(MM2::gfxDrawMode drawMode, int p1);
    static void vglEndShaded();

    static void Install();
};

class Aud3DObjectManagerHandler {
public:
    void InitAmbObjContainer(LPCSTR name);

    static void Install();
};

class mmGameMusicDataHandler {
public:
    bool LoadAmbientSFX(LPCSTR name);

    static void Install();
};

class vehCarAudioContainerHandler {
public:
    static void SetSirenCSVName(LPCSTR name);

    static void Install();
};

class vehPoliceCarAudioHandler {
public:
    void InitSirenAudio(MM2::vehCarSim *a1, MM2::vehCarDamage *a2, LPCSTR basename, LPCSTR sirenCsvFile, bool a5);
    void Reset();
    static void Install();
};

class gizFerryHandler {
public:
    void SetSpeed(float value);

    static void Install();
};

class gizParkedCarMgrHandler {
public:
    static void EnumeratePath(LPCSTR a1, const MM2::Matrix34* a2, bool a3);
    static void Install();
};

class datCallbackExtensionHandler {
public:
    static void Install();
};

class sdlPage16Handler {
public:
    static LPVOID blockPtr;
    static LPVOID attributePtr;

    void Draw(int p1, unsigned int p2);

    // this MUST clean up the stack, hence the stdcall
    static void __stdcall SetAttributePointer(LPVOID lpBlock);
    static void InvalidCmd(LPCSTR, int attr, int subtype);

    static void Install();
};

class lvlHandler {
public:
    void EnumerateSDL(int p1, MM2::SDLIteratorCB iter, void *context);

    static void SetAIRoad(const MM2::lvlSDL * lvlSDL, int road, bool p3);
    static int GetAIRoom(int room);
    static void InvalidCommand(int cmd, void *attrPtr, void *roadPtr);
    static void Install();
};

class memSafeHeapHandler {
public:
    void Init(void *memAllocator, unsigned int heapSize, bool p3, bool p4, bool checkAlloc);

    static void Install();
};

class mmGameHandler : handler_t {
public:
    void SendChatMessage(char *message);
    void InitWeather(void);
    void UpdateSteeringBrakes(void);
    void UpdateHorn(bool a1);
    void LaunchProp();

    static void Install();
};

class mmDirSndHandler {
public:
    static MM2::mmDirSnd* Init(ulong sampleRate, bool enableStero, int p3, int volume, char *deviceName, short p6, short p7);

    static void Install();
};

class BridgeFerryHandler {
public:
    void Cull(int lod);
    void Draw(int lod);

    static void Install();
};

class mmHudMapFeatureHandler {
public:
    void DrawColoredTri(unsigned int color, const MM2::Matrix34 &matrix);
    void DrawIcon(int iconColor, const MM2::Matrix34 &matrix);
    void DrawPlayer();
    void DrawCops();
    void DrawOpponents();

    static void Install();
};

class mmIconsFeatureHandler {
public:
    void RegisterOpponents(MM2::OppIconInfo *icons, int count, void *font);
    void RegisterOpponentsBlitz(MM2::OppIconInfo *icons, int count, void *font);
    void RegisterOpponentsMulti(MM2::OppIconInfo *icons, int count, void *font);

    static void Install();
};

class mmTextNodeHandler {
public:
    void GetTextDimensionsWhite(void const* a1, char const* a2, float& a3, float& a4);
    void GetTextDimensionsBlack(void const* a1, char const* a2, float& a3, float& a4);
    void AddTextWhite(void const* a1, char const* a2, int a3, float a4, float a5);
    void AddTextBlack(void const* a1, char const* a2, int a3, float a4, float a5);
    static void Install();
};

class mmDashViewHandler {
public:
    static float TachMinLimit;

    void UpdateCS();
    void FileIO(MM2::datParser* parser);
    void Init(char *vehName, MM2::mmPlayer *player);
    void Cull();

    static void Install();
};

class StreamHandler {
public:
    static MM2::Stream * Open(const char *filename, bool readOnly);

    static void Install();
};

class TextureVariantHandler {
public:
    static void InitVariantData();
    static void Reset();

    static MM2::gfxImage * LoadTextureVariant(const char *textureName, bool mipmaps);
    static MM2::gfxImage * PrepareTextureVariant(MM2::gfxImage* image, const char *textureName, bool mipmaps);

    static void InstallTextureVariantHandler();

    static void Install();
};

class PUMainHandler {
public:
    static void Install();
};

class mmPlayerHandler {
public:
    void ControlTrailerJoint();
    void Zoink();
    void Update();
    void Reset();
    void BustPlayer();
    void BustOpp();
    void Cooldown();
    int GetClosestCop();
    int GetClosestOpp();
    static void Install();
};

class mmCityListHandler {
public:
    void Load(char* cinfoName);
    static void Install();
};

class Dialog_NewPlayerHandler {
public:
    static void Install();
};

class ltLensFlareHandler {
public:
    static void Install();
};

class ltLightHandler {
public:
    MM2::ltLight* Constructor();
    static void ShutdownLights();
    static void Install();
};

class mmSingleRaceHandler {
public:
    void QueueCopVoice(float a1);
    void SetPriority(int a1);
    static void Install();
};

class mmSingleBlitzHandler {
public:
    void SetPriority(int a1);
    static void Install();
};

class mmSingleCircuitHandler {
public:
    void SetPriority(int a1);
    static void Install();
};

class mmSingleStuntHandler {
public:
    void SetPriority(int a1);
    static void Install();
};

class mmSingleRoamHandler {
public:
    void ResetToNearestLocation();
    void EscapeDeepWater();
    void HitWaterHandler();
    void SetPriority(int a1);
    static void Install();
};

class dgBangerInstanceHandler {
public:
    void DrawShadow();
    void DrawGlow();
    bool BeginGeom(const char* a1, const char* a2, int a3);
    static void Install();
};

class vehCarHandler : handler_t {
public:
    void Init(LPCSTR vehName, int variant, int colliderId, bool useGeometry, bool loadTrailer);
    void InitAudio(LPCSTR vehName, int vehType);
    const MM2::phBound * GetModelBound(int boundType);

    void MM1StyleTransmission();
    void PlayExplosion();
    void Splash();
    void Update();
    void PreUpdate();
    void Reset();

    static void Install(void);
};

class vehBreakableMgrHandler {
public:
    static void Install();
};

class vehCarModelFeatureHandler {
public:
    static const char* VehNameRemap(const char* basename);
    void Init(MM2::vehCar* car, const char* basename, int variant);
    void InitBound(const char* basename, bool useGeometry);
    void Draw(int a1);
    void DrawShadow();
    void DrawGlow();
    void DrawHeadlights(bool rotate);
    void DrawExtraHeadlights(bool rotate);
    void DrawFoglights();
    void DrawSiren(const MM2::Matrix34& carMatrix);
    void EjectOneShot();
    bool Collide(MM2::lvlSegment& segment, MM2::lvlIntersection* intersection, int roomId, MM2::lvlInstance* ignoreInstance, ushort instanceFlags, int collideFlags, const MM2::Vector3& lightPos);
    static void Install();
};

class vehWheelHandler {
public:
    void Init(MM2::vehCarSim* carSimPtr, const char* vehicleBasename, const char* wheelName, MM2::Vector3 centerOfGravity, MM2::phInertialCS* inertialCs, int wheelCount, int flags);
    void Update();
    static void Install();
};

class fxTexelDamageHandler {
public:
    static void Install();
};

class pedestrianInstanceHandler {
public:
    void Init(char* a1, int a2, int a3);
    void DrawRagdoll();
    void Draw(int a1);
    void DrawShadow();
    bool IsCollidable();
    MM2::phBound * GetBound(int a1);

    void aiMapClean();
    void aiMapInit(char* a1, char* a2, char* a3, const MM2::dgStatePack* a4, int a5, MM2::vehCar* a6, bool a7);

    static void Install();
};

class aiVehicleInstanceFeatureHandler {
public:
    bool InitVehicleGeom(const char* basename, const char* bodyGeom, int bodyFlags);
    bool InitBreakable(const char* basename, const char* breakableName, int geomId, int flags);
    void InitDefaultBreakables(const char* basename, const char* breakableName, int geomId);
    void InitAdditionalBreakables(const char* basename, const char* breakableName, int geomId);
    void DrawPart(MM2::modStatic* model, const MM2::Matrix34& matrix, MM2::modShader* shaders, bool reflected);
    void Reset();
    void Draw(int lod);
    void DrawShadow();
    void DrawGlow();
    void DrawHeadlights();
    bool Collide(MM2::lvlSegment& segment, MM2::lvlIntersection* intersection, int roomId, MM2::lvlInstance* ignoreInstance, ushort instanceFlags, int collideFlags);
    static void Install();
};

class aiVehicleAmbientFeatureHandler {
public:
    void Init(char* vehName, int index);
    static void Install();
};

class vehTrailerFeatureHandler {
public:
    void Update();
    void Reset();
    static void Install();
};

class vehTrailerInstanceFeatureHandler {
public:
    void Draw(int a1);
    void DrawTwhl4(int a1, int a2, MM2::Matrix34& a3, MM2::modShader* a4);
    void DrawTwhl5(int a1, int a2, MM2::Matrix34& a3, MM2::modShader* a4);
    void DrawShadow();
    void DrawGlow();
    void DrawPart(int a1, int a2, const MM2::Matrix34& a3, MM2::modShader* a4);
    void DrawPartReflections(MM2::modStatic* a1, const MM2::Matrix34& a2, MM2::modShader* a3);
    static void Install();
};

struct MultiTireTrackContainer {
public:
    MM2::gfxTexture* TrackTexture;
    MM2::gfxTexture* OffroadTrackTexture;
};

class multiTireTrackHandler {
    void UpdateTrack(void* lvlTrackManagerPtr, void* wheel);
    static void Install();
};

class vehCarSimHandler {
public:
    static void Install();
};

class vehCableCarInstanceHandler {
public:
    void DrawShadow();
    void DrawGlow();
    bool BeginGeom(const char* a1, const char* a2, int a3);
    static void Install();
};

class mmArrowHandler {
public:
    void SetShape(LPCSTR modelName, LPCSTR dirName, bool useLVertex, MM2::Vector3* a4);
    void Update();
    static void Install();
};

class vehSirenHandler {
public:
    void Update();
    void Reset();
    static MM2::vehSiren* OperatorNew(uint size);
    static void Install();
};

class aiPoliceOfficerFeatureHandler {
public:
    BOOL IsPlayerDrivingMadly(MM2::vehCar* perpCar);
    BOOL IsOppDrivingMadly(MM2::vehCar* perpCar);
    BOOL Offroad(MM2::vehCar* perpCar);
    BOOL TireScreeching(MM2::vehCar* perpCar);
    BOOL HornSpamming(MM2::vehCar* perpCar);
    void PerpEscapes(bool a1);
    void DetectPerpetrator();
    void Update();
    static void Install();
};

class vehEngineHandler {
public:
    float CalcTorque(float a1);
    static void Install();
};

class mmExternalViewHandler {
public:
    void Init(MM2::mmPlayer *player);
    void ResChange(int width, int height);
    void Cull();
    void DrawSlidingGauge();
    void DrawGearIndicator();
    void DrawLinearGauge();
    void DrawSpeedIndicator();
    void DrawPursuitBar();
    void DrawCooldownBar();
    static void Install();
};

class MMDMusicManagerHandler {
public:
    void MatchMusicToPlayerSpeed(float speed);
    void UpdateMusic(float speed, int numCops, bool isAirborne);
    static void Install();
};

class MenuManagerHandler {
public:
    void Update();
    static void Install();
};

class camPovCSHandler {
public:
    void UpdatePOV();
    static void Install();
};