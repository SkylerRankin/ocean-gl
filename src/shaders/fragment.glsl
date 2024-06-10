#version 330

in vec3 normal;
in vec3 fragmentPosition;
out vec4 outColor;
uniform vec3 cameraPosition;

vec3 lightPosition = vec3(6.0, 10.0, -10.0);
vec3 baseColor = vec3(0.1, 0.5, 0.7);
float ambient = 0.5;
float specularStrength = 0.5;

void main() {
    // Phong lighting model

    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
    vec3 viewDirection = normalize(cameraPosition - fragmentPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float diffuse = max(dot(normal, lightDirection), 0.0);
    vec3 specular = specularStrength * pow(max(dot(viewDirection, reflectDirection), 0.0), 32) * vec3(1.0, 1.0, 1.0);

    vec3 color = (ambient + diffuse + specular) * baseColor;

    outColor = vec4(color, 1.0);
};
