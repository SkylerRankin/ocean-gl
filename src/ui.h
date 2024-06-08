#pragma once
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

typedef struct {
	const glm::vec3* cameraPosition;
	const glm::vec3* cameraForward;
	const float* frameTime;
} UIInputs;

namespace UI {
	typedef struct {
		ImGuiWindowFlags windowFlags;
	} UIState;

	void init(GLFWwindow* window);
	void enableFocus();
	void removeFocus();
	void setupFrame();
	void renderFrame();
	UIInputs& getInputs();
}
