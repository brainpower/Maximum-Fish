#include "Creature.hpp"

#include <cmath>

#include "sbe/event/Event.hpp"
#include "sbe/Config.hpp"
#include "sbe/geom/PointHelpers.hpp"
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
float Creature::maxAngle = 0;
float Creature::fleeFactor = 0;


Creature::Creature( const std::shared_ptr<Species>& Species)
 : 	done(true),
	currentHealth(100),
	curAgeFactor(0.0),
	age(0),
	lastmating(0),
	causeOfDeath(NONE),
	Position( 0, 0 ),
	prevMove( 0, 0 ),
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

/**
 * copies everything except critical pointers
 */

Creature::Creature(const Creature &o)
	: done(o.done),
	  currentHealth(o.currentHealth),
	  curAgeFactor(o.curAgeFactor),
	  age(o.age),
	  lastmating(o.lastmating),
	  causeOfDeath(o.causeOfDeath),
	  Position(o.Position),
	  prevMove(o.prevMove),
	  mySpecies(o.mySpecies),
	  currentTile(nullptr)
{
}

const std::string& Creature::getSpeciesString() const
{
	return mySpecies->getName();
}

void Creature::setPositionUnsafe( const glm::point2& pos)
{
	Position = pos;
}

void Creature::setPosition( const glm::point2& pos)
{
	prevMove = pos - Position;
	Position = pos;
	updateTileFromPos();
}

void Creature::updateTileFromPos()
{
	updateTileFromPos(Simulator::GetTerrain());
}

void Creature::updateTileFromPos(std::shared_ptr<Terrain> t)
{
	auto& newTile = t->getTile(Position);

	if ( currentTile != newTile)
	{
		if (currentTile) currentTile->removeCreature( shared_from_this() );
		newTile->addCreature( shared_from_this() );
		//if ( mySpecies->getType() == Species::HERBA ) newTile->addUsedNutrition( NutritionValue );
		currentTile = newTile;
	}
}

void Creature::movePosition( const glm::point2& pos)
{
	setPosition( Position + pos );
}

void Creature::live()
{
	if ( done ) return;
	done = true;
	recalcAgeFactor();

	// damage from environment
	resistance = healthPercentage() * currentResistance();
	calcDamage();

	bool didsomethingthistick = false;
	//std::list<std::shared_ptr<Creature>> nearby = Simulator::GetTerrain()->getNearby(this->getPosition(), 2.0);

	if(mySpecies->getType() == Species::HERBIVORE)
	{
		std::shared_ptr<Creature> nearest = Simulator::GetTerrain()->getNearest(Position, mySpecies->getReach(), Species::CARNIVORE);
		if(nearest) didsomethingthistick = moveTo(Position + glm::normalize(Position - nearest->getPosition())*currentMaxSpeed());
	}


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

	if ( !didsomethingthistick ) {
/*		if ( mySpecies->getType() == Species::CARNIVORE )
		{

			std::shared_ptr<Creature> nearest = Simulator::GetTerrain()->getNearest(Position, mySpecies->getReach(), Species::HERBIVORE);
			if(nearest)
				didsomethingthistick = moveTo(Position + glm::normalize(nearest->getPosition() - Position)*currentMaxSpeed());
		}
		else*/
			move();
	}

	age++;
}

//################################################
//					HUNTING
//################################################

bool Creature::huntFood()
{
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
		switch(mySpecies->getType())
		{
			case Species::CARNIVORE:
				// consume our prey
				currentHealth += nearest->getCurrentHealth();
				if ( currentHealth > currentMaxHealth() ) currentHealth = currentMaxHealth();
				nearest->die(EATEN);
				break;

			case Species::HERBIVORE:
				float diff = currentMaxHealth() - currentHealth;
				float nearestHealth = nearest->getCurrentHealth();
				if ( diff > nearest->getCurrentHealth() )
				{
					currentHealth += nearestHealth;
					nearest->die(EATEN);
				}
				else
				{
					currentHealth += diff;
					nearest->setCurrentHealth(nearestHealth-diff);
				}
		}
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
	glm::point2 newPosition = Position;

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
	newborn->recalcAgeFactor();

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

	if(hab < 10 || rnd(Simulator::GetRnd()) < migProb)
		for (int i = 0; i < 15; ++i)
			if (randomMove()) return;

	// if no move was made
	prevMove.x = 0;
	prevMove.y = 0;
}


bool Creature::moveTo( glm::point2 Target )
{
	// move as far in direction of the target as possible
	glm::vec2 direction = glm::normalize( Target - Position );
	direction *= glm::vec2(mySpecies->getMaxSpeed(), currentMaxSpeed());
	direction += Position;

	if ( glm::distance( Target, Position ) > currentMaxSpeed() )
	{
		if ( validPos( direction ) )
			setPosition( direction );
		return false;
	// move to the target
	} else {
		if ( validPos( direction ) )
		{
			setPosition( Target );
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool Creature::randomMove()
{
	glm::point2 NewPosition = Position;

	NewPosition = getNewPosition();

	float hab = 1;
	const std::shared_ptr<Tile>& newtile = Simulator::GetTerrain()->getTile( NewPosition );

	if (newtile) hab = newtile->getHabitability(mySpecies);

	if ( 	hab <= 0.0f
		|| glm::distance( Position, NewPosition) > currentMaxSpeed()
		|| !validPos( NewPosition )
		) return false;

	setPosition( NewPosition );
	return true;
}

glm::vec2 Creature::getNewPosition()
{
	std::uniform_real_distribution<float> rndNoDir( -currentMaxSpeed() , currentMaxSpeed() );
	std::uniform_real_distribution<float> rnd( 0 , currentMaxSpeed() );
	glm::vec2 newPos;

	if(prevMove.x == 0 && prevMove.y == 0)
	{
		newPos.x = Position.x + rndNoDir(Simulator::GetRnd());
		newPos.y = Position.y + rndNoDir(Simulator::GetRnd());
	}
	else
	{
		std::uniform_real_distribution<float> rndAngle( -maxAngle , maxAngle );
		int angle = (int)(rndAngle(Simulator::GetRnd()));
		glm::vec2 unitLast = glm::normalize(prevMove);

		float spd = std::abs(rnd(Simulator::GetRnd()));

		float currentAngle = std::acos(unitLast.x);

		newPos.x = Position.x + (std::cos(currentAngle + (float)angle) * spd);
		newPos.y = Position.y + (std::sin(currentAngle + (float)angle) * spd);
	}

	return newPos;
}

bool Creature::validPos( glm::point2 NewPosition ) const
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
	if ( currentHealth < 0 ) {
		if(age > old * mySpecies->getMaxAge()) die(OLD);
		else die(STARVED);
	}

	// clipped at 100% ( no bonuses for to much water )
	currentHealth -= waterDamage();
	if ( currentHealth < 0 ) {
		if(age > old * mySpecies->getMaxAge()) die(OLD);
		else die(THIRST);
	}

	//--damage from wrong elevation/temperature
	currentHealth-= envDamage();
	if ( currentHealth < 0 )
	{
		if(age > old * mySpecies->getMaxAge()) die(OLD);
		else die(FROZEN);
	}
}

void Creature::die(CauseOfDeath cod)
{
	setCurrentHealth(-1);
	// make sure we won't be simulated later
	causeOfDeath = cod;
	done = true;
	// remove used nutrition
	//if ( mySpecies->getType() == Species::HERBA ) currentTile->addUsedNutrition( -NutritionValue );
}

void Creature::loadConfigValues()
{
	NutritionValue =       Engine::getCfg()->get<float>("sim.creature.NutritionValue");
	huntingThreshold =     Engine::getCfg()->get<float>("sim.creature.huntingThreshold");
	matingThreshold =      Engine::getCfg()->get<float>("sim.creature.mating.HealthThreshold");
	matingAge =            Engine::getCfg()->get<float>("sim.creature.mating.minAge");
	matingHealthCost =     Engine::getCfg()->get<float>("sim.creature.mating.HealthCost");
	migProb =              Engine::getCfg()->get<float>("sim.creature.migProb");
	altModifier1 =         Engine::getCfg()->get<float>("sim.creature.env.altModifier1");
	altModifier2 =         Engine::getCfg()->get<float>("sim.creature.env.altModifier2");
	envMult =              Engine::getCfg()->get<float>("sim.creature.env.Mult");
	ageExponent =          Engine::getCfg()->get<int>("sim.creature.ageExponent");
	plantEnvDmgFactor =    Engine::getCfg()->get<float>("sim.creature.env.plantEnvDmgFactor");
	maxAngle =             Engine::getCfg()->get<float>("sim.creature.maxAngle");
	fleeFactor =           Engine::getCfg()->get<float>("sim.creature.fleeFactor");
}
