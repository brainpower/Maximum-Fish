#include "SimStateIOPlugin.hpp"

#include "TerrainIOPlugin.hpp"
#include "SpeciesIOPlugin.hpp"
#include "CreatureIOPlugin.hpp"

#include "sbe/Engine.hpp"
#include "sbe/io/IO.hpp"

#include "simulator/world/Terrain.hpp"
#include "simulator/world/Creature.hpp"
#include "simulator/world/Species.hpp"

using boost::property_tree::ptree;

SimStateIOPlugin::~SimStateIOPlugin(){}

SimStateIOPlugin::ObjPtr SimStateIOPlugin::loadObject(const boost::property_tree::ptree::value_type &node){

	ObjPtr re;
	if(node.first != "SimState") return re;

	try{

		const ptree& pt = node.second;

		auto tiop = std::dynamic_pointer_cast<TerrainIOPlugin>(Engine::GetIO()->getPlugin(std::type_index( typeid(Terrain) )));
		auto siop = std::dynamic_pointer_cast<SpeciesIOPlugin>(Engine::GetIO()->getPlugin(std::type_index( typeid(Species) )));
		auto ciop = std::dynamic_pointer_cast<CreatureIOPlugin>(Engine::GetIO()->getPlugin(std::type_index( typeid(Creature) )));

		re.reset( new SimState() );
		re->species = std::make_shared<std::vector<std::shared_ptr<Species>>>();

		for( const ptree::value_type& e : pt){
			if(e.first == "Terrain")       re->terrain = tiop->loadObject(e);
			else if (e.first == "Species") re->species->push_back(siop->loadObject(e));
		}
		for( const ptree::value_type& e : pt)
			if (e.first == "Creature")
				re->creatures.push_back(ciop->loadObject(e, re));


		if(!re->terrain || re->species->empty() || re->creatures.empty()) {
			Engine::out(Engine::ERROR) << "[SimStateIOPlugin] Error loading SimState from ptree!" << std::endl;
			return re;
		}

		re->seeder.reset(new std::mt19937());
		std::stringstream(pt.get<std::string>("sim.random.seeder")) >> (*(re->seeder));

		for ( const boost::property_tree::ptree::value_type& e : pt){
			if(e.first == "gen"){
				auto genpt = e.second;

				std::shared_ptr<std::mt19937> gen(new std::mt19937());
				std::stringstream(genpt.get<std::string>("gen")) >> (*(gen));
				re->gens.push_back(gen);
			}
		}

		re->currentTick  = pt.get<int>("sim.currentTick"); // get ticks
		re->currentSeed  = pt.get<int>("sim.random.seed"); // get seed
		re->numGenerated = pt.get<unsigned int>("sim.random.numGenerated");
		re->numThreads   = pt.get<int>("sim.numThreads");

	}
	catch ( boost::property_tree::ptree_error ) {

		Engine::out(Engine::ERROR) << "[SimStateIOPlugin] Error loading SimState from ptree!" << std::endl;
		re.reset();
		return re;
	}

	return re;

}

bool SimStateIOPlugin::saveObject( const std::string& name, const SimState &state, boost::property_tree::ptree &root){

	try {

		ptree pt;

		auto tiop = std::dynamic_pointer_cast<TerrainIOPlugin>(Engine::GetIO()->getPlugin(std::type_index( typeid(Terrain) )));
		auto siop = std::dynamic_pointer_cast<SpeciesIOPlugin>(Engine::GetIO()->getPlugin(std::type_index( typeid(Species) )));
		auto ciop = std::dynamic_pointer_cast<CreatureIOPlugin>(Engine::GetIO()->getPlugin(std::type_index( typeid(Creature) )));

		tiop->saveObject( "Terrain", *(state.terrain), pt );

		for( auto c : state.creatures )
			ciop->saveObject("Creature", *c, pt);

		for( auto s : *state.species )
			siop->saveObject("Species", *s, pt);

		pt.put("sim.currentTick", state.currentTick);

		// save state of random engines
		std::stringstream ss;
		ss << (*(state.seeder));
		pt.put("sim.random.seeder", ss.str());

		for( auto gen : state.gens){
			ptree genspt;
			ss.str("");
			ss << (*gen);
			genspt.put("gen", ss.str());
			pt.add_child( "gen", genspt);
		}

		pt.put("sim.random.seed", state.currentSeed);
		pt.put("sim.random.numGenerated", state.numGenerated);
		pt.put("sim.numThreads", state.numThreads);

		root.add_child("SimState", pt);
		return true;
	}
	catch ( boost::property_tree::ptree_error )
    {
        Engine::out(Engine::ERROR) << "[SimStateIOPlugin] Error saving SimState to ptree!" << std::endl;
        return false;
    }

}
