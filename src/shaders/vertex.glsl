#version 330

in vec3 vertexPosition;
out vec3 normal;
out vec3 fragmentPosition;
uniform mat4 projection;
uniform mat4 view;
uniform float time;

const float PI = 3.1415926535897932384626433832795;

void main() {
	float amplitude = 0.5;
	float wavelength = 5;
	float speed = 1;

	float k = 2 * PI / wavelength;
	float f = k * (vertexPosition.x - speed * time);

	float height = amplitude * sin(f);
	vec3 position = vec3(vertexPosition);
	position.x += amplitude * cos(f);
	position.y = height;
	vec3 tangent = normalize(vec3(1.0 - k * amplitude * sin(f), k * amplitude * cos(f), 0.0));

	gl_Position = projection * view * vec4(position, 1.0);
	normal = vec3(-tangent.y, tangent.x, 0);
	fragmentPosition = position;
};
