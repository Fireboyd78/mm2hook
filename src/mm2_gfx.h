#pragma once
#include "mm2_common.h"

namespace MM2
{
    namespace $
    {
        namespace gfxLight {
            declhook(0x4B1C00, _MemberFunc<void>, Reset);
        }
        namespace ltLight {
            declhook(0x59ACB0, _MemberFunc<void>, Draw);
            declhook(0x59AD90, _MemberFunc<void>, DrawGlow);
            declhook(0x59AE30, _Func<void>, DrawGlowBegin);
            declhook(0x59AEF0, _Func<void>, DrawGlowEnd);
            declhook(0x59B390, _Func<void>, SetUpGfxLightBegin);
            declhook(0x59B5B0, _MemberFunc<void>, SetUpGfxLight);
            declhook(0x59B5B0, _Func<void>, SetUpGfxLightEnd);
        }
    }

    class gfxViewport
    {

    };

    class cltLight
    {

    };
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

    struct gfxMaterial {
        Vector4 Diffuse;
        Vector4 Ambient;
        Vector4 Specular;
        Vector4 Emissive;

        float Shininess;

        ColorARGB Color; // 32-bit diffuse color

        AGE_API void Reset(void) {
            ageHook::Thunk<0x4B1BB0>::Call<void>(this);
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

    class gfxImage {
    public:
        void Scale(int a1, int a2)                          { ageHook::Thunk<0x4AEDC0>::Call<void>(this, a1, a2); }
    };

    class gfxPacket {
    public:
        gfxPacket *Next;
        IDirect3DVertexBuffer7 *VertexBuffer;
        int *Positions;
        void *Vertices;
        ushort VertexTypeDesc;
        short StartVertex;
        ushort AdjunctCount;
        ushort TriCount;
        ushort *Indices;
        uint unk_1C;
        uint unk_20;
        byte unk_24;
        uint unk_28;
    };
    
    // yes, this is actually how it is in MM2
    // seems like Angel moved everything to gfxPacket

    class gfxPacketList : public gfxPacket {};

    declhook(0x004B30F0, _Func<gfxTexture *>, $gfxGetTexture);

    declhook(0x4ABE00, _Func<bool>, $gfxAutoDetect);
    declhook(0x4A8CE0, _Func<void>, $gfxPipeline_SetRes);

    declhook(0x4A88F0, _Func<WNDPROC>, $gfxPipeline_gfxWindowProc);

    declhook(0x4AC3D0, _Func<LPD3DENUMDEVICESCALLBACK7>, $DeviceCallback);
    declhook(0x4AC6F0, _Func<LPDDENUMMODESCALLBACK2>, $ResCallback);

    declhook(0x682FA0, _Type<HWND>, hWndParent);
    declhook(0x6830B8, _Type<HWND>, hWndMain);

    declhook(0x68311C, _Type<LPCSTR>, lpWindowTitle);

    declhook(0x6830F0, _Type<ATOM>, ATOM_Class);
    declhook(0x683108, _Type<LPCSTR>, IconID);

    declhook(0x5CA3EC, _Type<bool>, pageFlip);
    declhook(0x5CA3ED, _Type<bool>, hasBorder);
    declhook(0x5CA3EE, _Type<bool>, useMultiTexture);
    declhook(0x5CA664, _Type<bool>, enableHWTnL);

    declhook(0x68451D, _Type<bool>, novblank);

    declhook(0x6830D0, _Type<bool>, inWindow);
    declhook(0x6830D1, _Type<bool>, isMaximized);
    declhook(0x6830D2, _Type<bool>, tripleBuffer);
    declhook(0x6830D3, _Type<bool>, useReference);
    declhook(0x6830D4, _Type<bool>, useSoftware);
    declhook(0x6830D5, _Type<bool>, useAgeSoftware);
    declhook(0x6830D6, _Type<bool>, useBlade);
    declhook(0x6830D7, _Type<bool>, useSysMem);

    declhook(0x6830D8, _Type<int>, useInterface);

    declhook(0x684518, _Type<LPDIRECTDRAWCREATEEX>, lpDirectDrawCreateEx);

    declhook(0x6830A8, _Type<IDirectDraw7 *>, lpDD);
    declhook(0x6830AC, _Type<IDirect3D7 *>, lpD3D);
    declhook(0x6830C8, _Type<IDirect3DDevice7 *>, lpD3DDev);
    declhook(0x6830CC, _Type<IDirectDrawSurface7 *>, lpdsRend);

    declhook(0x683130, _Type<gfxInterface>, gfxInterfaces);
    declhook(0x6844C0, _Type<unsigned int>, gfxInterfaceCount);

    declhook(0x6844C8, _Type<int>, gfxInterfaceChoice);

    declhook(0x6844B0, _Type<int>, gfxMinScreenWidth);
    declhook(0x6844CC, _Type<int>, gfxMinScreenHeight);

    declhook(0x6844FC, _Type<int>, gfxMaxScreenWidth);
    declhook(0x6844D8, _Type<int>, gfxMaxScreenHeight);

    declhook(0x6B165C, _Type<int>, gfxTexQuality);
    declhook(0x6857D0, _Type<int>, gfxTexReduceSize);

    declhook(0x6830F4, _Type<float>, window_fWidth);
    declhook(0x683120, _Type<float>, window_fHeight);

    declhook(0x683128, _Type<int>, window_iWidth);
    declhook(0x683100, _Type<int>, window_iHeight);

    declhook(0x6830E4, _Type<int>, window_ZDepth);
    declhook(0x6830F8, _Type<int>, window_ColorDepth);

    declhook(0x6830EC, _Type<int>, window_X);
    declhook(0x683110, _Type<int>, window_Y);

    template<>
    void luaAddModule<module_gfx>(LuaState L) {

    }
}