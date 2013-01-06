#ifndef TILE_H
#define TILE_H

#include "sbe/Geom.hpp"

class Tile
{
	public:

		Tile( Geom::Vec2 _Position, float _height, float _nutrition, float _baseHumidity );

		float getHeight(){return height;}
		float getNutrition(){return nutrition;}
		float getBaseHumidity(){return baseHumidity;}
		float getHumidity(){return humidity;}
		float calcTemperature();

	private:

		friend class TerrainIOPlugin;

		Geom::Vec2 Position;

		float height;
		/// nutrition value on that tile ( required for plant growth
		float nutrition;
		/// general humidity on that tile
		float baseHumidity;

		/// number of plants on that field, used to determine overpopulation
		int biomass;
		/// current humidity, decreased through "feeding" plants
		float humidity;
};


#endif // TILE_H

