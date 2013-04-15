#include "StasisPodIOPlugin.hpp"
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

StasisPodIOPlugin::~StasisPodIOPlugin(){}

StasisPodIOPlugin::ObjPtr StasisPodIOPlugin::loadObject(const boost::property_tree::ptree::value_type &node){

	ObjPtr re;
	if(node.first != "StasisPod") return re;

	try{

		const ptree& pt = node.second;

		auto siop = std::dynamic_pointer_cast<SimStateIOPlugin>(Engine::GetIO()->getPlugin(std::type_index( typeid(SimState) )));

		re.reset( new StasisPod() );

		for( const ptree::value_type& e : pt){
			if (e.first == "SimState") {
				re->_pod.push_back(siop->loadObject(e));
			}
		}

		if(re->_pod.empty()) {
			Engine::out(Engine::ERROR) << "[StasisPodIOPlugin] Error loading StasisPod from ptree!" << std::endl;
			return re;
		}

	}
	catch ( boost::property_tree::ptree_error ) {

		Engine::out(Engine::ERROR) << "[StasisPodIOPlugin] Error loading StasisPod from ptree!" << std::endl;
		re.reset();
		return re;
	}

	return re;

}

bool StasisPodIOPlugin::saveObject( const std::string& name, const StasisPod &pod, boost::property_tree::ptree &root){

	try {

		ptree pt;

		auto siop = std::dynamic_pointer_cast<SimStateIOPlugin>(Engine::GetIO()->getPlugin(std::type_index( typeid(SimState) )));

		for( auto s : pod._pod )
			siop->saveObject("SimState", *s, pt);

		root.add_child("StasisPod", pt);
		return true;
	}
	catch ( boost::property_tree::ptree_error )
    {
        Engine::out(Engine::ERROR) << "[StasisPodIOPlugin] Error saving StasisPod to ptree!" << std::endl;
        return false;
    }

}
