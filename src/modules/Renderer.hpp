#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "sbe/Module.hpp"

class Screen;

class Renderer : public Module
{
	public:
		Renderer();
		virtual ~Renderer();


	private:
		virtual void Init();
		virtual void DeInit();

		std::shared_ptr<Screen> Scr;

};

#endif // Renderer_H

