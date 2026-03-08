#version 330 core

out vec4 FragColor;

in vec2 vNdc;

uniform mat4 uViewProjection;
uniform mat4 uInvViewProjection;
uniform vec2 uViewportSize;
uniform float uCellSize;

float gridLine(vec2 worldPos, float spacing, float widthPx)
{
    vec2 coord = worldPos / spacing;

    vec2 grid = abs(fract(coord) - 0.5);

    float line = min(grid.x, grid.y);

    return 1.0 - smoothstep(0.48, 0.5, line);
}

void main()
{
    vec4 world = uInvViewProjection * vec4(vNdc, 0.0, 1.0);
    vec2 worldPos = world.xy / world.w;

    float cellSize = max(uCellSize, 1.0);
    float minor = gridLine(worldPos, cellSize, 1.0);
    float major = gridLine(worldPos, cellSize * 5.0, 1.5);

    vec3 baseColor  = vec3(0.10, 0.11, 0.12);
    vec3 minorColor = vec3(0.20, 0.22, 0.24);
    vec3 majorColor = vec3(0.32, 0.35, 0.38);

    vec3 color = mix(baseColor, minorColor, minor * 0.6);
    color = mix(color, majorColor, major);

    FragColor = vec4(color, 1.0);
}