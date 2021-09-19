#pragma once
#include <mm2_common.h>

#include <modules\core.h>
#include <modules\data.h>
#include <modules\node.h>
#include <modules\ped.h>

#include "ai\aiMap.h"
#include "ai\aiTrafficLight.h"
#include "ai\aiPedAudio.h"
#include "ai\aiRouteRacer.h"
#include "ai\aiPoliceOfficer.h"
#include "ai\aiVehiclePlayer.h"
#include "ai\aiCableCar.h"
#include "ai\aiSubway.h"
#include "ai\ArrayList.h"
#include "ai\aiPath.h"
#include "ai\aiIntersection.h"
#include "ai\mcHookman.h"
#include "ai\aiPedestrian.h"
#include "ai\aiPoliceForce.h"
#include "ai\aiVehicleAmbient.h"
#include "ai\aiVehicleMGR.h"
#include "ai\aiCTFRacer.h"
#include "ai\aiData.h"
#include "ai\aiCityData.h"
#include "ai\aiVehicle.h"
#include "ai\aiRailSet.h"
#include "ai\aiVehiclePhysics.h"
#include "ai\aiVehicleSpline.h"
#include "ai\aiBanger.h"
#include "ai\aiVehicleData.h"

namespace MM2
{
    // Forward declarations


    // External declarations
    

    // Class definitions

    namespace $
    {
        namespace aiVehicleSpline {
            declhook(0x568410, _MemberFunc<void>, UpdateObstacleMap);
            declhook(0x551CB0, _MemberFunc<void>, StopVoice);
            declhook(0x551CA0, _MemberFunc<void>, PlayHorn);
        }
        namespace aiPedestrian {
            declhook(0x54B9C0, _MemberFunc<void>, Update);
        }
        namespace aiPoliceOfficer {
            declhook(0x53DBF0, _MemberFunc<void>, StartSiren);
            declhook(0x53DC40, _MemberFunc<void>, StopSiren);
            declhook(0x53DFD0, _MemberFunc<void>, DetectPerpetrator);
            declhook(0x53F170, _MemberFunc<void>, PerpEscapes);
        }
        namespace aiVehicleManager {
            declhook(0x6B31AC, _Type<MM2::aiVehicleManager *>, Instance);
        }
    }
    
    // Lua initialization

}