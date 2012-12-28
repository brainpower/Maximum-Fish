#include "SpeciesIOPlugin.hpp"

SpeciesIOPlugin::ObjPtr SpeciesIOPlugin::loadObject(const boost::property_tree::ptree::value_type &node){
		ObjPtr re;
	if (node.first != "Species") return re;

	try
	{
		const ptree& pt = node.second;

		re.reset( new Species() );

		re->setMaxAge( pt.get<int>("maxAge") );
		re->setMaxHealth( pt.get<int>("maxHealth") );
		re->setMaxSpeed( pt.get<int>("maxSpeed") );
		re->setReach( pt.get<int>("reach") );
		re->setResistance( pt.get<int>("resistance") );
		re->setBreedingSpeed( pt.get<int>("breedingSpead") );
		re->setCarnivore( pt.get<bool>("carnivore") );
		re->setFoodRequirement( pt.get<int>("foodRequirement") );
		re->setWaterRequirement( pt.get<int>("waterRequirement") );
		re->setOptimalHeight( pt.get<int>("optimalHeight") );

		//re->setImageSetName( pt.get<string>("imageSetName") );
	}
	catch ( boost::property_tree::ptree_error )
	{
		Engine::out() << "[SpeciesIOPlugin] Error loading Species from ptree!";
		re.reset();
		return re;
	}

	return re;
}

bool SpeciesIOPlugin::saveObject(const Species &s, boost::property_tree::ptree &r){
	try
	{
		ptree pt;

		pt.put<int>("maxAge"), s.getMaxAge());
		pt.put<int>("maxHealth"), s.getMaxHealth() );
		pt.put<int>("maxSpeed"), s.getMaxSpeed() );
		pt.put<int>("reach"), s.getReach() );
		pt.put<int>("resistance"), s.getResistance() );
		pt.put<int>("breedingSpead"), s.getBreedingSpeed() );
		pt.put<bool>("carnivore"), s.IsCarnivore() );
		pt.put<int>("foodRequirement"), s.getFoodRequirement() );
		pt.put<int>("waterRequirement"), s.getWaterRequirement() );
		pt.put<int>("optimalHeight"), s.getOptimalHeight() );

		//pt.put<string>("imageSetName", s.getImageSetName());

		r.add_child( "Species", pt);
		return true;
	}
	catch ( boost::property_tree::ptree_error )
	{
		Engine::out() << "[SpeciesIOPlugin] Error saving Species to ptree!";
		return false;
	}
}
