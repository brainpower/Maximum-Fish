#ifndef CREATURE_HPP
#define CREATURE_HPP

#include <memory>

#include "sbe/geom/Point.hpp"

class Species;
class Tile;

/**
	The Creature class is the main scaff for simulation, this is where also the basic stuff is initialized.
	Also basic AI calculations are happening here.
*/
class Creature : public std::enable_shared_from_this<Creature>
{
	public:
		Creature( const std::shared_ptr<Species>& Species);
		virtual ~Creature() {};

		void live();

		void setCurrentHealth(const float ch){ currentHealth = ch;}
		void setCurrentTile(const std::shared_ptr<Tile> t){ currentTile = t;}
		void setAge(const int a){ age = a; }
		/// set a new position and update the list of creatures in the affected Tiles
		void setPosition(const Geom::Pointf& pos);
		/// set a new position without updating the list of creatures in the affected Tiles
		/// used if you just want to create a creature without affecting the simulator state ( yet )
		void setPositionUnsafe(const Geom::Pointf& pos);
		void movePosition(const Geom::Pointf& pos);

		float getCurrentHealth() const { return currentHealth; }
		int getAge() const { return age; }
		const std::string& getSpeciesString() const;
		const std::shared_ptr<Species>& getSpecies() const {return mySpecies;};
		const Geom::Vec2f& getPosition() const { return Position; }
		const std::shared_ptr<Tile>& getTile() const {return currentTile;}

		bool validPos( Geom::Pointf NewPosition );

		// neccessary to update currentTile after loading from "savegame"
		void updateTileFromPos();

		static void loadConfigValues();

	private:

		// static settings loaded from the config
		static float pNutritionDiv;
		static float huntingThreshold;
		static float matingThreshold;
		static float matingAge;
		static float matingHealthCost;
		static float migProb;
		static float altModifier1;
		static float altModifier2;
		static float envMult;
		// -- END STATIC SETTINGS --



		friend class CreatureIOPlugin;
		void huntFood();
		/// handles eating the nearest possible prey defined by the filter lambda
		void huntNearest( std::function< bool( std::shared_ptr<Creature> ) > filter );
		void mateNearest( std::function< bool( std::shared_ptr<Creature> ) > filter );
		void mate();
		void move(int found);
		bool moveYourAss();
		/**
			Calculates environmental effects (damage from temperature etc)
		*/
		void calcEnv();


		/// describes the current health of the Creature ( max is species->maxage, usually around 100 )
		float currentHealth;
		int age;
		Geom::Vec2f Position;

		//References
		std::shared_ptr<Species> mySpecies;
		std::shared_ptr<Tile> currentTile;
};


#endif // CREATURE_HPP
