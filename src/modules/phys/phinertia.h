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
        byte _buffer[0x1B0];
    protected:
        ageHook::Field<0xC, float> _mass;
        ageHook::Field<0x54, Matrix34> _matrix;
        ageHook::Field<0x9C, Vector3> _force;
        ageHook::Field<0xA8, Vector3> _torque;
    public:
        AGE_API phInertialCS() {
            scoped_vtable x(this);
            ageHook::Thunk<0x475DE0>::Call<void>(this);
        }

        //props
        inline float getMass(void) const {
            return _mass.get(this);
        }

        inline Vector3 getTorque(void) const {
            return _torque.get(this);
        }

        inline Vector3 getForce(void) const {
            return _force.get(this);
        }

        inline Vector3 getPosition(void) const {
            auto mtx = getMatrix();
            return Vector3(mtx->m30, mtx->m31, mtx->m32);
        }

        inline Matrix34 * getMatrix(void) const {
            return _matrix.ptr(this);
        }

        inline void setPosition(Vector3 *position) const {
            auto mtx = getMatrix();
            mtx->m30 = position->X;
            mtx->m31 = position->Y;
            mtx->m32 = position->Z;
        }

        inline void setTorque(Vector3 torque) const {
            _torque.set(this, torque);
        }

        inline void setForce(Vector3 torque) const {
            _force.set(this, torque);
        }

        inline void setMatrix(Matrix34 *matrix) const {
            getMatrix()->Set(matrix);
        }

        inline void addForce(Vector3 force) {
            auto current = getForce();
            setForce(Vector3(current.X + force.X, current.Y + force.Y, current.Z + force.Z));
        }

        inline void addTorque(Vector3 force) {
            auto current = getTorque();
            setTorque(Vector3(current.X + force.X, current.Y + force.Y, current.Z + force.Z));
        }

        inline void setMass(float mass) {
            _mass.set(this, mass);
        }

        //members (not all here yet!)
        AGE_API void Zero()                     { ageHook::Thunk<0x475DED>::Call<void>(this); }
        AGE_API void Reset()                    { ageHook::Thunk<0x475E10>::Call<void>(this); }
        AGE_API void Freeze()                   { ageHook::Thunk<0x475E20>::Call<void>(this); }
        AGE_API void ZeroForces()               { ageHook::Thunk<0x475E60>::Call<void>(this); }
        AGE_API void Init(float mass, float inertiaTensorX, float inertiaTensorY, float inertiaTensorZ)
                                                { ageHook::Thunk<0x475FC0>::Call<void>(this, mass, inertiaTensorX, inertiaTensorY, inertiaTensorZ); }
        AGE_API void InitBoxMass(float mass, float inertiaBoxX, float inertiaBoxY, float inertiaBoxZ)
                                                { ageHook::Thunk<0x4760D0>::Call<void>(this, mass, inertiaBoxX, inertiaBoxY, inertiaBoxZ); }
        AGE_API void Rotate(Vector3 *angles)    { ageHook::Thunk<0x476A00>::Call<void>(this, angles); }
        AGE_API void ClearInertialValues()      { ageHook::Thunk<0x476B20>::Call<void>(this); }
        
        AGE_API void MoveICS()                  { ageHook::Thunk<0x478680>::Call<void>(this); }

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

                .addFunction("AddForce", &addForce)
                .addFunction("AddTorque", &addTorque)

                .addProperty("Mass", &getMass, &setMass)
                .addProperty("Force", &getForce, &setForce)
                .addProperty("Torque", &getTorque, &setTorque)
                .addProperty("Position", &getPosition, &setPosition)
                .addProperty("Matrix", &getMatrix, &setMatrix)
            .endClass();
        }
    };

    ASSERT_SIZEOF(phInertialCS, 0x1B4);

    // Lua initialization

}