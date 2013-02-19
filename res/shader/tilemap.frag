#version 130

uniform sampler2D tilemap;
uniform sampler2D tileGraphics;

void main()
{
  vec2 tilePos=gl_TexCoord[0].xy/32;
  float index = floor(texture2D(tilemap,tilePos).r*256);
  vec2 baseTilePos=0.25*floor(vec2(mod(index,4),index/4)); 
  vec2 internalPos=0.25*mod(gl_TexCoord[0].xy*32,1);
  gl_FragColor=texture2D(tileGraphics,baseTilePos+internalPos);
}

