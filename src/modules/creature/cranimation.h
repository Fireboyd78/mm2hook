#pragma once
#include <modules\creature.h>
#include <modules\data.h>

namespace MM2
{
    // Forward declarations
    class crAnimation;

    // External declarations
    extern class datRefCount;

    // Class definitions

    //TODO
    class crAnimation : public datRefCount {
    public:
        static void OutputAnimationList()           { hook::StaticThunk<0x57D0F0>::Call<void>(); }
    };

    // Lua initialization

}