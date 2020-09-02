#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehStuck;

    // External declarations


    // Class definitions
    class vehStuck : public asNode
    {
    private:
        int State;
        float StuckTime;
        Vector3 LastImpactPos;
        float TimeThresh;
        float PosThresh;
        float MoveThresh;
        float PosThreshSqr;
        float MoveThreshSqr;
        float Turn;
        float Rotation;
        float Translation;
        phInertialCS *m_InertialCSPtr;
        vehCarSim *m_CarSimPtr;
    private:
    public:
        AGE_API vehStuck()                              { ageHook::Thunk<0x4D5FB0>::Call<void>(this); }

        AGE_API void Init(vehCarSim *carSim, const char *name) 
                                                        { ageHook::Thunk<0x4D6090>::Call<void>(this, carSim, name); }
        
        AGE_API static void ComputeConstantsStatic(vehStuck* stuck)
                                                        { ageHook::StaticThunk<0x4D6040>::Call<void>(stuck); }
        void ComputeConstants()                         { vehStuck::ComputeConstantsStatic(this); }

        /*
            asNode virtuals
        */
        AGE_API void Reset() override                   { ageHook::Thunk<0x4D6060>::Call<void>(this); }
        AGE_API void Update() override                  { ageHook::Thunk<0x4D6140>::Call<void>(this); }
        AGE_API void FileIO(datParser& parser) override { ageHook::Thunk<0x4D6510>::Call<void>(this); }
        AGE_API const char* GetDirName() override       { return ageHook::Thunk<0x4D6080>::Call<const char*>(this); }
        AGE_API char * GetClassName() override          { return ageHook::Thunk<0x4D65F0>::Call<char*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehStuck, asNode>("vehStuck")
                //properties
                .addVariableRef("Turn", &vehStuck::Turn)
                .addVariableRef("Rotation", &vehStuck::Rotation)
                .addVariableRef("Translation", &vehStuck::Translation)
                .addVariableRef("TimeThresh", &vehStuck::TimeThresh)
                .addVariableRef("PosThresh", &vehStuck::PosThresh)
                .addVariableRef("MoveThresh", &vehStuck::MoveThresh)

                .addFunction("Init", &Init)
                .addFunction("ComputeConstants", &ComputeConstants)

                .endClass();
        }
    };


    ASSERT_SIZEOF(vehStuck, 0x54);

    // Lua initialization

}