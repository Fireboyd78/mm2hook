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
        byte _buffer[0x1B4];
    protected:
        ageHook::Field<0x54, Matrix34> _matrix;
    public:
        AGE_API phInertialCS() {
            scoped_vtable x(this);
            ageHook::Thunk<0x475DE0>::Call<void>(this);
        }

        //props
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

        inline void setMatrix(Matrix34 *matrix) const {
            getMatrix()->Set(matrix);
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

                .addProperty("Position", &getPosition, &setPosition)
                .addProperty("Matrix", &getMatrix, &setMatrix)
            .endClass();
        }
    };


    // Lua initialization

}