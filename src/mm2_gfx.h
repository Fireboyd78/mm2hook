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
            HOOK_API AGEHook<0x4B1C00>::MemberFunc<void> Reset;
        }
		namespace ltLight {
			HOOK_API AGEHook<0x0059ACB0>::MemberFunc<void> Draw;
			HOOK_API AGEHook<0x0059AD90>::MemberFunc<void> DrawGlow;
			HOOK_API AGEHook<0x0059AE30>::Func<void> DrawGlowBegin;
			HOOK_API AGEHook<0x0059AEF0>::Func<void> DrawGlowEnd;
			HOOK_API AGEHook<0x0059B390>::Func<void> SetUpGfxLightBegin;
			HOOK_API AGEHook<0x0059B5B0>::MemberFunc<void> SetUpGfxLight;
			HOOK_API AGEHook<0x0059B5B0>::Func<void> SetUpGfxLightEnd;
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
        
        static AGEHook<0x684D34>::Type<bool> sm_EnableSetLOD;
        static AGEHook<0x684D36>::Type<bool> sm_Allow32;
    };

	HOOK_API AGEHook<0x004B30F0>::Func<gfxTexture *> $gfxGetTexture;

    HOOK_API AGEHook<0x4ABE00>::Func<bool> $gfxAutoDetect;
    HOOK_API AGEHook<0x4A8CE0>::Func<void> $gfxPipeline_SetRes;

    HOOK_API AGEHook<0x4A88F0>::Func<WNDPROC> $gfxPipeline_gfxWindowProc;

    HOOK_API AGEHook<0x4AC3D0>::Func<LPD3DENUMDEVICESCALLBACK7> $DeviceCallback;
    HOOK_API AGEHook<0x4AC6F0>::Func<LPDDENUMMODESCALLBACK2> $ResCallback;

    HOOK_API AGEHook<0x682FA0>::Type<HWND> hWndParent;
    HOOK_API AGEHook<0x6830B8>::Type<HWND> hWndMain;

    HOOK_API AGEHook<0x68311C>::Type<LPCSTR> lpWindowTitle;

    HOOK_API AGEHook<0x6830F0>::Type<ATOM> ATOM_Class;
    HOOK_API AGEHook<0x683108>::Type<LPCSTR> IconID;

    HOOK_API AGEHook<0x5CA3EC>::Type<bool> pageFlip;
    HOOK_API AGEHook<0x5CA3ED>::Type<bool> hasBorder;
    HOOK_API AGEHook<0x5CA3EE>::Type<bool> useMultiTexture;
    HOOK_API AGEHook<0x5CA664>::Type<bool> enableHWTnL;

    HOOK_API AGEHook<0x68451D>::Type<bool> novblank;

    HOOK_API AGEHook<0x6830D0>::Type<bool> inWindow;
    HOOK_API AGEHook<0x6830D1>::Type<bool> isMaximized;
    HOOK_API AGEHook<0x6830D2>::Type<bool> tripleBuffer;
    HOOK_API AGEHook<0x6830D3>::Type<bool> useReference;
    HOOK_API AGEHook<0x6830D4>::Type<bool> useSoftware;
    HOOK_API AGEHook<0x6830D5>::Type<bool> useAgeSoftware;
    HOOK_API AGEHook<0x6830D6>::Type<bool> useBlade;
    HOOK_API AGEHook<0x6830D7>::Type<bool> useSysMem;

    HOOK_API AGEHook<0x6830D8>::Type<int> useInterface;

    HOOK_API AGEHook<0x684518>::Type<LPDIRECTDRAWCREATEEX> lpDirectDrawCreateEx;

    HOOK_API AGEHook<0x6830A8>::Type<IDirectDraw7 *> lpDD;
    HOOK_API AGEHook<0x6830AC>::Type<IDirect3D7 *> lpD3D;
    HOOK_API AGEHook<0x6830C8>::Type<IDirect3DDevice7 *> lpD3DDev;
    HOOK_API AGEHook<0x6830CC>::Type<IDirectDrawSurface7 *> lpdsRend;

    HOOK_API AGEHook<0x683130>::Type<gfxInterface> gfxInterfaces;
    HOOK_API AGEHook<0x6844C0>::Type<unsigned int> gfxInterfaceCount;

    HOOK_API AGEHook<0x6844C8>::Type<int> gfxInterfaceChoice;

    HOOK_API AGEHook<0x6844B0>::Type<int> gfxMinScreenWidth;
    HOOK_API AGEHook<0x6844CC>::Type<int> gfxMinScreenHeight;

    HOOK_API AGEHook<0x6844FC>::Type<int> gfxMaxScreenWidth;
    HOOK_API AGEHook<0x6844D8>::Type<int> gfxMaxScreenHeight;

    HOOK_API AGEHook<0x6B165C>::Type<int> gfxTexQuality;
    HOOK_API AGEHook<0x6857D0>::Type<int> gfxTexReduceSize;

    HOOK_API AGEHook<0x6830F4>::Type<float> window_fWidth;
    HOOK_API AGEHook<0x683120>::Type<float> window_fHeight;

    HOOK_API AGEHook<0x683128>::Type<int> window_iWidth;
    HOOK_API AGEHook<0x683100>::Type<int> window_iHeight;

    HOOK_API AGEHook<0x6830E4>::Type<int> window_ZDepth;
    HOOK_API AGEHook<0x6830F8>::Type<int> window_ColorDepth;

    HOOK_API AGEHook<0x6830EC>::Type<int> window_X;
    HOOK_API AGEHook<0x683110>::Type<int> window_Y;
}
