#version 100
precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D screenTexture;

void main()
{
    gl_FragColor.rgb = texture2D(screenTexture, fragTexCoord).rgb;
    gl_FragColor.a = 1.0f;
}
