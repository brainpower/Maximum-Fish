#ifndef MAIN_MENUE_H
#define MAIN_MENUE_H

#include "sbe/event/EventUser.hpp"

#include "sbe/geom/Point.hpp"


namespace sfg
{
	class Window;
}

namespace sbe {
    class FileBrowse;
}

/// MainMenu
class MainMenu : public sbe::EventUser
{
	public:
		MainMenu( const glm::ivec2 Size = glm::ivec2(200, 300));
		virtual ~MainMenu() {};

		virtual void HandleEvent( Event& e);

	private:
		void InitWindow(const glm::ivec2 Size);
		void BtnResumeClick();
		void BtnNewClick();
		void BtnSaveClick();
		void BtnLoadClick();
		void BtnExitClick();
		void updatePosition();

		std::shared_ptr<sfg::Window> Win;
		std::shared_ptr<sbe::FileBrowse> fb;

		unsigned int currentlabeltext;

};

#endif // MAIN_MENUE_H
