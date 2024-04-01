#version 430

layout (binding = 0) uniform sampler2D samp;
in vec2 tc;
in vec3 varyingNormal;

out vec4 color;

void main() {
    color = texture(samp, tc);
}