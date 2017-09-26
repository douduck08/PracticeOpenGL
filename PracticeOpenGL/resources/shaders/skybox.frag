#version 330 core
uniform samplerCube texture0;

in vec3 TexCoords;
out vec4 color;

void main() {
    if(textureSize(texture0, 0).x > 0) {
        color = texture(texture0, TexCoords);
    } else {
        color = vec4(1.f, 0.f, 1.f, 1.f);
    }
}
