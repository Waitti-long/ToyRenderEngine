#version 430

layout (location = 0) in vec3 position;
out vec4 colour;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 r_mat;

void main(void) {
    gl_Position = proj_matrix * mv_matrix * r_mat * vec4(position, 1.0);
    if (gl_VertexID % 4 == 0) {
        colour = vec4(1.0, 0.0, 0.0, 1.0);
    } else if (gl_VertexID % 4 == 1) {
        colour = vec4(0.0, 1.0, 0.0, 1.0);
    } else if (gl_VertexID % 4 == 2) {
        colour = vec4(0.0, 0.0, 1.0, 1.0);
    } else if (gl_VertexID % 4 == 3) {
        colour = vec4(1.0, 1.0, 0.0, 1.0);
    }
}