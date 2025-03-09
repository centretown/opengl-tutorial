#version 100
precision mediump float;

attribute vec2 TexCoords;

varying vec4 FragColor;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoords);
}
