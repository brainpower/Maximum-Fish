#ifndef CREATURE_H
#define CREATURE_H

#include "sbe/event/Event.hpp"
#include "sbe/event/EventUser.hpp"

/**
The Creature class is the main scaff for simulation, this is where als the basic stuff is initialized.
Also basic calculation are happening here.
*/
class Creature : public EventUser
{
	public:
		Creature();
		virtual ~Creature() {};
		virtual void HandleEvent(Event& e);
		
	private:
		
		//Attributes
		int currentHealth;
		int hunger;
		int water;
		int age;
		
		//Character
		int maxHealth;
		int speed;
		int reach;
		int resistance;
		int breedingSpeed;
		
		//Modificat0rs
		int consumption;
		int aging;
		
};


#endif // CREATURE_H