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
m_pause(false) {

	boost::this_thread::sleep(boost::posix_time::milliseconds(100));

	RegisterForEvent("EVT_TICK");
	RegisterForEvent("EVT_QUIT");

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
		if(m_pause == false)
		{
			m_pause = true;
		} else {
			m_pause = false;
		}
	}
	else if (e.Is("EVT_SAVE_TERRAIN"))
	{
		Engine::GetResMgr()->saveObject( "DebugTerrain", Terra);
	}
	else if (e.Is("EVT_QUIT"))
	{
		Module::Get()->RequestQuit();
	}
}

void Simulator::init()
{

	registerIOPlugins();

	Engine::out() << "[Simulator] Creating Terrain" << std::endl;

	Terra.reset ( new Terrain() );

	Engine::out() << "[Simulator] Generating Debug Terrain" << std::endl;

	Terra->CreateDebugTerrain();

	Engine::out() << "[Simulator] Simulation is in paused" << std::endl;

	m_pause = true;
}

void Simulator::tick()
{
//	Engine::out() << "[Simulator] tick" << std::endl;

	while(!m_pause)
	{
		for(std::shared_ptr<Creature> c_ptr: Creatures)
		{


			//and ya god said live creature !... LIVE !!!
			c_ptr->live();
		}

				//make some freaking event, man
		Event e("UpdateCreatureRenderList");
		e.SetData ( Creatures );
		Module::Get()->QueueEvent(e, true);
	}
}

void Simulator::registerIOPlugins()
{
	Engine::out() << "[Simulator] Loading IO plugins:" << std::endl;

	std::shared_ptr<IOPlugin> CreatureIOP ( new CreatureIOPlugin() );
	Engine::GetResMgr()->registerResource<Creature>( iResource::createResInfo("Creature", true, false), CreatureIOP);

	std::shared_ptr<IOPlugin> SpeciesIOP ( new SpeciesIOPlugin() );
	Engine::GetResMgr()->registerResource<Species>( iResource::createResInfo("Species", true, false), SpeciesIOP);

	std::shared_ptr<IOPlugin> TerrainIOP ( new TerrainIOPlugin() );
	Engine::GetResMgr()->registerResource<Terrain>( iResource::createResInfo("Terrain", true, false), TerrainIOP);

	Engine::out() << "[Simulator] IO Plugins loaded." << std::endl;
}

