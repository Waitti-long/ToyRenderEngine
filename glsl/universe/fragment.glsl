#version 430

struct SpotLight {
    vec3 position;
    vec4 color;
};

uniform SpotLight spot_light;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

in vec2 tc;
in vec3 varyingNormal;
in vec4 varyingVertPos;
in vec3 varyingColor;
in float varyingMetallic;

out vec4 color;

// shadow map
uniform float enable_shadow_map;
uniform mat4 shadow_mvp;
in vec4 shadow_coord;
layout (binding = 1) uniform sampler2DShadow shadow_texture;
uniform int pcf_kernel;

// ssao
uniform float enable_ssao;
layout (binding = 2) uniform sampler2D ssao_texture;
uniform vec2 screen_size;

// g buffer
layout (binding = 3) uniform sampler2D g_position;

// ssr
uniform float enable_ssr;
layout (binding = 4) uniform sampler2D g_normal;
layout (binding = 5) uniform sampler2D g_color;

float SampleSSAO(vec2 uv) {
    float count = 0.0;
    float total = 0.0;
    vec2 pixel = vec2(1.0, 1.0) / screen_size.xy;
    int kernel_size = 16;
    for (int x = -kernel_size; x < kernel_size; x++) {
        for (int y = -kernel_size; y < kernel_size; y++) {
            total += texture(ssao_texture, uv + pixel * vec2(x, y)).r;
            count += 1;
        }
    }
    return total / count;
}

float PCF() {
    if (pcf_kernel == 0) {
        return textureProj(shadow_texture, shadow_coord);
    }

    mat4 inv_mv = inverse(mv_matrix);
    float total = 0.0, count = 0.0;
    for (int x = -pcf_kernel; x < pcf_kernel; x++) {
        for (int y = -pcf_kernel; y < pcf_kernel; y++) {
            vec2 sample_pos = (gl_FragCoord.xy + vec2(x, y)) / screen_size.xy;
            vec4 sample_position = texture(g_position, sample_pos);
            vec4 sample_coord = shadow_mvp * inv_mv * vec4(sample_position.xyz, 1.0);
            total += textureProj(shadow_texture, sample_coord);
            count++;
        }
    }
    return total / count;
}

bool RayMarch(vec3 origin, vec3 direction, out vec3 dist) {
    float delta = 0.01;
    int st = 0;
    while (st++ < 1000) {
        origin += direction * delta;
        vec4 offset = proj_matrix * vec4(origin, 1.0);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        vec4 pos = texture(g_position, offset.xy);
        if (origin.z < -pos.w) {
            dist = texture(g_color, offset.xy).rgb;
            color.r = st / 2;
            return true;
        }
    }
    return false;
}

vec3 SSR(vec3 L, vec3 N, vec3 V) {
    vec3 R = reflect(-V, N);
    vec3 dist = vec3(0, 0, 0);
    if (RayMarch(varyingVertPos.xyz, R, dist)) {
        return dist;
    }
    return vec3(0.0, 0.0, 0.0);
}

void main() {
    vec3 L = normalize(spot_light.position - varyingVertPos.xyz);
    vec3 N = normalize((norm_matrix * vec4(varyingNormal, 1.0)).xyz);
    vec3 V = normalize(-varyingVertPos.xyz);
    vec3 R = reflect(-L, N);

    vec3 k = vec3(0.4, 0.4, 0.4);

    if (enable_ssao == 1.0) {
        vec4 proj_pos = proj_matrix * varyingVertPos;
        vec2 vert_pos = gl_FragCoord.xy / screen_size.xy;
        float ratio = SampleSSAO(vert_pos);
        k = k * ratio;
    } else if (enable_ssr == 1.0 && varyingMetallic == 1.0) {
        k = k * SSR(L, N, V);
    }

    float ref = max(dot(N, L), 0.0) + pow(max(dot(R, V), 0.0f), 8);

    if (enable_shadow_map == 1.0) {
        k += ref * PCF();
    } else {
        k += ref;
    }

    color = vec4(varyingColor * k, 1.0);
    color.rgb = vec3(pow(color.r, 1 / 2.2), pow(color.g, 1 / 2.2), pow(color.b, 1 / 2.2));
}