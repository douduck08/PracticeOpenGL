#version 330 core
layout(location = 0) out vec3 outColor;
uniform sampler2D texture0;
uniform sampler2D texture1; //shadow map

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform vec3 viewPos;
uniform DirLight dirLight;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in vec4 ShadowCoord;
out vec4 color;

vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir, float visibility);

void main() {
    float visibility = 1.0f;
    if (texture(texture1, ShadowCoord.xy).r  <  ShadowCoord.z - 0.005){
        visibility = 0.5f;
    }
    
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = CalcDirLight(dirLight, normal, viewDir, visibility);
    
    color = vec4(result, 1.0f);
    outColor = result;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float visibility) {
    vec3 lightDir = normalize(-light.direction);
    vec3 color = vec3(texture(texture0, TexCoord));
    
    vec3 ambient = light.ambient * color;
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * color;
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.f);
    vec3 specular = light.specular * spec * color;
    
    return ambient + visibility * (diffuse + specular);
}
