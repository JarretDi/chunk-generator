#version 330 core

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 colour;

uniform vec3 vertexColour;
uniform sampler2D texture1;

void main() {
   colour = vec4(texture(texture1, texCoord).rgb, 1.0);
}