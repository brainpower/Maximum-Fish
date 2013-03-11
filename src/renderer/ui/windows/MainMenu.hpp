#ifndef MAIN_MENUE_H
#define MAIN_MENUE_H

#include "sbe/event/EventUser.hpp"

#include "sbe/geom/Point.hpp"

#include <SFGUI/SFGUI.hpp>

/// MainMenu
class MainMenu : public EventUser
{
	public:

		MainMenu( const Geom::Vec2 Size = Geom::Vec2(200, 300));
		virtual ~MainMenu() {};

		virtual void HandleEvent( Event& e);

	private:
		void CreateWindow(const Geom::Vec2 Size);
        void BtnResumeClick();

        void BtnSaveClick();
        void BtnLoadClick();

        void BtnExitClick();
        void BtnExitCancelClick();
        void BtnExitConfirmClick();

		void updatePosition();

		sfg::Window::Ptr Win;
		sfg::Button::Ptr BtnExit;
		sfg::Box::Ptr ExitConfirmation;
		unsigned int currentlabeltext;

};

#endif // MAIN_MENUE_H

