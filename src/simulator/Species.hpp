#ifndef SPECIES_H
#define SPECIES_H

#include "sbe/event/Event.hpp"
#include "sbe/event/EventUser.hpp"


class Species : public EventUser
{
	public:
		int getMaxAge() {return maxAge;}
		int getMaxHealth() {return maxHealth;}
		int getMaxSpeed() {return maxSpeed;}
		int getReach() {return Reach;}
		int getResistance() {return resistance;}
		int getBreedingSpeed() {return breedingSpeed;}
		bool getIsCarnivore() {return isCarnivore;}
		int getFoodRequirement() {return foodRequirement;}
		int getWaterRequirement() {return waterRequirement;}
		int getOptimalHeight() {return optimalHeight;}
		
		void HandleEvent( Event& e );
	private:

		std::string name;
		int maxAge;
		int maxHealth;
		int maxSpeed;
		int Reach;
		int resistance;
		int breedingSpeed;
		
		bool isCarnivore;
		int foodRequirement;
		int waterRequirement;

		/**
			Determines the optimal Height for the species to live and
			also the required temperature
		*/
		int optimalHeight;

		//std::vector<std::shared_ptr<Creature>> Creatures;

		std::string ImageSetName;
		int Frame;
};

#endif // SPECIES_H

