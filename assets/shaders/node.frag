#version 330 core
out vec4 FragColor;

uniform vec3 uNodeColor;

void main() {
    FragColor = vec4(uNodeColor, 1.0);
}
