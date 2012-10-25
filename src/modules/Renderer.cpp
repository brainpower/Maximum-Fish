#include "Renderer.hpp"

#include <boost/lexical_cast.hpp>

#include "sbe/event/SFMLEvent.hpp"

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}
void Renderer::Init()
{
	Engine::out() << "[Renderer] Screen..." << std::endl;

	Engine::out() << "Creating Window." << std::endl;

	// create the renderwindow
	Engine::GetApp().create( sf::VideoMode ( 800, 600 ), "Schiffbruch again!" );
}

void Renderer::DeInit()
{
	Engine::GetApp().close();
}

