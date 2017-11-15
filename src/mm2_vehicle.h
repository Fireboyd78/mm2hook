#pragma once
#include "mm2_common.h"
#include "mm2_inst.h"
#include "mm2_game.h"

namespace MM2
{
    // Forward declarations
    class vehCar;
    class vehCarAudioContainer;
    class vehPoliceCarAudio;

    class vehCarModel : public lvlInstance {
    public:
        AGE_API vehCarModel()                               { ageHook::Thunk<0x4CCF20>::Call<void>(this); }
        AGE_API ~vehCarModel()                              { ageHook::Thunk<0x4CCF80>::Call<void>(this); }

        AGE_API void BreakElectrics(Vector3* a1)            { ageHook::Thunk<0x4CEFE0>::Call<void>(this, a1); }
        AGE_API void ClearDamage()                          { ageHook::Thunk<0x4CDFF0>::Call<void>(this); }
        AGE_API void EjectOneshot()                         { ageHook::Thunk<0x4CDCA0>::Call<void>(this); }
        AGE_API bool GetVisible()                           { return ageHook::Thunk<0x4CF070>::Call<bool>(this); }
        AGE_API void SetVisible(bool a1)                    {ageHook::Thunk<0x4CF050>::Call<void>(this, a1); }
        
        /*
        lvlInstance virtuals
        */

        virtual AGE_API void Reset()  override              { ageHook::Thunk<0x4CDFD0>::Call<void>(this); }
        virtual AGE_API Vector3 const & GetPosition() override
                                                            { return ageHook::Thunk<0x4CEF50>::Call<Vector3 const &>(this); }
        virtual AGE_API Matrix34 const & GetMatrix(Matrix34 const & a1)
                                                            { return ageHook::Thunk<0x4CEF50>::Call<Matrix34 const &>(this, a1); }
        virtual AGE_API void SetMatrix(Matrix34 const & a1) override 
                                                            { ageHook::Thunk<0x4CEFA0>::Call<void>(this); }
        virtual AGE_API dgPhysEntity* GetEntity() override  { return ageHook::Thunk<0x4CEFC0>::Call<dgPhysEntity*>(this); }
        virtual AGE_API dgPhysEntity* AttachEntity() override 
                                                            { return ageHook::Thunk<0x4CEFD0>::Call<dgPhysEntity*>(this); }
        virtual AGE_API Vector3 const * GetVelocity() override
                                                            { return ageHook::Thunk<0x4CEF80>::Call<Vector3 const*>(this); }

        virtual AGE_API void Draw(int a1) override          { ageHook::Thunk<0x4CE040>::Call<void>(this, a1); }
        virtual AGE_API void DrawShadow() override          { ageHook::Thunk<0x4CE940>::Call<void>(this); }
        virtual AGE_API void DrawShadowMap() override       { ageHook::Thunk<0x4CEA90>::Call<void>(this); }
        virtual AGE_API void DrawGlow() override            { ageHook::Thunk<0x4CEB90>::Call<void>(this); }
        virtual AGE_API void DrawReflected(float a1) override
                                                            { ageHook::Thunk<0x4CF080>::Call<void>(this, a1); }
        virtual AGE_API unsigned int SizeOf() override      { return ageHook::Thunk<0x4CDFE0>::Call<int>(this); }



        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehCarModel>("vehCarModel")
                .addFunction("Reset", &Reset)
                .addFunction("BreakElectrics", &BreakElectrics)
                .addFunction("ClearDamage", &ClearDamage)
                .addFunction("EjectOneshot", &EjectOneshot)
                .addFunction("GetVisible", &GetVisible)
                .addFunction("SetVisible", &SetVisible)
            .endClass();
        }
    private:
        byte _buffer[0xCC];
    };

    class vehCarSim : public asNode {
    protected:
        //ageHook::Field<0x25C, vehEngine *> _engine;
    public:
        AGE_API vehCarSim()                                 { ageHook::Thunk<0x4CB660>::Call<void>(this); }
        AGE_API ~vehCarSim()                                { ageHook::Thunk<0x4CB8E0>::Call<void>(this); }

        AGE_API bool BottomedOut()                          { return ageHook::Thunk<0x4CBB40>::Call<bool>(this); }
        AGE_API bool OnGround()                             { return ageHook::Thunk<0x4CBB00>::Call<bool>(this); }
        AGE_API void ReconfigureDrivetrain()                { ageHook::Thunk<0x4CC0B0>::Call<void>(this); }
        AGE_API void SetHackedImpactParams()                { ageHook::Thunk<0x4CC080>::Call<void>(this); }
        AGE_API void RestoreImpactParams()                  { ageHook::Thunk<0x4CC050>::Call<void>(this); }

        /*
            asNode virtuals
        */

        virtual AGE_API void Update()  override             {ageHook::Thunk<0x4CC8E0>::Call<void>(this); }
        virtual AGE_API void Reset()  override              { ageHook::Thunk<0x4CBA70>::Call<void>(this); }
        virtual AGE_API void FileIO(datParser &parser)  override
                                                            { ageHook::Thunk<0x4CCC70>::Call<void>(this); }
        virtual AGE_API char* GetClassName() override       { return ageHook::Thunk<0x4CCF10>::Call<char*>(this); }
        virtual AGE_API char const* GetDirName() override   { return ageHook::Thunk<0x4CBAF0>::Call<char const*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehCarSim>("vehCarSim")
                .addFunction("Reset", &Reset)
                .addFunction("BottomedOut", &BottomedOut)
                .addFunction("OnGround", &OnGround)
                .addFunction("ReconfigureDrivetrain", &ReconfigureDrivetrain)
                .addFunction("SetHackedImpactParams", &SetHackedImpactParams)
                .addFunction("RestoreImpactParams", &RestoreImpactParams)
                .endClass();
        }
    private:
        byte _buffer[0x1560];
    };

    class vehCarDamage : public asNode {
    public:
        AGE_API vehCarDamage()                              { ageHook::Thunk<0x4CA380>::Call<void>(this); }
        AGE_API ~vehCarDamage()                             { ageHook::Thunk<0x4CA530>::Call<void>(this); }

        AGE_API void AddDamage(float a1)                    { ageHook::Thunk<0x4CAEC0>::Call<void>(this, a1); }
        AGE_API void ClearDamage()                          { ageHook::Thunk<0x4CAE80>::Call<void>(this); }

        /*
            vehCarDamage virtuals
        */

        virtual AGE_API float GetDamageModifier(phCollider* a1) 
                                                            { return ageHook::Thunk<0x4CB650>::Call<float>(this, a1); }
        
        /*
            asNode virtuals
        */
        virtual AGE_API void Update() override              { ageHook::Thunk<0x5B2C30>::Call<void>(this); }
        virtual AGE_API void Reset() override               { ageHook::Thunk<0x4CAE60>::Call<void>(this); }
        virtual AGE_API void FileIO(datParser &parser) override
                                                            { ageHook::Thunk<0x4CB400>::Call<void>(this); }
        virtual AGE_API char* GetClassName() override       { return ageHook::Thunk<0x4CB640>::Call<char*>(this); }
        virtual AGE_API char const* GetDirName() override   { return ageHook::Thunk<0x4CA5F0>::Call<char const*>(this); }
        

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehCarDamage>("vehCarDamage")
                .addFunction("Reset", &Reset)
                .addFunction("AddDamage", &AddDamage)
                .addFunction("ClearDamage", &ClearDamage)
            .endClass();
        }
    private:
        byte _buffer[0x3A4];
    };
    
    class vehCar : public dgPhysEntity {
        // vehCarSim: 0xB8 (size: ~0x1560)
    protected:
        ageHook::Field<0xC0, vehCarDamage *> _damage;
        ageHook::Field<0xB8, vehCarSim *> _sim;
        ageHook::Field<0xBC, vehCarModel *> _model;
    public:
        AGE_API vehCar(BOOL a1)                             { ageHook::Thunk<0x42BAB0>::Call<void>(this, a1); }
        AGE_API ~vehCar()                                   { ageHook::Thunk<0x42BCC0>::Call<void>(this); }


        inline vehCarDamage* getCarDamage(void) const {
            return _damage.get(this);
        };

        inline vehCarSim* getCarSim(void) const {
            return _sim.get(this);
        }

        inline vehCarModel* getModel(void) const {
            return _model.get(this);
        }

        AGE_API void SetDrivable(BOOL a1, BOOL a2)          { ageHook::Thunk<0x42C2C0>::Call<void>(this, a1, a2); }
        AGE_API void ClearDamage()                          { ageHook::Thunk<0x42C450>::Call<void>(this); }
        AGE_API bool IsPlayer()                             { return ageHook::Thunk<0x42C890>::Call<bool>(this); }

        /*
            dgPhysEntity virtuals
        */

        virtual AGE_API bool RequiresTerrainCollision() override
                                                            { return ageHook::Thunk<0x42CA90>::Call<bool>(this); }
        virtual AGE_API lvlInstance* GetInst() override     { return ageHook::Thunk<0x42CA80>::Call<lvlInstance *>(this); }
        virtual AGE_API phInertialCs* GetICS()              { return ageHook::Thunk<0x42CA70>::Call<phInertialCs *>(this); }
        virtual AGE_API void PostUpdate() override          { ageHook::Thunk<0x42C8B0>::Call<void>(this); }
        virtual AGE_API void Update() override              { ageHook::Thunk<0x42C690>::Call<void>(this); }
        virtual AGE_API void PreUpdate() override           { ageHook::Thunk<0x42C480>::Call<void>(this); }


        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehCar>("vehCar")
                .addFunction("getCarDamage", &getCarDamage)
                .addFunction("getCarSim", &getCarSim)
                .addFunction("getModel", &getModel)
                .addFunction("GetInst", &GetInst)
                .addFunction("ClearDamage", &ClearDamage)
                .addFunction("SetDrivable", &SetDrivable, LUA_ARGS(bool,bool))
                .addFunction("IsPlayer", &IsPlayer)
            .endClass();
        }
    private:
        byte _buffer[0x25C];
    };

    class vehCarAudioContainer {
    public:
        AGE_API static void SetSirenCSVName(LPCSTR name) {
            ageHook::StaticThunk<0x4D0C80>::Call<void>(name);
        };
    };

    class vehPoliceCarAudio {
    public:
        static ageHook::Type<int> iNumCopsPursuingPlayer;
    };
}