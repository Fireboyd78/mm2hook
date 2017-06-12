#include "mm2_rgl.h"
using namespace MM2;

/* psdl.obj */

HOOK_SET(0x45CFD0, _Func<void>, $vglTexCoord2f);
HOOK_SET(0x45CFF0, _Func<void>, $vglVertex3f$1);
HOOK_SET(0x45D080, _Func<void>, $vglVertex3f$2);

void MM2::vglTexCoord2f(float u, float v) {
    $vglTexCoord2f(u, v);
}
void MM2::vglVertex3f(float x, float y, float z) {
    $vglVertex3f$1(x, y, z);
}
void MM2::vglVertex3f(Vector3 position) {
    $vglVertex3f$2(position);
}

/* vgl.obj */

HOOK_SET(0x4A5370, _Func<void>, $vgl_VERTEX_VCT1);
HOOK_SET(0x4A5400, _Func<void>, $vgl_VERTEX_VNT1);
HOOK_SET(0x4A5490, _Func<void>, $vglSetFormat);
HOOK_SET(0x4A5500, _Func<void>, $vglBegin);
HOOK_SET(0x4A5A90, _Func<void>, $vglEnd);
HOOK_SET(0x4A5B10, _Func<void>, $vglBeginBatch);
HOOK_SET(0x4A5B80, _Func<void>, $vglEndBatch);
HOOK_SET(0x4A5BA0, _Func<void>, $vglBindTexture);
HOOK_SET(0x4A5C50, _Func<void>, $vglDrawLabel);
HOOK_SET(0x4A5D00, _Func<void>, $vglDrawLabelf);
HOOK_SET(0x4A5D30, _Func<void>, $vglSetCloudMap);
HOOK_SET(0x4A5DE0, _Func<void>, $vglSetOffset);
HOOK_SET(0x4A5F00, _Func<bool>, $rglEnableDisable);
HOOK_SET(0x4A6000, _Func<bool>, $rglIsEnabled);
HOOK_SET(0x4A6060, _Func<void>, $rglWorldMatrix);
HOOK_SET(0x4A6110, _Func<void>, $rglWorldIdentity);
HOOK_SET(0x4A6140, _Func<void>, $rglCard);
HOOK_SET(0x4A6160, _Func<void>, $rglBindTexture2);
HOOK_SET(0x4A6190, _Func<void>, $tglDrawParticle);
HOOK_SET(0x4A6550, _Func<void>, $tglDrawRotatedParticle);
HOOK_SET(0x4A7880, _Func<uint>, $mkfrgba);

void MM2::vgl_VERTEX_VCT1(float x, float y, float z) {
    $vgl_VERTEX_VCT1(x, y, z);
}
void MM2::vgl_VERTEX_VNT1(float x, float y, float z) {
    $vgl_VERTEX_VNT1(x, y, z);
}
void MM2::vglSetFormat(uint p1, uint p2, uint p3, uint p4) {
    $vglSetFormat(p1, p2, p3, p4);
}
void MM2::vglBegin(gfxDrawMode drawMode, int p2) {
    $vglBegin(drawMode, p2);
}
void MM2::vglEnd(void) {
    $vglEnd();
}
void MM2::vglBeginBatch(void) {
    $vglBeginBatch();
}
void MM2::vglEndBatch(void) {
    $vglEndBatch();
}
void MM2::vglBindTexture(gfxTexture *texture) {
    $vglBindTexture(texture);
}
void MM2::vglDrawLabel(const Vector3 &position, const char *text) {
    $vglDrawLabel(&position, text);
}
template <typename ...TArgs>
void MM2::vglDrawLabelf(const Vector3 &position, const char *format, TArgs ...args) {
    $vglDrawLabelf(&position, format, args...);
}
void MM2::vglSetCloudMap(const char *texture) {
    $vglSetCloudMap(texture);
}
void MM2::vglSetOffset(float x, float y, float z) {
    $vglSetOffset(x, y, z);
}
bool MM2::rglEnableDisable(rglToken token, bool value) {
    return $rglEnableDisable(token, value);
}
bool MM2::rglIsEnabled(rglToken token) {
    return $rglIsEnabled(token);
}
void MM2::rglWorldMatrix(const Matrix34 &mtx) {
    $rglWorldMatrix(&mtx);
}
void MM2::rglWorldIdentity(void) {
    $rglWorldIdentity();
}
void MM2::rglCard(const Vector3 &position) {
    $rglCard(&position);
}
void MM2::rglBindTexture2(const gfxTexture *texture) {
    $rglBindTexture2(texture);
}
void MM2::tglDrawParticle(const Vector3 &p1, float p2, const Vector4 &p3) {
    $tglDrawParticle(&p1, p2, &p3);
}
void MM2::tglDrawRotatedParticle(const Vector3 &p1, float p2, float p3, const Vector4 &p4) {
    $tglDrawRotatedParticle(&p1, p2, p3, &p4);
}
uint MM2::mkfrgba(float r, float g, float b, float a) {
    return $mkfrgba(r, g, b, a);
}

/* rglext.obj */

HOOK_SET(0x4A6970, _Func<void>, $rglOpenPipe);
HOOK_SET(0x4A6A00, _Func<void>, $rglClosePipe);
HOOK_SET(0x4A6A10, _Func<void>, $rglBeginFrame);
HOOK_SET(0x4A6A40, _Func<void>, $rglEndFrame);
HOOK_SET(0x4A6A50, _Func<void>, $rglCameraMatrix);
HOOK_SET(0x4A6A70, _Func<int>, $rglAlive);
HOOK_SET(0x4A6AB0, _Func<void>, $rglDrawBox);
HOOK_SET(0x4A6D30, _Func<void>, $rglDrawSolidBox);
HOOK_SET(0x4A7050, _Func<void>, $rglDrawAxis);
HOOK_SET(0x4A70F0, _Func<void>, $rglDrawSphere$1);
HOOK_SET(0x4A7200, _Func<void>, $rglDrawSphere$2);
HOOK_SET(0x4A7230, _Func<void>, $rglDrawEllipsoid);
HOOK_SET(0x4A7310, _Func<void>, $rglDrawParticle);
HOOK_SET(0x4A7540, _Func<void>, $rglResetDrawTexture);
HOOK_SET(0x4A7560, _Func<void>, $rglDrawTexture);
HOOK_SET(0x4A7750, _Func<void>, $rglPushMatrix);
HOOK_SET(0x4A7790, _Func<void>, $rglPopMatrix);
HOOK_SET(0x4A77D0, _Func<void>, $rglMultMatrix);

void MM2::rglOpenPipe(int p1, int p2, bool p3) {
    $rglOpenPipe(p1, p2, p3);
}
void MM2::rglClosePipe(void) {
    $rglClosePipe();
}
void MM2::rglBeginFrame(uint p1) {
    $rglBeginFrame(p1);
}
void MM2::rglEndFrame(void) {
    $rglEndFrame();
}
void MM2::rglCameraMatrix(const Matrix34 &p1) {
    $rglCameraMatrix(&p1);
}
int MM2::rglAlive(void) {
    return $rglAlive();
}
void MM2::rglDrawBox(const Vector3 &p1, const Matrix34 &p2) {
    $rglDrawBox(&p1, &p2);
}
void MM2::rglDrawSolidBox(const Vector3 &p1, const Matrix34 &p2, const Vector3 &p3) {
    $rglDrawSolidBox(&p1, &p2, &p3);
}
void MM2::rglDrawAxis(float p1, const Matrix34 &p2) {
    $rglDrawAxis(p1, &p2);
}
void MM2::rglDrawSphere(float p1, int p2) {
    $rglDrawSphere$1(p1, p2);
}
void MM2::rglDrawSphere(float p1, const Matrix34 &p2, int p3) {
    $rglDrawSphere$2(p1, &p2, p3);
}
void MM2::rglDrawEllipsoid(const Vector3 &p1, const Matrix34 &p2, int p3) {
    $rglDrawEllipsoid(&p1, &p2, p3);
}
void MM2::rglDrawParticle(const Vector3 &p1, float p2, const Vector4 &p3) {
    $rglDrawParticle(&p1, p2, &p3);
}
void MM2::rglResetDrawTexture(void) {
    $rglResetDrawTexture();
}
void MM2::rglDrawTexture(gfxTexture *texture) {
    $rglDrawTexture(texture);
}
void MM2::rglPushMatrix(void) {
    $rglPushMatrix();
}
void MM2::rglPopMatrix(void) {
    $rglPopMatrix();
}
void MM2::rglMultMatrix(Matrix34 &p1) {
    $rglMultMatrix(&p1);
}