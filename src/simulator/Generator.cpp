#include "Generator.hpp"

#include "Simulator.hpp"
#include "simulator/world/Creature.hpp"
#include "simulator/world/Terrain.hpp"

#include "sbe/Config.hpp"

Generator::Generator( Simulator& S ) : Sim(S)
{

}

void Generator::CreateSpeciesWithCreatures(  Species::SPECIES_TYPE type, int SpeciesCount, int CreatureCount )
{
	for ( int i = 0; i < SpeciesCount; ++i)
	{
		std::shared_ptr<Species> S = createSpecies( type );
		Sim.SpeciesList.push_back( S );
		for ( int j = 0; j < (CreatureCount/SpeciesCount); ++j)
		{
			std::shared_ptr<Creature> C = createCreature( S );
			if (!C) continue;
			C->updateTileFromPos();
			Sim.Creatures.push_back(C);
		}
	}
}

std::shared_ptr<Species> Generator::createRandomSpecies()
{
	std::uniform_int_distribution<int> type_rnd(0,2);
	std::uniform_int_distribution<int> temp_rnd(Engine::getCfg()->get<int>("sim.species.rnd.temp.min"),Engine::getCfg()->get<int>("sim.species.rnd.temp.max"));

	Species::SPECIES_TYPE t = (Species::SPECIES_TYPE) type_rnd(Sim.rnd());

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

	std::shared_ptr<Species> S ( new Species( name + "_" + boost::lexical_cast<std::string>(Sim.SpeciesList.size())) );

	S->setType( t );
	S->setOptimalTemperature( temp_rnd( Sim.rnd() ) );

	return S;
}

std::shared_ptr<Creature> Generator::createRandomCreature()
{
	std::uniform_real_distribution<float> rnd(0,Sim.Terra->getSize().x);
	std::uniform_int_distribution<int> species_rnd(0,Sim.SpeciesList.size()-1);

	std::shared_ptr<Creature> ptr_creature = std::shared_ptr<Creature>(new Creature( Sim.SpeciesList[species_rnd(Sim.rnd())] ));
	Geom::Pointf Position (rnd(Sim.rnd()),rnd(Sim.rnd()));

	float hab = Simulator::GetTerrain()->getTile(Position)->getHabitability(1,ptr_creature->getSpecies());

	if( hab > 0.0f && ptr_creature->validPos( Position ) )
	{
		ptr_creature->setPositionUnsafe( Position );
		return ptr_creature;
	}

	return std::shared_ptr<Creature>();
}

std::shared_ptr<Creature> Generator::createCreature( const std::string& specName )
{
	auto it = std::find_if(Sim.SpeciesList.begin(), Sim.SpeciesList.end(), [&specName](const std::shared_ptr<Species>& s){ return s->getName() == specName; } );
	if ( it == Sim.SpeciesList.end() )
	{
		Engine::out(Engine::ERROR) << "[Simulator::addCreature] Species " << specName << " not found!" << std::endl;
		return std::shared_ptr<Creature>();
	}

	return createCreature( *it );
}

std::shared_ptr<Creature> Generator::createCreature( const std::shared_ptr<Species>& spec )
{

	std::uniform_int_distribution<int> agedist(0, spec->getMaxAge() );
	std::uniform_int_distribution<int> healthdist(0, spec->getMaxHealth() );
	std::uniform_real_distribution<float> dist(0, Sim.Terra->getSize().x );
	std::shared_ptr<Creature> ptr_creature = std::shared_ptr<Creature>(new Creature( spec ));

	// try a few times, but make sure we're not stuck in a loop
	for (int tries = 0; tries < 1000; ++tries)
	{
		Geom::Pointf Position (dist(Sim.rnd()),dist(Sim.rnd()));

		float hab = Simulator::GetTerrain()->getTile(Position)->getHabitability(1,ptr_creature->getSpecies());
		if( hab > 0.0f && ptr_creature->validPos( Position ) )
		{
			ptr_creature->setCurrentHealth( healthdist( Sim.rnd() ) );
			ptr_creature->setAge( agedist( Sim.rnd() ) );
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

	std::shared_ptr<Species> S ( new Species( name + "_" + boost::lexical_cast<std::string>(Sim.SpeciesList.size())) );

	S->setType( type );
	S->setOptimalTemperature( temp_rnd( Sim.rnd() ) );

	return S;
}
