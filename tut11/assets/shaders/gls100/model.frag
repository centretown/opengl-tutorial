#version 100
precision mediump float;

varying vec2 fragTexCoord;

uniform sampler2D texture_diffuse1;

void main()
{
    gl_FragColor.rgb = texture2D(texture_diffuse1, fragTexCoord).rgb;
    gl_FragColor.a = 1.0f;
}
