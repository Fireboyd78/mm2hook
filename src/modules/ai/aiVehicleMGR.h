#pragma once
#include <modules\ai.h>
#include <modules\ai\aiVehicleData.h>

#include "aiVehicleSpline.h"
#include "aiRailSet.h"

namespace MM2
{
    // Forward declarations
    class aiVehicleActive;
    class aiVehicleManager;

    // External declarations
    extern class asNode;
    extern class ltLight;

    // Class definitions
    class aiVehicleActive : dgPhysEntity {
    private:
        datCallback* CallBack;
        asParticles Particles;
        asParticles Particles2;
        asBirthRule BirthRule;
        asBirthRule BirthRule2;
        float CurDamage;
        float MaxDamage;
        int field_2D0;
        lvlInstance* Instance;
        phInertialCS ICS;
        phSleep Sleep;
        vehWeelCheap WheelFrontLeft;
        vehWeelCheap WheelFrontRight;
        vehWeelCheap WheelBackLeft;
        vehWeelCheap WheelBackRight;
    public:
        inline vehWeelCheap * getWheel(int num) {
            switch (num) {
            case 0:
                return &WheelFrontLeft;
            case 1:
                return &WheelFrontRight;
            case 2:
                return &WheelBackLeft;
            case 3:
                return &WheelBackRight;
            }
            return nullptr;
        }

        /*
            dgPhysEntity virtuals
        */
        virtual AGE_API void Update() override                    { hook::Thunk<0x553890>::Call<void>(this); }
        virtual AGE_API void PostUpdate() override                { hook::Thunk<0x553960>::Call<void>(this); }
        virtual AGE_API phInertialCS* GetICS() override           { return hook::Thunk<0x5543B0>::Call<phInertialCS*>(this); }
        virtual AGE_API lvlInstance* GetInst() override           { return hook::Thunk<0x553430>::Call<lvlInstance*>(this); }
        virtual AGE_API void DetachMe() override                  { hook::Thunk<0x553690>::Call<void>(this); }
    };
    ASSERT_SIZEOF(aiVehicleActive, 0xAFC);

    class aiVehicleManager : public asNode {
    private:
        aiVehicleData VehicleDatas[32];
        int NumVehicleDatas;
        aiVehicleActive* ActiveActives[32];
        aiVehicleActive AiVehicleActives[32];
        short AttachedCount;
        short Gap;
        ltLight* SharedLight;
    public:
        static hook::Type<aiVehicleManager *> Instance;
        static hook::Type<int> SignalClock;

        AGE_API aiVehicleManager(void) {
            scoped_vtable x(this);
            hook::Thunk<0x553B30>::Call<void>(this);
        }

        AGE_API virtual ~aiVehicleManager(void) {
            scoped_vtable x(this);
            hook::Thunk<0x553C2>::Call<void>(this);
        }

        //members
        AGE_API void Init(char *unused)                           { hook::Thunk<0x553CE0>::Call<void>(this, unused); }
        AGE_API int AddVehicleDataEntry(LPCSTR name)              { return hook::Thunk<0x553FA0>::Call<int>(this, name); }
        AGE_API void SaveEntry()                                  { hook::Thunk<0x5541E0>::Call<void>(this); }
        /*
        AGE_API aiVehicleActive Attach
        AGE_API aiVehicleActive Detach
        */

        //asNode overrides
        AGE_API void Reset() override                             { hook::Thunk<0x553D60>::Call<void>(this); }
        AGE_API void Update() override                            { hook::Thunk<0x553EA0>::Call<void>(this); }

        //helpers
        int getDataCount() {
            return this->NumVehicleDatas;
        }

        aiVehicleData * getData(int num) {
            //clamp
            int max = getDataCount();
            if (num >= max)
                num = max - 1;

            //return data
            return &this->VehicleDatas[num];
        }

        inline ltLight * getSharedLight()
        {
            return this->SharedLight;
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiVehicleManager, asNode>("aiVehicleManager")
                .addStaticProperty("Instance", [] { return Instance.get(); })
                
                .addFunction("AddVehicleDataEntry", &AddVehicleDataEntry)
                .addFunction("SaveEntry", &SaveEntry)

                .addPropertyReadOnly("DataCount", &getDataCount)
                .addFunction("GetData", &getData)
            .endClass();
        }
    };
    ASSERT_SIZEOF(aiVehicleManager, 0x177A4);

    class aiVehicleInstance : public lvlInstance {
    public:
        static int AmbientHeadlightStyle;
        static int MM1StyleShadows;
        static bool ShadowTransparency;
    private:
        aiVehicleSpline* Spline;
        short SignalFrequency;
        byte SignalState;
        short LOD;
        short Variant;
        vehBreakableMgr* BreakableMgr;
        Vector3 HeadlightPosition;
        Vector3 VehiclePosition;
    public:
        AGE_API aiVehicleInstance(aiVehicleSpline* spline, char* basename)
        {
            hook::Thunk<0x551D90>::Call<void>(this, spline, basename);
        }

        //properties
        inline aiVehicleSpline * getSpline()
        {
            return this->Spline;
        }

        inline int getSignalFrequency()
        {
            return this->SignalFrequency;
        }

        inline byte getSignalState()
        {
            return this->SignalState;
        }

        inline void setSignalState(byte state)
        {
            this->SignalState = state;
        }

        inline short getLOD()
        {
            return this->LOD;
        }

        inline void setLOD(int lod)
        {
            this->LOD = lod;
        }

        inline short getVariant()
        {
            return this->Variant;
        }

        inline vehBreakableMgr * getBreakableMgr()
        {
            return this->BreakableMgr;
        }

        inline Vector3 getHeadlightPosition()
        {
            return this->HeadlightPosition;
        }

        inline Vector3 getVehiclePosition()
        {
            return this->VehiclePosition;
        }

        //members
        AGE_API aiVehicleData * GetData()                          { return hook::Thunk<0x553F80>::Call<aiVehicleData*>(this); }
        AGE_API void SetColor()                                    { hook::Thunk<0x552110>::Call<void>(this); }
        AGE_API bool InitBreakable(const char* basename, const char* breakableName, int geomId)
                                                                   { return hook::Thunk<0x552010>::Call<bool>(this, basename, breakableName, geomId); }

        //overrides
        AGE_API void Reset() override                              { hook::Thunk<0x552100>::Call<void>(this); }
        AGE_API Vector3 const& GetPosition() override              { return hook::Thunk<0x553030>::Call<Vector3 const&>(this); }
        AGE_API Matrix34 const& GetMatrix(Matrix34* a1) override   { return hook::Thunk<0x553020>::Call<Matrix34 const&>(this, a1); }
        AGE_API void SetMatrix(Matrix34 const &a1) override        { hook::Thunk<0x553010>::Call<void>(this, &a1); }
        AGE_API dgPhysEntity* GetEntity() override                 { return hook::Thunk<0x552F50>::Call<dgPhysEntity*>(this); }
        AGE_API dgPhysEntity* AttachEntity() override              { return hook::Thunk<0x552FB0>::Call<dgPhysEntity*>(this); }
        AGE_API void Detach() override                             { hook::Thunk<0x552F80>::Call<void>(this); }
        AGE_API void Draw(int lod) override                        { hook::Thunk<0x552160>::Call<void>(this, lod); }
        AGE_API void DrawShadow() override                         { hook::Thunk<0x552CC0>::Call<void>(this); }
        AGE_API void DrawShadowMap() override                      { hook::Thunk<0x552F30>::Call<void>(this); }
        AGE_API void DrawGlow() override                           { hook::Thunk<0x552930>::Call<void>(this); }
        AGE_API void DrawReflected(float a1) override              { hook::Thunk<0x552CB0>::Call<void>(this, a1); }
        AGE_API unsigned int SizeOf() override                     { return hook::Thunk<0x553060>::Call<int>(this); }
        AGE_API phBound* GetBound(int a1) override                 { return hook::Thunk<0x552F40>::Call<phBound*>(this, a1); }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiVehicleInstance, lvlInstance>("aiVehicleInstance")
                //members
                .addFunction("GetData", &GetData)
                .endClass();
        }
    };
    ASSERT_SIZEOF(aiVehicleInstance, 0x3C);

    // Lua initialization

}