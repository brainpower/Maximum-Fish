#include "Terrain.hpp"

#include "sbe/event/Event.hpp"
#include "sbe/Module.hpp"
#include "sbe/geom/Helpers.hpp"
#include "sbe/Config.hpp"

#include "sbe/gfx/MapPlotter.hpp"

#include "Creature.hpp"


#include <random>

Terrain::Terrain()
 : Size(0,0),
 /*humidityFactor(1.0),
 globalTemp(20),*/
 maxElevation(0)
{
	humidityFactor = Engine::getCfg()->get<float>("sim.terrain.humidityFactor");
	globalTemp = Engine::getCfg()->get<float>("sim.terrain.globalTemp");

	// set some const values
	Tile::maxsandheight = Engine::getCfg()->get<float>("sim.terrain.maxsandheight");
	Tile::maxgrassheight = Engine::getCfg()->get<float>("sim.terrain.maxgrassheight");
	Tile::maxwalkableHumidity = Engine::getCfg()->get<float>("sim.terrain.maxwalkablehumidity");

}

std::shared_ptr<Tile>& Terrain::getTile( Geom::Vec2f pos )
{
	unsigned int index = (int)(pos.x) * Size.x + (int)(pos.y);
	if (!validPos(pos) || index >= Tiles.size()) return InvalidTile;
	return Tiles[ index ];
}

float Terrain::getTileElevation(Geom::Vec2f pos)
{
	unsigned int index = (int)(pos.x) * Size.x + (int)(pos.y);
	if (!validPos(pos) || index > Tiles.size()) return -1;
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
	Module::Get()->QueueEvent(Event("UpdateTileRenderList", Tiles), true);
}

std::list<std::shared_ptr<Tile>> Terrain::getNeighbours(Tile& T)
{
	std::list<std::shared_ptr<Tile>> ret;

	for (int x = T.getPosition().x-1; x < T.getPosition().x+1; ++x)
	{
		for (int y = T.getPosition().y-1; y < T.getPosition().y+1; ++y)
		{
			auto _T = getTile( Geom::Vec2f(x,y) );
			if (!_T || (x = T.getPosition().x && y == T.getPosition().y)) break;

			ret.push_back(_T);
		}
	}

	return ret;
}

std::list<std::shared_ptr<Creature>> Terrain::getNearby(Geom::Vec2f Position, float radius, std::function<bool(const std::shared_ptr<Creature>&)> filter )
{
	std::list<std::shared_ptr<Creature>> ret;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( Geom::Vec2f(x,y) );
			if (!T) break;

			for (std::shared_ptr<Creature>& C : T->getCreatures())
			{
				if ( Geom::distance( C->getPosition(), Position ) < radius && filter ( C ) ) ret.push_back(C);
			}
		}
	}

	return ret;
}

std::shared_ptr<Creature> Terrain::getNearest(Geom::Vec2f Position, float radius, std::function<bool(const std::shared_ptr<Creature>&)> filter )
{
	std::shared_ptr<Creature> nearest;
	float mindist = radius;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( Geom::Vec2f(x,y) );
			if (!T) break;

			for (std::shared_ptr<Creature>& C : T->getCreatures())
			{
				float curdist = Geom::distance( C->getPosition(), Position );
				if ( curdist < mindist && filter ( C ) )
				{
					nearest = C;
					mindist = curdist;
	}	}	}	}

	return nearest;
}

void Terrain::CreateMapPlotters()
{

	std::vector<float> population;
	std::vector<float> height;
	std::vector<float> humidity;


	for ( std::shared_ptr<Tile>& T : Tiles )
	{
		humidity.push_back(T->getHumidity() );
		population.push_back(T->getCreatures().size());
		height.push_back( T->getHeight() );
	}

	Event e("DISPLAY_MAP");

	std::shared_ptr<MapPlotter> p( new MapPlotter("Population density" ));
	p->setData( population, Size, true );
	p->plot();
	e.SetData( p );
	Module::Get()->QueueEvent( e, true );

	p.reset( new MapPlotter( "Humidity", MapPlotter::PLOT_HEATMAP) );
	p->setData( humidity, Size, true );
	p->plot();
	e.SetData( p );
	Module::Get()->QueueEvent( e, true );

	p.reset( new MapPlotter("Heightmap") );
	p->setData( height, Size, true );
	p->plot();
	e.SetData( p );
	Module::Get()->QueueEvent( e, true );

}

void Terrain::CreateDebugTerrain()
{
	Tiles.clear();
	{
		int tmp = Engine::getCfg()->get<float>("sim.terragen.debug.size");
		Size = Geom::Vec2( tmp, tmp );
	}
	// maximum height to generate
	float maxHeight = Engine::getCfg()->get<float>("sim.terragen.debug.maxheight");
	//float minheight = 0;

	float maxFallofDist = Size.x/2;

	float waterlimit = Engine::getCfg()->get<float>("sim.terragen.debug.waterlimit");

	Geom::Pointf Mid = Geom::Pointf( Size.x/2, Size.y/2 );


	std::default_random_engine gen;
	std::uniform_real_distribution<float> rnd;


	for ( int x = 0; x < Size.x; ++x)
	{
		for ( int y = 0; y < Size.y; ++y)
		{
			Geom::Pointf TileMid = Geom::Pointf( x+.5, y+.5 );
			float HeightFactor = (1 - Geom::distance( TileMid, Mid )/maxFallofDist );
			HeightFactor = HeightFactor < 0 ? 0 : HeightFactor;
			float TileHeight = maxHeight*HeightFactor;
			float Humidity = 0;


			if (TileHeight > maxElevation) maxElevation = TileHeight;

			if(TileHeight < waterlimit)
			{
				Humidity = 1;
			}
			Tile *tmp = new Tile( Geom::Point(x,y), TileHeight, rnd(gen), Humidity );
			std::shared_ptr<Tile> T(tmp);
			Tiles.push_back ( T );
		}
	}

	tilemapImage.reset ( new sf::Image);
	tilemapImage->create(Size.x ,Size.y);
	for(int c = 0; c < Size.x; c++)
	{
		for(int r = 0; r < Size.y; r++)
		{
			Geom::Vec2f pos(c, r);
			sf::Color tile(getTile(pos)->getTileSpriteIndex(),0,0,0);
			tilemapImage->setPixel(c, r, tile);
		}
	}

	Module::Get()->QueueEvent(Event("UpdateTilemapTexture", tilemapImage), true);
}
