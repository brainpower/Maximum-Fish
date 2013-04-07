#ifndef CREATURE_HPP
#define CREATURE_HPP

#include <memory>
#include <cmath>

#include "sbe/geom/Point.hpp"

#include "Species.hpp"
#include "Tile.hpp"

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
		void die();

		float getCurrentHealth() const { return currentHealth; }
		int getAge() const { return age; }
		const std::string& getSpeciesString() const;
		const std::shared_ptr<Species>& getSpecies() const {return mySpecies;};
		const Geom::Vec2f& getPosition() const { return Position; }
		const std::shared_ptr<Tile>& getTile() const {return currentTile;}

		//## Calculated Values ##
		inline float ageFactor()
		{
			static const float a = 0.6;
			//static const float b = 0.164993; // a=0.4
			static const float b = 0.0888669; // a=0.6
			//static const float b = 0.0378908; // a=0.8

			float ma = mySpecies->getMaxAge();
			float re = -a*std::pow( (2*(age+0.5*b*ma) - ma) / ma, ageExponent) + 1;
			return re > 0 ? re : 0;
		}
		inline float currentMaxHealth() { return mySpecies->getMaxHealth()*currentResistance(); }
		inline float healthPercentage() { return currentHealth/currentMaxHealth(); }
		inline float currentResistance() { return mySpecies->getResistance() * ageFactor(); }
		inline float minAge() { return mySpecies->getMaxAge()*matingAge; }
		inline float mHealthCost() { return currentMaxHealth() * matingHealthCost; }
		inline float currentMaxSpeed() { return mySpecies->getMaxSpeed() * currentResistance(); }

		inline float envDamage() {
			float envDmg = std::pow( (currentTile->calcTemperature() - mySpecies->getOptimalTemperature()) / altModifier1, altModifier2);
			if ( mySpecies->getType() == Species::HERBA ) envDmg = envDmg*plantEnvDmgFactor;
			return envDmg/currentResistance() * envMult;
		}
		inline float waterSupply() { return currentTile->getCurrentHumidity()/mySpecies->getWaterRequirement(); }
		inline float waterDamage() { return  waterSupply()<1 ? mySpecies->getMaxHealth()*mySpecies->getWaterRequirement() * std::pow(1 - waterSupply(), 3) : 0; }
		inline float foodDamage() { return mySpecies->getMaxHealth()*mySpecies->getFoodRequirement()*currentResistance();}

		//'' END Calculated Values ##

		bool validPos( Geom::Pointf NewPosition ) const ;

		// neccessary to update currentTile after loading from "savegame"
		void updateTileFromPos();

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
		// -- END STATIC SETTINGS --

		friend class CreatureIOPlugin;

		bool huntFood();
		/// handles eating the nearest possible prey defined by the filter lambda
		bool huntNearest( int type  );

		bool mate();
		/// spawn a child
		void reproduce( std::shared_ptr<Creature> otherparent = std::shared_ptr<Creature>());

		void move();
		bool moveTo(const Geom::Vec2f Target);
		bool randomMove();
		Geom::Vec2f getNewPosition();
		/**
			Calculates environmental effects (damage from temperature, water/food requirement)
		*/
		void calcDamage();

		/// describes the current health of the Creature ( max is species->maxage, usually around 100 )
		float currentHealth;
		int age;
		int lastmating;
		Geom::Vec2f Position;
		Geom::Vec2f prevMove;

		//References
		std::shared_ptr<Species> mySpecies;
		std::shared_ptr<Tile> currentTile;
};


#endif // CREATURE_HPP
