#include "Simulator.hpp"

#include <boost/thread.hpp>

#include "sbe/ResourceManager.hpp"
#include "sbe/gfx/GraphPlotter.hpp"
#include "sbe/geom/Helpers.hpp"

#include "sbe/Config.hpp"

#include "Terrain.hpp"
#include "Creature.hpp"
#include "Tile.hpp"

#include "resources/CreatureIOPlugin.hpp"
#include "resources/SpeciesIOPlugin.hpp"
#include "resources/TerrainIOPlugin.hpp"

#include <string>

Simulator* Simulator::Instance = nullptr;

Simulator::Simulator():
currentTick(0), isPaused(false) {

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

	RegisterForEvent("PLOT_COUNTS");

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
	else if(e.Is("TERRAIN_CLICKED", typeid( Geom::Pointf )))
	{
		// cast into desired type
		HandleClick( boost::any_cast<Geom::Pointf>( e.Data() ) );
	}
	else if (e.Is("SET_SIM_TPS", typeid(unsigned int)))
	{
		Module::Get()->SetTPS(boost::any_cast<unsigned int>(e.Data()));
	}
	else if (e.Is("PLOT_COUNTS"))
	{
		std::shared_ptr<GraphPlotter> p = CreateCountPlotter();
		if ( p->isValid() )
		{
			Event ev("DISPLAY_GRAPH");
			ev.SetData( p );
			Module::Get()->QueueEvent(ev, true);
		}

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
		saveWhole( Engine::getCfg()->get<std::string>("system.sim.debugsavepath"));
	}
	else if (e.Is("EVT_LOAD_WHOLE_TEST")) {
		loadWhole( Engine::getCfg()->get<std::string>("system.sim.debugsavepath"));
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

	NewSimulation(Engine::getCfg()->get<int>("system.sim.defaultSeed"));
}

void Simulator::NewSimulation( int seed )
{
	currentTick = 0;

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
	CreatureCounts[0] = 0;
	CreatureCounts[1] = 0;
	CreatureCounts[2] = 0;


	// add default speciees
	std::shared_ptr<Species> S ( new Species( "UNDEFINED_SPECIES" ));
	SpeciesList.push_back( S );

	CreateSpeciesWithCreatures( Species::HERBA, 		Engine::getCfg()->get<int>("system.sim.terragen.plantSpecies"), Engine::getCfg()->get<int>("system.sim.terragen.plantCount") );
	CreateSpeciesWithCreatures( Species::HERBIVORE, 	Engine::getCfg()->get<int>("system.sim.terragen.herbivoreSpecies"), Engine::getCfg()->get<int>("system.sim.terragen.herbivoreCount") );
	CreateSpeciesWithCreatures( Species::CARNIVORE, 	Engine::getCfg()->get<int>("system.sim.terragen.carnivoreSpecies"), Engine::getCfg()->get<int>("system.sim.terragen.carnivoreCount") );

	Engine::out(Engine::INFO) << "[Simulator] Simulation is set up" << std::endl;

	isPaused = Engine::getCfg()->get<bool>("system.sim.pauseOnStart");
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

		if ( !isPaused )
		{
			Event e("UpdateCreatureRenderList");
			e.SetData ( Creatures );
			Module::Get()->QueueEvent(e, true);
		}

		RendererUpdate.restart();
	}
}

void Simulator::logTickStats()
{
	HerbaeCounts.push_back(CreatureCounts[(int)Species::HERBA]);
	HerbivoreCounts.push_back(CreatureCounts[(int)Species::HERBIVORE]);
	CarnivoreCounts.push_back(CreatureCounts[(int)Species::CARNIVORE]);

	//ProcessingTimes.push_back(  )
}

std::shared_ptr<GraphPlotter> Simulator::CreateCountPlotter()
{
	std::shared_ptr<GraphPlotter> re( new GraphPlotter );

	Graph g;
	g.Size = Geom::Point( Engine::getCfg()->get<int>("system.sim.countplot.size.x"),Engine::getCfg()->get<int>("system.sim.countplot.size.y"));
	g.AxisSize = Geom::Point(Engine::getCfg()->get<int>("system.sim.countplot.axissize.x"), Engine::getCfg()->get<int>("system.sim.countplot.axissize.y"));
	g.addCurve( Curve("Herbs", HerbaeCounts, sf::Color::Green) );
	g.addCurve( Curve("Herbivore", HerbivoreCounts, sf::Color::Blue) );
	g.addCurve( Curve("Carnivore", CarnivoreCounts, sf::Color::Red) );

	re->setGraph( g );

	return re;
}

void Simulator::HandleClick( const Geom::Pointf& pos)
{



	// Check if the Coordinates are valid
	if (!(
			(0 <= pos.x && pos.x < Terra->getSize().x + 1 )
		 && (0 <= pos.y && pos.y < Terra->getSize().y +1 )
		))
	{
		Event e("TILE_CLICKED");
		e.SetData( std::shared_ptr<Tile>() );
		Module::Get()->QueueEvent(e, true);
		return;
	}



	std::shared_ptr<Tile>& T = Terra->getTile( pos );

	Event e("TILE_CLICKED");
	e.SetData( T );
	Module::Get()->QueueEvent(e, true);


	Event ev("CREATURE_CLICKED");
	ev.SetData( std::shared_ptr<Creature>() );

	for ( std::shared_ptr<Creature>& C : T->getCreatures())
	{
		if ( Geom::distance(pos, C->getPosition()) < .1 )
		{
			ev.SetData( C );
			// only return 1 Creature
			break;
		}
	}

	Module::Get()->QueueEvent(ev, true);

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
	std::uniform_int_distribution<int> temp_rnd(Engine::getCfg()->get<int>("system.sim.species.rnd.temp.min"),Engine::getCfg()->get<int>("system.sim.species.rnd.temp.max"));

	Species::SPECIES_TYPE t = (Species::SPECIES_TYPE) type_rnd(gen);
		numGenerated++; // one more random number generated, don't like counting it this way

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
		numGenerated++; // one more random number generated, don't like counting it this way

	SpeciesList.push_back( S );
}

void Simulator::addRandomCreature()
{
	std::uniform_real_distribution<float> rnd(0,32);
	std::uniform_int_distribution<int> species_rnd(0,SpeciesList.size()-1);

	std::shared_ptr<Creature> ptr_creature = std::shared_ptr<Creature>(new Creature( SpeciesList[species_rnd(gen)] ));
		numGenerated++; // one more random number generated, don't like counting it this way
	Geom::Pointf Position (rnd(gen),rnd(gen));
		numGenerated+=2; // two more random number generated, don't like counting it this way

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
			numGenerated+=2; // two more random number generated, don't like counting it this way

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
	std::uniform_int_distribution<int> temp_rnd(Engine::getCfg()->get<int>("system.sim.species.rnd.temp.min"),Engine::getCfg()->get<int>("system.sim.species.rnd.temp.max"));

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
		numGenerated++; // one more random number generated, don't like counting it this way
	SpeciesList.push_back( S );

	return S->getName();
}

void Simulator::saveWhole(const std::string &savePath){
	bool wasPaused = isPaused;

	isPaused = true; // pause sim while saving

	// add save path to IO stack
	if(savePath.empty() || !Engine::GetIO()->addPath(savePath))
	{
		Event e("EVT_SAVE_BAD");
		e.SetData( std::string("Save path invalid ( sim.debugsavepath in config )!") );
		Module::Get()->QueueEvent(e, true);
		return;
	}

	Engine::out(Engine::SPAM) << "Save to path: " << Engine::GetIO()->topPath() << std::endl;

	// do some saving...
	if(!Engine::GetIO()->saveObject( "Terrain", *Terra, true)){  // should we overwrite?

		Event e("EVT_SAVE_BAD");
		e.SetData( std::string("Error saving Terrain!") );
		Module::Get()->QueueEvent(e, true);

	}	else if(!Engine::GetIO()->saveObjects<Creature>(Creatures.begin(), Creatures.end(), true)){

		Event e("EVT_SAVE_BAD");
		e.SetData( std::string("Error saving Creatures!") );
		Module::Get()->QueueEvent(e, true);

	} else if(!Engine::GetIO()->saveObjects<Species>(SpeciesList.begin(), SpeciesList.end(), true)){ // should we overwrite?

		Event e("EVT_SAVE_BAD");
		e.SetData( std::string("Error saving Species'!") );
		Module::Get()->QueueEvent(e, true);

	} else {
		auto simCfg = shared_ptr<Config>(new Config("simState.info", "sim"));
		simCfg->set("sim.currentTick", currentTick);
		// save state of random engine
		simCfg->set("sim.random.seed", currentSeed);
		simCfg->set("sim.random.numGenerated", numGenerated);

		simCfg->save();

		Event e("EVT_SAVE_GOOD");
		Module::Get()->QueueEvent(e, true);

	}


	if(!savePath.empty())
		Engine::GetIO()->popPath(); // pop save path from IO stack

	isPaused = wasPaused; // continue, if not wasPaused

}

void Simulator::loadWhole(const std::string &loadPath){
	bool wasPaused = isPaused;
	isPaused = true; // pause sim while saving

	if(loadPath.empty() || !Engine::GetIO()->addPath(loadPath))
	{
		Event e("EVT_LOAD_BAD");
		e.SetData( std::string("Load path invalid ( sim.debugsavepath in config )!") );
		Module::Get()->QueueEvent(e, true);
		return;
	}

	// do some loading...

	auto tmp  = Engine::GetIO()->loadObjects<Terrain>();
	auto tmp2 = Engine::GetIO()->loadObjects<Species>();
	auto tmp3 = Engine::GetIO()->loadObjects<Creature>();
	auto simCfg = shared_ptr<Config>(new Config("simState.info", "sim"));

	if(tmp.empty() || tmp2.empty() || tmp3.empty() || !simCfg){
		Event e("EVT_LOAD_BAD");
		e.SetData(std::string("Error loading"));
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
	Creatures   = a2list<shared_ptr<Creature>>(tmp3.cbegin(),tmp3.cend()); // resets creaturelist

	currentTick  = simCfg->get<int>("sim.currentTick");// get ticks
	currentSeed  = simCfg->get<int>("sim.random.seed");// get seed
	numGenerated = simCfg->get<unsigned int>("sim.random.numGenerated");

	// reset random engine
	gen.seed( currentSeed );
	gen.discard(numGenerated);// load random engine state

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
	Event e("UpdateCreatureRenderList");
	e.SetData ( Creatures );
	Module::Get()->QueueEvent(e, true);

	RendererUpdate.restart();

	// loading done...
	Event e2("EVT_LOAD_GOOD");
	Module::Get()->QueueEvent(e2, true);

	isPaused = wasPaused;

	Engine::out(Engine::SPAM) << Creatures.front()->getSpeciesString() << std::endl;
	Engine::out(Engine::SPAM) << GetSpecies(Creatures.front()->getSpeciesString())->getName() << std::endl;
}

template<class T, class FwdIterator>
std::list<T> Simulator::a2list(FwdIterator begin, FwdIterator end){
	std::list<T> lst;
	for( auto it = begin; it != end; ++it){
		lst.push_back(*it);
	}
	return lst;
}
