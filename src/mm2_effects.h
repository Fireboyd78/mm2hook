#pragma once
#include "mm2_common.h"
#include "mm2_gfx.h"
#include "mm2_model.h"

#include <cstdint>

namespace MM2
{
    extern class modShader;
    extern class gfxTexture;

    extern class modStatic;
    extern class modShader;

    struct TexelDamageTri
    {
        uint16_t Indices[3];
        uint16_t Texture;
    };

    struct fxTexelDamage
    {
        int AdjunctCount;
        int TriCount;
        int TextureCount;
        Vector3 *Positions;
        Vector2 *TexCoords;
        TexelDamageTri *DamageTris;

        // Referenced by vehCarModel::Draw @ 0x4CE0CB, offset 0x18
        modShader *CurrentShaders;
        modShader *CleanShaders;
        gfxTexture **DamageTextures;

        ANGEL_ALLOCATOR
        fxTexelDamage();

        int Init(modStatic *model, modShader *shaders, int shaderCount);

        void Reset();
        void ApplyDamage(Vector3 *damagePosition, float maxDist);

        void Kill();
    };

    ASSERT_SIZEOF(TexelDamageTri, 8);

    // Allocated by vehCarModel::Init @ 0x4CD408, size 0x24
    ASSERT_SIZEOF(fxTexelDamage, 0x24);


    class mmText {
    public:
        BYTE byte0;
        BYTE byte1;

        static AGE_API gfxBitmap* CreateFitBitmap(char const* text, void const* font, int color, int bgColor)
        {
            return hook::StaticThunk<0x532310>::Call<gfxBitmap*>(text, font, color, bgColor);
        }
    };

    class mmTextData {
    public:
        Vector2 Pos;
        uint32_t Flags;
        HFONT Font;
        char Text[256];
    };

    class mmTextNode : public asNode {
    public:
        Vector2 Pos;
        uint32_t EntryCount;
        uint32_t MaxEntries;
        uint32_t DrawBits;
        mmText dword2C;
        mmTextData* pTextEntries;
        gfxBitmap* Bitmap;
        BOOL bModified;
        uint32_t dword3C;
        uint32_t dword40;
        uint8_t byte44;
        uint32_t FGColor;
        uint32_t BGColor;
        uint32_t HiglightColor;

        AGE_API void GetTextDimensions(void const* a1, char const* a2, float& a3, float& a4)
        {
            hook::Thunk<0x532B10>::Call<void>(this, a1, a2, &a3, &a4);
        }

        AGE_API void AddText(void const* a1, char const* a2, int a3, float a4, float a5)
        {
            hook::Thunk<0x532C70>::Call<void>(this, a1, a2, a3, a4, a5);
        }
    };
}