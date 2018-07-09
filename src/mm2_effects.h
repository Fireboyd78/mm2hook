#pragma once

#include <cstdint>

namespace MM2
{
    class Vector2;
    class Vector3;

    class modShader;
    class gfxTexture;

    class modStatic;
    class modShader;

    struct TexelDamageTri
    {
        uint16_t Indices[3];
        uint16_t Texture;
    };

    static_assert(sizeof(TexelDamageTri) == 8, "");

    struct fxTexelDamage
    {
        int AdjunctCount;
        int TriCount;
        int TextureCount;
        MM2::Vector3 *Positions;
        MM2::Vector2 *TexCoords;
        TexelDamageTri *DamageTris;

        // Referenced by vehCarModel::Draw @ 0x4CE0CB, offset 0x18
        modShader *CurrentShaders;
        modShader *CleanShaders;
        gfxTexture **DamageTextures;

        fxTexelDamage();

        int Init(modStatic* model, modShader* shaders, int shaderCount);

        void Reset();
        void ApplyDamage(Vector3* damagePosition, float maxDist);

        void Kill();
    };

    // Allocated by vehCarModel::Init @ 0x4CD408, size 0x24
    static_assert(sizeof(fxTexelDamage) == 0x24, "");
}

class fxTexelDamageHandler {
public:
    static void Install();
};