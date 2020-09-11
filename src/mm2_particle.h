#pragma once
#include "mm2_common.h"
#include "mm2_base.h"
#include "mm2_rgl.h"

namespace MM2
{
    // Forward declarations
    struct asSparkPos;
    struct asMeshCardInfo;
    struct asMeshCardVertex;

    class asBirthRule;
    class asParticles;

    // External declarations
    extern class asNode;

    struct asMeshCardVertex {
        float x;
        float y;
        float z;
        float w;

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<asMeshCardVertex>("asMeshCardVertex")
                .addVariableRef("x", &asMeshCardVertex::x)
                .addVariableRef("y", &asMeshCardVertex::y)
                .addVariableRef("z", &asMeshCardVertex::z)
                .addVariableRef("w", &asMeshCardVertex::w)
                .endClass();
        }
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
        float SpewTimeLimit;
        float Gravity;
        float Height;
        float Intensity;
        ColorARGB Color;
        int DAlpha;
        int DAlphaVar;
        int DRotation;
        int DRotationVar;
        int TexFrameStart;
        int TexFrameEnd;
        int InitialBlast;
        int BirthFlags;
        void(__cdecl* OnSparkAdded)(struct asSparkInfo*, struct asSparkPos*);
    public:
        AGE_API asBirthRule(void) {
            scoped_vtable x(this);
            hook::Thunk<0x45ECE0>::Call<void>(this);
        }

        AGE_API virtual ~asBirthRule(void) {
            scoped_vtable x(this);
            hook::Thunk<0x45FBF0>::Call<void>(this);
        }

        //helpers
        inline std::tuple<byte, byte, byte, byte> getColorTuple(void) {
            return std::make_tuple(Color.r, Color.g, Color.b, Color.a);
        }

        inline void setColorTuple(std::tuple<byte, byte, byte, byte> setColor) {
            auto myColor = &this->Color;
            myColor->a = std::get<3>(setColor);
            myColor->r = std::get<0>(setColor);
            myColor->g = std::get<1>(setColor);
            myColor->b = std::get<2>(setColor);
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<asBirthRule, asNode>("asBirthRule")
                .addConstructor(LUA_ARGS())
                .addVariableRef("Position", &asBirthRule::Position)
                .addVariableRef("PositionVar", &asBirthRule::PositionVar)
                .addVariableRef("Velocity", &asBirthRule::Velocity)
                .addVariableRef("VelocityVar", &asBirthRule::VelocityVar)
                .addVariableRef("Life", &asBirthRule::Life)
                .addVariableRef("LifeVar", &asBirthRule::LifeVar)
                .addVariableRef("Mass", &asBirthRule::Mass)
                .addVariableRef("MassVar", &asBirthRule::MassVar)
                .addVariableRef("Radius", &asBirthRule::Radius)
                .addVariableRef("RadiusVar", &asBirthRule::RadiusVar)
                .addVariableRef("DRadius", &asBirthRule::DRadius)
                .addVariableRef("DRadiusVar", &asBirthRule::DRadiusVar)
                .addVariableRef("Drag", &asBirthRule::Drag)
                .addVariableRef("DragVar", &asBirthRule::DragVar)
                .addVariableRef("Damp", &asBirthRule::Damp)
                .addVariableRef("DampVar", &asBirthRule::DampVar)
                .addVariableRef("SpewRate", &asBirthRule::SpewRate)
                .addVariableRef("SpewTimeLimit", &asBirthRule::SpewTimeLimit)
                .addVariableRef("Gravity", &asBirthRule::Gravity)
                .addVariableRef("Height", &asBirthRule::Height)
                .addVariableRef("Intensity", &asBirthRule::Intensity)
                .addProperty("Color", &getColorTuple, &setColorTuple)
                .addVariableRef("DAlpha", &asBirthRule::DAlpha)
                .addVariableRef("DAlphaVar", &asBirthRule::DAlphaVar)
                .addVariableRef("DRotation", &asBirthRule::DRotation)
                .addVariableRef("DRotationVar", &asBirthRule::DRotationVar)
                .addVariableRef("TexFrameStart", &asBirthRule::TexFrameStart)
                .addVariableRef("TexFrameEnd", &asBirthRule::TexFrameEnd)
                .addVariableRef("InitialBlast", &asBirthRule::InitialBlast)
                .addVariableRef("BirthFlags", &asBirthRule::BirthFlags)
                .endClass();
        }
    };

    ASSERT_SIZEOF(asBirthRule, 0xB4);

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
        ANGEL_ALLOCATOR;

        AGE_API asParticles(void) {
            scoped_vtable x(this);
            hook::Thunk<0x460EB0>::Call<void>(this);
        }

        AGE_API virtual ~asParticles(void) {
            scoped_vtable x(this);
            hook::Thunk<0x4619E0>::Call<void>(this);
        }

        void InitLua(int count, int wt, int ht) {
            Init(count, wt, ht, 4, nullptr);
        }

        AGE_API void Init(int nParticles, int nTilesW, int nTilesH, int nVertices, asMeshCardVertex *mesh)
                                                            { hook::Thunk<0x460FB0>::Call<void>(this, nParticles, nTilesW, nTilesH, nVertices, mesh); }
        AGE_API void Blast(int a1, asBirthRule *rule)       { hook::Thunk<0x461490>::Call<void>(this, a1, rule); }
        AGE_API void Reset()                                { hook::Thunk<0x461040>::Call<void>(this); }
        AGE_API void Update()                               { hook::Thunk<0x4610F0>::Call<void>(this); }
        AGE_API void SetTexture(const char *tex)            { hook::Thunk<0x461090>::Call<void>(this, tex); }
        AGE_API void SetTexture(gfxTexture *tex)            { hook::Thunk<0x461050>::Call<void>(this, tex); }

        //member hlepers
        inline asBirthRule * getBirthRule(void) {
            return pBirthRule;
        }

        inline void setBirthRule(asBirthRule *rule) {
            pBirthRule = rule;
        }

        /*
            asParticles Virtuals
        */
        virtual AGE_API void Cull()                         { hook::Thunk<0x4615A0>::Call<void>(this); }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<asParticles>("asParticles")
                .addConstructor(LUA_ARGS())
                .addFunction("Blast", &Blast)
                .addFunction("Update", &Update)
                .addFunction("Init", &InitLua)
                .addFunction("Reset", &Reset)
                .addFunction("Cull", &Cull)
                .addFunction<void (asParticles::*)(const char* a1)>("SetTexture", &SetTexture)
                .addProperty("BirthRule", &getBirthRule, &setBirthRule)
            .endClass();
        }
    };

    template<>
    void luaAddModule<module_particle>(LuaState L) {
        luaBind<asMeshCardVertex>(L);
        luaBind<asBirthRule>(L);
        luaBind<asParticles>(L);
    }
}