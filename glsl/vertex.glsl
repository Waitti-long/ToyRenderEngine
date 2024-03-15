#version 430

layout (location = 0) in vec3 position;
out vec4 varyingColor;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 r_mat;

void main(void) {
    gl_Position = proj_matrix * mv_matrix * r_mat * vec4(position, 1.0);
    varyingColor = vec4(position, 1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}