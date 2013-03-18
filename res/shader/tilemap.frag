#version 130

uniform sampler2D tilemap;
uniform sampler2D tileGraphics;

uniform float TerrainSize;
uniform float TileRenderSize;
uniform vec2 AtlasSize;

precision highp float;

void main()
{
  	float TileRenderFactor = TileRenderSize / TerrainSize;
	
	vec2 tilePos = gl_TexCoord[0].xy / (TileRenderFactor*TerrainSize);
	
  	// r is a float from 0-1, we want int from 0-256
  	float index = floor(texture2D(tilemap,tilePos).r*256);
	
	//gl_FragColor = vec4( TerrainSize/32 , 0, 0, 1);
	
	//calc texCoords of the Tile on the atlas we need
	// /AtlasSize.x because we have x tiles on the atlas( 0..1 ), so tile 0 = 0.0, 1 = 1/x, 2 = 2/x etc	
  	vec2 baseTilePos = vec2(index / AtlasSize.x, floor( index/AtlasSize.x ) / AtlasSize.y );
	
	// which pixel on the tile do we need
	// our texcoords have a range from 0..TerrainSize compensate
	vec2 Coords = gl_TexCoord[0].xy / TileRenderFactor;
	
	// clamp to only get the values of the current tile and divide by 4 ( 4 imgs on the atlas ) to get valid texcoords for the atlas
  	vec2 internalPos = mod( Coords, 1.0);
	internalPos /= AtlasSize;
	
	gl_FragColor = texture2D( tileGraphics, baseTilePos+internalPos );
}

