#ifndef CREATURE_HPP
#define CREATURE_HPP

#include <memory>
#include <cmath>

#include "sbe/geom/Point.hpp"

#include "Species.hpp"
#include "Tile.hpp"

class Terrain;

/**
	The Creature class is the main scaff for simulation, this is where also the basic stuff is initialized.
	Also basic AI calculations are happening here.
*/
class Creature : public std::enable_shared_from_this<Creature>
{
	public:
		Creature( const std::shared_ptr<Species>& Species);
		Creature( const Creature &o);
		virtual ~Creature() {};
		enum CauseOfDeath {EATEN = 0, STARVED, OLD, FROZEN, THIRST, NONE};
        int getCauseOfDeath() { return causeOfDeath; }

		void live();

		void setCurrentHealth(const float ch){ currentHealth = ch;}
		void setCurrentTile(const std::shared_ptr<Tile> &t){ currentTile = t;}
		void setAge(const int a){ age = a; }
        void setLastMating( const int lm ) { lastmating = lm; }
		void recalcAgeFactor() { curAgeFactor = ageFactor(); }

		/// set a new position and update the list of creatures in the affected Tiles
		void setPosition(const glm::point2& pos);
		/// set a new position without updating the list of creatures in the affected Tiles
		/// used if you just want to create a creature without affecting the simulator state ( yet )
		void setPositionUnsafe(const glm::point2& pos);
		void movePosition(const glm::point2& pos);
		void die(CauseOfDeath cod);

		float getCurrentHealth() const { return currentHealth; }
		int getAge() const { return age; }
        int getLastMating( const int lm ) { return lastmating; }
		const std::string& getSpeciesString() const;
		const std::shared_ptr<Species>& getSpecies() const {return mySpecies;};
		const glm::vec2& getPosition() const { return Position; }
		const std::shared_ptr<Tile>& getTile() const {return currentTile;}

		//## Calculated Values ##
		/*
			!!!!!!!!!!!!!!!!!!!!!!   README    !!!!!!!!!!!!!!!!!!!!!!!
			THESE FUNCTIONS ARE JUST CALCULATIONS ( OUTSOURCED FORMULAS ) AND SHOULD NEVER CHANGE THE CREATURE DIRECTLY
			IT IS ASSUMED THESE CAN BE CALLED FROM THE OUTSIDE WITHOUT MODIFYING THE CREATURE.
		*/



		inline float ageFactor() const
		{
			static const float a = 0.6;
			//static const float b = 0.164993; // a=0.4
			static const float b = 0.0888669; // a=0.6
			//static const float b = 0.0378908; // a=0.8

			float ma = mySpecies->getMaxAge();
			float re = -a*std::pow( (2*(age+0.5*b*ma) - ma) / ma, ageExponent) + 1;
			return re > 0 ? re : 0;
		}
		inline float currentMaxHealth() const { return mySpecies->getMaxHealth()*currentResistance(); }
		inline float healthPercentage() const { return currentHealth/currentMaxHealth(); }
		inline float currentResistance() const { return mySpecies->getResistance() * curAgeFactor; }
		inline float minAge() const { return mySpecies->getMaxAge()*matingAge; }
		inline float mHealthCost() const { return currentMaxHealth() * matingHealthCost; }
		inline float currentMaxSpeed() const { return mySpecies->getMaxSpeed() * currentResistance(); }

		inline float envDamage() const {
			float envDmg = std::pow( (currentTile->calcTemperature() - mySpecies->getOptimalTemperature()) / altModifier1, altModifier2);
			if ( mySpecies->getType() == Species::HERBA ) envDmg = envDmg*plantEnvDmgFactor;
			envDmg = envDmg/currentResistance() * envMult;
			return envDmg;
		}
		inline float waterSupply() const { return currentTile->getCurrentHumidity()/mySpecies->getWaterRequirement(); }
		inline float waterDamage() const {
			return waterSupply()<1 ? mySpecies->getMaxHealth()*mySpecies->getWaterRequirement() * std::pow(1 - waterSupply(), 3) : 0;
		}
		inline float foodDamage() const {
			return mySpecies->getMaxHealth()*mySpecies->getFoodRequirement()*currentResistance();
		}

		//'' END Calculated Values ##

		bool validPos( glm::point2 NewPosition ) const ;

		// neccessary to update currentTile after loading from "savegame"
		void updateTileFromPos();
		void updateTileFromPos(std::shared_ptr<Terrain> t);

		static void loadConfigValues();

		/// true if this Creature already had its turn this tick
		bool done;

		// static settings but used in Tile
		static float NutritionValue;

	private:

		// static settings loaded from the config
		static float huntingThreshold;

		static float matingThreshold;
		static float matingAge;
		static float matingHealthCost;

		static float migProb;
		static float altModifier1;
		static float altModifier2;

		static float envMult;
		static float plantEnvDmgFactor;
		static float maxAngle;

		static float resistance;
		static int   ageExponent;

		static float fleeFactor;
		// -- END STATIC SETTINGS --

		//defines when a creature is "old" (in relation to maxAge)
		const float old = 0.9;

		friend class CreatureIOPlugin;

		bool huntFood();
		/// handles eating the nearest possible prey defined by the filter lambda
		bool huntNearest( int type  );

		bool mate();
		/// spawn a child
		void reproduce( std::shared_ptr<Creature> otherparent = std::shared_ptr<Creature>());

		void move();
		bool moveTo(const glm::vec2 Target);
		bool randomMove();
		glm::vec2 getNewPosition();
		/**
			Calculates environmental effects (damage from temperature, water/food requirement)
		*/
		void calcDamage();

		/// describes the current health of the Creature ( max is species->maxage, usually around 100 )
		float currentHealth;
		float curAgeFactor;
		int age;
		int lastmating;
        int causeOfDeath;
		glm::vec2 Position;
		glm::vec2 prevMove;

		//References
		std::shared_ptr<Species> mySpecies;
		std::shared_ptr<Tile> currentTile;
};


#endif // CREATURE_HPP
