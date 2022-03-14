#pragma once
#include "fixed.h"

namespace MM2
{
    // Forward declarations


    // External declarations


    // Class definitions
    class lvlLandmark : public lvlFixedRotY
    {
    public:
        /*
            lvlInstance virtuals
        */

        virtual AGE_API int IsVisible(const gfxViewport *a1) override
                                                                    { return hook::Thunk<0x468030>::Call<int>(this, a1); }

        virtual AGE_API int Init(const char *a1, const Matrix34 &a2, int a3) override
                                                                    { return hook::Thunk<0x468010>::Call<int>(this, a1, &a2, a3); }

        virtual AGE_API unsigned int SizeOf(void) override          { return sizeof(this); }

        virtual AGE_API bool IsLandmark() override                  { return hook::Thunk<0x467FF0>::Call<bool>(this); }
        virtual AGE_API bool IsCollidable() override                { return hook::Thunk<0x465EB0>::Call<bool>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<lvlLandmark, lvlFixedRotY>("lvlLandmark")
                .endClass();
        }
    };
    ASSERT_SIZEOF(lvlLandmark, 0x28);

    // Lua initialization

}