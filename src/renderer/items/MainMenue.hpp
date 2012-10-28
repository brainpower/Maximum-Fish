#ifndef MAIN_MENUE_H
#define MAIN_MENUE_H

#include "sbe/event/EventUser.hpp"

#include <SFGUI/SFGUI.hpp>

class MainMenue : public EventUser
{
	public:

		MainMenue( const Geom::Point& RelativePosition = Geom::Point(0,200), const Geom::Vec2 Size = Geom::Vec2(150, 300));
		virtual ~MainMenue() {};

		virtual void HandleEvent( Event& e);

	private:
		void CreateWindow(const Geom::Point& RelativePosition, const Geom::Vec2 Size);
        void BtnResumeClick();
        void BtnExitClick();

		sfg::Window::Ptr Win;
		unsigned int currentlabeltext;

};

#endif // MAIN_MENUE_H

