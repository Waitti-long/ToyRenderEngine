#version 430
out vec4 color;
in vec2 tc;
in vec4 varyingColor;
in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec3 varyingHalfVector;

struct PositionalLight {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
};

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};
uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;

layout (binding = 0) uniform sampler2D samp;

void main(void) {
    vec3 L = normalize(varyingLightDir);
    vec3 N = normalize(varyingNormal);
    vec3 H = normalize(varyingHalfVector);

    vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;
    vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(dot(N, L), 0.0);
    vec3 specular = material.specular.xyz * light.specular.xyz * pow(max(dot(H, N), 0.0f), material.shininess * 3.0);
    vec4 c = vec4(ambient + diffuse + specular, 1.0);

    color = texture(samp, tc) * 0.5 + c * 0.5;
}