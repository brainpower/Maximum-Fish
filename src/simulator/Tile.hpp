#ifndef TILE_H
#define TILE_H

class Tile
{
	public:
		int getElevation(){return height;}
		int getNutrition(){return nutrition;}
		int getBaseHumidity(){return base_humidity;}
		int getHumidity(){return humidity;}
		float calcTemperature();

	private:
		int height;
		int nutrition;
		int base_humidity;

		/// number of plants on that field, used to determine overpopulation
		int biomass;
		int humidity;
};


#endif // TILE_H

