#pragma once
#include "mm2_common.h"
#include "mm2_gfx.h"

namespace MM2
{
    // External declarations
    extern class gfxTexture;

    /* Types currently unknown */
    enum rglToken {
        RGLTOKEN_TYPE1 = 0,
        RGLTOKEN_TYPE2 = 1,
        RGLTOKEN_TYPE3 = 2,
        RGLTOKEN_TYPE4 = 3,
    };

    /* Custom implementation of D3D[T]LVERTEX */
    struct vglVCT1_t {
        Vector3 position;

        DWORD reserved;

        DWORD color;
        DWORD specular;

        Vector2 texCoords;
    };

    /* Custom implementation of D3DVERTEX */
    struct vglVNT1_t {
        Vector3 position;
        Vector3 normals;

        Vector2 texCoords;
    };

    AGE_EXT void vglTexCoord2f(float u, float v);
    AGE_EXT void vglVertex3f(float x, float y, float z);
    AGE_EXT void vglVertex3f(Vector3 position);
    AGE_EXT void vgl_VERTEX_VCT1(float x, float y, float z);
    AGE_EXT void vgl_VERTEX_VNT1(float x, float y, float z);
    AGE_EXT void vglSetFormat(uint p1, uint p2, uint p3, uint p4);
    AGE_EXT void vglBegin(gfxDrawMode drawMode, int p2);
    AGE_EXT void vglEnd(void);
    AGE_EXT void vglBeginBatch(void);
    AGE_EXT void vglEndBatch(void);
    AGE_EXT void vglBindTexture(gfxTexture *texture);
    AGE_EXT void vglDrawLabel(const Vector3 &position, const char *text);

    template <typename ...TArgs>
    AGE_EXT void vglDrawLabelf(const Vector3 &position, const char *format, TArgs ...args);

    AGE_EXT void vglSetCloudMap(const char *texture);
    AGE_EXT void vglSetOffset(float x, float y, float z);
    AGE_EXT bool rglEnableDisable(rglToken token, bool value);
    AGE_EXT bool rglIsEnabled(rglToken token);
    AGE_EXT void rglWorldMatrix(const Matrix34 &mtx);
    AGE_EXT void rglWorldIdentity(void);
    AGE_EXT void rglCard(const Vector3 &position);
    AGE_EXT void rglBindTexture2(const gfxTexture *texture);
    AGE_EXT void tglDrawParticle(const Vector3 &p1, float p2, const Vector4 &p3);
    AGE_EXT void tglDrawRotatedParticle(const Vector3 &p1, float p2, float p3, const Vector4 &p4);

    AGE_EXT uint mkfrgba(float r, float g, float b, float a);

    AGE_EXT void rglOpenPipe(int p1, int p2, bool p3);
    AGE_EXT void rglClosePipe(void);
    AGE_EXT void rglBeginFrame(uint p1);
    AGE_EXT void rglEndFrame(void);
    AGE_EXT void rglCameraMatrix(const Matrix34 &p1);
    AGE_EXT int rglAlive(void);
    AGE_EXT void rglDrawBox(const Vector3 &p1, const Matrix34 &p2);
    AGE_EXT void rglDrawSolidBox(const Vector3 &p1, const Matrix34 &p2, const Vector3 &p3);
    AGE_EXT void rglDrawAxis(float p1, const Matrix34 &p2);
    AGE_EXT void rglDrawSphere(float p1, int p2);
    AGE_EXT void rglDrawSphere(float p1, const Matrix34 &p2, int p3);
    AGE_EXT void rglDrawEllipsoid(const Vector3 &p1, const Matrix34 &p2, int p3);
    AGE_EXT void rglDrawParticle(const Vector3 &p1, float p2, const Vector4 &p3);
    AGE_EXT void rglResetDrawTexture(void);
    AGE_EXT void rglDrawTexture(gfxTexture *texture);
    AGE_EXT void rglPushMatrix(void);
    AGE_EXT void rglPopMatrix(void);
    AGE_EXT void rglMultMatrix(Matrix34 &p1);

    HOOK_EXPORT(0x661928, _Type<uint>,          vglCloudMapEnable);

    HOOK_EXPORT(0x661970, _Type<void(*)(float, float, float)>,
                                                vglVertex);

    HOOK_EXPORT(0x661974, _Type<uint>,          vglCurrentColorTest);

    HOOK_EXPORT(0x661974, _Type<uint>,          vglCurrentColor);
    HOOK_EXPORT(0x682B48, _Type<vglVCT1_t *>,   vglVCT1);
    HOOK_EXPORT(0x682B4C, _Type<float>,         vglCurrentNZ);
    HOOK_EXPORT(0x682B50, _Type<float>,         vglCurrentNX);
    HOOK_EXPORT(0x682B54, _Type<float>,         vglCurrentNY);
    HOOK_EXPORT(0x682B58, _Type<int>,           vglCount);
    HOOK_EXPORT(0x682B5C, _Type<float>,         vglCurrentT);
    HOOK_EXPORT(0x682B60, _Type<float>,         vglCurrentS);
    HOOK_EXPORT(0x682B68, _Type<vglVNT1_t *>,   vglVNT1);
    HOOK_EXPORT(0x682B74, _Type<gfxTexture *>,  vglCloudMapTexture);

    HOOK_EXPORT(0x682F98, _Type<Matrix44 *>,    mtxstack);
}