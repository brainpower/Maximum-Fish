#ifndef MAIN_MENUE_H
#define MAIN_MENUE_H

#include "sbe/event/EventUser.hpp"

#include <SFGUI/SFGUI.hpp>

class MainMenu : public EventUser
{
	public:

		MainMenu( const Geom::Vec2 Size = Geom::Vec2(200, 300));
		virtual ~MainMenu() {};

		virtual void HandleEvent( Event& e);

	private:
		void CreateWindow(const Geom::Vec2 Size);
        void BtnResumeClick();
        void BtnExitClick();
		void updatePosition();

		sfg::Window::Ptr Win;
		unsigned int currentlabeltext;

};

#endif // MAIN_MENUE_H
