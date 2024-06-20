#version 410 core

in vec3 normal;
out vec4 outColor;

void main() {
    outColor = vec4(0, 1, 1, 1);
    outColor = vec4((normal + 1) / 2, 1);
};
