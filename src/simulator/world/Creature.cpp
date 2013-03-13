#include "Creature.hpp"

#include <cmath>

#include "sbe/event/EventUser.hpp"
#include "sbe/event/Event.hpp"
#include "sbe/Config.hpp"

#include "simulator/Simulator.hpp"

#include "Species.hpp"
#include "Terrain.hpp"
#include "Tile.hpp"

// static variables
float Creature::pNutritionDiv = 4;
float Creature::huntingThreshold = 0.75;
float Creature::matingThreshold = 0.9;
float Creature::matingAge = 0.2;
float Creature::matingHealthCost = 0.25;
float Creature::migProb = 0.01;
float Creature::altModifier1 = 16;
float Creature::altModifier2 = 2;
float Creature::envMult = 0.0001;



Creature::Creature( const std::shared_ptr<Species>& Species)
 : 	currentHealth(100),
	age(0),
	lastmating(0),
	Position( 0, 0 ),
	mySpecies (Species)
{
	if (!Species)
	{
		Engine::out(Engine::ERROR) << "[Creature] constructed with invalid Species" << std::endl;
	}
	else
	{
		currentHealth = Species->getMaxHealth();
	}
}

const std::string& Creature::getSpeciesString() const
{
	return mySpecies->getName();
}

void Creature::setPositionUnsafe( const Geom::Pointf& pos)
{
	Position = pos;
}

void Creature::setPosition( const Geom::Pointf& pos)
{
	updateTileFromPos();
	Position = pos;
}

void Creature::updateTileFromPos(){
	auto& newTile = Simulator::GetTerrain()->getTile(Position);

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
	//std::list<std::shared_ptr<Creature>> nearby = Simulator::GetTerrain()->getNearby(this->getPosition(), 2.0);
	float healthPercentage = currentHealth/mySpecies->getMaxHealth();

	if ( healthPercentage < huntingThreshold )
	{
		huntFood();
	}
	else if ( age - lastmating > mySpecies->getBreedingSpeed()
				&& healthPercentage > matingThreshold
				&& age > mySpecies->getMaxAge()*matingAge )
	{
		mate();
	}
	else
	{
		move(found);
	}

	age++;
}

void Creature::huntFood()
{
	int foodFound = 0;
	int damage = 0;

	switch (mySpecies->getType())
	{
		case Species::HERBA:
			// TODO: calculate damage from insufficient water or nutrition in our tile
			damage += pNutritionDiv*(mySpecies->getFoodRequirement() - foodFound);
			break;

		case Species::CARNIVORE:
			huntNearest( [&]( const std::shared_ptr<Creature>& C ) { return (C->getSpecies()->getType() == Species::HERBIVORE); });
			damage += mySpecies->getFoodRequirement() - foodFound;
			break;

		case Species::HERBIVORE:
			huntNearest( [&]( const std::shared_ptr<Creature>& C ) { return (C->getSpecies()->getType() == Species::HERBA); });
			damage += mySpecies->getFoodRequirement() - foodFound;
			break;
	}

}

void Creature::huntNearest( std::function< bool( std::shared_ptr<Creature> ) > filter )
{
	// get nearest creature
	std::shared_ptr<Creature> nearest = Simulator::GetTerrain()->getNearest(Position, mySpecies->getReach(), filter);
	// nothing found? move randomly
	if ( !nearest ) {
		move(0);
		return;
	}

	// check if we can reach our target in a single tick
	if ( Geom::distance( nearest->getPosition(), Position ) > mySpecies->getMaxSpeed() )
	{
		// move in direction of the target
		Geom::Vec2f target = Geom::normalize( nearest->getPosition() - Position );
		target *= Geom::Vec2f(mySpecies->getMaxSpeed(), mySpecies->getMaxSpeed());
		target += Position;
		if (validPos( target ) )
			setPosition( target );
	} else {
		// consume our prey
		currentHealth += nearest->getCurrentHealth();
		if(currentHealth > mySpecies->getMaxHealth()) currentHealth = mySpecies->getMaxHealth();
		nearest->setCurrentHealth(0);
	}
}

void Creature::mate()
{
	mateNearest( [&]( const std::shared_ptr<Creature>& C )
		{
			 return ( C->getSpecies() == mySpecies
					&& C.get() != this
					&& C->getCurrentHealth() > matingThreshold
					&& C->getAge() > (C->getSpecies()->getMaxAge()*matingAge));
		});
}

void Creature::mateNearest(std::function< bool( std::shared_ptr<Creature> ) > filter)
{
	std::shared_ptr<Creature> nearest = Simulator::GetTerrain()->getNearest(Position, mySpecies->getReach(), filter);
	if ( !nearest ) {
		move(0);
		return;
	}

	if ( Geom::distance( nearest->getPosition(), Position ) > mySpecies->getMaxSpeed() )
	{
		Geom::Vec2f target = Geom::normalize( nearest->getPosition() - Position );
		target *= Geom::Vec2f(mySpecies->getMaxSpeed(), mySpecies->getMaxSpeed());
		target += Position;
		if ( validPos( target ) )
			setPosition( target );

	} else {

		lastmating = age;

		float healthCost = mySpecies->getMaxHealth() * matingHealthCost;
		auto newborn = std::shared_ptr<Creature>(new Creature(mySpecies));

		newborn->setPosition(Position);
		newborn->setCurrentHealth( healthCost*2 );
		Simulator::GetCreatures().push_back(newborn);

		currentHealth -= healthCost;
		nearest->setCurrentHealth(nearest->getCurrentHealth() - healthCost);

		//std::cout << "New creature created" << std::endl;
	}
}

bool Creature::moveYourAss()
{
	std::uniform_real_distribution<float> rnd(-(mySpecies->getMaxSpeed()), mySpecies->getMaxSpeed());

	Geom::Pointf NewPosition = Position;

	NewPosition.x += rnd(Simulator::GetRnd());
	NewPosition.y += rnd(Simulator::GetRnd());

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

	float hab = currentTile->getHabitability(found, mySpecies);

	std::uniform_real_distribution<float> rnd(0, 1);


	if( hab < 10 ) {
		for (int i = 0; i < 1000; ++i) { if (moveYourAss()) return; }
	}
	else
	{
	//maybe GTFO
	if( rnd(Simulator::GetRnd()) < migProb )
		for (int i = 0; i < 1000; ++i) { if (moveYourAss()) return; }
	}
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

	//--damage from wrong elevation/temperature
	int damage = (int)(pow((double)( currentTile->getHeight() - mySpecies->getOptimalTemperature() ) / altModifier1, altModifier2));

	damage += (mySpecies->getWaterRequirement()- currentTile->getHumidity() );

	float fdmg = (float)(damage)*envMult;

	currentHealth -= fdmg;
}

void Creature::loadConfigValues()
{
	pNutritionDiv = 	Engine::getCfg()->get<float>("sim.creature.pNutritionDiv");
	huntingThreshold = 	Engine::getCfg()->get<float>("sim.creature.huntingThreshold");
	matingThreshold = 	Engine::getCfg()->get<float>("sim.creature.matingThreshold");
	matingAge = 		Engine::getCfg()->get<float>("sim.creature.matingAge");
	matingHealthCost = 	Engine::getCfg()->get<float>("sim.creature.matingHealthCost");
	migProb = 			Engine::getCfg()->get<float>("sim.creature.migProb");
	altModifier1 = 		Engine::getCfg()->get<float>("sim.creature.altModifier1");
	altModifier2 = 		Engine::getCfg()->get<float>("sim.creature.altModifier2");
	envMult = 			Engine::getCfg()->get<float>("sim.creature.envMult");
}
