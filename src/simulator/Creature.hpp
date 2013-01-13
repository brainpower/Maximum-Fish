#ifndef CREATURE_HPP
#define CREATURE_HPP

#include <memory>

#include "sbe/event/EventUser.hpp"

class Species;
class Tile;

/**
The Creature class is the main scaff for simulation, this is where als the basic stuff is initialized.
Also basic calculation are happening here.
*/
class Creature : public EventUser, public std::enable_shared_from_this<Creature>
{
	public:
		Creature();
		virtual ~Creature() {};
		virtual void HandleEvent( Event& e);

		void live();

		inline void setCurrentHealth(const int ch){ currentHealth = ch;}
		inline void setCurrentTile(const std::shared_ptr<Tile> t){ currentTile = t;}
		inline void setAge(const int a){ age = a; }
		inline void setSpecies(const std::string &s){ /* TODO: do some magic here ;) */}
		inline void setSpecies(const std::shared_ptr<Species> s){mySpecies = s;}
		inline void setPosition(const float x, const float y) { Position = Geom::Vec2f(x,y); } // TODO: update tile on pos update

		inline int getCurrentHealth() const { return currentHealth; }
		inline int getAge() const { return age; }
		inline std::string getSpeciesString() const { /* TODO: same magic here, but the other way round */ }
		inline Geom::Vec2f getPosition() const { return Position; }

	private:

		friend class CreatureIOPlugin;
		int huntFood();
		void mate();
		void move(int found);
		bool moveYourAss();
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
		std::shared_ptr<Tile> currentTile;
};


#endif // CREATURE_HPP
