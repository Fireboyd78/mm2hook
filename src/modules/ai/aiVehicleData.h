#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward declarations
    class aiVehicleData;

    // External declarations


    // Class definitions
    class aiVehicleData : public asNode {
    private:
        Vector3 Size;
        Vector3 MaxAng;
        Vector3 CG;
        int unknown60;
        int unknown64;
        int unknown68;
        int unknown72;
        int unknown76;
        int unknown80;
        int unknown84;
        int unknown88;
        int unknown92;
        int unknown96;
        int unknown100;
        int unknown104;
        int unknown108;
        int unknown112;
        int unknown116;
        int unknown120;
        int unknown124;
        int unknown128;
        float Mass;
        float Friction;
        float Elasticity;
        float MaxDamage;
        float PtxThresh;
        float Spring;
        float Damping;
        float RubberSpring;
        float RubberDamp;
        float Limit;
        int unknown172;
        int unknown176;
        int unknown180;
        int unknown184;

    public:
        //overrides
        AGE_API char* GetClassName() override           { return ageHook::Thunk<0x56F940>::Call<char*>(this); }
        AGE_API const char* GetDirName() override       { return ageHook::Thunk<0x56F950>::Call<const char*>(this); }
        AGE_API void FileIO(datParser& a1) override     { ageHook::Thunk<0x56F7C0>::Call<void>(this, a1); }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiVehicleData, asNode>("aiVehicleData")
                .endClass();
        }
    };

    // Lua initialization

}