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
	Shader fragmentShader;

	const int waveCount = 20;
	int totalQuads;
	int waterMeshSize = 1000;
	float waterMeshResolution = 0.5f;
	float waveParameters[4 * 20];

public:
	void init(Engine* engine, GLuint skyboxTexture);
	void render();
	void setViewMatrix(glm::mat4 view);
	void setProjectionMatrix(glm::mat4 projection);
private:
	void updateVBO();
	void setWaveParameters();
};