#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehCarDamage;

    // External declarations
    extern class asBirthRule;
    extern class asParticles;

    // Class definitions

    class vehCarDamage : public asNode {
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
            return *getPtr<asParticles*>(this, 0x360);
        }

        inline float getMaxDamage(void) {
            return *getPtr<float>(this, 0x34);
        }

        inline void setMaxDamage(float maxDamage) {
            auto dmgPtr = getPtr<float>(this, 0x34);
            *dmgPtr = maxDamage;
        }

        inline float getMedDamage(void) {
            return *getPtr<float>(this, 0x38);
        }

        inline void setMedDamage(float medDamage) {
            auto dmgPtr = getPtr<float>(this, 0x38);
            *dmgPtr = medDamage;
        }

        inline float getCurDamage(void) {
            return *getPtr<float>(this, 0x30);
        }

        inline void setCurDamage(float curDamage) {
            auto dmgPtr = getPtr<float>(this, 0x30);
            *dmgPtr = curDamage;
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehCarDamage>("vehCarDamage")
                .addFunction("Reset", &Reset)
                .addFunction("AddDamage", &AddDamage)
                .addFunction("ClearDamage", &ClearDamage)
                .addProperty("CurDamage", &getCurDamage, &setCurDamage)
                .addProperty("MedDamage", &getMedDamage, &setMedDamage)
                .addProperty("MaxDamage", &getMaxDamage, &setMaxDamage)
                .addPropertyReadOnly("Particles", &getParticles)
                .addStaticProperty("EngineSmokeRule", [] { return EngineSmokeRule.get(); })
            .endClass();
        }
    private:
        byte _buffer[0x3A4];
    };

    // Lua initialization

}