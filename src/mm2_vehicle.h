#pragma once
#include "mm2_common.h"
#include "mm2_inst.h"
#include "mm2_game.h"
#include "mm2_breakable.h"

namespace MM2
{
    // Forward declarations
    class mmVehInfo;
    class mmVehList;
    class vehCar;
    class vehCarAudioContainer;
    class vehPoliceCarAudio;

    class mmVehInfo {
    private:
        char baseName[40];
        char description[80];
        char colors[64];
        int flags;
        int order;
        BOOL valid;
        int unlockScore;
        int unlockFlags;
        BOOL isLocked;
        int horsepower;
        int topSpeed;
        int durability;
        int mass;
        int rewardFlags;
        int unk_10C;
        int scoringBias;
        float uiDist;
        float unk_118;
        float unk_11C;
        unsigned long tuningCRC;
    public:
        AGE_API mmVehInfo(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x529720>::Call<void>(this);
            POP_VTABLE();
        }

        virtual AGE_API ~mmVehInfo(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x529770>::Call<void>(this);
            POP_VTABLE();
        }

        inline char * GetBaseName(void) {
            return baseName;
        }

        inline char * GetDescription(void) {
            return description;
        }
    };

    class mmVehList {
    private:
        mmVehInfo **vehInfos;
        mmVehInfo *defaultVehicle;
        int numVehicles;
    public:
        AGE_API mmVehList(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x524550>::Call<void>(this);
            POP_VTABLE();
        }

        virtual AGE_API ~mmVehList(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x524570>::Call<void>(this);
            POP_VTABLE();
        }

        AGE_API int GetVehicleID(char *vehicle)             { return ageHook::Thunk<0x5246B0>::Call<int>(this, vehicle); }

        AGE_API mmVehInfo * GetVehicleInfo(int vehicle)     { return ageHook::Thunk<0x5245E0>::Call<mmVehInfo *>(this, vehicle); }
        AGE_API mmVehInfo * GetVehicleInfo(char *vehicle)   { return ageHook::Thunk<0x524610>::Call<mmVehInfo *>(this, vehicle); }
    };

    declhook(0x6B1CA8, _Type<mmVehList *>, VehicleListPtr);

    class vehCarModel : public lvlInstance {
    protected:
        ageHook::Field<0xAC, vehBreakableMgr *> _mechanicalBreakableMgr;
        ageHook::Field<0xA0, vehBreakableMgr *> _genericBreakableMgr;
    public:
        AGE_API vehCarModel()                               { ageHook::Thunk<0x4CCF20>::Call<void>(this); }
        AGE_API ~vehCarModel()                              { ageHook::Thunk<0x4CCF80>::Call<void>(this); }

        inline vehBreakableMgr * getGenBreakableMgr(void) const {
            return _genericBreakableMgr.get(this);
        }
        
        inline vehBreakableMgr * getMechBreakableMgr(void) const {
            return _mechanicalBreakableMgr.get(this);
        }

        AGE_API void BreakElectrics(Vector3* a1)            { ageHook::Thunk<0x4CEFE0>::Call<void>(this, a1); }
        AGE_API void ClearDamage()                          { ageHook::Thunk<0x4CDFF0>::Call<void>(this); }
        AGE_API void EjectOneshot()                         { ageHook::Thunk<0x4CDCA0>::Call<void>(this); }
        AGE_API bool GetVisible()                           { return ageHook::Thunk<0x4CF070>::Call<bool>(this); }
        AGE_API void SetVisible(bool a1)                    { ageHook::Thunk<0x4CF050>::Call<void>(this, a1); }
        
        /*
            lvlInstance virtuals
        */

        AGE_API void Reset()  override                      { ageHook::Thunk<0x4CDFD0>::Call<void>(this); }
        AGE_API Vector3 const & GetPosition() override
                                                            { return ageHook::Thunk<0x4CEF50>::Call<Vector3 const &>(this); }
        AGE_API Matrix34 const & GetMatrix(Matrix34 &a1)
                                                            { return ageHook::Thunk<0x4CEF90>::Call<Matrix34 const &>(this, &a1); }
        AGE_API void SetMatrix(Matrix34 const &a1) override 
                                                            { ageHook::Thunk<0x4CEFA0>::Call<void>(this, &a1); }
        AGE_API dgPhysEntity* GetEntity() override          { return ageHook::Thunk<0x4CEFC0>::Call<dgPhysEntity*>(this); }
        AGE_API dgPhysEntity* AttachEntity() override 
                                                            { return ageHook::Thunk<0x4CEFD0>::Call<dgPhysEntity*>(this); }
        AGE_API Vector3 const * GetVelocity() override
                                                            { return ageHook::Thunk<0x4CEF80>::Call<Vector3 const*>(this); }

        AGE_API void Draw(int a1) override                  { ageHook::Thunk<0x4CE040>::Call<void>(this, a1); }
        AGE_API void DrawShadow() override                  { ageHook::Thunk<0x4CE940>::Call<void>(this); }
        AGE_API void DrawShadowMap() override               { ageHook::Thunk<0x4CEA90>::Call<void>(this); }
        AGE_API void DrawGlow() override                    { ageHook::Thunk<0x4CEB90>::Call<void>(this); }
        AGE_API void DrawReflected(float a1) override
                                                            { ageHook::Thunk<0x4CF080>::Call<void>(this, a1); }
        AGE_API unsigned int SizeOf() override              { return ageHook::Thunk<0x4CDFE0>::Call<int>(this); }



        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehCarModel>("vehCarModel")
                .addFunction("getGenBreakableMgr", &getGenBreakableMgr)
                .addFunction("getMechBreakableMgr", &getMechBreakableMgr)
                .addFunction("GetPosition", &GetPosition)
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
        ageHook::Field<0x24C, float> _speed;
        //ageHook::Field<0x25C, vehEngine *> _engine;
    public:
        inline float getSpeed(void) {
            return _speed.get(this);
        };

        AGE_API vehCarSim()                                 { ageHook::Thunk<0x4CB660>::Call<void>(this); }
        AGE_API ~vehCarSim()                                { ageHook::Thunk<0x4CB8E0>::Call<void>(this); }

        AGE_API bool BottomedOut()                          { return ageHook::Thunk<0x4CBB40>::Call<bool>(this); }
        AGE_API bool OnGround()                             { return ageHook::Thunk<0x4CBB00>::Call<bool>(this); }
        AGE_API void ReconfigureDrivetrain()                { ageHook::Thunk<0x4CC0B0>::Call<void>(this); }
        AGE_API void SetHackedImpactParams()                { ageHook::Thunk<0x4CC080>::Call<void>(this); }
        AGE_API void RestoreImpactParams()                  { ageHook::Thunk<0x4CC050>::Call<void>(this); }
        AGE_API void SetResetPos(Vector3 * a1)              { ageHook::Thunk<0x4CC830>::Call<void>(this, a1); }
        
        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { ageHook::Thunk<0x4CC8E0>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x4CBA70>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser)  override
                                                            { ageHook::Thunk<0x4CCC70>::Call<void>(this); }
        AGE_API char* GetClassName() override               { return ageHook::Thunk<0x4CCF10>::Call<char*>(this); }
        AGE_API char const* GetDirName() override           { return ageHook::Thunk<0x4CBAF0>::Call<char const*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehCarSim>("vehCarSim")
                .addFunction("Reset", &Reset)
                .addFunction("BottomedOut", &BottomedOut)
                .addFunction("OnGround", &OnGround)
                .addFunction("ReconfigureDrivetrain", &ReconfigureDrivetrain)
                .addFunction("SetHackedImpactParams", &SetHackedImpactParams)
                .addFunction("RestoreImpactParams", &RestoreImpactParams)
                .addFunction("SetResetPos", &SetResetPos)
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

        AGE_API void Update() override                      { ageHook::Thunk<0x5B2C30>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x4CAE60>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser) override
                                                            { ageHook::Thunk<0x4CB400>::Call<void>(this); }
        AGE_API char* GetClassName() override               { return ageHook::Thunk<0x4CB640>::Call<char*>(this); }
        AGE_API char const* GetDirName() override           { return ageHook::Thunk<0x4CA5F0>::Call<char const*>(this); }
        

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


        AGE_API void ClearDamage()                          { ageHook::Thunk<0x42C450>::Call<void>(this); }
        AGE_API bool IsPlayer()                             { return ageHook::Thunk<0x42C890>::Call<bool>(this); }
        AGE_API void Init(char const* a1, int a2, int a3, bool a4, bool a5)
                                                            { ageHook::Thunk<0x42BE10>::Call<void>(this, a1, a2, a3, a4, a5); }
        AGE_API void InitAudio(char const* a1, int a2)      { ageHook::Thunk<0x42C1F0>::Call<void>(this, a1, a2); }
        AGE_API void SetDrivable(BOOL a1, BOOL a2)          { ageHook::Thunk<0x42C2C0>::Call<void>(this, a1, a2); }

        /*
            dgPhysEntity virtuals
        */

        AGE_API bool RequiresTerrainCollision() override
                                                            { return ageHook::Thunk<0x42CA90>::Call<bool>(this); }
        AGE_API lvlInstance* GetInst() override             { return ageHook::Thunk<0x42CA80>::Call<lvlInstance *>(this); }
        AGE_API phInertialCs* GetICS()                      { return ageHook::Thunk<0x42CA70>::Call<phInertialCs *>(this); }
        AGE_API void PostUpdate() override                  { ageHook::Thunk<0x42C8B0>::Call<void>(this); }
        AGE_API void Update() override                      { ageHook::Thunk<0x42C690>::Call<void>(this); }
        AGE_API void PreUpdate() override                   { ageHook::Thunk<0x42C480>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehCar>("vehCar")
                .addConstructor(LUA_ARGS(bool))
                .addFunction("getCarDamage", &getCarDamage)
                .addFunction("getCarSim", &getCarSim)
                .addFunction("getModel", &getModel)
                .addFunction("Init", &Init)
                .addFunction("InitAudio", &InitAudio)
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

        AGE_API vehPoliceCarAudio * GetPoliceCarAudioPtr() {
            return ageHook::Thunk<0x4D1790>::Call<vehPoliceCarAudio *>(this);
        };
    };

    class vehPoliceCarAudio {
    public:
        static ageHook::Type<int> iNumCopsPursuingPlayer;
    };

    template<>
    void luaAddModule<module_vehicle>(LuaState L) {
        luaBind<vehCar>(L);
        luaBind<vehCarDamage>(L);
        luaBind<vehCarModel>(L);
        luaBind<vehCarSim>(L);
    }
}