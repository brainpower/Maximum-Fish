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
		re->setAge			( pt.get<int>("age") );
		re->setPosition		( pt.get<float>("pos.y"), pt.get<float>(r, i, "pos.y") );
		//c->setSpecies(get<string>(r, i, "species")); // how to save? or restore?
		//c->setCurrentTile(get<well, ...>(r, i, "tile")); // or is position enough?

	}
	catch ( boost::property_tree::ptree_error )
	{
		Engine::out() << "[CreatureIOPlugin] Error loading Creature from ptree!"
		return re.reset();
	}

	return re;
}


bool CreatureIOPlugin::saveObject(const Creature &c, boost::property_tree::ptree &r)
{
	try
	{
		ptree pt;

		pt.put<int>("currentHealth", c.getCurrentHealth());
		pt.put<int>(r, i, "age", c.getAge());
		pt.put<float>(r, i, "pos.x", c.getPosition().x());
		pt.put<float>(r, i, "pos.y", c.getPosition().y());
		//save species
		//save tile?

		r.add( "Creature", pt);
		return true;
	}
	catch ( boost::property_tree::ptree_error )
	{
		Engine::out() << "[CreatureIOPlugin] Error saving Creature to ptree!"
		return false;
	}
}
