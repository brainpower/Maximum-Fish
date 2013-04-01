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

		re->currentHealth = pt.get<float>("currentHealth");
		re->age           = pt.get<int>("age");
		re->lastmating	  = pt.get<int>("lastmating");
		re->Position.x = pt.get<float>("pos.x");
		re->Position.y = pt.get<float>("pos.y");
		re->prevMove.x = pt.get<float>("prevMove.x");
		re->prevMove.y = pt.get<float>("prevMove.y");

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
		pt.put<float>("currentHealth", c.currentHealth);
		pt.put<int>("age", c.age);
		pt.put<float>("pos.x", c.Position.x);
		pt.put<float>("pos.y", c.Position.y);
		pt.put<float>("prevMove.x", c.prevMove.x);
		pt.put<float>("prevMove.y", c.prevMove.y);
		pt.put<int>("lastmating", c.lastmating);
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
