#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <random>
#include <cstdlib>

#include "engine.h"
#include "shader.h"
#include "ui.h"
#include "loader.h"


void Engine::setup(GLFWwindow* window) {
    this->window = window;

    cubemap.init();
    water.init(this, cubemap.texture);
    testObject.loadOBJ("cube/cube");

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

    glm::mat4 projection = camera.getProjectionMatrix(windowSize.x / (float)windowSize.y);
    water.setProjectionMatrix(projection);
    cubemap.setProjectionMatrix(projection);
}

void Engine::renderFrame() {
    float currentTime = glfwGetTime();
    float elapsedTime = currentTime - lastFrameTime;

    frameTimeAverage = frameTimeAverageDecay * frameTimeAverage + (1.0f - frameTimeAverageDecay) * elapsedTime;

    handleInputs(elapsedTime);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (windowSize.x != width || windowSize.y != height) {
        windowResizeCallback(width, height);
    }

    glm::vec3 wavePosition;
    glm::vec3 waveNormal;
    water.approximateWaveGeometry(camera.position, currentTime, wavePosition, waveNormal);
    bool cameraUnderwater = wavePosition.y > camera.position.y;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UI::setupFrame();
    cubemap.render(cameraUnderwater);
    water.render(currentTime, cameraUnderwater);
    testObject.render(
        camera.getViewMatrix(),
        camera.getProjectionMatrix(windowSize.x / (float)windowSize.y),
        (float) glfwGetTime()
    );
    UI::renderFrame();
    glfwSwapBuffers(window);
    
    lastFrameTime = currentTime;
}

void Engine::handleInputs(float elapsedTime) {
    glfwPollEvents();
    camera.frameUpdate(elapsedTime);
    water.setViewMatrix(camera.getViewMatrix());
    cubemap.setViewMatrix(camera.getViewMatrix());
}
