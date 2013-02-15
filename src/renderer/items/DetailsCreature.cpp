#include "DetailsCreature.hpp"

#include <boost/format.hpp>
using boost::format;
using boost::io::group;

DetailsCreature::DetailsCreature(const std::shared_ptr<Creature>& _creature)
{
    currentCreature = _creature;

    //Label
    DetailsLabel = sfg::Label::Create();
    DetailsLabel->SetAlignment( sf::Vector2f(0.f, 0.f) );

    //Box
    DetailsBox = sfg::Box::Create();

    UpdateLabel();
}

void DetailsCreature::UpdateLabel()
{

	DetailsLabel->SetText("");
	AddToLabel(DetailsLabel, "Spezies", currentCreature->getSpeciesString());
	AddToLabel(DetailsLabel, "Age", str( format("%d") % currentCreature->getAge() ) );
	AddToLabel(DetailsLabel, "Health", str( format("%d") % currentCreature->getCurrentHealth() ));
	AddToLabel(DetailsLabel, "Position", str( format("%.2f,%.2f") % currentCreature->getPosition().x %  currentCreature->getPosition().y ));

}

sfg::Widget::Ptr DetailsCreature::Get()
{
    return DetailsLabel;
}
