#version 410 core

in vec3 textureCoordinate;
out vec4 outColor;
uniform samplerCube cubemap;

void main() {
    vec3 textureColor = texture(cubemap, textureCoordinate).xyz;
    vec3 horizonColor = vec3(1);

    // TODO: logistic curve can do this effect without branch
    if (textureCoordinate.y < 0) {
        outColor = vec4(horizonColor, 1);
    } else if (textureCoordinate.y < 0.1) {
        outColor = vec4(mix(horizonColor, textureColor, textureCoordinate.y / 0.1), 1);
    } else {
        outColor = vec4(textureColor, 1);
    }
}
