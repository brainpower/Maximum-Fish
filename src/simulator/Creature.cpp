#include "Creature.hpp"

#include <cmath>

#include "sbe/event/EventUser.hpp"
#include "sbe/event/Event.hpp"

#include "Species.hpp"
#include "Terrain.hpp"
#include "Simulator.hpp"
#include "Tile.hpp"

Creature::Creature( const std::shared_ptr<Species>& Species)
 : 	currentHealth(100),
	age(0),
	Position( 0, 0 ),
	mySpecies (Species)
{
	if (!Species)
	{
		Engine::out(Engine::ERROR) << "[Creature] constructed with invalid Species" << std::endl;
	}
	else
	{
		this->setCurrentHealth(Species->getMaxHealth());
	}
}

const std::string& Creature::getSpeciesString() const
{
	return mySpecies->getName();
}

void Creature::setPosition( const Geom::Pointf& pos)
{
	auto& newTile = Simulator::GetTerrain()->getTile(pos);

	Position = pos;

	if ( currentTile != newTile)
	{
		if (currentTile) currentTile->removeCreature( shared_from_this() );
		newTile->addCreature( shared_from_this() );
		currentTile = newTile;
	}
}

void Creature::movePosition( const Geom::Pointf& pos)
{
	setPosition( Position + pos );
}

void Creature::live()
{
	// damage from environment
	calcEnv();
	// feed
	int found = 0;
	if(currentHealth/mySpecies->getMaxHealth() < 0.75)
	{
		huntFood(); 
	}
	//else if(Artgenossen in der Naehe)
	//{
	//	mate();
	//}
	else
	{
		move(found);
	}
	// move ( or not )
	// and try to reproduce
	//
}

void Creature::huntFood()
{
	const float pNutritionDiv = 4;	//reduces importance of nutrition to plants
	int foodFound = 0;
	std::shared_ptr<Terrain> terra = Simulator::GetTerrain();

	switch (mySpecies->getType())
	{
		case Species::HERBA:
			//only plant-related calculations ###NO_AI###
			break;
		case Species::CARNIVORE:
			{
			//look for prey, hunt ###AI###
			float min_dist 	= mySpecies->getReach();
			std::shared_ptr<Creature> nearest;
			
			terra->getNearby(Position, mySpecies->getReach(), 
				[&]( const std::shared_ptr<Creature>& C ) 
				{ 
					if (C->getSpecies()->getType() != Species::HERBIVORE) return false;
					if ( float dist = Geom::distance( C->getPosition(), Position ) < min_dist)
					{ 
						min_dist = dist; nearest = C; 
					}
					return  false;
				});
			
			if ( !nearest )
			{
				move(0);
				break;
			}
			
			if ( Geom::distance( nearest->getPosition(), Position ) > mySpecies->getMaxSpeed() )
			{
				Geom::Vec2f target = Geom::normalize( nearest->getPosition() - Position );
				target *= Geom::Vec2f(mySpecies->getMaxSpeed(), mySpecies->getMaxSpeed());
				target += Position;
				if (validPos( target ) )
					setPosition( target );
				
			} else {
				currentHealth += nearest->getCurrentHealth();
				if(currentHealth > mySpecies->getMaxHealth()) currentHealth = mySpecies->getMaxHealth();
				nearest->setCurrentHealth(0);
			}
			
			break;
			}
		case Species::HERBIVORE:
			//look for plants ###AI###
			//possibly:
			//foodFound = forage(); <-- will be defined in creature
			break;
	}

	int damage = 0;

	//to calculate health effects due to nutrition check if this creature is a plant or an animal
	if(mySpecies->getType() != Species::HERBA) {
		damage += mySpecies->getFoodRequirement() - foodFound;
	}
	else {
		damage += pNutritionDiv*(mySpecies->getFoodRequirement() - foodFound);
	}
}

void Creature::mate()
{
}

bool Creature::moveYourAss()
{
	std::uniform_real_distribution<float> rnd(-(mySpecies->getMaxSpeed()), mySpecies->getMaxSpeed());

	Geom::Pointf NewPosition = Position;

	NewPosition.x += rnd(Simulator::GetEngine());
	NewPosition.y += rnd(Simulator::GetEngine());

	float hab = 0;
	std::shared_ptr<Tile>& newtile = Simulator::GetTerrain()->getTile( NewPosition );
	if (newtile) hab = newtile->getHabitability(1,mySpecies);

	/*
	Engine::out() << "Pos: " << Position << std::endl;
	Engine::out() << "NewPos: " << NewPosition << std::endl;
	Engine::out() << "Hab: " << hab << std::endl;
	Engine::out() << "dist: " << Geom::distance( Position, NewPosition) << std::endl;
	Engine::out() << "maxspeed: " << mySpecies->getMaxSpeed() << std::endl;
	*/

	if( Geom::distance( Position, NewPosition) < mySpecies->getMaxSpeed())
	{
		if( !validPos( NewPosition ) ||  (hab <= 0.0f))
			return false;

		setPosition( NewPosition );
		return true;
	}

	return false;	
}

void Creature::move(int found)
{
	if ( mySpecies->getMaxSpeed() == 0 ) return;
	
	float migProb = 1;

	float hab = currentTile->getHabitability(found, mySpecies);

	std::uniform_real_distribution<float> rnd(0, 99);


	if(hab < 10) {
		for (int i = 0; i < 1000; ++i) { if (moveYourAss()) return; }
	}
	else
	{
	//maybe GTFO
	if(rnd(Simulator::GetEngine()) < migProb)
		for (int i = 0; i < 1000; ++i) { if (moveYourAss()) return; }
	}


	/*std::uniform_real_distribution<float> rnd(-2, 2);

	float x = Position.x + rnd(Simulator::GetEngine());
	float y = Position.y + rnd(Simulator::GetEngine());

	if ( x > 32 || x < 0 ) x = 16;
	if ( y > 32 || y < 0 ) y = 16;*/

}

bool Creature::validPos( Geom::Pointf NewPosition )
{
	std::shared_ptr<Tile> newtile = Simulator::GetTerrain()->getTile( NewPosition );
	
	if (!newtile) return false;	
	if (!newtile->isWater()) return true;
	return false;
}

void Creature::calcEnv()
{
	//####### calculate external effects on creature ########
	float mult = 0.0001;

	//modifiers
	const float altModifier1 = 16;	//stretches out the range where creatures don't take much damage from wrong elevation \__/ -> \____/
	const float altModifier2 = 2;	//higher values here mean steeper rise in damage from moving further away from optimal elevation; only use even numbers! \__/ -> |__|


	//--damage from wrong elevation/temperature
	int damage = (int)(pow((double)( currentTile->getHeight() - mySpecies->getOptimalTemperature() ) / altModifier1, altModifier2));

	damage += (mySpecies->getWaterRequirement()- currentTile->getHumidity() );
	
	float fdmg = (float)(damage)*mult;

	currentHealth -= fdmg;
}
