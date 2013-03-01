#include "MRenderer.hpp"

#include "Renderer/Screen.hpp"

MRenderer::MRenderer()
{

}

MRenderer::~MRenderer()
{

}
void MRenderer::Init()
{
	Engine::out(Engine::INFO) << "[MRenderer] Screen..." << std::endl;
	Scr.reset( new Screen() );
}

void MRenderer::DeInit()
{
	Engine::GetApp().close();
	Scr.reset();
}

