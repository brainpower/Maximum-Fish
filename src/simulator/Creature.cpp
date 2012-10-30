#include "Creature.hpp"

#include "sbe/event/EventUser.hpp"
#include "sbe/event/Event.hpp"

Creature::Creature()
{
//variables to be initialized after discussion
}

void Creature::HandleEvent(Event& e)
{
	if (e.Is("EVT_TICK"))
	{
		//here will be the stuff which happens per tick like food - consumption, and so on
	}
}

