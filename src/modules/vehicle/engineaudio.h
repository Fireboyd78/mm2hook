#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehEngineAudio;

    // External declarations
    extern class vehEngineSampleWrapper;

    // Class definitions

    class vehEngineAudio {
    private:
        int SampleCount;
        vehEngineSampleWrapper** Samples;
    };
    ASSERT_SIZEOF(vehEngineAudio, 8);

    // Lua initialization

}