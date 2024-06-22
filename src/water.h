#pragma once
#include "glCommon.h"
#include "shader.h"

static const int VERTICES_PER_QUAD = 6;
static const float QUAD_VERTEX_POSITIONS[] = {
	-0.5, 0.0, -0.5,
	-0.5, 0.0, 0.5,
	0.5, 0.0, 0.5,
	-0.5, 0.0, -0.5,
	0.5, 0.0, 0.5,
	0.5, 0.0, -0.5,
};

class Engine;

class Water {
private:
	Engine* engine;

	GLuint vao;
	GLuint vbo;
	GLuint skyboxTexture;
	GLuint program;

	Shader vertexShader;
	Shader tessControlShader;
	Shader tessEvalShader;
	Shader fragmentShader;

	glm::vec2 patchTileSize = glm::vec2(100, 100);
	glm::vec2 patchSize = glm::vec2(100, 100);

	const int waveCount = 20;
	float waveParameters[4 * 20];

public:
	void init(Engine* engine, GLuint skyboxTexture);
	void render(float time, bool cameraUnderwater);
	void setViewMatrix(glm::mat4 view);
	void setProjectionMatrix(glm::mat4 projection);
	void approximateWaveGeometry(glm::vec3 location, float time, glm::vec3& wavePosition, glm::vec3& waveNormal);
private:
	void setWaveParameters();
};