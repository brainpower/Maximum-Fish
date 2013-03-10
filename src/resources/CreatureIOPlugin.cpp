#include "CreatureIOPlugin.hpp"
#include "sbe/Engine.hpp"

#include "simulator/Simulator.hpp"

using boost::property_tree::ptree;

CreatureIOPlugin::~CreatureIOPlugin()
{

}

CreatureIOPlugin::ObjPtr CreatureIOPlugin::loadObject(const boost::property_tree::ptree::value_type &node)
{
	ObjPtr re;
	if (node.first != "Creature") return re;

	try
	{
		const ptree& pt = node.second;

		re.reset( new Creature( Simulator::GetSpecies(pt.get<std::string>("species")) ) );

		re->currentHealth = pt.get<int>("currentHealth");
		re->age           = pt.get<int>("age");

		re->Position.x = pt.get<float>("pos.x");
		re->Position.y = pt.get<float>("pos.y");

	}
	catch ( boost::property_tree::ptree_error )
	{
		Engine::out() << "[CreatureIOPlugin] Error loading Creature from ptree!" << std::endl;
		re.reset();
		return re;
	}

	return re;
}


bool CreatureIOPlugin::saveObject( const std::string& name, const Creature &c, boost::property_tree::ptree &r)
{
	try
	{
		ptree pt;

		pt.put<std::string>("Name", name);
		pt.put<int>("currentHealth", c.currentHealth);
		pt.put<int>("age", c.age);
		pt.put<float>("pos.x", c.Position.x);
		pt.put<float>("pos.y", c.Position.y);
		pt.put<std::string>("species", c.getSpeciesString());

		r.add_child( "Creature", pt);
		return true;
	}
	catch ( boost::property_tree::ptree_error )
	{
		Engine::out() << "[CreatureIOPlugin] Error saving Creature to ptree!" << std::endl;
		return false;
	}
}
