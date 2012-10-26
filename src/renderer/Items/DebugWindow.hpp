#ifndef DGB_WINDOW_H
#define DGB_WINDOW_H

#include "sbe/event/EventUser.hpp"

#include <SFGUI/SFGUI.hpp>

class DebugWindow : public EventUser
{
	public:

		DebugWindow( const Geom::Point& RelativePosition = Geom::Point(0,0), const Geom::Vec2 Size = Geom::Vec2(300, 300));
		virtual ~DebugWindow() {};

		virtual void HandleEvent( Event& e);

	private:
		void CreateWindow(const Geom::Point& RelativePosition, const Geom::Vec2 Size);
		void UpdateText();

		sfg::Label::Ptr Text;
		sfg::Window::Ptr Win;

		// a list of debugging strings which are set by event
		std::map<std::string, std::string> DebugStrings;
};

#endif // DGB_WINDOW_H

