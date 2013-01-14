#ifndef CONTROL_H
#define CONTROL_H

#include "sbe/event/EventUser.hpp"

#include <SFGUI/SFGUI.hpp>

class Control : public EventUser
{
	public:

		Control( const Geom::Vec2 Size = Geom::Vec2(250, 30));
		virtual ~Control() {};

		virtual void HandleEvent( Event& e);

	private:
		void CreateWindow(const Geom::Vec2 Size);
		sfg::ToggleButton::Ptr BtnDbgWin;
        sfg::ToggleButton::Ptr BtnIPanWin;
        sfg::ToggleButton::Ptr BtnMnMnWin;
        sfg::ToggleButton::Ptr BtnMiMapWin;
        sfg::ToggleButton::Ptr BtnSimPause;

        void BtnDbgWinClick();
        void BtnIPanWinClick();
        void BtnMnMnWinClick();
        void BtnMiMapWinClick();
        void BtnSimPauseClick();

		void updatePosition();

		sfg::Window::Ptr Win;
		unsigned int currentlabeltext;

};

#endif // CONTROL_H

