#include "Tile.hpp"
#include "Terrain.hpp"
#include "simulator/Simulator.hpp"
#include "Creature.hpp"

#include <memory>

#include "sbe/Config.hpp"



float Tile::tempdiffpermeter = 0;
float Tile::maxsandheight = 0;
float Tile::maxgrassheight = 0;
float Tile::maxwalkableHumidity = 0;

void Tile::loadConfigValues()
{
	// set some const values
	tempdiffpermeter = Engine::getCfg()->get<float>("sim.terrain.tempdiffpermeter");
	maxsandheight = Engine::getCfg()->get<float>("sim.terrain.maxsandheight");
	maxgrassheight = Engine::getCfg()->get<float>("sim.terrain.maxgrassheight");
	maxwalkableHumidity = Engine::getCfg()->get<float>("sim.terrain.maxwalkablehumidity");
}

Tile::Tile( Geom::Point _Position, float _height, float _nutrition, float _baseHumidity )
 : Position(_Position),
  height(_height),
  nutrition( _nutrition ),
  baseHumidity( _baseHumidity ),
  biomass(0),
  humidity( _baseHumidity )
{

}

float Tile::calcTemperature()
{
	return Simulator::GetTerrain()->getGlobalTemp() - tempdiffpermeter*height;
}

float Tile::getHabitability(const std::shared_ptr<Species>& sp)
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
	float r1 = 10000 / ( 1 + pow( (tmp - (float)sp->getOptimalTemperature() ),2 ) );

	//std::cout << "r1: " << r1 << " r2: " << r2 << " hum: " << hum << std::endl;

	float ret = r1 * hum;

	//Engine::out() << "hab: " << ret << std::endl;

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
