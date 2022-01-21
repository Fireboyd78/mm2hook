#pragma once
#include <modules\banger.h>

namespace MM2
{
    // Forward declarations
    class dgTreeRenderer;

    // External declarations


    // Class definitions

    class dgTreeRenderer {
    public:
        static hook::Type<dgTreeRenderer*> Instance;

        AGE_API void AddTree(dgBangerInstance* inst, int lod)                 { hook::Thunk<0x442B20>::Call<void>(this, inst, lod); }
        AGE_API void RenderTrees()                                            { hook::Thunk<0x442B50>::Call<void>(this); }
    };

    // Lua initialization

}