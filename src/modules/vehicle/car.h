#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward delcarations
    class vehCar;

    // External declarations
    extern class dgPhysEntity;
    extern class lvlInstance;
    extern class phInertialCS;

    extern class vehCarAudioContainer;
    extern class vehCarDamage;
    extern class vehCarModel;
    extern class vehCarSim;
    extern class vehSplash;
    extern class vehTrailer;

    // Class definitions

    class vehCar : public dgPhysEntity {
        // vehCarSim: 0xB8 (size: ~0x1560)
    protected:
        ageHook::Field<0xC0, vehCarDamage *> _damage;
        ageHook::Field<0xB8, vehCarSim *> _sim;
        ageHook::Field<0xBC, vehCarModel *> _model;
        ageHook::Field<0xE0, vehSplash *> _splash;
        ageHook::Field<0x254, vehCarAudioContainer *> _audio;
        ageHook::Field<0xD8, vehTrailer *> _trailer;
    public:
        AGE_API vehCar(BOOL a1)                             { ageHook::Thunk<0x42BAB0>::Call<void>(this, a1); }
        AGE_API ~vehCar()                                   { ageHook::Thunk<0x42BCC0>::Call<void>(this); }

        static ageHook::Type<bool> sm_DrawHeadlights;

        inline vehCarDamage * getCarDamage(void) const {
            return _damage.get(this);
        };

        inline vehCarSim * getCarSim(void) const {
            return _sim.get(this);
        }

        inline vehCarModel * getModel(void) const {
            return _model.get(this);
        }

        inline vehSplash * getSplash(void) const {
            return _splash.get(this);
        }

        inline vehCarAudioContainer * getAudio(void) const {
            return _audio.get(this);
        }

        inline vehTrailer * getTrailer(void) const {
            return _trailer.get(this);
        }

        AGE_API void Reset()                                { ageHook::Thunk<0x42C330>::Call<void>(this); }

        AGE_API void ClearDamage()                          { ageHook::Thunk<0x42C450>::Call<void>(this); }
        AGE_API bool IsPlayer()                             { return ageHook::Thunk<0x42C890>::Call<bool>(this); }
        AGE_API void Init(char const* a1, int a2, int a3, bool a4, bool a5)
                                                            { ageHook::Thunk<0x42BE10>::Call<void>(this, a1, a2, a3, a4, a5); }
        AGE_API void InitAudio(char const* a1, int a2)      { ageHook::Thunk<0x42C1F0>::Call<void>(this, a1, a2); }
        AGE_API void SetDrivable(BOOL a1, BOOL a2)          { ageHook::Thunk<0x42C2C0>::Call<void>(this, a1, a2); }

        /*
            dgPhysEntity virtuals
        */

        AGE_API bool RequiresTerrainCollision() override
                                                            { return ageHook::Thunk<0x42CA90>::Call<bool>(this); }
        AGE_API lvlInstance* GetInst() override             { return ageHook::Thunk<0x42CA80>::Call<lvlInstance *>(this); }
        AGE_API phInertialCS* GetICS()                      { return ageHook::Thunk<0x42CA70>::Call<phInertialCS *>(this); }
        AGE_API void PostUpdate() override                  { ageHook::Thunk<0x42C8B0>::Call<void>(this); }
        AGE_API void Update() override                      { ageHook::Thunk<0x42C690>::Call<void>(this); }
        AGE_API void PreUpdate() override                   { ageHook::Thunk<0x42C480>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehCar, dgPhysEntity>("vehCar")
                .addConstructor(LUA_ARGS(bool))
                //properties
                .addPropertyReadOnly("vehCarDamage", &getCarDamage)
                .addPropertyReadOnly("vehCarSim", &getCarSim)
                .addPropertyReadOnly("vehCarModel", &getModel)
                .addPropertyReadOnly("vehSplash", &getSplash)
                .addPropertyReadOnly("Audio", &getAudio)
                .addPropertyReadOnly("Trailer", &getTrailer)

                //functions
                .addFunction("Init", &Init)
                .addFunction("InitAudio", &InitAudio)
                .addFunction("Reset", &Reset)
                .addFunction("ClearDamage", &ClearDamage)
                .addFunction("SetDrivable", &SetDrivable, LUA_ARGS(bool,bool))
                .addFunction("IsPlayer", &IsPlayer)
            .endClass();
        }
    private:
        byte _buffer[0x25C];
    };

    // Lua initialization

}