#include "DetailsSpecies.hpp"

#include <boost/format.hpp>
using boost::format;
using boost::io::group;

DetailsSpecies::DetailsSpecies(const std::shared_ptr<Species>& _species)
{
    currentSpecies = _species;

	DetailsBox = sfg::Box::Create( sfg::Box::VERTICAL, 3.0f );
	Update();
}

void DetailsSpecies::Update()
{
    DetailsBox->RemoveAll();

	AddLabel( DetailsBox, "Name", str( format("%s") % currentSpecies->getName()));
	AddLabel( DetailsBox, "Max age", str( format("%d") % currentSpecies->getMaxAge()));
	AddLabel( DetailsBox, "Max health", str( format("%d") % currentSpecies->getMaxHealth()));
	AddLabel( DetailsBox, "Max speed", str( format("%.2f") % currentSpecies->getMaxSpeed()));
	AddLabel( DetailsBox, "Reach", str( format("%.2f") % currentSpecies->getReach()));
	AddLabel( DetailsBox, "Resistance", str( format(": %.2f") % currentSpecies->getResistance()));
	AddLabel( DetailsBox, "Breeding speed", str( format("%.2f") % currentSpecies->getBreedingSpeed()));
	AddLabel( DetailsBox, "Food requirement", str( format("%d") % currentSpecies->getFoodRequirement()));
	AddLabel( DetailsBox, "Water requirement", str( format("%d") % currentSpecies->getWaterRequirement()));
	AddLabel( DetailsBox, "Optimal temperature", str( format("%d") % currentSpecies->getOptimalTemperature()));
}

sfg::Box::Ptr DetailsSpecies::Get()
{
    return DetailsBox;
}
