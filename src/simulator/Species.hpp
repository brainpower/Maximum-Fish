#ifndef SPECIES_H
#define SPECIES_H

#include "sbe/event/Event.hpp"
#include "sbe/event/EventUser.hpp"


class Species : public EventUser
{
	public:

		/// Differentiates between different lifeforms. The indices also define which frame to use from the tileset
		enum SPECIES_TYPE {
			HERBA = 0,
			HERBIVORE,
			CARNIVORE
		};

		Species(const std::string &_name);

		Species(const std::string& _name,
				bool _isCarnivore,
				int _maxAge,
				int _maxHealth,
				float _maxSpeed,
				float _Reach,
				float _resistance,
				int _breedingSpeed,
				int _foodRequirement,
				int waterRequirement,
				int optimalTemperature );

		virtual ~Species();

		//-1 to lazy, oh boy
		int getMaxAge() const    {return maxAge;}
		int getMaxHealth() const {return maxHealth;}
		float getMaxSpeed() const  {return maxSpeed;}
		float getReach() const     {return Reach;}
		float getResistance()const  {return resistance;}
		int getBreedingSpeed() const {return breedingSpeed;}
		SPECIES_TYPE getType() const {return type;}
		int getFoodRequirement() const {return foodRequirement;}
		int getWaterRequirement() const {return waterRequirement;}
		int getOptimalTemperature() const {return optimalTemperature;}

		const std::string& getName() { return name; }

		//+1 style
		void setMaxAge(int ma) {                        maxAge = ma;}
		void setMaxHealth(int mh) {                  maxHealth = mh;}
		void setMaxSpeed(float ms) {                  maxSpeed = ms;}
		void setReach(float r) {                            Reach = r;}
		void setResistance(float re){                 resistance = re;}
		void setBreedingSpeed(int bs) {          breedingSpeed = bs;}
		void setType(SPECIES_TYPE t) {                 type = t; if (type == HERBA) maxSpeed = 0; }
		void setFoodRequirement(int fr) {      foodRequirement = fr;}
		void setWaterRequirement(int wr){     waterRequirement = wr;}
		void setOptimalTemperature(int oh) {optimalTemperature = oh;}

		void HandleEvent( Event& e );
	private:

		friend class SpeciesIOPlugin;

		std::string name;
		/// age in ticks
		int maxAge;
		/// maximum health, should be around 100
		int maxHealth;
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
		/// how much food is required to keep the current health level
		int foodRequirement;
		/// how much water is required to keep the current health level
		int waterRequirement;

		/**
			Determines the optimal Height for the species to live and
			also the required temperature
		*/
		int optimalTemperature;

		//std::vector<std::shared_ptr<Creature>> Creatures;

		std::string ImageSetName;
		int Frame;
};

#endif // SPECIES_H
