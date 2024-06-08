#include <stdio.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include <libloaderapi.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine.h"
#include "shader.h"

static Engine engine;

static void errorCallback(int error, const char* description) {
    printf("GLFW error (%d): %s\n", error, description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    engine.keyCallback(key, scancode, action, mods);
}

static void mousePositionCallback(GLFWwindow* window, double x, double y) {
    engine.mousePositionCallback(x, y);
}

static void mouseEnteredCallback(GLFWwindow* window, int entered) {
    engine.mouseEnteredCallback(entered);
}

int main(void) {
    if (!glfwInit()) {
        printf("Failed to initialize GLFW.\n");
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(640, 480, "ocean-gl", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(errorCallback);
    glfwSetKeyCallback(window, &keyCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, &mousePositionCallback);
    glfwSetCursorEnterCallback(window, &mouseEnteredCallback);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    engine.setup(window);

    double prevFrameTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        engine.elapsedTime = currentTime - prevFrameTime;
        engine.handleInputs();
        engine.renderFrame();
        prevFrameTime = currentTime;
    }

    glfwTerminate();
    return 0;
}
