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
        bool HasLights;
        bool Active;
        int LightCount;
        ltLight *ltLightPool;
        Vector3 lightPositions[4];
        float RotationRate;
        void* LensFlarePtr;

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
            if (index >= 4)
                index = 3;
            return &ltLightPool[index];
        }

        inline Vector3 getLightPosition(int index) const {
            //cap index
            if(index < 0)
                index = 0;
            if (index >= 4)
                index = 3;
            return lightPositions[index];
        }

        inline void setLightPosition(int index, Vector3 position) {
            //cap index
            if (index < 0)
                index = 0;
            if (index >= 4)
                index = 3;
            lightPositions[index] = position;
        }

        //member funcs
        AGE_API bool Init()                          { return ageHook::Thunk<0x4D6680>::Call<bool>(this); }
        AGE_API bool AddLight(Vector3* position, Vector3* color)                    
                                                     { return ageHook::Thunk<0x4D6740>::Call<bool>(this, position, color); }
        AGE_API void Reset()                         { ageHook::Thunk<0x4D6820>::Call<void>(this); }
        AGE_API void Update()                        { ageHook::Thunk<0x4D6830>::Call<void>(this); }
        AGE_API void Draw(Matrix34* a1)              { ageHook::Thunk<0x4D6880>::Call<void>(this, a1); }

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
    ASSERT_SIZEOF(vehSiren, 0x44);


    // Lua initialization

}