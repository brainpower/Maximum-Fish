#include "Tile.hpp"
#include "Terrain.hpp"
#include "Simulator.hpp"
#include <memory>

#include "Creature.hpp"

Tile::Tile( Geom::Point _Position, float _height, float _nutrition, float _baseHumidity )
 : Position(_Position),
  height(_height),
  nutrition( _nutrition ),
  baseHumidity( _baseHumidity ),
  biomass(0),
  humidity(0)
{

}

float Tile::calcTemperature()
{
	int n = 255;		//heightmap: 256 possible values, therefore 255 needed to calculate the difference in elevation when brightness changes by 1
	float d = 0.007;	//temperature decreases by approximately 0.007°C per meter

	std::shared_ptr<Terrain> t = Simulator::GetTerrain();
	int max_h = t->getMaxElevation();
	float t_0 = t->getGlobalTemp();
	float temp_h = t_0 - ((max_h/n)*d)*height;

	return temp_h;
}

float Tile::getHabitability(int food, std::shared_ptr<Species> sp)
{
	float hum = getBaseHumidity();
	float wReq = sp->getWaterRequirement();

	if(hum > 0.95)
	{
		return 0;
	}
	else if(hum >= wReq)
	{
		hum = 1;
	} else {
		hum = hum/wReq;
	}

	float tmp = calcTemperature();
	//float consp = getNumConspecifics(sp); << hasn't been implemented yet
	float r1 = (10000/(1+pow((tmp-(float)sp->getOptimalTemperature()),2)));
	float r2 = (1-(1/(1+(float)food)));

	//std::cout << "r1: " << r1 << " r2: " << r2 << " hum: " << hum << std::endl;

	float ret = /* (1-(1/(1+Artgenossen))) * */ r1 * r2 * 1/*hum*/;

	return ret;
}

int Tile::getNumConspecifics(std::shared_ptr<Species> sp, int radius)
{
	int num = 0;
	std::vector<std::shared_ptr<Tile>> tiles = Simulator::GetTerrain()->get_nearby(*this, radius);
	for(std::shared_ptr<Tile> t : tiles)
	{
		for(std::shared_ptr<Creature> c : t->getCreatures())
		{
			if(c->getSpecies() == sp)
			{
				num++;
			}
		}
	}
	return num;
}

int Tile::getTileSpriteIndex()
{
	if(this->isWater()) return 0;
	
	float maxElev = Simulator::GetTerrain()->getMaxElevation();
	if(this->height < maxElev * .05) return 1;
	if(this->height < maxElev * .9) return 3;
	return 2;
}

void Tile::addCreature ( const std::shared_ptr<Creature>& p )
{
	Creatures.push_back(p);
}

void Tile::removeCreature( const std::shared_ptr<Creature>& p)
{
	Creatures.remove( p );
}
