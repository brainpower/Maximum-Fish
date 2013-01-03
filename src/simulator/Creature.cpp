#include "Creature.hpp"

#include <cmath>

#include "sbe/event/EventUser.hpp"
#include "sbe/event/Event.hpp"

#include "Simulator.hpp"
#include "Tile.hpp"

Creature::Creature()
{
//variables to be initialized after discussion


	// if we left our old Tile
	currentTile = Simulator::GetTerrain()->getTile( Position );
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
	calcEnv();
	// feed
	huntFood();
	// move ( or not )
	move();
	// and try to reproduce
	mate();
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

void Creature::move()
{
	// plants dont move
	if (mySpecies->getMaxSpeed() == 0) return;




	// if we left our old Tile
	currentTile = Simulator::GetTerrain()->getTile( Position );

}

void Creature::calcEnv()
{
	//####### calculate external effects on creature ########

	//modifiers
	const float altModifier1 = 16;	//stretches out the range where creatures don't take much damage from wrong elevation \__/ -> \____/
	const float altModifier2 = 4;	//higher values here mean steeper rise in damage from moving further away from optimal elevation; only use even numbers! \__/ -> |__|


	//--damage from wrong elevation/temperature
	int damage = (int)(pow((double)( currentTile->getElevation() - mySpecies->getOptimalTemperature() ) / altModifier1, altModifier2));

	damage = damage + (mySpecies->getWaterRequirement()- currentTile->getHumidity() );

	currentHealth = currentHealth - damage;
}


