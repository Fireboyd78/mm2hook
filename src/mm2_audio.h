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
        LPDIRECTSOUNDBUFFER lpDirectSoundBuffer;
        LPDIRECTSOUND lpDirectSound;

        HWND hWnd;

        DWORD unk_10;

        PDSDEVICEDESC pNextDevice, pCurrentDevice;

        int unk_1C;
        int unk_20;
        int unk_24;

        int deviceFlags;

        byte unk_2C;
        byte enable3D;
        byte unk_2E;
        byte unk_2F;

        DWORD soundEnabled;

        int deviceCaps;
        int unk_38;

        static MM2FnHook<void> $$ctor;
        static MM2FnHook<void> $$dtor;

        static MM2FnHook<void> $DeInit;

        static MM2FnHook<DirSnd *> $Init;
        static MM2FnHook<int> $Init3DListener;

        static MM2FnHook<int> $IsDSDeviceInList;
        static MM2FnHook<void> $SetDeviceRating;
        static MM2FnHook<int> $EnumDSDevices;

        static MM2FnHook<int> $InitPrimarySoundBuffer;
        static MM2FnHook<int> $CreatePrimaryInterfaceAndBuffer;

        static MM2FnHook<void> $ClearDSDeviceList;

        static MM2FnHook<void> $SetPrimaryBufferFormat;

        static MM2FnHook<void> $ScanAvailableDSFormats;
        static MM2FnHook<char *> $TranslateDSError;

        static MM2FnHook<void> $SetBitDepth;
        static MM2FnHook<char **> $GetDeviceNames;
        static MM2FnHook<UINT> $GetNum3DHallBufs;

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

        AGE_API static DirSnd * Init(DWORD p1, byte p2, bool p3, char *p4) {
            return $Init(p1, p2, p3, p4);
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
        UINT eaxEnabled;
        UINT dsound3DEnabled;

        static MM2FnHook<void> $$ctor;
        static MM2FnHook<void> $$dtor;

        static MM2FnHook<void> $DeInit;
        static MM2FnHook<mmDirSnd *> $Init;

        static MM2FnHook<int> $InitPrimarySoundBuffer;

        static MM2FnHook<UINT> $EAXEnabled;
        static MM2FnHook<UINT> $DSound3DEnabled;
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

        AGE_API static mmDirSnd * Init(DWORD p1, BYTE p2, int p3, int p4, char *p5, short p6, short p7) {
            return $Init(p1, p2, p3, p4, p5, p6, p7);
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