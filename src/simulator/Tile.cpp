#include "Tile.hpp"
#include "Terrain.hpp"
#include "Simulator.hpp"
#include <memory>

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