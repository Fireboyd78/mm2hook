#pragma once
#include "mm2_common.h"
#include "mm2_level.h"
#include "mm2_base.h"
#include "mm2_vector.h"
#include "mm2_rgl.h"

namespace MM2
{
    // Forward declarations
    struct asSparkPos;
    struct asMeshCardInfo;
    struct asMeshCardVertex;
    class asBirthRule;
    class asParticles;

    struct asMeshCardVertex {
        float x;
        float y;
        float z;
        float w;
    };

    struct asSparkPos
    {
        byte TexCoordOffset;
        byte OffsetMask;
        byte SomeFlag;
        float Scale;
        uint Color;
        float ShadowHeight;
        Vector3 Pos;
    };

    struct asMeshCardInfo
    {
        int Count;
        uint PositionsIndex;
        Vector2 *Positions;
        Vector2 *TexCoords;

        void asMeshCardInfo::Draw(asSparkPos *sparks, int sparkCount)
        {
            byte currentCullMode = *(byte*)0x6856BB;

            for (int i = 0; i < sparkCount; ++i)
            {
                asSparkPos* spark = &sparks[i];

                Vector2* positions = &this->Positions[4 * ((this->PositionsIndex - 1) & spark->OffsetMask)];
                Vector2* texcoords = &this->TexCoords[4 * spark->TexCoordOffset];

                vglCurrentColor = spark->Color;

                vglBegin(gfxDrawMode::DRAWMODE_TRIANGLEFAN, 0);
                for (int j = 0; j < this->Count; ++j)
                {
                    int aj = currentCullMode != 2 ? this->Count - j - 1 : j;

                    vglTexCoord2f(texcoords[aj].X, texcoords[aj].Y);

                    float scaleX = spark->Scale * positions[aj].X;
                    float scaleY = spark->Scale * positions[aj].Y;

                    float z = gfxRenderState::sm_Camera->m23 * scaleY + gfxRenderState::sm_Camera->m13 * scaleX + spark->Pos.Z;
                    float y = gfxRenderState::sm_Camera->m22 * scaleY + gfxRenderState::sm_Camera->m12 * scaleX + spark->Pos.Y;
                    float x = gfxRenderState::sm_Camera->m21 * scaleY + gfxRenderState::sm_Camera->m11 * scaleX + spark->Pos.X;

                    vglVertex(x, y, z);
                }
                vglEnd();
            }
        }
    };

    class asParticles {
    public:
        uint dword4;
        int SparkCount;
        uint dwordC;
        struct asSparkPos *pSparks;
        gfxTexture *Texture;
        asMeshCardInfo MeshCardInfo;
        uint32_t field_28;
        uint32_t field_2C;
        float dword30;
        uint dword34;
        uint dword38;
        asBirthRule *pBirthRule;
        uint dword40;
        uint dword44;
        uint dword48;
        uint dword4C;
        float dword50;
    public:
        AGE_API void Init(int a1, int a2, int a3, int a4, asMeshCardVertex* a5)
                                                            { ageHook::Thunk<0x460FB0>::Call<void>(this, a1, a2, a3, a5, a5); }
        AGE_API void Blast(int a1, asBirthRule* a2)         { ageHook::Thunk<0x461490>::Call<void>(this, a1, a2); }
        AGE_API void Reset()                                { ageHook::Thunk<0x461040>::Call<void>(this); }
        AGE_API void Update()                               { ageHook::Thunk<0x4610F0>::Call<void>(this); }
        AGE_API void SetTexture(char const* a1)             { ageHook::Thunk<0x461090>::Call<void>(this, a1); }
        AGE_API void SetTexture(gfxTexture* a1)             { ageHook::Thunk<0x461050>::Call<void>(this, a1); }

        /*
            asParticles Virtuals
        */
        AGE_API virtual ~asParticles()                      { ageHook::Thunk<0x4619E0>::Call<void>(this); }
        AGE_API virtual void Cull()                         { ageHook::Thunk<0x4615A0>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<asParticles, asNode>("asParticles")
                .addFunction("Blast", &Blast)
                .addFunction("Update", &Update)
                .addFunction("Init", &Init)
                .addFunction("Reset", &Reset)
                .addFunction<void (asParticles::*)(const char* a1)>("SetTexture", &SetTexture)
                .endClass();
        }
    };

    class asBirthRule : public asNode
    {
    public:
        Vector3 Position;
        Vector3 PositionVar;
        Vector3 Velocity;
        Vector3 VelocityVar;
        float Life;
        float LifeVar;
        float Mass;
        float MassVar;
        float Radius;
        float RadiusVar;
        float DRadius;
        float DRadiusVar;
        float Drag;
        float DragVar;
        float Damp;
        float DampVar;
        float SpewRate;
        float SpewRateLimit;
        float Gravity;
        float Height;
        float Intensity;
        int Color;
        int DAlpha;
        int DAlphaVar;
        int DRotation;
        int DRotationVar;
        int TexFrameStart;
        int TexFrameEnd;
        int InitialBlast;
        int BirthFlags;
        void(__cdecl *OnSparkAdded)(struct asSparkInfo *, struct asSparkPos *);
    };

    template<>
    void luaAddModule<module_particle>(LuaState L) {
        luaBind<asParticles>(L);
    }
}