#include "Simulator.hpp"

#include <boost/thread.hpp>

#include "sbe/ResourceManager.hpp"
#include "sbe/gfx/GraphPlotter.hpp"
#include "sbe/geom/Helpers.hpp"
#include "sbe/sfg/Message.hpp"

#include "sbe/Config.hpp"

#include "world/Terrain.hpp"
#include "world/Creature.hpp"
#include "world/Tile.hpp"

#include "Generator.hpp"

#include "resources/CreatureIOPlugin.hpp"
#include "resources/SpeciesIOPlugin.hpp"
#include "resources/TerrainIOPlugin.hpp"

#include <SFML/System/Clock.hpp>

#include <iterator>
#include <string>
#include <sstream>
#include <random>

Simulator* Simulator::Instance = nullptr;

Simulator::Simulator():
currentTick(0), numGenerated(0), isPaused(false) {

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

	RegisterForEvent("SET_SIM_TPS");

	RegisterForEvent("PLOT_GRAPH");
	RegisterForEvent("UPDATE_OVERLAYS");

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
		advance();
	}
	else if(e.Is("SIM_PAUSE"))
	{
		isPaused = true;
		Engine::getCfg()->set("sim.paused", isPaused);
		Module::Get()->QueueEvent(Event("UpdateCreatureRenderList", Creatures), true);
	}
	else if(e.Is("SIM_UNPAUSE"))
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
	else if(e.Is("TOGGLE_SIM_PAUSE"))
	{
		isPaused = !isPaused;
		Engine::getCfg()->set("sim.paused", isPaused);
		if (isPaused) Module::Get()->QueueEvent(Event("UpdateCreatureRenderList", Creatures), true);
	}
	else if(e.Is("TERRAIN_CLICKED", typeid( Geom::Pointf )))
	{
		// cast into desired type
		HandleClick( boost::any_cast<Geom::Pointf>( e.Data() ) );
	}
	else if (e.Is("SET_SIM_TPS", typeid(unsigned int)))
	{
		Module::Get()->SetTPS(boost::any_cast<unsigned int>(e.Data()));
	}
	else if ( e.Is( "PLOT_GRAPH", typeid( std::string ) ) )
	{
		std::string graphName = boost::any_cast<std::string>( e.Data() );
		///TODO: Choose the right Graph to plot
		if ( graphName == "Population" )
		{
			std::shared_ptr<sbe::GraphPlotter> p = CreateCountPlotter();
			if ( p->isValid() )
			Module::Get()->QueueEvent(Event("DISPLAY_GRAPH", p), true);
		}
	}
	else if ( e.Is( "UPDATE_OVERLAYS" ) )
	{
		Terra->CreateMapPlotters();
	}
	else if (e.Is("RESET_SIMULATION"))
	{
		NewSimulation();
	}
	else if (e.Is("EVT_SAVE_TERRAIN"))
	{
		Engine::GetResMgr()->saveObject( "DebugTerrain", Terra, true);
	}
	else if (e.Is("EVT_SAVE_WHOLE_TEST")) {
		saveWhole( Engine::getCfg()->get<std::string>("sim.debugsavepath"));
	}
	else if (e.Is("EVT_LOAD_WHOLE_TEST")) {
		loadWhole( Engine::getCfg()->get<std::string>("sim.debugsavepath"));
	}
	else if (e.Is("EVT_SAVE_WHOLE", typeid(std::string))) {
		saveWhole(boost::any_cast<std::string>(e.Data()));
	}
	else if (e.Is("EVT_LOAD_WHOLE", typeid(std::string))) {
		loadWhole(boost::any_cast<std::string>(e.Data()));
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

	numThreads = Engine::getCfg()->get<int>("sim.numThreads");
	minimizeParallelRuns = Engine::getCfg()->get<bool>("sim.minimizeParallelRuns");
	multiThreaded = numThreads > 1;



	NewSimulation(Engine::getCfg()->get<int>("sim.defaultSeed"));
}

void Simulator::NewSimulation( int seed )
{
	std::mt19937* rng = new std::mt19937(seed);
	Engine::out(Engine::INFO) << "[Simulator] Seed is >> " << boost::lexical_cast<std::string>(seed) << " <<" << std::endl;
	currentSeed = seed;

	std::vector<std::shared_ptr<Species>> Spec;
	std::list<std::shared_ptr<Creature>> Creatures;

	Engine::out(Engine::INFO) << "[Simulator] Creating Terrain" << std::endl;
	// we have to set it here, so Simulator::GetTerrain() will work in Tile
	std::shared_ptr<Terrain> T( new Terrain() );
	Terra = T;
	T->CreateDebugTerrain();
	Generator G (Spec, Creatures, *T, *rng);

	G.CreateSpeciesWithCreatures( Species::HERBA, 		Engine::getCfg()->get<int>("sim.terragen.plantSpecies"), Engine::getCfg()->get<int>("sim.terragen.plantCount") );
	G.CreateSpeciesWithCreatures( Species::HERBIVORE, 	Engine::getCfg()->get<int>("sim.terragen.herbivoreSpecies"), Engine::getCfg()->get<int>("sim.terragen.herbivoreCount") );
	G.CreateSpeciesWithCreatures( Species::CARNIVORE, 	Engine::getCfg()->get<int>("sim.terragen.carnivoreSpecies"), Engine::getCfg()->get<int>("sim.terragen.carnivoreCount") );

	NewSimulation( rng, T, Spec, Creatures );
}

void Simulator::NewSimulation(
	std::mt19937* rng,
	std::shared_ptr<Terrain> newTerrain,
	std::vector<std::shared_ptr<Species>>& newSpecies,
	std::list<std::shared_ptr<Creature>>& newCreatures )
{
	if ( multiThreaded)
	{
		stopThreads();
		initThreads();
	}


	currentTick = 0;
	simulateTicks = 0;
	TicksToSim = 0;

	Engine::out(Engine::INFO) << "[Simulator] Random engine" << std::endl;
	gen.reset( rng );
	numGenerated = 0;

	Creature::loadConfigValues();

	Engine::out(Engine::INFO) << "[Simulator] Terrain" << std::endl;
	Terra = newTerrain;
	Terra->UpdateTileMap();

	Engine::out(Engine::INFO) << "[Simulator] Creatures and species" << std::endl;
	CreatureCounts[0] = 0;
	CreatureCounts[1] = 0;
	CreatureCounts[2] = 0;

	Creatures.clear();
	Creatures = newCreatures;

	// count Creatures once
	for( auto C : Creatures )
	{
		CreatureCounts[ (int)(C->getSpecies()->getType()) ]++;
		C->updateTileFromPos();
	}

	SpeciesList.clear();
	SpeciesList = newSpecies;
	// add default species
	std::shared_ptr<Species> S ( new Species( "UNDEFINED_SPECIES", Species::SPECIES_TYPE::HERBA));
	SpeciesList.push_back( S );

	Engine::out(Engine::INFO) << "[Simulator] Simulation is set up" << std::endl;

	isPaused = Engine::getCfg()->get<bool>("sim.pauseOnStart");
	Engine::getCfg()->set("sim.paused", isPaused);


	// categorize tiles for parallel computation of the simulation
	Terra->CreateParallelisationGraph();

	// send terrain to renderer
	Terra->UpdateTerrain();
	// send creatures to renderer
	Module::Get()->QueueEvent(Event("UpdateCreatureRenderList", Creatures), true);

	Terra->CreateMapPlotters();

	RendererUpdate.restart();

	Engine::out() << "[Simulator] Init Graphs for GraphBook" << std::endl;
	Module::Get()->QueueEvent( Event( "ADD_GRAPH_TO_BOOK", std::string("Any other graph") ), true );//@TODO
	Module::Get()->QueueEvent( Event( "ADD_GRAPH_TO_BOOK", std::string("Population") ), true );
}

void Simulator::initThreads()
{

	startBarrier.reset( new boost::barrier( numThreads+1) );
	endBarrier.reset( new boost::barrier( numThreads+1 ) );

	std::uniform_int_distribution<int> seeder;

	for ( int thread = 0; thread < numThreads; ++thread)
	{


		//Engine::out() << "Thread " << thread << ": workload " << from << "-" << to << std::endl;

		std::shared_ptr<std::list<std::shared_ptr<Tile>>> cur;
		cur.reset ( new std::list<std::shared_ptr<Tile>> );
		CurrentLists.push_back(cur);
		cur.reset ( new std::list<std::shared_ptr<Tile>> );
		NextLists.push_back(cur);

		//Engine::out() << "from " << (*it1)->getPosition() << std::endl;

		CreatureCounters.push_back( std::shared_ptr<int> (new int[3], ArrDeleter() ) );

		threads.push_back( std::shared_ptr<boost::thread>( new boost::thread( boost::bind( &Simulator::thread, this, CurrentLists[thread], CreatureCounters[thread], thread ))));
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

	CreatureCounters.clear();
	threads.clear();
	CurrentLists.clear();
	NextLists.clear();
}

void Simulator::thread(std::shared_ptr<std::list<std::shared_ptr<Tile>>> list, std::shared_ptr<int> _CreatureCounts, int seed)
{
	gen.reset( new std::mt19937(seed));
	while ( !boost::this_thread::interruption_requested() )
	{
		startBarrier->wait();

		tick( list, _CreatureCounts );

		endBarrier->wait();
	}
}
void Simulator::advance()
{
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

			Module::Get()->QueueEvent(Event("UpdateCreatureRenderList", Creatures), true);
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
			for( auto& batch : Terra->getColors())
				for ( auto& T : batch)
					for ( auto Cit = T->getCreatures().begin(); Cit != T->getCreatures().end(); )
					{
						if ( (*Cit)->getCurrentHealth() <= 0 )
						{
							Creatures.remove(*Cit);
							auto it2 = Cit++;
							T->removeCreature(*it2);
						}
						else
						{
							CreatureCounts[ (int)((*Cit)->getSpecies()->getType()) ]++;
							(*Cit)->done = false;
							(*(Cit++))->live();
						}
					}

			// old non-MT compatible version
//			for(auto it = Creatures.begin(); it != Creatures.end(); )
//			{
//				if((*it)->getCurrentHealth() <= 0)
//				{
//					(*it)->getTile()->removeCreature(*it);
//					auto it2 = it++;
//					Creatures.erase( it2 );
//				}
//				else
//				{
//					CreatureCounts[ (int)((*it)->getSpecies()->getType()) ]++;
//					(*it)->done = false;
//					(*(it++))->live();
//				}
//			}
		}

		currentTick++;
		logTickStats();
	}

	// update the renderer at up to 30 fps
	if (RendererUpdate.getElapsedTime() > sf::milliseconds(33))
	{
		Module::Get()->DebugString("#Species", boost::lexical_cast<std::string>(SpeciesList.size()));
		Module::Get()->DebugString("#Plants", boost::lexical_cast<std::string>( CreatureCounts[0] ));
		Module::Get()->DebugString("#Herbivores", boost::lexical_cast<std::string>( CreatureCounts[1] ));
		Module::Get()->DebugString("#Carnivores", boost::lexical_cast<std::string>( CreatureCounts[2] ));
		Module::Get()->DebugString("#Tick", boost::lexical_cast<std::string>( currentTick ));
		Module::Get()->DebugString("#rnds", boost::lexical_cast<std::string>( numGenerated ));
		if ( !isPaused )
		{
			Module::Get()->QueueEvent(Event("UpdateCreatureRenderList", Creatures), true);

			if (Engine::getCfg()->get<bool>("system.ui.Overlays.live")) Terra->CreateMapPlotters();
		}

		RendererUpdate.restart();
	}
}

void Simulator::tick(std::shared_ptr<std::list<std::shared_ptr<Tile>>> list, std::shared_ptr<int> _CreatureCounts)
{
	int* CreatureCounts = _CreatureCounts.get();
	CreatureCounts[0] = 0;
	CreatureCounts[1] = 0;
	CreatureCounts[2] = 0;

//	for( std::shared_ptr<Tile>& T : (*list))
	//	Engine::out() << "tile: " << T->getPosition() << std::endl;

	int i = 0;
	//for( std::shared_ptr<Tile>& T : (*list))
	for(auto it = list->cbegin(); it != list->cend(); it++)
	{
		i++;
		//Engine::out() << "Tick: tile " << i << std::endl;
		//for ( auto C : T->getCreatures() )
		auto Cs = (*it)->getCreatures();
		for ( auto it2 = Cs.cbegin(); it2 != Cs.cend(); it2++)
		{
			//and ya god said live creature !... LIVE !!!
			if (*it2 && (*it2)->getCurrentHealth() > 0)
			{
				(*it2)->live();
				CreatureCounts[ (int)((*it2)->getSpecies()->getType()) ]++;
			}
		}
	}
}


void Simulator::parallelTick()
{

	//  makes sure our threads are waiting at the "endBarrier" needed for the loop
	// do this by starting an empty batch
	for ( int thread = 0; thread < numThreads; ++thread)
		CurrentLists[thread]->clear();
	startBarrier->wait();

	int b = 0;
	for( auto& batch : Terra->getColors())
	{
		b++;
		int worksize = std::ceil(batch.size() / numThreads);

		for ( int thread = 0; thread < numThreads; ++thread)
			NextLists[thread]->clear();


		// prepare the next batch of work
		if ( minimizeParallelRuns )
		{
			int curthread = 0;
			for ( auto& T : batch )
			{
				if ( !T ) curthread = (curthread+1)%numThreads;
				else
					NextLists[curthread]->push_back(T);
			}
		}
		else
		{
			// sort the tile into the threads
			int curthread = 0;
			for ( auto& T : batch)
			{
				NextLists[curthread]->push_back(T);
				curthread = (curthread+1)%numThreads;
			}
			// old method: evenly split the number of tiles
//			for ( int thread = 0; thread < numThreads; ++thread)
//			{
//				int from = thread*worksize;
//				int to   = thread*worksize > batch.size()  ?  batch.size()-1  :  (thread+1)*worksize;
//				TileIt it1 = batch.begin(); std::advance( it1, from);
//				TileIt it2 = batch.begin(); std::advance( it2, to);
//
//				NextLists[thread]->clear();
//				std::copy(it1, it2, std::inserter( *(NextLists[thread]), NextLists[thread]->end() ));
//			}
		}


		// wait till the threads are finished
		endBarrier->wait();

		// cleanup dead creatures
		for(auto it = Creatures.begin(); it != Creatures.end(); )
		{
			if((*it)->getCurrentHealth() <= 0)
			{
				(*it)->getTile()->removeCreature(*it);
				auto it2 = it++;
				Creatures.erase( it2 );
			}
			else { (*(++it))->done = false; }
		}

		// collect counts
		for ( int thread = 0; thread < numThreads; ++thread)
		{
			CreatureCounts[0] += (CreatureCounters[thread].get())[0];
			CreatureCounts[1] += (CreatureCounters[thread].get())[1];
			CreatureCounts[2] += (CreatureCounters[thread].get())[2];
		}

		// swap the lists
		for ( int thread = 0; thread < numThreads; ++thread )
			CurrentLists[thread]->swap( *(NextLists[thread]) );

		startBarrier->wait();

	}

	// make sure we correctly stop the last batch
	endBarrier->wait();

	// cleanup dead creatures
	for(auto it = Creatures.begin(); it != Creatures.end(); )
	{
		if((*it)->getCurrentHealth() <= 0)
		{
			(*it)->getTile()->removeCreature(*it);
			auto it2 = it++;
			Creatures.erase( it2 );
		}
		else { (*(++it))->done = false; }
	}

	// collect counts
	for ( int thread = 0; thread < numThreads; ++thread)
	{
		CreatureCounts[0] += (CreatureCounters[thread].get())[0];
		CreatureCounts[1] += (CreatureCounters[thread].get())[1];
		CreatureCounts[2] += (CreatureCounters[thread].get())[2];
	}

}

void Simulator::logTickStats()
{
	HerbaeCounts.push_back(CreatureCounts[(int)Species::HERBA]);
	HerbivoreCounts.push_back(CreatureCounts[(int)Species::HERBIVORE]);
	CarnivoreCounts.push_back(CreatureCounts[(int)Species::CARNIVORE]);

	//ProcessingTimes.push_back(  )
}

std::shared_ptr<sbe::GraphPlotter> Simulator::CreateCountPlotter()
{
	std::shared_ptr<sbe::GraphPlotter> re( new sbe::GraphPlotter );

	sbe::Graph g;
	g.Size = Geom::Point( Engine::getCfg()->get<int>("sim.countplot.size.x"),Engine::getCfg()->get<int>("sim.countplot.size.y"));
	g.AxisSize = Geom::Point(Engine::getCfg()->get<int>("sim.countplot.axissize.x"), Engine::getCfg()->get<int>("sim.countplot.axissize.y"));
	g.addCurve( sbe::Curve("Herbs", HerbaeCounts, sf::Color::Green) );
	g.addCurve( sbe::Curve("Herbivore", HerbivoreCounts, sf::Color::Blue) );
	g.addCurve( sbe::Curve("Carnivore", CarnivoreCounts, sf::Color::Red) );

	re->setGraph( g );

	return re;
}

void Simulator::HandleClick( const Geom::Pointf& pos)
{
	const std::shared_ptr<Tile>& T = Terra->getTile( pos );


	// Check if the Coordinates are valid
	if (!T)
	{
		Module::Get()->QueueEvent(Event("TILE_CLICKED", std::shared_ptr<Tile>()), true);
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
	if (tmp)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(200));
		Module::Get()->QueueEvent(Event("UpdateCreatureRenderList", Creatures), true);
	}
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

	Engine::out(Engine::INFO) << "[Simulator] IO Plugins loaded." << std::endl;
}

void Simulator::saveWhole(const std::string &savePath){
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

	Engine::out(Engine::SPAM) << "Save to path: " << Engine::GetIO()->topPath() << std::endl;

	// do some saving...
	if(!Engine::GetIO()->saveObject( "Terrain", *Terra, true)){  // should we overwrite?

		Event e("EVT_SAVE_BAD", std::string("Error saving Terrain!"));
		Module::Get()->QueueEvent(e, true);

	}	else if(!Engine::GetIO()->saveObjects<Creature>(Creatures.begin(), Creatures.end(), true)){

		Event e("EVT_SAVE_BAD", std::string("Error saving Creatures!"));
		Module::Get()->QueueEvent(e, true);

	} else if(!Engine::GetIO()->saveObjects<Species>(SpeciesList.begin(), SpeciesList.end(), true)){ // should we overwrite?

		Event e("EVT_SAVE_BAD", std::string("Error saving Species'!"));
		Module::Get()->QueueEvent(e, true);

	} else {
		auto simCfg = std::shared_ptr<sbe::Config>( new sbe::Config("simState.info", "sim") );
		simCfg->set("sim.currentTick", currentTick);
		// save state of random engine
		std::stringstream ss;
		ss << (*gen);
		simCfg->set("sim.random.gen", ss.str());
		simCfg->set("sim.random.seed", currentSeed);
		simCfg->set("sim.random.numGenerated", numGenerated);

		simCfg->save();

		Module::Get()->QueueEvent("EVT_SAVE_GOOD", true);

	}


	if(!savePath.empty())
		Engine::GetIO()->popPath(); // pop save path from IO stack

	isPaused = wasPaused; // continue, if not wasPaused
	Engine::getCfg()->set("sim.paused", isPaused);
}

void Simulator::loadWhole(const std::string &loadPath){
	bool wasPaused = isPaused;
	isPaused = true; // pause sim while saving
	Engine::getCfg()->set("sim.paused", isPaused);

	if(loadPath.empty() || !Engine::GetIO()->addPath(loadPath))
	{
		Event e("EVT_LOAD_BAD", std::string("Load path invalid ( sim.debugsavepath in config )!"));
		Module::Get()->QueueEvent(e, true);
		return;
	}

	// do some loading...
	auto tmp  = Engine::GetIO()->loadObjects<Terrain>();
	auto tmp2 = Engine::GetIO()->loadObjects<Species>();
	auto tmp3 = Engine::GetIO()->loadObjects<Creature>();
	auto simCfg = std::shared_ptr<sbe::Config>( new sbe::Config("simState.info", "sim") );

	if(tmp.empty() || tmp2.empty() || tmp3.empty() || !simCfg)
	{
		Event e("EVT_LOAD_BAD", std::string("Error loading"));
		Module::Get()->QueueEvent(e, true);

		Engine::out(Engine::ERROR) << "Error loading from '" << loadPath << "'!" << std::endl;
		return;
	}

	if(!loadPath.empty())
		Engine::GetIO()->popPath(); // pop load path from IO stack

	std::list<std::shared_ptr<Creature>> newCreatures;
	std::copy( tmp3.cbegin(), tmp3.cend(), std::inserter(newCreatures, newCreatures.end()) );

	// create random engine
	std::mt19937* newGen = new std::mt19937();
	std::stringstream(simCfg->get<std::string>("sim.random.gen")) >> (*gen);

	NewSimulation( newGen, tmp[0], tmp2, newCreatures );

	currentTick  = simCfg->get<int>("sim.currentTick"); // get ticks
	currentSeed  = simCfg->get<int>("sim.random.seed"); // get seed
	numGenerated = simCfg->get<unsigned int>("sim.random.numGenerated");

	// loading done...
	Module::Get()->QueueEvent("EVT_LOAD_GOOD", true);

	isPaused = wasPaused;
	Engine::getCfg()->set("sim.paused", isPaused);
}
