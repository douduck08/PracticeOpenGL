#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;

void main() {
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyzw;
    TexCoords = position;
}
