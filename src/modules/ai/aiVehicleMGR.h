#pragma once
#include <modules\ai.h>
#include <modules\ai\aiVehicleData.h>

namespace MM2
{
    // Forward declarations
    class aiVehicleManager;

    // External declarations
    extern class asNode;

    // Class definitions

    class aiVehicleManager : public asNode {
    private:
        byte _buffer[0x177A4 - sizeof(asNode)];
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
            return *getPtr<int>(this, 0x1798);
        }

        aiVehicleData * getData(int num) {
            //clamp
            int max = getDataCount();
            if (num >= max)
                num = max - 1;

            //return data
            auto dataArray =  getPtr<aiVehicleData>(this, 0x18);
            return &dataArray[num];
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