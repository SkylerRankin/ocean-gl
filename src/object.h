#pragma once
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <string>

#include "glCommon.h"
#include "shader.h"


class Object {
private:
	GLuint vao;
	GLuint vbo;
	GLuint program;
	Shader vertexShader;
	Shader fragmentShader;
	int renderVertices;
	glm::mat4 modelTransform;
public:
	void loadOBJ(const char* name);
	void render(glm::mat4 view, glm::mat4 projection, float time);
};
