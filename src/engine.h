#pragma once
#include "glCommon.h"

#include "shader.h"
#include "camera.h"

static const int VERTICES_PER_QUAD = 6;
static const float QUAD_VERTEX_POSITIONS[] = {
	-0.5, 0.0, -0.5,
	-0.5, 0.0, 0.5,
	0.5, 0.0, 0.5,
	-0.5, 0.0, -0.5,
	0.5, 0.0, 0.5,
	0.5, 0.0, -0.5,
};

struct RenderObject {
	GLuint vbo;
};

class Engine {
private:
	GLFWwindow* window;
	Camera camera;

	GLuint currentVAO;
	GLuint currentVBO;

	// GL handles for the water mesh
	GLuint waterVAO;
	GLuint waterVBO;
	Shader waterVertexShader;
	Shader waterFragmentShader;
	GLuint waterShaderProgram;

	// Number of quads in the water mesh
	glm::ivec2 waterMeshSize = glm::ivec2(50, 50);

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
	int getWaterMeshSize();
	void updateWaterMeshSize(int size);

private:
	void renderFrame();
	void handleInputs(float elapsedTime);
	void windowResizeCallback(int width, int height);

	void updateWaterMeshVBO();
};
