#include "DetailsCreature.hpp"

#include <boost/format.hpp>
using boost::format;
using boost::io::group;

DetailsCreature::DetailsCreature(const std::shared_ptr<Creature>& _creature)
{
    currentCreature = _creature;

    DetailsBox = sfg::Box::Create( sfg::Box::VERTICAL, 3.0f );
    Update();
}

void DetailsCreature::Update()
{
	DetailsBox->RemoveAll();

	AddLabel(DetailsBox, "Spezies", currentCreature->getSpeciesString());
	AddLabel(DetailsBox, "Age", str( format("%d") % currentCreature->getAge() ) );
	AddLabel(DetailsBox, "Health", str( format("%d") % currentCreature->getCurrentHealth() ));
	AddLabel(DetailsBox, "Position", str( format("%.2f,%.2f") % currentCreature->getPosition().x %  currentCreature->getPosition().y ));

}

sfg::Box::Ptr DetailsCreature::Get()
{
    return DetailsBox;
}
