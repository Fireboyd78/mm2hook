#pragma once
#include <modules\creature.h>

namespace MM2
{
    // Forward delcarations
    class dgRagdoll;

    // External declarations


    // Class definitions

    //TODO
    class dgRagdoll {
    public:
        AGE_API Matrix34* GetMatrix()                       { return ageHook::Thunk<0x5A7D70>::Call<Matrix34*>(this); }
    };


    // Lua initialization

}