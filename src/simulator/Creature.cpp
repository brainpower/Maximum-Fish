#include "Creature.hpp"

#include <cmath>

#include "sbe/event/EventUser.hpp"
#include "sbe/event/Event.hpp"

#include "Species.hpp"
#include "Terrain.hpp"
#include "Simulator.hpp"
#include "Tile.hpp"

Creature::Creature()
{

}

void Creature::HandleEvent(Event& e)
{
	if (e.Is("EVT_TICK"))
	{
		live();
	}
}

void Creature::live()
{
	// damage from environment
	//calcEnv();
	// feed
	int found = 10; // = huntFood(); <-- use this as soon as it works
	// move ( or not )
	move(found);
	// and try to reproduce
	//mate();
}

int Creature::huntFood()
{
	const float pNutritionDiv = 4;	//reduces importance of nutrition to plants
	int foodFound = 0;


	//check if this creature is a plant, carnivore or herbivore
	if(mySpecies->getMaxSpeed() != 0) {
		if(mySpecies->IsCarnivore()) {
			/*CARNIVORE*/
			//look for prey, hunt ###AI###
			//possibly:
			//foodFound = prey(); <-- will be defined in creature
		}
		else {
			/*HERBIVORE*/
			//look for plants ###AI###
			//possibly:
			//foodFound = forage(); <-- will be defined in creature
		}
	}
	else {
		//only plant-related calculations ###NO_AI###
	}

	int damage = 0;

	//to calculate health effects due to nutrition check if this creature is a plant or an animal
	if(mySpecies->getMaxSpeed() != 0) {
		damage = damage + (mySpecies->getFoodRequirement() - foodFound);
	}
	else {
		damage = damage + pNutritionDiv*(mySpecies->getFoodRequirement() - foodFound);
	}
}

void Creature::mate()
{
}

bool Creature::moveYourAss()
{
	std::uniform_real_distribution<float> rnd(-(mySpecies->getMaxSpeed()), mySpecies->getMaxSpeed());

	float x = Position.x;
	float y = Position.y;

	x = x + rnd(Simulator::GetEngine());
	y = y + rnd(Simulator::GetEngine());

	if((sqrt(pow(x,2)+sqrt(pow(y,2))) > mySpecies->getMaxSpeed()))
	{
		if(( x > 32 || x < 0 ) || ( y > 32 || y < 0 ))
		{
			return false;
		}
		else
		{
			setPosition(x, y);
			auto& newTile = Simulator::GetTerrain()->getTile(Position);

			if ( currentTile != newTile)
			{
				currentTile->removeCreature( shared_from_this() );
				newTile->addCreature( shared_from_this() );
				currentTile = newTile;
			}

			return true;
		}
	}
	else {
		return false;
	}
}

void Creature::move(int found)
{
	float migProb = 20;

	float hab = currentTile->getHabitability(found, mySpecies);

	std::uniform_real_distribution<float> rnd(0, 100);


	if(hab < 1)
	{
		//GTFO
		while(!moveYourAss()){}
	}
	else
	{
		//maybe GTFO
		if(rnd(Simulator::GetEngine()) < migProb) {
			while(!moveYourAss()){}
		}
	}

	/*std::uniform_real_distribution<float> rnd(-2, 2);

	float x = Position.x + rnd(Simulator::GetEngine());
	float y = Position.y + rnd(Simulator::GetEngine());

	if ( x > 32 || x < 0 ) x = 16;
	if ( y > 32 || y < 0 ) y = 16;*/

}

void Creature::calcEnv()
{
	//####### calculate external effects on creature ########

	//modifiers
	const float altModifier1 = 16;	//stretches out the range where creatures don't take much damage from wrong elevation \__/ -> \____/
	const float altModifier2 = 4;	//higher values here mean steeper rise in damage from moving further away from optimal elevation; only use even numbers! \__/ -> |__|


	//--damage from wrong elevation/temperature
	int damage = (int)(pow((double)( currentTile->getHeight() - mySpecies->getOptimalTemperature() ) / altModifier1, altModifier2));

	damage = damage + (mySpecies->getWaterRequirement()- currentTile->getHumidity() );

	currentHealth = currentHealth - damage;
}
