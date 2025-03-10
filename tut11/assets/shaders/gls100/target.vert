#version 100
precision mediump float;

attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec2 aTexCoords;

varying vec3 fragPos;
varying vec3 fragNormal;
varying vec2 fragTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    fragPos = vec3(model * vec4(aPos, 1.0));
    fragNormal = aNormal;
    fragTexCoords = aTexCoords;
    gl_Position = projection * view * vec4(fragPos, 1.0);
}
