#version 130

precision highp float;

void main()
{
	gl_FragColor = gl_TexCoord[gl_Position];
}