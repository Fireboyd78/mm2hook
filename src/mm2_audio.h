#pragma once
#include "mm2_common.h"
#include "mm2_base.h"

/*
    DirSnd is based on the "Full-Duplex Filter" sample found in the VC98 SDK
    I've archived these files under "src\vc98_sdk\fdfilter" for reference
*/
typedef struct tag_dsdevdesc
{
    GUID    guDevice;            // Device GUID
    PSTR    pszDeviceDesc;        // Description string
    struct tag_dsdevdesc *pNext;
} DSDEVICEDESC, *PDSDEVICEDESC;

typedef struct tag_fd
{
    DWORD   dwCode;
    BOOL    fEnabled;
} FORMATDATA, *PFORMATDATA;

namespace MM2
{
    // Forward declarations
    class DirSnd;
    class mmDirSnd;
    class Aud3DObject;
    class AudCreatureContainer;
    class AudManager;
    class mmCNRSpeech;

    namespace $
    {
        namespace DirSnd
        {
            declhook(0x5A5080, _Func<MM2::DirSnd *>, Init);

            declhook(0x5A4ED0, _MemberFunc<void>, $$ctor);
            declhook(0x5A4F30, _MemberFunc<void>, $$dtor);

            declhook(0x5A4FD0, _MemberFunc<void>, DeInit);

            declhook(0x5A5160, _MemberFunc<int>, Init3DListener);

            declhook(0x5A51C0, _MemberFunc<int>, IsDSDeviceInList);
            declhook(0x5A5390, _MemberFunc<void>, SetDeviceRating);
            declhook(0x5A5510, _MemberFunc<int>, EnumDSDevices);

            declhook(0x5A5530, _MemberFunc<int>, InitPrimarySoundBuffer);
            declhook(0x5A5630, _MemberFunc<int>, CreatePrimaryInterfaceAndBuffer);

            declhook(0x5A5770, _MemberFunc<void>, ClearDSDeviceList);

            declhook(0x5A5860, _MemberFunc<void>, SetPrimaryBufferFormat);

            declhook(0x5A5950, _MemberFunc<void>, ScanAvailableDSFormats);
            declhook(0x5A5A30, _MemberFunc<char *>, TranslateDSError);

            declhook(0x5A5BB0, _MemberFunc<void>, SetBitDepth);
            declhook(0x5A5BD0, _MemberFunc<char **>, GetDeviceNames);
            declhook(0x5A5C00, _MemberFunc<UINT>, GetNum3DHallBufs);
        }
        namespace mmDirSnd
        {
            declhook(0x51CC50, _Func<MM2::mmDirSnd *>, Init);

            declhook(0x51CBE0, _MemberFunc<void>, $$ctor);
            declhook(0x51CC00, _MemberFunc<void>, $$dtor);

            declhook(0x51CC10, _MemberFunc<void>, DeInit);

            declhook(0x51CD90, _MemberFunc<int>, InitPrimarySoundBuffer);

            declhook(0x51CE60, _MemberFunc<UINT>, EAXEnabled);
            declhook(0x51CE70, _MemberFunc<UINT>, DSound3DEnabled);
        }
    }

    class AudSoundBase : public asNode {
    private:
        byte _buffer[0x40];
    private:
        //lua helper
        bool LoadLua(LPCSTR wavName, int handle) {
            return Load(wavName, handle, false) != FALSE;
        }
    public:
        AGE_API AudSoundBase() {
            scoped_vtable x(this);
            ageHook::Thunk<0x50D580>::Call<void>(this);
        };

        AGE_API AudSoundBase(unsigned int flags, int soundHandleCount, int unused) {
            scoped_vtable x(this);
            ageHook::Thunk<0x50D6D0>::Call<void>(this, flags, soundHandleCount, unused);
        };

        AGE_API ~AudSoundBase() {
            Warningf("audSoundBase DTOR");
            scoped_vtable x(this);
            ageHook::Thunk<0x50D7B0>::Call<void>(this);
        };

        /*
            asNode virtuals
        */
        AGE_API void Update() override                      { ageHook::Thunk<0x50DBE0>::Call<void>(this); }

        /*
            AudSoundBase
        */
        AGE_API static unsigned int Get3DFlags()            { return ageHook::StaticThunk<0x50E070>::Call<unsigned int>(); }
        AGE_API static unsigned int Get2DFlags()            { return ageHook::StaticThunk<0x50E030>::Call<unsigned int>(); }
        AGE_API static unsigned int GetSoft2DFlags()        { return ageHook::StaticThunk<0x50E040>::Call<unsigned int>(); }

        AGE_API bool IsPlaying()                            { return ageHook::Thunk<0x50E250>::Call<bool>(this); }
        AGE_API void Enable3DMode()                         { ageHook::Thunk<0x50E2F0>::Call<void>(this); }
        AGE_API void Disable3DMode()                        { ageHook::Thunk<0x50E300>::Call<void>(this); }
        AGE_API BOOL Load(LPCSTR wavName, int handle, bool unknown)
                                                            { return ageHook::Thunk<0x50DE90>::Call<BOOL>(this, wavName, handle, unknown); }
        AGE_API void PlayOnce(float volume, float pitch)    { ageHook::Thunk<0x50E090>::Call<void>(this, volume, pitch); }
        AGE_API void PlayLoop(float volume, float pitch)    { ageHook::Thunk<0x50E150>::Call<void>(this, volume, pitch); }
        AGE_API void Stop()                                 { ageHook::Thunk<0x50E1F0>::Call<void>(this); }
        AGE_API void SetFrequency(float frequency)          { ageHook::Thunk<0x50DAB0>::Call<void>(this, frequency); }
        AGE_API void SetVolume(float volume)                { ageHook::Thunk<0x50DA30>::Call<void>(this, volume); }
        AGE_API void SetPan(float pan, int a2)              { ageHook::Thunk<0x50DB30>::Call<void>(this, pan, a2); }
        AGE_API void SetSoundHandleIndex(int index)         { ageHook::Thunk<0x50E2C0>::Call<void>(this, index); }
        AGE_API void SetSubPath(LPCSTR path)                { ageHook::Thunk<0x50D8D0>::Call<void>(this, path); }
        AGE_API void SetPlayPosition(unsigned int position)
                                                            { ageHook::Thunk<0x50E2E0>::Call<void>(this, position); }
        AGE_API int GetSoundHandleIndex()                   { return ageHook::Thunk<0x50E2D0>::Call<int>(this); }
        AGE_API int GetNumSoundHandles()                    { return ageHook::Thunk<0x50E310>::Call<int>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<AudSoundBase, asNode>("AudSoundBase")
                .addFactory([](unsigned int flags = 0xC2, int soundHandleCount = 1) { //0xC2 is the return value of Get2DFlags
                    return new AudSoundBase(flags, soundHandleCount, -1);
                    }, LUA_ARGS(_opt<unsigned int>, _opt<int>))

                .addStaticFunction("Get3DFlags", &Get3DFlags)
                .addStaticFunction("Get2DFlags", &Get2DFlags)
                .addStaticFunction("GetSoft2DFlags", &GetSoft2DFlags)
        
                .addProperty("NumSoundHandles", &GetNumSoundHandles)
                .addProperty("IsPlaying", &IsPlaying)
                .addProperty("SoundHandleIndex", &GetSoundHandleIndex, &SetSoundHandleIndex)

                .addFunction("Load", &LoadLua)

                .addFunction("Enable3DMode", &Enable3DMode)
                .addFunction("Disable3DMode", &Disable3DMode)

                .addFunction("PlayOnce", &PlayOnce)
                .addFunction("PlayLoop", &PlayLoop)
                .addFunction("SetFrequency", &SetFrequency)
                .addFunction("SetPan", &SetPan)
                .addFunction("SetVolume", &SetVolume)
                .addFunction("SetSubPath", &SetSubPath)
                .endClass();
        }
    };

    class Aud3DObject {

    };

    class AudCreatureContainer : public Aud3DObject {
    public:
        AGE_API void PlayImpactReaction(float a1)            { ageHook::Thunk<0x510CC0>::Call<void>(this, a1); }
        AGE_API void PlayAvoidanceReaction(float a1)         { ageHook::Thunk<0x510CF0>::Call<void>(this, a1); }
    };

    class DirSnd {
    protected:
        /*0x4*/
        LPDIRECTSOUNDBUFFER lpDSBuffer;
        LPDIRECTSOUND lpDS;

        HWND hWnd;

        /*0x10*/
        int unk_10;

        PDSDEVICEDESC pNextDevice, pCurrentDevice;

        // not 100% sure on this one, but it does make sense
        // DirSnd::~DirSnd() will try to release this if it's not null
        // but it never gets set to anything, and the 3d listener stuff is empty
        LPDIRECTSOUND3DLISTENER lpDS3DListener;

        /*0x20*/
        int deviceCount;

        int bitDepth;
        int flags;

        bool is3DEnabled; // did the 3D listener get created?
        bool isInitialized; // was Init called?

        /*0x30*/
        DWORD isSoundEnabled; // did the primary sound buffer get created?

        int deviceCaps;

        PDSDEVICEDESC *ppSoundDevices;

        AGE_API int CreatePrimaryInterfaceAndBuffer(DSBUFFERDESC *p1) {
            return $::DirSnd::CreatePrimaryInterfaceAndBuffer(this, p1);
        };
    public:
        AGE_API DirSnd() {
            scoped_vtable x(this);
            $::DirSnd::$$ctor(this);
        };

        AGE_API ~DirSnd() {
            scoped_vtable x(this);
            $::DirSnd::$$dtor(this);
        };

        AGE_API static DirSnd * Init(DWORD sampleRate, bool enableStero, bool enable3D, char *deviceName) {
            return $::DirSnd::Init(sampleRate, enableStero, enable3D, deviceName);
        };

        AGE_API int Init3DListener(float p1, float p2, float p3, float p4, float p5) {
            return $::DirSnd::Init3DListener(this, p1, p2, p3, p4, p5);
        };

        AGE_API int IsDSDeviceInList(GUID *guid) {
            return $::DirSnd::IsDSDeviceInList(this, guid);
        };

        AGE_API void DeInit(void) {
            $::DirSnd::DeInit(this);
        };

        AGE_API void SetDeviceRating(DSDEVICEDESC *p1) {
            $::DirSnd::SetDeviceRating(this, p1);
        };

        AGE_API int EnumDSDevices(void) {
            return $::DirSnd::EnumDSDevices(this);
        };

        AGE_API void ClearDSDeviceList(void) {
            $::DirSnd::ClearDSDeviceList(this);
        };

        AGE_API void SetPrimaryBufferFormat(DWORD p1, BYTE p2) {
            $::DirSnd::SetPrimaryBufferFormat(this, p1, p2);
        };

        AGE_API void ScanAvailableDSFormats(void) {
            $::DirSnd::ScanAvailableDSFormats(this);
        };

        AGE_API char * TranslateDSError(long p1) {
            return $::DirSnd::TranslateDSError(this, p1);
        };

        AGE_API void SetBitDepth(int p1) {
            $::DirSnd::SetBitDepth(this, p1);
        };

        AGE_API char ** GetDeviceNames(void) {
            return $::DirSnd::GetDeviceNames(this);
        };

        AGE_API UINT GetNum3DHallBufs(void) {
            return $::DirSnd::GetNum3DHallBufs(this);
        };

        virtual AGE_API int InitPrimarySoundBuffer(DWORD p1, BYTE p2, char *p3) {
            return $::DirSnd::InitPrimarySoundBuffer(this, p1, p2, p3);
        };
    };

    class mmDirSnd : public DirSnd {
    protected:
        /*0x3C*/
        UINT eaxEnabled;
        UINT dsound3DEnabled;
    public:
        AGE_API mmDirSnd() {
            scoped_vtable x(this);
            $::mmDirSnd::$$ctor(this);
        };

        AGE_API ~mmDirSnd() {
            scoped_vtable x(this);
            $::mmDirSnd::$$dtor(this);
        };

        AGE_API static mmDirSnd * Init(ulong sampleRate, bool enableStero, int p3, int volume, char *deviceName, short p6, short p7) {
            return $::mmDirSnd::Init(sampleRate, enableStero, p3, volume, deviceName, p6, p7);
        };

        AGE_API void DeInit(short p1, short p2) {
            $::mmDirSnd::DeInit(this, p1, p2);
        };

        AGE_API UINT EAXEnabled(void) {
            return $::mmDirSnd::EAXEnabled(this);
        };

        AGE_API UINT DSound3DEnabled(void) {
            return $::mmDirSnd::DSound3DEnabled(this);
        };

        virtual AGE_API int InitPrimarySoundBuffer(DWORD p1, BYTE p2, char *p3) {
            return $::mmDirSnd::InitPrimarySoundBuffer(this, p1, p2, p3);
        };
    };

    class AudImpact {
    public:
        AGE_API void Play(float a1, int a2) {
            ageHook::Thunk<0x511860>::Call<void>(this, a1, a2);
        }
    };

    class mmCNRSpeech {
    public:
        AGE_API void Play(LPCSTR a1)                         { ageHook::Thunk<0x5A7800>::Call<void>(this, a1); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<mmCNRSpeech>("mmCNRSpeech")
                .addFunction("Play", &Play)
            .endClass();
        }
    };

    class mmRaceSpeech {
    public:
        AGE_API void PlayUnlockVehicle()                     { ageHook::Thunk<0x51A6C0>::Call<void>(this); }
        AGE_API void PlayUnlockTexture()                     { ageHook::Thunk<0x51A720>::Call<void>(this); }
        AGE_API void PlayUnlockRace()                        { ageHook::Thunk<0x51A660>::Call<void>(this); }
        AGE_API void PlayResultsWin()                        { ageHook::Thunk<0x51A890>::Call<void>(this); }
        AGE_API void PlayResultsPoor()                       { ageHook::Thunk<0x51A840>::Call<void>(this); }
        AGE_API void PlayResultsMid()                        { ageHook::Thunk<0x51A8E0>::Call<void>(this); }
        AGE_API void PlayResults(int a1, int a2)             { ageHook::Thunk<0x51A800>::Call<void>(this, a1, a2); }
        AGE_API void PlayRaceProgress()                      { ageHook::Thunk<0x51A7D0>::Call<void>(this); }
        AGE_API void PlayPreRace()                           { ageHook::Thunk<0x51A590>::Call<void>(this); }
        AGE_API void PlayFinalLap()                          { ageHook::Thunk<0x51A780>::Call<void>(this); }
        AGE_API void PlayFinalCheckPoint()                   { ageHook::Thunk<0x51A750>::Call<void>(this); }
        AGE_API void PlayDamagePenalty()                     { ageHook::Thunk<0x51A7A0>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<mmRaceSpeech>("mmRaceSpeech")
                .addFunction("PlayUnlockVehicle", &PlayUnlockVehicle)
                .addFunction("PlayUnlockTexture", &PlayUnlockTexture)
                .addFunction("PlayUnlockRace", &PlayUnlockRace)
                .addFunction("PlayResultsWin", &PlayResultsWin)
                .addFunction("PlayResultsPoor", &PlayResultsPoor)
                .addFunction("PlayResultsMid", &PlayResultsMid)
                .addFunction("PlayResults", &PlayResults)
                .addFunction("PlayRaceProgress", &PlayRaceProgress)
                .addFunction("PlayPreRace", &PlayPreRace)
                .addFunction("PlayFinalLap", &PlayFinalLap)
                .addFunction("PlayFinalCheckPoint", &PlayFinalCheckPoint)
                .addFunction("PlayDamagePenalty", &PlayDamagePenalty)
            .endClass();
        }
    };

    class AudManagerBase : public asNode {
    public:
        AGE_API AudManagerBase() {
            scoped_vtable x(this);
            ageHook::Thunk<0x50EE10>::Call<void>(this);
        };

        AGE_API ~AudManagerBase() {
            scoped_vtable x(this);
            ageHook::Thunk<0x50EE40>::Call<void>(this);
        };

        //instance
        static ageHook::Type<AudManagerBase*> Instance;

        //members
        AGE_API BOOL IsStereo()                         { return ageHook::Thunk<0x50F0D0>::Call<BOOL>(this); }

        //asNode overrides
        AGE_API virtual void Update() override          { ageHook::Thunk<0x50F130>::Call<void>(this); }
        AGE_API virtual void UpdatePaused() override    { ageHook::Thunk<0x50F1A0>::Call<void>(this); }

        //lua helpers
        inline bool getIsStereo() {
            return IsStereo() == TRUE;
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<AudManagerBase, asNode>("AudManagerBase")
                .addStaticFunction("Instance", [] { return (AudManagerBase*)Instance; })

                .addPropertyReadOnly("IsStereo", &getIsStereo)
            .endClass();
        }
    };

    class AudManager : public AudManagerBase {
    public:
        AGE_API AudManager() {
            scoped_vtable x(this);
            ageHook::Thunk<0x519290>::Call<void>(this);
        };

        AGE_API ~AudManager() {
            scoped_vtable x(this);
            ageHook::Thunk<0x5192D0>::Call<void>(this);
        };

        static ageHook::Type<AudManager*> Instance;

        //members
        AGE_API mmCNRSpeech* GetCNRSpeechPtr()               { return ageHook::Thunk<0x5195C0>::Call<mmCNRSpeech*>(this); }
        AGE_API mmRaceSpeech* GetRaceSpeechPtr()             { return ageHook::Thunk<0x519580>::Call<mmRaceSpeech*>(this); }
        
        //asNode overrides
        AGE_API virtual void Update() override               { ageHook::Thunk<0x519D00>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<AudManager, AudManagerBase>("AudManager")
                .addStaticFunction("Instance", [] { return (AudManager*)Instance; })

                .addFunction("GetCNRSpeechPtr", &GetCNRSpeechPtr)
                .addFunction("GetRaceSpeechPtr", &GetRaceSpeechPtr)
            .endClass();
        }
    };


    template<>
    void luaAddModule<module_audio>(LuaState L) {
        luaBind<AudManagerBase>(L);
        luaBind<AudManager>(L);
        luaBind<AudSoundBase>(L);
        luaBind<mmCNRSpeech>(L);
        luaBind<mmRaceSpeech>(L);
    }
}