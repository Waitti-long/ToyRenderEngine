#version 430

uniform vec3 samples[64];
uniform mat4 proj_matrix;
uniform vec2 screen_size;

in vec4 varyingVertPos;

layout (binding = 0) uniform sampler2D g_position;
layout (binding = 1) uniform sampler2D g_normal;

layout (location = 0) out float occlusion;
// for debug
layout (location = 1) out vec3 debug_output;

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
                 43758.5453123) * 2.0 - 1.0;
}

void main() {
    vec4 proj_pos = proj_matrix * varyingVertPos;
    vec2 vert_pos = gl_FragCoord.xy / screen_size.xy;

    vec3 frag_pos = texture(g_position, vert_pos).xyz;
    vec3 normal = texture(g_normal, vert_pos).xyz;
    vec3 random_vec = vec3(random(vert_pos), random(vert_pos * 2.0), 0.0);

    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    occlusion = 1.0;

    float radius = 1.0;
    float kernel_size = 64;
    for (int i = 0; i < kernel_size; i++) {
        vec3 samp = samples[i];
        vec3 sam = frag_pos + TBN * samp;
        vec4 offset = vec4(sam, 1.0);
        offset = proj_matrix * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        float sample_depth = -texture(g_position, offset.xy).w;
        if (sample_depth >= sam.z) {
            occlusion += 1.0;
        }
//        float range_check = smoothstep(0.0, 1.0, radius / abs(sample_depth - varyingVertPos.z));
//        if (sample_depth >= sam.z) {
//            occlusion += 1.0 * range_check;
//        }
    }

    occlusion = 1.0 - (occlusion / kernel_size);
}