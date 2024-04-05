#version 430

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 color;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

out vec4 varyingVertPos;
out vec3 varyingNormal;
out vec3 varyingColor;

void main() {
    vec4 P = mv_matrix * vec4(position, 1.0);

    gl_Position = proj_matrix * P;

    varyingVertPos = P;
    varyingNormal = normalize((norm_matrix * vec4(normal, 1.0)).xyz);
    varyingColor = color;
}