#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class phJoint;

    // External declarations


    // Class definitions

    class phJoint {
    protected:
        hook::Field<0x04, phInertialCS *> _linkA;
        hook::Field<0x08, phInertialCS *> _linkB;
    public:
        inline phInertialCS* getFirstLink(void) const {
            return _linkA.get(this);
        }

        inline phInertialCS* getSecondLink(void) const {
            return _linkB.get(this);
        }

        virtual AGE_API bool IsBroken(void)                 { return hook::Thunk<0x5961F0>::Call<bool>(this); }
        virtual AGE_API void ComputeInvMassMatrix(phInertialCS* a1, Matrix34* a2, const Vector3* a3) 
                                                            { hook::Thunk<0x595E90>::Call<void>(this); }
        virtual AGE_API void ComputeInvMassMatrix()         { hook::Thunk<0x595DF0>::Call<void>(this); };
        virtual AGE_API void ComputeJointForce()            { hook::Thunk<0x595EA0>::Call<void>(this); };
        virtual AGE_API void ComputeJointPush()             { hook::Thunk<0x5960C0>::Call<void>(this); };
        virtual AGE_API Matrix34* GetInvMassMatrix()        { return hook::Thunk<0x595AD0>::Call<Matrix34*>(this); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<phJoint>("phJoint")
                .addPropertyReadOnly("FirstLink", &getFirstLink)
                .addPropertyReadOnly("SecondLink", &getSecondLink)
                .addFunction("IsBroken", &IsBroken)
                .endClass();
        }
    };

    // Lua initialization

}