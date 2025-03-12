#version 100
precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture1;

void main()
{
    gl_FragColor.rgba = texture2D(texture1, fragTexCoord).rgba;
}
