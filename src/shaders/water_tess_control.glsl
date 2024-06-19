#version 410 core

layout (vertices=4) out; // 4 control points per patch
uniform vec3 cameraPosition;

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    if (gl_InvocationID == 0) {
        const float minDistance = 10;
        const float maxDistance = 2000;

        float distance00 = clamp(distance(gl_in[0].gl_Position.xyz, cameraPosition), minDistance, maxDistance) / (maxDistance - minDistance);
        float distance01 = clamp(distance(gl_in[1].gl_Position.xyz, cameraPosition), minDistance, maxDistance) / (maxDistance - minDistance);
        float distance10 = clamp(distance(gl_in[2].gl_Position.xyz, cameraPosition), minDistance, maxDistance) / (maxDistance - minDistance);
        float distance11 = clamp(distance(gl_in[3].gl_Position.xyz, cameraPosition), minDistance, maxDistance) / (maxDistance - minDistance);

        const float minTessLevel = 7;
        const float maxTessLevel = 75;

        float tessLevel0 = floor(mix(maxTessLevel, minTessLevel, min(distance00, distance10)) / 2.0) + 1;
        float tessLevel1 = floor(mix(maxTessLevel, minTessLevel, min(distance00, distance01)) / 2.0) + 1;
        float tessLevel2 = floor(mix(maxTessLevel, minTessLevel, min(distance01, distance11)) / 2.0) + 1;
        float tessLevel3 = floor(mix(maxTessLevel, minTessLevel, min(distance10, distance11)) / 2.0) + 1;

        gl_TessLevelOuter[0] = tessLevel0;
        gl_TessLevelOuter[1] = tessLevel1;
        gl_TessLevelOuter[2] = tessLevel2;
        gl_TessLevelOuter[3] = tessLevel3;

        gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
        gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
    }
}
