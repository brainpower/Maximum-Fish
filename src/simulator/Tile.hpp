#ifndef TILE_H
#define TILE_H

#include "sbe/Geom.hpp"
#include "Species.hpp"

class Creature;

class Tile
{
	public:

		Tile( Geom::Point _Position, float _height, float _nutrition, float _baseHumidity );

		float getHeight(){return height;}
		float getNutrition(){return nutrition;}
		float getBaseHumidity(){return baseHumidity;}
		float getHumidity(){return humidity;}
		float calcTemperature();
		Geom::Point getPosition() { return Position; }
		float getHabitability(int food, std::shared_ptr<Species> sp);
		int getNumConspecifics(std::shared_ptr<Species> sp, int radius);

		void addCreature ( const std::shared_ptr<Creature>& p );

		void removeCreature( const std::shared_ptr<Creature>& p);

		std::list<std::shared_ptr<Creature>>& getCreatures() { return Creatures; }

	private:

		friend class TerrainIOPlugin;

		Geom::Point Position;

		float height;
		/// nutrition value on that tile ( required for plant growth
		float nutrition;
		/// general humidity on that tile
		float baseHumidity;

		/// number of plants on that field, used to determine overpopulation
		int biomass;
		/// current humidity, decreased through "feeding" plants
		float humidity;

		/// a list of all Creatures on that Tile
		std::list<std::shared_ptr<Creature>> Creatures;

};


#endif // TILE_H

