
#ifndef CREATUREIOPLUGIN_HPP
#define CREATUREIOPLUGIN_HPP


#include <boost/property_tree/ptree.hpp>
#include <string>

#include "sbe/io/IOPlugin.hpp"
#include "../simulator/Creature.hpp"

/// iTreeIOPlugin for Creature
class CreatureIOPlugin : public iTreeIOPlugin<Creature>
{

public:

	CreatureIOPlugin()
		: iTreeIOPlugin( "Creatures.info" ) {}

	virtual ~CreatureIOPlugin();

	virtual ObjPtr loadObject(const boost::property_tree::ptree::value_type &node);
	virtual bool saveObject( const std::string& name, const Creature &o, boost::property_tree::ptree &root);

};

#endif // CREATUREIOPLUGIN_HPP
