#pragma once

// MM2 uses DirectX 7
#define DIRECT3D_VERSION    0x0700
#define DIRECTINPUT_VERSION 0x0700
#include <d3d.h>
#include <dinput.h>

#include "mm2_utils.h"
#include "mm2_vector.h"
#include "mm2_city.h"

namespace MM2
{
    typedef HRESULT(WINAPI *LPDIRECTDRAWCREATEEX)(GUID *, LPVOID *, REFIID, IUnknown *);

    static MM2RawFnHook<WNDPROC>                            $gfxWindowProc(NULL, NULL, 0x4A88F0);
    static MM2RawFnHook<LPD3DENUMDEVICESCALLBACK7>          $DeviceCallback(NULL, NULL, 0x4AC3D0);
    static MM2RawFnHook<LPDDENUMMODESCALLBACK2>             $ResCallback(NULL, NULL, 0x4AC6F0);

    static MM2RawFnHook<bool(*)(bool)>                      $gfxAutoDetect(NULL, NULL, 0x4ABE00);
    static MM2RawFnHook<void(*)(int, int, int, int, bool)>  $setRes(NULL, NULL, 0x4A8CE0);

    static MM2PtrHook<LPDIRECTDRAWCREATEEX>                 $DirectDrawCreateEx(NULL, NULL, 0x684518);

    static MM2PtrHook<IDirectDraw7 *>                       lpDD(NULL, NULL, 0x6830A8);
    static MM2PtrHook<IDirect3D7 *>                         lpD3D(NULL, NULL, 0x6830AC);
    static MM2PtrHook<IDirectDrawSurface7 *>                lpdsRend(NULL, NULL, 0x6830CC);

    struct gfxInterface; // Forward declare
    static MM2PtrHook<gfxInterface>                         gfxInterfaces(NULL, NULL, 0x683130);
    static MM2PtrHook<uint32_t>                             gfxInterfaceCount(NULL, NULL, 0x6844C0);

    static MM2PtrHook<uint32_t>                             gfxMaxScreenWidth(NULL, NULL, 0x6844FC);
    static MM2PtrHook<uint32_t>                             gfxMaxScreenHeight(NULL, NULL, 0x6844D8);

    static MM2PtrHook<HWND>                                 hWndParent(NULL, NULL, 0x682FA0);
    static MM2PtrHook<HWND>                                 hWndMain(NULL, NULL, 0x6830B8);

    static MM2PtrHook<LPCSTR>                               lpWindowTitle(NULL, NULL, 0x68311C);
    static MM2PtrHook<ATOM>                                 ATOM_Class(NULL, NULL, 0x6830F0);
    static MM2PtrHook<LPCSTR>                               IconID(NULL, NULL, 0x683108);

    static MM2PtrHook<BOOL>                                 inWindow(NULL, NULL, 0x6830D0);
    static MM2PtrHook<BOOL>                                 isMaximized(NULL, NULL, 0x6830D1);
    static MM2PtrHook<BOOL>                                 hasBorder(NULL, NULL, 0x5CA3ED);

    static MM2PtrHook<DWORD>                                WndPosX(NULL, NULL, 0x6830EC);
    static MM2PtrHook<DWORD>                                WndPosY(NULL, NULL, 0x683110);
    static MM2PtrHook<DWORD>                                WndWidth(NULL, NULL, 0x683128);
    static MM2PtrHook<DWORD>                                WndHeight(NULL, NULL, 0x683100);

    static MM2FnHook<void>                                  $vglBegin(NULL, NULL, 0x4A5500);
    static MM2FnHook<void>                                  $vglEnd(NULL, NULL, 0x4A5A90);

    static MM2PtrHook<cityTimeWeatherLighting>              timeWeatherLighting(NULL, NULL, 0x6299A8);

    static MM2PtrHook<int>                                  timeOfDay(NULL, NULL, 0x62B068);

    static MM2PtrHook<UINT32>                               vglCurrentColor(NULL, NULL, 0x661974);

    static MM2FnHook<UINT32>                                $sdlPage16_GetShadedColor(NULL, NULL, 0x450880);

    enum gfxDeviceType
    {
        Software = 0,    // Software (No 3D Video Card)
        Hardware = 1,    // Hardware (3D Video Card)
        HardwareWithTnL = 2     // Hardware (3D Video Card With T&L)
    };

    enum gfxDepthFlags
    {
        Depth16 = 0x400,
        Depth24 = 0x200,
        Depth32 = 0x100
    };

    struct gfxResData
    {
        uint16_t ScreenWidth;
        uint16_t ScreenHeight;
        uint16_t ColorDepth;      // Always 16
        uint16_t Is16BitColor;    // = (ColorDepth == 16) + 6 // Always 7
    };

    struct gfxInterface
    {
        /*0x00*/ GUID GUID;
        /*0x10*/ char Name[64];

        /*0x50*/ uint32_t DeviceCaps;

        /*0x54*/ gfxDeviceType DeviceType;

        /*0x58*/ uint32_t ResolutionCount;   // Max of 64 resolutions
        /*0x5C*/ uint32_t ResolutionChoice;

        /*0x60*/ gfxDepthFlags AcceptableDepths;  // Used to check if mmResolution::Depth is allowed

        /*0x64*/ uint32_t AvailableMemory;
        /*0x68*/ uint32_t VendorID;
        /*0x6C*/ uint32_t DeviceID;

        /*0x70*/ gfxResData Resolutions[64];
    };

    namespace gfxCallback
    {
        void setRes(int width,
                    int height,
                    int cdepth,
                    int zdepth,
                    bool detectArgs);

        void gfxWindowCreate(LPCSTR lpWindowName);

        LRESULT APIENTRY WndProcNew(HWND hWnd,
                                    UINT uMsg,
                                    WPARAM wParam,
                                    LPARAM lParam);

        BOOL __stdcall AutoDetectCallback(GUID*    lpGUID,
                                          LPSTR    lpDriverDescription,
                                          LPSTR    lpDriverName,
                                          LPVOID   lpContext);

        DWORD CalculateShadedColor(int color);

        void vglBegin(int gfxMode, int p1);

        void vglEnd(void);

        void ProgressRect(int x,
                          int y,
                          int width,
                          int height,
                          DWORD color);
    }
}