#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class phBoundSphere;

    // External declarations


    // Class definitions
    class phBoundSphere : public phBound
    {
    private:
        float Radius;
        phMaterial Material;

    public:
        inline float getRadius()
        {
            return this->Radius;
        }

        AGE_API void SetRadius(float radius)            { hook::Thunk<0x484510>::Call<void>(this, radius); }
        AGE_API void ScaleRadius(float scale)           { hook::Thunk<0x484550>::Call<void>(this, scale); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<phBoundSphere, phBound>("phBoundSphere")
                .addProperty("Radius", &getRadius, &SetRadius)
                .addFunction("ScaleRadius", &ScaleRadius)
                .endClass();
        }
    };
    ASSERT_SIZEOF(phBoundSphere, 0x80);

    // Lua initialization

}