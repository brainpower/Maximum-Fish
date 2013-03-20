#include "Simulator.hpp"

#include <boost/thread.hpp>

#include "sbe/ResourceManager.hpp"
#include "sbe/gfx/GraphPlotter.hpp"
#include "sbe/geom/Helpers.hpp"

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
	}
	else if(e.Is("SIM_UNPAUSE"))
	{
		isPaused = false;
		Engine::getCfg()->set("sim.paused", isPaused);
	}
	else if(e.Is("TOGGLE_SIM_PAUSE"))
	{
		isPaused = !isPaused;
		Engine::getCfg()->set("sim.paused", isPaused);
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
	multiThreaded = numThreads > 1;

	NewSimulation(Engine::getCfg()->get<int>("sim.defaultSeed"));
}

void Simulator::NewSimulation( int seed )
{
	currentTick = 0;

	Engine::out(Engine::INFO) << "[Simulator] Seeding random engine" << std::endl;
	Engine::out(Engine::INFO) << "[Simulator] Seed is >> " << boost::lexical_cast<std::string>(seed) << " <<" << std::endl;
	gen.seed( seed );
	currentSeed = seed;
	numGenerated = 0;

	Creature::loadConfigValues();

	Engine::out(Engine::INFO) << "[Simulator] Creating Terrain" << std::endl;
	Terra.reset ( new Terrain() );

	Engine::out(Engine::INFO) << "[Simulator] Generating Debug Terrain" << std::endl;
	Terra->CreateDebugTerrain();

	Engine::out(Engine::INFO) << "[Simulator] Creating creatures and species" << std::endl;

	SpeciesList.clear();
	Creatures.clear();
	CreatureCounts[0] = 0;
	CreatureCounts[1] = 0;
	CreatureCounts[2] = 0;


	// add default species
	std::shared_ptr<Species> S ( new Species( "UNDEFINED_SPECIES", Species::SPECIES_TYPE::HERBA));
	SpeciesList.push_back( S );
	Generator G (*this);


	G.CreateSpeciesWithCreatures( Species::HERBA, 		Engine::getCfg()->get<int>("sim.terragen.plantSpecies"), Engine::getCfg()->get<int>("sim.terragen.plantCount") );
	G.CreateSpeciesWithCreatures( Species::HERBIVORE, 	Engine::getCfg()->get<int>("sim.terragen.herbivoreSpecies"), Engine::getCfg()->get<int>("sim.terragen.herbivoreCount") );
	G.CreateSpeciesWithCreatures( Species::CARNIVORE, 	Engine::getCfg()->get<int>("sim.terragen.carnivoreSpecies"), Engine::getCfg()->get<int>("sim.terragen.carnivoreCount") );

	Engine::out(Engine::INFO) << "[Simulator] Simulation is set up" << std::endl;

	isPaused = Engine::getCfg()->get<bool>("sim.pauseOnStart");
	Engine::getCfg()->set("sim.paused", isPaused);
	// count Creatures once

	for(auto it = Creatures.begin(); it != Creatures.end(); ++it)
		CreatureCounts[ (int)((*it)->getSpecies()->getType()) ]++;

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

void Simulator::advance()
{
	if(!isPaused)
	{

		sf::Clock TickTimer;

		// cleanup dead creatures
		for(auto it = Creatures.begin(); it != Creatures.end(); )
		{
			if((*it)->getCurrentHealth() <= 0)
			{
				(*it)->getTile()->removeCreature(*it);
				auto it2 = it++;
				Creatures.erase( it2 );
			}
			else
			{
				(*it)->done = false;
				++it;
			}
		}

		if ( !multiThreaded )
		{
			std::shared_ptr<std::list<std::shared_ptr<Tile>>> l;
			l.reset( new std::list<std::shared_ptr<Tile>>(Terra->getTileList()) );
			tick(l, CreatureCounts);
		}
		else
			parallelTick();

		Engine::out() << "Tick took " << TickTimer.getElapsedTime().asMilliseconds() << " ms, with " << numThreads << " Threads" << std::endl;

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

void Simulator::tick(std::shared_ptr<std::list<std::shared_ptr<Tile>>> list, int* CreatureCounts)
{
	CreatureCounts[0] = 0;
	CreatureCounts[1] = 0;
	CreatureCounts[2] = 0;

	for( std::shared_ptr<Tile>& T : (*list))
		Engine::out() << "tile: " << T->getPosition() << std::endl;

	int i = 0;
	for( std::shared_ptr<Tile>& T : (*list))
	{
		i++;
		//Engine::out() << "Tick: tile " << i << std::endl;
		for ( auto C : T->getCreatures() )
		{
			//and ya god said live creature !... LIVE !!!
			if (C && !C->done)
			{
				C->live();
				CreatureCounts[ (int)(C->getSpecies()->getType()) ]++;
			}
		}
	}
}

void Simulator::parallelTick()
{

	CreatureCounts[0] = 0;
	CreatureCounts[1] = 0;
	CreatureCounts[2] = 0;

	int b = 0;
	for( auto& batch : Terra->getColors())
	{
		b++;
		int worksize = std::ceil(batch.size() / numThreads);

		std::vector< int* > CreatureCounters;

		Engine::out() << "batch " << b << std::endl;
		Engine::out() << "Work: " << batch.size() << std::endl;
		Engine::out() << "Worksize: " << worksize << std::endl;

		std::vector<std::shared_ptr<std::list<std::shared_ptr<Tile>>>> Lists;

		for ( int thread = 0; thread < numThreads; ++thread)
		{
			int from = thread*worksize;
			int to   = thread*worksize > batch.size()  ?  batch.size()  :  (thread+1)*worksize;

			Engine::out() << "Thread " << thread << ": workload " << from << "-" << to << std::endl;

			TileIt it1 = batch.begin();
			std::advance( it1, from);
			TileIt it2 = batch.begin();
			std::advance( it2, to);

			std::shared_ptr<std::list<std::shared_ptr<Tile>>> cur;
			cur.reset ( new std::list<std::shared_ptr<Tile>> );
			Lists.push_back(cur);

			std::copy(it1, it2, std::inserter( *cur, cur->end() ));

			Engine::out() << "from " << (*it1)->getPosition() << std::endl;
			Engine::out() << "to " << (*it2)->getPosition() << std::endl;

			CreatureCounters.push_back( new int[3] );
		}

		boost::thread* threads[4];

		Engine::out() << "Starting Threads" << std::endl;
		sf::Clock BatchTimer;

		for ( int thread = 0; thread < numThreads; ++thread )
			threads[thread] = new boost::thread( boost::bind( &Simulator::tick, this, Lists[thread], CreatureCounters[thread] ) );


		for ( int thread = 0; thread < numThreads; ++thread )
		{
			Engine::out() << "Joining Thread " << thread << ": " << BatchTimer.getElapsedTime().asMilliseconds() << " ms" << std::endl;
			threads[thread]->join();
			delete threads[thread];
			threads[thread] = 0;

			CreatureCounts[0] += CreatureCounters[thread][0];
			CreatureCounts[1] += CreatureCounters[thread][1];
			CreatureCounts[2] += CreatureCounters[thread][2];

			delete CreatureCounters[thread];
		}

		Lists.clear();
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
		ss << gen;
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

	// clean up, necessary?
	Terra.reset();
	SpeciesList.clear();
	Creatures.clear();

	Terra       = tmp[0]; // resets Terra, there should be only one
	SpeciesList = tmp2; // resets specieslist
	std::copy( tmp3.cbegin(), tmp3.cend(), std::inserter(Creatures, Creatures.end()) );


	currentTick  = simCfg->get<int>("sim.currentTick"); // get ticks
	currentSeed  = simCfg->get<int>("sim.random.seed"); // get seed
	numGenerated = simCfg->get<unsigned int>("sim.random.numGenerated");

	// reset random engine
	std::stringstream ss(simCfg->get<std::string>("sim.random.gen"));
	ss >> gen;

	// reset statistics
	CreatureCounts[0] = 0;
	CreatureCounts[1] = 0;
	CreatureCounts[2] = 0;

	// count Creatures once, or save it too?
	// also, while we're at it, update Tile <-> Creature references
	for(auto it = Creatures.begin(); it != Creatures.end(); ++it){
		CreatureCounts[ (int)((*it)->getSpecies()->getType()) ]++;

		(*it)->updateTileFromPos();
	}

	// send terrain to renderer
	Terra->UpdateTerrain();
	// send creatures to renderer
	Module::Get()->QueueEvent(Event("UpdateCreatureRenderList", Creatures), true);

	RendererUpdate.restart();

	// loading done...
	Module::Get()->QueueEvent("EVT_LOAD_GOOD", true);

	isPaused = wasPaused;
	Engine::getCfg()->set("sim.paused", isPaused);

	Engine::out(Engine::SPAM) << Creatures.front()->getSpeciesString() << std::endl;
	Engine::out(Engine::SPAM) << GetSpecies(Creatures.front()->getSpeciesString())->getName() << std::endl;
}
