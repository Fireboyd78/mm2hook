#pragma once
#include <modules\creature.h>

namespace MM2
{
    // Forward declarations
    class crSkeleton;
    class crSkeletonData;

    // External declarations


    // Class definitions

    class crSkeleton {
    public:
        AGE_API void Init(crSkeletonData *a1, Matrix34 *a2) { hook::Thunk<0x57C5D0>::Call<void>(this, a1, a2); }
        AGE_API void Update()                               { hook::Thunk<0x57C630>::Call<void>(this); }
        AGE_API void Attach(Matrix34 *a1)                   { hook::Thunk<0x57C650>::Call<void>(this, a1); }
        AGE_API void Attach(Matrix44 *a1)                   { hook::Thunk<0x57C690>::Call<void>(this, a1); }
    };

    // TODO
    class crSkeletonData {};

    // Lua initialization

}