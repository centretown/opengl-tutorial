#version 100
precision mediump float;

attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec2 aTexCoords;

varying vec2 fragTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    fragTexCoord = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
