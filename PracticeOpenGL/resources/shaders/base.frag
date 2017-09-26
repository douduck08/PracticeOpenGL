#version 330 core
uniform sampler2D texture0;

in vec3 VertColor;
in vec2 TexCoord;
out vec4 color;

void main() {
    color = texture(texture0, TexCoord) * vec4(VertColor, 1.f);
}
