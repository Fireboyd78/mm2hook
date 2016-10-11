#include "mm2_audio.h"
using namespace MM2;

/*
    DirSnd
*/
MM2FnHook<void>     DirSnd::$$ctor                              ( NULL, NULL, 0x5A4ED0 );
MM2FnHook<void>     DirSnd::$$dtor                              ( NULL, NULL, 0x5A4F30 );

MM2FnHook<void>     DirSnd::$DeInit                             ( NULL, NULL, 0x5A4FD0 );

MM2FnHook<DirSnd *> DirSnd::$Init                               ( NULL, NULL, 0x5A5080 );
MM2FnHook<int>      DirSnd::$Init3DListener                     ( NULL, NULL, 0x5A5160 );

MM2FnHook<int>      DirSnd::$IsDSDeviceInList                   ( NULL, NULL, 0x5A51C0 );
MM2FnHook<void>     DirSnd::$SetDeviceRating                    ( NULL, NULL, 0x5A5390 );
MM2FnHook<int>      DirSnd::$EnumDSDevices                      ( NULL, NULL, 0x5A5510 );

MM2FnHook<int>      DirSnd::$InitPrimarySoundBuffer             ( NULL, NULL, 0x5A5530 );
MM2FnHook<int>      DirSnd::$CreatePrimaryInterfaceAndBuffer    ( NULL, NULL, 0x5A5770 );

MM2FnHook<void>     DirSnd::$ClearDSDeviceList                  ( NULL, NULL, 0x5A5860 );

MM2FnHook<void>     DirSnd::$SetPrimaryBufferFormat             ( NULL, NULL, 0x5A5950 );

MM2FnHook<void>     DirSnd::$ScanAvailableDSFormats             ( NULL, NULL, 0x5A5A30 );
MM2FnHook<char *>   DirSnd::$TranslateDSError                   ( NULL, NULL, 0x5A5BB0 );

MM2FnHook<void>     DirSnd::$SetBitDepth                        ( NULL, NULL, 0x5A5BD0 );
MM2FnHook<char **>  DirSnd::$GetDeviceNames                     ( NULL, NULL, 0x5A5C00 );
MM2FnHook<UINT>     DirSnd::$GetNum3DHallBufs                   ( NULL, NULL, 0x5A5630 );

/*
    mmDirSnd
*/
 MM2FnHook<void>    mmDirSnd::$$ctor                            ( NULL, NULL, 0x51CBE0 );
 MM2FnHook<void>    mmDirSnd::$$dtor                            ( NULL, NULL, 0x51CC00 );

 MM2FnHook<void>    mmDirSnd::$DeInit                           ( NULL, NULL, 0x51CC10 );

 MM2FnHook<mmDirSnd *>
                    mmDirSnd::$Init                             ( NULL, NULL, 0x51CC50 );

 MM2FnHook<int>     mmDirSnd::$InitPrimarySoundBuffer           ( NULL, NULL, 0x51CD90 );

 MM2FnHook<UINT>    mmDirSnd::$EAXEnabled                       ( NULL, NULL, 0x51CE60 );
 MM2FnHook<UINT>    mmDirSnd::$DSound3DEnabled                  ( NULL, NULL, 0x51CE70 );