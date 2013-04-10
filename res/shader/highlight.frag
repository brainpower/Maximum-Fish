#version 130

uniform sampler2D creatures;
precision highp float;

void main()
{
	vec4 hl = vec4(gl_Color.xyz, 0);
	vec4 tex = texture2D(creatures, gl_TexCoord[0].xy);
	vec4 new = tex + hl*(1-tex.a);
	if ( gl_Color.a != 1 && new.a > 0 ) new.a = 1;
	gl_FragColor = new;
	
}
