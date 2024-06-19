#pragma once
#include "glCommon.h"
#include "shader.h"
#include "camera.h"
#include "cubemap.h"
#include "water.h"


class Engine {
public:
	Camera camera;
private:
	GLFWwindow* window;

	Water water;
	Cubemap cubemap;

	bool hasWaveParameterUpdate = false;

	glm::ivec2 windowSize;
	glm::vec2 previousMousePosition = glm::vec2(0);
	bool cameraFocus = true;
	float frameTimeAverageDecay = 0.9f;
	float frameTimeAverage = 1.0;
	float lastFrameTime;

public:
	void setup(GLFWwindow* window);
	void renderFrame();
	void keyCallback(int key, int scancode, int action, int mods);
	void mousePositionCallback(double x, double y);
	void mouseEnteredCallback(int entered);

private:
	void handleInputs(float elapsedTime);
	void windowResizeCallback(int width, int height);
};
