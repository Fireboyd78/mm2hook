#pragma once
#include "mm2_common.h"
#include "mm2_inst.h"
#include "mm2_game.h"

namespace MM2
{
    // Forward declarations
    class vehCar;
    class vehCarAudioContainer;
    class vehPoliceCarAudio;

    class vehCarDamage : public asNode {
    public:
        //vehCarDamage virtual
        virtual AGE_API float GetDamageModifier(phCollider* a1) { return ageHook::Thunk<0x4CB650>::Call<float>(this, a1); }
        
        //asNode virtuals
        virtual AGE_API char const* GetDirName() override { return ageHook::Thunk<0x4CA5F0>::Call<char const*>(this); }
        virtual AGE_API char* GetClassName() override { return ageHook::Thunk<0x4CB640>::Call<char*>(this); }
        virtual AGE_API void FileIO(datParser &parser) override { ageHook::Thunk<0x4CB400>::Call<void>(this); }
        virtual AGE_API void Reset() override { ageHook::Thunk<0x4CAE60>::Call<void>(this); }
        virtual AGE_API void Update() override { ageHook::Thunk<0x5B2C30>::Call<void>(this); }

        //non virtuals
        AGE_API void AddDamage(float a1) { ageHook::Thunk<0x4CAEC0>::Call<void>(this, a1); }
        AGE_API void ClearDamage() { ageHook::Thunk<0x4CAE80>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehCarDamage>("vehCarDamage")
                .addFunction("Reset", &Reset)
                .addFunction("AddDamage", &AddDamage)
                .addFunction("ClearDamage", &ClearDamage)
            .endClass();
        }
    };
    
    //TODO : inheritance
    class vehCar : public dgPhysEntity {
        // vehCarSim: 0xB8 (size: ~0x1560)
    protected:
        ageHook::Field<0xC0, vehCarDamage> _damage;
        
    public:
        //dgPhysEntity virtuals
        virtual bool RequiresTerrainCollision() override { return ageHook::Thunk<0x42CA90>::Call<bool>(this); }
        virtual lvlInstance* GetInst() override { return ageHook::Thunk<0x42CA80>::Call<lvlInstance *>(this); }
        virtual phInertialCs* GetICS() { return ageHook::Thunk<0x42CA70>::Call<phInertialCs *>(this); }
        virtual void PostUpdate() override { ageHook::Thunk<0x42C8B0>::Call<void>(this); }
        virtual void Update() override { ageHook::Thunk<0x42C690>::Call<void>(this); }
        virtual void PreUpdate() override { ageHook::Thunk<0x42C480>::Call<void>(this); }

        //vehCar
        AGE_API void SetDrivable(bool a1, bool a2) {
            ageHook::Thunk<0x42C2C0>::Call<void>(this, a1, a2);
        }

        AGE_API void ClearDamage() {
            ageHook::Thunk<0x42C450>::Call<void>(this);
        }

        AGE_API bool IsPlayer() {
            return ageHook::Thunk<0x42C890>::Call<int>(this);
        }

        //todo: this is broken
        inline vehCarDamage* getCarDamage(void) const {
            return _damage.ptr(this);
        };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehCar>("vehCar")
                .addFunction("getCarDamage", &getCarDamage)
                .addFunction("GetInst", &GetInst)
                .addFunction("ClearDamage", &ClearDamage)
                .addFunction("SetDrivable", &SetDrivable)
                .addFunction("IsPlayer", &IsPlayer)
            .endClass();
        }
    private:
        byte _buffer[0x25C];
    };

    class vehCarAudioContainer {
    public:
        AGE_API static void SetSirenCSVName(LPCSTR name) {
            ageHook::StaticThunk<0x4D0C80>::Call<void>(name);
        };
    };

    class vehPoliceCarAudio {
    public:
        static ageHook::Type<int> iNumCopsPursuingPlayer;
    };
}