#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class phBoundHotdog;

    // External declarations


    // Class definitions
    class phBoundHotdog : public phBound
    {
    private:
        float Radius;
        float Height;
        phMaterial Material;
    public:
        inline float getRadius()
        {
            return this->Radius;
        }

        inline void setRadius(float radius) 
        {
            this->SetSize(radius, this->getHeight());
        }

        inline float getHeight()
        {
            return this->Height;
        }

        inline void setHeight(float height)
        {
            this->SetSize(this->getRadius(), height);
        }

        AGE_API void SetSize(float radius, float height)           { hook::Thunk<0x480B60>::Call<void>(this, radius, height); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<phBoundHotdog, phBound>("phBoundHotdog")
                .addProperty("Radius", &getRadius, &setRadius)
                .addProperty("Height", &getHeight, &setHeight)
                .endClass();
        }
    };
    ASSERT_SIZEOF(phBoundHotdog, 0x84);


    // Lua initialization

}