#ifndef SPECIES_H
#define SPECIES_H

#include "sbe/event/Event.hpp"
#include "sbe/event/EventUser.hpp"


class Species : public EventUser
{

	public:
		Species(const std::string &name);
		virtual ~Species() = default;

		//-1 to lazy, oh boy
		int getMaxAge() const    {return maxAge;}
		int getMaxHealth() const {return maxHealth;}
		int getMaxSpeed() const  {return maxSpeed;}
		int getReach() const     {return Reach;}
		int getResistance()const  {return resistance;}
		int getBreedingSpeed() const {return breedingSpeed;}
		bool IsCarnivore() const {return isCarnivore;}
		int getFoodRequirement() const {return foodRequirement;}
		int getWaterRequirement() const {return waterRequirement;}
		int getOptimalTemperature() const {return optimalTemperature;}

		//+1 style
		void setMaxAge(int ma) {                        maxAge = ma;}
		void setMaxHealth(int mh) {                  maxHealth = mh;}
		void setMaxSpeed(int ms) {                    maxSpeed = ms;}
		void setReach(int r) {                            Reach = r;}
		void setResistance(int re){                 resistance = re;}
		void setBreedingSpeed(int bs) {          breedingSpeed = bs;}
		void setCarnivore(bool c) {                 isCarnivore = c;}
		void setFoodRequirement(int fr) {      foodRequirement = fr;}
		void setWaterRequirement(int wr){     waterRequirement = wr;}
		void setOptimalTemperature(int oh) {optimalTemperature = oh;}

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
		int optimalTemperature;

		//std::vector<std::shared_ptr<Creature>> Creatures;

		std::string ImageSetName;
		int Frame;
};

#endif // SPECIES_H
