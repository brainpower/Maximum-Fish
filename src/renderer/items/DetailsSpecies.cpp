#include "DetailsSpecies.hpp"

#include <boost/format.hpp>
using boost::format;
using boost::io::group;

DetailsSpecies::DetailsSpecies(const std::shared_ptr<Species>& _species)
{
    currentSpecies = _species;

    DetailsLabel = sfg::Label::Create();
    DetailsLabel->SetAlignment( sf::Vector2f(0.f, 0.f) );

	Update();
}

void DetailsSpecies::Update()
{
	DetailsLabel->SetText("");
	AddToLabel( DetailsLabel, "Name", str( format("%s") % currentSpecies->getName()));
	AddToLabel( DetailsLabel, "Max age", str( format("%d") % currentSpecies->getMaxAge()));
	AddToLabel( DetailsLabel, "Max health", str( format("%d") % currentSpecies->getMaxHealth()));
	AddToLabel( DetailsLabel, "Max speed", str( format("%.2f") % currentSpecies->getMaxSpeed()));
	AddToLabel( DetailsLabel, "Reach", str( format("%.2f") % currentSpecies->getReach()));
	AddToLabel( DetailsLabel, "Resistance", str( format(": %.2f") % currentSpecies->getResistance()));
	AddToLabel( DetailsLabel, "Breeding speed", str( format("%.2f") % currentSpecies->getBreedingSpeed()));
	AddToLabel( DetailsLabel, "Food requirement", str( format("%d") % currentSpecies->getFoodRequirement()));
	AddToLabel( DetailsLabel, "Water requirement", str( format("%d") % currentSpecies->getWaterRequirement()));
	AddToLabel( DetailsLabel, "Optimal temperature", str( format("%d") % currentSpecies->getOptimalTemperature()));

}

sfg::Widget::Ptr DetailsSpecies::Get()
{
    return DetailsLabel;
}
