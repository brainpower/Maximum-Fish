#ifndef INFOPANEL_H
#define INFOPANEL_H

#include "sbe/event/EventUser.hpp"
#include "renderer/items/DetailsEmpty.hpp"
#include "renderer/items/DetailsCreature.hpp"
#include "renderer/items/DetailsSpecies.hpp"
#include "renderer/items/DetailsTile.hpp"
#include "renderer/items/Selector.hpp"
#include "renderer/items/Manipulator.hpp"

#include <SFGUI/SFGUI.hpp>

/// Info panel on the righthand side of the screen showing details and lists
class InfoPanel : public EventUser
{
	public:

		InfoPanel( const Geom::Point& RelativePosition = Geom::Point(280, 0), const Geom::Vec2 Size = Geom::Vec2(300, 600));
		virtual ~InfoPanel() {};

		virtual void HandleEvent( Event& e);
	private:
		void CreateWindow(const Geom::Point& RelativePosition, const Geom::Vec2 Size);
		void updatePosition();

		sfg::Window::Ptr Win;
        sfg::Notebook::Ptr Topleveltabs;
        std::unique_ptr<Selector> MySelector;
        std::unique_ptr<Manipulator> MyManipulator;
		sfg::Box::Ptr SelectionBox;
		sfg::Box::Ptr ManipulationBox;
};

#endif // INFOPANEL_H

