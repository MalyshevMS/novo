#pragma once

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <novo-core/Window.hpp>

class Debugger {
private:
    Novo::Window* _window = nullptr;
public:
    Debugger(Novo::Window& window) {
        _window = &window;
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplGlfw_InitForOpenGL(_window->getHandle(), true);
    }

    void frame_start() {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = _window->getSize().x;
        io.DisplaySize.y = _window->getSize().y;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
    }

    void frame_end() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};