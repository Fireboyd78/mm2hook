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

    template<>
    void luaAddModule<module_vehicle>(LuaState L) {
        luaBind<vehBreakable>(L);
        luaBind<vehBreakableMgr>(L);
        luaBind<vehCar>(L);
        luaBind<vehCarDamage>(L);
        luaBind<vehCarModel>(L);
        luaBind<vehEngine>(L);
        luaBind<vehCarSim>(L);
        luaBind<vehTransmission>(L);
        luaBind<vehTrailer>(L);
        luaBind<vehTrailerInstance>(L);
    }
}