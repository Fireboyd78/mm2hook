#include "mm2_audio.h"
using namespace MM2;

/*
    DirSnd
*/
DECLARE_HOOK(DirSnd::$$ctor);
DECLARE_HOOK(DirSnd::$$dtor);

DECLARE_HOOK(DirSnd::$DeInit);

DECLARE_HOOK(DirSnd::$Init);
DECLARE_HOOK(DirSnd::$Init3DListener);

DECLARE_HOOK(DirSnd::$IsDSDeviceInList);
DECLARE_HOOK(DirSnd::$SetDeviceRating);
DECLARE_HOOK(DirSnd::$EnumDSDevices);

DECLARE_HOOK(DirSnd::$InitPrimarySoundBuffer);
DECLARE_HOOK(DirSnd::$CreatePrimaryInterfaceAndBuffer);

DECLARE_HOOK(DirSnd::$ClearDSDeviceList);

DECLARE_HOOK(DirSnd::$SetPrimaryBufferFormat);

DECLARE_HOOK(DirSnd::$ScanAvailableDSFormats);
DECLARE_HOOK(DirSnd::$TranslateDSError);

DECLARE_HOOK(DirSnd::$SetBitDepth);
DECLARE_HOOK(DirSnd::$GetDeviceNames);
DECLARE_HOOK(DirSnd::$GetNum3DHallBufs);
                                                                
/*                                                              
    mmDirSnd                                                    
*/                                                              
DECLARE_HOOK(mmDirSnd::$$ctor);
DECLARE_HOOK(mmDirSnd::$$dtor);

DECLARE_HOOK(mmDirSnd::$DeInit);
DECLARE_HOOK(mmDirSnd::$Init);

DECLARE_HOOK(mmDirSnd::$InitPrimarySoundBuffer);

DECLARE_HOOK(mmDirSnd::$EAXEnabled);
DECLARE_HOOK(mmDirSnd::$DSound3DEnabled);