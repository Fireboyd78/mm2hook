// dear imgui: Renderer + Platform Binding for Angel Game Engine

#include <stdint.h>     // uint64_t
#include <cstring>      // memcpy
#include "..\imgui.h"
#include "imgui_impl_age.h"

// AGE
#include <mm2_rgl.h>
#include <mm2_common.h>
#include <mm2_gfx.h>
#include <mm2_data.h>
using namespace MM2;

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127) // condition expression is constant
#endif

// Vtx struct since D3D7 can't do Vec2 positions
struct ImDrawVertAGE
{
    Vector3 pos;
    ImU32 col;
    Vector2 uv;
};

// Data
static gfxViewport*             g_Viewport = NULL;
static gfxTexture*              g_Texture = NULL;
static double                   g_Time = 0.0;
static char*                    g_ClipboardTextData = NULL;

static void ImGui_ImplAGE_SetupRenderState(ImDrawData* draw_data)
{
    rglEnableDisable(rglToken::RGL_BLEND, true);
    //rglEnableDisable(rglToken::RGL_DEPTH_TEST, false);
    rglEnableDisable(rglToken::RGL_LIGHTING, false);
    rglEnableDisable(rglToken::RGL_CULL_FACE, false);

    float L = draw_data->DisplayPos.x;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

    g_Viewport->Ortho(L, R, B, T, -1.0f, +1.0f);
    gfxPipeline::ForceSetViewport(g_Viewport);
}

// Render function.
void ImGui_ImplAGE_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

    // Backup AGE state that will be modified
    auto prevVP = gfxCurrentViewport.get();

    bool lastAlphaEnable = rglIsEnabled(rglToken::RGL_BLEND);
    //bool lastZEnable = rglIsEnabled(rglToken::RGL_DEPTH_TEST);
    bool lastLighting = rglIsEnabled(rglToken::RGL_LIGHTING);
    bool lastCull = rglIsEnabled(rglToken::RGL_CULL_FACE);

    // Setup desired render state
    ImGui_ImplAGE_SetupRenderState(draw_data);

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];

        //Convert to D3D7 compatible
        static ImVector<ImDrawVertAGE> vertices;
        vertices.resize(cmd_list->VtxBuffer.Size);
        for (int i = 0; i < cmd_list->VtxBuffer.Size; i++)
        {
            const ImDrawVert* src_v = &cmd_list->VtxBuffer[i];
            ImDrawVertAGE* dst_v = &vertices[i];
            
            dst_v->pos.X = src_v->pos.x;
            dst_v->pos.Y = src_v->pos.y;
            dst_v->pos.Z = 0.f;

            dst_v->uv.X = src_v->uv.x;
            dst_v->uv.Y = src_v->uv.y;

            unsigned char* cSrc= (unsigned char*)&src_v->col;
            unsigned char* cDst = (unsigned char*)&dst_v->col;
            cDst[0] = cSrc[2];
            cDst[1] = cSrc[1];
            cDst[2] = cSrc[0];
            cDst[3] = cSrc[3];
        }

        const unsigned short* indices = NULL;
        if (sizeof(ImDrawIdx) == 2)
        {
            indices = (const unsigned short*)cmd_list->IdxBuffer.Data;
        }
        else if (sizeof(ImDrawIdx) == 4)
        {
            static ImVector<unsigned short> indices_converted;
            indices_converted.resize(cmd_list->IdxBuffer.Size);
            for (int i = 0; i < cmd_list->IdxBuffer.Size; ++i)
                indices_converted[i] = (unsigned short)cmd_list->IdxBuffer.Data[i];
            indices = indices_converted.Data;
        }
            
        // Render command lists
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplAGE_SetupRenderState(draw_data);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                vglBindTexture((gfxTexture*)pcmd->TextureId); 

                vglBegin(MM2::gfxDrawMode::DRAWMODE_TRIANGLELIST, 0); //This doesn't render anything. But it does flush the render state for us!
                lpD3DDev->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST, 
                                               D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
                                               (LPVOID)&vertices[pcmd->VtxOffset],
                                               vertices.size(),
                                               (LPWORD)&indices[pcmd->IdxOffset],
                                               pcmd->ElemCount,
                                               0);
              
            }
        }
    }

    rglEnableDisable(rglToken::RGL_BLEND, lastAlphaEnable);
    //rglEnableDisable(rglToken::RGL_DEPTH_TEST, lastZEnable);
    rglEnableDisable(rglToken::RGL_LIGHTING, lastLighting);
    rglEnableDisable(rglToken::RGL_CULL_FACE, lastCull);

    if (gfxCurrentViewport.get() != prevVP)
        gfxPipeline::ForceSetViewport(prevVP);
}

bool ImGui_ImplAGE_CreateDeviceObjects()
{
    // Build texture atlas
    Warningf("ImGui_ImplAGE_CreateDeviceObjects");

    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Create texture
    auto img = gfxImage::Create(width, height, gfxImage::gfxImageFormat::ARGB_8888, gfxImage::gfxImageFormat::NONE, 0);
    memcpy(img->pImageData, pixels, sizeof(int) * width * height);

    // Convert software texture to hardware texture.
    auto tex = gfxTexture::Create(img, true);
    
    // Store our identifier
    io.Fonts->TexID = (void*)tex;
    g_Texture = tex;

    return true;
}

void ImGui_ImplAGE_InvalidateDeviceObjects()
{
    Warningf("ImGui_ImplAGE_InvalidateDeviceObjects");
    /*
    * Will crash on gfxTextureCache::Evict.
    * TODO: Fix I guess?
    if (g_Texture)
    {
        delete g_Texture;
        ImGui::GetIO().Fonts->TexID = NULL;
        g_Texture = NULL;
    }
    */
}

bool ImGui_ImplAGE_Init(gfxViewport *viewport)
{
    // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;       // We can honor GetMouseCursor() values (optional)
    io.BackendPlatformName = io.BackendRendererName = "imgui_impl_AGE";
    
    // Set data
    g_Viewport = viewport;

    //Create device objects
    ImGui_ImplAGE_InvalidateDeviceObjects();
    ImGui_ImplAGE_CreateDeviceObjects();

    return true;
}

void ImGui_ImplAGE_Shutdown()
{
    ImGui_ImplAGE_InvalidateDeviceObjects();

    g_Time = 0.0;
}

// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
bool ImGui_ImplAGE_ProcessEvent(void* ev)
{
    ImGuiIO& io = ImGui::GetIO();
    return false;
}

static void ImGui_ImplAGE_UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return;
}

void ImGui_ImplAGE_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    w = window_iWidth;
    h = window_iHeight;
    io.DisplaySize = ImVec2((float)w, (float)h);

    // Setup time step
    double current_time = datTimeManager::ElapsedTime;
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;

    ImGui_ImplAGE_UpdateMouseCursor();
}
