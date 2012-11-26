
#ifndef CREATUREIOPLUGIN_HPP
#define CREATUREIOPLUGIN_HPP


#include <boost/property_tree/ptree.hpp>

#include "sbe/io/IOPlugin.hpp"
#include "../simulator/Creature.hpp"

class CreatureIOPlugin : public iTreeIOPlugin<Creature> {
	CreatureIOPlugin(){}
	virtual ~CreatureIOPlugin(){}

#ifndef __GCC_4_6__ // gcc < 4.7 doesn't support override
	ObjectList loadObjects(boost::property_tree::ptree &root) override;
	bool saveObject( const Creature &o, boost::property_tree::ptree &root) override;
#else
	ObjectList loadObjects(boost::property_tree::ptree &root);
	bool saveObject( const Creature &o, boost::property_tree::ptree &root);
#endif
};

#endif // CREATUREIOPLUGIN_HPP
