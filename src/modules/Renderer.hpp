#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "sbe/Module.hpp"


class Renderer : public Module
{
	public:
		Renderer();
		virtual ~Renderer();


	private:
		virtual void Init();
		virtual void DeInit();


};

#endif // Renderer_H

