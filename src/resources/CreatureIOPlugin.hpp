
#ifndef CREATUREIOPLUGIN_HPP
#define CREATUREIOPLUGIN_HPP


#include <boost/property_tree/ptree.hpp>

#include "sbe/io/IOPlugin.hpp"
#include "../simulator/Creature.hpp"

class CreatureIOPlugin : public iTreeIOPlugin<Creature> {
	CreatureIOPlugin(){}
	virtual ~CreatureIOPlugin(){}

	ObjectList loadObjects(boost::ptree::node &root) override;
	bool saveObject( const Creature &o, boost::ptree::node &root) override;
};

#endif // CREATUREIOPLUGIN_HPP
