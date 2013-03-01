#ifndef Frontend_H
#define Frontend_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "sbe/Module.hpp"

class Screen;

class MRenderer : public Module
{
	public:
		MRenderer();
		virtual ~MRenderer();


	private:
		virtual void Init();
		virtual void DeInit();

		std::shared_ptr<Screen> Scr;

};

#endif // Frontend_H

