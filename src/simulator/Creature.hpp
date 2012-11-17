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
		int age;
		Geom::Vec2f Position;

		//References
		std::shared_ptr<Species> mySpecies;
};


#endif // CREATURE_H
