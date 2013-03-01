#version 130

uniform sampler2D tilemap;
uniform sampler2D tileGraphics;

precision highp float;

void main()
{
	const int TileSize = 32;
	const int TileRenderSize = 128;
	const int TileCount = 32;
  	const int TileRenderFactor = TileRenderSize / TileSize;
	
	vec2 tilePos = gl_TexCoord[0].xy / (TileRenderFactor*TileCount);
	
  	// r is a float from 0-1, we want int from 0-256
  	float index = texture2D(tilemap,tilePos).r*256;
	
	//gl_FragColor = vec4( index/4 , 0, 0, 1);
	
	//calc texCoords of the Tile on the atlas we need
	// /4 because we have 4 tiles on the atlas, so tile 0 = 0.0, 1 = 0.25, etc
  	vec2 baseTilePos = vec2(index / 4.0, 0);
	
	// which pixel on the tile do we need
	// our texcoords have a range from 0..32 so our tiles with 128 px would end at 4, compensate
	vec2 Coords = gl_TexCoord[0].xy / TileRenderFactor;
	// clamp to only get the values of the current tile and divide by 4 ( 4 imgs on the atlas ) to get valid texcoords for the atlas
  	vec2 internalPos = mod( Coords, 1.0) / 4;
	
	gl_FragColor = texture2D( tileGraphics, baseTilePos+internalPos );
}

