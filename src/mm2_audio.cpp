#include "mm2_audio.h"
#include "mm2_data.h"
using namespace MM2;

mmDirSnd* mmDirSnd::Init(int sampleRate, bool enableStero, int a4, float volume, LPCSTR deviceName, bool enable3D)
{
    // TODO: Load the device name from player config?
    if (*deviceName == '\0')
    {
        datArgParser::Get("defaultsounddev", 0, &deviceName);
    }

    LogFile::Format("mmDirSnd::Init - Using Device: %s\n", deviceName);

    // TODO: Set sampling rate (see 0x519640 - int __thiscall AudManager::SetBitDepthAndSampleRate(int this, int bitDepth, int samplingRate))
    // TODO: Redo SetPrimaryBufferFormat to set sampleSize? (see 0x5A5860 -void __thiscall DirSnd::SetPrimaryBufferFormat(mmDirSnd *this, int sampleRate, bool allowStero))
    return $mmDirSndInit(48000, enableStero, a4, volume, deviceName, enable3D);
}