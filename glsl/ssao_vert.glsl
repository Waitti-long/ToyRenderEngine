#version 430

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec4 varyingVertPos;

void main() {
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
    varyingVertPos = proj_matrix * mv_matrix * vec4(position, 1.0);
}