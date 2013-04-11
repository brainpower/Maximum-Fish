#include "Terrain.hpp"

#include "sbe/event/Event.hpp"
#include "sbe/Module.hpp"

#include "sbe/geom/Helpers.hpp"
#include "sbe/geom/PointHelpers.hpp"
#include "sbe/geom/RectHelpers.hpp"

#include "sbe/Config.hpp"

#include "sbe/gfx/MapPlotter.hpp"

#include "Creature.hpp"

#include <SFML/Graphics/Image.hpp>

#include <random>
#include <cmath>

Terrain::Terrain()
 : Size(0,0),
 /*humidityFactor(1.0),
 globalTemp(20),*/
 maxElevation(0)
{
	humidityFactor = Engine::getCfg()->get<float>("sim.terrain.humidityFactor");
	globalTemp = Engine::getCfg()->get<float>("sim.terrain.globalTemp");

	Tile::loadConfigValues();
}

Terrain::Terrain(const Terrain &o)
 : Size(o.Size),
   maxElevation(o.maxElevation),
   humidityFactor(o.humidityFactor),
   globalTemp(o.globalTemp) {

		InvalidTile.reset(new Tile(*(o.InvalidTile))); // necessary?

		for( auto &t : o.Tiles) { // deep copy of tiles
			Tiles.push_back(std::shared_ptr(new Tile(*t)));
		}

		tilemapImage.reset( new sf::Image(*(o.tilemapImage)));

		CreateParallelisationGraph();

		// other things neccessary?
}

const std::shared_ptr<Tile>& Terrain::getTile( Geom::Vec2f pos ) const
{
	unsigned int index = Geom::linear(pos, Size.x);
	if (!validPos(pos) || index >= Tiles.size()) return InvalidTile;
	return Tiles[ index ];
}

float Terrain::getTileElevation(Geom::Vec2f pos) const
{
	unsigned int index = Geom::linear(pos, Size.x);
	if (!validPos(pos) || index > Tiles.size()) return -1;
	return Tiles[ index ]->getHeight();
}

float Terrain::getMaxElevation() const
{
	return maxElevation;
}

float Terrain::getGlobalTemp() const
{
	return globalTemp;
}

void Terrain::UpdateTerrain() const
{
	Module::Get()->QueueEvent(Event("UpdateTileRenderList", Tiles), true);
}

std::list<std::shared_ptr<Tile>> Terrain::getTileList() const
{
	std::list<std::shared_ptr<Tile>> re;
	for ( const std::shared_ptr<Tile>& T : Tiles ) re.push_back(T);
	return re;
}

std::list<std::shared_ptr<Tile>> Terrain::getNeighbours(Tile& T) const
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

std::list<std::shared_ptr<Creature>> Terrain::getNearby(Geom::Vec2f Position, float radius, std::function<bool(const std::shared_ptr<Creature>&)> filter ) const
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

std::list<std::shared_ptr<Creature>> Terrain::getNearby(Geom::Vec2f Position, float radius, int type, std::function<bool(const std::shared_ptr<Creature>&)> filter ) const
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

std::list<std::shared_ptr<Creature>> Terrain::getNearby(Geom::Vec2f Position, float radius, std::shared_ptr<Species> species,  std::function<bool(const std::shared_ptr<Creature>&)> filter ) const
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


std::shared_ptr<Creature> Terrain::getNearest(Geom::Vec2f Position, float radius, std::function<bool(const std::shared_ptr<Creature>&)> filter ) const
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

std::shared_ptr<Creature> Terrain::getNearest(Geom::Vec2f Position, float radius, int type, std::function<bool(const std::shared_ptr<Creature>&)> filter ) const
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

std::shared_ptr<Creature> Terrain::getNearest(Geom::Vec2f Position, float radius, std::shared_ptr<Species> species,  std::function<bool(const std::shared_ptr<Creature>&)> filter ) const
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

void Terrain::CreateParallelisationGraph()
{
	using namespace Geom;

	unsigned int currentID = 0;
	unsigned int idsAssigned = 0;

	int maxreach = Engine::getCfg()->get<int>("sim.creatureActionRadius");
	bool minimizeParallelRuns = Engine::getCfg()->get<bool>("sim.minimizeParallelRuns");

	Colors.clear();

	while ( idsAssigned < Tiles.size())
	{
		currentID++;
		Colors.emplace_back();

		for ( auto Tile = Tiles.begin(); Tile != Tiles.end(); ++Tile )
		{
			// already set in another color?
			if ( (*Tile)->getParallelId() != 0 ) continue;

			// if not check all other tiles before this one for overlapping
			for ( auto Tile2 = Tiles.begin(); Tile2 != Tile; ++Tile2 )
			{
				// only consider tiles of the same id
				if ((*Tile2)->getParallelId() != currentID) continue;

				Point points1[4];
				Point points2[4];
				decompose( points2, Rect((*Tile2)->getPosition(), (*Tile2)->getPosition()+1 ));

				if ( minimizeParallelRuns )
				{
					// we dont need the points of p1 here, but the corner points of the maxreach x maxreach sized rectangle
					Point points2[4];
					points1[0] = (*Tile)->getPosition();
					points1[1] = (*Tile)->getPosition() + Point( maxreach, 0 );
					points1[2] = (*Tile)->getPosition() + maxreach;
					points1[3] = (*Tile)->getPosition() + Point( 0, maxreach );
				}
				else
				{
					decompose( points1, Rect((*Tile )->getPosition(), (*Tile )->getPosition()+1 ));
				}

				for ( int i = 0; i < 4; ++i)
					for ( int j = 0; j < 4; ++j )
						//if ( std::abs( points1[i].x - points2[j].x) < maxreach || std::abs( points1[i].y - points2[j].y) < maxreach )
						if ( distance( points1[i], points2[j]) < maxreach || (std::abs( points1[i].x - points2[j].x) < maxreach && std::abs( points1[i].y - points2[j].y) < maxreach) )
						{
							//Engine::out() << "ID: " << currentID << "/" << (*Tile2)->getParallelId() << " Tile: " << (*Tile)->getPosition() << " check: " << (*Tile2)->getPosition() << " failed! " << points1[i] << " -> " << points2[j] << " = " << distance( points1[i], points2[j]) << " dx: " << std::abs( points1[i].x - points2[j].x) << " dy: " << std::abs( points1[i].y - points2[j].y) << std::endl;
							goto nexttile;
						}

			}

			// all distance checks were succefull
			if (minimizeParallelRuns)
			{
				Vec2 Pos = (*Tile)->getPosition();

				for (int x = Pos.x; x < Pos.x + maxreach; ++x)
				{
					if ( x >= Size.x ) break;
					for (int y = Pos.y; y < Pos.y + maxreach; ++y)
					{
						if ( y >= Size.y ) break;

						Tiles[ linear(x,y, Size.x) ]->setParallelId( currentID );
						Colors.back().push_back( Tiles[ linear(x,y, Size.x) ] );
						idsAssigned++;
					}
				}
			}
			else
			{
				(*Tile)->setParallelId( currentID );
				Colors.back().push_back( *Tile );
				idsAssigned++;
			}


			nexttile:;
		}


	}

	Engine::out() << "Parallelisation: " << Colors.size() << " Colors/IDs" << std::endl;
	for ( int i = 0; i < Colors.size(); ++i )
		Engine::out() << "    ID:" << i+1 << " - Tiles: " << (Colors.begin()++)->size() << std::endl;

}

void Terrain::CreateMapPlotters()
{

	std::vector<float> population;
	std::vector<float> height;
	std::vector<float> humidity;
	std::vector<float> nutrition;
	std::vector<float> parallelIds;


	for ( std::shared_ptr<Tile>& T : Tiles )
	{
		nutrition.push_back(T->getNutrition() );
		humidity.push_back(T->getCurrentHumidity() );
		population.push_back(T->Creatures.size());
		height.push_back( T->getHeight() );
		parallelIds.push_back( T->getParallelId() );
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

	p.reset( new sbe::MapPlotter( "Nutrition", sbe::MapPlotter::PLOT_HEATMAP) );
	p->setData( nutrition, Size, true );
	p->plot();
	e.SetData( p );
	Module::Get()->QueueEvent( e, true );

	p.reset( new sbe::MapPlotter("Heightmap") );
	p->setData( height, Size, true );
	p->plot();
	e.SetData( p );
	Module::Get()->QueueEvent( e, true );

	p.reset( new sbe::MapPlotter("Parallelisation", sbe::MapPlotter::PLOT_HEATMAP) );
	p->setData( parallelIds, Size, true );
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
	std::uniform_real_distribution<float> nutritionrnd(Engine::getCfg()->get<float>("sim.terragen.debug.nutrition.min"),
														Engine::getCfg()->get<float>("sim.terragen.debug.nutrition.max"));


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

			if( TileHeight < waterlimit ) Humidity = 1;

			Tile *tmp = new Tile( Geom::Point(x,y), TileHeight, nutritionrnd(gen), Humidity );
			std::shared_ptr<Tile> T(tmp);
			Tiles.push_back ( T );
		}
	}
}

void Terrain::UpdateTileMap()
{
	maxElevation = 0;
	for ( int y = 0; y < Size.y; ++y)
		for ( int x = 0; x < Size.x; ++x)
		{
			float h = getTile(Geom::Vec2f(x,y))->getHeight();
			if ( h > maxElevation ) maxElevation = h;
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
