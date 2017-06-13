#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern struct gfxInterface;

    namespace $
    {
        namespace gfxLight
        {
            HOOK_EXPORT(0x4B1C00, _MemberFunc<void>, Reset);
        }
        namespace ltLight {
            HOOK_EXPORT(0x0059ACB0, _MemberFunc<void>, Draw);
            HOOK_EXPORT(0x0059AD90, _MemberFunc<void>, DrawGlow);
            HOOK_EXPORT(0x0059AE30, _Func<void>, DrawGlowBegin);
            HOOK_EXPORT(0x0059AEF0, _Func<void>, DrawGlowEnd);
            HOOK_EXPORT(0x0059B390, _Func<void>, SetUpGfxLightBegin);
            HOOK_EXPORT(0x0059B5B0, _MemberFunc<void>, SetUpGfxLight);
            HOOK_EXPORT(0x0059B5B0, _Func<void>, SetUpGfxLightEnd);
        }
    }

    // what kind of primitive to draw
    // same values as D3DPRIMITIVETYPE
    enum gfxDrawMode {
        DRAWMODE_POINTLIST      = 1,
        DRAWMODE_LINELIST       = 2,
        DRAWMODE_LINESTRIP      = 3,
        DRAWMODE_TRIANGLELIST   = 4,
        DRAWMODE_TRIANGLESTRIP  = 5,
        DRAWMODE_TRIANGLEFAN    = 6,
    };

    /*
        gfxInterface
    */
    enum gfxDeviceType {
        Software        = 0,    // Software (No 3D Video Card)
        Hardware        = 1,    // Hardware (3D Video Card)
        HardwareWithTnL = 2     // Hardware (3D Video Card With T&L)
    };

    enum gfxDepthFlags {
        Depth16 = 0x400,
        Depth24 = 0x200,
        Depth32 = 0x100
    };

    struct gfxResData {
        unsigned short ScreenWidth;
        unsigned short ScreenHeight;
        unsigned short ColorDepth;
        unsigned short Flags; // = ((ColorDepth == 16) + 6)
    };

    struct gfxInterface {
        /*0x00*/ GUID GUID;
        /*0x10*/ char Name[64];

        /*0x50*/ unsigned int DeviceCaps;

        /*0x54*/ gfxDeviceType DeviceType;

        /*0x58*/ unsigned int ResolutionCount;   // Max of 64 resolutions
        /*0x5C*/ unsigned int ResolutionChoice;

        /*0x60*/ gfxDepthFlags AcceptableDepths;  // Used to check if mmResolution::Depth is allowed

        /*0x64*/ unsigned int AvailableMemory;
        /*0x68*/ unsigned int VendorID;
        /*0x6C*/ unsigned int DeviceID;

        /*0x70*/ gfxResData Resolutions[64];
    };

    /* Basically a 1:1 copy of D3DLIGHT */
    struct gfxLight {
    public:
        D3DLIGHTTYPE Type;

        Vector4 Diffuse;
        Vector4 Specular;
        Vector4 Ambient;

        Vector3 Position;
        Vector3 Direction;

        float Range;
        float Falloff;

        float Attenuation0;
        float Attenuation1;
        float Attenuation2;

        float Theta;
        float Phi;

        void Reset(void) {
            $::gfxLight::Reset(this);
        };
    };

    class gfxTexture {
    public:
        /*0x00*/uint VglBindIndex;

        /*0x04*/const char *Name;

        /*0x08*/ushort Width;
        /*0x0A*/ushort Height;

        /*0x0C*/uint TexEnv;
        /*0x10*/uint Color;

        /*0x14*/IDirectDrawSurface7 *DirectDrawSurface;
        /*0x18*/IDirectDrawPalette *DirectDrawPalette;

        /*0x1C*/void *CacheEntry;
        /*0x20*/void *CachePool;

        /*0x24*/uint RefCount;

        /*0x28*/gfxTexture *PrevLOD;
        /*0x2C*/gfxTexture *NextLOD;

        /*0x30*/byte MaxLODCount;
        /*0x31*/byte LODCount;
        
        static ageHook::Type<bool> sm_EnableSetLOD;
        static ageHook::Type<bool> sm_Allow32;
    };

    HOOK_EXPORT(0x004B30F0, _Func<gfxTexture *>, $gfxGetTexture);

    HOOK_EXPORT(0x4ABE00, _Func<bool>, $gfxAutoDetect);
    HOOK_EXPORT(0x4A8CE0, _Func<void>, $gfxPipeline_SetRes);

    HOOK_EXPORT(0x4A88F0, _Func<WNDPROC>, $gfxPipeline_gfxWindowProc);

    HOOK_EXPORT(0x4AC3D0, _Func<LPD3DENUMDEVICESCALLBACK7>, $DeviceCallback);
    HOOK_EXPORT(0x4AC6F0, _Func<LPDDENUMMODESCALLBACK2>, $ResCallback);

    HOOK_EXPORT(0x682FA0, _Type<HWND>, hWndParent);
    HOOK_EXPORT(0x6830B8, _Type<HWND>, hWndMain);

    HOOK_EXPORT(0x68311C, _Type<LPCSTR>, lpWindowTitle);

    HOOK_EXPORT(0x6830F0, _Type<ATOM>, ATOM_Class);
    HOOK_EXPORT(0x683108, _Type<LPCSTR>, IconID);

    HOOK_EXPORT(0x5CA3EC, _Type<bool>, pageFlip);
    HOOK_EXPORT(0x5CA3ED, _Type<bool>, hasBorder);
    HOOK_EXPORT(0x5CA3EE, _Type<bool>, useMultiTexture);
    HOOK_EXPORT(0x5CA664, _Type<bool>, enableHWTnL);

    HOOK_EXPORT(0x68451D, _Type<bool>, novblank);

    HOOK_EXPORT(0x6830D0, _Type<bool>, inWindow);
    HOOK_EXPORT(0x6830D1, _Type<bool>, isMaximized);
    HOOK_EXPORT(0x6830D2, _Type<bool>, tripleBuffer);
    HOOK_EXPORT(0x6830D3, _Type<bool>, useReference);
    HOOK_EXPORT(0x6830D4, _Type<bool>, useSoftware);
    HOOK_EXPORT(0x6830D5, _Type<bool>, useAgeSoftware);
    HOOK_EXPORT(0x6830D6, _Type<bool>, useBlade);
    HOOK_EXPORT(0x6830D7, _Type<bool>, useSysMem);

    HOOK_EXPORT(0x6830D8, _Type<int>, useInterface);

    HOOK_EXPORT(0x684518, _Type<LPDIRECTDRAWCREATEEX>, lpDirectDrawCreateEx);

    HOOK_EXPORT(0x6830A8, _Type<IDirectDraw7 *>, lpDD);
    HOOK_EXPORT(0x6830AC, _Type<IDirect3D7 *>, lpD3D);
    HOOK_EXPORT(0x6830C8, _Type<IDirect3DDevice7 *>, lpD3DDev);
    HOOK_EXPORT(0x6830CC, _Type<IDirectDrawSurface7 *>, lpdsRend);

    HOOK_EXPORT(0x683130, _Type<gfxInterface>, gfxInterfaces);
    HOOK_EXPORT(0x6844C0, _Type<unsigned int>, gfxInterfaceCount);

    HOOK_EXPORT(0x6844C8, _Type<int>, gfxInterfaceChoice);

    HOOK_EXPORT(0x6844B0, _Type<int>, gfxMinScreenWidth);
    HOOK_EXPORT(0x6844CC, _Type<int>, gfxMinScreenHeight);

    HOOK_EXPORT(0x6844FC, _Type<int>, gfxMaxScreenWidth);
    HOOK_EXPORT(0x6844D8, _Type<int>, gfxMaxScreenHeight);

    HOOK_EXPORT(0x6B165C, _Type<int>, gfxTexQuality);
    HOOK_EXPORT(0x6857D0, _Type<int>, gfxTexReduceSize);

    HOOK_EXPORT(0x6830F4, _Type<float>, window_fWidth);
    HOOK_EXPORT(0x683120, _Type<float>, window_fHeight);

    HOOK_EXPORT(0x683128, _Type<int>, window_iWidth);
    HOOK_EXPORT(0x683100, _Type<int>, window_iHeight);

    HOOK_EXPORT(0x6830E4, _Type<int>, window_ZDepth);
    HOOK_EXPORT(0x6830F8, _Type<int>, window_ColorDepth);

    HOOK_EXPORT(0x6830EC, _Type<int>, window_X);
    HOOK_EXPORT(0x683110, _Type<int>, window_Y);
}
