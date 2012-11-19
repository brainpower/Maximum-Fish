#ifndef CREATURE_H
#define CREATURE_H

#include <memory>

#include "sbe/event/EventUser.hpp"

#include "Species.hpp"

class Tile;

/**
The Creature class is the main scaff for simulation, this is where als the basic stuff is initialized.
Also basic calculation are happening here.
*/
class Creature : public EventUser
{
	public:
		Creature();
		virtual ~Creature() {};
		virtual void HandleEvent( Event& e);

	private:
		void live();

		int huntFood();
		void mate();
		void move();
		/**
			Calculates environmental effects (damage from temperature etc)
		*/
		void calcEnv();


		//Attributes
		int currentHealth;
		int age;
		Geom::Vec2f Position;

		//References
		std::shared_ptr<Species> mySpecies;
		Tile* currentTile;
};


#endif // CREATURE_H
