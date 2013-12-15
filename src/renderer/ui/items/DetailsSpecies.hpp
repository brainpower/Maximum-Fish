#ifndef DETAILS_SPECIES_H
#define DETAILS_SPECIES_H

#include "renderer/ui/items/Details.hpp"
#include "simulator/world/Species.hpp"


/// Shows Details about a Species
class DetailsSpecies : public Details
{
	public:

		DetailsSpecies(const std::shared_ptr<Species>& _species);
		virtual ~DetailsSpecies() {}

        virtual std::shared_ptr<sfg::Widget> GetLabel();
        virtual std::shared_ptr<sfg::Widget> GetBox();
        virtual void UpdateLabel();
        virtual void UpdateBox();
    private:

		std::shared_ptr<Species> currentSpecies;
};

#endif // DETAILS_SPECIES_H

