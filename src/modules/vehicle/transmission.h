#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehTransmission;

    // External declarations


    // Class definitions

    class vehTransmission : public asNode {
    protected:
        ageHook::Field<0x24, int> _gear;
    public:
        inline int getGear(void) {
            return _gear.get(this);
        };

        AGE_API void Upshift()                              { ageHook::Thunk<0x4CF570>::Call<void>(this); }
        AGE_API void Downshift()                            { ageHook::Thunk<0x4CF5B0>::Call<void>(this); }
        AGE_API void SetReverse()                           { ageHook::Thunk<0x4CF6C0>::Call<void>(this); }
        AGE_API void SetNeutral()                           { ageHook::Thunk<0x4CF6D0>::Call<void>(this); }
        AGE_API void SetForward()                           { ageHook::Thunk<0x4CF6E0>::Call<void>(this); }
        AGE_API void SetCurrentGear(int a1)                 { ageHook::Thunk<0x4CF700>::Call<void>(this, a1); }
        AGE_API void Automatic(BOOL a1)                     { ageHook::Thunk<0x4CF6B0>::Call<void>(this, a1); }
        AGE_API float GearRatioFromMPH(float a1)            { return ageHook::Thunk<0x4CF530>::Call<float>(this, a1); }

        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { ageHook::Thunk<0x4CF600>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x4CF200>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser)  override
                                                            { ageHook::Thunk<0x4CF740>::Call<void>(this); }
        AGE_API char* GetClassName() override               { return ageHook::Thunk<0x4CF880>::Call<char*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehTransmission, asNode>("vehTransmission")
                .addProperty("Gear", &getGear, &SetCurrentGear)
                .addFunction("Automatic", &Automatic, LUA_ARGS(bool))
                .addFunction("Downshift", &Downshift)
                .addFunction("Upshift", &Upshift)
                .addFunction("SetReverse", &SetReverse)
                .addFunction("SetNeutral", &SetNeutral)
                .addFunction("SetForward", &SetForward)
                .addFunction("SetCurrentGear", &SetCurrentGear)
                .addFunction("GearRatioFromMPH", &GearRatioFromMPH)
                .endClass();
        }
    private:
        byte _buffer[0x4C];
    };

    // Lua initialization

}