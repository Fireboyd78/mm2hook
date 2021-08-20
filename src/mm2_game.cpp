#include "mm2_game.h"
using namespace MM2;

/*
    mmGameManager
*/
declfield(mmGameManager::Instance)(0x5E0D08);

/*
    RadialGauge
*/
declfield(RadialGauge::bDebugPivot)(0x62762C);

/*
    mmExternalView
*/
bool mmExternalView::EnableMM1StyleHud = false;
bool mmExternalView::EnableMouseBar = false;
bool mmExternalView::SwitchFromMPH2KPH = false;