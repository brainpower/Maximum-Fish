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

		re.reset( new Species( pt.get<std::string>("name"), (Species::SPECIES_TYPE)pt.get<int>("type") ));

		re->maxAge             = pt.get<int>("maxAge");
		re->maxHealth          = pt.get<float>("maxHealth");
		re->maxSpeed           = pt.get<float>("maxSpeed");
		re->Reach              = pt.get<float>("reach");
		re->resistance         = pt.get<float>("resistance");
		re->breedingSpeed      = pt.get<int>("breedingSpead");
		re->foodRequirement    = pt.get<int>("foodRequirement");
		re->waterRequirement   = pt.get<int>("waterRequirement");
		re->optimalTemperature = pt.get<int>("optimalTemperature");

		re->ImageSetName       = pt.get<std::string>("imageSetName");
		re->Frame              = pt.get<int>("frame");
	}
	catch ( boost::property_tree::ptree_error )
	{
		Engine::out() << "[SpeciesIOPlugin] Error loading Species from ptree!" << std::endl;
		re.reset();
		return re;
	}

	return re;
}

bool SpeciesIOPlugin::saveObject( const std::string& name, const Species &s, boost::property_tree::ptree &r){
	try
	{
		ptree pt;

		pt.put<std::string>("name",       s.name);
		pt.put<int>("maxAge",             s.maxAge);
		pt.put<float>("maxHealth",          s.maxHealth );
		pt.put<float>("maxSpeed",           s.maxSpeed );
		pt.put<float>("reach",              s.Reach );
		pt.put<float>("resistance",         s.resistance );
		pt.put<int>("breedingSpead",      s.breedingSpeed );
		pt.put<int>("type",         s.type );
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
		Engine::out() << "[SpeciesIOPlugin] Error saving Species to ptree!" << std::endl;
		return false;
	}
}
