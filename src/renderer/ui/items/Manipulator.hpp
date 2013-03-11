#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include "sbe/event/EventUser.hpp"
#include "renderer/ui/items/DetailsEmpty.hpp"
#include "renderer/ui/items/DetailsCreature.hpp"
#include "renderer/ui/items/DetailsSpecies.hpp"
#include "renderer/ui/items/DetailsTile.hpp"

#include <SFGUI/SFGUI.hpp>

/// Base class for manipulators for simulation entities
class Manipulator : public EventUser
{
	public:

		Manipulator();
		virtual ~Manipulator() {};

		virtual void HandleEvent( Event& e );
		virtual sfg::Box::Ptr Get();
    private:
        bool ThisNotSelector;
		void SwitchToSelector();
        void CreateBox();
		void SetInformation_1();
		void SetInformation_2();
		void ResetInformation();

        std::shared_ptr<Details> CurrentDetailsBox;
		sfg::Box::Ptr WholeBox;

        sfg::Label::Ptr Information;
};

#endif // MANIPULATOR_H

