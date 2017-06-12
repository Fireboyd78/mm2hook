#pragma once
#include "mm2_common.h"
#include "mm2_inst.h"
#include "mm2_ai.h"
#include "mm2_base.h"
#include "mm2_audio.h"

namespace MM2
{
    // Forward declarations
    extern class aiMap;
    extern class aiPedestrianInstance;
    extern class aiVehicleManager;

    //Forward declarations
    extern class aiPedestrian;

    //External declarations
    extern class asNode;

    namespace $
    {
        namespace aiVehiclePhysics {

        }
        namespace aiRouteRacer {

        }
        namespace aiMap
        {
            HOOK_API AGEHook<0x538840>::MemberFunc<void> Dump;
            //HOOK_API
            HOOK_API AGEHook<0x00534AB0>::MemberFunc<aiPedestrian *>Pedestrian;
        }
        namespace aiVehicleSpline {
            HOOK_API AGEHook<0x00568410>::MemberFunc<void> UpdateObstacleMap;
            HOOK_API AGEHook<0x00551CB0>::MemberFunc<void> StopVoice;
            HOOK_API AGEHook<0x00551CA0>::MemberFunc<void> PlayHorn;
        }
        namespace aiPoliceOfficer {
            HOOK_API AGEHook<0x0053DBF0>::MemberFunc<void> StartSiren;
            HOOK_API AGEHook<0x0053DFD0>::MemberFunc<void> DetectPerpetrator;
        }
        namespace aiVehicleManager {
            HOOK_API AGEHook<0x006B31AC>::Type<MM2::aiVehicleManager *> Instance;
        }
        HOOK_API AGEHook<0x6B2E10>::Type<MM2::aiMap> AIMAP;
    }

    class aiPedestrian {
    public:
        inline aiPedAudio* getAudio(void) const {
            return getPtr<aiPedAudio>(this, 0xC0);
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
            $::aiMap::Dump(this);
        };

        static aiMap* Instance(void) {
            return &$::AIMAP;
        };

        aiPedestrian* Pedestrian(int num) {
            return $::aiMap::Pedestrian(this, num);
        }
    };

    HOOK_API AGEHook<0x544150>::MemberFunc<void> $aiPath_UpdatePedestrians;
    HOOK_API AGEHook<0x54B9C0>::MemberFunc<void> $aiPedestrian_Update;

    HOOK_API AGEHook<0x550EF0>::MemberFunc<void> $aiPoliceForce_Reset;
}