#ifndef TILE_H
#define TILE_H

#include "sbe/geom/Point.hpp"
#include "Species.hpp"

#include <list>
#include <unordered_map>

class Creature;

/// A single Tile on the Terrain
class Tile
{
	public:

		Tile( Geom::Point _Position, float _height, float _nutrition, float _baseHumidity );

		float getHeight(){return height;}
		float getNutrition(){return nutrition;}
		float getBaseHumidity(){return baseHumidity;}
		float getHumidity(){return humidity;}
		Geom::Point getPosition() { return Position; }

		float calcTemperature();
		Geom::Pointf getCenter() { return Geom::Pointf(Position.x+0.5, Position.y+0.5); }
		float getHabitability(int food, std::shared_ptr<Species> sp)	;
		bool isWater() {return baseHumidity > 0.95;}
		int getTileSpriteIndex();

		void addCreature ( const std::shared_ptr<Creature>& p );
		void removeCreature( const std::shared_ptr<Creature>& p);
		std::list<std::shared_ptr<Creature>>& getCreatures() { return Creatures; }

		/// called by the terrain on construction
		static void loadConfigValues();

	private:

		friend class Terrain;
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
		std::list<std::shared_ptr<Creature>> Types[3];
		std::unordered_map<std::shared_ptr<Species>, std::list<std::shared_ptr<Creature>>> SpeciesList;


		static float maxsandheight;
		static float maxgrassheight;
		static float maxwalkableHumidity;
		///temperature decreases by approximately 0.007°C per meter
		static float tempdiffpermeter;

};


#endif // TILE_H

