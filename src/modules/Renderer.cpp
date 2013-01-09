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
	Engine::out(Engine::INFO) << "[Renderer] Screen..." << std::endl;
	Scr.reset( new Screen() );
}

void Renderer::DeInit()
{
	Engine::GetApp().close();
	Scr.reset();
}

