#version 430
out vec4 color;
in vec2 tc;
in vec4 varyingColor;
layout (binding = 0) uniform sampler2D samp;

void main(void) {
    color = texture(samp, tc) * 0.5 + varyingColor * 0.5;
}