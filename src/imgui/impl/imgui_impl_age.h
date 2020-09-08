// dear imgui: Renderer + Platform Binding for Angel Game Engine

#pragma once
#include "..\imgui.h"     // IMGUI_IMPL_API

namespace MM2 {
	class gfxViewport;
}

IMGUI_IMPL_API bool     ImGui_ImplAGE_Init(MM2::gfxViewport *viewport);
IMGUI_IMPL_API void     ImGui_ImplAGE_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplAGE_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplAGE_RenderDrawData(ImDrawData* draw_data);
IMGUI_IMPL_API bool     ImGui_ImplAGE_ProcessEvent(void* event);

// Use if you want to reset your rendering device without losing Dear ImGui state.
IMGUI_IMPL_API bool     ImGui_ImplAGE_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplAGE_InvalidateDeviceObjects();
