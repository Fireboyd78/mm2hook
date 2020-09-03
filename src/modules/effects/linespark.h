#pragma once
#include <modules\effects.h>

namespace MM2
{
    // Forward declarations
    class asLineSparks;

    // External declarations


    // Class definitions
    class asLineSparks {
    private:
        byte _buffer[0x60];
    public:
        AGE_API asLineSparks()                           { ageHook::Thunk<0x460690>::Call<void>(this); }

        AGE_API void Init(int count, char *textureName)  { ageHook::Thunk<0x460740>::Call<void>(this, count, textureName); }
        AGE_API void RadialBlast(int count, Vector3 *radius, Vector3 *velocity)
                                                         { ageHook::Thunk<0x460830>::Call<void>(this, count, radius, velocity); }
    };

    // Lua initialization

}