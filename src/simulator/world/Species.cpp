#include "Species.hpp"

#include "sbe/Config.hpp"
#include "sbe/Engine.hpp"

Species::Species(const std::string& _name, SPECIES_TYPE t)
 : name( _name ),
 maxAge( 0 ),
 maxHealth( 0),
 maxSpeed( 0 ),
 Reach( 0 ),
 resistance( 0 ),
 breedingSpeed( 0 ),
 type( t ),
 foodRequirement( 0 ),
 waterRequirement( 0 ),
 optimalTemperature( 0 ),
 ImageSetName(""),
 Frame( t )
{
	std::string typestring = type2String( type );
	Reach = Engine::getCfg()->get<float>("sim.species.defaults.reach")
			* Engine::getCfg()->get<float>("sim.species.defaults.reach." + typestring);

	breedingSpeed = Engine::getCfg()->get<float>("sim.species.defaults.breedingSpeed")
					* Engine::getCfg()->get<int>("sim.species.defaults.breedingSpeed." + typestring);

	//Engine::out() << "species!" << std::endl;
	maxAge = Engine::getCfg()->get<int>("sim.species.defaults.maxAge");
	maxHealth = Engine::getCfg()->get<int>("sim.species.defaults.maxHealth");
	maxSpeed = Engine::getCfg()->get<float>("sim.species.defaults.maxSpeed")
					* Engine::getCfg()->get<int>("sim.species.defaults.maxSpeed." + typestring);

	resistance = Engine::getCfg()->get<float>("sim.species.defaults.resistance");

	maxRegeneration =  Engine::getCfg()->get<float>("sim.species.defaults.maxRegeneration")
						* Engine::getCfg()->get<float>("sim.species.defaults.maxRegeneration." + typestring);

	foodRequirement =  Engine::getCfg()->get<float>("sim.species.defaults.foodRequirement")
						* Engine::getCfg()->get<float>("sim.species.defaults.foodRequirement." + typestring);

	waterRequirement = Engine::getCfg()->get<float>("sim.species.defaults.waterRequirement")
						* Engine::getCfg()->get<float>("sim.species.defaults.waterRequirement." + typestring);

	optimalTemperature = Engine::getCfg()->get<int>("sim.species.defaults.optimalTemperature");

	ImageSetName = Engine::getCfg()->get<std::string>("system.renderer.creatureImageSet");

}

Species::~Species()
{

}
