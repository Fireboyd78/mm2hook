#pragma once
#include <modules\level.h>
#include <modules\phys\phmaterial.h>

namespace MM2
{
    // Forward declarations
    class lvlMaterial;

    // External declarations
    extern class datAsciiTokenizer;
    extern class phMaterial;

    // Class definitions
    class lvlMaterial : public phMaterial {
    private:
        float Drag;
        float Width;
        float Height;
        float Depth;
        short PtxIndex[2];
        float PtxThreshold[2];
    public:
        AGE_API lvlMaterial() {
            scoped_vtable x(this);
            ageHook::Thunk<0x4664C0>::Call<void>(this);
        }

        //members
        AGE_API void Load(datAsciiTokenizer *reader)            { ageHook::Thunk<0x466510>::Call<void>(this, reader); }

        //virtuals
        AGE_API void Copy(const phMaterial *material) override  { ageHook::Thunk<0x4668E0>::Call<void>(this, material); }
        AGE_API void Save(datAsciiTokenizer *writer) override   { ageHook::Thunk<0x4666F0>::Call<void>(this, writer); }
    };

    ASSERT_SIZEOF(lvlMaterial, 0x4C);

    // Lua initialization

}