#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern struct gfxInterface;

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
    protected:
        static AGEHook<0x4B1C00>::Func<void> $Reset;
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
            $Reset(this);
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

    static AGEHook<0x4ABE00>::Func<bool> $gfxAutoDetect;
    static AGEHook<0x4A8CE0>::Func<void> $gfxPipeline_SetRes;

    static AGEHook<0x4A88F0>::Func<WNDPROC> $gfxPipeline_gfxWindowProc;

    static AGEHook<0x4AC3D0>::Func<LPD3DENUMDEVICESCALLBACK7> $DeviceCallback;
    static AGEHook<0x4AC6F0>::Func<LPDDENUMMODESCALLBACK2> $ResCallback;

    static AGEHook<0x682FA0>::Type<HWND> hWndParent;
    static AGEHook<0x6830B8>::Type<HWND> hWndMain;

    static AGEHook<0x68311C>::Type<LPCSTR> lpWindowTitle;

    static AGEHook<0x6830F0>::Type<ATOM> ATOM_Class;
    static AGEHook<0x683108>::Type<LPCSTR> IconID;

    static AGEHook<0x5CA3EC>::Type<bool> pageFlip;
    static AGEHook<0x5CA3ED>::Type<bool> hasBorder;
    static AGEHook<0x5CA3EE>::Type<bool> useMultiTexture;
    static AGEHook<0x5CA664>::Type<bool> enableHWTnL;

    static AGEHook<0x68451D>::Type<bool> novblank;

    static AGEHook<0x6830D0>::Type<bool> inWindow;
    static AGEHook<0x6830D1>::Type<bool> isMaximized;
    static AGEHook<0x6830D2>::Type<bool> tripleBuffer;
    static AGEHook<0x6830D3>::Type<bool> useReference;
    static AGEHook<0x6830D4>::Type<bool> useSoftware;
    static AGEHook<0x6830D5>::Type<bool> useAgeSoftware;
    static AGEHook<0x6830D6>::Type<bool> useBlade;
    static AGEHook<0x6830D7>::Type<bool> useSysMem;

    static AGEHook<0x6830D8>::Type<int> useInterface;

    static AGEHook<0x684518>::Type<LPDIRECTDRAWCREATEEX> lpDirectDrawCreateEx;

    static AGEHook<0x6830A8>::Type<IDirectDraw7 *> lpDD;
    static AGEHook<0x6830AC>::Type<IDirect3D7 *> lpD3D;
    static AGEHook<0x6830C8>::Type<IDirect3DDevice7 *> lpD3DDev;
    static AGEHook<0x6830CC>::Type<IDirectDrawSurface7 *> lpdsRend;

    static AGEHook<0x683130>::Type<gfxInterface> gfxInterfaces;
    static AGEHook<0x6844C0>::Type<unsigned int> gfxInterfaceCount;

    static AGEHook<0x6844C8>::Type<int> gfxInterfaceChoice;

    static AGEHook<0x6844B0>::Type<int> gfxMinScreenWidth;
    static AGEHook<0x6844CC>::Type<int> gfxMinScreenHeight;

    static AGEHook<0x6844FC>::Type<int> gfxMaxScreenWidth;
    static AGEHook<0x6844D8>::Type<int> gfxMaxScreenHeight;

    static AGEHook<0x6B165C>::Type<int> gfxTexQuality;
    static AGEHook<0x6857D0>::Type<int> gfxTexReduceSize;

    static AGEHook<0x6830F4>::Type<float> window_fWidth;
    static AGEHook<0x683120>::Type<float> window_fHeight;

    static AGEHook<0x683128>::Type<int> window_iWidth;
    static AGEHook<0x683100>::Type<int> window_iHeight;

    static AGEHook<0x6830E4>::Type<int> window_ZDepth;
    static AGEHook<0x6830F8>::Type<int> window_ColorDepth;

    static AGEHook<0x6830EC>::Type<int> window_X;
    static AGEHook<0x683110>::Type<int> window_Y;
}
