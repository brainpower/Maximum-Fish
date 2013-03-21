#include "Creature.hpp"

#include <cmath>

#include "sbe/event/Event.hpp"
#include "sbe/Config.hpp"
#include "sbe/geom/Helpers.hpp"

#include "simulator/Simulator.hpp"

#include "Terrain.hpp"
#include "Tile.hpp"

// static variables
float Creature::huntingThreshold = 0;
float Creature::matingThreshold = 0;
float Creature::matingAge = 0;
float Creature::matingHealthCost = 0;
float Creature::migProb = 0;
float Creature::altModifier1 = 0;
float Creature::altModifier2 = 0;
float Creature::envMult = 0;
float Creature::resistance = 0;
int   Creature::ageExponent = 0;
float Creature::plantEnvDmgFactor = 0;
float Creature::NutritionValue = 0;


Creature::Creature( const std::shared_ptr<Species>& Species)
 : 	done(true),
	currentHealth(100),
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
	Position = pos;
	updateTileFromPos();
}

void Creature::updateTileFromPos(){
	auto& newTile = Simulator::GetTerrain()->getTile(Position);

	if ( currentTile != newTile)
	{
		if (currentTile) currentTile->removeCreature( shared_from_this() );
		newTile->addCreature( shared_from_this() );
		if ( mySpecies->getType() == Species::HERBA ) newTile->addUsedNutrition( NutritionValue );
		currentTile = newTile;
	}
}

void Creature::movePosition( const Geom::Pointf& pos)
{
	setPosition( Position + pos );
}

void Creature::live()
{
	done = true;
	// damage from environment
	resistance = healthPercentage() * currentResistance();
	calcDamage();

	// do nothing, we're dead
	// the simulator will remove this creature after the current tick
	if ( currentHealth <= 0 ) {
		die();
		return;
	}

	bool didsomethingthistick = false;
	//std::list<std::shared_ptr<Creature>> nearby = Simulator::GetTerrain()->getNearby(this->getPosition(), 2.0);


	if ( healthPercentage() < huntingThreshold )
	{
		didsomethingthistick = huntFood();
	}
	else if ( age - lastmating > mySpecies->getBreedingSpeed()/resistance
				&& healthPercentage() > matingThreshold
				&& age > minAge() )
	{
		didsomethingthistick = mate();
	}

	if ( !didsomethingthistick )
		move();

	age++;

	// do nothing, we're dead
	// the simulator will remove this creature after the current tick
	if ( currentHealth <= 0 )
		die();
}

//################################################
//					HUNTING
//################################################

bool Creature::huntFood()
{
	float nutritionAmount = 0;
	switch (mySpecies->getType())
	{
		case Species::HERBA:
			currentHealth += currentMaxHealth() * currentTile->getFoodSupply() * mySpecies->getMaxRegeneration();
			if ( currentHealth > currentMaxHealth() ) currentHealth = currentMaxHealth();
			return true;
			break;

		case Species::CARNIVORE:
			return huntNearest( Species::HERBIVORE );
			break;

		case Species::HERBIVORE:
			return huntNearest( Species::HERBA );
			break;
	}

}

bool Creature::huntNearest( int type )
{
	// get nearest creature
	std::shared_ptr<Creature> nearest = Simulator::GetTerrain()->getNearest(Position, mySpecies->getReach(), type);
	if ( !nearest ) return false;

	// move to prey
	if ( moveTo( nearest->getPosition() ) ) {
		// consume our prey
		currentHealth += nearest->getCurrentHealth();
		if ( currentHealth > currentMaxHealth() ) currentHealth = currentMaxHealth();
		nearest->die();
	}

	return true;
}

//################################################
//					MATING
//################################################

bool Creature::mate()
{
	switch (mySpecies->getType())
	{
		case Species::HERBA:
			reproduce();
			return true;
			break;

		// fall through
		case Species::CARNIVORE:
		case Species::HERBIVORE:

			std::shared_ptr<Creature> nearest = Simulator::GetTerrain()->getNearest(Position, mySpecies->getReach(), mySpecies, [&]( const std::shared_ptr<Creature>& C )
			{
				 return ( C.get() != this
						&& C->getCurrentHealth() > matingThreshold
						&& C->getAge() > C->minAge());
			});

			if ( !nearest ) return false;

			// move to mating partner
			if ( moveTo( nearest->getPosition() ) )
				reproduce( nearest );
			return true;

		break;
	}

}


void Creature::reproduce( std::shared_ptr<Creature> otherparent)
{
	lastmating = age;
	auto newborn = std::shared_ptr<Creature>(new Creature(mySpecies));
	Geom::Pointf newPosition = Position;

	// for plants
	if ( ! otherparent )
	{
		std::uniform_real_distribution<float> rnd(-(mySpecies->getReach()), mySpecies->getReach());
		for (int i = 0; i < 10; ++i)
		{
			newPosition.x += rnd(Simulator::GetRnd());
			newPosition.y += rnd(Simulator::GetRnd());
			if ( validPos(newPosition)) break;
		}
	}

	if ( !validPos(newPosition) ) newPosition = Position;

	newborn->setPosition(newPosition);
	// health from two parents for animals, from one for plants
	newborn->setCurrentHealth( otherparent? mHealthCost()*2 : mHealthCost() );
	Simulator::GetCreatures().push_back(newborn);

	currentHealth -= mHealthCost();
	// plants dont need another parent
	if ( otherparent ) otherparent->setCurrentHealth(otherparent->getCurrentHealth() - mHealthCost());
}

//################################################
//					MOVING
//################################################

void Creature::move()
{
	if ( mySpecies->getMaxSpeed() == 0 ) return;

	float hab = currentTile->getHabitability(mySpecies);

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


bool Creature::moveTo( Geom::Pointf Target )
{
	// move as far in direction of the target as possible
	if ( Geom::distance( Target, Position ) > currentMaxSpeed() )
	{
		Geom::Vec2f direction = Geom::normalize( Target - Position );
		direction *= Geom::Vec2f(mySpecies->getMaxSpeed(), currentMaxSpeed());
		direction += Position;
		if ( validPos( direction ) )
			setPosition( direction );
		return false;
	// move to the target
	} else {
		setPosition( Target );
		return true;
	}
}

bool Creature::moveYourAss()
{
	std::uniform_real_distribution<float> rnd( -currentMaxSpeed() , currentMaxSpeed() );

	Geom::Pointf NewPosition = Position;

	NewPosition.x += rnd(Simulator::GetRnd());
	NewPosition.y += rnd(Simulator::GetRnd());

	float hab = 0;
	const std::shared_ptr<Tile>& newtile = Simulator::GetTerrain()->getTile( NewPosition );
	if (newtile) hab = newtile->getHabitability(mySpecies);

	/*
	Engine::out() << "Pos: " << Position << std::endl;
	Engine::out() << "NewPos: " << NewPosition << std::endl;
	Engine::out() << "Hab: " << hab << std::endl;
	Engine::out() << "dist: " << Geom::distance( Position, NewPosition) << std::endl;
	Engine::out() << "maxspeed: " << mySpecies->getMaxSpeed() << std::endl;
	*/

	if( Geom::distance( Position, NewPosition) < currentMaxSpeed())
	{
		if( !validPos( NewPosition ) ||  (hab <= 0.0f))
			return false;

		setPosition( NewPosition );
		return true;
	}

	return false;
}

bool Creature::validPos( Geom::Pointf NewPosition ) const
{
	const std::shared_ptr<Tile> newtile = Simulator::GetTerrain()->getTile( NewPosition );

	if (!newtile || newtile->isWater()) return false;
	return true;
}

//################################################
//					VARIOUS
//################################################

/**
 	Calculate Damage from food and water requirement and required temperature
 */
void Creature::calcDamage()
{
	currentHealth -= foodDamage();
	// clipped at 100% ( no bonuses for to much water )
	currentHealth -= waterDamage();
	//--damage from wrong elevation/temperature
	currentHealth-= envDamage();
}

void Creature::die()
{
	setCurrentHealth(-1);
	// remove used nutrition
	if ( mySpecies->getType() == Species::HERBA ) currentTile->addUsedNutrition( -NutritionValue );
}

void Creature::loadConfigValues()
{
	NutritionValue =     Engine::getCfg()->get<float>("sim.creature.NutritionValue");
	huntingThreshold =     Engine::getCfg()->get<float>("sim.creature.huntingThreshold");
	matingThreshold =      Engine::getCfg()->get<float>("sim.creature.mating.HealthThreshold");
	matingAge =            Engine::getCfg()->get<float>("sim.creature.mating.minAge");
	matingHealthCost =     Engine::getCfg()->get<float>("sim.creature.mating.HealthCost");
	migProb =              Engine::getCfg()->get<float>("sim.creature.migProb");
	altModifier1 =         Engine::getCfg()->get<float>("sim.creature.env.altModifier1");
	altModifier2 =         Engine::getCfg()->get<float>("sim.creature.env.altModifier2");
	envMult =              Engine::getCfg()->get<float>("sim.creature.env.Mult");
	ageExponent =          Engine::getCfg()->get<int>("sim.creature.ageExponent");
	plantEnvDmgFactor = Engine::getCfg()->get<float>("sim.creature.env.plantEnvDmgFactor");
}
