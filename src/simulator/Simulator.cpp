#include "Simulator.hpp"

#include <boost/thread.hpp>

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


Simulator* Simulator::Instance = nullptr;

Simulator::Simulator() : isPaused(true), isInitialized(false) {

	Instance = this;

	CreatureCounts[0] = 0;
	CreatureCounts[1] = 0;
	CreatureCounts[2] = 0;

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
}

std::shared_ptr<Species>& Simulator::getSpecies( const std::string& name )
{
	for ( auto& S : *_state->_species )
	{
		if ( S->getName() == name) return S;
	}

	return _state->_species->back(); // INVALID_SPECIES
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
	else if(e.Is("TERRAIN_CLICKED", typeid( Geom::Pointf )) && isInitialized )
	{
		// cast into desired type
		HandleClick( boost::any_cast<Geom::Pointf>( e.Data() ) );
	}
	else if (e.Is("SET_SIM_TPS", typeid(unsigned int)))
	{
		Module::Get()->SetTPS(boost::any_cast<unsigned int>(e.Data()));
	}
	else if ( e.Is( "UPDATE_OVERLAYS" ) && isInitialized  )
	{
		_state->_terrain->CreateMapPlotters();
	}
	else if (e.Is("RESET_SIMULATION"))
	{
		bool wasPaused = isPaused;
		isPaused = true;
		Engine::getCfg()->set("sim.paused", isPaused);

		NewSimulation( m_specs, m_counts, Engine::getCfg()->get<int>("sim.defaultSeed"));

		isPaused = wasPaused;
		Engine::getCfg()->set("sim.paused", isPaused);
	}
	else if (e.Is("EVT_SAVE_TERRAIN") && isInitialized )
	{
		Engine::GetResMgr()->saveObject( "DebugTerrain", _state->_terrain, true);
	}
	else if (e.Is("EVT_SAVE_WHOLE", typeid(std::string)) && isInitialized ) {
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
	// we have to make sure the renderer is setup before we can send the updateXXrenderlist events
	boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

	numThreads = Engine::getCfg()->get<int>("sim.numThreads");
	multiThreaded = numThreads > 1;

	_pod.reset(new StasisPod());
	_freezeRate = Engine::getCfg()->get<int>("sim.freezeRate");

	_tid.reset(new int(-1));

	//NewSimulation(Engine::getCfg()->get<int>("sim.defaultSeed"));

}


void Simulator::NewSimulation( std::shared_ptr<std::vector<std::shared_ptr<Species>>> _spec, std::shared_ptr<std::vector<int>> _specnum, int seed )
{
	std::shared_ptr<SimState> state(new SimState());

	std::mt19937* rng = new std::mt19937(seed);
	Engine::out(Engine::INFO) << "[Simulator] Seed is >> " << boost::lexical_cast<std::string>(seed) << " <<" << std::endl;
	state->_currentSeed = seed;
	// set a valid thread num in state, weirdly Simulator::numThreads seems to contain a invalid number of threads
	state->_numThreads = numThreads;

	Engine::out(Engine::INFO) << "[Simulator] Creating Terrain" << std::endl;
	state->_terrain.reset( new Terrain() );
	state->_species = _spec;
	state->_creatures.clear();
	// we have to set it here, so Simulator::GetTerrain() will work in Tile
	//this->setState(state, true);
	_state = state;

	state->_terrain->CreateDebugTerrain( _state->_currentSeed );

    Engine::out(Engine::INFO) << "[Simulator] Creatures and species" << std::endl;
	Generator G (state, *rng);
	int countMult =  Engine::getCfg()->get<int>("sim.terragen.count");
	G.CreateCreatures(_spec, _specnum, countMult);

	NewSimulation( state, rng);
}

void Simulator::NewSimulation(
	std::shared_ptr<SimState> state,
	std::mt19937* rng)
{

	state->_seeder.reset( rng ); // must do it this early for initThreads to work

	if ( multiThreaded)
	{
		stopThreads();
		initThreads();
	}

	state->_currentTick = 0;
	simulateTicks = 0;
	TicksToSim = 0;
	state->_numGenerated = 0;

	Creature::loadConfigValues();

    resetStats();
	// count Creatures once
	for( auto C : state->_creatures )
	{
		CreatureCounts[ (int)(C->getSpecies()->getType()) ]++;
		C->updateTileFromPos();
	}

	Engine::out(Engine::INFO) << "[Simulator] Simulation is set up" << std::endl;

	isPaused = Engine::getCfg()->get<bool>("sim.pauseOnStart");
	Engine::getCfg()->set("sim.paused", isPaused);


	// categorize tiles for parallel computation of the simulation
	state->_terrain->CreateParallelisationGraph();

	// send terrain to renderer
	state->_terrain->UpdateTileMap();
	state->_terrain->UpdateTerrain();
	// send creatures to renderer
	UpdateCreatureRenderList();

	isInitialized = true;

	state->_terrain->CreateMapPlotters();

	RendererUpdate.restart();

	Engine::out() << "[Simulator] Init Graphs for GraphBook" << std::endl;
	CreatePlotters();
	auto data = std::make_pair( std::string( "Population" ), CountGraph );
	Module::Get()->QueueEvent(Event("ADD_GRAPH_TO_BOOK", data), true);

	auto data2 = std::make_pair( std::string( "Means of Death" ), DeathGraph );
	Module::Get()->QueueEvent(Event("ADD_GRAPH_TO_BOOK", data2), true);


	_pod->clear();
	Engine::out() << "[Sim] Freeze" << std::endl;
	_pod->freeze(_state);
	Engine::out() << "[Sim] Freezed - " << _pod->peekTop()->_currentTick << "/" << _pod->peekTop()->_numThreads << std::endl;
}

void Simulator::initThreads()
{
	startBarrier.reset( new boost::barrier( numThreads+1) );
	endBarrier.reset( new boost::barrier( numThreads+1 ) );
	_state->_gens.clear();

	std::uniform_int_distribution<int> seeder;

	for ( int thread = 0; thread < numThreads; ++thread)
	{
		std::shared_ptr<std::list<std::shared_ptr<Tile>>> cur;
		cur.reset ( new std::list<std::shared_ptr<Tile>> );
		CurrentLists.push_back(cur);
		cur.reset ( new std::list<std::shared_ptr<Tile>> );
		NextLists.push_back(cur);

		_state->_gens.push_back(std::shared_ptr<std::mt19937>( new std::mt19937(seeder(*(_state->_seeder))) ));
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

void Simulator::thread(std::shared_ptr<std::list<std::shared_ptr<Tile>>> list, const int tid)
{
	_tid.reset(new int(tid));
	while ( !boost::this_thread::interruption_requested() )
	{
		startBarrier->wait();

		tick( list );

		endBarrier->wait();
	}
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
				std::string text = boost::lexical_cast<std::string>( TicksToSim ) + " Ticks Simulated in " + boost::lexical_cast<std::string>(TickTimer.restart().asSeconds()) + " s.";
				std::shared_ptr<sbe::Message> m( new sbe::Message(sbe::Message::OK, "Simulation Stopped", text, "PAUSELOCK_DOWN", true ));

				Module::Get()->QueueEvent( Event("NEW_MESSAGE", m), true );
			}

			UpdateCreatureRenderList();
			SendScaleUpdate();
		}
		if ( simulateTicks > 0 ) simulateTicks--;

		CreatureCounts[0] = 0;
		CreatureCounts[1] = 0;
		CreatureCounts[2] = 0;

		if ( multiThreaded )
		{
			parallelTick();
		}
		else
		{
			for( auto& batch : _state->_terrain->getColors())
				for ( auto& T : batch)
					for ( auto Cit = T->getCreatures().begin(); Cit != T->getCreatures().end(); )
					{
						if ( (*Cit)->getCurrentHealth() <= 0 )
						{
							auto ptr = *(Cit++);
							T->removeCreature(ptr);
							_state->_creatures.remove(ptr);
						}
						else
						{
							CreatureCounts[ (int)((*Cit)->getSpecies()->getType()) ]++;
							(*Cit)->done = false;
							(*(Cit++))->live();
						}
					}
		}

		_state->_currentTick++;

		logTickStats();

		if(! (_state->_currentTick % _freezeRate))
		{
			Engine::out() << "[Sim] Freeze" << std::endl;
			_pod->freeze(_state);
			Engine::out() << "[Sim] Freezed" << std::endl;
		}


	}

	// update the renderer at up to 30 fps
	if (RendererUpdate.getElapsedTime() > sf::milliseconds(33))
	{
		SendScaleUpdate();

		Module::Get()->DebugString("#Species", boost::lexical_cast<std::string>(_state->_species->size()));
		Module::Get()->DebugString("#Plants", boost::lexical_cast<std::string>( CreatureCounts[0] ));
		Module::Get()->DebugString("#Herbivores", boost::lexical_cast<std::string>( CreatureCounts[1] ));
		Module::Get()->DebugString("#Carnivores", boost::lexical_cast<std::string>( CreatureCounts[2] ));
		Module::Get()->DebugString("#Tick", boost::lexical_cast<std::string>( _state->_currentTick ));
		Module::Get()->DebugString("#rnds", boost::lexical_cast<std::string>( _state->_numGenerated ));
		Module::Get()->DebugString("Dead - Eaten", boost::lexical_cast<std::string>( EatenCounts.back() ));
		Module::Get()->DebugString("Dead - Starved", boost::lexical_cast<std::string>( StarvedCounts.back() ));
		Module::Get()->DebugString("Dead - Thirst", boost::lexical_cast<std::string>( ThirstCounts.back() ));
		Module::Get()->DebugString("Dead - Frozen", boost::lexical_cast<std::string>( FrozenCounts.back() ));
		Module::Get()->DebugString("Dead - Old", boost::lexical_cast<std::string>( OldCounts.back() ));

		if ( !isPaused )
		{
			UpdateCreatureRenderList();
			if (Engine::getCfg()->get<bool>("system.ui.Overlays.live")) _state->_terrain->CreateMapPlotters();
		}

		RendererUpdate.restart();
	}
}

void Simulator::SendScaleUpdate()
{
	int maxTick = std::max( _state->_currentTick, _pod->peekTop()->_currentTick );
		sf::Vector2i data = sf::Vector2i( _state->_currentTick, maxTick );
		Module::Get()->QueueEvent(Event("SIM_UPDATE_TICK_SCALE", data ), true);
}

void Simulator::tick(std::shared_ptr<std::list<std::shared_ptr<Tile>>> list)
{
	for(auto it = list->cbegin(); it != list->cend(); it++)
	{
		auto Cs = (*it)->getCreatures();
		for ( auto it2 = Cs.cbegin(); it2 != Cs.cend(); it2++)
			if (*it2 && (*it2)->getCurrentHealth() > 0)
				(*it2)->live();
	}
}


void Simulator::parallelTick()
{
	int eaten = 0;
	int frozen = 0;
	int starved = 0;
	int thirst = 0;
	int old = 0;

	//  makes sure our threads are waiting at the "endBarrier" needed for the loop
	// do this by starting an empty batch
	for ( int thread = 0; thread < numThreads; ++thread)
		CurrentLists[thread]->clear();
	startBarrier->wait();

	for( auto& batch : _state->_terrain->getColors())
	{

		for ( int thread = 0; thread < numThreads; ++thread)
			NextLists[thread]->clear();

		// prepare the next batch of work
		int curthread = 0;
		for ( auto& T : batch )
		{
			if ( !T ) curthread = (curthread+1)%numThreads;
			else 	  NextLists[curthread]->push_back(T);
		}

//		for ( int i = 0; i < numThreads; ++i)
//		{
//			Engine::out() << "Thread: " << i << " has " << NextLists[i]->size() << std::endl;
//		}

		// wait till the threads are finished
		endBarrier->wait();

		// cleanup dead creatures
		for(auto it = _state->_creatures.begin(); it != _state->_creatures.end(); )
		{

			if ((*it)->getCurrentHealth() <= 0)
			{
				switch((*it)->causeOfDeath)
				{
					case Creature::CauseOfDeath::EATEN:
						eaten++;
						break;
					case Creature::CauseOfDeath::FROZEN:
						frozen++;
						break;
					case Creature::CauseOfDeath::THIRST:
						thirst++;
						break;
					case Creature::CauseOfDeath::STARVED:
						starved++;
						break;
					case Creature::CauseOfDeath::OLD:
						old++;
						break;
				}
				(*it)->getTile()->removeCreature(*it);
				auto it2 = it++;
				_state->_creatures.erase( it2 );
			}
			else {
				(*(++it))->done = false;
			}
		}

		// swap the lists
		for ( int thread = 0; thread < numThreads; ++thread )
			CurrentLists[thread]->swap( *(NextLists[thread]) );

		startBarrier->wait();
	}

	// make sure we correctly stop the last batch
	endBarrier->wait();

	// cleanup dead creatures
	for(auto it = _state->_creatures.begin(); it != _state->_creatures.end(); )
	{

		if ((*it)->getCurrentHealth() <= 0)
		{
			switch((*it)->causeOfDeath)
			{
				case Creature::CauseOfDeath::EATEN:
					eaten++;
					break;
				case Creature::CauseOfDeath::FROZEN:
					frozen++;
					break;
				case Creature::CauseOfDeath::THIRST:
					thirst++;
					break;
				case Creature::CauseOfDeath::STARVED:
					starved++;
					break;
				case Creature::CauseOfDeath::OLD:
					old++;
					break;
			}
			(*it)->getTile()->removeCreature(*it);
			auto it2 = it++;
			_state->_creatures.erase( it2 );
		}
		else {
			CreatureCounts[ (int)((*it)->getSpecies()->getType()) ]++;
			(*(++it))->done = false;
		}
	}
	EatenCounts.push_back((int)eaten);
	StarvedCounts.push_back((int)starved);
	FrozenCounts.push_back((int)frozen);
	ThirstCounts.push_back((int)thirst);
	OldCounts.push_back((int)old);
}

void Simulator::logTickStats()
{
	if(!isInitialized) return;

	HerbaeCounts.push_back(CreatureCounts[(int)Species::HERBA]);
	HerbivoreCounts.push_back(CreatureCounts[(int)Species::HERBIVORE]);
	CarnivoreCounts.push_back(CreatureCounts[(int)Species::CARNIVORE]);

	CountGraph->updateCurveData( "Herbs", HerbaeCounts );
	CountGraph->updateCurveData( "Herbivore", HerbivoreCounts );
	CountGraph->updateCurveData( "Carnivore", CarnivoreCounts );

	DeathGraph->updateCurveData("Dead - Eaten", EatenCounts   );
	DeathGraph->updateCurveData("Dead - Frozen", FrozenCounts);
	DeathGraph->updateCurveData("Dead - Starved", StarvedCounts);
	DeathGraph->updateCurveData("Dead - Thirst", ThirstCounts);
	DeathGraph->updateCurveData("Dead - Old", OldCounts);

	//ProcessingTimes.push_back(  )
}

void Simulator::resetStats()
{
    CreatureCounts[0] = 0;
	CreatureCounts[1] = 0;
	CreatureCounts[2] = 0;

    EatenCounts.clear();
	StarvedCounts.clear();
	FrozenCounts.clear();
	ThirstCounts.clear();
	OldCounts.clear();

	EatenCounts.push_back(0);
	StarvedCounts.push_back(0);
	FrozenCounts.push_back(0);
	ThirstCounts.push_back(0);
	OldCounts.push_back(0);

	CarnivoreCounts.clear();
	HerbaeCounts.clear();
	HerbivoreCounts.clear();
}

void Simulator::CreatePlotters()
{
	if(!isInitialized) return;

	CountGraph.reset(new sbe::GraphPlotter );
	DeathGraph.reset(new sbe::GraphPlotter );
	{
	sbe::Graph g;
	g.Size = Geom::Point( Engine::getCfg()->get<int>("sim.countplot.size.x"),Engine::getCfg()->get<int>("sim.countplot.size.y"));
	g.AxisSize = Geom::Point(Engine::getCfg()->get<int>("sim.countplot.axissize.x"), Engine::getCfg()->get<int>("sim.countplot.axissize.y"));
	g.addCurve( sbe::Curve("Herbs", HerbaeCounts, sf::Color::Green) );
	g.addCurve( sbe::Curve("Herbivore", HerbivoreCounts, sf::Color::Blue) );
	g.addCurve( sbe::Curve("Carnivore", CarnivoreCounts, sf::Color::Red) );
	CountGraph->setGraph( g );
	}

	sbe::Graph g;
	g.Size = Geom::Point( Engine::getCfg()->get<int>("sim.countplot.size.x"),Engine::getCfg()->get<int>("sim.countplot.size.y"));
	g.AxisSize = Geom::Point(Engine::getCfg()->get<int>("sim.countplot.axissize.x"), Engine::getCfg()->get<int>("sim.countplot.axissize.y"));
	g.addCurve( sbe::Curve("Dead - Eaten", EatenCounts, sf::Color::Black) );
	g.addCurve( sbe::Curve("Dead - Frozen", FrozenCounts, sf::Color::Cyan) );
	g.addCurve( sbe::Curve("Dead - Starved", StarvedCounts, sf::Color::Yellow) );
	g.addCurve( sbe::Curve("Dead - Thirst", ThirstCounts, sf::Color::Magenta) );
	g.addCurve( sbe::Curve("Dead - Old", OldCounts, sf::Color(0, 80, 0)) );
	DeathGraph->setGraph( g );

}

void Simulator::HandleClick( const Geom::Pointf& pos)
{
	if( !isInitialized ) return;

	const std::shared_ptr<Tile>& T = _state->_terrain->getTile( pos );

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
		float dist = Geom::distance(pos, C->getPosition());
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

	RI.resize(_state->_creatures.size());
	int i = 0;
	for ( auto& C : _state->_creatures )
	{
		RI[i++] = std::make_tuple( C->getPosition(), (int)C->getSpecies()->getType(), C->getSpecies().get() );
	}

	Module::Get()->QueueEvent(Ev, true);
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
	if(!isInitialized) return;
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

	_pod->freeze(_state); // freeze latest state

	// do some saving...
	//~ if(!Engine::GetIO()->saveObject( "tick"+boost::lexical_cast<std::string>(_state->_currentTick), *_state, true)){
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

	std::mt19937* newGen = new std::mt19937();

	if(!setState(latestState) ){
		Event e("EVT_LOAD_BAD", std::string("Error loading"));
		Module::Get()->QueueEvent(e, true);

		Engine::out(Engine::ERROR) << "[Simulator]Error loading from '" << loadPath << "'!" << std::endl;
		Engine::out(Engine::ERROR) << "[Simulator]Won't continue with SimState saved with " << latestState->_numThreads
		                           << " threads using " << numThreads << "threads!" << std::endl;

		delete newGen;
		return;
	}

	_pod = tmp[0];
	NewSimulation( latestState, newGen );

	// loading done...
	Module::Get()->QueueEvent("EVT_LOAD_GOOD", true);

	isPaused = wasPaused;
	Engine::getCfg()->set("sim.paused", isPaused);
}

void Simulator::forwardTo(const int i){
	bool wasPaused = isPaused;

	Engine::out(Engine::SPAM) << "[Simulator][forwardTo] forwarding to " << i << std::endl;
	setState(_pod->peekTick(i));
	Engine::out(Engine::SPAM) << "[Simulator][forwardTo] got state for tick " << _state->_currentTick << std::endl;
	TicksToSim = i - _state->_currentTick;

	isPaused = false;
	Engine::getCfg()->set("sim.paused", isPaused);

	TickTimer.restart();
	simulateTicks = TicksToSim;
	Engine::out(Engine::SPAM) << "[Simulator][forwardTo] yet to simulate " << TicksToSim << std::endl;

	//~ isPaused = wasPaused;
	//~ Engine::getCfg()->set("sim.paused", isPaused);
}
