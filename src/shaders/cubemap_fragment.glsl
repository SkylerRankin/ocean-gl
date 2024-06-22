#version 410 core

in vec3 textureCoordinate;
out vec4 outColor;
uniform int underwaterFlag;
uniform samplerCube cubemap;

vec3 horizonColors[2] = vec3[2](
    vec3(1, 1, 1),
    vec3(0.078, 0.447, 0.549)
);

void main() {
    vec3 textureColor = texture(cubemap, textureCoordinate).xyz;
    vec3 horizonColor = horizonColors[underwaterFlag];

    // When underwaterFlag is set, horizon color is used instead of cubemap texture sample.
    textureColor = mix(textureColor, horizonColor, float(underwaterFlag));

    // TODO: logistic curve can do this effect without branch
    if (textureCoordinate.y < 0) {
        outColor = vec4(horizonColor, 1);
    } else if (textureCoordinate.y < 0.1) {
        outColor = vec4(mix(horizonColor, textureColor, textureCoordinate.y / 0.1), 1);
    } else {
        outColor = vec4(textureColor, 1);
    }
}
