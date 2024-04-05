#version 430

layout (location = 0) out vec4 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec3 g_color;

in vec4 varyingVertPos;
in vec3 varyingNormal;
in vec3 varyingColor;

void main() {
    g_position.xyz = varyingVertPos.xyz;
    g_normal = varyingNormal;
    g_color = varyingColor;
    g_position.a = gl_FragCoord.a;
}