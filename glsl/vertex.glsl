#version 430

layout (location = 0) in vec3 position;
out vec4 varyingColor;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 r_mat;

void main(void) {
    float x = gl_InstanceID * 2, y = 0, z = 0;
    mat4 ins_trans_mat = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, x, y, z, 1.0);
    gl_Position = proj_matrix * mv_matrix * r_mat * ins_trans_mat * vec4(position, 1.0);
    varyingColor = vec4(position, 1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}