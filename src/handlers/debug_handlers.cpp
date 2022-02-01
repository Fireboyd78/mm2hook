#include "debug_handlers.h"

using namespace MM2;

static init_handler g_debug_handlers[] = {
	CreateHandler<vehWheelDebugHandler>("vehWheelDebugHandler"),
    CreateHandler<aiMapDebugHandler>("aiMapDebugHandler")
};

/*
    vehWheelDebugHandler
*/

Matrix34 wheelMatrixCopy;
Matrix34 carMatrixCopy;
Vector3 wheelDebugPos;

void vehWheelDebugHandler::Cull()
{
    //
    //rglEnableDisable(MM2::rglToken::RGLTOKEN_TYPE2, false); //no shading please! (WRONG TOKEN!)

    //draw veh world matrix
    auto carSimPtr = *getPtr<vehCarSim*>(this, 0x18);
    if (carSimPtr) {
        auto carMatrix = *getPtr<Matrix34>(carSimPtr, 0x1D4);
        rglDrawAxis(3.f, carMatrix);
    }

    //draw wheel matrix
    auto wheelMatrixPtr = getPtr<Matrix34>(this, 0x24);
    rglDrawAxis(1.f, *wheelMatrixPtr);

    //draw box around wheel
    Vector3 wheelCenterVector = *getPtr<Vector3>(this, 0x1B0);
    float wheelRadius = *getPtr<float>(this, 0x1B0);
    float wheelWidth = *getPtr<float>(this, 0x1C0);

    vglCurrentColor = 0xFF00FFFF;
    Vector3 wheelBoxVector = Vector3(wheelWidth, wheelRadius, wheelRadius);
    rglDrawBox(wheelBoxVector, *wheelMatrixPtr);

    //draw dotted matrix
    if (carSimPtr) {
        auto carMatrixPtr = getPtr<Matrix34>(carSimPtr, 0x1D4);
        carMatrixCopy = Matrix34(*carMatrixPtr);
        wheelMatrixCopy = Matrix34(*wheelMatrixPtr);
        wheelMatrixCopy.Dot(*carMatrixPtr);
        rglDrawAxis(1.f, wheelMatrixCopy);
    }

    //draw hit position
    Matrix34 identityMatrix = Matrix34();
    identityMatrix.Identity();

    rglWorldMatrix(identityMatrix);
    vglBegin(MM2::gfxDrawMode::DRAWMODE_LINELIST, 0);
    vglCurrentColor = 0xFF3F21FD;

    Vector3 lasSurfNormal = *getPtr <Vector3>(this, 0x18C);
    Vector3 lastHitPosition = *getPtr <Vector3>(this, 0x1A4);
    vglVertex3f(lastHitPosition.X, lastHitPosition.Y, lastHitPosition.Z);
    vglVertex3f(lastHitPosition.X + lasSurfNormal.X, lastHitPosition.Y + lasSurfNormal.Y, lastHitPosition.Z + lasSurfNormal.Z);

    //draw vector things
    if (wheelMatrixPtr)
    {
        vglBegin(MM2::gfxDrawMode::DRAWMODE_LINELIST, 0);
        vglCurrentColor = 0xFFFF6A00;
        
        Vector3 rearwardDir = *getPtr <Vector3>(this, 0x198);
        vglVertex3f(wheelMatrixPtr->m30, wheelMatrixPtr->m31, wheelMatrixPtr->m32);
        vglVertex3f(wheelMatrixPtr->m30 + rearwardDir.X, wheelMatrixPtr->m31 + rearwardDir.Y, wheelMatrixPtr->m32 + rearwardDir.Z);
    }

    //draw suspension force
    float targetTravel = *getPtr<float>(this, 0x1F8);
    float compressRate = *getPtr<float>(this, 0x204);
    float sforce2 = *getPtr<float>(this, 0x208);
    float sforce1 = *getPtr<float>(this, 0x1FC);

    float pbumpdisp = *getPtr<float>(this, 0x1C8);
    float pfric = *getPtr<float>(this, 0x1D0);

    char* slipBool = *getPtr<short>(this, 0x224) ? "true" : "false";
    float slipAmt = *getPtr<float>(this, 0x220);

    wheelDebugPos.X = wheelMatrixPtr->m30;
    wheelDebugPos.Y = wheelMatrixPtr->m31 + fabsf(wheelRadius) + 0.5f;
    wheelDebugPos.Z = wheelMatrixPtr->m32;

    vglDrawLabelf(wheelDebugPos, "s_travel %.2f\ns_rate %.2f\ns_forcea %.2f\ns_forceb %.2f\np_disp %.2f\np_fric %.2f\nw_slipping %s\nw_slipamt %.2f", targetTravel, compressRate, sforce1, sforce2, pbumpdisp, pfric, slipBool, slipAmt);


	vglEnd();
}

void vehWheelDebugHandler::Update()
{
	hook::Thunk<0x4D34E0>::Call<void>(this); //call original
    asCullManager::Instance->DeclareCullable(reinterpret_cast<asCullable *>(this));
}

void vehWheelDebugHandler::Install()
{
	if (cfgWheelDebug.Get()) {
		InstallVTableHook("vehWheel::Update (Debug Hook)", &Update, {
			0x5B2DD4
		});

        InstallVTableHook("vehWheel::Cull", &Cull, {
            0x5B2DD0
        });
	}
}

/*
    aiMapVehicleHandler
*/
int test = 0;
void aiMapDebugHandler::Cull()
{
    //Warningf("CULL CALLED AT %d", test);
    test = 0;

    auto thisCasted = reinterpret_cast<aiMap*>(this);

    //draw routing nodes
    auto oppCount = thisCasted->numOpponents;
    if (oppCount == 0)
        return;

    //draw hit position
    Matrix34 identityMatrix = Matrix34();
    identityMatrix.Identity();

    rglWorldMatrix(identityMatrix);
    vglBegin(MM2::gfxDrawMode::DRAWMODE_LINELIST, 0);
    vglCurrentColor = 0xFF0060FF;

    Warningf("l1");
    for (int i = 0; i < oppCount; i++) {
        auto opp = thisCasted->opponents[i];
        auto routeNodesPtr = getPtr<aiRouteNode>(opp, 0x2D4);

        Warningf("l2");
        auto rn1_aaa = &routeNodesPtr[0];
        auto myPos = mmGameManager::Instance->getGame()->getPlayer()->getCar()->getModel()->GetPosition();
        vglVertex3f(rn1_aaa->Position);
        vglVertex3f(myPos);

        for (int j = 0; j < 40 - 1; j++) {
            //Warningf("r1-2");
            auto rn1 = &routeNodesPtr[j];
            auto rn2 = &routeNodesPtr[j+1];
            
            /*if (rn1->Position.Mag() < 0.01f || rn2->Position.Mag() < 0.01f)
                break;*/
            if (j == 16) //D:
                break;

            Warningf("v3f");
            vglVertex3f(rn1->Position.X, rn1->Position.Y, rn1->Position.Z);
            vglVertex3f(rn2->Position.X, rn1->Position.Y, rn1->Position.Z);
        }
        break;
    }

    //Warningf("ve");
    vglEnd();
}

void aiMapDebugHandler::UpdatePaused()
{
    test = 2;
    hook::Thunk<0x5374E0>::Call<void>(this); //call original
    asCullManager::Instance->DeclareCullable(reinterpret_cast<asCullable *>(this));
}


void aiMapDebugHandler::Install()
{
    if (cfgAiDebug.Get()) {
        InstallVTableHook("aiMap::UpdatePaused (Debug Hook)", 
            &UpdatePaused, {
                0x5B5474
            });

        InstallVTableHook("aiMap::Cull", 
            &Cull, {
                0x05B5464
            });
    }
}
