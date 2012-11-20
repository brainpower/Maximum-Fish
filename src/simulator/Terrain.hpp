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
	void setHumidityFactor( float humidity_factor );

	private:

		// calculate Rainmap
		void calculateHumidity();

		float humidity_factor;

		std::vector<std::vector<Tile>> Tiles;

};

#endif // TERRAIN_H

