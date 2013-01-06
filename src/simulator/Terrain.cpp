#include "Terrain.hpp"

#include "sbe/event/Event.hpp"
#include "sbe/Module.hpp"
#include "sbe/Geom.hpp"

#include <random>

std::shared_ptr<Tile> Terrain::getTile( Geom::Vec2f pos )
{
	int index = (int)(pos.x()) * Size.x() + (int)(pos.y());
	return Tiles[ index ];
}

float Terrain::getTileElevation(Geom::Vec2f pos)
{
	int index = (int)(pos.x()) * Size.x() + (int)(pos.y());
	return Tiles[ index ]->getHeight();
}

float Terrain::getMaxElevation()
{
	return maxElevation;
}

float Terrain::getGlobalTemp()
{
	return globalTemp;
}

void Terrain::UpdateTerrain()
{
	//make a freakin Event, man
	Event e("UpdateTileRenderList");
	e.SetData( Tiles );
	Module::Get()->QueueEvent(e, true);
}

void Terrain::CreateDebugTerrain()
{
	Tiles.clear();

	Size = Geom::Vec2( 100, 100 );

	float maxHeight = 1500;
	float minheight = 0;

	float maxFallofDist = Size.x()/2;

	Geom::Pointf Mid = Geom::Pointf( Size.x()/2, Size.y()/2 );


	std::default_random_engine gen;
	std::uniform_real_distribution<float> rnd;


	for ( int x = 0; x < Size.x(); ++x)
	{
		for ( int y = 0; y < Size.y(); ++y)
		{
			Geom::Pointf TileMid = Geom::Pointf( x+.5, y+.5 );
			float HeightFactor = (1 - Geom::distance( TileMid, Mid )/maxFallofDist ) ;
			HeightFactor = HeightFactor < 0 ? 0 : HeightFactor;
			float TileHeight = maxHeight*HeightFactor;

			if (TileHeight > maxElevation) maxElevation = TileHeight;

			std::shared_ptr<Tile> T ( new Tile( Geom::Point(x,y), TileHeight, rnd(gen), 0 ) );
			Tiles.push_back ( T );
		}
	}

	UpdateTerrain();
}

