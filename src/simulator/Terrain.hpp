#ifndef TERRAIN_H
#define TERRAIN_H

#include <list>
#include <memory>

#include "sbe/Geom.hpp"
#include "Tile.hpp"


class Terrain
{
	public:

	void update_terrain();
	
	int getTileElevation(Geom::Vec2f pos);
	Tile* getTile( Geom::Vec2f pos );
	int getMaxElevation();
	float getGlobalTemp();
	void setHumidityFactor( const float hf ) { humidity_factor = hf;}

	private:

		// calculate Rainmap
		void calculateHumidity();

		float humidity_factor;
		float global_temp;              //temperature at sea level
		int max_elevation;              //highest elevation (in m) on the map, lowest is 0 (sea level)

		std::list<std::shared_ptr<Tile>> m_tiles;

};

#endif // TERRAIN_H
