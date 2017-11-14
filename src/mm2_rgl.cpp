#include "mm2_rgl.h"
using namespace MM2;

declfield(MM2::vglCloudMapEnable)   (0x661928);

declfield(MM2::vglVertex)           (0x661970);

declfield(MM2::vglCurrentColor)     (0x661974);
declfield(MM2::vglVCT1)             (0x682B48);
declfield(MM2::vglCurrentNZ)        (0x682B4C);
declfield(MM2::vglCurrentNX)        (0x682B50);
declfield(MM2::vglCurrentNY)        (0x682B54);
declfield(MM2::vglCount)            (0x682B58);
declfield(MM2::vglCurrentT)         (0x682B5C);
declfield(MM2::vglCurrentS)         (0x682B60);
declfield(MM2::vglVNT1)             (0x682B68);
declfield(MM2::vglCloudMapTexture)  (0x682B74);

declfield(MM2::mtxstack)            (0x682F98);

/* psdl.obj */

void    MM2::vglTexCoord2f          (float u, float v)                                          { return _StaticThunk<0x45CFD0>::Call<void>(u, v); }
void    MM2::vglVertex3f            (float x, float y, float z)                                 { return _StaticThunk<0x45CFF0>::Call<void>(x, y, z); }
void    MM2::vglVertex3f            (Vector3 position)                                          { return _StaticThunk<0x45D080>::Call<void>(position); }

/* vgl.obj */

void    MM2::vgl_VERTEX_VCT1        (float x, float y, float z)                                 { return _StaticThunk<0x4A5370>::Call<void>(x, y, z); }
void    MM2::vgl_VERTEX_VNT1        (float x, float y, float z)                                 { return _StaticThunk<0x4A5400>::Call<void>(x, y, z); }
void    MM2::vglSetFormat           (uint p1, uint p2, uint p3, uint p4)                        { return _StaticThunk<0x4A5490>::Call<void>(p1, p2, p3, p4); }
void    MM2::vglBegin               (gfxDrawMode drawMode, int p2)                              { return _StaticThunk<0x4A5500>::Call<void>(drawMode, p2); }
void    MM2::vglEnd                 (void)                                                      { return _StaticThunk<0x4A5A90>::Call<void>(); }
void    MM2::vglBeginBatch          (void)                                                      { return _StaticThunk<0x4A5B10>::Call<void>(); }
void    MM2::vglEndBatch            (void)                                                      { return _StaticThunk<0x4A5B80>::Call<void>(); }
void    MM2::vglBindTexture         (gfxTexture *texture)                                       { return _StaticThunk<0x4A5BA0>::Call<void>(texture); }
void    MM2::vglDrawLabel           (const Vector3 &position, const char *text)                 { return _StaticThunk<0x4A5C50>::Call<void>(&position, text); }

template <typename ...TArgs>
void    MM2::vglDrawLabelf          (const Vector3 &position, const char *format, TArgs ...args)
                                                                                                { return _StaticThunk<0x4A5D00>::Call<void>(&position, format, args...); }

void    MM2::vglSetCloudMap         (const char *texture)                                       { return _StaticThunk<0x4A5D30>::Call<void>(texture); }
void    MM2::vglSetOffset           (float x, float y, float z)                                 { return _StaticThunk<0x4A5DE0>::Call<void>(x, y, z); }
bool    MM2::rglEnableDisable       (rglToken token, bool value)                                { return _StaticThunk<0x4A5F00>::Call<bool>(token, value); }
bool    MM2::rglIsEnabled           (rglToken token)                                            { return _StaticThunk<0x4A6000>::Call<bool>(token); }
void    MM2::rglWorldMatrix         (const Matrix34 &mtx)                                       { return _StaticThunk<0x4A6060>::Call<void>(&mtx); }
void    MM2::rglWorldIdentity       (void)                                                      { return _StaticThunk<0x4A6110>::Call<void>();  }
void    MM2::rglCard                (const Vector3 &position)                                   { return _StaticThunk<0x4A6140>::Call<void>(&position); }
void    MM2::rglBindTexture2        (const gfxTexture *texture)                                 { return _StaticThunk<0x4A6160>::Call<void>(texture); }
void    MM2::tglDrawParticle        (const Vector3 &p1, float p2, const Vector4 &p3)            { return _StaticThunk<0x4A6190>::Call<void>(&p1, p2, &p3); }
void    MM2::tglDrawRotatedParticle (const Vector3 &p1, float p2, float p3, const Vector4 &p4)  { return _StaticThunk<0x4A6550>::Call<void>(&p1, p2, p3, &p4); }
uint    MM2::mkfrgba                (float r, float g, float b, float a)                        { return _StaticThunk<0x4A7880>::Call<uint>(r, g, b, a); }

/* rglext.obj */

void    MM2::rglOpenPipe            (int p1, int p2, bool p3)                                   { return _StaticThunk<0x4A6970>::Call<void>(p1, p2, p3); }
void    MM2::rglClosePipe           (void)                                                      { return _StaticThunk<0x4A6A00>::Call<void>(); }
void    MM2::rglBeginFrame          (uint p1)                                                   { return _StaticThunk<0x4A6A10>::Call<void>(p1); }
void    MM2::rglEndFrame            (void)                                                      { return _StaticThunk<0x4A6A40>::Call<void>(); }
void    MM2::rglCameraMatrix        (const Matrix34 &p1)                                        { return _StaticThunk<0x4A6A50>::Call<void>(&p1); }
int     MM2::rglAlive               (void)                                                      { return _StaticThunk<0x4A6A70>::Call<int>(); }
void    MM2::rglDrawBox             (const Vector3 &p1, const Matrix34 &p2)                     { return _StaticThunk<0x4A6AB0>::Call<void>(&p1, &p2); }
void    MM2::rglDrawSolidBox        (const Vector3 &p1, const Matrix34 &p2, const Vector3 &p3)  { return _StaticThunk<0x4A6D30>::Call<void>(&p1, &p2, &p3); }
void    MM2::rglDrawAxis            (float p1, const Matrix34 &p2)                              { return _StaticThunk<0x4A7050>::Call<void>(p1, &p2); }
void    MM2::rglDrawSphere          (float p1, int p2)                                          { return _StaticThunk<0x4A70F0>::Call<void>(p1, p2); }
void    MM2::rglDrawSphere          (float p1, const Matrix34 &p2, int p3)                      { return _StaticThunk<0x4A7200>::Call<void>(p1, &p2, p3); }
void    MM2::rglDrawEllipsoid       (const Vector3 &p1, const Matrix34 &p2, int p3)             { return _StaticThunk<0x4A7230>::Call<void>(&p1, &p2, p3); }
void    MM2::rglDrawParticle        (const Vector3 &p1, float p2, const Vector4 &p3)            { return _StaticThunk<0x4A7310>::Call<void>(&p1, p2, &p3); }
void    MM2::rglResetDrawTexture    (void)                                                      { return _StaticThunk<0x4A7540>::Call<void>(); }
void    MM2::rglDrawTexture         (gfxTexture *texture)                                       { return _StaticThunk<0x4A7560>::Call<void>(texture); }
void    MM2::rglPushMatrix          (void)                                                      { return _StaticThunk<0x4A7750>::Call<void>(); }
void    MM2::rglPopMatrix           (void)                                                      { return _StaticThunk<0x4A7790>::Call<void>(); }
void    MM2::rglMultMatrix          (Matrix34 &p1)                                              { return _StaticThunk<0x4A77D0>::Call<void>(&p1); }