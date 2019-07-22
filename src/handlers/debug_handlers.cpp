#include "debug_handlers.h"

using namespace MM2;

static init_handler g_debug_handlers[] = {
	CreateHandler<vehWheelDebugHandler>("vehWheelDebugHandler")
};
Matrix34 wheelMatrixCopy;
Matrix34 carMatrixCopy;

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
	Vector3 wheelBoxVector = Vector3(wheelWidth, wheelRadius, wheelRadius );
	rglDrawBox(wheelBoxVector, *wheelMatrixPtr);

	//draw dotted matrix
	if (carSimPtr) {
		auto carMatrixPtr = getPtr<Matrix34>(carSimPtr, 0x1D4);
		carMatrixCopy = Matrix34(*carMatrixPtr);
		wheelMatrixCopy = Matrix34(*wheelMatrixPtr);
		wheelMatrixCopy.Dot(carMatrixPtr);
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
	vglEnd();
}

void vehWheelDebugHandler::Update()
{
	ageHook::Thunk<0x4D34E0>::Call<void>(this); //call original
	ageHook::Thunk<0x4A15B0>::ThisCall<void>(*(vehWheelDebugHandler**)0x661784, this); //major hackily declare cullable :D
}

void vehWheelDebugHandler::Install()
{
	if (cfgWheelDebug.Get()) {
		InstallVTableHook("vehWheel::Update (Debug Hook)", &Update, {
			0x5B2DD4
		});
	}

	InstallVTableHook("vehWheel::Cull", &Cull, {
		0x5B2DD0
	});
}
