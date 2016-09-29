#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class cityTimeWeatherLighting;

    class cityTimeWeatherLighting {
    public:
        /*0x00*/float KeyHeading;
        /*0x04*/float KeyPitch;
        /*0x08*/Vector3 KeyColor;

        /*0x14*/float Fill1Heading;
        /*0x18*/float Fill1Pitch;
        /*0x1C*/Vector3 Fill1Color;

        /*0x28*/float Fill2Heading;
        /*0x2C*/float Fill2Pitch;
        /*0x30*/Vector3 Fill2Color;

        // "ambient light levels" that get calculated using the Ambient color?
        // doesn't care for any of the Key/Pitch values
        /*0x3C*/int AmbientLevel3; // fullbright
        /*0x40*/int AmbientLevel2;
        /*0x44*/int AmbientLevel1;

        /*0x48*/int Ambient; // looks like the alpha is ignored
    };
}