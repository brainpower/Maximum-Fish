#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "sbe/Module.h"


class LocalGameView : public Module
{
	public:
		LocalGameView();
		virtual ~LocalGameView();


	private:
		virtual void Init();
		virtual void DeInit();


};

#endif // LOCALGAMEVIEW_H

