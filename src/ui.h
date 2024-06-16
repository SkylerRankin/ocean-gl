#pragma once
#include <glm/vec3.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glCommon.h"
#include "engine.h"

typedef struct {
	const glm::vec3* cameraPosition;
	const glm::vec3* cameraForward;
	const float* frameTime;
} UIInputs;

namespace UI {
	typedef struct {
		bool hasPendingUpdate;
		float lastValue;
	} SliderState;

	typedef struct {
		Engine* engine;
		ImGuiWindowFlags windowFlags;

		SliderState waterMeshSizeSlider;
		SliderState waterMeshResolutionSlider;
	} UIState;

	void init(GLFWwindow* window, Engine* engine);
	void enableFocus();
	void removeFocus();
	void setupFrame();
	void renderFrame();
	UIInputs& getInputs();
}
