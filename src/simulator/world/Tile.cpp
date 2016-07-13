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


/**
 * copies everyting except the Creature lists
 * those have to be updated using Creature::updateTileFromPos
 * or addCreature()
 */
Tile::Tile(const Tile &o)
	: Position(o.Position),
	  height(o.height),
	  nutrition(o.nutrition),
		baseHumidity(o.baseHumidity),
		usedNutrition(usedNutrition),
		currentHumidity(o.currentHumidity),
		parallelID(o.parallelID)
{
}

void Tile::loadConfigValues()
{
	// set some const values
	tempdiffpermeter = Engine::getCfg()->get<float>("sim.terrain.tempdiffpermeter");
	maxsandheight = Engine::getCfg()->get<float>("sim.terrain.maxsandheight");
	maxgrassheight = Engine::getCfg()->get<float>("sim.terrain.maxgrassheight");
	maxwalkableHumidity = Engine::getCfg()->get<float>("sim.terrain.maxwalkablehumidity");
}

Tile::Tile( glm::ipoint2 _Position, float _height, float _nutrition, float _baseHumidity )
 : Position(_Position),
  height(_height),
  nutrition( _nutrition ),
  usedNutrition(0),
  baseHumidity( _baseHumidity ),
  parallelID(0),
  currentHumidity( _baseHumidity ),
  Creatures(std::list<std::shared_ptr<Creature>>()),
  SpeciesList(std::unordered_map<std::shared_ptr<Species>, std::list<std::shared_ptr<Creature>>>())
{
}

float Tile::calcTemperature() const
{
	return Simulator::GetTerrain()->getGlobalTemp() - tempdiffpermeter*height;
}

float Tile::getHabitability(const std::shared_ptr<Species>& sp) const
{
	if(isWater()) return 0;

	float wReq = sp->getWaterRequirement();
	float hum = (currentHumidity >= wReq) ? 1 : currentHumidity/wReq;
	float tmp = calcTemperature();
	float r1 = 10000 / ( 1 + pow( (tmp - (float)sp->getOptimalTemperature() ),2 ) );
	float ret = r1 * hum;

	return ret;
}

int Tile::getTileSpriteIndex() const
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
	usedNutrition = Types[Species::HERBA].size() * Creature::NutritionValue;
}



void Tile::removeCreature( const std::shared_ptr<Creature>& p)
{
	Creatures.remove(p);
	Types[p->getSpecies()->getType()].remove(p);
	SpeciesList[p->getSpecies()].remove(p);
	usedNutrition = Types[Species::HERBA].size() * Creature::NutritionValue;
}
