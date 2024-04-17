#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 color;
layout (location = 4) in float metallic;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

out vec2 tc;
out vec3 varyingNormal; // 视觉空间顶点法向量
out vec4 varyingVertPos;
out vec3 varyingColor;
out float varyingMetallic;

// shadow map
uniform mat4 shadow_mvp;
out vec4 shadow_coord;

void main() {
    vec4 P = mv_matrix * vec4(position, 1.0);

    gl_Position = proj_matrix * P;
    varyingVertPos = P;
    tc = texCoord;
    varyingNormal = normalize((norm_matrix * vec4(normal, 1.0)).xyz);
    varyingColor = color;
    varyingMetallic = metallic;

    // shadow map
    shadow_coord = shadow_mvp * vec4(position, 1.0);
}