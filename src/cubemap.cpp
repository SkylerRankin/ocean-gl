#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <windows.h>
#include <libloaderapi.h>
#include <iostream>
#include <filesystem>

#include "cubemap.h"

void Cubemap::init() {
    std::vector<std::string> images = {
        "res/cubemap/4.png",
        "res/cubemap/2.png",
        "res/cubemap/1.png",
        "res/cubemap/6.png",
        "res/cubemap/3.png",
        "res/cubemap/5.png"
    };

    WCHAR executableDirectoryBuffer[MAX_PATH];
    GetModuleFileNameW(NULL, executableDirectoryBuffer, MAX_PATH);
    unsigned int endIndex = MAX_PATH - 1;
    while (executableDirectoryBuffer[endIndex] != '\\') {
        executableDirectoryBuffer[endIndex--] = 0;
    }

    std::wstring wideString{ executableDirectoryBuffer, endIndex };
    std::string executableDirectory = std::string(wideString.begin(), wideString.end());

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    program = glCreateProgram();

    vertexShader.compileAndAttach(program, GL_VERTEX_SHADER, "cubemap_vertex.glsl");
    fragmentShader.compileAndAttach(program, GL_FRAGMENT_SHADER, "cubemap_fragment.glsl");
    glLinkProgram(program);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTICES), CUBE_VERTICES, GL_STATIC_DRAW);

    GLuint vertexPositionLocation = glGetAttribLocation(program, "vertexPosition");
    glEnableVertexAttribArray(vertexPositionLocation);
    glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, channels;
    for (int i = 0; i < images.size(); i++) {
        std::filesystem::path path;
        path.append(executableDirectory);
        path.append(images.at(i));

        unsigned char* data = stbi_load(path.generic_string().c_str(), &width, &height, &channels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Failed to load cubemap image at " << path.generic_string().c_str() << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::render() {
    glDepthMask(GL_FALSE);
    glBindVertexArray(vao);
    glUseProgram(program);
    vertexShader.setUniformMat4("view", view);
    vertexShader.setUniformMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}

void Cubemap::setViewMatrix(glm::mat4 view) {
    this->view = glm::mat4(glm::mat3(view));
}

void Cubemap::setProjectionMatrix(glm::mat4 projection) {
    this->projection = projection;
}
