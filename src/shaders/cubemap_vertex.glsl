#version 410 core

in vec3 vertexPosition;
out vec3 textureCoordinate;
uniform mat4 projection;
uniform mat4 view;

void main() {
    textureCoordinate = vertexPosition;
    gl_Position = projection * view * vec4(vertexPosition * 100, 1.0);
}
