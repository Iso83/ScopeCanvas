#version 330 core
layout (location = 0) in vec2 aLocalPos;

uniform mat4 uViewProjection;
uniform vec2 uNodePosition;
uniform vec2 uNodeSize;

void main() {
    vec2 worldPos = uNodePosition + (aLocalPos * uNodeSize);
    gl_Position = uViewProjection * vec4(worldPos, 0.0, 1.0);
}
