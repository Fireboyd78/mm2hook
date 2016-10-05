#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern struct gfxInterface;

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

    struct gfxInterface {
        /*0x00*/ GUID GUID;
        /*0x04*/ char Name[64];
 
        /*0x44*/ uint32_t DeviceCaps;
 
        /*0x48*/ gfxDeviceType DeviceType;
 
        /*0x4C*/ uint32_t ResolutionCount;   // Max of 64 resolutions
        /*0x50*/ uint32_t ResolutionChoice;
 
        /*0x54*/ gfxDepthFlags AcceptableDepths;  // Used to check if mmResolution::Depth is allowed
 
        /*0x58*/ uint32_t AvailableMemory;
        /*0x5C*/ uint32_t VendorID;
        /*0x60*/ uint32_t DeviceID;
 
        /*0x64*/ gfxResData Resolutions[64];
    };
}