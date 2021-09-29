#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
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
    extern class vehSiren;
    extern class vehGyro;
    extern class vehStuck;
    extern class vehWheelPtx;

    // Class definitions

    class vehCar : public dgPhysEntity {
        byte _buffer[0x25C];
    protected:
        hook::Field<0xC0, vehCarDamage *> _damage;
        hook::Field<0xB8, vehCarSim *> _sim; // size: ~0x1560
        hook::Field<0xBC, vehCarModel *> _model;
        hook::Field<0xE0, vehSplash *> _splash;
        hook::Field<0x254, vehCarAudioContainer *> _audio;
        hook::Field<0xD8, vehTrailer *> _trailer;
        hook::Field<0xC8, vehSiren *> _siren;
        hook::Field<0xC4, vehWheelPtx *> _wheelPtx;
        hook::Field<0xD0, vehGyro *> _gyro;
        hook::Field<0xCC, vehStuck*> _stuck;
    public:
        AGE_API vehCar(BOOL a1)                             { hook::Thunk<0x42BAB0>::Call<void>(this, a1); }
        AGE_API ~vehCar()                                   { hook::Thunk<0x42BCC0>::Call<void>(this); }

        static hook::Type<bool> sm_DrawHeadlights;

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

        inline vehSiren * getSiren(void) const {
            return _siren.get(this);
        }

        inline vehWheelPtx * getWheelPtx(void) const {
            return _wheelPtx.get(this);
        }

        inline vehGyro * getGyro(void) const {
            return _gyro.get(this);
        }

        inline vehStuck* getStuck(void) const {
            return _stuck.get(this);
        }

        AGE_API void Reset()                                { hook::Thunk<0x42C330>::Call<void>(this); }

        AGE_API void ClearDamage()                          { hook::Thunk<0x42C450>::Call<void>(this); }
        AGE_API bool IsPlayer()                             { return hook::Thunk<0x42C890>::Call<bool>(this); }
        AGE_API void Init(char const *a1, int a2, int a3, bool a4, bool a5)
                                                            { hook::Thunk<0x42BE10>::Call<void>(this, a1, a2, a3, a4, a5); }
        AGE_API void InitAudio(char const *a1, int a2)      { hook::Thunk<0x42C1F0>::Call<void>(this, a1, a2); }
        AGE_API void SetDrivable(bool drivable, int mode)   { hook::Thunk<0x42C2C0>::Call<void>(this, drivable, mode); }

        /*
            dgPhysEntity virtuals
        */

        AGE_API bool RequiresTerrainCollision() override
                                                            { return hook::Thunk<0x42CA90>::Call<bool>(this); }
        AGE_API lvlInstance* GetInst() override             { return hook::Thunk<0x42CA80>::Call<lvlInstance *>(this); }
        AGE_API phInertialCS* GetICS()                      { return hook::Thunk<0x42CA70>::Call<phInertialCS *>(this); }
        AGE_API void PostUpdate() override                  { hook::Thunk<0x42C8B0>::Call<void>(this); }
        AGE_API void Update() override                      { hook::Thunk<0x42C690>::Call<void>(this); }
        AGE_API void PreUpdate() override                   { hook::Thunk<0x42C480>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehCar, dgPhysEntity>("vehCar")
                .addConstructor(LUA_ARGS(bool))
                //properties
                .addPropertyReadOnly("CarDamage", &getCarDamage)
                .addPropertyReadOnly("CarSim", &getCarSim)
                .addPropertyReadOnly("Model", &getModel)
                .addPropertyReadOnly("Splash", &getSplash)
                .addPropertyReadOnly("Audio", &getAudio)
                .addPropertyReadOnly("Trailer", &getTrailer)
                .addPropertyReadOnly("Siren", &getSiren)
                .addPropertyReadOnly("WheelPtx", &getWheelPtx)
                .addPropertyReadOnly("Gyro", &getGyro)
                .addPropertyReadOnly("Stuck", &getStuck)

                //functions
                .addFunction("Init", &Init)
                .addFunction("InitAudio", &InitAudio)
                .addFunction("Reset", &Reset)
                .addFunction("ClearDamage", &ClearDamage)
                .addFunction("SetDrivable", &SetDrivable, LUA_ARGS(bool,int))
                .addFunction("IsPlayer", &IsPlayer)
            .endClass();
        }
    };

    // Lua initialization

}