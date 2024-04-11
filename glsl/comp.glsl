#version 430

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D input_image;
layout (rgba32f, binding = 1) uniform image2D output_image;

uniform float v[4];

shared vec4 mat_shared[16][16];

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    mat_shared[pos.x][pos.y] = imageLoad(input_image, pos);
    barrier();
    vec4 data = mat_shared[pos.x][pos.y];
    data = vec4(v[0], v[1], v[2], v[3]);
    imageStore(output_image, pos.xy, data);
}