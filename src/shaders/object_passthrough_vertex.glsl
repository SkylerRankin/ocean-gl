#version 410 core

in vec3 vertexPosition;
in vec3 textureCoordinate;
in vec3 vertexNormal;
out vec3 normal;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform float time;

void main() {
	gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    normal = normalize(mat3(transpose(inverse(model))) * vertexNormal);
};
