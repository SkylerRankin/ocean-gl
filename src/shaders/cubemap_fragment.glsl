#version 330 core

in vec3 textureCoordinate;
out vec4 outColor;
uniform samplerCube cubemap;

void main() {
    vec3 textureColor = texture(cubemap, textureCoordinate).xyz;
    vec3 blueColor = vec3(0.078, 0.447, 0.549);

    // TODO: logistic curve can do this effect without branch
    if (textureCoordinate.y < 0) {
        outColor = vec4(blueColor, 1);
    } else if (textureCoordinate.y < 0.1) {
        outColor = vec4(mix(blueColor, textureColor, textureCoordinate.y / 0.1), 1);
    } else {
        outColor = vec4(textureColor, 1);
    }
}
