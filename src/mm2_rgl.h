#pragma once
#include "mm2_common.h"
#include "mm2_gfx.h"

namespace MM2
{
    namespace $
    {
        /* psdl.obj */

        HOOK_API AGEHook<0x45CFD0>::Func<void> vglTexCoord2f;
        HOOK_API AGEHook<0x45CFF0>::Func<void> vglVertex3f$1;
        HOOK_API AGEHook<0x45D080>::Func<void> vglVertex3f$2;

        /* vgl.obj */

        HOOK_API AGEHook<0x4A5370>::Func<void> vgl_VERTEX_VCT1;
        HOOK_API AGEHook<0x4A5400>::Func<void> vgl_VERTEX_VNT1;
        HOOK_API AGEHook<0x4A5490>::Func<void> vglSetFormat;
        HOOK_API AGEHook<0x4A5500>::Func<void> vglBegin;
        HOOK_API AGEHook<0x4A5A90>::Func<void> vglEnd;
        HOOK_API AGEHook<0x4A5B10>::Func<void> vglBeginBatch;
        HOOK_API AGEHook<0x4A5B80>::Func<void> vglEndBatch;
        HOOK_API AGEHook<0x4A5BA0>::Func<void> vglBindTexture;
        HOOK_API AGEHook<0x4A5C50>::Func<void> vglDrawLabel;
        HOOK_API AGEHook<0x4A5D00>::Func<void> vglDrawLabelf;
        HOOK_API AGEHook<0x4A5D30>::Func<void> vglSetCloudMap;
        HOOK_API AGEHook<0x4A5DE0>::Func<void> vglSetOffset;
        HOOK_API AGEHook<0x4A5F00>::Func<bool> rglEnableDisable;
        HOOK_API AGEHook<0x4A6000>::Func<bool> rglIsEnabled;
        HOOK_API AGEHook<0x4A6060>::Func<void> rglWorldMatrix;
        HOOK_API AGEHook<0x4A6110>::Func<void> rglWorldIdentity;
        HOOK_API AGEHook<0x4A6140>::Func<void> rglCard;
        HOOK_API AGEHook<0x4A6160>::Func<void> rglBindTexture2;
        HOOK_API AGEHook<0x4A6190>::Func<void> tglDrawParticle;
        HOOK_API AGEHook<0x4A6550>::Func<void> tglDrawRotatedParticle;
        HOOK_API AGEHook<0x4A7880>::Func<uint> mkfrgba;

        /* rglext.obj */

        HOOK_API AGEHook<0x4A6970>::Func<void> rglOpenPipe;
        HOOK_API AGEHook<0x4A6A00>::Func<void> rglClosePipe;
        HOOK_API AGEHook<0x4A6A10>::Func<void> rglBeginFrame;
        HOOK_API AGEHook<0x4A6A40>::Func<void> rglEndFrame;
        HOOK_API AGEHook<0x4A6A50>::Func<void> rglCameraMatrix;
        HOOK_API AGEHook<0x4A6A70>::Func<int> rglAlive;
        HOOK_API AGEHook<0x4A6AB0>::Func<void> rglDrawBox;
        HOOK_API AGEHook<0x4A6D30>::Func<void> rglDrawSolidBox;
        HOOK_API AGEHook<0x4A7050>::Func<void> rglDrawAxis;
        HOOK_API AGEHook<0x4A70F0>::Func<void> rglDrawSphere$1;
        HOOK_API AGEHook<0x4A7200>::Func<void> rglDrawSphere$2;
        HOOK_API AGEHook<0x4A7230>::Func<void> rglDrawEllipsoid;
        HOOK_API AGEHook<0x4A7310>::Func<void> rglDrawParticle;
        HOOK_API AGEHook<0x4A7540>::Func<void> rglResetDrawTexture;
        HOOK_API AGEHook<0x4A7560>::Func<void> rglDrawTexture;
        HOOK_API AGEHook<0x4A7750>::Func<void> rglPushMatrix;
        HOOK_API AGEHook<0x4A7790>::Func<void> rglPopMatrix;
        HOOK_API AGEHook<0x4A77D0>::Func<void> rglMultMatrix;
    }

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

    HOOK_API AGEHook<0x661928>::Type<uint>          vglCloudMapEnable;

    HOOK_API AGEHook<0x661970>::Type<void (*)(float, float, float)>
                                                    vglVertex;

    HOOK_API AGEHook<0x661974>::Type<uint>          vglCurrentColor;
    HOOK_API AGEHook<0x682B48>::Type<vglVCT1_t *>   vglVCT1;
    HOOK_API AGEHook<0x682B4C>::Type<float>         vglCurrentNZ;
    HOOK_API AGEHook<0x682B50>::Type<float>         vglCurrentNX;
    HOOK_API AGEHook<0x682B54>::Type<float>         vglCurrentNY;
    HOOK_API AGEHook<0x682B58>::Type<int>           vglCount;
    HOOK_API AGEHook<0x682B5C>::Type<float>         vglCurrentT;
    HOOK_API AGEHook<0x682B60>::Type<float>         vglCurrentS;
    HOOK_API AGEHook<0x682B68>::Type<vglVNT1_t *>   vglVNT1;
    HOOK_API AGEHook<0x682B74>::Type<gfxTexture *>  vglCloudMapTexture;

    HOOK_API AGEHook<0x682F98>::Type<Matrix44 *>    mtxstack;

    namespace
    {
        AGE_EXT void vglTexCoord2f(float u, float v) {
            $::vglTexCoord2f(u, v);
        }
        AGE_EXT void vglVertex3f(float x, float y, float z) {
            $::vglVertex3f$1(x, y, z);
        }
        AGE_EXT void vglVertex3f(Vector3 position) {
            $::vglVertex3f$2(position);
        }

        AGE_EXT void vgl_VERTEX_VCT1(float x, float y, float z) {
            $::vgl_VERTEX_VCT1(x, y, z);
        }
        AGE_EXT void vgl_VERTEX_VNT1(float x, float y, float z) {
            $::vgl_VERTEX_VNT1(x, y, z);
        }
        AGE_EXT void vglSetFormat(uint p1, uint p2, uint p3, uint p4) {
            $::vglSetFormat(p1, p2, p3, p4);
        }
        AGE_EXT void vglBegin(gfxDrawMode drawMode, int p2) {
            $::vglBegin(drawMode, p2);
        }
        AGE_EXT void vglEnd(void) {
            $::vglEnd();
        }
        AGE_EXT void vglBeginBatch(void) {
            $::vglBeginBatch();
        }
        AGE_EXT void vglEndBatch(void) {
            $::vglEndBatch();
        }
        AGE_EXT void vglBindTexture(gfxTexture *texture) {
            $::vglBindTexture(texture);
        }
        AGE_EXT void vglDrawLabel(const Vector3 &position, const char *text) {
            $::vglDrawLabel(&position, text);
        }
        template <typename ...TArgs>
        AGE_EXT void vglDrawLabelf(const Vector3 &position, const char *format, TArgs ...args) {
            $::vglDrawLabelf(&position, format, args...);
        }
        AGE_EXT void vglSetCloudMap(const char *texture) {
            $::vglSetCloudMap(texture);
        }
        AGE_EXT void vglSetOffset(float x, float y, float z) {
            $::vglSetOffset(x, y, z);
        }
        AGE_EXT bool rglEnableDisable(rglToken token, bool value) {
            return $::rglEnableDisable(token, value);
        }
        AGE_EXT bool rglIsEnabled(rglToken token) {
            return $::rglIsEnabled(token);
        }
        AGE_EXT void rglWorldMatrix(const Matrix34 &mtx) {
            $::rglWorldMatrix(&mtx);
        }
        AGE_EXT void rglWorldIdentity(void) {
            $::rglWorldIdentity();
        }
        AGE_EXT void rglCard(const Vector3 &position) {
            $::rglCard(&position);
        }
        AGE_EXT void rglBindTexture2(const gfxTexture *texture) {
            $::rglBindTexture2(texture);
        }
        AGE_EXT void tglDrawParticle(const Vector3 &p1, float p2, const Vector4 &p3) {
            $::tglDrawParticle(&p1, p2, &p3);
        }
        AGE_EXT void tglDrawRotatedParticle(const Vector3 &p1, float p2, float p3, const Vector4 &p4) {
            $::tglDrawRotatedParticle(&p1, p2, p3, &p4);
        }
        AGE_EXT uint mkfrgba(float r, float g, float b, float a) {
            return $::mkfrgba(r, g, b, a);
        }

        AGE_EXT void rglOpenPipe(int p1, int p2, bool p3) {
            $::rglOpenPipe(p1, p2, p3);
        }
        AGE_EXT void rglClosePipe(void) {
            $::rglClosePipe();
        }
        AGE_EXT void rglBeginFrame(uint p1) {
            $::rglBeginFrame(p1);
        }
        AGE_EXT void rglEndFrame(void) {
            $::rglEndFrame();
        }
        AGE_EXT void rglCameraMatrix(const Matrix34 &p1) {
            $::rglCameraMatrix(&p1);
        }
        AGE_EXT int rglAlive(void) {
            return $::rglAlive();
        }
        AGE_EXT void rglDrawBox(const Vector3 &p1, const Matrix34 &p2) {
            $::rglDrawBox(&p1, &p2);
        }
        AGE_EXT void rglDrawSolidBox(const Vector3 &p1, const Matrix34 &p2, const Vector3 &p3) {
            $::rglDrawSolidBox(&p1, &p2, &p3);
        }
        AGE_EXT void rglDrawAxis(float p1, const Matrix34 &p2) {
            $::rglDrawAxis(p1, &p2);
        }
        AGE_EXT void rglDrawSphere(float p1, int p2) {
            $::rglDrawSphere$1(p1, p2);
        }
        AGE_EXT void rglDrawSphere(float p1, const Matrix34 &p2, int p3) {
            $::rglDrawSphere$2(p1, &p2, p3);
        }
        AGE_EXT void rglDrawEllipsoid(const Vector3 &p1, const Matrix34 &p2, int p3) {
            $::rglDrawEllipsoid(&p1, &p2, p3);
        }
        AGE_EXT void rglDrawParticle(const Vector3 &p1, float p2, const Vector4 &p3) {
            $::rglDrawParticle(&p1, p2, &p3);
        }
        AGE_EXT void rglResetDrawTexture(void) {
            $::rglResetDrawTexture();
        }
        AGE_EXT void rglDrawTexture(gfxTexture *texture) {
            $::rglDrawTexture(texture);
        }
        AGE_EXT void rglPushMatrix(void) {
            $::rglPushMatrix();
        }
        AGE_EXT void rglPopMatrix(void) {
            $::rglPopMatrix();
        }
        AGE_EXT void rglMultMatrix(Matrix34 &p1) {
            $::rglMultMatrix(&p1);
        }
    }
}