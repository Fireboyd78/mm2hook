#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class phMaterial;

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

    ASSERT_SIZEOF(phMaterial, 0x30);
    

    // Lua initialization

}