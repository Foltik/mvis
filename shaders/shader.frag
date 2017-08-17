#version 330 core

out vec4 color;

in vec3 pcol;

void main() {
    color = vec4(pcol, 1.0);
}