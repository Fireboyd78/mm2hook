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

    /*
        Type declarations
    */

    extern ageHook::Type<uint>              vglCloudMapEnable;

    extern ageHook::Type<void(*)(float, float, float)>
                                            vglVertex;

    extern ageHook::Type<uint>              vglCurrentColor;
    extern ageHook::Type<vglVCT1_t *>       vglVCT1;
    extern ageHook::Type<float>             vglCurrentNZ;
    extern ageHook::Type<float>             vglCurrentNX;
    extern ageHook::Type<float>             vglCurrentNY;
    extern ageHook::Type<int>               vglCount;
    extern ageHook::Type<float>             vglCurrentT;
    extern ageHook::Type<float>             vglCurrentS;
    extern ageHook::Type<vglVNT1_t *>       vglVNT1;
    extern ageHook::Type<gfxTexture *>      vglCloudMapTexture;

    extern ageHook::Type<Matrix44 *>        mtxstack;

    /*
        Lua helper
    */
    extern void luaSetColor(float r, float g, float b, float a);
    extern void luaGfxDrawFont(double x, double y, const char* text);

    /*
        Function declarations
    */

    extern void vglTexCoord2f(float u, float v);
    extern void vglVertex3f(float x, float y, float z);
    extern void vglVertex3f(Vector3 position);
    extern void vgl_VERTEX_VCT1(float x, float y, float z);
    extern void vgl_VERTEX_VNT1(float x, float y, float z);
    extern void vglSetFormat(uint p1, uint p2, uint p3, uint p4);
    extern void vglBegin(gfxDrawMode drawMode, int p2);
    extern void vglEnd(void);
    extern void vglBeginBatch(void);
    extern void vglEndBatch(void);
    extern void vglBindTexture(gfxTexture *texture);
    extern void vglDrawLabel(const Vector3 &position, const char *text);

    extern void vglDrawLabelf(const Vector3 &position, const char *format, ...);
    extern void gfxDrawFont(int x, int y, const char* text);

    extern void vglSetCloudMap(const char *texture);
    extern void vglSetOffset(float x, float y, float z);
    extern bool rglEnableDisable(rglToken token, bool value);
    extern bool rglIsEnabled(rglToken token);
    extern void rglWorldMatrix(const Matrix34 &mtx);
    extern void rglWorldIdentity(void);
    extern void rglCard(const Vector3 &position);
    extern void rglBindTexture2(const gfxTexture *texture);
    extern void tglDrawParticle(const Vector3 &p1, float p2, const Vector4 &p3);
    extern void tglDrawRotatedParticle(const Vector3 &p1, float p2, float p3, const Vector4 &p4);

    extern uint mkfrgba(float r, float g, float b, float a);

    extern void rglOpenPipe(int p1, int p2, bool p3);
    extern void rglClosePipe(void);
    extern void rglBeginFrame(uint p1);
    extern void rglEndFrame(void);
    extern void rglCameraMatrix(const Matrix34 &p1);
    extern int rglAlive(void);
    extern void rglDrawBox(const Vector3 &p1, const Matrix34 &p2);
    extern void rglDrawSolidBox(const Vector3 &p1, const Matrix34 &p2, const Vector3 &p3);
    extern void rglDrawAxis(float p1, const Matrix34 &p2);
    extern void rglDrawSphere(float p1, int p2);
    extern void rglDrawSphere(float p1, const Matrix34 &p2, int p3);
    extern void rglDrawEllipsoid(const Vector3 &p1, const Matrix34 &p2, int p3);
    extern void rglDrawParticle(const Vector3 &p1, float p2, const Vector4 &p3);
    extern void rglResetDrawTexture(void);
    extern void rglDrawTexture(gfxTexture *texture);
    extern void rglPushMatrix(void);
    extern void rglPopMatrix(void);
    extern void rglMultMatrix(Matrix34 &p1);

    template<>
    void luaAddModule<module_rgl>(LuaState L) {
        LuaBinding(L)
            .addFunction("gfxDrawFont", &luaGfxDrawFont)

            .addFunction("vglTexCoord2f", &vglTexCoord2f)
            .addFunction("vglVertex3f", static_cast<void(*)(Vector3 vector)>(&vglVertex3f))
            .addFunction("vglBegin", &vglBegin)
            .addFunction("vglEnd", &vglEnd)
            .addFunction("vglBeginBatch", &vglBeginBatch)
            .addFunction("vglEndBatch", &vglEndBatch)
            .addFunction("vglBindTexture", &vglBindTexture)
            .addFunction("vglDrawLabel", &vglDrawLabel)

            .addFunction("mkfrgba", &mkfrgba)

            .addFunction("rglWorldMatrix", &rglWorldMatrix)
            .addFunction("rglWorldIdentity", &rglWorldIdentity)
            .addFunction("rglCameraMatrix", &rglCameraMatrix)

            .addFunction("rglDrawBox", &rglDrawBox)
            .addFunction("rglDrawSolidBox", &rglDrawSolidBox)
            .addFunction("rglDrawAxis", &rglDrawAxis)
            .addFunction("rglDrawSphere", static_cast<void(*)(float p1, int p2)>(&rglDrawSphere))
            .addFunction("rglDrawParticle", &rglDrawParticle)
            .addFunction("rglResetDrawTexture", &rglResetDrawTexture)
            .addFunction("rglDrawTexture", &rglDrawTexture)
            .addFunction("rglPushMatrix", &rglPushMatrix)
            .addFunction("rglPopMatrix", &rglPopMatrix)
            .addFunction("rglMultMatrix", &rglMultMatrix)

            .addFunction("rglSetColor", &luaSetColor);
    }
}