#include "MRenderer.hpp"

#include "sbe/gfx/Screen.hpp"
#include "renderer/SimView.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

MRenderer::MRenderer()
{

}

MRenderer::~MRenderer()
{

}
void MRenderer::Init()
{
	Engine::out(Engine::INFO) << "[MRenderer] Screen..." << std::endl;
	Scr.reset( new sbe::Screen );
	Sim.reset ( new SimView );
}

void MRenderer::DeInit()
{
	Sim.reset();
	Scr.reset();
	Engine::GetApp().close();
}
