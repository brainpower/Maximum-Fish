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

		Tile( glm::ipoint2 _Position, float _height, float _nutrition, float _baseHumidity );
		Tile(const Tile &o);

		float getHeight() const {return height;}
		float getNutrition() const {return nutrition;}
		float getUsedNutrition() const {return usedNutrition;}
		float getBaseHumidity() const {return baseHumidity;}
		float getCurrentHumidity() const {return currentHumidity;}
		/// return the percentage of availablenutrition/usednutrition, but no more than 100%
		inline float getFoodSupply() const { return nutrition/usedNutrition > 1 ? 1 : nutrition/usedNutrition; }

		glm::ipoint2 getPosition() const { return Position; }

		float calcTemperature() const;
		glm::point2 getCenter() const { return glm::point2(Position.x+0.5, Position.y+0.5); }
		float getHabitability(const std::shared_ptr<Species>& sp) const;
		bool isWater() const {return currentHumidity >= Tile::maxwalkableHumidity;}
		int getTileSpriteIndex() const;

		void setNutrition(float n) { nutrition = n; }
		void setBaseHumidity(float bh) { baseHumidity = bh; currentHumidity = bh; }
		void setCurrentHumidity(float ch) { currentHumidity = ch; }

		void addCreature ( const std::shared_ptr<Creature>& p );
		void removeCreature( const std::shared_ptr<Creature>& p);
		std::list<std::shared_ptr<Creature>>& getCreatures() { return Creatures; }

		/// called by the terrain on construction
		static void loadConfigValues();

		unsigned int getParallelId() const { return parallelID; }
		void setParallelId(unsigned int p) { parallelID = p;}

	private:

		friend class Terrain;
		friend class TerrainIOPlugin;

		glm::ipoint2 Position;

		float height;
		/// general nutrition value on that tile ( required for plant growth
		float nutrition;
		/// general humidity on that tile
		float baseHumidity;

		/// currently required Nutrition to fully support all plants
		float usedNutrition;
		/// current humidity, decreased through Weather or global settings
		float currentHumidity;

		/// all Tiles with the same ID can be simulated in parallel
		unsigned int parallelID;

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
