#version 430

uniform vec2 noise_scale;
uniform vec3 samples[64];
uniform mat4 proj_matrix;

in vec4 varyingVertPos;

layout (binding = 0) uniform sampler2D g_position;
layout (binding = 1) uniform sampler2D g_normal;
layout (binding = 2) uniform sampler2D ssao_noise;

layout (location = 0) out float occlusion;

void main() {
    vec4 proj_pos = proj_matrix * varyingVertPos;
    vec2 vert_pos = (proj_pos.xy / proj_pos.w) * 0.5 + 0.5;

    vec3 frag_pos = texture(g_position, vert_pos).xyz;
    vec3 normal = texture(g_normal, vert_pos).xyz;
    vec3 random_vec = texture(ssao_noise, varyingVertPos.xy * noise_scale).xyz;

    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    occlusion = 0.0;
    float radius = 1.0;
    for (int i = 0; i < 64; i++) {
        vec3 sam = TBN * samples[i];
        sam = frag_pos + sam * radius;

        vec4 offset = vec4(sam, 1.0);
        offset = proj_matrix * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        float sample_depth = -texture(g_position, offset.xy).w;
        float range_check = smoothstep(0.0, 1.0, radius / abs(varyingVertPos.z - sample_depth));
        if (sample_depth >= sam.z) {
            occlusion += 1.0 * range_check;
        }
    }

    occlusion /= 64;
}