#ifndef CREATURE_H
#define CREATURE_H


/**
The Creature class is the main scaff for simulation, this is where als the basic stuff is initialized.
Also basic calculation are happening here.
*/
class Creature
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
		Geom::Vec2 Position;

		//Character
		int maxHealth;
		int speed;
		int reach;
		int resistance;
		int breedingSpeed;

		//Modificat0rs
		int consumption;
		int aging;

		std::shared_ptr<Species> mySpecies;
};


#endif // CREATURE_H
