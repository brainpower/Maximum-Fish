#ifndef TERRAIN_H
#define TERRAIN_H

#include "sbe/Geom.hpp"

#include "Tile.hpp"

class Terrain
{
	public:

	int getTileAltitude(Geom::Vec2f pos);
	Tile* getTile( Geom::Vec2f pos );
	//
	void setHumidityFactor( const float hf ) { humidity_factor = hf;}

	private:

		// calculate Rainmap
		void calculateHumidity();

		float humidity_factor;
		float global_temp;              //global temperature multiplier,
		int max_elevation;              //highest elevation (in m) on the map, lowest is 0 (sea level)

		std::vector<std::vector<Tile>> Tiles;

};

#endif // TERRAIN_H
