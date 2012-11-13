#ifndef TERRAIN_H
#define TERRAIN_H

class Tile;

class Terrain
{





	//
	void setHumidityFactor( float humidity_factor );

	private

		// calculate Rainmap
		void calculateHumidity();

		float humidity_factor;
		std::vector<std::vector<Tile>> Tiles;

}

#endif // TERRAIN_H

