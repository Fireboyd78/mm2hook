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
        byte _buffer[0x1AD];
    protected:
        hook::Field<0xC, float> _mass;
        hook::Field<0x2C, float> _maxVelocity;
        hook::Field<0x54, Matrix34> _matrix;
        hook::Field<0x9C, Vector3> _force;
        hook::Field<0x84, Vector3> _velocity;
        hook::Field<0x3C, Vector3> _scaledVelocity;
        hook::Field<0xA8, Vector3> _torque;
    public:
        AGE_API phInertialCS() {
            scoped_vtable x(this);
            hook::Thunk<0x475DE0>::Call<void>(this);
        }

        //props
        inline float GetMass(void) const {
            return _mass.get(this);
        }

        inline void SetMass(float mass) {
            _mass.set(this, mass);
        }

        inline float GetMaxVelocity(void) const {
            return _maxVelocity.get(this);
        }

        inline void SetMaxVelocity(float velocity) {
            _maxVelocity.set(this, velocity);
        }

        inline Vector3 GetPosition(void) const {
            return this->GetMatrix()->GetRow(3);
        }

        inline Vector3 GetVelocity(void) const {
            return _velocity.get(this);
        }

        inline void SetVelocity(Vector3 velocity) {
            _velocity.set(this, velocity);
            _scaledVelocity.set(this, velocity * this->GetMass());
        }

        inline Matrix34 * GetMatrix(void) const {
            return _matrix.ptr(this);
        }

        inline void SetPosition(Vector3 *position) const {
            auto mtx = GetMatrix();
            mtx->m30 = position->X;
            mtx->m31 = position->Y;
            mtx->m32 = position->Z;
        }

        inline void SetMatrix(Matrix34 *matrix) const {
            GetMatrix()->Set(matrix);
        }

        inline Vector3 GetForce(void) const {
            return _force.get(this);
        }

        inline void AddForce(Vector3 force) {
            auto current = GetForce();
            SetForce(current + force);
        }

        inline void SetForce(Vector3 force) const {
            _force.set(this, force);
        }

        inline Vector3 GetTorque(void) const {
            return _torque.get(this);
        }

        inline void AddTorque(Vector3 torque) {
            auto current = GetTorque();
            SetTorque(current + torque);
        }

        inline void SetTorque(Vector3 torque) const {
            _torque.set(this, torque);
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