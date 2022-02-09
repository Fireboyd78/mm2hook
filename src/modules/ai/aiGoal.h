#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward declarations
    class aiGoal;
    class aiGoalAvoidPlayer;

    // External declarations


    // Class definitions
    class aiGoal {
    private:
        int field_0;
        int UpdateCount;
    };
    ASSERT_SIZEOF(aiGoal, 8);

    class aiGoalAvoidPlayer : public aiGoal {
    private:
        int field_8;
        float Side;
        float Heading;
        float PlayerSideReactDist;
        aiRailSet* RailSet;
        aiVehicleAmbient* VehicleAmbient;
        int field_20;
    private:
        AGE_API void AvoidPlayer()
        {
            auto AIMAP = &aiMap::Instance;
            auto player = AIMAP->Player(this->VehicleAmbient->getPlayerId());
            auto playerMtx = player->getCar()->getCarSim()->getICS()->GetMatrix();
            auto vehicleMtx = &this->VehicleAmbient->GetMatrix();

            float distDiffX = playerMtx->m30 - vehicleMtx->m30;
            float distDiffY = playerMtx->m31 - vehicleMtx->m31;
            float distDiffZ = playerMtx->m32 - vehicleMtx->m32;

            float distX = vehicleMtx->m01 * distDiffY + vehicleMtx->m02 * distDiffZ + vehicleMtx->m00 * distDiffX;
            float distZ = -vehicleMtx->m21 * distDiffY + -vehicleMtx->m22 * distDiffZ + -vehicleMtx->m20 * distDiffX;

            float sideDistX, sideDistZ;

            bool isDivided = this->RailSet->getCurLink()->getFlags() & 1;

            if (!isDivided || this->RailSet->getCurLane())
            {
                if (this->Side >= 0.f)
                {
                    sideDistX = distX - this->PlayerSideReactDist;
                    goto LABEL_0;
                }
            }
            else if (frand() < 0.5f) {
                this->VehicleAmbient->setCurSpeed(this->VehicleAmbient->getCurSpeed() - 0.5f);
                sideDistZ = distZ;
                goto LABEL_1;
            }

            sideDistX = this->PlayerSideReactDist + distX;
        LABEL_0:
            sideDistZ = atan2(sideDistX, distZ);
        LABEL_1:
            float speedCalc = this->VehicleAmbient->getCurSpeed() / (this->RailSet->getCurLink()->getSpeedLimit() + this->RailSet->getExheedLimit());

            if ((byte)this->field_20)
            {
                if (sideDistZ >= -0.02f) {
                    if (sideDistZ > 0.02f)
                        sideDistZ = 0.02f;
                    this->Heading -= speedCalc * sideDistZ;
                }
                else {
                    this->Heading += speedCalc * 0.02f;
                }
            }
            else {
                this->Heading -= speedCalc * 0.02f;
            }

            vehicleMtx->MakeRotateY(this->Heading);
            float seconds = datTimeManager::Seconds;

            this->VehicleAmbient->setCurSpeed(this->RailSet->getCurAccelFactor() * seconds + this->VehicleAmbient->getCurSpeed());
            if (this->VehicleAmbient->getCurSpeed() < 0.25f)
                this->VehicleAmbient->setCurSpeed(0.f);

            float speedCalc2 = this->RailSet->getCurLink()->getSpeedLimit() + this->RailSet->getExheedLimit();
            if (speedCalc2 < this->VehicleAmbient->getCurSpeed())
                this->VehicleAmbient->setCurSpeed(speedCalc2);

            float speedMultiplier = seconds * this->VehicleAmbient->getCurSpeed();
            this->RailSet->setRoadDist(speedMultiplier + this->RailSet->getRoadDist());

            vehicleMtx->m30 -= vehicleMtx->m20 * speedMultiplier;
            vehicleMtx->m31 -= vehicleMtx->m21 * speedMultiplier;
            vehicleMtx->m32 -= vehicleMtx->m22 * speedMultiplier;

            this->VehicleAmbient->SolveYPositionAndOrientation();
        }
    public:
        inline void avoidPlayer()
        {
            this->AvoidPlayer();
        }
    };
    ASSERT_SIZEOF(aiGoalAvoidPlayer, 0x24);

    // Lua initialization

}