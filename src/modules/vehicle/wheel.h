#pragma once
#include <modules\vehicle.h>
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class vehWheel;

    // External declarations
    extern class vehCarSim;
    extern class lvlMaterial;

    // Class definitions
    class vehWheel : public asNode {
    private:
        vehCarSim *m_CarSimPtr;
        phInertialCS *m_InertialCSPtr;
        int WheelFlags;
        Matrix34 WheelMatrix;
        float TireDispLimitLat;
        float TireDispLimitLong;
        float TireDampCoefLat;
        float TireDampCoefLong;
        float TireDragCoefLat;
        float TireDragCoefLong;
        float SteeringLimit;
        float CamberLimit;
        float WobbleLimit;
        float BrakeCoef;
        float HandbrakeCoef;
        float SteeringOffset;
        float SuspensionLimit;
        float SuspensionExtent;
        float SuspensionFactor;
        float SuspensionDampCoef;
        int WheelCount;
        lvlSegment Segment;
        lvlIntersection Intersection;
        lvlSegmentInfo SegmentInfo;
        lvlInstance* Instance;
        BOOL IsGrounded;
        Vector3 WheelSpaceVelocity;
        float MotorAdjustedForwardVelocity;
        Vector3 SidewaysDirection;
        Vector3 LastSurfaceNormal;
        Vector3 RearwardsDirection;
        Vector3 LastHitPosition;
        Vector3 Center;
        float Radius;
        float Width;
        float NormalLoad;
        float BumpDisplacement;
        float MaterialDrag;
        float MaterialFriction;
        float MaterialDepth;
        float MaterialHeight;
        float MaterialWidth;
        float BumpDepth;
        float AccumulatedRotation;
        float InputBrakeAmount;
        float BrakeCoefLoaded;
        float HandbrakeCoefLoaded;
        float SteerAmount;
        float TargetSuspensionTravel;
        float CurrentSuspensionForce;
        float SuspensionForceCopy;
        float SuspensionCompressionRate;
        float SuspensionForceTwo_;
        float SuspensionMaxForce;
        float SuspensionRestingPosition;
        float SuspensionDampForce;
        float WobbleAmount;
        float CamberAmount;
        float LastSlippage;
        short MajorlySlipping;
        char LastGroundedStatus;
        bool BottomedOut;
        float CurrentTireDispLat;
        float CurrentTireDispLong;
        float DispLatRate;
        float DispLongRate;
        float DispLongRateScaled;
        float RotationRate;
        float LatSlipPercent;
        float LongSlipPercent;
        float DispLimitLongLoaded;
        float DampCoefLongLoaded;
        float OptimumSlipPercent;
        float StaticFric;
        float SlidingFric;
        float DispLimitLatLoaded;
        float DampCoefLatLoaded;
        float InvSquaredOptimumSlipPercent;
        lvlMaterial *CurrentPhysicsMaterial;
    public:
        static hook::Type<float> WeatherFriction;

        inline std::tuple<float, float> computeFriction(float slip) {
            float visualFriction = 0.f;
            float functionalFriction = this->ComputeFriction(slip, &visualFriction);
            return std::make_tuple(functionalFriction, visualFriction);
        }

        inline Matrix34 getMatrix() {
            return this->WheelMatrix;
        }

        inline bool isGrounded() {
            return this->IsGrounded == TRUE;
        }

        inline lvlMaterial * getCurrentPhysicsMaterial() {
            return this->CurrentPhysicsMaterial;
        }

        inline float getRadius(void) {
            return this->Radius;
        }

        inline void setRadius(float radius) {
            this->Radius = radius;
        }
        
        inline float getWidth(void) {
            return this->Width;
        }

        inline void setWidth(float width) {
            this->Width = width;
        }

        inline Vector3 getCenter(void) {
            return this->Center;
        }

        inline void setCenter(Vector3 center) {
            this->Center = center;
        }
        
        inline float getLatSlipPercent(void) {
            return this->LatSlipPercent;
        }

        inline float getLongSlipPercent(void) {
            return this->LongSlipPercent;
        }

        inline float getRotationRate(void) {
            return this->RotationRate;
        }

        inline float getAccumulatedRotation(void) {
            return this->AccumulatedRotation;
        }

        inline float getWobbleLimit(void) {
            return this->WobbleLimit;
        }

        inline void setWobbleLimit(float limit) {
            this->WobbleLimit = limit;
        }

        inline float getWobbleAmount(void) {
            return this->WobbleAmount;
        }

        inline vehCarSim* getCarSim() {
            return this->m_CarSimPtr;
        }
    public:
        AGE_API vehWheel()                                     { hook::Thunk<0x4D2190>::Call<void>(this); }

        AGE_API void Init(vehCarSim* carSimPtr, const char* vehicleBasename, const char* wheelName, Vector3 centerOfGravity, phInertialCS* inertialCs, int wheelCount, int flags)
        {
            Matrix34 outMatrix;

            this->m_CarSimPtr = carSimPtr;
            this->WheelFlags |= flags;
            this->WheelCount = wheelCount;
            this->m_InertialCSPtr = inertialCs;

            if (GetPivot(outMatrix, vehicleBasename, wheelName)) {
                this->Center.X = outMatrix.m30;
                this->Center.Y = outMatrix.m31;
                this->Center.Z = outMatrix.m32;

                float halfHeight = (outMatrix.m11 - outMatrix.m01) * 0.5f;
                this->Radius = fabs(halfHeight);

                this->Width = outMatrix.m10 - outMatrix.m00;
            }

            this->ComputeConstants();
        }

        AGE_API void CopyVars(vehWheel *copyFrom)              { hook::Thunk<0x4D4110>::Call<void>(this, copyFrom); }

        AGE_API void ComputeConstants()                        { hook::Thunk<0x4D23F0>::Call<void>(this); }
        AGE_API void AddNormalLoad(float a1)                   { hook::Thunk<0x4D2490>::Call<void>(this, a1); }
        AGE_API void SetNormalLoad(float a1)                   { hook::Thunk<0x4D24C0>::Call<void>(this, a1); }
        AGE_API void SetInputs(float a1, float a2, float a3)
                                                               { hook::Thunk<0x4D3F80>::Call<void>(this, a1, a2, a3); }
        AGE_API int GetSurfaceSound()                          { return hook::Thunk<0x4D3F60>::Call<int>(this); }
        AGE_API float GetVisualDispVert()                      { return hook::Thunk<0x4D4030>::Call<float>(this); }
        AGE_API float GetVisualDispLat()                       { return hook::Thunk<0x4D4090>::Call<float>(this); }
        AGE_API float GetVisualDispLong()                      { return hook::Thunk<0x4D40D0>::Call<float>(this); }

        AGE_API float ComputeFriction(float slip, float *vF)   { return hook::Thunk<0x4D25D0>::Call<float>(this, slip, vF); }
        /*
            asNode virtuals
        */

        AGE_API void Reset() override                          { hook::Thunk<0x4D22E0>::Call<void>(this); }
        AGE_API void Update() override                         { hook::Thunk<0x4D34E0>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser) override        { hook::Thunk<0x4D41C0>::Call<void>(this); }
        AGE_API char * GetClassName() override                 { return hook::Thunk<0x4D43C0>::Call<char *>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehWheel, asNode>("vehWheel")
                //properties
                .addVariableRef("SuspensionExtent", &vehWheel::SuspensionExtent)
                .addVariableRef("SuspensionLimit", &vehWheel::SuspensionLimit)
                .addVariableRef("SuspensionFactor", &vehWheel::SuspensionFactor)
                .addVariableRef("SuspensionDampCoef", &vehWheel::SuspensionDampCoef)
                .addVariableRef("SteeringLimit", &vehWheel::SteeringLimit)
                .addVariableRef("SteeringOffset", &vehWheel::SteeringOffset)
                .addVariableRef("BrakeCoef", &vehWheel::BrakeCoef)
                .addVariableRef("HandbrakeCoef", &vehWheel::HandbrakeCoef)
                .addVariableRef("CamberLimit", &vehWheel::CamberLimit)
                .addVariableRef("WobbleLimit", &vehWheel::WobbleLimit)
                .addVariableRef("TireDispLimitLong", &vehWheel::TireDispLimitLong)
                .addVariableRef("TireDampCoefLong", &vehWheel::TireDampCoefLong)
                .addVariableRef("TireDragCoefLong", &vehWheel::TireDragCoefLong)
                .addVariableRef("TireDispLimitLat", &vehWheel::TireDispLimitLat)
                .addVariableRef("TireDampCoefLat", &vehWheel::TireDampCoefLat)
                .addVariableRef("TireDragCoefLat", &vehWheel::TireDragCoefLat)
                .addVariableRef("OptimumSlipPercent", &vehWheel::OptimumSlipPercent)
                .addVariableRef("StaticFric", &vehWheel::StaticFric)
                .addVariableRef("SlidingFric", &vehWheel::SlidingFric)

                .addProperty("Radius", &getRadius, &setRadius)
                .addProperty("Width", &getWidth, &setWidth)
                
                .addPropertyReadOnly("CurrentPhysicsMaterial", &getCurrentPhysicsMaterial)
                .addPropertyReadOnly("LatSlipPercent", &getLatSlipPercent)
                .addPropertyReadOnly("LongSlipPercent", &getLongSlipPercent)
                
                //functions
                .addFunction("CopyVars", &CopyVars)
                .addFunction("ComputeConstants", &ComputeConstants)
                .addFunction("AddNormalLoad", &AddNormalLoad)
                .addFunction("SetNormalLoad", &SetNormalLoad)
                .addFunction("SetInputs", &SetInputs)
                .addFunction("GetSurfaceSound", &GetSurfaceSound)
                .addFunction("GetVisualDispVert", &GetVisualDispVert)
                .addFunction("GetVisualDispLat", &GetVisualDispLat)
                .addFunction("GetVisualDispLong", &GetVisualDispLong)
                .addFunction("ComputeFriction", &computeFriction)
            .endClass();
        }
    };

    ASSERT_SIZEOF(vehWheel, 0x26C);

    // Lua initialization

}