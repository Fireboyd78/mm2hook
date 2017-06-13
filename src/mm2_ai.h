#pragma once
#include "mm2_common.h"
#include "mm2_inst.h"
#include "mm2_ai.h"
#include "mm2_base.h"
#include "mm2_audio.h"

namespace MM2
{
    // Forward declarations
    class aiMap;
    class aiPedestrian;
    class aiPedestrianInstance;
    class aiVehicleManager;

    //External declarations
    extern class asNode;

    namespace $
    {
        namespace aiVehicleSpline {
            HOOK_EXPORT(0x00568410, _MemberFunc<void>, UpdateObstacleMap);
            HOOK_EXPORT(0x00551CB0, _MemberFunc<void>, StopVoice);
            HOOK_EXPORT(0x00551CA0, _MemberFunc<void>, PlayHorn);
        }
        namespace aiPoliceOfficer {
            HOOK_EXPORT(0x0053DBF0, _MemberFunc<void>, StartSiren);
            HOOK_EXPORT(0x0053DFD0, _MemberFunc<void>, DetectPerpetrator);
        }
        namespace aiVehicleManager {
            HOOK_EXPORT(0x006B31AC, _Type<MM2::aiVehicleManager *>, Instance);
        }
        HOOK_EXPORT(0x6B2E10, _Type<MM2::aiMap>, AIMAP);
    }

    class aiPedestrian {
    protected:
        ageHook::Field<0xC0, aiPedAudio *> _audio;
    public:
        inline aiPedAudio* getAudio(void) const {
            return _audio.get(this);
        };
    };

    class aiVehicleManager : public asNode {
    public:
        AGE_API static aiVehicleManager * Instance(void) {
            return $::aiVehicleManager::Instance;
        };
    };

    class aiMap {
    public:
        AGE_API void Dump(void) {
            ageHook::Thunk<0x538840>::Call<void>(this);
        };

        static aiMap* Instance(void) {
            return &$::AIMAP;
        };

        aiPedestrian* Pedestrian(int num) {
            return ageHook::Thunk<0x534AB0>::Call<aiPedestrian *>(this, num);
        }
    };

    HOOK_EXPORT(0x544150, _MemberFunc<void>, $aiPath_UpdatePedestrians);
    HOOK_EXPORT(0x54B9C0, _MemberFunc<void>, $aiPedestrian_Update);

    HOOK_EXPORT(0x550EF0, _MemberFunc<void>, $aiPoliceForce_Reset);
}