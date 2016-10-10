#pragma once
#include "mm2_common.h"

#define DIRECTSOUND_VERSION 0x0700
#include <mmreg.h>
#include <dsound.h>

namespace MM2
{
    struct mmDirSnd; // Forward Declare
    static MM2RawFnHook<mmDirSnd*(*)(int, bool, int, float, LPCSTR, bool)> 
        $mmDirSndInit (NULL, NULL, 0x51CC50);

    struct mmDirSnd
    {
        struct VTable
        {
            int(mmDirSnd::*InitPrimarySoundBuffer)(int      sampleRate,
                                                   bool     allowStero,
                                                   LPCSTR   lpDeviceName);
        } *lpVTbl;

        LPDIRECTSOUNDBUFFER lpDirectSoundBuffer;
        LPDIRECTSOUND lpDirectSound;
        HWND hWnd;
        int gap10;

        struct mmSoundDevice
        {
            GUID GUID;
            LPCSTR Name;
            DWORD Rating;
            mmSoundDevice *lpNextDevice;
        } *lpFirstDevice, *lpCurrentDevice;

        int field_1C;
        int DeviceCount;
        int BitDepth;
        int DeviceFlags;
        byte byte2C;
        byte Enable3D;
        byte byte2E;
        byte byte2F;
        DWORD SoundEnabled;
        int DeviceCaps;
        int dword38;
        int field_3C;
        float Volume;

        static mmDirSnd* Init(int       sampleRate,
                              bool      enableStero,
                              int       a4,
                              float     volume,
                              LPCSTR    deviceName,
                              bool      enable3D);
    };
}