#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (binding = 0) uniform sampler2D samp;
out vec2 tc;
out vec4 varyingColor;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 r_mat;

void main(void) {
    gl_Position = proj_matrix * mv_matrix * r_mat * vec4(position, 1.0);
    varyingColor = vec4(position, 1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
    tc = texCoord;
}