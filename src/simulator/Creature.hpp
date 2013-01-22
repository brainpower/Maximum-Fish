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
class Creature : public std::enable_shared_from_this<Creature>
{
	public:
		Creature( const std::shared_ptr<Species>& Species);
		virtual ~Creature() {};

		void live();

		void setCurrentHealth(const float ch){ currentHealth = ch;}
		void setCurrentTile(const std::shared_ptr<Tile> t){ currentTile = t;}
		void setAge(const int a){ age = a; }
		void setPosition(const Geom::Pointf& pos);

		float getCurrentHealth() const { return currentHealth; }
		int getAge() const { return age; }
		const std::string& getSpeciesString() const;
		const std::shared_ptr<Species>& getSpecies() const {return mySpecies;};
		const Geom::Vec2f& getPosition() const { return Position; }

	private:

		friend class CreatureIOPlugin;
		void huntFood();
		void mate();
		void move(int found);
		bool moveYourAss();
		/**
			Calculates environmental effects (damage from temperature etc)
		*/
		void calcEnv();


		/// describes the current health of the Creature ( max is species->maxage, usually around 100 )
		float currentHealth;
		int age;
		Geom::Vec2f Position;

		//References
		std::shared_ptr<Species> mySpecies;
		std::shared_ptr<Tile> currentTile;
};


#endif // CREATURE_HPP
