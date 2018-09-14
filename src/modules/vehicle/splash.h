#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehSplash;

    // External declarations


    // Class definitions

    class vehSplash : public asNode {
    public:
        AGE_API vehSplash()                                 { ageHook::Thunk<0x4D6A00>::Call<void>(this); }
        AGE_API ~vehSplash()                                { ageHook::Thunk<0x4D6F30>::Call<void>(this); }

        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { ageHook::Thunk<0x4D6BF0>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x4D6A70>::Call<void>(this); }
    };


    // Lua initialization

}