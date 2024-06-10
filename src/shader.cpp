#include <iostream>
#include <windows.h>
#include <libloaderapi.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"


std::string Shader::executableDirectory;

Shader::Shader() : id(-1), program(-1) {}

void Shader::setExecutableDirectory() {
    if (Shader::executableDirectory.empty()) {
        WCHAR executableDirectory[MAX_PATH];
        GetModuleFileNameW(NULL, executableDirectory, MAX_PATH);
        unsigned int i = MAX_PATH - 1;
        while (executableDirectory[i] != '\\') {
            executableDirectory[i--] = 0;
        }

        std::wstring wideString{ executableDirectory, i };
        Shader::executableDirectory = std::string(wideString.begin(), wideString.end());
    }
}

void Shader::compileAndAttach(GLuint program, GLenum shaderType, const std::string& filename) {
    setExecutableDirectory();

    this->program = program;
    std::filesystem::path path;
    path.append(Shader::executableDirectory);
    path.append("shaders");
    path.append(filename);

    std::ifstream inputStream;
    inputStream.open(path);

    if (!inputStream.is_open()) {
        std::cerr << "Failed to open shader file from " << path << "." << std::endl;
        return;
    }

    inputStream.seekg(0, inputStream.end);
    std::streampos size = inputStream.tellg();
    inputStream.seekg(0, inputStream.beg);
    std::vector<char> buffer(size);
    inputStream.read(buffer.data(), size);
    const char* shaderSource = buffer.data();

    id = glCreateShader(shaderType);
    glShaderSource(id, 1, &shaderSource, 0);
    inputStream.close();
    glCompileShader(id);

    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint maxLength;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
        std::string errorLog;
        errorLog.resize(maxLength);
        glGetShaderInfoLog(id, maxLength, &maxLength, errorLog.data());
        std::cerr << "Failed to compile shader from file " << path << ". " << errorLog << std::endl;
        return;
    }

    glAttachShader(program, id);
}

void Shader::setUniformFloat(const std::string & name, float value) {
    if (!attributeLocations.contains(name)) {
        attributeLocations.emplace(name, glGetUniformLocation(program, name.c_str()));
    }
    glUniform1f(attributeLocations.at(name), value);
}

void Shader::setUniformVec3(const std::string& name, glm::vec3 value) {
    if (!attributeLocations.contains(name)) {
        attributeLocations.emplace(name, glGetUniformLocation(program, name.c_str()));
    }
    glm::value_ptr(value);
    glUniform3f(attributeLocations.at(name), value.x, value.y, value.z);
}

void Shader::setUniformMat4(const std::string& name, glm::mat4 mat) {
    if (!attributeLocations.contains(name)) {
        attributeLocations.emplace(name, glGetUniformLocation(program, name.c_str()));
    }
    glUniformMatrix4fv(attributeLocations.at(name), 1, GL_FALSE, glm::value_ptr(mat));
}
