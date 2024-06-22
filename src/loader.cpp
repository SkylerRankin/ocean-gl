#include <filesystem>
#include <fstream>
#include <iostream>

#include "loader.h"

extern std::string executableDirectory;

typedef struct {
    std::string currentObjectName;
    std::string currentMaterialFile;
    std::string currentMaterialName;
} ParsingState;

static ParsingState state;

void parseLine(std::vector<std::string>& lineItems, OBJ::File& objFile) {
    auto& first = lineItems.at(0);
    if (first.compare("v") == 0) {
        // Vertex
        if (lineItems.size() >= 4) {
            glm::vec4 vertex = glm::vec4(
                std::stof(lineItems.at(1)),
                std::stof(lineItems.at(2)),
                std::stof(lineItems.at(3)),
                1.0f
            );

            if (lineItems.size() == 5) {
                vertex.w = std::stof(lineItems.at(4));
            }

            objFile.vertices.push_back(vertex);
        }
    } else if (first.compare("vt") == 0) {
        // Texture coordinate
        if (lineItems.size() >= 3) {
            glm::vec3 textureCoordinate = glm::vec3(
                std::stof(lineItems.at(1)),
                std::stof(lineItems.at(2)),
                0.0f
            );

            if (lineItems.size() == 4) {
                textureCoordinate.z = std::stof(lineItems.at(3));
            }

            objFile.textureCoordinates.push_back(textureCoordinate);
        }
    } else if (first.compare("vn") == 0) {
        // Vertex normal
        if (lineItems.size() == 4) {
            glm::vec3 normal = glm::vec3(
                std::stof(lineItems.at(1)),
                std::stof(lineItems.at(2)),
                std::stof(lineItems.at(3))
            );

            objFile.normals.push_back(normal);
        }
    } else if (first.compare("f") == 0) {
        // Face
        OBJ::Face face;
        face.materialFile = state.currentMaterialFile;
        face.material = state.currentMaterialName;
        for (int i = 0; i < face.points.size(); i++) {
            face.points[i].vertexIndex = -1;
            face.points[i].textureIndex = -1;
            face.points[i].normalIndex = -1;
        }
        
        for (int i = 1; i < lineItems.size(); i++) {
            std::string& item = lineItems.at(i);
            int valueStart = 0;
            int valueIndex = 0;
            while (valueStart < item.length()) {
                int valueEnd = item.find("/", valueStart);
                if (valueEnd == -1) {
                    valueEnd = item.length();
                }

                int index = std::stoi(item.substr(valueStart, valueEnd - valueStart));

                if (valueIndex == 0) {
                    face.points[i - 1].vertexIndex = index;
                } else if (valueIndex == 1) {
                    face.points[i - 1].textureIndex = index;
                } else if (valueIndex == 2) {
                    face.points[i - 1].normalIndex = index;
                }

                valueStart = valueEnd + 1;
                valueIndex++;
            }
        }

        if (objFile.objects.find(state.currentObjectName) == objFile.objects.end()) {
            objFile.objects.emplace(state.currentObjectName, OBJ::Object{});
        }

        objFile.objects.at(state.currentObjectName).faces.push_back(face);
    } else if (first.compare("s") == 0) {
        // Smooth shading
        bool smoothShadingEnabled = lineItems.at(1).compare("off") != 0;
        // TODO handle smooth shading
    } else if (first.compare("o") == 0) {
        // Object name
        state.currentObjectName = lineItems.at(1);
    } else if (first.compare("mtllib") == 0) {
        // External material file
        state.currentMaterialFile = lineItems.at(1);
    } else if (first.compare("usemtl") == 0) {
        // Use material
        state.currentMaterialName = lineItems.at(1);
    } else {
        std::cerr << "Unsupported OBJ file operation \"" << first << "\"." << std::endl;
    }
}

void OBJ::parseOBJ(std::string objFilename, OBJ::File& objFile) {
    std::filesystem::path path;
    path.append(executableDirectory);
    path.append("res");
    path.append("model");
    path.append(objFilename);

    std::ifstream inputStream;
    inputStream.open(path);

    if (!inputStream.is_open()) {
        std::cerr << "Failed to open obj file from " << path << "." << std::endl;
        return;
    }

    objFile.filename = path.string();

    inputStream.seekg(0, inputStream.end);
    std::streampos size = inputStream.tellg();
    inputStream.seekg(0, inputStream.beg);
    std::vector<char> buffer(size);
    inputStream.read(buffer.data(), size);

    state.currentObjectName = "default";

    int lineStart = 0;
    std::vector<std::string> lineItems;
    for (int i = 0; i < size; i++) {
        if (buffer[i] == 0xA) {
            std::string line = std::string(buffer.begin() + lineStart, buffer.begin() + i);
            if (!line.starts_with("#")) {
                lineItems.clear();
                int itemStart = 0;
                while (itemStart < line.length()) {
                    int endIndex = line.find_first_of(" ", itemStart);
                    if (endIndex == -1) {
                        endIndex = line.length();
                    }

                    lineItems.push_back(line.substr(itemStart, endIndex - itemStart));
                    itemStart = endIndex + 1;
                }
                parseLine(lineItems, objFile);
            }
            lineStart = i + 1;
        }
    }
}

void OBJ::printOBJ(OBJ::File& objFile) {
    std::cout << "Obj file: " << objFile.filename << std::endl;
    std::cout << "Vertices (" << objFile.vertices.size() << "):" << std::endl;
    for (auto& vertex : objFile.vertices) {
        std::cout << "\t(" << vertex.x << ", " << vertex.y << ", " << vertex.z << ", " << vertex.w << ")" << std::endl;
    }

    std::cout << "Texture coordinates (" << objFile.textureCoordinates.size() << "):" << std::endl;
    for (auto& textureCoordinate : objFile.textureCoordinates) {
        std::cout << "\t(" << textureCoordinate.x << ", " << textureCoordinate.y << ", " << textureCoordinate.z << ")" << std::endl;
    }

    std::cout << "Normals (" << objFile.normals.size() << "):" << std::endl;
    for (auto& normal : objFile.normals) {
        std::cout << "\t(" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
    }

    std::cout << "Objects (" << objFile.objects.size() << "):" << std::endl;
    for (auto& pair : objFile.objects) {
        std::cout << "\t\"" << pair.first << "\"" << std::endl;
        for (auto& face : pair.second.faces) {
            std::cout << "\tMaterial: " << face.material << " from " << face.materialFile << std::endl;
            std::cout << "\tPoints: ";
            for (auto& point : face.points) {
                std::cout << "(v=" << point.vertexIndex << ", t=" << point.textureIndex << ", n=" << point.normalIndex << ") ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}
