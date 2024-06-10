#pragma once
#include <string>
#include <glm/mat4x4.hpp>
#include <map>

#include "glCommon.h"

class Shader {
public:
	GLuint id;
private:
	GLuint program;
	static std::string executableDirectory;
	std::map<std::string, GLint> attributeLocations;
	
public:
	Shader();
	void compileAndAttach(GLuint program, GLenum shaderType, const std::string& filename);
	void setUniformFloat(const std::string& name, float value);
	void setUniformVec3(const std::string& name, glm::vec3 value);
	void setUniformMat4(const std::string& name, glm::mat4 mat);
private:
	void setExecutableDirectory();
	void compile(GLenum shaderType, const std::string& filename);
};
