#ifndef CREATURE_DETAILS_H
#define CREATURE_DETAILS_H

#include "sbe/event/EventUser.hpp"

#include <SFGUI/SFGUI.hpp>

class CreatureDetails : public EventUser
{
	public:

		CreatureDetails( const Geom::Point& RelativePosition = Geom::Point(280, 0), const Geom::Vec2 Size = Geom::Vec2(300, 600));
		virtual ~CreatureDetails() {};

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

#endif // CREATURE_DETAILS_H

