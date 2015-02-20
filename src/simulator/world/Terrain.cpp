#include "Terrain.hpp"

#include <sbe/event/Event.hpp>
#include <sbe/Module.hpp>

#include <sbe/geom/Helpers.hpp>
#include <sbe/geom/PointHelpers.hpp>
#include <sbe/geom/RectHelpers.hpp>
#include <glm/gtx/norm.hpp>

#include <sbe/Config.hpp>

#include <sbe/gfx/MapPlotter.hpp>

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
   globalTemp(o.globalTemp),
   InvalidTile()
{

	for( const std::shared_ptr<Tile> &t : o.Tiles) { // deep copy of tiles
		Tiles.push_back(std::shared_ptr<Tile>(new Tile(*t)));
	}

	CreateParallelisationGraph();
}

const std::shared_ptr<Tile>& Terrain::getTile( glm::vec2 pos ) const
{
	unsigned int index = geom::linear(pos, Size.x);
	if (!validPos(pos) || index >= Tiles.size()) return InvalidTile;
	return Tiles[ index ];
}

float Terrain::getTileElevation(glm::vec2 pos) const
{
	unsigned int index = geom::linear(pos, Size.x);
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
			auto _T = getTile( glm::vec2(x,y) );
			if (!_T || (x = T.getPosition().x && y == T.getPosition().y)) break;

			ret.push_back(_T);
		}
	}

	return ret;
}

std::list<std::shared_ptr<Creature>> Terrain::getNearby(glm::vec2 Position, float radius, std::function<bool(const std::shared_ptr<Creature>&)> filter ) const
{
	std::list<std::shared_ptr<Creature>> ret;

	// we're using squared distances
	float r2 = radius*radius;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( glm::vec2(x,y) );
			if (!T) break;

			for (std::shared_ptr<Creature>& C : T->Creatures)
			{
				if ( glm::distance2( C->getPosition(), Position ) < r2 && filter ( C ) ) ret.push_back(C);
			}
		}
	}

	return ret;
}

std::list<std::shared_ptr<Creature>> Terrain::getNearby(glm::vec2 Position, float radius, int type, std::function<bool(const std::shared_ptr<Creature>&)> filter ) const
{
	std::list<std::shared_ptr<Creature>> ret;

	// we're using squared distances
	float r2 = radius*radius;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( glm::vec2(x,y) );
			if (!T) break;

			for (std::shared_ptr<Creature>& C : T->Types[type])
			{
				if ( glm::distance2( C->getPosition(), Position ) < r2 && filter ( C ) ) ret.push_back(C);
			}
		}
	}

	return ret;
}

std::list<std::shared_ptr<Creature>> Terrain::getNearby(glm::vec2 Position, float radius, std::shared_ptr<Species> species,  std::function<bool(const std::shared_ptr<Creature>&)> filter ) const
{
	std::list<std::shared_ptr<Creature>> ret;

	// we're using squared distances
	float r2 = radius*radius;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( glm::vec2(x,y) );
			if (!T) break;

			for (std::shared_ptr<Creature>& C : T->SpeciesList[species])
			{
				if ( glm::distance2( C->getPosition(), Position ) < r2 && filter ( C ) ) ret.push_back(C);
			}
		}
	}

	return ret;
}


std::shared_ptr<Creature> Terrain::getNearest(glm::vec2 Position, float radius, std::function<bool(const std::shared_ptr<Creature>&)> filter ) const
{
	std::shared_ptr<Creature> nearest;

	// squared distance, avoid squrt in further comparisions
	float mindist2 = radius*radius;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( glm::vec2(x,y) );
			if (!T || (x==Position.y && y == Position.y)) continue;

			for (std::shared_ptr<Creature>& C : T->Creatures)
			{
				float curdist = glm::distance2( C->getPosition(), Position );
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

std::shared_ptr<Creature> Terrain::getNearest(glm::vec2 Position, float radius, int type, std::function<bool(const std::shared_ptr<Creature>&)> filter ) const
{
	std::shared_ptr<Creature> nearest;

	// squared distance, avoid squrt in further comparisions
	float mindist2 = radius*radius;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( glm::vec2(x,y) );
			if (!T || (x==Position.y && y == Position.y)) continue;

			for (std::shared_ptr<Creature>& C : T->Types[type])
			{
				float curdist = glm::distance2( C->getPosition(), Position );
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

std::shared_ptr<Creature> Terrain::getNearest(glm::vec2 Position, float radius, std::shared_ptr<Species> species,  std::function<bool(const std::shared_ptr<Creature>&)> filter ) const
{
	std::shared_ptr<Creature> nearest;

	// squared distance, avoid squrt in further comparisions
	float mindist2 = radius*radius;

	for (int x = Position.x-radius; x < Position.x+radius; ++x)
	{
		for (int y = Position.y-radius; y < Position.y+radius; ++y)
		{
			std::shared_ptr<Tile> T = getTile( glm::vec2(x,y) );
			if (!T || (x==Position.y && y == Position.y)) continue;

			for (std::shared_ptr<Creature>& C : T->SpeciesList[species])
			{
				float curdist = glm::distance2( C->getPosition(), Position );
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
	int maxreach = Engine::getCfg()->get<int>("sim.creatureActionRadius");

	Colors.clear();
	Colors.resize(4);

	for( int y = 0; y < Size.y; y+=maxreach)
	{
		for ( int x = 0; x < Size.x; x+=maxreach)
		{
			int col = ((x)/maxreach) % 2;
			int odd = ((y)/maxreach) % 2;
			int id  = (2*odd)+col;

			int xx,yy, count = 0;
			for (yy = 0; yy < maxreach; ++yy)
			{
				if (y+yy >= Size.y) break;
				for (xx = 0; xx < maxreach; ++xx)
				{
					if (x+xx >= Size.x) break;

					getTile({ x+xx,y+yy })->setParallelId( id );
					Colors[ id ].push_back( getTile({ x+xx,y+yy }) );
					count++;
				}
			}

			// acts as a divider between the squares of this color
			Colors[ id ].emplace_back();
//			Engine::out() << "Color[" << id << "] " << x << "," << y << " size " << xx << "," << yy << " c: " << count << " . Size: " << Colors[id].size() << std::endl;
		}
	}

//	Engine::out() << "Parallelisation: " << Colors.size() << " Colors/IDs" << std::endl;
//	int i = 0;
//	for ( auto it = Colors.begin(); it != Colors.end(); ++it )
//		Engine::out() << "    ID:" << ++i << " - Tiles: " << it->size() << std::endl;

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

void Terrain::CreateDebugTerrain( int seed )
{
	Tiles.clear();
	{
		int tmp = Engine::getCfg()->get<float>("sim.terragen.debug.size");
		Size = glm::ivec2( tmp, tmp );
	}
	// maximum height to generate
	float maxHeight = Engine::getCfg()->get<float>("sim.terragen.debug.maxheight");
	//float minheight = 0;

	float maxFallofDist = Size.x/2;

	float waterlimit = Engine::getCfg()->get<float>("sim.terragen.debug.waterlimit");

	glm::point2 Mid = glm::point2( Size.x/2, Size.y/2 );


	std::mt19937 gen(seed);
	std::uniform_real_distribution<float> rnd;
	std::uniform_real_distribution<float> nutritionrnd(Engine::getCfg()->get<float>("sim.terragen.debug.nutrition.min"),
														Engine::getCfg()->get<float>("sim.terragen.debug.nutrition.max"));


	float minHumidity = Engine::getCfg()->get<float>("sim.terragen.debug.humidity.min");
	float maxHumidity = Engine::getCfg()->get<float>("sim.terragen.debug.humidity.max");


	for ( int y = 0; y < Size.y; ++y)
	{
		for ( int x = 0; x < Size.x; ++x)
		{
			glm::point2 TileMid = glm::point2( x+.5, y+.5 );
			float HeightFactor = (1 - glm::distance( TileMid, Mid )/maxFallofDist );
			HeightFactor = HeightFactor < 0 ? 0 : HeightFactor;
			float TileHeight = maxHeight*HeightFactor;

			Tile *tmp = new Tile( glm::ipoint2(x,y), TileHeight, nutritionrnd(gen), ((TileHeight < waterlimit)?1:0));
			std::shared_ptr<Tile> T(tmp);
			Tiles.push_back ( T );
		}
	}

	calculateHumidity();
}

void Terrain::calculateHumidity()
{
	float tilesize = Engine::getCfg()->get<float>("sim.terragen.debug.tilesizeinmeters");
	float _amount = Engine::getCfg()->get<float>("sim.terragen.debug.humidity.rainamount");
	float _rainfall = Engine::getCfg()->get<float>("sim.terragen.debug.humidity.rainfall");

	for ( int y = 0; y < Size.y; ++y)
	{
		float amount = _amount;

		for ( int x = 0; x < Size.x ; ++x)
		{

			if ( getTile({x,y})->getBaseHumidity() == 1 ) {amount = _amount; continue;}

			float rainfall = _rainfall;
			float steigung = 0;

			if ( x > 0) {steigung = ( getTile({x,y})->getHeight() - getTile({x-1,y})->getHeight() )/tilesize; }
			else  { steigung = ( getTile({x+1,y})->getHeight() - getTile({x,y})->getHeight() )/tilesize; }
			//steigung = std::sqrt( steigung );
			if ( steigung > 0 ) rainfall = rainfall * (1+steigung);

			float tmp = amount*rainfall;
			if ( tmp >= 1 ) tmp = 0.99;
			amount -= tmp;

			//Engine::out(Engine::SPAM) << "Pos: " << x << "/"  << y << " Amount/rainfall/tmp:  " << amount << "/" << rainfall << "/" << tmp << " - " << steigung << std::endl;

			getTile({x,y})->setBaseHumidity( tmp );
		}
	}

	/* random version */
	/*
	float minHumidity = Engine::getCfg()->get<float>("sim.terragen.debug.humidity.min");
	float maxHumidity = Engine::getCfg()->get<float>("sim.terragen.debug.humidity.max");


	for ( int y = 0; y < Size.y; ++y)
	{
		for ( int x = 0; x < Size.x; ++x)
		{
			glm::point2 TileMid = glm::point2( x+.5, y+.5 );
			float HeightFactor = (1 - glm::distance( TileMid, Mid )/maxFallofDist );
			HeightFactor = HeightFactor < 0 ? 0 : HeightFactor;
			float Humidity = minHumidity + (maxHumidity-minHumidity)*(1-HeightFactor);
		}
	}
	*/
}

void Terrain::calcMaxElevation()
{
    maxElevation = 0;
	for ( int y = 0; y < Size.y; ++y)
		for ( int x = 0; x < Size.x; ++x)
		{
			float h = getTile(glm::vec2(x,y))->getHeight();
			if ( h > maxElevation ) maxElevation = h;
		}
}

void Terrain::UpdateTileMap()
{
    calcMaxElevation();

	tilemapImage.reset ( new sf::Image);
	tilemapImage->create(Size.x ,Size.y);
	for(int x = 0; x < Size.x; x++)
	{
		for(int y = 0; y < Size.y; y++)
		{
			glm::vec2 pos(x, y);
			sf::Color tile(getTile(pos)->getTileSpriteIndex(),0,0,0);
			tilemapImage->setPixel(x, y, tile);
		}
	}

	Module::Get()->QueueEvent(Event("UpdateTilemapTexture", tilemapImage), true);
}
