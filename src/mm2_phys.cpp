#include "mm2_phys.h"
using namespace MM2;

/*
    dgPhysManager
*/
declfield(dgPhysManager::PlayerInst)(0x65D9E0);
declfield(dgPhysManager::Instance)(0x65D9DC);
declfield(dgPhysManager::Gravity)(0x5C6C1C);
declfield(dgPhysManager::OpponentOptimization)(0x660F20);

declfield(dgPhysManager::perfObjPairCollecting)(0x660F08);
declfield(dgPhysManager::perfPhysGathering)(0x660F0C);
declfield(dgPhysManager::perfMoverUpdate)(0x655DAC);
declfield(dgPhysManager::perfTotalCollisionTime)(0x655DA8);
declfield(dgPhysManager::perfPostCollision)(0x65D9D8);
declfield(dgPhysManager::perfPhysCollide)(0x655DB0);