#ifndef SPECIES_H
#define SPECIES_H

#include "sbe/event/Event.hpp"
#include "sbe/event/EventUser.hpp"


class Species : public EventUser
{
	public:

		void HandleEvent( Event& e );
	private:

		std::string name;
		int maxAge;
		int maxHealth;
		int maxSpeed;
		int Reach;
		int resistance;
		int breedingSpeed;

		int foodRequirement;
		int waterRequirement;

		/**
			Determines the optimal Height for the species to live and
			also the required temperature
		*/
		int optimalHeight;

		std::vector<std::shared_ptr<Creature>> Creatures;

		std::string ImageSetName;
		int Frame;
};

#endif // SPECIES_H

