#version 330 core
layout (location = 0) in vec4 vertex;

out vec2 TexCoords;
out vec2 Coords;

uniform mat4 model;
uniform mat4 projection;

void main() {
    vec4 wc = model * vec4(vertex.xy, 0.0, 1.0);
    Coords = vec2(wc.xy);
    TexCoords = vertex.zw;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}