#version 100

precision mediump float;

// Input vertex attributes
attribute vec3 aPos;
attribute vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    vec4 worldPos = model * vec4(aPos, 1.0f);
    gl_Position = projection * view * worldPos;
}
