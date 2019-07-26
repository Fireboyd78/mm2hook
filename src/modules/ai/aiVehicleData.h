#pragma once
#include <modules\ai.h>
#include <modules\phys\bound.h>

namespace MM2
{
    // Forward declarations
    class aiVehicleData;

    // External declarations
    extern class dgBoundBox;

    // Class definitions
    class aiVehicleData : public asNode {
    private:
        Vector3 Size;
        Vector3 MaxAng;
        Vector3 CG;
        Vector3 WheelPositions[6];
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
        float WheelRadius;
        int unknown176;
        int DataId;
        dgBoundBox* BoundingBox;

    public:
        //overrides
        AGE_API char* GetClassName() override           { return ageHook::Thunk<0x56F940>::Call<char*>(this); }
        AGE_API const char* GetDirName() override       { return ageHook::Thunk<0x56F950>::Call<const char*>(this); }
        AGE_API void FileIO(datParser& a1) override     { ageHook::Thunk<0x56F7C0>::Call<void>(this, a1); }

        //lua helpers
        Vector3* GetWheelPosition(int id) {
            //clamp
            if (id > 5)
                id = 5;

            //
            return &WheelPositions[id];
        }

        void SetWheelPosition(int id, Vector3* position) {
            //clamp
            if (id > 5)
                id = 5;

            auto wheelPosPtr = &WheelPositions[id];
            wheelPosPtr->X = position->X;
            wheelPosPtr->Y = position->Y;
            wheelPosPtr->Z = position->Z;
        }

        int getDataId() {
            return DataId;
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiVehicleData, asNode>("aiVehicleData")
                .addPropertyReadOnly("DataId", &getDataId)
                .addVariableRef("Size", &aiVehicleData::Size)
                .addVariableRef("CG", &aiVehicleData::CG)
                .addVariableRef("MaxAng", &aiVehicleData::MaxAng)
                .addVariableRef("Mass", &aiVehicleData::Mass)
                .addVariableRef("Friction", &aiVehicleData::Friction)
                .addVariableRef("MaxDamage", &aiVehicleData::MaxDamage)
                .addVariableRef("PtxThresh", &aiVehicleData::PtxThresh)
                .addVariableRef("Spring", &aiVehicleData::Spring)
                .addVariableRef("Damping", &aiVehicleData::Damping)
                .addVariableRef("RubberSpring", &aiVehicleData::RubberSpring)
                .addVariableRef("RubberDamp", &aiVehicleData::RubberDamp)
                .addVariableRef("Limit", &aiVehicleData::Limit)

                .addVariableRef("WheelRadius", &aiVehicleData::WheelRadius)

                .addFunction("SetWheelPosition", &SetWheelPosition)
                .addFunction("GetWheelPosition", &GetWheelPosition)

                .endClass();
        }
    };

    // Lua initialization

}