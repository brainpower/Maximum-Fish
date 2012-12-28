#ifndef CREATURE_HPP
#define CREATURE_HPP

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

		inline void setCurrentHealth(const int ch){ currentHealth = ch;}
		inline void setAge(const int a){ age = a; }
		inline void setSpecies(const string &s){ /* TODO: do some magic here ;) */}
		inline void setPosition(const float x, const float y) { Position = Geom::Vec2f(x,y); }

		inline int getCurrentHealth() const { return currentHealth; }
		inline int getAge() const { return age; }
		inline string getSpeciesString() { /* TODO: same magic here, but the other way round */ }
		inline Geom::Vec2f getPosition() const { return Position; }
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


#endif // CREATURE_HPP
