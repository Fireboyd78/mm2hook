#pragma once
#include "mm2_common.h"

/*
    DirSnd is based on the "Full-Duplex Filter" sample found in the VC98 SDK
    I've archived these files under "src\vc98_sdk\fdfilter" for reference
*/
typedef struct tag_dsdevdesc
{
    GUID    guDevice;		    // Device GUID
    PSTR    pszDeviceDesc;	    // Description string
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
    class Aud3DObject;
    class AudCreatureContainer;
    class DirSnd;
    class mmDirSnd;

    namespace $DirSnd
    {
        HOOK_EXPORT(0x5A5080, _Func<MM2::DirSnd *>, Init);

        HOOK_EXPORT(0x5A4ED0, _MemberFunc<void>, $$ctor);
        HOOK_EXPORT(0x5A4F30, _MemberFunc<void>, $$dtor);

        HOOK_EXPORT(0x5A4FD0, _MemberFunc<void>, DeInit);
        HOOK_EXPORT(0x5A5160, _MemberFunc<int>, Init3DListener);
        HOOK_EXPORT(0x5A51C0, _MemberFunc<int>, IsDSDeviceInList);
        HOOK_EXPORT(0x5A5390, _MemberFunc<void>, SetDeviceRating);
        HOOK_EXPORT(0x5A5510, _MemberFunc<int>, EnumDSDevices);
        HOOK_EXPORT(0x5A5530, _MemberFunc<int>, InitPrimarySoundBuffer);
        HOOK_EXPORT(0x5A5630, _MemberFunc<int>, CreatePrimaryInterfaceAndBuffer);
        HOOK_EXPORT(0x5A5770, _MemberFunc<void>, ClearDSDeviceList);
        HOOK_EXPORT(0x5A5860, _MemberFunc<void>, SetPrimaryBufferFormat);
        HOOK_EXPORT(0x5A5950, _MemberFunc<void>, ScanAvailableDSFormats);
        HOOK_EXPORT(0x5A5A30, _MemberFunc<char *>, TranslateDSError);
        HOOK_EXPORT(0x5A5BB0, _MemberFunc<void>, SetBitDepth);
        HOOK_EXPORT(0x5A5BD0, _MemberFunc<char **>, GetDeviceNames);
        HOOK_EXPORT(0x5A5C00, _MemberFunc<UINT>, GetNum3DHallBufs);
    }
    namespace $mmDirSnd
    {
        HOOK_EXPORT(0x51CC50, _Func<mmDirSnd *>, Init);

        HOOK_EXPORT(0x51CBE0, _MemberFunc<void>, $$ctor);
        HOOK_EXPORT(0x51CC00, _MemberFunc<void>, $$dtor);

        HOOK_EXPORT(0x51CC10, _MemberFunc<void>, DeInit);
        HOOK_EXPORT(0x51CD90, _MemberFunc<int>, InitPrimarySoundBuffer);
        HOOK_EXPORT(0x51CE60, _MemberFunc<UINT>, EAXEnabled);
        HOOK_EXPORT(0x51CE70, _MemberFunc<UINT>, DSound3DEnabled);
    }
    namespace $AudCreatureContainer {
        HOOK_EXPORT(0x510CF0, _MemberFunc<void>, PlayAvoidanceReaction);
    }

    class Aud3DObject {

    };

    class AudCreatureContainer : public Aud3DObject {
    public:
        void PlayAvoidanceReaction(float intensity) {
            $AudCreatureContainer::PlayAvoidanceReaction(this, intensity);
        }
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
            return $DirSnd::CreatePrimaryInterfaceAndBuffer(this, p1);
        };
    public:
        AGE_API DirSnd() {
            PUSH_VTABLE();
            $DirSnd::$$ctor(this);
            POP_VTABLE();
        };

        AGE_API ~DirSnd() {
            PUSH_VTABLE();
            $DirSnd::$$dtor(this);
            POP_VTABLE();
        };

        AGE_API static DirSnd * Init(DWORD sampleRate, bool enableStero, bool enable3D, char *deviceName) {
            return $DirSnd::Init(sampleRate, enableStero, enable3D, deviceName);
        };

        AGE_API int Init3DListener(float p1, float p2, float p3, float p4, float p5) {
            return $DirSnd::Init3DListener(this, p1, p2, p3, p4, p5);
        };

        AGE_API int IsDSDeviceInList(GUID *guid) {
            return $DirSnd::IsDSDeviceInList(this, guid);
        };

        AGE_API void DeInit(void) {
            $DirSnd::DeInit(this);
        };

        AGE_API void SetDeviceRating(DSDEVICEDESC *p1) {
            $DirSnd::SetDeviceRating(this, p1);
        };

        AGE_API int EnumDSDevices(void) {
            return $DirSnd::EnumDSDevices(this);
        };

        AGE_API void ClearDSDeviceList(void) {
            $DirSnd::ClearDSDeviceList(this);
        };

        AGE_API void SetPrimaryBufferFormat(DWORD p1, BYTE p2) {
            $DirSnd::SetPrimaryBufferFormat(this, p1, p2);
        };

        AGE_API void ScanAvailableDSFormats(void) {
            $DirSnd::ScanAvailableDSFormats(this);
        };

        AGE_API char * TranslateDSError(long p1) {
            return $DirSnd::TranslateDSError(this, p1);
        };

        AGE_API void SetBitDepth(int p1) {
            $DirSnd::SetBitDepth(this, p1);
        };

        AGE_API char ** GetDeviceNames(void) {
            return $DirSnd::GetDeviceNames(this);
        };

        AGE_API UINT GetNum3DHallBufs(void) {
            return $DirSnd::GetNum3DHallBufs(this);
        };

        AGE_API virtual int InitPrimarySoundBuffer(DWORD p1, BYTE p2, char *p3) {
            return $DirSnd::InitPrimarySoundBuffer(this, p1, p2, p3);
        };
    };

    class mmDirSnd : public DirSnd {
    protected:
        /*0x3C*/
        UINT eaxEnabled;
        UINT dsound3DEnabled;
    public:
        AGE_API mmDirSnd() {
            PUSH_VTABLE();
            $mmDirSnd::$$ctor(this);
            POP_VTABLE();
        };

        AGE_API ~mmDirSnd() {
            PUSH_VTABLE();
            $mmDirSnd::$$dtor(this);
            POP_VTABLE();
        };

        AGE_API static mmDirSnd * Init(ulong sampleRate, bool enableStero, int p3, int volume, char *deviceName, short p6, short p7) {
            return $mmDirSnd::Init(sampleRate, enableStero, p3, volume, deviceName, p6, p7);
        };

        AGE_API void DeInit(short p1, short p2) {
            $mmDirSnd::DeInit(this, p1, p2);
        };

        AGE_API UINT EAXEnabled(void) {
            return $mmDirSnd::EAXEnabled(this);
        };

        AGE_API UINT DSound3DEnabled(void) {
            return $mmDirSnd::DSound3DEnabled(this);
        };

        AGE_API virtual int InitPrimarySoundBuffer(DWORD p1, BYTE p2, char *p3) {
            return $mmDirSnd::InitPrimarySoundBuffer(this, p1, p2, p3);
        };
    };
}
