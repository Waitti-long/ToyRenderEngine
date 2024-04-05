#version 430

struct SpotLight {
    vec3 position;
    vec4 color;
};

uniform SpotLight spot_light;
uniform mat4 norm_matrix;

in vec2 tc;
in vec3 varyingNormal;
in vec4 varyingVertPos;
in vec3 varyingColor;

out vec4 color;

// shadow map
uniform float enable_shadow_map;
in vec4 shadow_coord;
layout (binding = 1) uniform sampler2DShadow shadow_texture;

void main() {
    vec3 L = normalize(spot_light.position - varyingVertPos.xyz);
    vec3 N = normalize((norm_matrix * vec4(varyingNormal, 1.0)).xyz);
    vec3 V = normalize(-varyingVertPos.xyz);
    vec3 R = reflect(-L, N);

    float k = 0.3;
    if (enable_shadow_map == 0.0 || textureProj(shadow_texture, shadow_coord) == 1.0) {
        k = k + max(dot(N, L), 0.0) + pow(max(dot(R, V), 0.0f), 2);
    }
    color = vec4(varyingColor * k, 1.0);
    color.rgb = vec3(pow(color.r, 1 / 2.2), pow(color.g, 1 / 2.2), pow(color.b, 1 / 2.2));
}