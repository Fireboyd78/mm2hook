#pragma once
#include <modules\creature.h>

namespace MM2
{
    // Forward declarations
    class dgRagdoll;

    // External declarations


    // Class definitions

    //TODO
    class dgRagdoll {
    public:
        AGE_API Matrix34* GetMatrix()                       { return hook::Thunk<0x5A7D70>::Call<Matrix34*>(this); }
    };


    // Lua initialization

}