#pragma once
#include <modules\vehicle.h>
#include <modules\phys.h>
#include "carsim.h"

namespace MM2
{
    // Forward declarations
    class vehTrailer;
    class vehTrailerInstance;

    // External declarations
    extern class asNode;
    extern class dgPhysEntity;
    extern class dgTrailerJoint;

    // Class definitions

    class vehTrailer : public dgPhysEntity, public asNode {
    protected:
        hook::Field<0x1E4, vehCarSim *> _sim;
        hook::Field<0xCC, dgTrailerJoint> _joint;
        hook::Field<0x528, vehWheel> _twhl0;
        hook::Field<0x794, vehWheel> _twhl1;
        hook::Field<0xA00, vehWheel> _twhl2;
        hook::Field<0xC6C, vehWheel> _twhl3;
    public:
        inline vehCarSim* getCarSim(void) const {
            return _sim.get(this);
        }

        inline dgTrailerJoint * getJoint(void) const {
            return _joint.ptr(this);
        }

        inline vehWheel * getWheel(int num) {
            switch (num) {
            case 0:
                return _twhl0.ptr(this);
            case 1:
                return _twhl1.ptr(this);
            case 2:
                return _twhl2.ptr(this);
            case 3:
                return _twhl3.ptr(this);
            }
            return nullptr;
        }

        AGE_API void Init(const char* basename, const Vector3* a2, vehCarSim* a3, int a4)
        {
            hook::Thunk<0x4D72F0>::Call<void>(this, basename, a2, a3, a4);

            Matrix34 diffMatrix;

            if (GetPivot(diffMatrix, basename, "trailer_twhl4")) {
                getCarSim()->TrailerBackBackLeftWheelPosDiff.X = diffMatrix.m30 - getWheel(2)->getCenter().X;
                getCarSim()->TrailerBackBackLeftWheelPosDiff.Y = diffMatrix.m31 - getWheel(2)->getCenter().Y;
                getCarSim()->TrailerBackBackLeftWheelPosDiff.Z = diffMatrix.m32 - getWheel(2)->getCenter().Z;
            }

            if (GetPivot(diffMatrix, basename, "trailer_twhl5")) {
                getCarSim()->TrailerBackBackRightWheelPosDiff.X = diffMatrix.m30 - getWheel(3)->getCenter().X;
                getCarSim()->TrailerBackBackRightWheelPosDiff.Y = diffMatrix.m31 - getWheel(3)->getCenter().Y;
                getCarSim()->TrailerBackBackRightWheelPosDiff.Z = diffMatrix.m32 - getWheel(3)->getCenter().Z;
            }
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehTrailer, dgPhysEntity>("vehTrailer")
                .addPropertyReadOnly("vehCarSim", &getCarSim)
                .addPropertyReadOnly("Joint", &getJoint)
            .endClass();
        }
    };

    class vehTrailerInstance : public lvlInstance {
    private:
        vehTrailer* Trailer;
        int field_18;
        Vector3 TrailerHitchPosition;
    public:
        inline vehTrailer * getTrailer(void) const {
            return this->Trailer;
        }

        inline Vector3 getTrailerHitchPosition(void) {
            return this->TrailerHitchPosition;
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehTrailerInstance, lvlInstance>("vehTrailerInstance")
                //properties
                .addPropertyReadOnly("Trailer", &getTrailer)
                .addProperty("TrailerHitchPosition", &getTrailerHitchPosition)
            .endClass();
        }
    };
    ASSERT_SIZEOF(vehTrailerInstance, 0x28);

    // Lua initialization

}