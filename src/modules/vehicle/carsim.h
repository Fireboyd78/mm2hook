#pragma once
#include <modules\vehicle.h>

#include "transmission.h"
#include "engine.h"
#include "drivetrain.h"
#include "wheel.h"
#include "aero.h"
#include "axle.h"

namespace MM2
{
    // Forward declarations
    class vehCarSim;

    // External declarations
    extern class datParser;
    extern class lvlInstance;
    
    // Class definitions

    class vehCarSim : public asNode {
    protected:
        ageHook::Field<0x24C, float> _speedMPH;
        ageHook::Field<0x248, float> _speed;
        ageHook::Field<0x254, int> _drivetrainType;
        ageHook::Field<0x210, const Vector3> _resetPosition;
        ageHook::Field<0x1D0, lvlInstance *> _instance;
        ageHook::Field<0x2E0, vehTransmission> _transmission;
        ageHook::Field<0x25C, vehEngine> _engine;
        ageHook::Field<0x3D4, vehDrivetrain> _drivetrain;
        ageHook::Field<0x420, vehDrivetrain> _freetrainLeft;
        ageHook::Field<0x46C, vehDrivetrain> _freetrainRight;
        ageHook::Field<0x4B8, vehWheel> _whl0;
        ageHook::Field<0x724, vehWheel> _whl1;
        ageHook::Field<0x990, vehWheel> _whl2;
        ageHook::Field<0xBFC, vehWheel> _whl3;
        ageHook::Field<0xE68, vehAxle> _frontAxle;
        ageHook::Field<0xF04, vehAxle> _rearAxle;
        ageHook::Field<0x154C, float> _brake;
        ageHook::Field<0x1550, float> _handbrake;
        ageHook::Field<0x1554, float> _steering;
        ageHook::Field<0x14F0, vehAero> _aero;
    public:
        inline int getDrivetrainType(void) {
            return _drivetrainType.get(this);
        }

        inline void setDrivetrainType(int type) {
            if (type < 0 || type > 2) //0 = RWD, 1 = FWD, 2 = 4WD
                return;
            _drivetrainType.set(this, type);
        }


        inline float getSteering(void) {
            return _steering.get(this);
        }

        inline void setSteering(float steering) {
            _steering.set(this, steering);
        }

        inline float getBrake(void) {
            return _brake.get(this);
        }

        inline void setBrake(float brake) {
            _brake.set(this, brake);
        }

        inline float getHandbrake(void) {
            return _handbrake.get(this);
        }

        inline void setHandbrake(float handBrake) {
            _handbrake.set(this, handBrake);
        }

        inline float getSpeed(void) {
            return _speed.get(this);
        }

        inline float getSpeedMPH(void) {
            return _speedMPH.get(this);
        };

        inline const Vector3 getResetPosition(void) {
            return _resetPosition.get(this);
        }

        inline lvlInstance * getInstance(void) {
            return _instance.get(this);
        }

        inline vehTransmission * getTransmission(void) const {
            return _transmission.ptr(this);
        }

        inline vehEngine * getEngine(void) const {
            return _engine.ptr(this);
        }

        inline vehDrivetrain * getDrivetrain(void) const {
            return _drivetrain.ptr(this);
        }

        inline vehDrivetrain * getLeftFreetrain(void) const {
            return _freetrainLeft.ptr(this);
        }

        inline vehDrivetrain * getRightFreetrain(void) const {
            return _freetrainRight.ptr(this);
        }

        inline vehWheel * getWheel(int num) const {
            switch (num) {
            case 0:
                return _whl0.ptr(this);
            case 1:
                return _whl1.ptr(this);
            case 2:
                return _whl2.ptr(this);
            case 3:
                return _whl3.ptr(this);
            }
            return nullptr;
        }

        inline vehAero * getAero(void) const {
            return _aero.ptr(this);
        }

        inline vehAxle * getFrontAxle(void) const {
            return _frontAxle.ptr(this);
        }

        inline vehAxle * getRearAxle(void) const {
            return _rearAxle.ptr(this);
        }

        AGE_API vehCarSim()                                 { ageHook::Thunk<0x4CB660>::Call<void>(this); }
        AGE_API ~vehCarSim()                                { ageHook::Thunk<0x4CB8E0>::Call<void>(this); }

        AGE_API int BottomedOut()                           { return ageHook::Thunk<0x4CBB40>::Call<int>(this); }
        AGE_API int OnGround()                              { return ageHook::Thunk<0x4CBB00>::Call<int>(this); }
        AGE_API void ReconfigureDrivetrain()                { ageHook::Thunk<0x4CC0B0>::Call<void>(this); }
        AGE_API void SetHackedImpactParams()                { ageHook::Thunk<0x4CC080>::Call<void>(this); }
        AGE_API void RestoreImpactParams()                  { ageHook::Thunk<0x4CC050>::Call<void>(this); }
        AGE_API void SetResetPos(Vector3 * a1)              { ageHook::Thunk<0x4CC830>::Call<void>(this, a1); }
        
        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { ageHook::Thunk<0x4CC8E0>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x4CBA70>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser)  override
                                                            { ageHook::Thunk<0x4CCC70>::Call<void>(this); }
        AGE_API char* GetClassName() override               { return ageHook::Thunk<0x4CCF10>::Call<char*>(this); }
        AGE_API char const* GetDirName() override           { return ageHook::Thunk<0x4CBAF0>::Call<char const*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehCarSim, asNode>("vehCarSim")
                .addPropertyReadOnly("Drivetrain", &getDrivetrain)
                .addPropertyReadOnly("LeftFreetrain", &getLeftFreetrain)
                .addPropertyReadOnly("RightFreetrain", &getRightFreetrain)

                .addPropertyReadOnly("Aero", &getAero)
                .addPropertyReadOnly("FrontAxle", &getFrontAxle)
                .addPropertyReadOnly("RearAxle", &getRearAxle)
                .addPropertyReadOnly("Transmission", &getTransmission)
                .addPropertyReadOnly("ResetPosition", &getResetPosition)
                .addPropertyReadOnly("Engine", &getEngine)
                .addPropertyReadOnly("Speed", &getSpeedMPH)

                .addProperty("DrivetrainType", &getDrivetrainType, &setDrivetrainType)
                .addProperty("Steering", &getSteering, &setSteering)
                .addProperty("Brake", &getBrake, &setBrake)
                .addProperty("Handbrake", &getHandbrake, &setHandbrake)

                .addFunction("GetWheel", &getWheel)

                .addFunction("BottomedOut", &BottomedOut)
                .addFunction("OnGround", &OnGround)
                .addFunction("ReconfigureDrivetrain", &ReconfigureDrivetrain)
                .addFunction("SetHackedImpactParams", &SetHackedImpactParams)
                .addFunction("RestoreImpactParams", &RestoreImpactParams)
                .addFunction("SetResetPos", &SetResetPos)
            .endClass();
        }
    private:
        byte _buffer[0x1560];
    };

    // Lua initialization

}