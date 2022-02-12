#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class phBoundBox;

    // External declarations


    // Class definitions
    class phBoundBox : public phBoundPolygonal
    {
    private:
        Vector3 Size;
        Vector3 Vertices[8];
        phPolygon Polygons[6];
        phMaterial** Materials;
    public:
        Vector3 getSize()
        {
            return this->Size;
        }

        AGE_API void SetSize(Vector3 const & size)            { hook::Thunk<0x47A1A0>::Call<void>(this, &size); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<phBoundBox, phBoundPolygonal>("phBoundBox")
                .addProperty("Size", &getSize, &SetSize)
                .endClass();
        }
    };
    ASSERT_SIZEOF(phBoundBox, 0x314);

    // Lua initialization

}