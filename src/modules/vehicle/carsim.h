#pragma once
#include <modules\vehicle.h>
#include <mm2_model.h>

#include "transmission.h"
#include "engine.h"
#include "drivetrain.h"
#include "wheel.h"
#include "aero.h"
#include "axle.h"
#include "suspension.h"

namespace MM2
{
    // Forward declarations
    class vehCarSim;

    // External declarations
    extern class datParser;
    extern class lvlInstance;
    extern class phInertialCS;
    extern class phCollider;
    extern class vehCarModel;

    // Class definitions

    class vehCarSim : public asNode {
    protected:
        phInertialCS InertialCS;
        phCollider *ColliderPtr;
        lvlInstance *LvlInstancePtr;
        Matrix34 WorldMatrix;
        Vector3 CenterOfGravity;
        Vector3 ResetPosition;
        Vector3 InertiaBoxCopy; //Copied on Init, used for aiVehiclePlayer distance calculations
        Vector3 InertiaBox;
        int unknown564;
        float BoundFriction;
        float BoundElasticity;
        int BoundPtr;
        float Mass;
        float Speed;
        float SpeedInMph;
        float ResetRotation;
        int DrivetrainType;  //0 = RWD, 1 = FWD, 2 = 4WD
        int ConfiguredDrivetrainType;  //0 = RWD, 1 = FWD, 2 = 4WD
        vehEngine Engine;
        vehTransmission Transmission;
        vehDrivetrain PrimaryDrivetrain;
        vehDrivetrain FreetrainLeft;
        vehDrivetrain FreetrainRight;
        vehWheel WheelFrontLeft;
        vehWheel WheelFrontRight;
        vehWheel WheelBackLeft;
        vehWheel WheelBackRight;
    public:
        vehAxle AxleFront;
        vehAxle AxleRear;
        vehSuspension ShaftSuspensions[2]; //SHAFT2-3
        vehSuspension ArmSuspensions[4]; //ARM0-4
        vehSuspension ShockSuspensions[4]; //SHOCK0-4
    protected:
        vehAero Aero;
        float CarFrictionHandling;
        int unknown5440;
        float unknown5444;
        int unknown5448;
        float BrakeInput;
        float HandBrakeInput;
        float SteeringInput;
        float SSSValue;
        float SSSThreshold;
    public:
        //EXTRA FIELDS. The hook expands on this class, this is only possible because it's only used like a pointer in the original MM code
        //These are the position differences from (WHL5-WHL3) and (WHL4-WHL2) / (TWHL5-TWHL3) and (TWHL4-TWHL2) for trailers
        Vector3 BackBackLeftWheelPosDiff;
        Vector3 BackBackRightWheelPosDiff;
        Vector3 TrailerBackBackLeftWheelPosDiff;
        Vector3 TrailerBackBackRightWheelPosDiff;
    public:
        inline float getBoundFriction(void) {
            return this->BoundFriction;
        }

        inline void setBoundFriction(float friction) {
            this->BoundFriction = friction;
            this->RestoreImpactParams(); //apply
        }

        inline float getBoundElasticity(void) {
            return this->BoundElasticity;
        }

        inline void setBoundElasticity(float elasticity) {
            this->BoundElasticity = elasticity;
            this->RestoreImpactParams(); //apply
        }

        inline Vector3 getInertiaBox(void) {
            return this->InertiaBox;
        }

        inline void setInertiaBox(Vector3 inertiaBox) {
            this->InertiaBox = inertiaBox;
            this->InertiaBoxCopy = inertiaBox;
        }

        inline void setAndApplyInertiaBox(Vector3 inertiaBox) {
            this->setInertiaBox(inertiaBox);
            
            auto box = this->getInertiaBox();
            this->getICS()->InitBoxMass(this->getMass(), box.X, box.Y, box.Z);
        }

        inline phInertialCS * getICS(void) {
            return &this->InertialCS;
        }

        inline Matrix34 * getWorldMatrix(void) {
            return &this->WorldMatrix;
        }

        inline float getMass(void) {
            return this->Mass;
        }

        inline void setMass(float mass) {
            this->Mass = mass;
        }

        inline void setAndApplyMass(float mass) {
            this->setMass(mass);
            
            auto box = this->getInertiaBox();
            this->getICS()->InitBoxMass(this->getMass(), box.X, box.Y, box.Z);
        }

        inline int getDrivetrainType(void) {
            return this->DrivetrainType;
        }

        inline void setDrivetrainType(int type) {
            if (type < 0 || type > 2) //0 = RWD, 1 = FWD, 2 = 4WD
                return;
            this->DrivetrainType = type;
        }

        inline float getSteering(void) {
            return this->SteeringInput;
        }

        inline void setSteering(float steering) {
            this->SteeringInput = steering;
        }

        inline float getBrake(void) {
            return this->BrakeInput;
        }

        inline void setBrake(float brake) {
            this->BrakeInput = brake;
        }

        inline float getHandbrake(void) {
            return this->HandBrakeInput;
        }

        inline void setHandbrake(float handBrake) {
            this->HandBrakeInput = handBrake;
        }

        inline float getSpeed(void) {
            return this->Speed;
        }

        inline float getSpeedMPH(void) {
            return this->SpeedInMph;
        };

        inline Vector3 getResetPosition(void) {
            return this->ResetPosition;
        }

        inline lvlInstance * getInstance(void) {
            return this->LvlInstancePtr;
        }

        inline vehTransmission * getTransmission(void) {
            return &this->Transmission;
        }

        inline vehEngine * getEngine(void) {
            return &this->Engine;
        }

        inline vehDrivetrain * getDrivetrain(void) {
            return &this->PrimaryDrivetrain;
        }

        inline vehDrivetrain * getLeftFreetrain(void) {
            return &this->FreetrainLeft;
        }

        inline vehDrivetrain * getRightFreetrain(void) {
            return &this->FreetrainRight;
        }

        inline vehWheel * getWheel(int num) {
            switch (num) {
            case 0:
                return &WheelFrontLeft;
            case 1:
                return &WheelFrontRight;
            case 2:
                return &WheelBackLeft;
            case 3:
                return &WheelBackRight;
            }
            return nullptr;
        }

        inline vehAero * getAero(void) {
            return &this->Aero;
        }

        inline vehAxle * getFrontAxle(void) {
            return &this->AxleFront;
        }

        inline vehAxle * getRearAxle(void) {
            return &this->AxleRear;
        }

        AGE_API vehCarSim()                                 { hook::Thunk<0x4CB660>::Call<void>(this); }
        AGE_API ~vehCarSim()                                { hook::Thunk<0x4CB8E0>::Call<void>(this); }

        AGE_API void Init(const char* basename, int colliderPtr, vehCarModel* carModelPtr)
        {
            //Call original 
            hook::Thunk<0x4CBB80>::Call<void>(this, basename, colliderPtr, carModelPtr);

            //We've expanded this class. Now for *our new stuff*
            Matrix34 diffMatrix;
            
            if (GetPivot(diffMatrix, basename, "whl4")) {
                BackBackLeftWheelPosDiff.X = diffMatrix.m30 - getWheel(2)->getCenter().X;
                BackBackLeftWheelPosDiff.Y = diffMatrix.m31 - getWheel(2)->getCenter().Y;
                BackBackLeftWheelPosDiff.Z = diffMatrix.m32 - getWheel(2)->getCenter().Z;
            }

            if (GetPivot(diffMatrix, basename, "whl5")) {
                BackBackRightWheelPosDiff.X = diffMatrix.m30 - getWheel(3)->getCenter().X;
                BackBackRightWheelPosDiff.Y = diffMatrix.m31 - getWheel(3)->getCenter().Y;
                BackBackRightWheelPosDiff.Z = diffMatrix.m32 - getWheel(3)->getCenter().Z;
            }

            //Copy WHL0 and WHL2 wobble limit values to WHL1 and WHL3 since their values aren't set by default
            WheelFrontRight.setWobbleLimit(WheelFrontLeft.getWobbleLimit());
            WheelBackRight.setWobbleLimit(WheelBackLeft.getWobbleLimit());
        }

        AGE_API int BottomedOut()                           { return hook::Thunk<0x4CBB40>::Call<int>(this); }
        AGE_API int OnGround()                              { return hook::Thunk<0x4CBB00>::Call<int>(this); }
        AGE_API void ReconfigureDrivetrain()                { hook::Thunk<0x4CC0B0>::Call<void>(this); }
        AGE_API void SetHackedImpactParams()                { hook::Thunk<0x4CC080>::Call<void>(this); }
        AGE_API void RestoreImpactParams()                  { hook::Thunk<0x4CC050>::Call<void>(this); }
        AGE_API void SetResetPos(Vector3 * a1)              { hook::Thunk<0x4CC830>::Call<void>(this, a1); }
        
        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { hook::Thunk<0x4CC8E0>::Call<void>(this); }
        AGE_API void Reset() override                       { hook::Thunk<0x4CBA70>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser)  override
                                                            { hook::Thunk<0x4CCC70>::Call<void>(this); }
        AGE_API char* GetClassName() override               { return hook::Thunk<0x4CCF10>::Call<char*>(this); }
        AGE_API char const* GetDirName() override           { return hook::Thunk<0x4CBAF0>::Call<char const*>(this); }

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

                .addPropertyReadOnly("WorldMatrix", &getWorldMatrix)

                .addVariableRef("CenterOfGravity", &vehCarSim::CenterOfGravity)
                .addVariableRef("SSSValue", &vehCarSim::SSSValue)
                .addVariableRef("SSSThreshold", &vehCarSim::SSSThreshold)
                .addVariableRef("CarFrictionHandling", &vehCarSim::CarFrictionHandling)

                .addProperty("Mass", &getMass, &setAndApplyMass)
                .addProperty("InertiaBox", &getInertiaBox, &setAndApplyInertiaBox)
                .addProperty("BoundFriction", &getBoundFriction, &setBoundFriction)
                .addProperty("BoundElasticity", &getBoundElasticity, &setBoundElasticity)
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
    };

    ASSERT_SIZEOF(vehCarSim, 0x1560 + 0xC + 0xC + 0xC + 0xC); //+4 extra fields

    // Lua initialization

}