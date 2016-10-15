#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern struct gfxInterface3;
    extern struct gfxInterface5;

    enum gfxDeviceType {
        Software = 0,    // Software (No 3D Video Card)
        Hardware = 1,    // Hardware (3D Video Card)
        HardwareWithTnL = 2     // Hardware (3D Video Card With T&L)
    };

    enum gfxDepthFlags {
        Depth16 = 0x400,
        Depth24 = 0x200,
        Depth32 = 0x100
    };

    struct gfxResData {
        uint16_t ScreenWidth;
        uint16_t ScreenHeight;
        uint16_t ColorDepth;      // Always 16
        uint16_t Is16BitColor;    // = (ColorDepth == 16) + 6 // Always 7
    };

    struct gfxInterfaceBase {
        /*0x00*/ GUID GUID;
        /*0x10*/ char Name[64];

        /*0x50*/ uint32_t DeviceCaps;

        /*0x54*/ gfxDeviceType DeviceType;

        /*0x58*/ uint32_t ResolutionCount;   // Max of 64 resolutions
        /*0x5C*/ uint32_t ResolutionChoice;

        /*0x60*/ gfxDepthFlags AcceptableDepths;  // Used to check if mmResolution::Depth is allowed
    };

    /* Beta 1 */
    struct gfxInterface3 : gfxInterfaceBase {
        /*0x64*/ gfxResData Resolutions[64];
    };

    /* Beta 2 / Retail */
    struct gfxInterface5 : gfxInterfaceBase {
        /*0x64*/ uint32_t AvailableMemory;
        /*0x68*/ uint32_t VendorID;
        /*0x6C*/ uint32_t DeviceID;
 
        /*0x70*/ gfxResData Resolutions[64];
    };
}