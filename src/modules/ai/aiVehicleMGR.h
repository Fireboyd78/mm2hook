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
        byte _buffer[0x177A4 - 4]; //size - vtable
    public:
        static ageHook::Type<aiVehicleManager *> Instance;

        AGE_API aiVehicleManager(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x553B30>::Call<void>(this);
            POP_VTABLE();
        }

        AGE_API virtual ~aiVehicleManager(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x553C2>::Call<void>(this);
            POP_VTABLE();
        }

        //members
        AGE_API void Init(char* unused)                           { ageHook::Thunk<0x553CE0>::Call<void>(this, unused); }
        AGE_API int AddVehicleDataEntry(LPCSTR name)              { return ageHook::Thunk<0x553FA0>::Call<int>(this, name); }
        AGE_API void SaveEntry()                                  { ageHook::Thunk<0x5541E0>::Call<void>(this); }
        /*
        AGE_API aiVehicleActive Attach
        AGE_API aiVehicleActive Detach
        */

        //asNode overrides
        AGE_API void Reset() override                             { ageHook::Thunk<0x553D60>::Call<void>(this); }
        AGE_API void Update() override                            { ageHook::Thunk<0x553EA0>::Call<void>(this); }

        //helpers
        int getDataCount() {
            return *getPtr<int>(this, 0x1798);
        }

        aiVehicleData* getData(int num) {
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

    //fix this!!
    //ASSERT_SIZEOF(aiVehicleManager, 0x177A4);

    // Lua initialization

}