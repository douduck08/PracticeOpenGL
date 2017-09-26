#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main() {
    vec4 worldPos = model * vec4(position, 1.0f);
    gl_Position = projection * view * worldPos;
    FragPos = worldPos.xyz;
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoords = texCoord;
}
