#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 color;

uniform mat4 depthMVP;

void main(){
    gl_Position =  depthMVP * vec4(position, 1);
}
