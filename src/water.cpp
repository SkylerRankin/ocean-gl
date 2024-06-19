#include <iostream>
#include <vector>

#include "water.h"
#include "engine.h"

void Water::init(Engine* engine, GLuint skyboxTexture) {
    this->engine = engine;
    this->skyboxTexture = skyboxTexture;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // 4 vertices per patch to form quads
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    std::vector<float> vertices;
    const glm::vec2 centerOffset = patchSize * patchTileSize / 2.0f;

    for (int x = 0; x < patchTileSize.x; x++) {
        for (int y = 0; y < patchTileSize.y; y++) {
            vertices.push_back(patchSize.x * x - centerOffset.x);
            vertices.push_back(0);
            vertices.push_back(patchSize.y * y - centerOffset.y);

            vertices.push_back(patchSize.x * (x + 1) - centerOffset.x);
            vertices.push_back(0);
            vertices.push_back(patchSize.y * y - centerOffset.y);

            vertices.push_back(patchSize.x * x - centerOffset.x);
            vertices.push_back(0);
            vertices.push_back(patchSize.y * (y + 1) - centerOffset.y);

            vertices.push_back(patchSize.x * (x + 1) - centerOffset.x);
            vertices.push_back(0);
            vertices.push_back(patchSize.y * (y + 1) - centerOffset.y);
        }
    }

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    program = glCreateProgram();
    vertexShader.compileAndAttach(program, GL_VERTEX_SHADER, "water_vertex.glsl");
    tessControlShader.compileAndAttach(program, GL_TESS_CONTROL_SHADER, "water_tess_control.glsl");
    tessEvalShader.compileAndAttach(program, GL_TESS_EVALUATION_SHADER, "water_tess_eval.glsl");
    fragmentShader.compileAndAttach(program, GL_FRAGMENT_SHADER, "water_fragment.glsl");
    glLinkProgram(program);
    glUseProgram(program);

    setWaveParameters();
    vertexShader.setUniformFloatv("waves", 80, waveParameters);
    tessEvalShader.setUniformFloatv("waves", 80, waveParameters);

    GLuint vertexPositionLocation = glGetAttribLocation(program, "vertexPosition");
    glEnableVertexAttribArray(vertexPositionLocation);
    glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Water::render() {
    glBindVertexArray(vao);
    glUseProgram(program);

    vertexShader.setUniformFloat("time", (float)glfwGetTime());
    tessEvalShader.setUniformFloat("time", (float)glfwGetTime());
    fragmentShader.setUniformVec3("cameraPosition", engine->camera.position);
    vertexShader.setUniformFloatv("waves", 40, waveParameters);

    glBindVertexArray(vao);
    glDrawArrays(GL_PATCHES, 0, 4 * patchTileSize.x * patchTileSize.y);

    // Renders wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDrawArrays(GL_PATCHES, 0, 4 * patchTileSize.x * patchTileSize.y);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Water::setViewMatrix(glm::mat4 view) {
    glUseProgram(program);
    vertexShader.setUniformMat4("view", view);
    tessEvalShader.setUniformMat4("view", view);
}

void Water::setProjectionMatrix(glm::mat4 projection) {
    glUseProgram(program);
    vertexShader.setUniformMat4("projection", projection);
    tessEvalShader.setUniformMat4("projection", projection);
}

void Water::setWaveParameters() {
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
        float wavelength = (maxWavelength - minWavelength) * wavelengthP + minWavelength;
        float steepness = 0.1;
        
        if (wavelength < 10) {
            steepness = 0.025;
        }

        r = rand() / (float)RAND_MAX;
        float yDirection = 0.5 + ((r * 2) - 1) * 0.5;

        waveParameters[wave * 4 + 0] = xDirection;
        waveParameters[wave * 4 + 1] = yDirection;
        waveParameters[wave * 4 + 2] = steepness;
        waveParameters[wave * 4 + 3] = wavelength;

        maxHeight += steepness / (2 * PI / wavelength);
    }

    minHeight = -maxHeight;

    std::cout << "Wave height range: [" << minHeight << ", " << maxHeight << "]" << std::endl;
}
