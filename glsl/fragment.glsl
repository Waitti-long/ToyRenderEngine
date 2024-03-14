#version 430
out vec4 color;
in vec4 varyingColor;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main(void) {
    color = varyingColor;
}