#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <random>
#include <cstdlib>

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

    const int sideQuadCount = waterMeshSize / waterMeshResolution;
    totalQuads = sideQuadCount * sideQuadCount;

    glm::vec2 centerOffset = glm::vec2(waterMeshSize) / 2.0f;
    int waterVertexCount = totalQuads * VERTICES_PER_QUAD;
    std::vector<float> waterPlaneVertices(waterVertexCount * 3, 0.0f);
    int bufferIndex = 0;
    for (int x = 0; x < sideQuadCount; x++) {
        for (int z = 0; z < sideQuadCount; z++) {
            for (int i = 0; i < 18; i += 3) {
                waterPlaneVertices.at(bufferIndex++) = (QUAD_VERTEX_POSITIONS[i + 0] + x) * waterMeshResolution - centerOffset.x;
                waterPlaneVertices.at(bufferIndex++) = (QUAD_VERTEX_POSITIONS[i + 1]);
                waterPlaneVertices.at(bufferIndex++) = (QUAD_VERTEX_POSITIONS[i + 2] + z) * waterMeshResolution - centerOffset.y;
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

void Engine::setupWaveParameters() {

    const float maxWavelength = 500.0f;
    const float minWavelength = 5.0f;
    srand(100);

    const float PI = 3.1415926535897932384626433832795;
    float maxHeight = 0;
    float minHeight = 0;

    for (int wave = 0; wave < waveCount; wave++) {
        float p = wave / (float)waveCount;
        float wavelengthP = powf(p, 3);
        float r = rand() / (float)RAND_MAX;

        float xDirection = (r * 2) - 1;
        float steepness = 0.1;
        float wavelength = (maxWavelength - minWavelength) * wavelengthP + minWavelength;

        r = rand() / (float)RAND_MAX;
        float yDirection = 0.5 + ((r * 2) - 1) * 0.5;

        waveParameters[wave * 4 + 0] = xDirection; // X direction
        waveParameters[wave * 4 + 1] = yDirection; // Y direction
        waveParameters[wave * 4 + 2] = steepness; // Steepness
        waveParameters[wave * 4 + 3] = wavelength; // Wavelength

        maxHeight += steepness / (2 * PI / wavelength);
    }

    minHeight = -maxHeight;

    std::cout << "Wave height range: [" << minHeight << ", " << maxHeight << "]" << std::endl;
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

    setupWaveParameters();
    waterVertexShader.setUniformFloatv("waves", 80, waveParameters);

    GLuint vertexPositionLocation = glGetAttribLocation(waterShaderProgram, "vertexPosition");
    glEnableVertexAttribArray(vertexPositionLocation);
    glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glClearColor(0.0f, 0.3f, 0.3f, 0.0f);
    glEnable(GL_DEPTH_TEST);

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
    return waterMeshSize;
}

float Engine::getWaterMeshResolution() {
    return waterMeshResolution;
}

float* Engine::getWaveParameters() {
    return waveParameters;
}

void Engine::updateWaterMeshSize(int size) {
    if (size != waterMeshSize) {
        waterMeshSize = size;
        updateWaterMeshVBO();
    }
}

void Engine::updateWaterMeshResolution(float resolution) {
    if (resolution != waterMeshResolution) {
        waterMeshResolution = resolution;
        updateWaterMeshVBO();
    }
}

void Engine::updateWaterParameter(int index, float value) {
    waveParameters[index] = value;
    hasWaveParameterUpdate = true;
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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    waterVertexShader.setUniformFloat("time", (float) glfwGetTime());
    waterFragmentShader.setUniformVec3("cameraPosition", camera.position);
    waterVertexShader.setUniformFloatv("waves", 40, waveParameters);

    glBindVertexArray(waterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glDrawArrays(GL_TRIANGLES, 0, totalQuads * VERTICES_PER_QUAD);

    UI::renderFrame();
    glfwSwapBuffers(window);
}

void Engine::handleInputs(float elapsedTime) {
    glfwPollEvents();
    camera.frameUpdate(elapsedTime);
    waterVertexShader.setUniformMat4("view", camera.getViewMatrix());
}
