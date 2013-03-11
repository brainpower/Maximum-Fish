#include "Creature.hpp"

#include <cmath>

#include "sbe/event/EventUser.hpp"
#include "sbe/event/Event.hpp"

#include "simulator/Simulator.hpp"

#include "Species.hpp"
#include "Terrain.hpp"
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

	float migProb = 1;

	float hab = currentTile->getHabitability(found, mySpecies);

	std::uniform_real_distribution<float> rnd(0, 99);


	if(hab < 10) {
		for (int i = 0; i < 1000; ++i) { if (moveYourAss()) return; }
	}
	else
	{
	//maybe GTFO
	if(rnd(Simulator::GetRnd()) < migProb)
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
