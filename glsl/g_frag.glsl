#version 430

layout (location = 0) out vec4 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec3 g_color;

uniform float z_near;
uniform float z_far;

in vec4 varyingVertPos;
in vec3 varyingNormal;
in vec3 varyingColor;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * z_near * z_far) / (z_far + z_near - z * (z_far - z_near));
}

void main() {
    g_position.xyz = varyingVertPos.xyz;
    g_normal = varyingNormal;
    g_color = varyingColor;
    g_position.a = LinearizeDepth(gl_FragCoord.z);
}