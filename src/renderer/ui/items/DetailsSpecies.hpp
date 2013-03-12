#ifndef DETAILS_SPECIES_H
#define DETAILS_SPECIES_H

#include "sbe/event/EventUser.hpp"
#include "renderer/ui/items/Details.hpp"
#include "simulator/world/Species.hpp"

#include <SFGUI/SFGUI.hpp>

/// Shows Details about a Species
class DetailsSpecies : public Details
{
	public:

		DetailsSpecies(const std::shared_ptr<Species>& _species);
		virtual ~DetailsSpecies() {}

        virtual sfg::Widget::Ptr GetLabel();
        virtual sfg::Widget::Ptr GetBox();
        virtual void UpdateLabel();
        virtual void UpdateBox();
    private:

		std::shared_ptr<Species> currentSpecies;
};

#endif // DETAILS_SPECIES_H
