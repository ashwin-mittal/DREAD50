#version 330 core
in vec2 TexCoords;
in vec2 Coords;
out vec4 color;

uniform bool light;
uniform vec2 position;
uniform sampler2D image;
uniform vec3 spriteColor;

void main() {
    if(light) {
        color = vec4(spriteColor, 1.0) * texture(image, TexCoords);
    }
    else {
        float distance = distance(Coords, position);
        if(distance > 75) {
            color = vec4(0.0, 0.0, 0.0, 1.0) * texture(image, TexCoords);
        }
        else {
            color = vec4(spriteColor, 1.0) * texture(image, TexCoords);
        }
    }    
}