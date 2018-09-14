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

        fxTexelDamage();

        int Init(modStatic *model, modShader *shaders, int shaderCount);

        void Reset();
        void ApplyDamage(Vector3 *damagePosition, float maxDist);

        void Kill();
    };

    ASSERT_SIZEOF(TexelDamageTri, 8);

    // Allocated by vehCarModel::Init @ 0x4CD408, size 0x24
    ASSERT_SIZEOF(fxTexelDamage, 0x24);
}