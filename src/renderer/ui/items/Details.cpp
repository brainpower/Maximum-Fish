#include "Details.hpp"

#include <SFGUI/Label.hpp>
#include <SFGUI/Box.hpp>

Details::Details()
{
	UpdateTimer.restart();
	RegisterForEvent("EVT_FRAME");
}


void Details::HandleEvent(Event& e)
{
	if (UpdateTimer.getElapsedTime() > sf::seconds(.33))
	{
		UpdateLabel();
		UpdateTimer.restart();
	}
}

void Details::AddToLabel( sfg::Label::Ptr Label, const std::string& key, const std::string& value )
{
	std::string start = (Label->GetText().getSize() == 0)?"":(Label->GetText() + "\n") ;
	Label->SetText(start + key + ": " + value);
}

void Details::AddLabel( sfg::Box::Ptr Box, const std::string& key, const std::string& value )
{
	sfg::Label::Ptr L = sfg::Label::Create( key +": "+ value );
	L->SetAlignment( sf::Vector2f(0.f, 0.f) );
	Box->Pack( L );
}
