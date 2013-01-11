#ifndef CREATURE_DETAILS_H
#define CREATURE_DETAILS_H

#include "sbe/event/EventUser.hpp"
#include "simulator/Creature.hpp"

#include <SFGUI/SFGUI.hpp>

class CreatureDetails : public EventUser
{
	public:

		CreatureDetails( const Creature* );
		virtual ~CreatureDetails() {};

		virtual void HandleEvent( Event& e);
		sfg::Box::Ptr Get();
	private:
		void CreateBox();
		sfg::Box::Ptr Details;
};

#endif // CREATURE_DETAILS_H

