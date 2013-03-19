#version 130

uniform sampler2D creatures;
precision highp float;

void main()
{
	vec4 temp = vec4(gl_Color.xyz, 0);
	gl_FragColor = texture2D(creatures, gl_TexCoord[0].xy)+temp;
}