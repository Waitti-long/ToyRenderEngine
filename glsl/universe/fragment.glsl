#version 430

struct SpotLight {
    vec3 position;
    vec4 color;
};

layout (binding = 0) uniform sampler2D samp;
uniform SpotLight spot_light;
uniform mat4 norm_matrix;

in vec2 tc;
in vec3 varyingNormal;
in vec4 varyingVertPos;

out vec4 color;

void main() {
    vec3 L = normalize(spot_light.position - varyingVertPos.xyz);
    vec3 N = normalize((norm_matrix * vec4(varyingNormal, 1.0)).xyz);
    vec3 V = normalize(-varyingVertPos.xyz);
    vec3 R = reflect(-L, N);


    color = texture(samp, tc) * (0.05 + max(dot(N, L), 0.0) + pow(max(dot(R, V), 0.0f), 3));
}