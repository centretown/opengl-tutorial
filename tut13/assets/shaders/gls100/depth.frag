#version 100
precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture1;

void main()
{
    gl_FragColor.rgb = texture2D(texture1, fragTexCoord).rgb;
    gl_FragColor.a = 1.0f;
}
