#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "engine.h"
#include "shader.h"

void Engine::setup(GLFWwindow* window) {
    this->window = window;
    const GLubyte* version = glGetString(GL_VERSION);
    printf("OpenGL version: %s\n", version);

    float vertexData[] = {
        -0.5, -0.5, 0.5,
        -0.5, 0.5, 0.0,
        0.5, 0.5, 0.0,
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    program = glCreateProgram();
    vertexShader.compileAndAttach(program, GL_VERTEX_SHADER, "vertex.glsl");
    fragmentShader.compileAndAttach(program, GL_FRAGMENT_SHADER, "fragment.glsl");
    glLinkProgram(program);
    glUseProgram(program);

    GLuint vertexPositionLocation = glGetAttribLocation(program, "vertexPosition");
    glEnableVertexAttribArray(vertexPositionLocation);
    glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glClearColor(0.0f, 0.3f, 0.3f, 0.0f);

    const float fov = 45.0f;
    const float screenWidth = 640;
    const float screenHeight = 480;
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), screenWidth / screenHeight, 0.1f, 100.0f);
    vertexShader.setUniformMat4("projection", projectionMatrix);
}

void Engine::keyCallback(int key, int scancode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_W:
    case GLFW_KEY_UP:
        // Up
        if (action == GLFW_PRESS) {
            camera.startMove(CAMERA_FORWARD);
        } else if (action == GLFW_RELEASE) {
            camera.endMove(CAMERA_FORWARD);
        }
        break;
    case GLFW_KEY_S:
    case GLFW_KEY_DOWN:
        // Down
        if (action == GLFW_PRESS) {
            camera.startMove(CAMERA_BACKWARD);
        } else if (action == GLFW_RELEASE) {
            camera.endMove(CAMERA_BACKWARD);
        }
        break;
    case GLFW_KEY_A:
    case GLFW_KEY_LEFT:
        // Left
        if (action == GLFW_PRESS) {
            camera.startMove(CAMERA_LEFT);
        } else if (action == GLFW_RELEASE) {
            camera.endMove(CAMERA_LEFT);
        }
        break;
    case GLFW_KEY_D:
    case GLFW_KEY_RIGHT:
        // Right
        if (action == GLFW_PRESS) {
            camera.startMove(CAMERA_RIGHT);
        } else if (action == GLFW_RELEASE) {
            camera.endMove(CAMERA_RIGHT);
        }
        break;
    case GLFW_KEY_LEFT_SHIFT:
        // Left shift
        if (action == GLFW_PRESS) {
            camera.startMove(CAMERA_DOWN);
        } else if (action == GLFW_RELEASE) {
            camera.endMove(CAMERA_DOWN);
        }
        break;
    case GLFW_KEY_SPACE:
        // Space
        if (action == GLFW_PRESS) {
            camera.startMove(CAMERA_UP);
        } else if (action == GLFW_RELEASE) {
            camera.endMove(CAMERA_UP);
        }
        break;
    }

}

void Engine::mousePositionCallback(double x, double y) {
    camera.moveMouse(glm::vec2(x, y));
}

void Engine::mouseEnteredCallback(int entered) {
    camera.mouseExit(~entered);
}

void Engine::renderFrame() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
}

void Engine::handleInputs() {
    glfwPollEvents();
    camera.frameUpdate(elapsedTime);
    vertexShader.setUniformMat4("view", camera.getViewMatrix());
}
