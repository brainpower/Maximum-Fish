#include "DetailsCreature.hpp"
#include "simulator/Species.hpp"

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
    DetailsBox = sfg::Box::Create( sfg::Box::VERTICAL, 3.0f );

    UpdateLabel();
}

void DetailsCreature::UpdateLabel()
{

	DetailsLabel->SetText("");
	AddToLabel(DetailsLabel, "Species", currentCreature->getSpeciesString());
	AddToLabel(DetailsLabel, "Age", str( format("%d") % currentCreature->getAge() ) );
	AddToLabel(DetailsLabel, "Health", str( format("%d") % currentCreature->getCurrentHealth() ));
	AddToLabel(DetailsLabel, "Position", str( format("%.2f,%.2f") % currentCreature->getPosition().x %  currentCreature->getPosition().y ));

}

void DetailsCreature::UpdateBox()
{

	DetailsBox->RemoveAll();
	DetailsBox->Pack( sfg::Label::Create( "Species:" ), false, true );
        sfg::ComboBox::Ptr dropdownSpecies( sfg::ComboBox::Create() );
        dropdownSpecies->AppendItem( currentCreature->getSpeciesString() );
	DetailsBox->Pack( dropdownSpecies, false, true );

	DetailsBox->Pack( sfg::Label::Create( "Age:" ), false, true );
        sfg::Scale::Ptr ageScale( sfg::Scale::Create() );
        ageScale->SetRange( 0, currentCreature->getSpecies()->getMaxAge() );
        ageScale->SetValue( currentCreature->getAge() );
	DetailsBox->Pack( ageScale, false, true );
        ageScale->SetRequisition( sf::Vector2f( ageScale->GetAllocation().width, 25 ) );

	DetailsBox->Pack( sfg::Label::Create( "Health:" ), false, true );
        sfg::Scale::Ptr healthScale( sfg::Scale::Create() );
        healthScale->SetRange( 0, currentCreature->getSpecies()->getMaxHealth() );
        healthScale->SetValue( currentCreature->getCurrentHealth() );
	DetailsBox->Pack( healthScale, false, true );
        healthScale->SetRequisition( sf::Vector2f( healthScale->GetAllocation().width, 25 ) );

}

sfg::Widget::Ptr DetailsCreature::GetLabel()
{
    return DetailsLabel;
}

sfg::Widget::Ptr DetailsCreature::GetBox()
{
    UpdateBox();
    return DetailsBox;
}
