#pragma once
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <string>

#include "glCommon.h"
#include "shader.h"
#include "water.h"


class Object {
public:
	glm::vec3 position;
	glm::vec3 rotationAngles = glm::vec3(0, 0, 0);

private:
	Water* water;
	GLuint vao;
	GLuint vbo;
	GLuint program;
	Shader vertexShader;
	Shader fragmentShader;
	int renderVertices;
	glm::mat4 modelTransform;

	const float rotationLagSpeed = 1.0f;
public:
	Object(Water* water) : water(water) {};
	void loadOBJ(const char* name);
	void render(glm::mat4 view, glm::mat4 projection, float time, float elapsedTime);
};
