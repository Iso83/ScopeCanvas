#version 330 core
layout(location = 0) in vec2 aWorldPos;

uniform mat4 uViewProjection;

void main()
{
    gl_Position = uViewProjection * vec4(aWorldPos, 0.0, 1.0);
}
