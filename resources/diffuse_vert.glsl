#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat3 N;
uniform vec2 tiling;

uniform vec3 cameraPos;

out vec3 worldPos;
out vec3 fragNor;
out vec2 texCoords;

void main() {
    worldPos = vec3(M * vec4(vertPos, 1.0f));
    fragNor = N * vertNor;
    texCoords = vec2(vertTex.x*tiling.x, vertTex.y*tiling.y);

    gl_Position = P * V * vec4(worldPos, 1.0f);
}