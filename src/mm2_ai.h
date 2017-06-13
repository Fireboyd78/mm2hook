#pragma once
#include "mm2_common.h"

#include "mm2_audio.h"
#include "mm2_base.h"

namespace MM2
{
    // Forward declarations
    class aiMap;
    class aiPedAudio;
    class aiPedestrian;
    class aiPedestrianInstance;
    class aiVehicleManager;

    //External declarations
    extern class asNode;
    extern class AudCreatureContainer;

    HOOK_EXPORT(0x6B2E10, _Type<MM2::aiMap>, $AIMAP);

    namespace $aiPath {
        HOOK_EXPORT(0x544150, _MemberFunc<void>, UpdatePedestrians);
    }
    namespace $aiPedestrian {
        HOOK_EXPORT(0x54B9C0, _MemberFunc<void>, Update);
    }
    namespace $aiPoliceForce {
        HOOK_EXPORT(0x550EF0, _MemberFunc<void>, Reset);
    }
    namespace $aiPoliceOfficer {
        HOOK_EXPORT(0x53DBF0, _MemberFunc<void>, StartSiren);
        HOOK_EXPORT(0x53DFD0, _MemberFunc<void>, DetectPerpetrator);
    }
    namespace $aiMap {
        HOOK_EXPORT(0x538840, _MemberFunc<void>, Dump);
        HOOK_EXPORT(0x534AB0, _MemberFunc<aiPedestrian *>, Pedestrian);
    }
    namespace $aiVehicleManager {
        HOOK_EXPORT(0x6B31AC, _Type<aiVehicleManager *>, Instance);
    }
    namespace $aiVehicleSpline {
        HOOK_EXPORT(0x568410, _MemberFunc<void>, UpdateObstacleMap);
        HOOK_EXPORT(0x551CB0, _MemberFunc<void>, StopVoice);
        HOOK_EXPORT(0x551CA0, _MemberFunc<void>, PlayHorn);
    }

    class aiPedAudio : public AudCreatureContainer {

    };

    class aiPedestrian {
    public:
        inline aiPedAudio* getAudio(void) const {
            return getPtr<aiPedAudio>(this, 0xC0);
        };
    };

    class aiVehicleManager : public asNode {
    public:
        AGE_API static aiVehicleManager * Instance(void) {
            return $aiVehicleManager::Instance;
        };
    };

    class aiMap {
    public:
        AGE_API void Dump(void) {
            $aiMap::Dump(this);
        };

        static aiMap* Instance(void) {
            return &($AIMAP);
        };

        aiPedestrian* Pedestrian(int num) {
            return $aiMap::Pedestrian(this, num);
        }
    };
}