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

	// boost::this_thread::sleep(boost::posix_time::milliseconds(100));

	RegisterForEvent("EVT_TICK");
	RegisterForEvent("EVT_QUIT");

	RegisterForEvent("TOGGLE_SIM_PAUSE");

	RegisterForEvent("EVT_SAVE_TERRAIN");
	RegisterForEvent("TERRAIN_CLICKED");

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

	Engine::out(Engine::INFO) << "[Simulator] Creating Terrain" << std::endl;

	Terra.reset ( new Terrain() );

	Engine::out(Engine::INFO) << "[Simulator] Generating Debug Terrain" << std::endl;

	Terra->CreateDebugTerrain();

	Engine::out(Engine::INFO) << "[Simulator] Simulation is in paused" << std::endl;

	isPaused = false;


	GetTerrain()->setMaxElevation(1500);

	// add default speciees
	std::shared_ptr<Species> S ( new Species( "UNDEFINED_SPECIES" ));
	SpeciesList.push_back( S );

	for (int i= 0; i < 10; ++i) addRandomSpecies();
	for(int i = 0; i < 1000; ++i) addRandomCreature();


}

void Simulator::tick()
{
//	Engine::out() << "[Simulator] tick" << std::endl;

	if(!isPaused)
	{
		int count = 0;

		for(std::shared_ptr<Creature> c_ptr: Creatures)
		{
			//and ya god said live creature !... LIVE !!!
			c_ptr->live();

			//for debug count Creatures
			count++;
		}

		//Engine::out() << "Living Creatures: " << count << std::endl;

		//make some freaking event, man
		Event e("UpdateCreatureRenderList");
		e.SetData ( Creatures );
		Module::Get()->QueueEvent(e, true);
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

void Simulator::addRandomSpecies()
{
	std::uniform_int_distribution<int> type_rnd(0,2);
	std::uniform_int_distribution<int> temp_rnd(0,255);

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
	ptr_creature->setPosition( Geom::Pointf(rnd(gen),rnd(gen)) );

	Creatures.push_back(ptr_creature);
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
