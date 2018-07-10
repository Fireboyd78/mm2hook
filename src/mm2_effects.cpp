#include "mm2_effects.h"
#include "mm2_model.h"

#include <algorithm>

namespace MM2
{
    const auto DisableGlobalSeed = (void(*)(void))(0x4BBD50);
    const auto EnableGlobalSeed = (void(*)(void))(0x4BBD80);
    const auto frand = (float(*)(void))(0x4BBE30);
    const auto ApplyBirdPoopDamage = (void(*)(gfxTexture *, gfxTexture *, float, float))(0x5923C0);

    fxTexelDamage::fxTexelDamage()
        : AdjunctCount(0)
        , TriCount(0)
        , TextureCount(0)
        , Positions(0)
        , TexCoords(0)
        , DamageTris(0)
        , CurrentShaders(0)
        , CleanShaders(0)
        , DamageTextures(0)
    { }

    int fxTexelDamage::Init(modStatic* model, modShader* shaders, int shaderCount)
    {
        if (model->PacketCount <= 0)
        {
            return 0;
        }

        TextureCount = shaderCount;
        CurrentShaders = new modShader[shaderCount];
        CleanShaders = new modShader[shaderCount];
        DamageTextures = new gfxTexture*[shaderCount];

        for (int i = 0; i < shaderCount; ++i)
        {
            CleanShaders[i].Texture = shaders[i].Texture;
            CleanShaders[i].Material = shaders[i].Material;

            CurrentShaders[i].Texture = CleanShaders[i].Texture;
            CurrentShaders[i].Material = CleanShaders[i].Material;

            DamageTextures[i] = nullptr;
        }

        bool hasDamageTextures = false;

        for (int i = 0; i < model->PacketCount; ++i)
        {
            int shaderIndex = model->ShaderIndices[i];
            modShader* cleanShader = &CleanShaders[shaderIndex];
            cleanShader->PreLoad();

            if (cleanShader->Texture)
            {
                char textureName[128];

                strcpy_s(textureName, cleanShader->Texture->Name);

                char* find = strrchr(textureName, '_');

                if (find && !_strcmpi(find, "_dmg"))
                {
                    *find = '\0';

                    DamageTextures[shaderIndex] = cleanShader->Texture;

                    gfxTexture* cleanTexture = $gfxGetTexture(textureName, 1);

                    if (cleanTexture)
                    {
                        cleanShader->Texture = cleanTexture;
                    }

                    hasDamageTextures = true;
                }
                else
                {
                    strcat_s(textureName, "_dmg");

                    gfxTexture* damageTexture = $gfxGetTexture(textureName, 1);

                    DamageTextures[shaderIndex] = damageTexture;

                    if (damageTexture)
                    {
                        hasDamageTextures = true;
                    }
                }

                modShader* dirtyShader = &CurrentShaders[shaderIndex];

                dirtyShader->Texture = cleanShader->Texture->Clone();
                dirtyShader->Material = new gfxMaterial(*cleanShader->Material);
            }
        }

        if (hasDamageTextures)
        {
            int adjunctCount = model->GetAdjunctCount();

            AdjunctCount = adjunctCount;

            Positions = new Vector3[adjunctCount];
            TexCoords = new Vector2[adjunctCount];

            int currentPos = 0;

            for (int i = 0; i < model->PacketCount; ++i)
            {
                for (gfxPacket* packet = model->ppPackets[i]; packet; packet = packet->Next)
                {
                    if (packet->StartVertex >= 0)
                    {
                        packet->DoLock();
                    }

                    for (int j = 0; j < packet->AdjunctCount; ++j)
                    {
                        packet->GetPosition(&Positions[currentPos], j);
                        packet->GetTexCoord(&TexCoords[currentPos], j);

                        ++currentPos;
                    }

                    if (packet->StartVertex >= 0)
                    {
                        packet->DoUnlock();
                    }
                }
            }

            int triCount = model->GetTriCount();

            TriCount = triCount;
            DamageTris = new TexelDamageTri[triCount];

            int currentTri = 0;
            int currentAdjunct = 0;

            for (int i = 0; i < model->PacketCount; ++i)
            {
                uint8_t shaderIndex = model->ShaderIndices[i];

                for (gfxPacket* packet = model->ppPackets[i]; packet; packet = packet->Next)
                {
                    for (int j = 0; j < (packet->TriCount / 3); ++j)
                    {
                        int tri[3];
                        packet->GetTri(tri, j);

                        DamageTris[currentTri].Indices[0] = (uint16_t)(currentAdjunct + tri[0]);
                        DamageTris[currentTri].Indices[1] = (uint16_t)(currentAdjunct + tri[1]);
                        DamageTris[currentTri].Indices[2] = (uint16_t)(currentAdjunct + tri[2]);
                        DamageTris[currentTri].Texture = shaderIndex;

                        ++currentTri;
                    }

                    currentAdjunct += packet->AdjunctCount;
                }
            }

            std::sort(DamageTris, DamageTris + triCount, [] (const TexelDamageTri& lhs, const TexelDamageTri& rhs)
            {
                return 0 + rhs.Texture - lhs.Texture;

            });

            return 1;
        }

        Kill();
        return 0;
    }

    void fxTexelDamage::Reset()
    {
        for (int i = 0; i < TextureCount; ++i)
        {
            if (CleanShaders[i].Texture != CurrentShaders[i].Texture)
            {
                CurrentShaders[i].Texture->Blit(
                    0, 0,
                    CleanShaders[i].Texture,
                    0, 0,
                    CleanShaders[i].Texture->Width, CleanShaders[i].Texture->Height
                );
            }

            if (CleanShaders[i].Material != CurrentShaders[i].Material)
            {
                *CleanShaders[i].Material = *CurrentShaders[i].Material;
            }
        }
    }

    void fxTexelDamage::ApplyDamage(Vector3* damagePosition, float maxDist)
    {
        // maxDist = 1.0f;
        // LogFile::Printf(1, "Texel Damage @ %f, %f, %f - Radius: %f", damagePosition->X, damagePosition->Y, damagePosition->Z, maxDist);

        DisableGlobalSeed();

        int lastTex = -1;

        for (int i = 0; i < TriCount; ++i)
        {
            TexelDamageTri* tri = &DamageTris[i];

            int texIndex = tri->Texture;

            gfxTexture* damageTexture = DamageTextures[texIndex];

            if (damageTexture)
            {
                int tri0 = tri->Indices[0];
                int tri1 = tri->Indices[1];
                int tri2 = tri->Indices[2];

                float dist0 = damagePosition->Dist(Positions[tri0]);
                float dist1 = damagePosition->Dist(Positions[tri1]);
                float dist2 = damagePosition->Dist(Positions[tri2]);

                float distMagniture = 1.0f / (dist0 + dist1 + dist2);

                if ((dist0 < maxDist) || (dist1 < maxDist) || (dist2 < maxDist))
                {
                    float randX = frand();
                    float randY = frand();
                    float randZ = frand();

                    float randMagnitude = 1.0f / (randX + randY + randZ);

                    randX *= randMagnitude;
                    randY *= randMagnitude;
                    randZ *= randMagnitude;

                    float texDamageX
                        = randX * TexCoords[tri0].X
                        + randY * TexCoords[tri1].X
                        + randZ * TexCoords[tri2].X;

                    float texDamageY
                        = randX * TexCoords[tri0].Y
                        + randY * TexCoords[tri1].Y
                        + randZ * TexCoords[tri2].Y;

                    if (CleanShaders[texIndex].Texture != CurrentShaders[texIndex].Texture)
                    {
                        ApplyBirdPoopDamage(
                            CurrentShaders[texIndex].Texture,
                            damageTexture,
                            texDamageX,
                            texDamageY);
                    }

                    /*if (texIndex > lastTex)
                    {
                        // Completely made up
                        float matChange = min(min(min(dist0, dist1), dist2) * 2.0f, 0.9f);

                        shader->Material->Shininess *= matChange;

                        shader->Material->Specular.X *= matChange;
                        shader->Material->Specular.Y *= matChange;
                        shader->Material->Specular.Z *= matChange;
                        shader->Material->Specular.W *= matChange;

                        lastTex = texIndex;
                    }*/
                }
            }
        }

        EnableGlobalSeed();
    }

    void fxTexelDamage::Kill()
    {
        if (TextureCount)
        {
            // TODO: Handle texture ref count

            for (int i = 0; i < TextureCount; ++i)
            {
                if (CleanShaders[i].Material != CurrentShaders[i].Material)
                {
                    delete CurrentShaders[i].Material;
                }
            }

            delete[] CurrentShaders;
            CurrentShaders = nullptr;

            delete[] CleanShaders;
            CleanShaders = nullptr;

            delete[] DamageTextures;
            DamageTextures = nullptr;

            TextureCount = 0;
        }

        if (AdjunctCount)
        {
            delete[] Positions;
            Positions = nullptr;

            delete[] TexCoords;
            TexCoords = nullptr;

            AdjunctCount = 0;
        }

        if (TriCount)
        {
            delete[] DamageTris;
            DamageTris = nullptr;

            TriCount = 0;
        }
    }
};

void fxTexelDamageHandler::Install()
{
    InstallCallback("fxTexelDamage::ApplyDamage", "",
        &MM2::fxTexelDamage::ApplyDamage, {
            cbHook<CALL>(0x4CAE46),
        }
    );

    InstallCallback("fxTexelDamage::Init", "",
        &MM2::fxTexelDamage::Init, {
            cbHook<CALL>(0x4CD492)
        }
    );

    InstallCallback("fxTexelDamage::Reset", "",
        &MM2::fxTexelDamage::Reset, {
            cbHook<CALL>(0x4CE018)
        }
    );

    InstallCallback("fxTexelDamage::Kill", "",
        &MM2::fxTexelDamage::Kill, {
            cbHook<JMP>(0x591CC0)
        }
    );
}