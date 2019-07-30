#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class phMaterial;
    class lvlMaterial;

    // External declarations


    // Class definitions
    class phMaterial {
    private:
        char Name[20];
        int unknown24;
        int unknown28;
        int unknown32;
        short EffectIndex;
        short SoundIndex;
        float Elasticity;
        float Friction;
    public:
        phMaterial() {
            PUSH_VTABLE();
            ageHook::Thunk<0x492FF0>::Call<void>(this);
            POP_VTABLE();
        }

        //members
        AGE_API void SetName(char* const name)                    { ageHook::Thunk<0x493050>::Call<void>(this, name);}
        AGE_API void Load(datAsciiTokenizer* tokenizer)           { ageHook::Thunk<0x493070>::Call<void>(this, tokenizer); }
        AGE_API void LoadBinary(Stream* stream)                   { ageHook::Thunk<0x4932A0>::Call<void>(this, stream); }

        //virtuals
        AGE_API virtual void Copy(const phMaterial* copyFrom)     { ageHook::Thunk<0x493270>::Call<void>(this, copyFrom);}
        AGE_API virtual void Save(datAsciiTokenizer* tokenizer)   { ageHook::Thunk<0x493160>::Call<void>(this, tokenizer);}
        AGE_API virtual void SaveBinary(Stream* stream)           { ageHook::Thunk<0x493340>::Call<void>(this, stream); }
    };

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
        AGE_API void Load(datAsciiTokenizer* tokenizer)           { ageHook::Thunk<0x466510>::Call<void>(this, tokenizer); }

        //virtuals
        AGE_API virtual void Copy(const phMaterial* copyFrom) override     { ageHook::Thunk<0x4668E0>::Call<void>(this, copyFrom);}
        AGE_API virtual void Save(datAsciiTokenizer* tokenizer) override   { ageHook::Thunk<0x4666F0>::Call<void>(this, tokenizer);}
    };

    ASSERT_SIZEOF(phMaterial, 0x30);
    ASSERT_SIZEOF(lvlMaterial, 0x4C);

    // Lua initialization

}