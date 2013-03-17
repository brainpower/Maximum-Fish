#ifndef MAIN_MENUE_H
#define MAIN_MENUE_H

#include "sbe/event/EventUser.hpp"

#include "sbe/geom/Point.hpp"

#include <SFGUI/SharedPtr.hpp>
namespace sfg
{
	class Window;
	class Button;
	class Box;
}

/// MainMenu
class MainMenu : public sbe::EventUser
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

		sfg::SharedPtr<sfg::Window> Win;
		sfg::SharedPtr<sfg::Button> BtnExit;
		sfg::SharedPtr<sfg::Box> ExitConfirmation;
		unsigned int currentlabeltext;

};

#endif // MAIN_MENUE_H

