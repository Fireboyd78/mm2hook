#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward declarations
    class aiObstacle;
    class aiVehicle;

    // External declarations
    extern class aiVehicleInstance;
    extern class dgPhysEntity;
    extern class phBound;
    extern class aiVehicleData;

    // Class definitions
    class aiVehicleInstance : public lvlInstance {
    public:
        aiVehicleInstance(void)                                    DONOTCALL;

        //overrides
        AGE_API Vector3 const& GetPosition() override              { return hook::Thunk<0x553030>::Call<Vector3 const&>(this); };
        AGE_API Matrix34 const& GetMatrix(Matrix34* a1) override   { return hook::Thunk<0x553020>::Call<Matrix34 const&>(this, a1); };
        AGE_API void SetMatrix(Matrix34 const *a1) override        { hook::Thunk<0x553010>::Call<void>(this, a1); }
        AGE_API dgPhysEntity* GetEntity() override                 { return hook::Thunk<0x52F50>::Call<dgPhysEntity*>(this); };
        AGE_API dgPhysEntity* AttachEntity() override              { return hook::Thunk<0x552FBD>::Call<dgPhysEntity*>(this); };
        AGE_API void Detach() override                             { hook::Thunk<0x552F80>::Call<void>(this); }
        AGE_API void Draw(int a1) override                         { hook::Thunk<0x5521600>::Call<void>(this, a1); }
        AGE_API void DrawShadow() override                         { hook::Thunk<0x552CC0>::Call<void>(this); }
        AGE_API void DrawShadowMap() override                      { hook::Thunk<0x552F30>::Call<void>(this); }
        AGE_API void DrawGlow() override                           { hook::Thunk<0x552930>::Call<void>(this); }
        AGE_API void DrawReflected(float a1) override              { hook::Thunk<0x552CB0>::Call<void>(this, a1); }
        AGE_API unsigned int SizeOf() override                     { return hook::Thunk<0x553060>::Call<int>(this); };
        AGE_API phBound* GetBound(int a1) override                 { return hook::Thunk<0x552F40>::Call<phBound*>(this, a1); };
        
        //members
        aiVehicleData* GetData()                                   { return hook::Thunk<0x553F80>::Call<aiVehicleData*>(this); }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiVehicleInstance, lvlInstance>("aiVehicleInstance")
                //members
                .addFunction("GetData", &GetData)
                .endClass();
        }
    };

    class aiObstacle {
    public:
        aiObstacle(void)                                    DONOTCALL;
        aiObstacle(const aiObstacle &&)                     DONOTCALL;

        virtual BOOL InAccident(void)                       { return hook::Thunk<0x53F5D0>::Call<BOOL>(this); }
        virtual void Position(Vector3 &a1)                  PURE;
        virtual float Speed(void)                           PURE;
        virtual float BreakThreshold(void)                  { return hook::Thunk<0x53F5E0>::Call<float>(this); }
        virtual BOOL Drivable(void)                         { return hook::Thunk<0x53F5F0>::Call<BOOL>(this); }
        virtual int CurrentRoadIdx(aiPath **a1, const bool *a2,int *a3)
                                                            PURE;
        virtual int CurrentRdVert(void)                     PURE;
        virtual void PreAvoid(const Vector3 &a1, const Vector3 &a2, float a3, Vector3 &a4, Vector3 &a5)
                                                            PURE;
        virtual float IsBlockingTarget(const Vector3 &a1, const Vector3 &a2, float a3, float a4)
                                                            PURE;

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<aiObstacle>("aiObstacle")
                .endClass();
        }
    };

    class aiVehicle : public aiObstacle {
    public:
        aiVehicle(void)                                     DONOTCALL;
        aiVehicle(const aiVehicle &&)                       DONOTCALL;

        void PreAvoid(const Vector3 &a1, const Vector3 &a2, float a3, Vector3 &a4, Vector3 &a5) override
                                                            FORWARD_THUNK;
        float IsBlockingTarget(const Vector3 &a1, const Vector3 &a2, float a3, float a4) override
                                                            FORWARD_THUNK;
        
        virtual void Update(void)                           { return hook::Thunk<0x556230>::Call<void>(this); };
        virtual void Reset(void)                            { return hook::Thunk<0x556210>::Call<void>(this); };
        virtual int Type(void)                              PURE;
        virtual Matrix34 & GetMatrix(void)                  PURE;
        virtual float FrontBumperDistance(void)             PURE;
        virtual float BackBumperDistance(void)              PURE;
        virtual float LSideDistance(void)                   PURE;
        virtual float RSideDistance(void)                   PURE;
        virtual int CurrentLane(void)                       PURE;
        virtual int CurrentRoadId(void)                     PURE;
        virtual void DrawId(void)                           PURE;
        virtual void ReplayDebug(void)                      { return hook::Thunk<0x556D00>::Call<void>(this); }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiVehicle, aiObstacle>("aiVehicle")
                .endClass();
        }
    };

    // Lua initialization

}