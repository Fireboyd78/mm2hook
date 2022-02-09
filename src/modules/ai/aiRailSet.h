#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward declarations
    class aiRailSet;

    // External declarations


    // Class definitions
    class aiRailSet {
    private:
        float BackBumperDist;
        float FrontBumperDist;
        float LSideDist;
        float RSideDist;
        short EnterInt;
        float RoadDist;
        float ResumeDist;
        float SubSectionDist;
        float TurnDist;
        float LaneRandomness;
        short WaitCount;
        short SSIdx;
        short CurLane;
        short NextLane;
        short TargetLane;
        int RailType;
        aiPath* NextLink;
        aiPath* CurLink;
        short field_40;
        short field_42;
        float CurAccelFactor;
        float ExheedLimit;
        float TargetVelocity;
        float CarReactDist;
        float VehicleAccelFactor;
        float SeparationDist;
        float IntersectionReactDist;
        float field_60;
        float field_64;
        float field_68;
        float field_6C;
        float field_70;
        float field_74;
        float field_78;
        float field_7C;
    public:
        inline short getCurLane()
        {
            return this->CurLane;
        }

        inline aiPath * getCurLink()
        {
            return this->CurLink;
        }

        inline float getExheedLimit()
        {
            return this->ExheedLimit;
        }

        inline float getCurAccelFactor()
        {
            return this->CurAccelFactor;
        }

        inline float getRoadDist()
        {
            return this->RoadDist;
        }

        inline void setRoadDist(float dist)
        {
            this->RoadDist = dist;
        }
    };
    ASSERT_SIZEOF(aiRailSet, 0x80);

    // Lua initialization

}