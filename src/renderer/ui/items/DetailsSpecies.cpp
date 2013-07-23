#include "DetailsSpecies.hpp"


#include <SFGUI/Label.hpp>
#include <SFGUI/Box.hpp>

#include <boost/format.hpp>
using boost::format;
using boost::io::group;

DetailsSpecies::DetailsSpecies(const std::shared_ptr<Species>& _species)
{
    currentSpecies = _species;

    //Label
    DetailsLabel = sfg::Label::Create();
    DetailsLabel->SetAlignment( sf::Vector2f(0.f, 0.f) );

    //Box
    DetailsBox = sfg::Box::Create( sfg::Box::VERTICAL, 3.0f );

	UpdateLabel();
    UpdateBox();
}

void DetailsSpecies::UpdateLabel()
{
	DetailsLabel->SetText("");
	AddToLabel( DetailsLabel, "Name", str( format("%s") % currentSpecies->getName()));
	AddToLabel( DetailsLabel, "Max age/ health", str( format("%d / %d") % currentSpecies->getMaxAge() % currentSpecies->getMaxHealth()));
	AddToLabel( DetailsLabel, "Speed / Reach", str( format("%.2f / %.2f") % currentSpecies->getMaxSpeed() % currentSpecies->getReach()));
	AddToLabel( DetailsLabel, "Resistance", str( format(": %.2f") % currentSpecies->getResistance()));
	AddToLabel( DetailsLabel, "Breeding speed", str( format("%.2f") % currentSpecies->getBreedingSpeed()));
	AddToLabel( DetailsLabel, "Food/Water req", str( format("%.2f / %.2f") % currentSpecies->getFoodRequirement() % currentSpecies->getWaterRequirement() ));
	AddToLabel( DetailsLabel, "Optimal temperature", str( format("%d") % currentSpecies->getOptimalTemperature()));

}

void DetailsSpecies::UpdateBox()
{

	DetailsBox->RemoveAll();
	///todo
}

sfg::Widget::Ptr DetailsSpecies::GetLabel()
{
    return DetailsLabel;
}

sfg::Widget::Ptr DetailsSpecies::GetBox()
{
    UpdateBox();
    return DetailsBox;
}
