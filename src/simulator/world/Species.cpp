#include "Species.hpp"

#include "sbe/Config.hpp"
#include "sbe/Engine.hpp"

Species::Species(const std::string& _name)
 : name( _name ),
 maxAge( 2400 ), /// 20*60*2 , 2min livetime at 20tps
 maxHealth( 100),
 maxSpeed( .05 ), /// 20 meters
 Reach( 5 ), /// 5 Tiles 'a 100m = 500m
 resistance( 1.0 ),
 breedingSpeed( 200 ), ///
 type( HERBIVORE ),
 foodRequirement( 10 ), /// a creature with 100hp lasts for 10 days?
 waterRequirement( 10 ), /// just
 optimalTemperature( 20 ), /// 20°C
 ImageSetName("Creatures_serious"),
 Frame( type )
{
	//Engine::out() << "species!" << std::endl;
	maxAge = Engine::getCfg()->get<int>("sim.species.defaults.maxAge");
	maxHealth = Engine::getCfg()->get<int>("sim.species.defaults.maxHealth");
	maxSpeed = Engine::getCfg()->get<float>("sim.species.defaults.maxSpeed");
	Reach = Engine::getCfg()->get<float>("sim.species.defaults.reach");
	resistance = Engine::getCfg()->get<float>("sim.species.defaults.resistance");
	breedingSpeed = Engine::getCfg()->get<int>("sim.species.defaults.breedingSpeed");
	foodRequirement = Engine::getCfg()->get<int>("sim.species.defaults.foodRequirement");
	waterRequirement = Engine::getCfg()->get<int>("sim.species.defaults.waterRequirement");
	optimalTemperature = Engine::getCfg()->get<int>("sim.species.defaults.optimalTemperature");
	ImageSetName = Engine::getCfg()->get<std::string>("sim.species.defaults.imageSetName");

	if (type == HERBA) maxSpeed = 0;
}

Species::~Species()
{

}

void Species::HandleEvent(Event &e){

}
