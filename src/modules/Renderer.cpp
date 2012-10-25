#include "Renderer.hpp"

#include <boost/lexical_cast.hpp>

#include "sbe/event/SFMLEvent.h"

LocalGameView::LocalGameView()
{

}

LocalGameView::~LocalGameView()
{

}
void LocalGameView::Init()
{
	Engine::out() << "[LocalGameView] Screen..." << std::endl;

	Engine::out() << "Creating Window." << std::endl;

	// create the renderwindow
	Engine::GetApp().create( sf::VideoMode ( 800, 600 ), "Schiffbruch again!" );
}

void LocalGameView::DeInit()
{
	Engine::GetApp().close();
}

