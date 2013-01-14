#include "DetailsEmpty.hpp"

DetailsEmpty::DetailsEmpty()
{
    DetailsBox = sfg::Box::Create( sfg::Box::VERTICAL, 3.0f );
    sfg::Label::Ptr label( sfg::Label::Create("Select a Creature,\nSpecies or Tile\nto see its details.") );
    DetailsBox->Pack(label);
}

sfg::Box::Ptr DetailsEmpty::Get()
{
    return DetailsBox;
}
