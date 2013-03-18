#include "Terrain.hpp"

#include "sbe/event/Event.hpp"
#include "sbe/Module.hpp"
#include "sbe/geom/Helpers.hpp"
#include "sbe/Config.hpp"

#include "sbe/gfx/MapPlotter.hpp"

#include "Creature.hpp"

#include <SFML/Graphics/Image.hpp>

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
	unsigned int index = Geom::linear(pos, Size.x);
	if (!validPos(pos) || index >= Tiles.size()) return InvalidTile;
	return Tiles[ index ];
}

float Terrain::getTileElevation(Geom::Vec2f pos)
{
	unsigned int index = Geom::linear(pos, Size.x);
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

	// we're using squared distances
	float r2 = radius*radius;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( Geom::Vec2f(x,y) );
			if (!T) break;

			for (std::shared_ptr<Creature>& C : T->Creatures)
			{
				if ( Geom::squaredist( C->getPosition(), Position ) < r2 && filter ( C ) ) ret.push_back(C);
			}
		}
	}

	return ret;
}

std::list<std::shared_ptr<Creature>> Terrain::getNearby(Geom::Vec2f Position, float radius, int type, std::function<bool(const std::shared_ptr<Creature>&)> filter )
{
	std::list<std::shared_ptr<Creature>> ret;

	// we're using squared distances
	float r2 = radius*radius;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( Geom::Vec2f(x,y) );
			if (!T) break;

			for (std::shared_ptr<Creature>& C : T->Types[type])
			{
				if ( Geom::squaredist( C->getPosition(), Position ) < r2 && filter ( C ) ) ret.push_back(C);
			}
		}
	}

	return ret;
}

std::list<std::shared_ptr<Creature>> Terrain::getNearby(Geom::Vec2f Position, float radius, std::shared_ptr<Species> species,  std::function<bool(const std::shared_ptr<Creature>&)> filter )
{
	std::list<std::shared_ptr<Creature>> ret;

	// we're using squared distances
	float r2 = radius*radius;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( Geom::Vec2f(x,y) );
			if (!T) break;

			for (std::shared_ptr<Creature>& C : T->SpeciesList[species])
			{
				if ( Geom::squaredist( C->getPosition(), Position ) < r2 && filter ( C ) ) ret.push_back(C);
			}
		}
	}

	return ret;
}


std::shared_ptr<Creature> Terrain::getNearest(Geom::Vec2f Position, float radius, std::function<bool(const std::shared_ptr<Creature>&)> filter )
{
	std::shared_ptr<Creature> nearest;

	// squared distance, avoid squrt in further comparisions
	float mindist2 = radius*radius;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( Geom::Vec2f(x,y) );
			if (!T || (x==Position.y && y == Position.y)) continue;

			for (std::shared_ptr<Creature>& C : T->Creatures)
			{
				float curdist = Geom::squaredist( C->getPosition(), Position );
				if ( curdist < mindist2 && filter ( C ) )
				{
					nearest = C;
					mindist2 = curdist;
				}
			}
		}
	}

	return nearest;
}

std::shared_ptr<Creature> Terrain::getNearest(Geom::Vec2f Position, float radius, int type, std::function<bool(const std::shared_ptr<Creature>&)> filter )
{
	std::shared_ptr<Creature> nearest;

	// squared distance, avoid squrt in further comparisions
	float mindist2 = radius*radius;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( Geom::Vec2f(x,y) );
			if (!T || (x==Position.y && y == Position.y)) continue;

			for (std::shared_ptr<Creature>& C : T->Types[type])
			{
				float curdist = Geom::squaredist( C->getPosition(), Position );
				if ( curdist < mindist2 && filter ( C ) )
				{
					nearest = C;
					mindist2 = curdist;
				}
			}
		}
	}

	return nearest;
}

std::shared_ptr<Creature> Terrain::getNearest(Geom::Vec2f Position, float radius, std::shared_ptr<Species> species,  std::function<bool(const std::shared_ptr<Creature>&)> filter )
{
	std::shared_ptr<Creature> nearest;

	// squared distance, avoid squrt in further comparisions
	float mindist2 = radius*radius;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( Geom::Vec2f(x,y) );
			if (!T || (x==Position.y && y == Position.y)) continue;

			for (std::shared_ptr<Creature>& C : T->SpeciesList[species])
			{
				float curdist = Geom::squaredist( C->getPosition(), Position );
				if ( curdist < mindist2 && filter ( C ) )
				{
					nearest = C;
					mindist2 = curdist;
				}
			}
		}
	}

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
		population.push_back(T->Creatures.size());
		height.push_back( T->getHeight() );
	}

	Event e("DISPLAY_MAP");

	std::shared_ptr<sbe::MapPlotter> p( new sbe::MapPlotter("Population density" ));
	p->setData( population, Size, true );
	p->plot();
	e.SetData( p );
	Module::Get()->QueueEvent( e, true );

	p.reset( new sbe::MapPlotter( "Humidity", sbe::MapPlotter::PLOT_HEATMAP) );
	p->setData( humidity, Size, true );
	p->plot();
	e.SetData( p );
	Module::Get()->QueueEvent( e, true );

	p.reset( new sbe::MapPlotter("Heightmap") );
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
	std::uniform_real_distribution<float> nutritionrnd(Engine::getCfg()->get<float>("sim.terragen.debug.nutrition.min"),Engine::getCfg()->get<float>("sim.terragen.debug.nutrition.max"));

	float minHumidity = Engine::getCfg()->get<float>("sim.terragen.debug.humidity.min");
	float maxHumidity = Engine::getCfg()->get<float>("sim.terragen.debug.humidity.max");


	for ( int y = 0; y < Size.y; ++y)
	{
		for ( int x = 0; x < Size.x; ++x)
		{
			Geom::Pointf TileMid = Geom::Pointf( x+.5, y+.5 );
			float HeightFactor = (1 - Geom::distance( TileMid, Mid )/maxFallofDist );
			HeightFactor = HeightFactor < 0 ? 0 : HeightFactor;
			float TileHeight = maxHeight*HeightFactor;
			float Humidity = minHumidity + (maxHumidity-minHumidity)*(1-HeightFactor);


			if (TileHeight > maxElevation) maxElevation = TileHeight;

			if(TileHeight < waterlimit)
			{
				Humidity = 1;
			}
			Tile *tmp = new Tile( Geom::Point(x,y), TileHeight, nutritionrnd(gen), Humidity );
			std::shared_ptr<Tile> T(tmp);
			Tiles.push_back ( T );
		}
	}

	tilemapImage.reset ( new sf::Image);
	tilemapImage->create(Size.x ,Size.y);
	for(int x = 0; x < Size.x; x++)
	{
		for(int y = 0; y < Size.y; y++)
		{
			Geom::Vec2f pos(x, y);
			sf::Color tile(getTile(pos)->getTileSpriteIndex(),0,0,0);
			tilemapImage->setPixel(x, y, tile);
		}
	}

	Module::Get()->QueueEvent(Event("UpdateTilemapTexture", tilemapImage), true);
}
