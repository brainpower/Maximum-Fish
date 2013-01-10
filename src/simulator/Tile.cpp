#include "Tile.hpp"
#include "Terrain.hpp"
#include "Simulator.hpp"
#include <memory>

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
	float hum = getHumidity();
	float tmp = calcTemperature();
	float ret = ((float)food/(float)sp->getFoodRequirement())*(hum/sp->getWaterRequirement())*(1/pow(tmp - sp->getOptimalTemperature(),2));
	return ret;
}
