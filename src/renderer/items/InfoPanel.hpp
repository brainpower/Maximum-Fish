#ifndef INFOPANEL_H
#define INFOPANEL_H

#include "sbe/event/EventUser.hpp"
#include "renderer/items/DetailsEmpty.hpp"
#include "renderer/items/DetailsCreature.hpp"
#include "renderer/items/DetailsSpecies.hpp"
#include "renderer/items/DetailsTile.hpp"

#include <SFGUI/SFGUI.hpp>

class InfoPanel : public EventUser
{
	public:

		InfoPanel( const Geom::Point& RelativePosition = Geom::Point(280, 0), const Geom::Vec2 Size = Geom::Vec2(300, 600));
		virtual ~InfoPanel() {};

		virtual void HandleEvent( Event& e);
	private:
		void CreateWindow(const Geom::Point& RelativePosition, const Geom::Vec2 Size);
        void SetDetail( const std::shared_ptr<Creature>& _creature);
        void SetDetail( const std::shared_ptr<Species>& _species);
        void SetDetail( const std::shared_ptr<Tile>& _tile);
		void updatePosition();

		sfg::Window::Ptr Win;
		sfg::Frame::Ptr CreatureFrame;
		sfg::Frame::Ptr SpeciesFrame;
		sfg::Frame::Ptr TileFrame;
		std::shared_ptr<Details> CurrentDetailsCreature;
		std::shared_ptr<Details> CurrentDetailsSpecies;
		std::shared_ptr<Details> CurrentDetailsTile;
};

#endif // INFOPANEL_H

