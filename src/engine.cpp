#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "engine.h"
#include "shader.h"
#include "ui.h"

void Engine::updateWaterMeshVBO() {
    // TODO: use EBO to reduce number of vertices?

    bool restoreVAO = false;
    GLuint previousVAO;
    if (currentVAO != waterVAO) {
        restoreVAO = true;
        previousVAO = currentVAO;
        glBindVertexArray(waterVAO);
    }

    bool restoreVBO = false;
    GLuint previousVBO;
    if (currentVBO != waterVBO) {
        restoreVBO = true;
        previousVBO = currentVBO;
        glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    }

    const int totalMeshWidth = 10;
    const float quadScale = totalMeshWidth / (float) waterMeshSize.x;

    glm::vec2 centerOffset = glm::vec2(waterMeshSize) / 2.0f;
    int waterVertexCount = waterMeshSize.x * waterMeshSize.y * sizeof(QUAD_VERTEX_POSITIONS) / sizeof(float);
    std::vector<float> waterPlaneVertices(waterVertexCount, 0.0f);
    int bufferIndex = 0;
    for (int x = 0; x < waterMeshSize.x; x++) {
        for (int z = 0; z < waterMeshSize.y; z++) {
            for (int i = 0; i < 18; i += 3) {
                waterPlaneVertices.at(bufferIndex++) = (QUAD_VERTEX_POSITIONS[i + 0] + x - centerOffset.x) * quadScale;
                waterPlaneVertices.at(bufferIndex++) = (QUAD_VERTEX_POSITIONS[i + 1]) * quadScale;
                waterPlaneVertices.at(bufferIndex++) = (QUAD_VERTEX_POSITIONS[i + 2] + z - centerOffset.y) * quadScale;
            }
        }
    }

    std::cout << "vertex buffer size: " << waterPlaneVertices.size() << std::endl;
    glBufferData(GL_ARRAY_BUFFER, waterPlaneVertices.size() * sizeof(float), waterPlaneVertices.data(), GL_STATIC_DRAW);

    if (restoreVAO) {
        glBindVertexArray(previousVAO);
    }

    if (restoreVBO) {
        glBindBuffer(GL_ARRAY_BUFFER, previousVBO);
    }
}

void Engine::setup(GLFWwindow* window) {
    this->window = window;

    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);
    glBindVertexArray(waterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    currentVAO = waterVAO;
    currentVBO = waterVBO;

    updateWaterMeshVBO();

    waterShaderProgram = glCreateProgram();
    waterVertexShader.compileAndAttach(waterShaderProgram, GL_VERTEX_SHADER, "vertex.glsl");
    waterFragmentShader.compileAndAttach(waterShaderProgram, GL_FRAGMENT_SHADER, "fragment.glsl");
    glLinkProgram(waterShaderProgram);
    glUseProgram(waterShaderProgram);

    GLuint vertexPositionLocation = glGetAttribLocation(waterShaderProgram, "vertexPosition");
    glEnableVertexAttribArray(vertexPositionLocation);
    glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glClearColor(0.0f, 0.3f, 0.3f, 0.0f);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    windowResizeCallback(width, height);

    UI::init(window, this);

    // Set pointers to UI values
    UIInputs& uiInputs = UI::getInputs();
    uiInputs.frameTime = &frameTimeAverage;

    lastFrameTime = glfwGetTime();
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
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) {
            if (cameraFocus) {
                UI::enableFocus();
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else {
                UI::removeFocus();
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            cameraFocus = !cameraFocus;
            camera.setMovementEnabled(cameraFocus);
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

void Engine::windowResizeCallback(int width, int height) {
    windowSize = glm::ivec2(width, height);
    glViewport(0, 0, width, height);
    waterVertexShader.setUniformMat4("projection", camera.getProjectionMatrix(windowSize.x / (float) windowSize.y));
}

int Engine::getWaterMeshSize() {
    return waterMeshSize.x;
}

void Engine::updateWaterMeshSize(int size) {
    if (size != waterMeshSize.x) {
        waterMeshSize.x = size;
        waterMeshSize.y = size;
        updateWaterMeshVBO();
    }
}

void Engine::update() {
    float currentTime = glfwGetTime();
    float elapsedTime = currentTime - lastFrameTime;

    frameTimeAverage = frameTimeAverageDecay * frameTimeAverage + (1.0f - frameTimeAverageDecay) * elapsedTime;

    handleInputs(elapsedTime);
    renderFrame();
    
    lastFrameTime = currentTime;
}

void Engine::renderFrame() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (windowSize.x != width || windowSize.y != height) {
        windowResizeCallback(width, height);
    }

    UI::setupFrame();

    glClear(GL_COLOR_BUFFER_BIT);

    waterVertexShader.setUniformFloat("time", (float) glfwGetTime());
    waterFragmentShader.setUniformVec3("cameraPosition", camera.position);

    glBindVertexArray(waterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glDrawArrays(GL_TRIANGLES, 0, waterMeshSize.x * waterMeshSize.y * VERTICES_PER_QUAD);

    UI::renderFrame();
    glfwSwapBuffers(window);
}

void Engine::handleInputs(float elapsedTime) {
    glfwPollEvents();
    camera.frameUpdate(elapsedTime);
    waterVertexShader.setUniformMat4("view", camera.getViewMatrix());
}
