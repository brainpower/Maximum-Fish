#ifndef TILE_H
#define TILE_H

#include "sbe/Geom.hpp"

class Tile
{
	public:

		Tile( Geom::Vec2 Position, int _height, int _nutrition, int _baseHumidity )
		 : height(_height), nutrition( _nutrition ), baseHumidity( _baseHumidity )
		{

		}

		int getHeight(){return height;}
		int getNutrition(){return nutrition;}
		int getBaseHumidity(){return baseHumidity;}
		int getHumidity(){return humidity;}
		float calcTemperature();

	private:

		friend class TerrainIOPlugin;

		Geom::Vec2 Position;

		int height;
		int nutrition;
		int baseHumidity;

		/// number of plants on that field, used to determine overpopulation
		int biomass;
		int humidity;
};


#endif // TILE_H

