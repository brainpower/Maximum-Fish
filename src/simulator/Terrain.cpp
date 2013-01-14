#include "Terrain.hpp"

#include "sbe/event/Event.hpp"
#include "sbe/Module.hpp"
#include "sbe/Geom.hpp"

#include <random>

std::shared_ptr<Tile>& Terrain::getTile( Geom::Vec2f pos )
{
	int index = (int)(pos.x) * Size.x + (int)(pos.y);
	if (index < 0 || index > Tiles.size()) return InvalidTile;
	return Tiles[ index ];
}

float Terrain::getTileElevation(Geom::Vec2f pos)
{
	int index = (int)(pos.x) * Size.x + (int)(pos.y);
	if (index < 0 || index > Tiles.size()) return -1;
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

std::vector<std::shared_ptr<Tile>> Terrain::get_nearby(Tile &tile, unsigned int radius)
{
	std::vector<std::shared_ptr<Tile>> ret;

	for(std::shared_ptr<Tile> t: Tiles)
	{
		if( ((t->getPosition().y < tile.getPosition().y-radius)  &&  (t->getPosition().y < tile.getPosition().y+radius))
			&& ((t->getPosition().x < tile.getPosition().x+radius) && (t->getPosition().x > tile.getPosition().x-radius)) )
		{
			ret.push_back(t);
		}
	}
	return ret;
}

void Terrain::CreateDebugTerrain()
{
	Tiles.clear();

	Size = Geom::Vec2( 32, 32 );

	float maxHeight = 1500;
	//float minheight = 0;

	float maxFallofDist = Size.x/2;

	Geom::Pointf Mid = Geom::Pointf( Size.x/2, Size.y/2 );


	std::default_random_engine gen;
	std::uniform_real_distribution<float> rnd;


	for ( int x = 0; x < Size.x; ++x)
	{
		for ( int y = 0; y < Size.y; ++y)
		{
			Geom::Pointf TileMid = Geom::Pointf( x+.5, y+.5 );
			float HeightFactor = (1 - Geom::distance( TileMid, Mid )/maxFallofDist ) ;
			HeightFactor = HeightFactor < 0 ? 0 : HeightFactor;
			float TileHeight = maxHeight*HeightFactor;
			float Humidity = 0;


			if (TileHeight > maxElevation) maxElevation = TileHeight;

			if(TileHeight < 0.02)
			{
				Humidity = 1;
			}
			Tile *tmp = new Tile( Geom::Point(x,y), TileHeight, rnd(gen), Humidity );
			std::shared_ptr<Tile> T(tmp);
			Tiles.push_back ( T );
			Humidity = 0;
		}
	}

	UpdateTerrain();
}
