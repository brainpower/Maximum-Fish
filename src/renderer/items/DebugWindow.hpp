#ifndef DGB_WINDOW_H
#define DGB_WINDOW_H

#include "sbe/event/EventUser.hpp"

#include <SFGUI/SFGUI.hpp>

class DebugWindow : public EventUser
{
	public:

		DebugWindow( const Geom::Point& RelativePosition = Geom::Point(0,0), const Geom::Vec2 Size = Geom::Vec2(600, 280));
		virtual ~DebugWindow() {};

		virtual void HandleEvent( Event& e);

	private:
		void CreateWindow(const Geom::Point& RelativePosition, const Geom::Vec2 Size);
		void UpdateText();

		sfg::Label::Ptr LogText;
		sfg::Label::Ptr DbgText;
		sfg::Label::Ptr DbgLabels;
		sfg::Box::Ptr	LogBox;
		sfg::Window::Ptr Win;
		sfg::ScrolledWindow::Ptr scrolledwindow;
		unsigned int currentlabeltext;

		// a list of debugging strings which are set by event
		std::map<std::string, std::string> DebugStrings;
};

#endif // DGB_WINDOW_H

