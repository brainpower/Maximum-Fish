#ifndef CREATURE_LIST_H
#define CREATURE_LIST_H

#include "sbe/event/EventUser.hpp"
#include "renderer/items/CreatureDetails.hpp"

#include <SFGUI/SFGUI.hpp>
class CreatureDetails;

class CreatureList : public EventUser
{
	public:

		CreatureList( const Geom::Point& RelativePosition = Geom::Point(280, 0), const Geom::Vec2 Size = Geom::Vec2(300, 600));
		virtual ~CreatureList() {};

		virtual void HandleEvent( Event& e);
	private:
		void CreateWindow(const Geom::Point& RelativePosition, const Geom::Vec2 Size);
		void UpdateText();
		void updatePosition();


		sfg::Window::Ptr Win;
		CreatureDetails* Details;
};

#endif // CREATURE_LIST_H

