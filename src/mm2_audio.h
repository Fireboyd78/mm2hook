#pragma once
#include "mm2_common.h"

#define DIRECTSOUND_VERSION 0x0700
#include <mmreg.h>
#include <dsound.h>

struct mmDirSnd
{
    struct VTable
    {
        int(mmDirSnd::*InitPrimarySoundBuffer)(int sampleRate, bool allowStero, LPCSTR lpDeviceName);
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
    int field_20;
    int field_24;
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
};