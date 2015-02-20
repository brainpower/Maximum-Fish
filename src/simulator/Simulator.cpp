#include "Simulator.hpp"

#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

#include "sbe/ResourceManager.hpp"
#include "sbe/gfx/GraphPlotter.hpp"
#include "sbe/geom/PointHelpers.hpp"
#include "sbe/sfg/Message.hpp"

#include "sbe/Config.hpp"

#include "world/Terrain.hpp"
#include "world/Creature.hpp"
#include "world/Tile.hpp"

#include "Generator.hpp"
#include "StasisPod.hpp"

#include "resources/CreatureIOPlugin.hpp"
#include "resources/SpeciesIOPlugin.hpp"
#include "resources/TerrainIOPlugin.hpp"
#include "resources/SimStateIOPlugin.hpp"
#include "resources/StasisPodIOPlugin.hpp"

#include <SFML/System/Clock.hpp>

#include <iterator>
#include <string>
#include <sstream>
#include <random>
#include <tuple>

#include <iostream>
#include <fstream>
#include <sstream>


Simulator* Simulator::Instance = nullptr;

Simulator::Simulator() : isPaused(true), isInitialized(false) {

	Instance = this;

	RegisterForEvent("EVT_TICK");
	RegisterForEvent("EVT_QUIT");

	RegisterForEvent("TOGGLE_SIM_PAUSE");
	RegisterForEvent("SIM_PAUSE");
	RegisterForEvent("SIM_UNPAUSE");
	RegisterForEvent("RESET_SIMULATION");

	RegisterForEvent("EVT_SAVE_TERRAIN");
	RegisterForEvent("EVT_SAVE_WHOLE");
	RegisterForEvent("EVT_SAVE_WHOLE_TEST");
	RegisterForEvent("EVT_LOAD_WHOLE_TEST");
	RegisterForEvent("EVT_LOAD_WHOLE");
	RegisterForEvent("TERRAIN_CLICKED");
	RegisterForEvent("SIM_FORWARD_TO_TICK");

	RegisterForEvent("SET_SIM_TPS");

	RegisterForEvent("UPDATE_OVERLAYS");
	RegisterForEvent("NEW_NONRANDOM_SIM");

	init();
	reset();
}


void Simulator::HandleEvent(Event& e)
{
	if(e.Is("EVT_TICK") && isInitialized )
	{
		advance();
	}
	else if(e.Is("SIM_PAUSE") && isInitialized )
	{
		isPaused = true;
		Engine::getCfg()->set("sim.paused", isPaused);
		UpdateCreatureRenderList();
	}
	else if(e.Is("SIM_UNPAUSE") && isInitialized )
	{
		isPaused = false;
		Engine::getCfg()->set("sim.paused", isPaused);

		if ( e.Data().type() == typeid(unsigned int))
		{
			TicksToSim =  boost::any_cast<unsigned int>(e.Data());
			TickTimer.restart();
			simulateTicks = TicksToSim;
		}
	}
	else if(e.Is("TOGGLE_SIM_PAUSE") && isInitialized )
	{
		isPaused = !isPaused;
		Engine::getCfg()->set("sim.paused", isPaused);
		if (isPaused) UpdateCreatureRenderList();
	}
	else if(e.Is("TERRAIN_CLICKED", typeid( glm::point2 )) && isInitialized )
	{
		// cast into desired type
		HandleClick( boost::any_cast<glm::point2>( e.Data() ) );
	}
	else if (e.Is("SET_SIM_TPS", typeid(unsigned int)))
	{
		Module::Get()->SetTPS(boost::any_cast<unsigned int>(e.Data()));
	}
	else if ( e.Is( "UPDATE_OVERLAYS" ) && isInitialized  )
	{
		state->terrain->CreateMapPlotters();
	}
	else if (e.Is("RESET_SIMULATION"))
	{

	    if ( !m_specs || !m_counts)
        {
            std::shared_ptr<sbe::Message> m( new sbe::Message(sbe::Message::OK, "No Simulation started.", "Please start a Simulation via 'New simulation' from the Main Menu before resetting." ));

            Module::Get()->QueueEvent( Event("NEW_MESSAGE", m), true );
            return;
        }

		bool wasPaused = isPaused;
		isPaused = true;
		Engine::getCfg()->set("sim.paused", isPaused);

		NewSimulation( m_specs, m_counts, Engine::getCfg()->get<int>("sim.defaultSeed"));

		isPaused = wasPaused;
		Engine::getCfg()->set("sim.paused", isPaused);
	}
	else if (e.Is("EVT_SAVE_TERRAIN") && isInitialized )
	{
		Engine::GetResMgr()->saveObject( "DebugTerrain", state->terrain, true);
	}
	else if (e.Is("EVT_SAVE_WHOLE", typeid(std::string)) ) {
		saveWhole(boost::any_cast<std::string>(e.Data()));
	}
	else if (e.Is("EVT_LOAD_WHOLE", typeid(std::string))) {
		loadWhole(boost::any_cast<std::string>(e.Data()));
	}
	else if (e.Is("EVT_QUIT"))
	{
		Module::Get()->RequestQuit();
	}
	else if (e.Is("SIM_FORWARD_TO_TICK")){
		forwardTo(boost::any_cast<int>(e.Data()));
	}
	else if(e.Is("NEW_NONRANDOM_SIM"))
	{
		if ( e.Data().type() == typeid(std::pair<std::shared_ptr<std::vector<std::shared_ptr<Species>>>, std::shared_ptr<std::vector<int>>>));
		{
			auto t_pair = boost::any_cast<std::pair<std::shared_ptr<std::vector<std::shared_ptr<Species>>>, std::shared_ptr<std::vector<int>>>>(e.Data());
			bool wasPaused = isPaused;
			isPaused = true;
			Engine::getCfg()->set("sim.paused", isPaused);

			NewSimulation(t_pair.first, t_pair.second, Engine::getCfg()->get<int>("sim.defaultSeed"));

			isPaused = wasPaused;
			Engine::getCfg()->set("sim.paused", isPaused);
			m_specs = t_pair.first;
			m_counts = t_pair.second;
		}
	}
}

void Simulator::init()
{
	registerIOPlugins();


	_pod.reset(new StasisPod());
	_freezeRate = Engine::getCfg()->get<int>("sim.freezeRate");

	tid.reset(new int(-1));

	//NewSimulation(Engine::getCfg()->get<int>("sim.defaultSeed"));
}


void Simulator::NewSimulation( std::shared_ptr<std::vector<std::shared_ptr<Species>>> spec, std::shared_ptr<std::vector<int>> specnum, int seed )
{
	std::shared_ptr<SimState> newstate(new SimState());

    newstate->numThreads = Engine::getCfg()->get<int>("sim.numThreads");

    newstate->seeder.reset( new std::mt19937(seed) );

	Engine::out(Engine::INFO) << "[Simulator] Seed is >> " << boost::lexical_cast<std::string>(seed) << " <<" << std::endl;
	newstate->currentSeed = seed;

    std::uniform_int_distribution<int> seed_dist;
	for ( int thread = 0; thread < newstate->numThreads; ++thread)
		newstate->gens.push_back(std::shared_ptr<std::mt19937>( new std::mt19937(seed_dist(*(newstate->seeder))) ));

	Engine::out(Engine::INFO) << "[Simulator] Creating Terrain" << std::endl;
	newstate->terrain.reset( new Terrain() );
	newstate->species = spec;

	// we have to set it here, so Simulator::GetTerrain() will work in Tile
	//this->setnewstate(newstate, true);
	state = newstate;

	newstate->terrain->CreateDebugTerrain( state->currentSeed );

    Engine::out(Engine::INFO) << "[Simulator] Creatures and species" << std::endl;
	Generator G (newstate);

	int countMult =  Engine::getCfg()->get<int>("sim.terragen.count");
	G.CreateCreatures(spec, specnum, countMult);

	NewSimulation( newstate );
}

void Simulator::NewSimulation( std::shared_ptr<SimState> newstate)
{
    reset();
    Module::Get()->QueueEvent("RESET_UI", true);


	if ( newstate->numThreads > 1 )
	{
		stopThreads();
		initThreads();
	}

	newstate->currentTick = 0;
	newstate->numGenerated = 0;

	Creature::loadConfigValues();

	// count Creatures once
	for( auto C : newstate->creatures )
	{
		Counts[ (int)(C->getSpecies()->getType()) ].back()++;
		C->updateTileFromPos();
	}

	Engine::out(Engine::INFO) << "[Simulator] Simulation is set up" << std::endl;




	// categorize tiles for parallel computation of the simulation
	newstate->terrain->CreateParallelisationGraph();

	// send terrain to renderer
	newstate->terrain->UpdateTileMap();
	newstate->terrain->UpdateTerrain();
	// send creatures to renderer
	UpdateCreatureRenderList();

	isInitialized = true;

	newstate->terrain->CreateMapPlotters();

	RendererUpdate.restart();


	CreatePlotters();

	_pod->clear();
	_pod->freeze(newstate);
}

void Simulator::reset()
{
    resetStats();
    simulateTicks = 0;
	TicksToSim = 0;
    isInitialized = false;

    isPaused = Engine::getCfg()->get<bool>("sim.pauseOnStart");
	Engine::getCfg()->set("sim.paused", isPaused);
}



void Simulator::initThreads()
{
	startBarrier.reset( new boost::barrier( state->numThreads+1) );
	endBarrier.reset( new boost::barrier( state->numThreads+1 ) );

	for ( int thread = 0; thread < state->numThreads; ++thread)
	{
		std::shared_ptr<std::list<std::shared_ptr<Tile>>> cur;
		cur.reset ( new std::list<std::shared_ptr<Tile>> );
		CurrentLists.push_back(cur);
		cur.reset ( new std::list<std::shared_ptr<Tile>> );
		NextLists.push_back(cur);

		threads.push_back( std::shared_ptr<boost::thread>( new boost::thread(
		    boost::bind( &Simulator::thread, this, CurrentLists[thread], thread ))));
	}
}

void Simulator::stopThreads()
{
	for ( std::shared_ptr<boost::thread> t : threads )
	{
		//Engine::out() << "Joining Thread " << thread << ": " << BatchTimer.getElapsedTime().asMilliseconds() << " ms" << std::endl;
		t->interrupt();
		t->join();
	}
	threads.clear();
	CurrentLists.clear();
	NextLists.clear();
}

void Simulator::advance()
{
	if(!isInitialized) return;

	if(!isPaused)
	{
		// last tick?
		if (simulateTicks == 1 )
		{
			isPaused = true;
			// dont notify on single-step
			if ( TicksToSim > 1 )
			{
				// remove "forwarding" message.
				Module::Get()->QueueEvent( Event("CLOSE_MESSAGE", std::string("Forwarding.") ), true );

				std::string text = boost::lexical_cast<std::string>( TicksToSim ) + " Ticks Simulated in " + boost::lexical_cast<std::string>(TickTimer.restart().asSeconds()) + " s.";
				std::shared_ptr<sbe::Message> m( new sbe::Message(sbe::Message::OK, "Simulation Stopped", text, "PAUSELOCK_DOWN", true ));

				Module::Get()->QueueEvent( Event("NEW_MESSAGE", m), true );
			}

			UpdateCreatureRenderList();
			SendScaleUpdate();
		}
		if ( simulateTicks > 0 ) simulateTicks--;


		if ( state->numThreads > 1 )
		{
			parallelTick();
		}
		else
		{
			for( auto& batch : state->terrain->getColors())
                tick( batch );

            // remove dead creatures, log stats
            CleanupTick();
		}

		state->currentTick++;

		if(! (state->currentTick % _freezeRate))
			_pod->freeze(state);

	}

	// update the renderer at up to 30 fps
	if (RendererUpdate.getElapsedTime() > sf::milliseconds(33))
	{
		SendScaleUpdate();

		Module::Get()->DebugString("#Species", boost::lexical_cast<std::string>(state->species->size()));
		Module::Get()->DebugString("#Plants", boost::lexical_cast<std::string>( Counts[ Species::HERBA ].back() ));
		Module::Get()->DebugString("#Herbivores", boost::lexical_cast<std::string>( Counts[ Species::HERBIVORE ].back() ));
		Module::Get()->DebugString("#Carnivores", boost::lexical_cast<std::string>( Counts[ Species::CARNIVORE ].back() ));
		Module::Get()->DebugString("#Tick", boost::lexical_cast<std::string>( state->currentTick ));
//		Module::Get()->DebugString("#rnds", boost::lexical_cast<std::string>( state->numGenerated ));
//		Module::Get()->DebugString("Dead - Eaten", boost::lexical_cast<std::string>( EatenCounts.back() ));
//		Module::Get()->DebugString("Dead - Starved", boost::lexical_cast<std::string>( StarvedCounts.back() ));
//		Module::Get()->DebugString("Dead - Thirst", boost::lexical_cast<std::string>( ThirstCounts.back() ));
//		Module::Get()->DebugString("Dead - Frozen", boost::lexical_cast<std::string>( FrozenCounts.back() ));
//		Module::Get()->DebugString("Dead - Old", boost::lexical_cast<std::string>( OldCounts.back() ));

		if ( !isPaused )
		{
			UpdateCreatureRenderList();
			if (Engine::getCfg()->get<bool>("system.ui.Overlays.live")) state->terrain->CreateMapPlotters();
		}

		RendererUpdate.restart();
	}

	if ( !isPaused)
	{
		// log stats and reset counters
		logTickStats();
	}
}

void Simulator::thread(std::shared_ptr<std::list<std::shared_ptr<Tile>>> list, const int _tid)
{
	tid.reset(new int(_tid));

	while ( !boost::this_thread::interruption_requested() )
	{
		startBarrier->wait();

		tick( *list );

		endBarrier->wait();
	}
}


void Simulator::tick(std::list<std::shared_ptr<Tile>>& list)
{
    //int creatures = 0;

	for(auto it = list.cbegin(); it != list.cend(); it++)
	{
	    // skip placeholders ( only required in singlethreaded mode, im MT mode lists are splitted at the dividers )
	    if ( !(*it) ) continue;

		auto Cs = (*it)->getCreatures();
		for ( auto it2 = Cs.cbegin(); it2 != Cs.cend(); it2++)
			if (*it2 && (*it2)->getCurrentHealth() > 0)
            {
                (*it2)->live();
                //creatures++;
            }

	}

//    std::stringstream ss;
//    ss << "Thread " << *tid << " got " << list.size() <<"/" << creatures << " Tiles/Creatures " << std::endl;
//    Engine::out() << ss.str();

}


void Simulator::parallelTick()
{

	//  makes sure our threads are waiting at the "endBarrier" needed for the loop
	// do this by starting an empty batch
	for ( int thread = 0; thread < state->numThreads; ++thread)
		CurrentLists[thread]->clear();
	startBarrier->wait();

	for( auto& batch : state->terrain->getColors())
	{

		for ( int thread = 0; thread < state->numThreads; ++thread)
			NextLists[thread]->clear();

		// prepare the next batch of work
		int curthread = 0;
		for ( auto& T : batch )
		{
			if ( !T ) curthread = (curthread+1)% state->numThreads;
			else 	  NextLists[curthread]->push_back(T);
		}

		// wait till the threads are finished
		endBarrier->wait();

        CleanupTick();

		// swap the lists
		for ( int thread = 0; thread < state->numThreads; ++thread )
			CurrentLists[thread]->swap( *(NextLists[thread]) );

		startBarrier->wait();
	}

	// make sure we correctly stop the last batch
	endBarrier->wait();

    CleanupTick();

}

void Simulator::CleanupTick()
{

	// cleanup dead creatures
	for(auto it = state->creatures.begin(); it != state->creatures.end(); )
	{
		int type = (int)((*it)->getSpecies()->getType());
		if ((*it)->getCurrentHealth() <= 0)
		{
			MeansOfDeathCounts[ (*it)->getCauseOfDeath() ].back()++;
			DeathCounts[ type ].back()++;

			(*it)->getTile()->removeCreature(*it);
			auto it2 = it++;
			state->creatures.erase( it2 );
		}
		else
        {
			Counts[ type ].back()++;
			if ( (*it)->getAge() == 0)
			{
				BirthCounts[ type ].back()++;
			}

			(*(++it))->done = false;
		}
	}



}

void Simulator::logTickStats()
{
	if(!isInitialized) return;

	CountGraph->updateCurveData( "Herbs", 		Counts[ Species::HERBA ]  );
	CountGraph->updateCurveData( "Herbivore", 	Counts[ Species::HERBIVORE ] );
	CountGraph->updateCurveData( "Carnivore", 	Counts[ Species::CARNIVORE ] );

	DeathGraph->updateCurveData("Dead - Eaten", 	MeansOfDeathCounts[ Creature::EATEN ]);
	DeathGraph->updateCurveData("Dead - Frozen", 	MeansOfDeathCounts[ Creature::FROZEN ]);
	DeathGraph->updateCurveData("Dead - Starved", 	MeansOfDeathCounts[ Creature::STARVED ]);
	DeathGraph->updateCurveData("Dead - Thirst", 	MeansOfDeathCounts[ Creature::THIRST ]);
	DeathGraph->updateCurveData("Dead - Old", 		MeansOfDeathCounts[ Creature::OLD ]);
	DeathGraph->updateCurveData("Dead - None", 		MeansOfDeathCounts[ Creature::NONE ]);

	BirthDeathGraph->updateCurveData("Deaths - Herbae", 		DeathCounts[ Species::HERBA ]);
	BirthDeathGraph->updateCurveData("Births - Herbae", 		BirthCounts[ Species::HERBA ]);
	BirthDeathGraph->updateCurveData("Deaths - Herbivores", 	DeathCounts[ Species::HERBIVORE ]);
	BirthDeathGraph->updateCurveData("Births - Herbivores", 	BirthCounts[ Species::HERBIVORE ]);
	BirthDeathGraph->updateCurveData("Deaths - Carnivores", 	DeathCounts[ Species::CARNIVORE ]);
	BirthDeathGraph->updateCurveData("Births - Carnivores", 	BirthCounts[ Species::CARNIVORE ]);

	// add new counter for the next tick
    for ( int i = 0; i <= Creature::NONE; ++i)
	{
		MeansOfDeathCounts[i].push_back(0);
	}


	for ( int i = 0; i < 3; ++i)
	{
		Counts[i].push_back(0);
		BirthCounts[i].push_back(0);
		DeathCounts[i].push_back(0);
	}

	//ProcessingTimes.push_back(  )
}

void Simulator::resetStats()
{

    for ( int i = 0; i <= Creature::NONE; ++i)
	{
		MeansOfDeathCounts[i].clear();
		MeansOfDeathCounts[i].push_back(0);
	}

	for ( int i = 0; i < 3; ++i)
	{
		Counts[i].clear();
		Counts[i].push_back(0);

		BirthCounts[i].clear();
		BirthCounts[i].push_back(0);

		DeathCounts[i].clear();
		DeathCounts[i].push_back(0);
	}
}

void Simulator::CreatePlotters()
{
	if(!isInitialized) return;

    Engine::out() << "[Simulator] Creating Graphs for GraphBook" << std::endl;

	CountGraph.reset(new sbe::GraphPlotter );
	DeathGraph.reset(new sbe::GraphPlotter );
	BirthDeathGraph.reset(new sbe::GraphPlotter );

	glm::ipoint2 Size = glm::ipoint2( Engine::getCfg()->get<int>("sim.plots.size.x"),Engine::getCfg()->get<int>("sim.plots.size.y") );

	{
        sbe::Graph g;
        g.Size = Size;
        g.AxisSize = glm::ipoint2( 1000, 1000 );
        g.addCurve( sbe::Curve("Herbs", 	Counts[ Species::HERBA ] , sf::Color::Green) );
        g.addCurve( sbe::Curve("Herbivore", Counts[ Species::HERBIVORE ], sf::Color::Blue) );
        g.addCurve( sbe::Curve("Carnivore", Counts[ Species::CARNIVORE ], sf::Color::Red) );
		CountGraph->setGraph( g );
	}

	{
		sbe::Graph g;
        g.Size = Size;
        g.AxisSize = glm::ipoint2( 1000, 1000 );
        g.addCurve( sbe::Curve("Dead - Eaten", 		MeansOfDeathCounts[ Creature::EATEN ], sf::Color::Black) );
        g.addCurve( sbe::Curve("Dead - Frozen", 	MeansOfDeathCounts[ Creature::FROZEN ], sf::Color::Cyan) );
        g.addCurve( sbe::Curve("Dead - Starved", 	MeansOfDeathCounts[ Creature::STARVED ], sf::Color::Yellow) );
        g.addCurve( sbe::Curve("Dead - Thirst", 	MeansOfDeathCounts[ Creature::THIRST ], sf::Color::Magenta) );
        g.addCurve( sbe::Curve("Dead - Old", 		MeansOfDeathCounts[ Creature::OLD ], sf::Color(0, 80, 0)) );
        g.addCurve( sbe::Curve("Dead - NONE", 		MeansOfDeathCounts[ Creature::NONE ], sf::Color(0, 102, 50)) );
		DeathGraph->setGraph( g );
	}

	{
        sbe::Graph g;
        g.Size = Size;
        g.AxisSize = glm::ipoint2( 1000, 1000 );
        g.addCurve( sbe::Curve("Deaths - Herbae", 		DeathCounts[ (int)Species::HERBA ], sf::Color( 102, 0, 0) ) );
        g.addCurve( sbe::Curve("Births - Herbae", 		BirthCounts[ (int)Species::HERBA ], sf::Color( 255, 0, 0) ) );
        g.addCurve( sbe::Curve("Deaths - Herbivores", 	DeathCounts[ (int)Species::HERBIVORE ], sf::Color( 0, 102, 0) ) );
        g.addCurve( sbe::Curve("Births - Herbivores", 	BirthCounts[ (int)Species::HERBIVORE ], sf::Color( 0, 255, 0) ) );
        g.addCurve( sbe::Curve("Deaths - Carnivores", 	DeathCounts[ (int)Species::CARNIVORE ], sf::Color( 0, 0, 102) ) );
        g.addCurve( sbe::Curve("Births - Carnivores", 	BirthCounts[ (int)Species::CARNIVORE ], sf::Color( 0, 0, 255) ) );
		BirthDeathGraph->setGraph( g );
	}

    //Engine::out() << "[Simulator] Queueing Population graph" << std::endl;
	auto data = std::make_pair( std::string( "Population" ), CountGraph );
	Module::Get()->QueueEvent(Event("ADD_GRAPH_TO_BOOK", data), true);

    //Engine::out() << "[Simulator] Queueing Population graph" << std::endl;
	auto data2 = std::make_pair( std::string( "Population Changes" ), BirthDeathGraph );
	Module::Get()->QueueEvent(Event("ADD_GRAPH_TO_BOOK", data2), true);

    //Engine::out() << "[Simulator] Queueing MoD graph" << std::endl;
	auto data3 = std::make_pair( std::string( "Means of Death" ), DeathGraph );
	Module::Get()->QueueEvent(Event("ADD_GRAPH_TO_BOOK", data3), true);

}

void Simulator::HandleClick( const glm::point2& pos)
{
	if( !isInitialized ) return;

	const std::shared_ptr<Tile>& T = state->terrain->getTile( pos );

	// Check if the Coordinates are valid
	if (!T)
	{
		Module::Get()->QueueEvent(Event("TILE_CLICKED", std::shared_ptr<Tile>()), true);
		Module::Get()->QueueEvent(Event("CREATURE_CLICKED", std::shared_ptr<Creature>()), true);
		boost::this_thread::sleep(boost::posix_time::milliseconds(50));
		UpdateCreatureRenderList();
		return;
	}

	Module::Get()->QueueEvent(Event("TILE_CLICKED", T), true);

	Event ev("CREATURE_CLICKED", std::shared_ptr<Creature>());

	// minimum distance to recognise a click
	float curdist = .2;
	std::shared_ptr<Creature> tmp;
	for ( const std::shared_ptr<Creature>& C : T->getCreatures())
	{
		float dist = glm::distance(pos, C->getPosition());
		if ( dist < curdist )
		{
			tmp = C;
			curdist = dist;
		}
	}

	if ( tmp ) ev.SetData( tmp );

	Module::Get()->QueueEvent(ev, true);

	// required to hightlight the creatures species in the renderer, as this can only be done when a new CreatureRenderlist is received
	boost::this_thread::sleep(boost::posix_time::milliseconds(50));
	UpdateCreatureRenderList();
}

void Simulator::UpdateCreatureRenderList()
{
	Event Ev("UpdateCreatureRenderList", std::vector<CreatureRenderInfo>() );
	 std::vector<CreatureRenderInfo>& RI = boost::any_cast< std::vector<CreatureRenderInfo>& >(Ev.Data());

	RI.resize(state->creatures.size());
	int i = 0;
	for ( auto& C : state->creatures )
	{
		RI[i++] = std::make_tuple( C->getPosition(), (int)C->getSpecies()->getType(), C->getSpecies().get() );
	}

	Module::Get()->QueueEvent(Ev, true);
}


std::shared_ptr<Species>& Simulator::getSpecies( const std::string& name )
{
	for ( auto& S : *state->species )
	{
		if ( S->getName() == name) return S;
	}

	return state->species->back(); // INVALID_SPECIES
}

std::shared_ptr<Species> Simulator::GetSpecies( const std::string& name, const std::shared_ptr<SimState> s )
{
	for ( auto& S : *s->species )
	{
		if ( S->getName() == name) return S;
	}

	return s->species->back(); // INVALID_SPECIES
}


void Simulator::SendScaleUpdate()
{
	int maxTick = std::max( state->currentTick, _pod->peekTop()->currentTick );
		sf::Vector2i data = sf::Vector2i( state->currentTick, maxTick );
		Module::Get()->QueueEvent(Event("SIM_UPDATE_TICK_SCALE", data ), true);
}

void Simulator::registerIOPlugins()
{
	Engine::out(Engine::INFO) << "[Simulator] Loading IO plugins:" << std::endl;

	std::shared_ptr<sbe::IOPlugin> CreatureIOP ( new CreatureIOPlugin() );
	Engine::GetResMgr()->registerResource<Creature>( sbe::iResource::createResInfo("Creature", true, false), CreatureIOP);

	std::shared_ptr<sbe::IOPlugin> SpeciesIOP ( new SpeciesIOPlugin() );
	Engine::GetResMgr()->registerResource<Species>( sbe::iResource::createResInfo("Species", true, false), SpeciesIOP);

	std::shared_ptr<sbe::IOPlugin> TerrainIOP ( new TerrainIOPlugin() );
	Engine::GetResMgr()->registerResource<Terrain>( sbe::iResource::createResInfo("Terrain", true, false), TerrainIOP);

	std::shared_ptr<sbe::IOPlugin> SimStateIOP ( new SimStateIOPlugin() );
	Engine::GetResMgr()->registerResource<SimState>( sbe::iResource::createResInfo("SimState", true, false), SimStateIOP);

	std::shared_ptr<sbe::IOPlugin> StasisPodIOP ( new StasisPodIOPlugin() );
	Engine::GetResMgr()->registerResource<StasisPod>( sbe::iResource::createResInfo("StasisPod", true, false), StasisPodIOP);

	Engine::out(Engine::INFO) << "[Simulator] IO Plugins loaded." << std::endl;
}

void Simulator::saveWhole(const std::string &savePath){
	if(!isInitialized)
	{
		Event e("EVT_SAVE_BAD", std::string("Simulation invalid!"));
		Module::Get()->QueueEvent(e, true);
		return;
	}

	bool wasPaused = isPaused;

	isPaused = true; // pause sim while saving
	Engine::getCfg()->set("sim.paused", isPaused);

	// add save path to IO stack
	if(savePath.empty() || !Engine::GetIO()->addPath(savePath))
	{
		Event e("EVT_SAVE_BAD", std::string("Save path invalid ( sim.debugsavepath in config )!"));
		Module::Get()->QueueEvent(e, true);
		return;
	}

	Engine::out(Engine::SPAM) << "[Simulator]Save to path: " << Engine::GetIO()->topPath() << std::endl;

	_pod->freeze(state); // freeze latest state

	// do some saving...
	//~ if(!Engine::GetIO()->saveObject( "tick"+boost::lexical_cast<std::string>(state->currentTick), *state, true)){
	if(!Engine::GetIO()->saveObject( "StasisPod", *_pod, true)){

		Event e("EVT_SAVE_BAD", std::string("Error saving StasisPod!"));
		Module::Get()->QueueEvent(e, true);

	} else {
		Module::Get()->QueueEvent("EVT_SAVE_GOOD", true);

	}


	if(!savePath.empty())
		Engine::GetIO()->popPath(); // pop save path from IO stack

	isPaused = wasPaused; // continue, if not wasPaused
	Engine::getCfg()->set("sim.paused", isPaused);
}

void Simulator::loadWhole(const std::string &loadPath)
{

	bool wasPaused = isPaused;
	isPaused = true; // pause sim while saving
	Engine::getCfg()->set("sim.paused", isPaused);

	if(loadPath.empty() || !Engine::GetIO()->addPath(loadPath))
	{
		Event e("EVT_LOAD_BAD", std::string("Load path \""+ loadPath +"\" invalid!"));
		Module::Get()->QueueEvent(e, true);
		return;
	}

	// do some loading...
	auto tmp  = Engine::GetIO()->loadObjects<StasisPod>();

	if(tmp.empty())
	{
		Event e("EVT_LOAD_BAD", std::string("Error loading from \"" + loadPath + "\"!"));
		Module::Get()->QueueEvent(e, true);

		Engine::out(Engine::ERROR) << "[Simulator]Error loading from '" << loadPath << "'!" << std::endl;
		return;
	}

	if(!loadPath.empty())
		Engine::GetIO()->popPath(); // pop load path from IO stack

	auto latestState = tmp[0]->tawTop(); // taw latest state

	setState(latestState);

	_pod = tmp[0];
	NewSimulation( latestState );

	// loading done...
	Module::Get()->QueueEvent("EVT_LOAD_GOOD", true);

	isPaused = wasPaused;
	Engine::getCfg()->set("sim.paused", isPaused);
}

void Simulator::forwardTo(const int i){
	bool wasPaused = isPaused;

	Engine::out(Engine::SPAM) << "[Simulator][forwardTo] forwarding to " << i << std::endl;
	setState(_pod->peekTick(i));
	Engine::out(Engine::SPAM) << "[Simulator][forwardTo] got state for tick " << state->currentTick << std::endl;
	TicksToSim = i - state->currentTick;

	isPaused = false;
	Engine::getCfg()->set("sim.paused", isPaused);

	TickTimer.restart();
	simulateTicks = TicksToSim;
	Engine::out(Engine::SPAM) << "[Simulator][forwardTo] yet to simulate " << TicksToSim << std::endl;

	std::string msg = "Please wait while we simulate ";
	msg += boost::lexical_cast<std::string>( TicksToSim );
	msg += " ticks.";
	std::shared_ptr<sbe::Message> M( new sbe::Message( sbe::Message::Type::MODAL , "Forwarding.", msg) );
	Module::Get()->QueueEvent( Event("NEW_MESSAGE", M), true );

	//~ isPaused = wasPaused;
	//~ Engine::getCfg()->set("sim.paused", isPaused);
}
