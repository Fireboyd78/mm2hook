#pragma once
#include <modules\level.h>
#include <modules\phys\phmaterial.h>

namespace MM2
{
    // Forward declarations


    // External declarations


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
        lvlMaterial() {
            PUSH_VTABLE();
            ageHook::Thunk<0x4664C0>::Call<void>(this);
            POP_VTABLE();
        }

        //members
        AGE_API void Load(datAsciiTokenizer* tokenizer) { ageHook::Thunk<0x466510>::Call<void>(this, tokenizer); }

        //virtuals
        AGE_API virtual void Copy(const phMaterial* copyFrom) override { ageHook::Thunk<0x4668E0>::Call<void>(this, copyFrom); }
        AGE_API virtual void Save(datAsciiTokenizer* tokenizer) override { ageHook::Thunk<0x4666F0>::Call<void>(this, tokenizer); }
    };

    ASSERT_SIZEOF(lvlMaterial, 0x4C);

    // Lua initialization

}