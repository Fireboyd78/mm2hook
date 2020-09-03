#pragma once
#include "mm2_common.h"

#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class mmVehInfo;
    class mmVehList;

    // External declarations
    extern class vehBreakable;
    extern class vehBreakableMgr;
    extern class vehCar;
    extern class vehCarDamage;
    extern class vehCarModel;
    extern class vehEngine;
    extern class vehCarSim;
    extern class vehTransmission;
    extern class vehTrailer;
    extern class vehTrailerInstance;

    class mmVehInfo {
    private:
        char baseName[40];
        char description[80];
        char colors[100];
        int flags;
        int order;
        BOOL isValid;
        int unlockScore;
        int unlockFlags;
        BOOL isLocked;
        int horsepower;
        int topSpeed;
        int durability;
        int mass;
        int rewardFlags;
        int unk_10C;
        float scoringBias;
        float uiDist;
        float unk_118;
        float unk_11C;
        unsigned int tuningCrc;
    public:
        AGE_API mmVehInfo(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x529720>::Call<void>(this);
        }

        virtual AGE_API ~mmVehInfo(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x529770>::Call<void>(this);
        }

        inline char * GetBaseName(void) {
            return baseName;
        }

        inline char * GetDescription(void) {
            return description;
        }

        inline char* GetColors(void) {
            return colors;
        }

        inline unsigned int GetTuningCRC(void) {
            return tuningCrc;
        }

        //helpers
        void SetBaseName(LPCSTR basename) {
            strcpy_s(this->baseName, 40, basename);
        }

        void SetDescription(LPCSTR description) {
            strcpy_s(this->description, 80, description);
        }

        void SetColors(LPCSTR colors) {
            strcpy_s(this->colors, 100, colors);
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<mmVehInfo>("mmVehInfo")
                .addProperty("Colors", &GetColors, &SetColors)
                .addProperty("Basename", &GetBaseName, &SetBaseName)
                .addProperty("Description", &GetDescription, &SetDescription)
                .addVariableRef("Flags", &mmVehInfo::flags)
                .addVariableRef("Order", &mmVehInfo::order)
                .addVariableRef("UnlockFlags", &mmVehInfo::unlockFlags)
                .addVariableRef("UnlockScore", &mmVehInfo::unlockScore)
                .addVariableRef("Horsepower", &mmVehInfo::horsepower)
                .addVariableRef("TopSpeed", &mmVehInfo::topSpeed)
                .addVariableRef("Durability", &mmVehInfo::durability)
                .addVariableRef("Mass", &mmVehInfo::mass)
                .addVariableRef("UIDist", &mmVehInfo::uiDist)
                .addPropertyReadOnly("TuningCRC", &GetTuningCRC)
                .endClass();
        }
    };

    class mmVehList {
    private:
        mmVehInfo **vehInfos;
        mmVehInfo *defaultVehicle;
        int numVehicles;
    public:
        AGE_API mmVehList(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x524550>::Call<void>(this);
        }

        virtual AGE_API ~mmVehList(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x524570>::Call<void>(this);
        }

        AGE_API void LoadAll()                              { ageHook::Thunk<0x524950>::Call<void>(this); }

        AGE_API int GetVehicleID(char *vehicle)             { return ageHook::Thunk<0x5246B0>::Call<int>(this, vehicle); }

        AGE_API mmVehInfo * GetVehicleInfo(int vehicle)     { return ageHook::Thunk<0x5245E0>::Call<mmVehInfo *>(this, vehicle); }
        AGE_API mmVehInfo * GetVehicleInfo(char *vehicle)   { return ageHook::Thunk<0x524610>::Call<mmVehInfo *>(this, vehicle); }

        AGE_API void SetDefaultVehicle(char* vehicle)       { ageHook::Thunk<0x524690>::Call<void>(this, vehicle); }
        AGE_API void Print()                                { ageHook::Thunk<0x524810>::Call<void>(this); }
        
        //helper
        inline int GetNumVehicles() {
            return this->numVehicles;
        }

        //lua
        static void BindLua(LuaState L) {
            /*throws nearly triple digit errors?
            LuaBinding(L).beginClass<mmVehList>("mmVehList")
                .addStaticProperty("Instance", [] { return (mmVehList*)0x6B1CA8; }) //HACK but it should work
                .addPropertyReadOnly("NumVehicles", &GetNumVehicles)
                .addFunction("GetVehicleInfo", static_cast<mmVehInfo *(mmVehList::*)(char*)>(&GetVehicleInfo))
                .addFunction("GetVehicleInfoFromID", static_cast<mmVehInfo *(mmVehList::*)(int)>(&GetVehicleInfo))
                .addFunction("GetVehicleID", &GetVehicleID)
                .addFunction("SetDefaultVehicle", &SetDefaultVehicle)
                .endClass();*/
        }
    };

    declhook(0x6B1CA8, _Type<mmVehList *>, VehicleListPtr);

    template<>
    void luaAddModule<module_vehicle>(LuaState L) {
        luaBind<vehWheel>(L);
        luaBind<vehAero>(L);
        luaBind<vehGyro>(L);
        luaBind<vehAxle>(L);
        luaBind<vehStuck>(L);
        luaBind<vehDrivetrain>(L);
        luaBind<vehSiren>(L);
        luaBind<vehBreakable>(L);
        luaBind<vehBreakableMgr>(L);
        luaBind<vehSplash>(L);
        luaBind<vehCar>(L);
        luaBind<vehCarDamage>(L);
        luaBind<vehCarModel>(L);
        luaBind<vehEngine>(L);
        luaBind<vehCarSim>(L);
        luaBind<vehTransmission>(L);
        luaBind<vehTrailer>(L);
        luaBind<vehTrailerInstance>(L);

        luaBind<mmVehInfo>(L);
        luaBind<mmVehList>(L);
    }
}