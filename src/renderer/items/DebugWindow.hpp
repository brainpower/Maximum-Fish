#ifndef DGB_WINDOW_H
#define DGB_WINDOW_H

#include "sbe/event/EventUser.hpp"
#include "../Screen.hpp"
#include <memory>
#include <SFGUI/SFGUI.hpp>

class DebugWindow : public EventUser
{
	enum FilterLevel
	{
		DEFAULT, VERBOSE, PEDANTIC
	};

	public:

		DebugWindow( const Geom::Point& RelativePosition = Geom::Point(0,0), const Geom::Vec2 Size = Geom::Vec2(600, 280));
		virtual ~DebugWindow() {};

		virtual void HandleEvent( Event& e);

	private:
		void CreateWindow(const Geom::Point& RelativePosition, const Geom::Vec2 Size);

		void OnConsoleInputActivation();

		void UpdateText(FilterLevel level = FilterLevel::VERBOSE);


		sfg::Label::Ptr LogText;
		sfg::Label::Ptr DbgText;
		sfg::Label::Ptr DbgLabels;
		sfg::Box::Ptr	LogBox;
		sfg::Window::Ptr Win;
		sfg::ScrolledWindow::Ptr scrolledwindow;
		unsigned int currentlabeltext;
		unsigned int updateCounter;

		// a list of debugging strings which are set by event
		std::map<std::string, std::string> DebugStrings;
};

#endif // DGB_WINDOW_H

