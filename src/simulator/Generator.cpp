#include "Generator.hpp"

#include "Simulator.hpp"
#include "simulator/world/Creature.hpp"
#include "simulator/world/Terrain.hpp"

#include "sbe/Config.hpp"

Generator::Generator( std::shared_ptr<SimState> _state)
: state(_state), rnd(*_state->seeder)
{

}

void Generator::CreateSpeciesWithCreatures(  Species::SPECIES_TYPE type, int SpeciesCount, int CreatureCount )
{
	for ( int i = 0; i < SpeciesCount; ++i)
	{
		std::shared_ptr<Species> S = createSpecies( type );
		state->species->push_back( S );
		for ( int j = 0; j < (CreatureCount/SpeciesCount); ++j)
		{
			std::shared_ptr<Creature> C = createCreature( S );
			if (!C) continue;
			state->creatures.push_back(C);
		}
	}
}

void Generator::CreateCreatures( std::shared_ptr<std::vector<std::shared_ptr<Species>>> _sp, std::shared_ptr<std::vector<int>> _spc, int _mult )
{
	for( int s = 0; s < _sp->size(); s++)
	{
		for(int c = 0; c < (_mult*(*_spc)[s]); c++)
		{
			std::shared_ptr<Creature> C = createCreature( (*_sp)[s] );
			if (!C) continue;
			state->creatures.push_back(C);
		}
	}

}


std::shared_ptr<Species> Generator::createRandomSpecies()
{
	std::uniform_int_distribution<int> type_rnd(0,2);
	std::uniform_int_distribution<int> temp_rnd(Engine::getCfg()->get<int>("sim.species.rnd.temp.min"),Engine::getCfg()->get<int>("sim.species.rnd.temp.max"));

	Species::SPECIES_TYPE t = (Species::SPECIES_TYPE) type_rnd( rnd );

	std::string name;
	switch (t)
	{
	case Species::SPECIES_TYPE::HERBA:
		name = "plant";
		break;
	case Species::SPECIES_TYPE::HERBIVORE:
		name = "herbivore";
		break;
	case Species::SPECIES_TYPE::CARNIVORE:
		name = "carnivore";
		break;
	}

	std::shared_ptr<Species> S ( new Species( name + "_" + boost::lexical_cast<std::string>(state->species->size()), t) );

	S->setType( t );
	S->setOptimalTemperature( temp_rnd( rnd ) );

	return S;
}

std::shared_ptr<Creature> Generator::createRandomCreature()
{
	std::uniform_real_distribution<float> pos_dist(0,state->terrain->getSize().x);
	std::uniform_int_distribution<int> species_dist(0,state->species->size()-1);

	std::shared_ptr<Creature> ptr_creature = std::shared_ptr<Creature>(new Creature( (*state->species)[species_dist(rnd)] ));
	Geom::Pointf Position (pos_dist(rnd),pos_dist(rnd));

	float hab = state->terrain->getTile(Position)->getHabitability(ptr_creature->getSpecies());

	if( hab > 0.0f && ptr_creature->validPos( Position ) )
	{
		ptr_creature->setPositionUnsafe( Position );
		return ptr_creature;
	}

	return std::shared_ptr<Creature>();
}

std::shared_ptr<Creature> Generator::createCreature( const std::string& specName )
{
	auto it = std::find_if(state->species->begin(), state->species->end(), [&specName](const std::shared_ptr<Species>& s){ return s->getName() == specName; } );
	if ( it == state->species->end() )
	{
		Engine::out(Engine::ERROR) << "[Simulator::addCreature] Species " << specName << " not found!" << std::endl;
		return std::shared_ptr<Creature>();
	}

	return createCreature( *it );
}

std::shared_ptr<Creature> Generator::createCreature( const std::shared_ptr<Species>& spec )
{

	std::uniform_int_distribution<int> age_dist(0, spec->getMaxAge() );
	std::uniform_int_distribution<int> health_dist(0, spec->getMaxHealth() );
	std::uniform_real_distribution<float> pos_dist(0, state->terrain->getSize().x );
	std::shared_ptr<Creature> ptr_creature = std::shared_ptr<Creature>(new Creature( spec ));

	// try a few times, but make sure we're not stuck in a loop
	for (int tries = 0; tries < 1000; ++tries)
	{
		Geom::Pointf Position (pos_dist(rnd),pos_dist(rnd));

        if ( !state->terrain->getTile(Position) ) continue;

		float hab = state->terrain->getTile(Position)->getHabitability(ptr_creature->getSpecies());
		if( hab > 0.0f && ptr_creature->validPos( Position ) )
		{
			ptr_creature->setCurrentHealth( health_dist( rnd ) );
			ptr_creature->setAge( age_dist( rnd ) );

            std::uniform_int_distribution<int> mating_dist( -ptr_creature->getAge(), 0 );
            ptr_creature->setLastMating( mating_dist( rnd ) );

			ptr_creature->setPositionUnsafe( Position );
			return ptr_creature;
		}
	}

    Engine::out(Engine::ERROR) << "[Generator] Unable to find valid Position for Creature!" << std::endl;
	return std::shared_ptr<Creature> ();
}

std::shared_ptr<Creature> Generator::createNonRandomCreature( const std::shared_ptr<Species>& spec )
{
	std::uniform_real_distribution<float> pos_dist(0, state->terrain->getSize().x );
	std::shared_ptr<Creature> ptr_creature = std::shared_ptr<Creature>(new Creature( spec ));

	// try a few times, but make sure we're not stuck in a loop
	for (int tries = 0; tries < 1000; ++tries)
	{
		Geom::Pointf Position (pos_dist(rnd),pos_dist(rnd));

		float hab = state->terrain->getTile(Position)->getHabitability(ptr_creature->getSpecies());
		if( hab > 0.0f && ptr_creature->validPos( Position ) )
		{
			ptr_creature->setCurrentHealth( spec->getMaxHealth() );
			ptr_creature->setAge( 0 );
			ptr_creature->setPositionUnsafe( Position );
			return ptr_creature;
		}
	}

	return std::shared_ptr<Creature> ();
}


std::shared_ptr<Species> Generator::createSpecies( Species::SPECIES_TYPE type )
{
	std::uniform_int_distribution<int> type_rnd(0,2);
	std::uniform_int_distribution<int> temp_rnd(Engine::getCfg()->get<int>("sim.species.rnd.temp.min"),Engine::getCfg()->get<int>("sim.species.rnd.temp.max"));

	std::string name;
	switch (type)
	{
		case Species::SPECIES_TYPE::HERBA:
			name = "plant";
		break;
		case Species::SPECIES_TYPE::HERBIVORE:
			name = "herbivore";
		break;
		case Species::SPECIES_TYPE::CARNIVORE:
			name = "carnivore";
		break;
	}

	std::shared_ptr<Species> S ( new Species( name + "_" + boost::lexical_cast<std::string>(state->species->size()), type) );

	S->setType( type );
	S->setOptimalTemperature( temp_rnd(rnd ) );

	return S;
}
