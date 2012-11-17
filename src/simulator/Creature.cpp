#include "Creature.hpp"
#include <Math>

#include "sbe/event/EventUser.hpp"
#include "sbe/event/Event.hpp"

Creature::Creature()
{
//variables to be initialized after discussion
}

void Creature::HandleEvent(Event& e)
{
	live();
}

void Creature::live()
{
	if (e.Is("EVT_TICK"))
	{	
		//modifiers
		const float altModifier1 = 16;	//stretches out the range where creatures don't take much damage from wrong altitude \__/ -> \____/
		const float altModifier2 = 4;	//higher values here mean steeper rise in damage from moving further away from optimal altitude; only use even numbers! \__/ -> |__|
		const float pNutritionDiv = 4;	//reduces importance of nutrition to plants
	
		int foodFound = 0;
	
		//check if this creature is a plant, carnivore or herbivore
		if(mySpecies->getMaxSpeed() != 0) {
			if(mySpecies->getIsCarnivore() == true) {
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
			//possibly:
			//foodFound = 
		}
		
		//####### calculate external effects on creature ########
		//--damage from wrong altitude
		int damage = (int)(pow((double)(/*altitude of current tile*/-mySpecies->getOptimalHeight())/altModifier1, altModifier2));
		
		//to calculate health effects due to nutrition check if this creature is a plant or an animal
		if(mySpecies->getMaxSpeed() != 0) {
			damage = damage + (mySpecies->getFoodRequirement()-foodFound);
		}
		else {
			damage = damage + pNutritionDiv*(mySpecies->getFoodRequirement()-foodFound);
		}
		damage = damage + (mySpecies->getWaterReqirement()-/*humidity of current tile*/);
		
		health = health - damage;
		
		//mate
		
		//move
	}
}

