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

		std::shared_ptr<Terrain> t;
		std::vector<std::shared_ptr<Species>> s;
		std::list<std::shared_ptr<Creature>> c;

		for( const ptree::value_type& e : pt){
			if(e.first == "Terrain"){
				t = tiop->loadObject(e);
			} else if (e.first == "Species") {
				s.push_back(siop->loadObject(e));
			} else if (e.first == "Creature") {
				c.push_back(ciop->loadObject(e));
			}
		}

		if(!t || s.empty() || c.empty()) {
			Engine::out(Engine::ERROR) << "[SimStateIOPlugin] Error loading SimState from ptree!" << std::endl;
			return re;
		}

		re.reset( new SimState() );

		std::copy( c.cbegin(), c.cend(), std::inserter(re->_creatures, re->_creatures.end()) );

		re->_species = s;
		re->_terrain = t;

		re->_gen.reset(new std::mt19937());
		std::stringstream(pt.get<std::string>("sim.random.gen")) >> (*(re->_gen));

		re->_currentTick  = pt.get<int>("sim.currentTick"); // get ticks
		re->_currentSeed  = pt.get<int>("sim.random.seed"); // get seed
		re->_numGenerated = pt.get<unsigned int>("sim.random.numGenerated");

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

		tiop->saveObject( "Terrain", *(state._terrain), pt );

		for( auto c : state._creatures )
			ciop->saveObject("Creature", *c, pt);

		for( auto s : state._species )
			siop->saveObject("Species", *s, pt);

		pt.put("sim.currentTick", state._currentTick);

		// save state of random engine
		std::stringstream ss;
		ss << (*(state._gen));
		pt.put("sim.random.gen", ss.str());
		pt.put("sim.random.seed", state._currentSeed);
		pt.put("sim.random.numGenerated", state._numGenerated);

		root.add_child("SimState", pt);
		return true;
	}
	catch ( boost::property_tree::ptree_error )
    {
        Engine::out(Engine::ERROR) << "[SimStateIOPlugin] Error saving SimState to ptree!" << std::endl;
        return false;
    }

}
