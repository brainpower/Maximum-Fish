#include "Species.hpp"

Species::Species(const std::string& _name)
 : name( _name ),
 maxAge( 2400 ), /// 20*60*2 , 2min livetime at 20tps
 maxHealth( 100),
 maxSpeed( .2 ), /// 20 meters
 Reach( 5 ), /// 5 Tiles 'a 100m = 500m
 resistance( 1.0 ),
 breedingSpeed( 200 ), /// breed every 200 ticks ~ 12times in live
 type( HERBIVORE ),
 foodRequirement( 10 ), /// a creature with 100hp lasts for 10 days?
 waterRequirement( 10 ), /// just
 optimalTemperature( 128 ), /// 256/2
 ImageSetName("Creatures_serious"),
 Frame( type )
{
	if (type == HERBA) maxSpeed = 0;
}

Species::~Species()
{

}

void Species::HandleEvent(Event &e){

}
