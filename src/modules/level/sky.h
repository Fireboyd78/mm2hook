#pragma once
#include <modules\level.h>

namespace MM2
{
    // Forward declarations


    // External declarations


    // Class definitions
    class lvlSky : public asCullable
    {
    private:
        modStatic* Model;
        modShader** Shaders;
        float HatYOffset;
        float YMultiplier;
        float RotationRate;
        float CurrentRotation;
        int TimeWeatherType;
        int ShaderCount;
        bool DrawEnabled;
        ColorARGB FogColors[16];
        short FogNearClip[16];
        short FogFarClip[16];
    public:
        std::tuple<byte, byte, byte, byte> getFogColor(int index) {
            if (index < 0 || index > 16)
                return std::make_tuple((byte)0, (byte)0, (byte)0, (byte) 0);

            auto color = FogColors[index];
            return std::make_tuple(color.r, color.g, color.b, color.a);
        }

        short getFogNearClip(int index) {
            return (index < 0 || index > 16) ? 0 : this->FogNearClip[index];
        }

        short getFogFarClip(int index) {
            return (index < 0 || index > 16) ? 0 : this->FogFarClip[index];
        }

        void setFogNearClip(int index, short clip) {
            if (index < 0 || index > 16)
                return;
            this->FogNearClip[index] = clip;
        }

        void setFogFarClip(int index, short clip) {
            if (index < 0 || index > 16)
                return;
            this->FogFarClip[index] = clip;
        }

        void setFogColor(int index, byte r, byte g, byte b, byte a) {
            if (index < 0 || index > 16)
                return;

            auto myColor = &this->FogColors[index];
            myColor->a = a;
            myColor->r = r;
            myColor->g = g;
            myColor->b = b;
        }
    public:

        /*
            lvlSky Virtuals
        */
        AGE_API virtual void Reset()                        { hook::Thunk<0x464C90>::Call<void>(this); }
        AGE_API virtual void Update()                       { hook::Thunk<0x464F90>::Call<void>(this); }
        AGE_API virtual void Draw()                         { hook::Thunk<0x464FB0>::Call<void>(this); }
        AGE_API virtual void DrawWithNoZState()             { hook::Thunk<0x4650B0>::Call<void>(this); }
        AGE_API virtual void DrawHat()                      { hook::Thunk<0x465140>::Call<void>(this); }
        

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<lvlSky, asCullable>("lvlSky")
                .addVariableRef("TimeWeatherType", &lvlSky::TimeWeatherType)
                .addVariableRef("DrawEnabled", &lvlSky::DrawEnabled)
                .addVariableRef("CurrentRotation", &lvlSky::CurrentRotation)
                .addVariableRef("RotationSpeed", &lvlSky::RotationRate)
                .addVariableRef("RotationRate", &lvlSky::RotationRate)
                .addVariableRef("YMultiplier", &lvlSky::YMultiplier)
                .addVariableRef("HatYOffset", &lvlSky::HatYOffset)
                .addFunction("GetFogColor", &getFogColor)
                .addFunction("GetFogNearClip", &getFogNearClip)
                .addFunction("GetFogFarClip", &getFogFarClip)
                .addFunction("SetFogColor", &setFogColor)
                .addFunction("SetFogNearClip", &setFogNearClip)
                .addFunction("SetFogFarClip", &setFogFarClip)
                .endClass();
        }
    };

    ASSERT_SIZEOF(lvlSky, 0xA8);

    // Lua initialization

}