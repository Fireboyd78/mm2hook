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
            declhook(0x568410, _MemberFunc<void>, UpdateObstacleMap);
            declhook(0x551CB0, _MemberFunc<void>, StopVoice);
            declhook(0x551CA0, _MemberFunc<void>, PlayHorn);
        }
        namespace aiPath {
            declhook(0x544150, _MemberFunc<void>, UpdatePedestrians);            
        }
        namespace aiPedestrian {
            declhook(0x54B9C0, _MemberFunc<void>, Update);
        }
        namespace aiPoliceForce {
            declhook(0x550EF0, _MemberFunc<void>, Reset);
        }
        namespace aiPoliceOfficer {
            declhook(0x53DBF0, _MemberFunc<void>, StartSiren);
            declhook(0x53DFD0, _MemberFunc<void>, DetectPerpetrator);
        }
        namespace aiVehicleManager {
            declhook(0x6B31AC, _Type<MM2::aiVehicleManager *>, Instance);
        }
    }

    class aiPedestrian {
    protected:
        ageHook::Field<0xC0, aiPedAudio> _audio;
    public:
        inline aiPedAudio* getAudio(void) const {
            return _audio.ptr(this);
        };
    };

    class aiVehicleManager : public asNode {
    public:
        static ageHook::Type<aiVehicleManager *> Instance;
    };

    class aiMap {
    public:
        static ageHook::Type<aiMap> Instance;

        AGE_API void Dump(void) {
            ageHook::Thunk<0x538840>::Call<void>(this);
        };

        aiPedestrian* Pedestrian(int num) {
            return ageHook::Thunk<0x534AB0>::Call<aiPedestrian *>(this, num);
        }
    };
}