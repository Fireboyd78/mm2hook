#pragma once
#include <modules\ai.h>
#include <modules\ai\aiVehicleData.h>

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
        byte buffer[0xA48];
    public:
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
        aiVehicleData vehicleDatas[32];
        int numVehicleDatas;
        aiVehicleActive* activeActives[32];
        aiVehicleActive aiVehicleActives[32];
        short attachedCount;
        short gap;
        ltLight* sharedLight;
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
            return this->numVehicleDatas;
        }

        aiVehicleData * getData(int num) {
            //clamp
            int max = getDataCount();
            if (num >= max)
                num = max - 1;

            //return data
            return &this->vehicleDatas[num];
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

    // Lua initialization

}