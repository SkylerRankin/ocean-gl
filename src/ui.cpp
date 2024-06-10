#include <format>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

#include "ui.h"

namespace UI {
    static UIState state = {
        .engine = nullptr,
        .windowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize,
        .waterSizeSliderState = {
            .hasPendingUpdate = false,
            .lastValue = 0
        }
    };

    static UIInputs inputs = {
        .cameraPosition = nullptr
    };

    void init(GLFWwindow* window, Engine* engine) {
        state.engine = engine;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();
        ImGui::SetWindowFocus(NULL);
    }

    void enableFocus() {
        state.windowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysAutoResize;
    }

    void removeFocus() {
        ImGui::SetWindowFocus(NULL);
        state.windowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize;
    }

    void setupFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("ocean-gl", NULL, state.windowFlags);

        ImGui::Text(std::format("camera position: ({0:.2f}, {1:.2f}, {2:.2f})",
            (*inputs.cameraPosition).x, (*inputs.cameraPosition).y, (*inputs.cameraPosition).z).c_str());

        ImGui::Text(std::format("camera forward: ({0:.2f}, {1:.2f}, {2:.2f})",
            (*inputs.cameraForward).x, (*inputs.cameraForward).y, (*inputs.cameraForward).z).c_str());

        float frameTime = (*inputs.frameTime);
        float fps = 1.0f / frameTime;
        ImGui::Text(std::format("Frame time average: {0:.2f} ms ({1:.0f} fps)", frameTime, fps).c_str());

        int waterMeshSize = state.engine->getWaterMeshSize();
        int newWaterMeshSize = waterMeshSize;
        ImGui::Text("Water Mesh Resolution");
        ImGui::SliderInt("# Quads per square side", &newWaterMeshSize, 5, 100);
        bool sliderActive = ImGui::IsItemActive();
        if (sliderActive) {
            state.waterSizeSliderState.hasPendingUpdate = true;
            state.waterSizeSliderState.lastValue = newWaterMeshSize;
        } else if (state.waterSizeSliderState.hasPendingUpdate) {
            state.waterSizeSliderState.hasPendingUpdate = false;
            state.engine->updateWaterMeshSize(state.waterSizeSliderState.lastValue);
        }

        ImGui::End();
    }

    void renderFrame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    UIInputs& getInputs() {
        return inputs;
    }
}
