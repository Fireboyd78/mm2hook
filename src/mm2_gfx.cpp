#include "mm2_gfx.h"

#include "mm2_input.h"
#include "mm2_data.h"
#include "mm2_color.h"

using namespace MM2;

void gfxCallback::setRes(int width, int height, int cdepth, int zdepth, bool detectArgs)
{
    LogFile::WriteLine("Additional graphics params enabled.");

    $setRes(width, height, cdepth, zdepth, true);
}

LRESULT APIENTRY gfxCallback::WndProcNew(HWND    hWnd,
                                         UINT    uMsg,
                                         WPARAM  wParam,
                                         LPARAM  lParam)
{
    switch (uMsg)
    {
        case WM_KEYUP: case WM_SYSKEYUP:
        {
            if (inputCallback::HandleKeyPress(wParam))
                return 0;
        } break;

        case WM_ACTIVATEAPP:
        {
            if (wParam == FALSE && datArgParser::Get("nopause"))
            {
                return 0;
            }
        } break;
    }

    return $gfxWindowProc(hWnd, uMsg, wParam, lParam);
}

void gfxCallback::gfxWindowCreate(LPCSTR lpWindowName)
{
    if (*hWndMain)
    {
        return;
    }

    if (*lpWindowTitle)
    {
        lpWindowName = *lpWindowTitle;
    }

    if (*ATOM_Class == NULL)
    {
        WNDCLASSA wc = { NULL };

        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProcNew;
        wc.cbClsExtra = NULL;
        wc.cbWndExtra = NULL;
        wc.hInstance = NULL;
        wc.hIcon = LoadIconA(GetModuleHandleA(NULL), *IconID ? *IconID : IDI_APPLICATION);
        wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(NULL);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = "gfxWindow";

        *ATOM_Class = RegisterClassA(&wc);
    }

    HDC hDC = GetDC(0);
    DWORD screenWidth = GetDeviceCaps(hDC, HORZRES);
    DWORD screenHeight = GetDeviceCaps(hDC, VERTRES);
    ReleaseDC(0, hDC);

    if (*WndPosX == -1)
    {
        *WndPosX = (screenWidth - WndWidth) / 2;
    }

    if (*WndPosY == -1)
    {
        *WndPosY = (screenHeight - WndHeight) / 2;
    }

    DWORD dwStyle = NULL;

    if (*inWindow)
    {
        if (*hWndParent)
        {
            dwStyle = WS_CHILD;
        }
        else if (*hasBorder = !datArgParser::Get("noborder"))
        {
            dwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        }
        else
        {
            dwStyle = WS_POPUP;
        }
    }
    else
    {
        dwStyle = WS_POPUP | WS_SYSMENU;
    }

    HWND hWND = CreateWindowExA(
        WS_EX_APPWINDOW,
        "gfxWindow",
        lpWindowName,
        dwStyle,
        WndPosX,
        WndPosY,
        640,
        480,
        hWndParent,
        0,
        0,
        0);

    *hWndMain = hWND;

    if (*inWindow)
    {
        RECT rect;

        GetClientRect(hWND, &rect);

        MoveWindow(
            hWND,
            WndPosX,
            WndPosY,
            2 * WndWidth - rect.right,
            2 * WndWidth - rect.bottom,
            0);
    }

    SetCursor(NULL);
    ShowCursor(FALSE);

    ShowWindow(hWND, TRUE);
    UpdateWindow(hWND);
    SetFocus(hWND);
}

BOOL __stdcall gfxCallback::AutoDetectCallback(GUID*    lpGUID,
                                               LPSTR    lpDriverDescription,
                                               LPSTR    lpDriverName,
                                               LPVOID   lpContext)
{
    Displayf("GPU AutoDetect - %s, %s, %s",
             GUIDToString(lpGUID),
             lpDriverDescription,
             lpDriverName);

    if ($DirectDrawCreateEx(lpGUID, (LPVOID*) &lpDD, IID_IDirectDraw7, nullptr) == DD_OK)
    {
        gfxInterface *gfxInterface = &gfxInterfaces[gfxInterfaceCount];

        gfxInterface->DeviceCaps = 1;
        gfxInterface->AcceptableDepths = gfxDepthFlags::Depth32;
        gfxInterface->DeviceType = gfxDeviceType::HardwareWithTnL;
        gfxInterface->ResolutionCount = 0;
        gfxInterface->ResolutionChoice = 0;

        strcpy(gfxInterface->Name, lpDriverDescription);

        DDDEVICEIDENTIFIER2 ddDeviceIdentifier = { NULL };

        if (lpDD->GetDeviceIdentifier(&ddDeviceIdentifier, 0) == DD_OK)
        {
            gfxInterface->VendorID = ddDeviceIdentifier.dwVendorId;
            gfxInterface->DeviceID = ddDeviceIdentifier.dwDeviceId;
            gfxInterface->GUID = ddDeviceIdentifier.guidDeviceIdentifier;
        }

        if (lpDD->QueryInterface(IID_IDirect3D7, (LPVOID*) &lpD3D) == DD_OK)
        {
            lpD3D->EnumDevices($DeviceCallback, gfxInterface);
            lpD3D->Release();

            *lpD3D = nullptr;
        }

        DDSCAPS2 ddsCaps = { NULL };

        ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;

        DWORD availableMemory = 0x40000000; // 1GB = 1024 * 1024 * 1024

        lpDD->GetAvailableVidMem(&ddsCaps, &availableMemory, NULL);

        gfxInterface->AvailableMemory = availableMemory;

        *gfxMaxScreenWidth = 0;
        *gfxMaxScreenHeight = 0;

        lpDD->EnumDisplayModes(0, 0, gfxInterface, $ResCallback);
        lpDD->Release();

        *lpDD = nullptr;

        ++*gfxInterfaceCount;
    }

    return TRUE;
}

DWORD gfxCallback::CalculateShadedColor(int color)
{
    auto timeWeather = &timeWeatherLighting[timeOfDay];

    Vector3 vglKeyColor     = addPitch(&timeWeather->KeyColor, timeWeather->KeyPitch);
    Vector3 vglFill1Color   = addPitch(&timeWeather->Fill1Color, timeWeather->Fill1Pitch);
    Vector3 vglFill2Color   = addPitch(&timeWeather->Fill2Color, timeWeather->Fill2Pitch);

    // convert the ambient to a vector3 for better accuracy
    Vector4 vglAmbient = ColorToVector<8, 8, 8, 8>(timeWeather->Ambient);

    Vector4 vglShadedColor =
    {
        normalize(vglKeyColor.X + vglFill1Color.X + vglFill2Color.X + vglAmbient.X),
        normalize(vglKeyColor.Y + vglFill1Color.Y + vglFill2Color.Y + vglAmbient.Y),
        normalize(vglKeyColor.Z + vglFill1Color.Z + vglFill2Color.Z + vglAmbient.Z),
        1
    };

    return $sdlPage16_GetShadedColor(color, VectorToColor<8, 8, 8, 8>(vglShadedColor));
}

UINT32 vglColor;

void gfxCallback::vglBegin(int gfxMode, int p1)
{
    vglColor = *vglCurrentColor;

    // calculate a nice shaded color ;)
    *vglCurrentColor = CalculateShadedColor(vglColor);

    $vglBegin(gfxMode, p1);
}

void gfxCallback::vglEnd(void)
{
    // restore color
    *vglCurrentColor = vglColor;
    $vglEnd();
}

void gfxCallback::ProgressRect(int x, int y, int width, int height, DWORD color)
{
    DDPIXELFORMAT ddPixelFormat = { sizeof(ddPixelFormat) };
    lpdsRend->GetPixelFormat(&ddPixelFormat);

    // Color is provided in 32 bit, we need to convert it to the correct type
    switch (ddPixelFormat.dwGBitMask)
    {
        case 0x03E0: // 555
        {
            color = ConvertColor<0, 8, 8, 8, 0, 5, 5, 5>(color);
        } break;

        case 0x07E0: // 565
        {
            color = ConvertColor<0, 8, 8, 8, 0, 5, 6, 5>(color);
        } break;

        case 0xFF00: // 888
        {
            // Already in the right format
        } break;

        default: // Unknown
        {
            color = -1;
        } break;
    }

    DDBLTFX ddBltFx = { sizeof(ddBltFx) };
    ddBltFx.dwFillColor = color;

    RECT position =
    {
        x,
        y,
        x + width,
        y + height
    };

    lpdsRend->Blt(&position, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddBltFx);
};