#include "CreatureIOPlugin.hpp"
#include <sstream>

using boost::property_tree::ptree;

CreatureIOPlugin::ObjPtr CreatureIOPlugin::loadObject(const boost::property_tree::ptree::value_type &node)
{
	ObjPtr re;
	if (node.first != "Creature") return re;

	try
	{
		const ptree& pt = node.second;

		re.reset( new Creature() );

		re->setCurrentHealth( pt.get<int>("currentHealth") );
		re->setAge( pt.get<int>("age") );
		re->setPosition( pt.get<float>("pos.x"), pt.get<float>("pos.y") );
		re->setSpecies(pt.get<string>("species"));
		// how to save? or restore? -> setSpecies has to take a string and do the right thing

	}
	catch ( boost::property_tree::ptree_error )
	{
		Engine::out() << "[CreatureIOPlugin] Error loading Creature from ptree!";
		re.reset();
		return re;
	}

	return re;
}


bool CreatureIOPlugin::saveObject(const Creature &c, boost::property_tree::ptree &r)
{
	try
	{
		ptree pt;

		pt.put<int>("currentHealth", c.getCurrentHealth());
		pt.put<int>("age", c.getAge());
		pt.put<float>("pos.x", c.getPosition().x());
		pt.put<float>("pos.y", c.getPosition().y());
		pt.put<string>("species", c.getSpeciesString());

		r.add_child( "Creature", pt);
		return true;
	}
	catch ( boost::property_tree::ptree_error )
	{
		Engine::out() << "[CreatureIOPlugin] Error saving Creature to ptree!";
		return false;
	}
}
