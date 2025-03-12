#version 100
precision mediump float;

// Input vertex attributes
attribute vec3 aPos;
attribute vec2 aTexCoords;
attribute vec4 vertexColor;

varying vec2 fragTexCoord;
varying vec4 fragColor;

void main()
{
    fragTexCoord = aTexCoords;
    fragColor = vertexColor;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
