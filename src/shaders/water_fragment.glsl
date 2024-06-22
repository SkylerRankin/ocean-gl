#version 410 core

in vec3 normal;
in vec3 fragmentPosition;
out vec4 outColor;
uniform int underwaterFlag;
uniform vec3 cameraPosition;
uniform samplerCube cubemap;

vec3 lightPosition = vec3(6.0, 10.0, -10.0);
vec3 baseColor = vec3(0.1, 0.5, 0.7);
float ambient = 0.7;
float specularStrength = 0.3;

float fogDistances[2] = float[2](4000, 1000);
float fogFactorMinimums[2] = float[2](0, 0.6);

void main() {
    vec3 shallowColor = vec3(0.098, 0.890, 0.772);
    vec3 deepColor = vec3(0.078, 0.447, 0.549);
    int minHeight = -37;
    int maxHeight = 37;
    float depth = (clamp(fragmentPosition.y, minHeight, maxHeight) + abs(minHeight)) / (abs(minHeight) + maxHeight);
    float x = depth;
    float t = 0;
    if (x < 0.5) {
        t = 4 * x * x * x;
    } else {
        t = 1 - pow(-2 * x + 2, 3) / 2;
    }
    vec3 color = mix(deepColor, shallowColor, t);

    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
    vec3 viewDirection = normalize(cameraPosition - fragmentPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float diffuse = max(dot(normal, lightDirection), 0.0) * 0.05;
    vec3 specular = specularStrength * pow(max(dot(viewDirection, reflectDirection), 0.0), 32) * vec3(1.0, 1.0, 1.0);
    color = (ambient + diffuse + specular) * color;

    vec3 reflectedViewDirection = reflect(-viewDirection, normalize(normal));
    // TODO: why is the reflection direction y sometimes negative? Normals are all pointing in y+.
    reflectedViewDirection.y = abs(reflectedViewDirection.y);

    vec3 skyReflectionColor = texture(cubemap, reflectedViewDirection).rgb;
    float fresnel = pow(1 - max(0, dot(normal, reflectedViewDirection)), 5);

    color = mix(color, skyReflectionColor, fresnel);

    float maxFogDistance = fogDistances[underwaterFlag];
    float cameraDistance = min(distance(cameraPosition, fragmentPosition), maxFogDistance);
    float fogFactorMinimum = fogFactorMinimums[underwaterFlag];
    float fogFactor = clamp(pow((cameraDistance / maxFogDistance), 3), fogFactorMinimum, 1);
    vec3 fogColors[2] = vec3[2](
        vec3(1),
        vec3(0.078, 0.447, 0.549)
    );
    color = mix(color, fogColors[underwaterFlag], fogFactor);

    outColor = vec4(color, 1.0);
};
