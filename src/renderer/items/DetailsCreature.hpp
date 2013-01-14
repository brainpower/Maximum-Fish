#ifndef DETAILS_CREATURE_H
#define DETAILS_CREATURE_H

#include "sbe/event/EventUser.hpp"
#include "renderer/items/Details.hpp"
#include "simulator/Creature.hpp"

#include <SFGUI/SFGUI.hpp>

class DetailsCreature : public Details
{
	public:

		DetailsCreature(const std::shared_ptr<Creature>& _creature);
		virtual ~DetailsCreature() {}

        virtual sfg::Box::Ptr Get();
        virtual void Update();
    protected:
        sfg::Box::Ptr DetailsBox;
	private:
		std::shared_ptr<Creature> currentCreature;
};

#endif // DETAILS_CREATURE_H

