#pragma once
#include <modules\node\fileio.h>
#include <modules\phys\phjoint.h>

namespace MM2
{
    // Forward declarations
    class dgTrailerJoint;

    // External declarations
    extern class vehCarSim;

    // Class definitions

    class dgTrailerJoint : public phJoint, public asFileIO {
    private:
        float ForceLimit;
        int JointStatus;
        Matrix34 RestOrientMat2;
        Matrix34 RestOrientMat;
        float RestoreLimitForceLean;
        float DampConstLean;
        float DampLinearLean;
        float RestoreForceRoll;
        float DampConstRoll;
        float DampLinearRoll;
        float LeanLimit;
        float NegativeRollLimit;
        float PositiveRollLimit;
        float LimitElasticityLean;
        float LimitElasticityRoll;
        float FreeRange;
        float FreeLean;
        float CosFreeLean;
        float FreeRoll;
        vehCarSim* CarSim;
    public:
        ANGEL_ALLOCATOR

        AGE_API dgTrailerJoint() {
            scoped_vtable x(this);
            hook::Thunk<0x592CC0>::Call<void>(this);
        }

        AGE_API ~dgTrailerJoint() {
            scoped_vtable x(this);
            hook::Thunk<0x592CF0>::Call<void>(this);
        }

        AGE_API void Init(char const* name, phInertialCS* ICS1, phInertialCS* ICS2, Vector3 const& Offset1, Vector3 const& Offset2)
                                                            { hook::Thunk<0x592D20>::Call<void>(this, name, ICS1, ICS2, &Offset1, &Offset2); }
        AGE_API void Reset()                                { hook::Thunk<0x592E50>::Call<void>(this); }
        AGE_API void BreakJoint()                           { hook::Thunk<0x5942D0>::Call<void>(this); }
        AGE_API void UnbreakJoint()                         { hook::Thunk<0x5942E0>::Call<void>(this); }
        AGE_API void SetForceLimit(float a1)                { hook::Thunk<0x593200>::Call<void>(this, a1); }
        AGE_API void SetFrictionLean(float a1, float a2, float a3)
                                                            { hook::Thunk<0x593040>::Call<void>(this, a1, a2, a3); }
        AGE_API void SetFrictionRoll(float a1, float a2, float a3)
                                                            { hook::Thunk<0x593070>::Call<void>(this, a1, a2, a3); }
        AGE_API void SetLeanLimit(float a1, float a2)       { hook::Thunk<0x5930A0>::Call<void>(this, a1, a2); }
        AGE_API void SetPosition(const Vector3* a1)         { hook::Thunk<0x592EC0>::Call<void>(this, a1); }
        AGE_API void SetRollLimit(float a1, float a2, float a3)
                                                            { hook::Thunk<0x5930E0>::Call<void>(this, a1, a2, a3); }

        /*
            phJoint virtuals
        */

        virtual AGE_API bool IsBroken(void) override        { return hook::Thunk<0x595B30>::Call<bool>(this); };
        virtual AGE_API void ComputeInvMassMatrix(phInertialCS* a1, Matrix34* a2, const Vector3* a3)  override
                                                            { hook::Thunk<0x595080>::Call<void>(this); };
        
        /*
            asFileIO virtuals
        */

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<dgTrailerJoint, phJoint>("dgTrailerJoint")
                .addConstructor(LUA_ARGS())
                .addFunction("Init", &Init)
                .addFunction("Reset", &Reset)
                .addFunction("BreakJoint", &BreakJoint)
                .addFunction("UnbreakJoint", &UnbreakJoint)
                .addFunction("SetForceLimit", &SetForceLimit)
                .addFunction("SetFrictionLean", &SetFrictionLean)
                .addFunction("SetFrictionRoll", &SetFrictionRoll)
                .addFunction("SetLeanLimit", &SetLeanLimit)
                .addFunction("SetRollLimit", &SetRollLimit)
                .endClass();
        }
    };
    ASSERT_SIZEOF(dgTrailerJoint, 0x11C);

    // Lua initialization

}