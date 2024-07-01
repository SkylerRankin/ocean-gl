#include <format>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "object.h"
#include "loader.h"

static const float PI = 3.1415926535897932384626433832795;

static float angleBetweenVectors(glm::vec2 a, glm::vec2 b) {
	return acos(glm::dot(a, b)) / (glm::length(a) * glm::length(b));
}

static float lag(float current, float target, float speed, float delta) {
	float diff = target - current;
	return current + diff * speed * delta;
}

void Object::loadOBJ(const char* name) {
	OBJ::File objFile;
	OBJ::parseOBJ(std::format("{0}.obj", name), objFile);

	// Format, 8 floats per vertex: vertex xyz, texture uv, normal xyz
	std::vector<float> vertexData;
	for (auto& pair : objFile.objects) {
		for (auto& face : pair.second.faces) {
			int triangles;
			const int indices[2][3] = { {0, 1, 2}, {0, 2, 3} };

			// TODO: divide arbitrary polygon into triangles, probably only support convex shapes
			switch (face.points.size()) {
			case 3:
				triangles = 1;
				break;
			case 4:
				triangles = 2;
				break;
			default:
				std::cerr << "Unsupported number of vertices (" << face.points.size() << ") in face." << std::endl;
				return;
			}

			for (int triangle = 0; triangle < triangles; triangle++) {
				for (int index = 0; index < 3; index++) {
					int i = indices[triangle][index];
					// Vertex position
					vertexData.push_back(objFile.vertices[face.points[i].vertexIndex - 1].x);
					vertexData.push_back(objFile.vertices[face.points[i].vertexIndex - 1].y);
					vertexData.push_back(objFile.vertices[face.points[i].vertexIndex - 1].z);
					// Texture coordinate
					vertexData.push_back(objFile.textureCoordinates[face.points[i].textureIndex - 1].x);
					vertexData.push_back(objFile.textureCoordinates[face.points[i].textureIndex - 1].y);
					// Normal
					vertexData.push_back(objFile.normals[face.points[i].normalIndex - 1].x);
					vertexData.push_back(objFile.normals[face.points[i].normalIndex - 1].y);
					vertexData.push_back(objFile.normals[face.points[i].normalIndex - 1].z);

					renderVertices++;
				}
			}
		}
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	program = glCreateProgram();
	vertexShader.compileAndAttach(program, GL_VERTEX_SHADER, "object_passthrough_vertex.glsl");
	fragmentShader.compileAndAttach(program, GL_FRAGMENT_SHADER, "object_passthrough_fragment.glsl");
	glLinkProgram(program);
	glUseProgram(program);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

	const int stride = 8;

	GLuint vertexPositionLocation = glGetAttribLocation(program, "vertexPosition");
	glEnableVertexAttribArray(vertexPositionLocation);
	glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	GLuint textureCoordinateLocation = glGetAttribLocation(program, "textureCoordinate");
	glEnableVertexAttribArray(textureCoordinateLocation);
	glVertexAttribPointer(textureCoordinateLocation, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));

	GLuint vertexNormalLocation = glGetAttribLocation(program, "vertexNormal");
	glEnableVertexAttribArray(vertexNormalLocation);
	glVertexAttribPointer(vertexNormalLocation, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(5 * sizeof(float)));

	modelTransform = glm::mat4(1);
	modelTransform = glm::scale(modelTransform, glm::vec3(1, 1, 1));
	vertexShader.setUniformMat4("model", modelTransform);
}

void Object::render(glm::mat4 view, glm::mat4 projection, float time, float elapsedTime) {
	glBindVertexArray(vao);
	glUseProgram(program);

	vertexShader.setUniformMat4("view", view);
	vertexShader.setUniformMat4("projection", projection);
	vertexShader.setUniformFloat("time", time);

	glm::vec3 wavePosition;
	glm::vec3 waveNormal;
	water->approximateWaveGeometry(position, time, wavePosition, waveNormal);

	float angleX = angleBetweenVectors(
		glm::vec2(waveNormal.x, waveNormal.y),
		glm::vec2(0, 1)
	) * (waveNormal.x < 0 ? -1 : 1);
	angleX = lag(rotationAngles.x, angleX, rotationLagSpeed, elapsedTime);

	float angleZ = angleBetweenVectors(
		glm::vec2(waveNormal.z, waveNormal.y),
		glm::vec2(0, 1)
	) * (waveNormal.z < 0 ? -1 : 1);
	angleZ = lag(rotationAngles.z, angleZ, rotationLagSpeed, elapsedTime);

	glm::mat4 model = glm::translate(modelTransform, glm::vec3(position.x, wavePosition.y, position.z));
	model = glm::rotate(model, angleX, glm::vec3(1, 0, 0));
	model = glm::rotate(model, angleZ, glm::vec3(0, 0, 1));
	vertexShader.setUniformMat4("model", model);

	rotationAngles.x = angleX;
	rotationAngles.z = angleZ;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glDrawArrays(GL_TRIANGLES, 0, renderVertices);
}
