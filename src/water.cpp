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

    updateVBO();

    program = glCreateProgram();
    vertexShader.compileAndAttach(program, GL_VERTEX_SHADER, "water_vertex.glsl");
    fragmentShader.compileAndAttach(program, GL_FRAGMENT_SHADER, "water_fragment.glsl");
    glLinkProgram(program);
    glUseProgram(program);

    setWaveParameters();
    vertexShader.setUniformFloatv("waves", 80, waveParameters);

    GLuint vertexPositionLocation = glGetAttribLocation(program, "vertexPosition");
    glEnableVertexAttribArray(vertexPositionLocation);
    glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Water::render() {
    glBindVertexArray(vao);
    glUseProgram(program);

    vertexShader.setUniformFloat("time", (float)glfwGetTime());
    fragmentShader.setUniformVec3("cameraPosition", engine->camera.position);
    vertexShader.setUniformFloatv("waves", 40, waveParameters);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glDrawArrays(GL_TRIANGLES, 0, totalQuads * VERTICES_PER_QUAD);
}

void Water::setViewMatrix(glm::mat4 view) {
    glUseProgram(program);
    vertexShader.setUniformMat4("view", view);
}

void Water::setProjectionMatrix(glm::mat4 projection) {
    glUseProgram(program);
    vertexShader.setUniformMat4("projection", projection);
}

void Water::updateVBO() {
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

    std::cout << "Vertex buffer size: " << waterPlaneVertices.size() << std::endl;
    glBufferData(GL_ARRAY_BUFFER, waterPlaneVertices.size() * sizeof(float), waterPlaneVertices.data(), GL_STATIC_DRAW);
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
