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
private:
	Water* water;
	GLuint vao;
	GLuint vbo;
	GLuint program;
	Shader vertexShader;
	Shader fragmentShader;
	int renderVertices;
	glm::mat4 modelTransform;
public:
	Object(Water* water) : water(water) {};
	void loadOBJ(const char* name);
	void render(glm::mat4 view, glm::mat4 projection, float time);
};
