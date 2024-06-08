#pragma once
#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"

class Engine {
public:
	double elapsedTime;
private:
	GLFWwindow* window;
	Camera camera;
	GLuint vao;
	GLuint vbo;
	GLuint program;
	Shader vertexShader;
	Shader fragmentShader;
	glm::vec2 previousMousePosition = glm::vec2(0);

public:
	void setup(GLFWwindow* window);
	void renderFrame();
	void handleInputs();
	void keyCallback(int key, int scancode, int action, int mods);
	void mousePositionCallback(double x, double y);
	void mouseEnteredCallback(int entered);
};
