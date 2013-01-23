#include "Simulator.hpp"

#include <boost/thread.hpp>

#include "sbe/ResourceManager.hpp"

#include "Terrain.hpp"
#include "Creature.hpp"
#include "Tile.hpp"

#include "resources/CreatureIOPlugin.hpp"
#include "resources/SpeciesIOPlugin.hpp"
#include "resources/TerrainIOPlugin.hpp"

#include <string>

Simulator* Simulator::Instance = nullptr;

Simulator::Simulator():
isPaused(false) {

	Instance = this;

	CreatureCounts[0] = 0;
	CreatureCounts[1] = 0;
	CreatureCounts[2] = 0;

	RegisterForEvent("EVT_TICK");
	RegisterForEvent("EVT_QUIT");

	RegisterForEvent("TOGGLE_SIM_PAUSE");
	RegisterForEvent("SIM_PAUSE");
	RegisterForEvent("SIM_UNPAUSE");
	RegisterForEvent("LOCK_SIM_ON_PAUSE");
	RegisterForEvent("RESET_SIMULATION");

	RegisterForEvent("EVT_SAVE_TERRAIN");
	RegisterForEvent("EVT_SAVE_WHOLE");
	RegisterForEvent("TERRAIN_CLICKED");

	RegisterForEvent("SET_SIM_TPS");

	init();
}

std::shared_ptr<Species>& Simulator::getSpecies( const std::string& name )
{
	for ( auto& S : SpeciesList )
	{
		if ( S->getName() == name) return S;
	}

	return SpeciesList[0];
}

void Simulator::HandleEvent(Event& e)
{
	if(e.Is("EVT_TICK"))
	{
		tick();
	}
	else if(e.Is("SIM_PAUSE"))
	{
		isPaused = true;
	}
	else if(e.Is("SIM_UNPAUSE"))
	{
		isPaused = false;
	}
	else if(e.Is("TOGGLE_SIM_PAUSE"))
	{
		isPaused = !isPaused;
	}
	else if(e.Is("TERRAIN_CLICKED"))
	{
		// check event datatype
		if (e.Data().type() != typeid( Geom::Pointf )) return;
		// cast into desired type
		HandleClick( boost::any_cast<Geom::Pointf>( e.Data() ) );
	}
	else if (e.Is("SET_SIM_TPS"))
	{
		Module::Get()->SetTPS(boost::any_cast<int>(e.Data()));
	}
	else if (e.Is("RESET_SIMULATION"))
	{
		NewSimulation();
	}
	else if (e.Is("EVT_SAVE_TERRAIN"))
	{
		Engine::GetResMgr()->saveObject( "DebugTerrain", Terra, true);
	}
	else if (e.Is("EVT_SAVE_WHOLE")){
		saveEvent(boost::any_cast<std::string>(e.Data()));
	}
	else if (e.Is("EVT_QUIT"))
	{
		Module::Get()->RequestQuit();
	}
}

void Simulator::init()
{
	registerIOPlugins();
	// we have to make sure the renderer is setup before we can send the updateXXrenderlist events
	boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

	NewSimulation();
}

void Simulator::NewSimulation( int seed )
{
	Engine::out(Engine::INFO) << "[Simulator] Seeding random engine" << std::endl;
	Engine::out(Engine::INFO) << "[Simulator] Seed is >> " << boost::lexical_cast<std::string>(seed) << " <<" << std::endl;
	gen.seed( seed );
	currentSeed = seed;

	Engine::out(Engine::INFO) << "[Simulator] Creating Terrain" << std::endl;
	Terra.reset ( new Terrain() );

	Engine::out(Engine::INFO) << "[Simulator] Generating Debug Terrain" << std::endl;
	Terra->CreateDebugTerrain();

	Engine::out(Engine::INFO) << "[Simulator] Creating creatures and species" << std::endl;

	SpeciesList.clear();
	Creatures.clear();

	// add default speciees
	std::shared_ptr<Species> S ( new Species( "UNDEFINED_SPECIES" ));
	SpeciesList.push_back( S );

	CreateSpeciesWithCreatures( Species::HERBA, 		10, 5000 );
	CreateSpeciesWithCreatures( Species::HERBIVORE, 	10, 1000 );
	CreateSpeciesWithCreatures( Species::CARNIVORE, 	10, 100 );

	Engine::out(Engine::INFO) << "[Simulator] Simulation is set up" << std::endl;

	isPaused = true;
	// count Creatures once
	for(auto it = Creatures.begin(); it != Creatures.end(); ++it)
		CreatureCounts[ (int)((*it)->getSpecies()->getType()) ]++;

	// send terrain to renderer
	Terra->UpdateTerrain();
	// send creatures to renderer
	Event e("UpdateCreatureRenderList");
	e.SetData ( Creatures );
	Module::Get()->QueueEvent(e, true);

	RendererUpdate.restart();
}

void Simulator::tick()
{
	if(!isPaused)
	{
		CreatureCounts[0] = 0;
		CreatureCounts[1] = 0;
		CreatureCounts[2] = 0;


		for(auto it = Creatures.begin(); it != Creatures.end();)
		{
			if((*it)->getCurrentHealth() <= 0)
			{
				(*it)->getTile()->removeCreature(*it);
				auto it2 = it++;
				Creatures.erase( it2 );
			}
			else
			{
				//and ya god said live creature !... LIVE !!!
				(*it)->live();
				CreatureCounts[ (int)((*it)->getSpecies()->getType()) ]++;
				++it;
			}
		}
	}

	// update the renderer at up to 30 fps
	if (RendererUpdate.getElapsedTime() > sf::milliseconds(33))
	{
		Module::Get()->DebugString("#Species", boost::lexical_cast<std::string>(SpeciesList.size()));
		Module::Get()->DebugString("#Plants", boost::lexical_cast<std::string>( CreatureCounts[0] ));
		Module::Get()->DebugString("#Herbivores", boost::lexical_cast<std::string>( CreatureCounts[1] ));
		Module::Get()->DebugString("#Carnivores", boost::lexical_cast<std::string>( CreatureCounts[2] ));

		if ( !isPaused )
		{
			Event e("UpdateCreatureRenderList");
			e.SetData ( Creatures );
			Module::Get()->QueueEvent(e, true);
		}

		RendererUpdate.restart();
	}
}


void Simulator::HandleClick( const Geom::Pointf& pos)
{
	// Check if the Coordinates are valid
	if (!(
			(0 <= pos.x && pos.x < Terra->getSize().x + 1 )
		 && (0 <= pos.y && pos.y < Terra->getSize().y +1 )
		))
		return;



	std::shared_ptr<Tile>& T = Terra->getTile( pos );


	Event e("TILE_CLICKED");
	e.SetData( T );
	Module::Get()->QueueEvent(e, true);

	for ( std::shared_ptr<Creature>& C : T->getCreatures())
	{
		if ( Geom::distance(pos, C->getPosition()) < .1 )
		{
			Event e("CREATURE_CLICKED");
			e.SetData( C );
			Module::Get()->QueueEvent(e, true);

			// only return 1 Creature
			return;
		}
	}

}


void Simulator::registerIOPlugins()
{
	Engine::out(Engine::INFO) << "[Simulator] Loading IO plugins:" << std::endl;

	std::shared_ptr<IOPlugin> CreatureIOP ( new CreatureIOPlugin() );
	Engine::GetResMgr()->registerResource<Creature>( iResource::createResInfo("Creature", true, false), CreatureIOP);

	std::shared_ptr<IOPlugin> SpeciesIOP ( new SpeciesIOPlugin() );
	Engine::GetResMgr()->registerResource<Species>( iResource::createResInfo("Species", true, false), SpeciesIOP);

	std::shared_ptr<IOPlugin> TerrainIOP ( new TerrainIOPlugin() );
	Engine::GetResMgr()->registerResource<Terrain>( iResource::createResInfo("Terrain", true, false), TerrainIOP);

	Engine::out(Engine::INFO) << "[Simulator] IO Plugins loaded." << std::endl;
}

void Simulator::CreateSpeciesWithCreatures(  Species::SPECIES_TYPE type, int SpeciesCount, int CreatureCount )
{
	for ( int i = 0; i < SpeciesCount; ++i)
	{
		std::string name = addSpecies( type );
		for ( int j = 0; j < (CreatureCount/SpeciesCount); ++j)
			addCreature( name );
	}
}

void Simulator::addRandomSpecies()
{
	std::uniform_int_distribution<int> type_rnd(0,2);
	std::uniform_int_distribution<int> temp_rnd(-20,40);

	Species::SPECIES_TYPE t = (Species::SPECIES_TYPE) type_rnd(gen);

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

	std::shared_ptr<Species> S ( new Species( name + "_" + boost::lexical_cast<std::string>(SpeciesList.size())) );

	S->setType( t );
	S->setOptimalTemperature( temp_rnd( gen ) );
	SpeciesList.push_back( S );
}

void Simulator::addRandomCreature()
{
	std::uniform_real_distribution<float> rnd(0,32);
	std::uniform_int_distribution<int> species_rnd(0,SpeciesList.size()-1);

	std::shared_ptr<Creature> ptr_creature = std::shared_ptr<Creature>(new Creature( SpeciesList[species_rnd(gen)] ));
	Geom::Pointf Position (rnd(gen),rnd(gen));

	float hab = Simulator::GetTerrain()->getTile(Position)->getHabitability(1,ptr_creature->getSpecies());

	if( hab > 0.0f && ptr_creature->validPos( Position ) )
	{
		ptr_creature->setPosition( Position );
		Creatures.push_back(ptr_creature);
	}
}

void Simulator::addCreature( const std::string& specName )
{
	std::uniform_real_distribution<float> rnd(0,32);

	auto it = std::find_if(SpeciesList.begin(), SpeciesList.end(), [&specName](const std::shared_ptr<Species>& s){ return s->getName() == specName; } );
	if ( it == SpeciesList.end() )
	{
		Engine::out(Engine::ERROR) << "[Simulator::addCreature] Species " << specName << " not found!" << std::endl;
		return;
	}

	std::shared_ptr<Creature> ptr_creature = std::shared_ptr<Creature>(new Creature( *it ));

	// try a few times, but make sure we're not stuck in a lop
	for (int tries = 0; tries < 100; ++tries)
	{
		Geom::Pointf Position (rnd(gen),rnd(gen));

		float hab = Simulator::GetTerrain()->getTile(Position)->getHabitability(1,ptr_creature->getSpecies());
		if( hab > 0.0f && ptr_creature->validPos( Position ) )
		{
			ptr_creature->setPosition( Position );
			Creatures.push_back(ptr_creature);
			return;
		}
	}
}

std::string Simulator::addSpecies( Species::SPECIES_TYPE type )
{
	std::uniform_int_distribution<int> type_rnd(0,2);
	std::uniform_int_distribution<int> temp_rnd(-20,40);

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

	std::shared_ptr<Species> S ( new Species( name + "_" + boost::lexical_cast<std::string>(SpeciesList.size())) );

	S->setType( type );
	S->setOptimalTemperature( temp_rnd( gen ) );
	SpeciesList.push_back( S );

	return S->getName();
}

void Simulator::saveEvent(const std::string &savePath){
	bool wasPaused = isPaused;

	isPaused = true; // pause sim while saving

	if(!savePath.empty())
		Engine::GetIO()->addPath(savePath); // add save path to IO stack

	// do some saving...
	if(!Engine::GetResMgr()->saveObject( "DebugTerrain", Terra, true)){  // should we overwrite?

		Event e("EVT_SAVE_BAD");
		e.SetData( std::string("Error saving Terrain!") );
		Module::Get()->QueueEvent(e, true);

	}	else if(!Engine::GetResMgr()->saveAllObjects<Creature>(true)){

		Event e("EVT_SAVE_BAD");
		e.SetData( std::string("Error saving Creatures!") );
		Module::Get()->QueueEvent(e, true);

	} else if(!Engine::GetResMgr()->saveAllObjects<Species>(true)){ // should we overwrite?

		Event e("EVT_SAVE_BAD");
		e.SetData( std::string("Error saving Species'!") );
		Module::Get()->QueueEvent(e, true);

	} else {

		Event e("EVT_SAVE_GOOD");
		Module::Get()->QueueEvent(e, true);

	}


	if(!savePath.empty())
		Engine::GetIO()->popPath(); // pop save path from IO stack

	isPaused = wasPaused; // continue, if not wasPaused

}
