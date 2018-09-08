#pragma once
#include "mm2_common.h"
#include "mm2_level.h"
#include "mm2_base.h"
#include "mm2_particle.h"

namespace MM2
{
    // Forward declarations
    class dgPhysManager;
    class dgPhysEntity;
    class dgBangerInstance;
    struct phCollider;
    class phInterialCs;
    class phBound;
    class phJoint;
    class phTrailerJoint;
    

    // External declarations
    extern class lvlInstance;
    
    class dgPhysManager {
    public:
        static ageHook::Type<dgPhysManager *> Instance;

        void DeclareMover(lvlInstance* instance, int a2, int a3) { ageHook::Thunk<0x468370>::Call<void>(this, instance, a2, a3); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<dgPhysManager>("dgPhysManager")
                //statics
                .addStaticFunction("Instance", [] {return (dgPhysManager *)Instance; })

                //functions
                .addFunction("DeclareMover", &DeclareMover)

                .endClass();
        }
    };

    class dgBangerData : public asNode
    {
        uint32_t Index;
        Vector3 vector31C;
        Vector3 vector328;
        int dword34;
        int dword38;
        int dword3C;
        Vector3 *Positions;
        short PositionCount;
        Vector3 dword48;
        int dword54;
        int dword58;
        int dword5C;
        asBirthRule BirthRule;
        int dword114;
        int Flags2;
        short word11C;
        byte byte11E;
        byte byte11F;
        byte Flags;
        byte byte121;
        int dword124;
        short word128;
        int dword12C;
        int dword130;
        int dword134;
        char pad138[28];
    };

    struct phCollider {
        //todo: figure the rest of this out!
    private:
        byte unknown[0x30];
    public:
        int colliderId; //?
    };

    class phInertialCS {
    protected:
        ageHook::Field<0x54, Matrix34> _matrix;
    public:
        inline Matrix34* getMatrix(void) const {
            return _matrix.ptr(this);
        }
    };

    class phMaterial {

    };

    class phIntersectionPoint {

    };

    class phIntersection {

    };

    class phSegment {

    };

    class phImpactBase {

    };

    class phJoint {
    protected:
        ageHook::Field<0x04, phInertialCS *> _linkA;
        ageHook::Field<0x08, phInertialCS *> _linkB;
    public:
        inline phInertialCS* getFirstLink(void) const {
            return _linkA.get(this);
        }

        inline phInertialCS* getSecondLink(void) const {
            return _linkB.get(this);
        }

        virtual AGE_API bool IsBroken(void)                 { return ageHook::Thunk<0x5961F0>::Call<bool>(this); }
        virtual AGE_API void ComputeInvMassMatrix(phInertialCS* a1, Matrix34* a2, const Vector3* a3) 
                                                            { ageHook::Thunk<0x595E90>::Call<void>(this); }
        virtual AGE_API void ComputeInvMassMatrix()         { ageHook::Thunk<0x595DF0>::Call<void>(this); };
        virtual AGE_API void ComputeJointForce()            { ageHook::Thunk<0x595EA0>::Call<void>(this); };
        virtual AGE_API void ComputeJointPush()             { ageHook::Thunk<0x5960C0>::Call<void>(this); };
        virtual AGE_API Matrix34* GetInvMassMatrix()        { return ageHook::Thunk<0x595AD0>::Call<Matrix34*>(this); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<phJoint>("phJoint")
                .addPropertyReadOnly("FirstLink", &getFirstLink)
                .addPropertyReadOnly("SecondLink", &getSecondLink)
                .addFunction("IsBroken", &IsBroken)
                .endClass();
        }
    };

    class dgTrailerJoint : public phJoint, public asFileIO {
    public:
        AGE_API void Reset()                                { ageHook::Thunk<0x592E50>::Call<void>(this); }
        AGE_API void BreakJoint()                           { ageHook::Thunk<0x5942D0>::Call<void>(this); }
        AGE_API void UnbreakJoint()                         { ageHook::Thunk<0x5942E0>::Call<void>(this); }
        AGE_API void SetForceLimit(float a1)                { ageHook::Thunk<0x593200>::Call<void>(this, a1); }
        AGE_API void SetFrictionLean(float a1, float a2, float a3)
                                                            { ageHook::Thunk<0x593040>::Call<void>(this, a1, a2, a3); }
        AGE_API void SetFrictionRoll(float a1, float a2, float a3)
                                                            { ageHook::Thunk<0x593070>::Call<void>(this, a1, a2, a3); }
        AGE_API void SetLeanLimit(float a1, float a2)       { ageHook::Thunk<0x5930A0>::Call<void>(this, a1, a2); }
        AGE_API void SetPosition(const Vector3* a1)         { ageHook::Thunk<0x592EC0>::Call<void>(this, a1); }
        AGE_API void SetRollLimit(float a1, float a2, float a3)
                                                            { ageHook::Thunk<0x5930E0>::Call<void>(this, a1, a2, a3); }

        /*
            phJoint virtuals
        */

        virtual AGE_API bool IsBroken(void) override        { return ageHook::Thunk<0x595B30>::Call<bool>(this); };
        virtual AGE_API void ComputeInvMassMatrix(phInertialCS* a1, Matrix34* a2, const Vector3* a3)  override
                                                            { ageHook::Thunk<0x595080>::Call<void>(this); };
        
        /*
            asFileIO virtuals
        */

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<dgTrailerJoint, phJoint>("dgTrailerJoint")
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

    class dgPhysEntity : public Base {
    public:
        virtual AGE_API void PreUpdate()                    { ageHook::Thunk<0x42CBE0>::Call<void>(this); }
        virtual AGE_API void Update()                       { ageHook::Thunk<0x46A120>::Call<void>(this); }
        virtual AGE_API void PostUpdate() PURE;
        virtual AGE_API phInertialCS * GetICS() PURE;
        virtual AGE_API phCollider * GetCollider()          { return ageHook::Thunk<0x42CBF0>::Call<phCollider *>(this); }
        virtual AGE_API lvlInstance * GetInst() PURE;
        virtual AGE_API void DetachMe()                     { ageHook::Thunk<0x42CC00>::Call<void>(this); }
        virtual AGE_API bool RequiresTerrainCollision()     { return ageHook::Thunk<0x42CC10>::Call<bool>(this); }
        virtual AGE_API void FirstImpactCallback()          { ageHook::Thunk<0x42CC20>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<dgPhysEntity>("dgPhysEntity")
                .addFunction("DetachMe", &DetachMe)
                .addFunction("RequiresTerrainCollision", &RequiresTerrainCollision)
                .addFunction("GetICS", &GetICS)
                .addFunction("GetCollider", &GetCollider)
                .addFunction("GetInst", &GetInst)
                .endClass();
        }
    };

    //TODO : find out inheritance / virtual stuff!!!
    class dgBangerInstance {
    public:
        void Draw(int a1) { ageHook::Thunk<0x4415E0>::Call<void>(this, a1); }
    };

    template<>
    void luaAddModule<module_phys>(LuaState L) {
        luaBind<dgPhysManager>(L);
        luaBind<phJoint>(L);
        luaBind<dgTrailerJoint>(L);
        luaBind<dgPhysEntity>(L);
    }
}