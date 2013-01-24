#include "DetailsEmpty.hpp"

DetailsEmpty::DetailsEmpty()
{
    DetailsLabel = sfg::Label::Create("Select a Creature,\nSpecies or Tile\nto see its details.");
    DetailsLabel->SetAlignment( sf::Vector2f(0.f, 0.f) );
}

sfg::Widget::Ptr DetailsEmpty::Get()
{
    return DetailsLabel;
}
