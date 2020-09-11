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
        inline int getPtxIndex(int num) {
            if (num < 0 || num > 1)
                return -1;
            return PtxIndex[num];
        }

        inline void setPtxIndex(int num, int index) {
            if (num < 0 || num > 1)
                return;
            PtxIndex[num] = (short)index;
        }

        inline float getPtxThreshold(int num) {
            if (num < 0 || num > 1)
                return 0.f;
            return PtxThreshold[num];
        }

        inline void setPtxThreshold(int num, float threshold) {
            if (num < 0 || num > 1)
                return;
            PtxThreshold[num] = threshold;
        }
    public:
        AGE_API lvlMaterial() {
            scoped_vtable x(this);
            hook::Thunk<0x4664C0>::Call<void>(this);
        }

        //members
        AGE_API void Load(datAsciiTokenizer *reader)            { hook::Thunk<0x466510>::Call<void>(this, reader); }

        //virtuals
        AGE_API void Copy(const phMaterial *material) override  { hook::Thunk<0x4668E0>::Call<void>(this, material); }
        AGE_API void Save(datAsciiTokenizer *writer) override   { hook::Thunk<0x4666F0>::Call<void>(this, writer); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<lvlMaterial, phMaterial>("lvlMaterial")
                //properties
                .addVariableRef("Drag", &lvlMaterial::Drag)
                .addVariableRef("Width", &lvlMaterial::Width)
                .addVariableRef("Height", &lvlMaterial::Height)
                .addVariableRef("Depth", &lvlMaterial::Depth)

                .addFunction("GetPtxIndex", &getPtxIndex)
                .addFunction("SetPtxIndex", &setPtxIndex)

                .addFunction("GetPtxThreshold", &getPtxThreshold)
                .addFunction("SetPtxThreshold", &setPtxThreshold)

                .endClass();
        }
    };

    ASSERT_SIZEOF(lvlMaterial, 0x4C);

    // Lua initialization

}