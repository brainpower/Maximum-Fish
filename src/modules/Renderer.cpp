#include "Renderer.hpp"

#include "renderer/Screen.hpp"

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}
void Renderer::Init()
{
	Engine::out() << "[Renderer] Screen..." << std::endl;
	Scr.reset( new Screen() );
}

void Renderer::DeInit()
{
	Engine::GetApp().close();
}

