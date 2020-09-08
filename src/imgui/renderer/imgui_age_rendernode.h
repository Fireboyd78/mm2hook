#pragma once
#include <mm2_common.h>
#include <modules/node.h>
#include <modules/input.h>
#include <mm2_gfx.h>
#include <mm2_lua.h>

#include <imgui/imgui.h>
#include <imgui/impl/imgui_impl_win32.h>
#include <imgui/impl/imgui_impl_age.h>
#include <imgui\impl\imgui_impl_win32.h>

using namespace MM2;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class mmImGuiManager : public asNode {
public:
    static mmImGuiManager* Instance;
private:
    gfxViewport* viewport;
    bool uiShowing = false;
    bool lastUiKeyPressed = false;
private:
    //mmInput::PutEventInQueue called from mmInput::ProcessMouseEvents
    void PutMouseEventInQueue(long long a1)
    {
        auto io = ImGui::GetIO();
        bool isGuiShowing = mmImGuiManager::Instance != nullptr && mmImGuiManager::Instance->IsShowing();

        if (!isGuiShowing || !io.WantCaptureMouse)
            ageHook::Thunk<0x52D5E0>::Call<void>(this, a1);
    }

    //mmInput::PutEventInQueue called from mmInput::ProcessKeyboardEvents
    void PutKeyboardEventInQueue(long long a1)
    {
        auto io = ImGui::GetIO();
        bool isGuiShowing = mmImGuiManager::Instance != nullptr && mmImGuiManager::Instance->IsShowing();

        if (!isGuiShowing || !io.WantCaptureKeyboard)
            ageHook::Thunk<0x52D5E0>::Call<void>(this, a1);
    }

    //ioEventQueue::Queue hook
    static void EventQueueQueue(int a1, int a2, int a3, int a4)
    {
        auto io = ImGui::GetIO();
        bool isGuiShowing = mmImGuiManager::Instance != nullptr && mmImGuiManager::Instance->IsShowing();

        if (isGuiShowing && io.WantCaptureKeyboard && a1 >= 7)
            return;

        if (isGuiShowing && io.WantCaptureMouse && a1 <= 6) 
        {
            ioMouse::ClearStates();
            return;
        }

        ageHook::StaticThunk<0x4BA9D0>::Call<void>(a1, a2, a3, a4);
    }

    //ioInput::Update hook (Currently doesn't respect useJoystick but that always seems to be 0 anyways?)
    static void IOInputHook()
    {
        auto io = ImGui::GetIO();
        bool isGuiShowing = mmImGuiManager::Instance != nullptr && mmImGuiManager::Instance->IsShowing();

        //update mouse
        if (!isGuiShowing || !io.WantCaptureMouse)
            ioMouse::Update();
        else
            ioMouse::ClearStates();

        //update keyboard
        if (!isGuiShowing || !io.WantCaptureKeyboard)
            ioKeyboard::Update();
        else
            ioKeyboard::ClearStates();

        //update pad
        ageHook::StaticThunk<0x4BB7A0>::Call<void>(); //ioPad::UpdateAll
    }
public:
    bool IsShowing() {
        return this->uiShowing;
    }

    void ToggleShowUI() {
        this->uiShowing = !this->uiShowing;
        ImGui::GetIO().MouseDrawCursor = this->uiShowing;
    }

    void Init() {
        // delete instance if we already have one
        if (mmImGuiManager::Instance != nullptr) {
            Warningf("Cleaning up old mmImGuiManager");

            mmImGuiManager::Instance->Shutdown();
            delete mmImGuiManager::Instance;
        }

        // init the AGE part
        viewport = gfxPipeline::CreateViewport();

        // init the imgui part
        Displayf("Initializing ImGui");

        ImGui::CreateContext();
        auto io = ImGui::GetIO();

        // Make the background a little more transparent
        auto style = &ImGui::GetStyle();
        ImVec4* colors = style->Colors;

        colors[ImGuiCol_WindowBg] = ImVec4(15 / 255.f, 15 / 255.f, 15 / 255.f, 200 / 255.f);

        // Initialize helper Platform and Renderer bindings
        ImGui_ImplWin32_Init(hWndMain.get());
        ImGui_ImplAGE_Init(viewport);

        // Steal input from game engine
        //mmInput:: ProcessXEvents hooks hook things like map key, toggle HUD key, etc.
        //ioInput:: hooks mainly the driving axes.  Accelerate, brake, etc.
        //ioEventQueue:: hooks the rest. Ex. mouse clicking used in menus
        InstallCallback("mmInput::ProcessMouseEvents", "Rewire input through the imGuiManager",
            &mmImGuiManager::PutMouseEventInQueue, {
                cbHook<CALL>(0x52CC4A),
            }
        );

        InstallCallback("mmInput::ProcessKeyboardEvents", "Rewire input through the imGuiManager",
            &mmImGuiManager::PutKeyboardEventInQueue, {
                cbHook<CALL>(0x52CC9F),
            }
        );

        InstallCallback("ioInput::Update", "Rewire input through the imGuiManager",
            &mmImGuiManager::IOInputHook, {
                cbHook<CALL>(0x4A916A),
            }
        );

        InstallCallback("ioEventQueue::Queue", "Rewire input through the imGuiManager",
            &mmImGuiManager::EventQueueQueue, {
                cbHook<CALL>(0x4BAA5D),
                cbHook<CALL>(0x4BAD72),
                cbHook<CALL>(0x4BAD93),
                cbHook<CALL>(0x4BADDB),
                cbHook<CALL>(0x4BAE3D),
                cbHook<CALL>(0x4BAE89),
                cbHook<CALL>(0x4BAEDB),
                cbHook<CALL>(0x4BAF4E),
                cbHook<CALL>(0x4BAFA7),
                cbHook<CALL>(0x4BB044),
            }
        );

        mmImGuiManager::Instance = this;
    }

    void Shutdown() {
        Displayf("Shutting down IMGUI");
        ImGui_ImplAGE_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

	virtual void Cull() override {
        // Feed inputs to dear imgui, start new frame
        ImGui_ImplAGE_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Lua
        MM2Lua::OnRenderUi();

        //
        ImGui::Render();
        ImGui_ImplAGE_RenderDrawData(ImGui::GetDrawData());
	}

	virtual void Update() override {
        // Showing toggle
        bool uiKeyPressed = ioKeyboard::GetKeyState(DIK_F10) != 0;
        if (uiKeyPressed && !this->lastUiKeyPressed) {
            ToggleShowUI();
        }
        this->lastUiKeyPressed = uiKeyPressed;

        //Declare Cullable
        if (uiShowing) 
        {
            asCullManager::Instance.get()->DeclareCullable2DFG(this);
        }
	}
};