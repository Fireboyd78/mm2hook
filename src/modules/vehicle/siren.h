#pragma once
#include <modules\vehicle.h>
#include <mm2_gfx.h>

namespace MM2
{
    // Forward declarations


    // External declarations
    extern class ltLight;

    // Class definitions
    class vehSiren
    {
    public:
        static float SirenRotationSpeed;
        static int SirenLightStyle;
    public:
        bool HasLights;
        bool Active;
        int LightCount;
        ltLight* ltLightPool;
        Vector3 lightPositions[4]; //we didn't use this field because it conflicts with the extra light positions which doesn't make them read their mtx files properly.
        float RotationRate;
        ltLensFlare* LensFlarePtr;
        //EXTRA FIELD. The hook expands on this class, this is only possible because it's only used like a pointer in the original MM code
        Vector3 extraLightPositions[24]; //SRN0-23

        //lua helpers
        inline bool getHasLights() const {
            return HasLights;
        }

        inline int getLightCount() const {
            return LightCount;
        }

        inline ltLight* getLight(int index) const {
            //cap index
            if (index < 0)
                index = 0;
            if (index >= 24)
                index = 23;
            return &ltLightPool[index];
        }

        inline Vector3 getLightPosition(int index) const {
            //cap index
            if(index < 0)
                index = 0;
            if (index >= 24)
                index = 23;
            return extraLightPositions[index];
        }

        inline void setLightPosition(int index, Vector3 position) {
            //cap index
            if (index < 0)
                index = 0;
            if (index >= 24)
                index = 23;
            extraLightPositions[index] = position;
        }

        //member funcs
        AGE_API bool Init()
        {
            if (this->ltLightPool == nullptr)
            {
                this->ltLightPool = new ltLight[24];
            }
            if (this->LensFlarePtr == nullptr)
            {
                this->LensFlarePtr = new ltLensFlare(0x14);
            }
            return true;
        }

        AGE_API bool AddLight(Vector3* position, Vector3* color)                    
        { 
            if (this->ltLightPool != nullptr && this->LightCount < 24)
            {
                this->HasLights = true;
                ltLightPool[LightCount].Type = 1;
                ltLightPool[LightCount].Color = *color;
                extraLightPositions[LightCount] = *position;
                ltLightPool[LightCount].SpotExponent = 3.f;
                ltLightPool[LightCount].Direction = Vector3(1.f, 0.f, 0.f);
                ltLightPool[LightCount].Direction.RotateY(LightCount * 1.5707964f);
                ++this->LightCount;
                return true;
            }

            return false;
        }

        AGE_API void Reset()
        {
            this->Active = false;
        }

        AGE_API void Update()
        {
            float rotationAmount = vehSiren::SirenRotationSpeed;

            if (this->ltLightPool != nullptr && this->Active)
            {
                for (int i = 0; i < this->LightCount; i++)
                {
                    this->ltLightPool[i].Direction.RotateY(datTimeManager::Seconds * this->RotationRate * rotationAmount);
                }
            }
        }

        AGE_API void Draw(Matrix34* a1)
        {
            if (this->ltLightPool == nullptr)
                return;

            ltLight::DrawGlowBegin();
            for (int i = 0; i < LightCount; i++)
            {
                auto light = &this->ltLightPool[i];
                auto lightPos = this->extraLightPositions[i];

                float lX = lightPos.Y * a1->m10 + lightPos.Z * a1->m20 + lightPos.X * a1->m00 + a1->m30;
                float lY = lightPos.Y * a1->m11 + lightPos.Z * a1->m21 + lightPos.X * a1->m01 + a1->m31;
                float lZ = lightPos.Y * a1->m12 + lightPos.Z * a1->m22 + lightPos.X * a1->m02 + a1->m32;
                light->Position = Vector3(lX, lY, lZ);

                Vector3* someCameraThing = (Vector3*)0x685490;
                light->DrawGlow(someCameraThing);
            }
            ltLight::DrawGlowEnd();

            //draw lensFlare
            hook::Thunk<0x4D6880>::Call<void>(this, a1);
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehSiren>("vehSiren")
                //ctor
                .addConstructor(LUA_ARGS())
                //variables
                .addPropertyReadOnly("HasLights", &getHasLights)
                .addPropertyReadOnly("LightCount", &getLightCount)
                .addVariableRef("Active", &vehSiren::Active)
                .addVariableRef("RotationRate", &vehSiren::RotationRate)

                //statics

                //lua members
                .addFunction("GetLight", &getLight)
                .addFunction("GetLightPosition", &getLightPosition)
                .addFunction("SetLightPosition", &setLightPosition)

                //members
                .addFunction("Init", &Init)
                .addFunction("AddLight", &AddLight)
                .addFunction("Reset", &Reset)
                .addFunction("Draw", &Draw)
                .addFunction("Update", &Update)
              

                .endClass();
        }
    };
    ASSERT_SIZEOF(vehSiren, 0x44 + 0x120); //+1 extra field


    // Lua initialization

}