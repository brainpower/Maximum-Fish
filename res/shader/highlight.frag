#version 130

uniform sampler2D creatures;
precision highp float;

void main()
{
	gl_FragColor = texture2D(creatures, gl_TexCoord[0].xy)*gl_Color;
}