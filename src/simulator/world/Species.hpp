#ifndef SPECIES_H
#define SPECIES_H

#include <string>
#include <memory>

/// Superclass for all Creatures, defines some modifiers applying to the whole species
class Species
{
	public:

		/// Differentiates between different lifeforms. The indices also define which frame to use from the tileset
		enum SPECIES_TYPE {
			HERBA = 0,
			HERBIVORE,
			CARNIVORE,
			UNKNOWN // keep this the last type!
		};

		static std::string type2String( SPECIES_TYPE t )
		{
			std::string typestring;
			switch ( t )
			{
				case HERBA:
					typestring = "plant";
					break;
				case HERBIVORE:
					typestring = "herbivore";
					break;
				case CARNIVORE:
					typestring = "carnivore";
					break;
			}
			return typestring;
		}

		static SPECIES_TYPE string2Type( const std::string& t )
		{
			SPECIES_TYPE type;
			if ( t=="plant" )	type = HERBA;
			else if ( t=="herbivore") type = HERBIVORE;
			else if ( t=="carnivore") type = CARNIVORE;
			else type = UNKNOWN;
			return type;
		}


		Species(const std::string &_name, SPECIES_TYPE t);

		virtual ~Species();

		//-1 to lazy, oh boy
		int getMaxAge() const    {return maxAge;}
		float getMaxHealth() const {return maxHealth;}
		float getMaxSpeed() const  {return maxSpeed;}
		float getReach() const     {return Reach;}
		float getResistance()const  {return resistance;}
		int getBreedingSpeed() const {return breedingSpeed;}
		SPECIES_TYPE getType() const {return type;}

		float getMaxRegeneration() const { return maxRegeneration; }
		float getFoodRequirement() const {return foodRequirement;}
		float getWaterRequirement() const {return waterRequirement;}
		int getOptimalTemperature() const {return optimalTemperature;}

		const std::string& getName() const { return name; }

		//+1 style
		void setMaxAge(int ma) {                        maxAge = ma;}
		void setMaxHealth(float mh) {                  maxHealth = mh;}
		void setMaxSpeed(float ms) {                  maxSpeed = ms;}
		void setReach(float r) {                            Reach = r;}
		void setResistance(float re){                 resistance = re;}
		void setBreedingSpeed(int bs) {          breedingSpeed = bs;}
		void setType(SPECIES_TYPE t) {                 type = t; if (type == HERBA) maxSpeed = 0; }

		void setMaxRegeneration(float mR) { maxRegeneration = mR; }
		void setFoodRequirement(float fr) {      foodRequirement = fr;}
		void setWaterRequirement(float wr){     waterRequirement = wr;}
		void setOptimalTemperature(int oh) {optimalTemperature = oh;}

	private:

		friend class SpeciesIOPlugin;

		std::string name;
		/// age in ticks
		int maxAge;
		/// maximum health, should be around 100
		float maxHealth;
		/// maximum speed a.k.a. range the creature can move in one tick
		float maxSpeed;
		/// range the creature can detect prey
		float Reach;
		/// resistance, just a factor ( in percent, eg 1.1 for 10% better )
		float resistance;
		/// determines time till next reproduction
		int breedingSpeed;
		/// different types of Creatures we want to simulate
		SPECIES_TYPE type;
		/// how much health can a creature absorb each tick
		float maxRegeneration;
		/// how much food is required to keep the current health level
		float foodRequirement;
		/// how much water is required to keep the current health level
		float waterRequirement;

		/**
			Determines the optimal Temperature for the species to live in °C
		*/
		int optimalTemperature;

		//std::vector<std::shared_ptr<Creature>> Creatures;

		std::string ImageSetName;
		int Frame;
};

#endif // SPECIES_H
