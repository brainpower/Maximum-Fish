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
		Creature( const std::shared_ptr<Species>& Species);
		virtual ~Creature() {};
		virtual void HandleEvent( Event& e);

		void live();

		void setCurrentHealth(const int ch){ currentHealth = ch;}
		void setCurrentTile(const std::shared_ptr<Tile> t){ currentTile = t;}
		void setAge(const int a){ age = a; }
		void setSpecies(const std::string &s){ /* TODO: do some magic here ;) */}
		void setSpecies(const std::shared_ptr<Species> s){mySpecies = s;}
		void setPosition(const Geom::Pointf& pos);

		int getCurrentHealth() const { return currentHealth; }
		int getAge() const { return age; }
		const std::string& getSpeciesString() const { return mySpecies->getName(); }
		const Geom::Vec2f& getPosition() const { return Position; }

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
