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
    extern class DirSnd;
    extern class mmDirSnd;

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

        /*
            Function hooks
        */

        static AGEHook<0x5A4ED0>::Func<void> $$ctor;
        static AGEHook<0x5A4F30>::Func<void> $$dtor;

        static AGEHook<0x5A4FD0>::Func<void> $DeInit;

        static AGEHook<0x5A5080>::Func<DirSnd *> $Init;
        static AGEHook<0x5A5160>::Func<int> $Init3DListener;

        static AGEHook<0x5A51C0>::Func<int> $IsDSDeviceInList;
        static AGEHook<0x5A5390>::Func<void> $SetDeviceRating;
        static AGEHook<0x5A5510>::Func<int> $EnumDSDevices;

        static AGEHook<0x5A5530>::Func<int> $InitPrimarySoundBuffer;
        static AGEHook<0x5A5770>::Func<int> $CreatePrimaryInterfaceAndBuffer;

        static AGEHook<0x5A5860>::Func<void> $ClearDSDeviceList;

        static AGEHook<0x5A5950>::Func<void> $SetPrimaryBufferFormat;

        static AGEHook<0x5A5A30>::Func<void> $ScanAvailableDSFormats;
        static AGEHook<0x5A5BB0>::Func<char *> $TranslateDSError;

        static AGEHook<0x5A5BD0>::Func<void> $SetBitDepth;
        static AGEHook<0x5A5C00>::Func<char **> $GetDeviceNames;
        static AGEHook<0x5A5630>::Func<UINT> $GetNum3DHallBufs;

        AGE_API int CreatePrimaryInterfaceAndBuffer(DSBUFFERDESC *p1) {
            return $CreatePrimaryInterfaceAndBuffer(this, p1);
        };
    public:
        AGE_API DirSnd() {
            PUSH_VTABLE();
            $$ctor(this);
            POP_VTABLE();
        };

        AGE_API ~DirSnd() {
            PUSH_VTABLE();
            $$dtor(this);
            POP_VTABLE();
        };

        AGE_API static DirSnd * Init(DWORD sampleRate, bool enableStero, bool enable3D, const char *deviceName) {
            return $Init(sampleRate, enableStero, enable3D, deviceName);
        };

        AGE_API int Init3DListener(float p1, float p2, float p3, float p4, float p5) {
            return $Init3DListener(this, p1, p2, p3, p4, p5);
        };

        AGE_API int IsDSDeviceInList(GUID *guid) {
            return $IsDSDeviceInList(this, guid);
        };

        AGE_API void DeInit(void) {
            $DeInit(this);
        };

        AGE_API void SetDeviceRating(DSDEVICEDESC *p1) {
            $SetDeviceRating(this, p1);
        };

        AGE_API int EnumDSDevices(void) {
            return $EnumDSDevices(this);
        };

        AGE_API void ClearDSDeviceList(void) {
            $ClearDSDeviceList(this);
        };

        AGE_API void SetPrimaryBufferFormat(DWORD p1, BYTE p2) {
            $SetPrimaryBufferFormat(this, p1, p2);
        };

        AGE_API void ScanAvailableDSFormats(void) {
            $ScanAvailableDSFormats(this);
        };

        AGE_API char * TranslateDSError(long p1) {
            return $TranslateDSError(this, p1);
        };

        AGE_API void SetBitDepth(int p1) {
            $SetBitDepth(this, p1);
        };

        AGE_API char ** GetDeviceNames(void) {
            return $GetDeviceNames(this);
        };

        AGE_API UINT GetNum3DHallBufs(void) {
            return $GetNum3DHallBufs(this);
        };

        AGE_API virtual int InitPrimarySoundBuffer(DWORD p1, BYTE p2, char *p3) {
            return $InitPrimarySoundBuffer(this, p1, p2, p3);
        };
    };

    class mmDirSnd : public DirSnd {
    protected:
        /*0x3C*/
        UINT eaxEnabled;
        UINT dsound3DEnabled;

        /*
            Function hooks
        */

        static AGEHook<0x51CBE0>::Func<void> $$ctor;
        static AGEHook<0x51CC00>::Func<void> $$dtor;

        static AGEHook<0x51CC10>::Func<void> $DeInit;
        static AGEHook<0x51CC50>::Func<mmDirSnd *> $Init;

        static AGEHook<0x51CD90>::Func<int> $InitPrimarySoundBuffer;

        static AGEHook<0x51CE60>::Func<UINT> $EAXEnabled;
        static AGEHook<0x51CE70>::Func<UINT> $DSound3DEnabled;
    public:
        AGE_API mmDirSnd() {
            PUSH_VTABLE();
            $$ctor(this);
            POP_VTABLE();
        };

        AGE_API ~mmDirSnd() {
            PUSH_VTABLE();
            $$dtor(this);
            POP_VTABLE();
        };

        AGE_API static mmDirSnd * Init(int sampleRate, bool enableStero, int a4, float volume, LPCSTR deviceName, bool enable3D) {
            return $Init(sampleRate, enableStero, a4, volume, deviceName, enable3D);
        };

        AGE_API void DeInit(short p1, short p2) {
            $DeInit(this, p1, p2);
        };

        AGE_API UINT EAXEnabled(void) {
            return $EAXEnabled(this);
        };

        AGE_API UINT DSound3DEnabled(void) {
            return $DSound3DEnabled(this);
        };

        AGE_API virtual int InitPrimarySoundBuffer(DWORD p1, BYTE p2, char *p3) {
            return $InitPrimarySoundBuffer(this, p1, p2, p3);
        };
    };
}
