#include "Simulator.hpp"

#include <boost/thread.hpp>

#include "sbe/ResourceManager.hpp"

#include "Terrain.hpp"
#include "Creature.hpp"
#include "Tile.hpp"

#include "resources/CreatureIOPlugin.hpp"
#include "resources/SpeciesIOPlugin.hpp"
#include "resources/TerrainIOPlugin.hpp"

Simulator* Simulator::Instance = nullptr;

Simulator::Simulator():
isPaused(false) {

	Instance = this;

	boost::this_thread::sleep(boost::posix_time::milliseconds(100));

	RegisterForEvent("EVT_TICK");
	RegisterForEvent("EVT_QUIT");

	RegisterForEvent("TOGGLE_SIM_PAUSE");

	RegisterForEvent("EVT_SAVE_TERRAIN");

	init();
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
	else if (e.Is("EVT_SAVE_TERRAIN"))
	{
		Engine::GetResMgr()->saveObject( "DebugTerrain", Terra, true);
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
	//TEST########################
	std::uniform_real_distribution<float> rnd(0, 32);

	std::shared_ptr<Species> ptr_species = std::shared_ptr<Species>(new Species("Deimuada"));
	ptr_species->setMaxSpeed(0.09);
	ptr_species->setFoodRequirement(10);
	ptr_species->setWaterRequirement(10);
	ptr_species->setOptimalTemperature(20);

	SpeciesList.push_back(ptr_species);
	//END TEST####################

	for(int i = 0; i < 1000; i++)
	{
		addCreature();

	}

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

//
//void Simulator::HandleClick( const Geom::Pointf& Pos)
//{
//	Event e("TILE_CLICKED");
//
//	int index = (Pos.x / TileSize) * TerrainSize + (Pos.y / TileSize) ;
//	e.SetData(CullTerrain[ index ]);
//
//	Module::QueueEvent(e);
//}


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

void Simulator::addCreature()
{
	std::uniform_real_distribution<float> rnd(0,32);
	std::shared_ptr<Creature> ptr_creature = std::shared_ptr<Creature>(new Creature());
	ptr_creature->setPosition(rnd(gen),rnd(gen));
	ptr_creature->setSpecies(SpeciesList.front());
	ptr_creature->setCurrentTile(Simulator::GetTerrain()->getTile(ptr_creature->getPosition()));

	Creatures.push_back(ptr_creature);
}
