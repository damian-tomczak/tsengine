#pragma once

const float gridSize = 100.0;

const float gridCellSize = 0.5;

vec4 gridColorThin = vec4(1, 0, 0, 1.0);

vec4 gridColorThick = vec4(0, 1, 0, 1.0);

const float gridMinPixelsBetweenCells = 2.0;

const vec3 pos[4] =
{
    vec3(-1.0, 0.0, -1.0),
    vec3(-1.0, 0.0,  1.0),
    vec3( 1.0, 0.0,  1.0),
    vec3( 1.0, 0.0, -1.0)
};

const int indices[6] = {
    0, 1, 2, 2, 3, 0
};
