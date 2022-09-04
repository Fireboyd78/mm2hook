#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class phInertialCS;

    // External declarations


    // Class definitions

    class phInertialCS {
    private:
        int field_0;
        int field_4;
        bool Active;
        float Mass;
        float InvMass;
        Vector3 AngInertia;
        Vector3 InvAngInertia;
        float MaxVelocity;
        float field_30;
        float field_34;
        float field_38;
        Vector3 ScaledVelocity;
        Vector3 AngularVelocity;
        Matrix34 Matrix;
        Vector3 Velocity;
        Vector3 Acceleration;
        Vector3 Force;
        Vector3 Torque;
        Vector3 field_B4;
        Vector3 field_C0;
        bool Oversample;
        Vector3 Force2;
        Vector3 Torque2;
        Vector3 NetPush;
        Vector3 NetTurn;
        Vector3 TotalAppliedPush;
        Vector3 LastTotalAppliedPush;
        bool IsBeingPushed;
        int field_11C;
        Matrix34 field_120;
        Matrix34 field_150;
        Matrix34 field_180;
        float field_1B0;
    public:
        AGE_API phInertialCS()
        {
            scoped_vtable x(this);
            hook::Thunk<0x475DE0>::Call<void>(this);
        }

        //props
        inline float GetMass(void)
        {
            return this->Mass;
        }

        inline void SetMass(float mass)
        {
            this->Mass =  mass;
        }

        inline float GetMaxVelocity(void)
        {
            return this->MaxVelocity;
        }

        inline void SetMaxVelocity(float velocity)
        {
            this->MaxVelocity = velocity;
        }

        inline Vector3 GetPosition(void)
        {
            return this->GetMatrix()->GetRow(3);
        }

        inline Vector3 GetVelocity(void)
        {
            return this->Velocity;
        }

        inline void SetVelocity(Vector3 velocity)
        {
            this->Velocity = velocity;
        }

        inline Vector3 GetScaledVelocity(void)
        {
            return this->ScaledVelocity;
        }

        inline void SetScaledVelocity(Vector3 velocity)
        {
            this->ScaledVelocity = velocity;
        }

        inline Vector3 GetAngularVelocity(void)
        {
            return this->AngularVelocity;
        }

        inline void SetAngularVelocity(Vector3 velocity)
        {
            this->AngularVelocity = velocity;
        }

        inline Matrix34 * GetMatrix(void)
        {
            return &this->Matrix;
        }

        inline void SetPosition(Vector3 position)
        {
            GetMatrix()->SetRow(3, position);
        }

        inline void SetMatrix(Matrix34 matrix)
        {
            GetMatrix()->Set(matrix);
        }

        inline Vector3 GetForce(void)
        {
            return this->Force;
        }

        inline void AddForce(Vector3 force)
        {
            SetForce(GetForce() + force);
        }

        inline void SetForce(Vector3 force)
        {
            this->Force = force;
        }

        inline Vector3 GetTorque(void)
        {
            return this->Torque;
        }

        inline void AddTorque(Vector3 torque)
        {
            SetTorque(GetTorque() + torque);
        }

        inline void SetTorque(Vector3 torque)
        {
            this->Torque = torque;
        }

        inline Vector3 GetTotalAppliedPush()
        {
            return this->TotalAppliedPush;
        }

        inline void SetTotalAppliedPush(Vector3 push)
        {
            this->TotalAppliedPush = push;
        }

        //members (not all here yet!)
        AGE_API void Zero()                     { hook::Thunk<0x475DED>::Call<void>(this); }
        AGE_API void Reset()                    { hook::Thunk<0x475E10>::Call<void>(this); }
        AGE_API void Freeze()                   { hook::Thunk<0x475E20>::Call<void>(this); }
        AGE_API void ZeroForces()               { hook::Thunk<0x475E60>::Call<void>(this); }
        AGE_API void Init(float mass, float inertiaTensorX, float inertiaTensorY, float inertiaTensorZ)
                                                { hook::Thunk<0x475FC0>::Call<void>(this, mass, inertiaTensorX, inertiaTensorY, inertiaTensorZ); }
        AGE_API void InitBoxMass(float mass, float inertiaBoxX, float inertiaBoxY, float inertiaBoxZ)
                                                { hook::Thunk<0x4760D0>::Call<void>(this, mass, inertiaBoxX, inertiaBoxY, inertiaBoxZ); }
        AGE_API void Rotate(Vector3 *angles)    { hook::Thunk<0x476A00>::Call<void>(this, angles); }
        AGE_API void ClearInertialValues()      { hook::Thunk<0x476B20>::Call<void>(this); }
        
        AGE_API void MoveICS()                  { hook::Thunk<0x478680>::Call<void>(this); }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<phInertialCS>("phInertialCS")
                .addFunction("Zero", &Zero)
                .addFunction("Reset", &Reset)
                .addFunction("Freeze", &Freeze)
                .addFunction("ZeroForces", &ZeroForces)
                .addFunction("Init", &Init)
                .addFunction("InitBoxMass", &InitBoxMass)
                .addFunction("Rotate", &Rotate)
                .addFunction("ClearInertialValues", &ClearInertialValues)
                .addFunction("MoveICS", &MoveICS)

                .addFunction("AddForce", &AddForce)
                .addFunction("AddTorque", &AddTorque)

                .addProperty("Mass", &GetMass, &SetMass)
                .addProperty("Force", &GetForce, &SetForce)
                .addProperty("Torque", &GetTorque, &SetTorque)
                .addProperty("Velocity", &GetVelocity, &SetVelocity)
                .addProperty("Position", &GetPosition, &SetPosition)
                .addProperty("Matrix", &GetMatrix, &SetMatrix)
            .endClass();
        }
    };

    ASSERT_SIZEOF(phInertialCS, 0x1B4);

    // Lua initialization

}