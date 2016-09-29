#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class vehCarAudioContainer;

    class vehCarAudioContainer {
    protected:
        static MM2FnHook<void> $SetSirenCSVName;
    public:
        static void SetSirenCSVName(LPCSTR name) {
            $SetSirenCSVName(name);
        };
    };
}