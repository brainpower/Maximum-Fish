#ifndef CREATURE_LIST_H
#define CREATURE_LIST_H

#include "sbe/event/EventUser.hpp"
#include "renderer/items/DetailsEmpty.hpp"
#include "renderer/items/DetailsCreature.hpp"
#include "renderer/items/DetailsSpecies.hpp"
#include "renderer/items/DetailsTile.hpp"

#include <SFGUI/SFGUI.hpp>

class CreatureList : public EventUser
{
	public:

		CreatureList( const Geom::Point& RelativePosition = Geom::Point(280, 0), const Geom::Vec2 Size = Geom::Vec2(300, 600));
		virtual ~CreatureList() {};

		virtual void HandleEvent( Event& e);
	private:
		void CreateWindow(const Geom::Point& RelativePosition, const Geom::Vec2 Size);
        void SetDetail( const std::shared_ptr<Creature>& _creature);
        void SetDetail( const std::shared_ptr<Species>& _species);
        void SetDetail( const std::shared_ptr<Tile>& _tile);
		void updatePosition();

		sfg::Window::Ptr Win;
		sfg::Window::Ptr DetailsWin;
		std::shared_ptr<Details> CurrentDetails;
};

#endif // CREATURE_LIST_H

