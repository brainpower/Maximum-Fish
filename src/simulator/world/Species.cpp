#include "Species.hpp"

#include "sbe/Config.hpp"
#include "sbe/Engine.hpp"

Species::Species(const std::string& _name, SPECIES_TYPE t)
 : name( _name ),
 maxAge( 2400 ), /// 20*60*2 , 2min livetime at 20tps
 maxHealth( 100),
 maxSpeed( .05 ), /// 20 meters
 Reach( 0 ), /// default varies for different types
 resistance( 1.0 ),
 breedingSpeed( 200 ), ///
 type( t ),
 foodRequirement( 10 ), /// a creature with 100hp lasts for 10 days?
 waterRequirement( 10 ), /// just
 optimalTemperature( 20 ), /// 20°C
 ImageSetName("Creatures_serious"),
 Frame( t )
{
	std::string typestring;
	switch ( type )
	{
		case HERBA:
			typestring = "plant";
			maxSpeed = 0;
			break;
		case HERBIVORE:
			typestring = "herbivore";
			break;
		case CARNIVORE:
			typestring = "carnivore";
			break;
	}

	Reach = Engine::getCfg()->get<float>("sim.species.defaults.reach." + typestring);
	breedingSpeed = Engine::getCfg()->get<int>("sim.species.defaults.breedingSpeed." + typestring);

	//Engine::out() << "species!" << std::endl;
	maxAge = Engine::getCfg()->get<int>("sim.species.defaults.maxAge");
	maxHealth = Engine::getCfg()->get<int>("sim.species.defaults.maxHealth");
	maxSpeed = Engine::getCfg()->get<float>("sim.species.defaults.maxSpeed");

	resistance = Engine::getCfg()->get<float>("sim.species.defaults.resistance");
	foodRequirement = Engine::getCfg()->get<int>("sim.species.defaults.foodRequirement");
	waterRequirement = Engine::getCfg()->get<int>("sim.species.defaults.waterRequirement");
	optimalTemperature = Engine::getCfg()->get<int>("sim.species.defaults.optimalTemperature");
	ImageSetName = Engine::getCfg()->get<std::string>("sim.species.defaults.imageSetName");

}

Species::~Species()
{

}
