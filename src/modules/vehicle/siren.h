#pragma once
#include <modules\vehicle.h>
#include <mm2_gfx.h>

namespace MM2
{
    // Forward declarations
    class vehSiren;

    // External declarations
    extern class ltLight;

    // Class definitions
    class vehSiren
    {
    public:
        static float SirenRotationSpeed;
        static float SirenRotationStyle;
    private:
        bool HasLights;
        bool Active;
        int LightCount;
        ltLight* ltLightPool;
        Vector3 lightPositions[4]; //we didn't use this field because it conflicts with the extra light positions which doesn't make them read their mtx files properly.
        float RotationRate;
        ltLensFlare* LensFlare;
        //EXTRA FIELD. The hook expands on this class, this is only possible because it's only used like a pointer in the original MM code
        Vector3 extraLightPositions[24]; //SRN0-23
    public:
        //lua helpers
        inline bool getHasLights() const {
            return this->HasLights;
        }

        inline bool getActive() const {
            return this->Active;
        }

        inline void setActive(bool active) {
            this->Active = active;
        }

        inline int getLightCount() const {
            return this->LightCount;
        }

        inline ltLight* getLight(int index) const {
            //cap index
            if (index < 0)
                index = 0;
            if (index >= 24)
                index = 23;
            return &this->ltLightPool[index];
        }

        inline Vector3 getLightPosition(int index) const {
            //cap index
            if(index < 0)
                index = 0;
            if (index >= 24)
                index = 23;
            return this->extraLightPositions[index];
        }

        inline void setLightPosition(int index, Vector3 position) {
            //cap index
            if (index < 0)
                index = 0;
            if (index >= 24)
                index = 23;
            this->extraLightPositions[index] = position;
        }

        inline float getRotationRate() const
        {
            return this->RotationRate;
        }

        inline ltLensFlare * getLensFlare()
        {
            return this->LensFlare;
        }

        ANGEL_ALLOCATOR

        AGE_API vehSiren(void)
        {
            scoped_vtable x(this);
            hook::Thunk<0x4D6600>::Call<void>(this);
        }

        AGE_API ~vehSiren(void)
        {
            scoped_vtable x(this);
            hook::Thunk<0x4D6630>::Call<void>(this);
        }

        //member funcs
        AGE_API bool Init()
        {
            return hook::Thunk<0x4D6680>::Call<bool>(this);
        }

        AGE_API bool AddLight(const Vector3& position, const Vector3& color)
        { 
            if (this->ltLightPool != nullptr && this->LightCount < 24)
            {
                float rotationStyle = vehSiren::SirenRotationStyle;

                this->HasLights = true;
                ltLightPool[LightCount].Type = 1;
                ltLightPool[LightCount].Color = color;
                extraLightPositions[LightCount] = position;
                ltLightPool[LightCount].SpotExponent = 3.f;
                ltLightPool[LightCount].Direction = Vector3(1.f, 0.f, 0.f);
                ltLightPool[LightCount].Direction.RotateY(LightCount * rotationStyle);
                ++this->LightCount;
                return true;
            }

            return false;
        }

        AGE_API void Reset()
        {
            hook::Thunk<0x4D6820>::Call<void>(this);
        }

        AGE_API void Update()
        {
            hook::Thunk<0x4D6830>::Call<void>(this);
        }

        AGE_API void Draw(const Matrix34& carMatrix)
        {
            hook::Thunk<0x4D6880>::Call<void>(this, &carMatrix);
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