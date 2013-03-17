#include "Tile.hpp"
#include "Terrain.hpp"
#include "simulator/Simulator.hpp"
#include "Creature.hpp"

#include <memory>

#include "sbe/Config.hpp"



const float Tile::d = 0.007;
float Tile::maxsandheight = 0;
float Tile::maxgrassheight = 0;
float Tile::maxwalkableHumidity = 0;

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
	return Simulator::GetTerrain()->getGlobalTemp() - d*height;
}

float Tile::getHabitability(int food, std::shared_ptr<Species> sp)
{
	float hum = getBaseHumidity();
	float wReq = sp->getWaterRequirement();

	//if(hum > Engine::getCfg()->get<float>("sim.terrain.maxwalkablehumidity"))
	if(hum > Tile::maxwalkableHumidity)
		return 0;

	if(hum >= wReq)
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

int Tile::getTileSpriteIndex()
{
	if(isWater()) return 0;

	float maxElev = Simulator::GetTerrain()->getMaxElevation();
	if(height < maxElev * Tile::maxsandheight) return 1;
	if(height < maxElev * Tile::maxgrassheight) return 2;
	return 3;
}

void Tile::addCreature ( const std::shared_ptr<Creature>& p )
{
	Creatures.push_back(p);
	Types[p->getSpecies()->getType()].push_back(p);
	SpeciesList[p->getSpecies()].push_back(p);
}



void Tile::removeCreature( const std::shared_ptr<Creature>& p)
{
	Creatures.remove(p);
	Types[p->getSpecies()->getType()].remove(p);
	SpeciesList[p->getSpecies()].remove(p);
}
