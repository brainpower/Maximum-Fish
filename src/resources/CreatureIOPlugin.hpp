
#ifndef CREATUREIOPLUGIN_HPP
#define CREATUREIOPLUGIN_HPP


#include <boost/property_tree/ptree.hpp>
#include <string>

#include "sbe/io/IOPlugin.hpp"
#include "../simulator/Creature.hpp"


class CreatureIOPlugin : public iTreeIOPlugin<Creature> {

	CreatureIOPlugin()
	{
		file = "Creatures.info";
	}
	virtual ~CreatureIOPlugin(){}


	virtual ObjPtr loadObject(const boost::property_tree::ptree::value_type &node);
	virtual bool saveObject( const Creature &o, boost::property_tree::ptree &root);

};

#endif // CREATUREIOPLUGIN_HPP
