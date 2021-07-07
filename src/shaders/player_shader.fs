#version 330 core
in vec2 Coords;
out vec4 color;

uniform bool light;
uniform vec2 position;
uniform vec3 u_Color;

void main() {
    if(light) {
        color = vec4(u_Color, 1.0);
    }
    else{
        float distance = distance(Coords, position);
        if(distance > 75) {
            color = vec4(0.0, 0.0, 0.0, 1.0);
        }
        else {
            color = vec4(u_Color, 1.0);
        }
    }
}