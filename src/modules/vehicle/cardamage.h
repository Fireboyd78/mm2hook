#pragma once
#include <modules\vehicle.h>
#include <modules\effects.h>

namespace MM2
{
    // Forward declarations
    class vehCarDamage;

    // External declarations
    extern class asBirthRule;
    extern class asParticles;
    extern class fxShardManager;
    extern class asLineSparks;

    // Class definitions

    class vehCarDamage : public asNode {
    private:
        datCallback* ImpactCB;
        datCallback* GameCallback;
        vehCar* vehCarPtr;
        bool DoublePivot;
        bool MirrorPivot;
        byte pad[2];
        int m_CurrentPivot;
        byte m_HasParticleTexture;
        bool EnableDamage;
        byte pad0[2];
        float CurrentDamage;
        float MaxDamage;
        float MedDamage;
        float ImpactThreshold;
        float RegenerateRate;
        byte ImpactsTable[768];
        float unk_344;
        Vector3 SmokeOffset;
        Vector3 SmokeOffset2;
        asParticles* Particles;
        byte ExhaustFlags;
        byte pad1[3];
        Vector3 Exhaust2Pos;
        Vector3 Exhaust1Pos;
        float SparkMultiplier;
        asLineSparks* Sparks;
        float TextelDamageRadius;
        fxShardManager* ShardMgr;
        float unk_390;
        bool m_TexelDamageFlag;
        byte pad2[3];
        Vector3 LastImpactPos;
    public:
        inline float getCurDamage(void) {
            return this->CurrentDamage;
        }

        inline float getMedDamage(void) {
            return this->MedDamage;
        }

        inline float getMaxDamage(void) {
            return this->MaxDamage;
        }
    public:
        AGE_API vehCarDamage()                              { ageHook::Thunk<0x4CA380>::Call<void>(this); }
        AGE_API ~vehCarDamage()                             { ageHook::Thunk<0x4CA530>::Call<void>(this); }

        AGE_API void AddDamage(float a1)                    { ageHook::Thunk<0x4CAEC0>::Call<void>(this, a1); }
        AGE_API void ClearDamage()                          { ageHook::Thunk<0x4CAE80>::Call<void>(this); }

        /*
            vehCarDamage virtuals
        */

        virtual AGE_API float GetDamageModifier(phCollider* a1) 
                                                            { return ageHook::Thunk<0x4CB650>::Call<float>(this, a1); }
        
        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { ageHook::Thunk<0x5B2C30>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x4CAE60>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser) override
                                                            { ageHook::Thunk<0x4CB400>::Call<void>(this); }
        AGE_API char* GetClassName() override               { return ageHook::Thunk<0x4CB640>::Call<char*>(this); }
        AGE_API char const* GetDirName() override           { return ageHook::Thunk<0x4CA5F0>::Call<char const*>(this); }
        
        //fields
        static ageHook::Type<asBirthRule*> EngineSmokeRule;

        inline asParticles* getParticles(void) {
            return this->Particles;
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehCarDamage>("vehCarDamage")
                .addFunction("Reset", &Reset)
                .addFunction("AddDamage", &AddDamage)
                .addFunction("ClearDamage", &ClearDamage)

                .addVariableRef("DamageAmount", &vehCarDamage::CurrentDamage)

                .addVariableRef("Enabled", &vehCarDamage::EnableDamage)
                .addVariableRef("MedDamage", &vehCarDamage::MedDamage)
                .addVariableRef("MaxDamage", &vehCarDamage::MaxDamage)
                .addVariableRef("ImpactThreshold", &vehCarDamage::ImpactThreshold)
                .addVariableRef("RegenerateRate", &vehCarDamage::RegenerateRate)
                .addVariableRef("SmokeOffset", &vehCarDamage::SmokeOffset)
                .addVariableRef("SmokeOffset2", &vehCarDamage::SmokeOffset2)
                .addVariableRef("TextelDamageRadius", &vehCarDamage::TextelDamageRadius)
                .addVariableRef("DoublePivot", &vehCarDamage::DoublePivot)
                .addVariableRef("MirrorPivot", &vehCarDamage::MirrorPivot)

                .addPropertyReadOnly("Particles", &getParticles)
                .addStaticProperty("EngineSmokeRule", [] { return EngineSmokeRule.get(); })
            .endClass();
        }
    };

    ASSERT_SIZEOF(vehCarDamage, 0x3A4);

    // Lua initialization

}