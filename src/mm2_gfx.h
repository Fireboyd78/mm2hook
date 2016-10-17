#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern struct gfxInterface;

    // what kind of primitive to draw
    // same values as D3DPRIMITIVETYPE
    enum gfxDrawMode {
        DRAWMODE_POINTLIST      = 1,
        DRAWMODE_LINELIST       = 2,
        DRAWMODE_LINESTRIP      = 3,
        DRAWMODE_TRIANGLELIST   = 4,
        DRAWMODE_TRIANGLESTRIP  = 5,
        DRAWMODE_TRIANGLEFAN    = 6,
    };

    /*
        gfxInterface
    */
    enum gfxDeviceType {
        Software        = 0,    // Software (No 3D Video Card)
        Hardware        = 1,    // Hardware (3D Video Card)
        HardwareWithTnL = 2     // Hardware (3D Video Card With T&L)
    };

    enum gfxDepthFlags {
        Depth16 = 0x400,
        Depth24 = 0x200,
        Depth32 = 0x100
    };

    struct gfxResData {
        unsigned short ScreenWidth;
        unsigned short ScreenHeight;
        unsigned short ColorDepth;
        unsigned short Flags; // = ((ColorDepth == 16) + 6)
    };

    struct gfxInterface {
        /*0x00*/ GUID GUID;
        /*0x10*/ char Name[64];

        /*0x50*/ unsigned int DeviceCaps;

        /*0x54*/ gfxDeviceType DeviceType;

        /*0x58*/ unsigned int ResolutionCount;   // Max of 64 resolutions
        /*0x5C*/ unsigned int ResolutionChoice;

        /*0x60*/ gfxDepthFlags AcceptableDepths;  // Used to check if mmResolution::Depth is allowed

        /*0x64*/ unsigned int AvailableMemory;
        /*0x68*/ unsigned int VendorID;
        /*0x6C*/ unsigned int DeviceID;

        /*0x70*/ gfxResData Resolutions[64];
    };
}
