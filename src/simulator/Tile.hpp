#ifndef TILE_H
#define TILE_H

class Tile
{
	public:
		int getAltitude(){return height;}
		int getNutrition(){return nutrition;}
		int getBaseHumidity(){return base_humidity;}
		int getHumidity(){return humidity;}

	private:
		int height;
		int nutrition;
		int base_humidity;

		/// number of plants on that field, used to determine overpopulation
		int biomass;
		int humidity;
};


#endif // TILE_H

