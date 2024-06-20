#pragma once
#include <string>
#include <vector>
#include <map>
#include <array>
#include <glm/glm.hpp>

namespace OBJ {
	typedef struct {
		int vertexIndex;
		int textureIndex;
		int normalIndex;
	} Element;

	typedef struct {
		std::array<Element, 4> points;
		std::string materialFile;
		std::string material;
	} Face;

	typedef struct {
		std::vector<Face> faces;
	} Object;

	typedef struct {
		std::string filename;
		std::map<std::string, Object> objects;
		std::vector<glm::vec4> vertices;
		std::vector<glm::vec3> textureCoordinates;
		std::vector<glm::vec3> normals;
	} File;

	void parseOBJ(std::string objFilename, File& objFile);
	void printOBJ(File& objFile);
}
