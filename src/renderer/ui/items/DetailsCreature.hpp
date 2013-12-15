#ifndef DETAILS_CREATURE_H
#define DETAILS_CREATURE_H

#include "sbe/event/EventUser.hpp"
#include "renderer/ui/items/Details.hpp"
#include "simulator/world/Creature.hpp"


/// Shows details about a Creature
class DetailsCreature : public Details
{
	public:

		DetailsCreature(const std::shared_ptr<Creature>& _creature);
		virtual ~DetailsCreature() {}

        virtual std::shared_ptr<sfg::Widget> GetLabel();
        virtual std::shared_ptr<sfg::Widget> GetBox();
        virtual void UpdateLabel();
        virtual void UpdateBox();
    private:

		std::shared_ptr<Creature> currentCreature;
};

#endif // DETAILS_CREATURE_H

