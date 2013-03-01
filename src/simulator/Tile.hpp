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
		Geom::Point getPosition() { return Position; }

		float calcTemperature();
		Geom::Pointf getCenter() { return Geom::Pointf(Position.x+0.5, Position.y+0.5); }
		float getHabitability(int food, std::shared_ptr<Species> sp)	;
		bool isWater() {return baseHumidity > 0.95;}
		int getTileSpriteIndex();

		void addCreature ( const std::shared_ptr<Creature>& p );
		void removeCreature( const std::shared_ptr<Creature>& p);
		std::list<std::shared_ptr<Creature>>& getCreatures() { return Creatures; }

		/// those are set by the terrain on construction
		static float maxsandheight;
		static float maxgrassheight;
		static float maxwalkableHumidity;

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

		///temperature decreases by approximately 0.007°C per meter
		static const float d;

};


#endif // TILE_H

