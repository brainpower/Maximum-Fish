
#ifndef CREATUREIOPLUGIN_HPP
#define CREATUREIOPLUGIN_HPP


#include <boost/property_tree/ptree.hpp>
#include <string>

#include "sbe/io/IOPlugin.hpp"
#include "../simulator/world/Creature.hpp"

class SimState;

/// iTreeIOPlugin for Creature
class CreatureIOPlugin : public sbe::iTreeIOPlugin<Creature>
{

public:

	CreatureIOPlugin()
		: iTreeIOPlugin( "Creatures.info" ) {}

	virtual ~CreatureIOPlugin();

	virtual ObjPtr loadObject(const boost::property_tree::ptree::value_type &node);
	virtual ObjPtr loadObject(const boost::property_tree::ptree::value_type &node, const std::shared_ptr<SimState> &s);
	virtual bool saveObject( const std::string& name, const Creature &o, boost::property_tree::ptree &root);

};

#endif // CREATUREIOPLUGIN_HPP
