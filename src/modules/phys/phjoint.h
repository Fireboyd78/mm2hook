#pragma once
#include <modules\phys\phinertia.h>

namespace MM2
{
    // Forward declarations
    class phJoint;

    // External declarations
    extern class phInertialCS;

    // Class definitions

    class phJoint {
    private:
        phInertialCS* ICS1;
        phInertialCS* ICS2;
        Vector3 Offset1;
        Vector3 Offset2;
        Vector3 PremultipliedOffset;
        Vector3 PremultipliedOffset1;
        Matrix34 InvMassMatrix;
    public:
        ANGEL_ALLOCATOR

        AGE_API phJoint() {
            scoped_vtable x(this);
            hook::Thunk<0x595B90>::Call<void>(this);
        }

        inline phInertialCS* getFirstLink(void) {
            return this->ICS1;
        }

        inline void setFirstLink(phInertialCS* ics) {
            this->ICS1 = ics;
        }

        inline phInertialCS* getSecondLink(void) {
            return this->ICS2;
        }

        inline void setSecondLink(phInertialCS* ics) {
            this->ICS2 = ics;
        }

        inline Vector3 getFirstOffset(void) {
            return this->Offset1;
        }

        inline void setFirstOffset(Vector3 offset) {
            this->Offset1 = offset;
        }

        inline Vector3 getSecondOffset(void) {
            return this->Offset2;
        }

        inline void setSecondOffset(Vector3 offset) {
            this->Offset2 = offset;
        }

        AGE_API void Init(phInertialCS* ICS1, phInertialCS* ICS2, Vector3 const& Offset)
                                                            { hook::Thunk<0x595C40>::Call<void>(this, ICS1, ICS2, &Offset); }
        AGE_API void Init(phInertialCS* ICS1, phInertialCS* ICS2, Vector3 const& Offset1, Vector3 const& Offset2)
                                                            { hook::Thunk<0x595D10>::Call<void>(this, ICS1, ICS2, &Offset1, &Offset2); }
        virtual AGE_API bool IsBroken(void)                 { return hook::Thunk<0x5961F0>::Call<bool>(this); }
        virtual AGE_API void ComputeInvMassMatrix(phInertialCS* a1, Matrix34* a2, const Vector3* a3) 
                                                            { hook::Thunk<0x595E90>::Call<void>(this); }
        virtual AGE_API void ComputeInvMassMatrix()         { hook::Thunk<0x595DF0>::Call<void>(this); }
        virtual AGE_API void ComputeJointForce()            { hook::Thunk<0x595EA0>::Call<void>(this); }
        virtual AGE_API void ComputeJointPush()             { hook::Thunk<0x5960C0>::Call<void>(this); }
        virtual AGE_API Matrix34* GetInvMassMatrix()        { return hook::Thunk<0x595AD0>::Call<Matrix34*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<phJoint>("phJoint")
                .addConstructor(LUA_ARGS())
                .addProperty("FirstLink", &getFirstLink, &setFirstLink)
                .addProperty("SecondLink", &getSecondLink, &setSecondLink)
                .addProperty("FirstOffset", &getFirstOffset, &setFirstOffset)
                .addProperty("SecondOffset", &getSecondOffset, &setSecondOffset)
                .addFunction("IsBroken", &IsBroken)
                .addFunction("Init", static_cast<void(phJoint::*)(phInertialCS*, phInertialCS*, const Vector3&, const Vector3&)>(&Init))
                .endClass();
        }
    };
    ASSERT_SIZEOF(phJoint, 0x6C);

    // Lua initialization

}