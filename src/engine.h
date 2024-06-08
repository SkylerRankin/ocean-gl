#pragma once
#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"

class Engine {
private:
	GLFWwindow* window;
	Camera camera;
	GLuint vao;
	GLuint vbo;
	GLuint program;
	Shader vertexShader;
	Shader fragmentShader;
	glm::ivec2 windowSize;
	glm::vec2 previousMousePosition = glm::vec2(0);
	bool cameraFocus = true;
	float frameTimeAverageDecay = 0.9f;
	float frameTimeAverage = 1.0;
	float lastFrameTime;

public:
	void setup(GLFWwindow* window);
	void update();
	void keyCallback(int key, int scancode, int action, int mods);
	void mousePositionCallback(double x, double y);
	void mouseEnteredCallback(int entered);

private:
	void renderFrame();
	void handleInputs(float elapsedTime);
	void windowResizeCallback(int width, int height);
};
