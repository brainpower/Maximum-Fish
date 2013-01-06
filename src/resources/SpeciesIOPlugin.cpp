#include "SpeciesIOPlugin.hpp"
#include "sbe/Engine.hpp"

using boost::property_tree::ptree;

SpeciesIOPlugin::~SpeciesIOPlugin()
{

}

SpeciesIOPlugin::ObjPtr SpeciesIOPlugin::loadObject(const boost::property_tree::ptree::value_type &node){
		ObjPtr re;
	if (node.first != "Species") return re;

	try
	{
		const ptree& pt = node.second;

		re.reset( new Species( pt.get<std::string>("name") ));

		re->maxAge             = pt.get<int>("maxAge");
		re->maxHealth          = pt.get<int>("maxHealth");
		re->maxSpeed           = pt.get<int>("maxSpeed");
		re->Reach              = pt.get<int>("reach");
		re->resistance         = pt.get<int>("resistance");
		re->breedingSpeed      = pt.get<int>("breedingSpead");
		re->isCarnivore        = pt.get<bool>("carnivore");
		re->foodRequirement    = pt.get<int>("foodRequirement");
		re->waterRequirement   = pt.get<int>("waterRequirement");
		re->optimalTemperature = pt.get<int>("optimalTemperature");

		re->ImageSetName       = pt.get<std::string>("imageSetName");
		re->Frame              = pt.get<int>("frame");
	}
	catch ( boost::property_tree::ptree_error )
	{
		Engine::out() << "[SpeciesIOPlugin] Error loading Species from ptree!";
		re.reset();
		return re;
	}

	return re;
}

bool SpeciesIOPlugin::saveObject( const std::string& name, const Species &s, boost::property_tree::ptree &r){
	try
	{
		ptree pt;

		pt.put<std::string>("Name",       name);
		pt.put<int>("maxAge",             s.maxAge);
		pt.put<int>("maxHealth",          s.maxHealth );
		pt.put<int>("maxSpeed",           s.maxSpeed );
		pt.put<int>("reach",              s.Reach );
		pt.put<int>("resistance",         s.resistance );
		pt.put<int>("breedingSpead",      s.breedingSpeed );
		pt.put<bool>("carnivore",         s.isCarnivore );
		pt.put<int>("foodRequirement",    s.foodRequirement );
		pt.put<int>("waterRequirement",   s.waterRequirement );
		pt.put<int>("optimalTemperature", s.optimalTemperature );

		pt.put<std::string>("imageSetName", s.ImageSetName );
		pt.put<int>("frame",                s.Frame);

		r.add_child( "Species", pt);
		return true;
	}
	catch ( boost::property_tree::ptree_error )
	{
		Engine::out() << "[SpeciesIOPlugin] Error saving Species to ptree!";
		return false;
	}
}
