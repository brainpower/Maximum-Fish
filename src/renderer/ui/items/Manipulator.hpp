#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include "sbe/event/EventUser.hpp"
#include "renderer/ui/items/DetailsEmpty.hpp"
#include "renderer/ui/items/DetailsCreature.hpp"
#include "renderer/ui/items/DetailsSpecies.hpp"
#include "renderer/ui/items/DetailsTile.hpp"

#include <SFGUI/SharedPtr.hpp>
namespace sfg
{
	class Box;
	class Label;
}

/// Base class for manipulators for simulation entities
class Manipulator : public sbe::EventUser
{
	public:

		Manipulator();
		virtual ~Manipulator() {};

		virtual void HandleEvent( Event& e );
		virtual sfg::SharedPtr<sfg::Box> Get();
    private:
        bool ThisNotSelector;
		void SwitchToSelector();
        void CreateBox();
		void SetInformation_1();
		void SetInformation_2();
		void ResetInformation();

        std::shared_ptr<Details> CurrentDetailsBox;
		sfg::SharedPtr<sfg::Box> WholeBox;

        sfg::SharedPtr<sfg::Label>  Information;
};

#endif // MANIPULATOR_H

