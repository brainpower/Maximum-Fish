#include "DetailsEmpty.hpp"


#include <SFGUI/Label.hpp>
#include <SFGUI/Box.hpp>

DetailsEmpty::DetailsEmpty()
{
    DetailsLabel = sfg::Label::Create("Select a Creature,\nSpecies or Tile\nto see its details.");
    DetailsBox = sfg::Box::Create( sfg::Box::VERTICAL, 3.0f );
    DetailsBox->Pack( sfg::Label::Create( "This is an empty Manipulator." ) );
    DetailsLabel->SetAlignment( sf::Vector2f(0.f, 0.f) );
}

sfg::Widget::Ptr DetailsEmpty::GetLabel()
{
    return DetailsLabel;
}

sfg::Widget::Ptr DetailsEmpty::GetBox()
{
    return DetailsBox;
}
