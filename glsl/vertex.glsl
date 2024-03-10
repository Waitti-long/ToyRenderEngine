#version 430
uniform float offset;
uniform float rotate;
void main(void) {
    mat4 rotate_mat = mat4(cos(rotate), sin(rotate), 0.0, 0.0, -sin(rotate), cos(rotate), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    if (gl_VertexID == 0) {
        gl_Position = rotate_mat * vec4(0.25 + offset, -0.25, 0.0, 1.0);
    } else if (gl_VertexID == 1) {
        gl_Position = vec4(-0.25 + offset, -0.25, 0.0, 1.0);
    } else {
        gl_Position = vec4(0.25 + offset, 0.25, 0.0, 1.0);
    }
}