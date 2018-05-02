#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

static bool g_bConsoleOpen;

static ConfigValue<bool> cfgInstantReplay       ("InstantReplay",           true);
static ConfigValue<bool> cfgUseAllParkedCars    ("UseAllParkedCars",        true);
static ConfigValue<bool> cfgUseAllTrafficColors ("UseAllTrafficColors",     true);

class asCullManagerHandler {
public:
    void Init(int maxCullables, int maxCullables2D);

    static void Install();
};

class cityLevelHandler {
public:
    void DrawRooms(const LPVOID viewport, unsigned int p2, LPVOID roomRecs, int numRooms);
    void PostUpdate();
    void SetObjectDetail(int lod);

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

class mmDashViewHandler {
public:
    static float TachMinLimit;
    
    void UpdateCS();
    void FileIO(MM2::datParser* parser);

    static void Install();
};

class StreamHandler {
public:
    static MM2::Stream * Open(const char *filename, bool readOnly);

    static void Install();
};

class TextureVariantHandler {
public:
    static MM2::gfxImage * LoadTextureVariant(const char *textureName, bool mipmaps);
    static MM2::gfxImage * PrepareTextureVariant(const char *textureName, bool mipmaps);

    static void InstallTextureVariantHandler();

    static void Install();
};

class PUMainHandler {
public:
    static void Install();
};