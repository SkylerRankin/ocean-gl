#include <format>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

#include "ui.h"

namespace UI {
    static UIState state = {
        .engine = nullptr,
        .windowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoInputs,
        .waterMeshSizeSlider = {
            .hasPendingUpdate = false,
            .lastValue = 0
        },
        .waterMeshResolutionSlider = {
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
        state.windowFlags = ImGuiWindowFlags_NoFocusOnAppearing;
    }

    void removeFocus() {
        ImGui::SetWindowFocus(NULL);
        state.windowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoInputs;
    }

    void setupFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("ocean-gl", NULL, state.windowFlags);

        if (ImGui::CollapsingHeader("World Info")) {
            ImGui::Text(std::format("Camera position: ({0:.2f}, {1:.2f}, {2:.2f})",
                (*inputs.cameraPosition).x, (*inputs.cameraPosition).y, (*inputs.cameraPosition).z).c_str());

            ImGui::Text(std::format("Camera forward: ({0:.2f}, {1:.2f}, {2:.2f})",
                (*inputs.cameraForward).x, (*inputs.cameraForward).y, (*inputs.cameraForward).z).c_str());

            float frameTime = (*inputs.frameTime);
            float fps = 1.0f / frameTime;
            ImGui::Text(std::format("Frame time average: {0:.3f} ms ({1:.0f} fps)", frameTime, fps).c_str());

            int waterMeshSize = state.engine->getWaterMeshSize();
            int newWaterMeshSize = waterMeshSize;
            ImGui::Text("Water Mesh Size");
            ImGui::SliderInt("Width/Length", &newWaterMeshSize, 4, 2000);
            bool sliderActive = ImGui::IsItemActive();
            if (sliderActive) {
                state.waterMeshSizeSlider.hasPendingUpdate = true;
                state.waterMeshSizeSlider.lastValue = newWaterMeshSize;
            } else if (state.waterMeshSizeSlider.hasPendingUpdate) {
                state.waterMeshSizeSlider.hasPendingUpdate = false;
                state.engine->updateWaterMeshSize(state.waterMeshSizeSlider.lastValue);
            }

            float waterMeshResolution = state.engine->getWaterMeshResolution();
            float newWaterMeshResolution = waterMeshResolution;
            ImGui::Text("Water Mesh Resolution");
            ImGui::SliderFloat("Quad scale", &newWaterMeshResolution, 0.0f, 10.0f);
            sliderActive = ImGui::IsItemActive();
            if (sliderActive) {
                state.waterMeshResolutionSlider.hasPendingUpdate = true;
                state.waterMeshResolutionSlider.lastValue = newWaterMeshResolution;
            } else if (state.waterMeshResolutionSlider.hasPendingUpdate) {
                state.waterMeshResolutionSlider.hasPendingUpdate = false;
                state.engine->updateWaterMeshResolution(state.waterMeshResolutionSlider.lastValue);
            }
        }

        if (ImGui::CollapsingHeader("Gerstner Waves")) {
            ImGui::Text("Wave count: 5");
            float* params = state.engine->getWaveParameters();
            for (int i = 0; i < 5; i++) {
                ImGui::SeparatorText(std::format("Wave {0}", i).c_str());
                ImGui::SliderFloat2(std::format("Direction##{0}", i).c_str(), &params[i * 4], -1, 1);
                ImGui::SliderFloat(std::format("Steepness##{0}", i).c_str(), &params[i * 4 + 2], 0, 1);
                ImGui::SliderFloat(std::format("Wavelength##{0}", i).c_str(), &params[i * 4 + 3], 0, 1000);
            }
        }

        ImGui::End();

        //ImGui::ShowDemoWindow();
    }

    void renderFrame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    UIInputs& getInputs() {
        return inputs;
    }
}
