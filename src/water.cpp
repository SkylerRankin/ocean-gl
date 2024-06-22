#include <iostream>
#include <vector>
#include <format>

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

void Water::render(float time, bool cameraUnderwater) {
    glBindVertexArray(vao);
    glUseProgram(program);

    vertexShader.setUniformFloat("time", time);
    tessEvalShader.setUniformFloat("time", time);
    fragmentShader.setUniformVec3("cameraPosition", engine->camera.position);
    fragmentShader.setUniformInt("underwaterFlag", static_cast<int>(cameraUnderwater));

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
    }
}

static glm::vec3 accumulateGerstnerWave(glm::vec3 vertexPosition, glm::vec2 direction, float steepness, float wavelength, float time, glm::vec3& tangent, glm::vec3& binormal) {
    direction = normalize(direction);
    const float PI = 3.1415926535897932384626433832795;

    float speed = 3;
    float k = 2 * PI / wavelength;
    float c = sqrt(9.81 / k);
    float f = k * (glm::dot(direction, glm::vec2(vertexPosition.x, vertexPosition.z)) - c * time * speed);
    float a = steepness / k;

    tangent += glm::vec3(
        -direction.x * direction.x * steepness * sin(f),
        direction.x * steepness * cos(f),
        -direction.x * direction.y * steepness * sin(f)
    );

    binormal += glm::vec3(
        -direction.x * direction.y * steepness * sin(f),
        direction.y * steepness * cos(f),
        -direction.y * direction.y * steepness * sin(f)
    );

    return glm::vec3(
        direction.x * (a * cos(f)),
        a * sin(f),
        direction.y * (a * cos(f))
    );
}

static void getSingleWaveGeometry(glm::vec3 location, float time, float* waveParameters, glm::vec3& wavePosition, glm::vec3& waveNormal) {
    glm::vec3 position = glm::vec3(location.x, 0, location.z);
    glm::vec3 tangent = glm::vec3(1.0, 0.0, 0.0);
    glm::vec3 binormal = glm::vec3(0.0, 0.0, 1.0);

    const int numWaves = 20;
    for (int i = 0; i < numWaves * 4; i += 4) {
        glm::vec2 direction = glm::vec2(waveParameters[i], waveParameters[i + 1]);
        float steepness = waveParameters[i + 2];
        float wavelength = waveParameters[i + 3];
        position += accumulateGerstnerWave(location, direction, steepness, wavelength, time, tangent, binormal);
    }

    wavePosition = position;
    waveNormal = normalize(cross(binormal, tangent));
}

/**
    The displacement for each vertex in the water surface mesh is computed in the tesselation eval and vertex shaders, so it is not
    quickly accessible on the CPU. This function uses the same wave function calculation to approximate the wave height at a given
    point. This is an approximation because the wave function modifies all components of the input vector, not just the height, and is
    not an invertable function. The more iterations used, the more accurate this approximation will be.
*/
void Water::approximateWaveGeometry(glm::vec3 desiredPosition, float time, glm::vec3& wavePosition, glm::vec3& waveNormal) {
    const int iterations = 10;

    glm::vec3 measurementLocation = glm::vec3(desiredPosition);
    glm::vec3 measuredPosition;
    glm::vec3 measuredNormal;

    for (int i = 0; i < iterations; i++) {
        getSingleWaveGeometry(measurementLocation, time, waveParameters, measuredPosition, measuredNormal);
        glm::vec3 offset = measuredPosition - desiredPosition;
        measurementLocation = measurementLocation - offset;
    }

    wavePosition = measuredPosition;
    waveNormal = measuredNormal;
}
