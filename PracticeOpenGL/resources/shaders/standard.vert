#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 depthBiasMVP;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;
out vec4 ShadowCoord;

void main() {
    vec4 worldPos = model * vec4(position, 1.0f);
    gl_Position = projection * view * worldPos;
    FragPos = worldPos.xyz;
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoord = texCoord;
    ShadowCoord = depthBiasMVP * vec4(position, 1);
}
