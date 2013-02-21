#version 130

uniform sampler2D tilemap;
uniform sampler2D tileGraphics;

void main()
{
	//this has to be divided by the fucking size of ONE tile
	//so don't change anything if you have no clue what you do
  	vec2 tilePos=gl_TexCoord[0].xy/32;

  	// this has to be multiplied by 256 to get the right value
  	float index = floor(texture2D(tilemap,tilePos).r*256);

  	// division by the number of tiles in one row of the tileset
  	vec2 baseTilePos=0.25*floor(vec2(mod(index,4),index/4));

  	// again the size of a tile   
  	vec2 internalPos=0.25*mod(gl_TexCoord[0].xy*32,1);
  	gl_FragColor=texture2D(tileGraphics,baseTilePos+internalPos);
}

