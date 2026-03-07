#version 330 core
layout (location = 0) in vec2 aLocalPos;

uniform mat4 uViewProjection;
uniform vec2 uRectPosition;
uniform vec2 uRectSize;

void main() {
    vec2 worldPos = uRectPosition + (aLocalPos * uRectSize);
    gl_Position = uViewProjection * vec4(worldPos, 0.0, 1.0);
}
