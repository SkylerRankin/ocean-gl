#version 410 core

layout (quads, fractional_odd_spacing, ccw) in;

out vec3 fragmentPosition;
out vec3 normal;
uniform float time;
uniform mat4 view;
uniform mat4 projection;
uniform float waves[20 * 4];


vec3 accumulateGerstnerWave(vec3 vertexPosition, vec2 direction, float steepness, float wavelength, inout vec3 tangent, inout vec3 binormal) {
	direction = normalize(direction);
    const float PI = 3.1415926535897932384626433832795;

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
    // Control points of patch
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // Calculate position of vertex within patch
    vec4 p0 = (p01 - p00) * gl_TessCoord.x + p00;
    vec4 p1 = (p11 - p10) * gl_TessCoord.x + p10;
    vec4 p = (p1 - p0) * gl_TessCoord.y + p0;

    // Apply wave function to get position and normal
    vec3 position = vec3(p);
	vec3 tangent = vec3(1.0, 0.0, 0.0);
	vec3 binormal = vec3(0.0, 0.0, 1.0);

	const int numWaves = 20;
	for (int i = 0; i < numWaves * 4; i += 4) {
		vec2 direction = vec2(waves[i], waves[i + 1]);
		float steepness = waves[i + 2];
		float wavelength = waves[i + 3];
		position += accumulateGerstnerWave(p.xyz, direction, steepness, wavelength, tangent, binormal);
	}

	gl_Position = projection * view * vec4(position, 1.0);
	normal = normalize(cross(binormal, tangent));
	fragmentPosition = position;
}
