#version 100

precision mediump float;

// Input vertex attributes
attribute vec3 aPos;
attribute vec2 aTexCoords;
attribute vec4 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec2 fragTexCoord;
varying vec4 fragColor;

void main()
{
    fragTexCoord = aTexCoords;
    fragColor = vertexColor;
    vec4 worldPos = model * vec4(aPos, 1.0f);
    gl_Position = projection * view * worldPos;
}
