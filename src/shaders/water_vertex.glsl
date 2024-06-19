#version 330

in vec3 vertexPosition;
out vec3 normal;
out vec3 fragmentPosition;
uniform mat4 projection;
uniform mat4 view;
uniform float time;
uniform float waves[20 * 4];

const float PI = 3.1415926535897932384626433832795;

vec3 accumulateGerstnerWave(vec2 direction, float steepness, float wavelength, inout vec3 tangent, inout vec3 binormal) {
	direction = normalize(direction);

	float speed = 3;
	float k = 2 * PI / wavelength;
	float c = sqrt(9.81 / k);
	float f = k * (dot(direction, vertexPosition.xz) - c * time * speed);
	float a = steepness / k;

	tangent += vec3(
		-direction.x * direction.x * steepness * sin(f),
		direction.x * steepness * cos(f),
		-direction.x * direction.y * steepness * sin(f)
	);

	binormal += vec3(
		-direction.x * direction.y * steepness * sin(f),
		direction.y * steepness * cos(f),
		-direction.y * direction.y * steepness * sin(f)
	);

	return vec3(
		direction.x * (a * cos(f)),
		a * sin(f),
		direction.y * (a * cos(f))
	);
}

void main() {
	vec3 position = vec3(vertexPosition);
	vec3 tangent = vec3(1.0, 0.0, 0.0);
	vec3 binormal = vec3(0.0, 0.0, 1.0);

	const float maxWavelength = 50.0;
	const float minWavelength = 1.0;

	const int numWaves = 20;
	for (int i = 0; i < numWaves * 4; i += 4) {
		vec2 direction = vec2(waves[i], waves[i + 1]);
		float steepness = waves[i + 2];
		float wavelength = waves[i + 3];
		position += accumulateGerstnerWave(direction, steepness, wavelength, tangent, binormal);
	}

	gl_Position = projection * view * vec4(position, 1.0);
	normal = normalize(cross(binormal, tangent));

	fragmentPosition = position;
};
