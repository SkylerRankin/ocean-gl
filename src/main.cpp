#include <iostream>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include <libloaderapi.h>

#include "glCommon.h"
#include "engine.h"
#include "shader.h"
#include <imgui.h>

static Engine engine;
std::string executableDirectory;

static void errorCallback(int error, const char* description) {
    std::cout << "GLFW error (" << error << "): " << description << std::endl;
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key != GLFW_KEY_ESCAPE && ImGui::GetIO().WantCaptureKeyboard) return;
    engine.keyCallback(key, scancode, action, mods);
}

static void mousePositionCallback(GLFWwindow* window, double x, double y) {
    if (ImGui::GetIO().WantCaptureMouse) return;
    engine.mousePositionCallback(x, y);
}

static void mouseEnteredCallback(GLFWwindow* window, int entered) {
    engine.mouseEnteredCallback(entered);
}

int main(void) {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(1400, 800, "ocean-gl", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(errorCallback);
    glfwSetKeyCallback(window, &keyCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, &mousePositionCallback);
    glfwSetCursorEnterCallback(window, &mouseEnteredCallback);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "OpenGL version: " << version << std::endl;

    WCHAR wideExecutableDirectory[MAX_PATH];
    GetModuleFileNameW(NULL, wideExecutableDirectory, MAX_PATH);
    unsigned int endIndex = MAX_PATH - 1;
    while (wideExecutableDirectory[endIndex] != '\\') {
        wideExecutableDirectory[endIndex--] = 0;
    }

    std::wstring wideString{ wideExecutableDirectory, endIndex };
    executableDirectory = std::string(wideString.begin(), wideString.end());
    std::cout << "Executable directory: " << executableDirectory << std::endl;

    engine.setup(window);

    while (!glfwWindowShouldClose(window)) {
        engine.renderFrame();
    }

    glfwTerminate();
    return 0;
}
