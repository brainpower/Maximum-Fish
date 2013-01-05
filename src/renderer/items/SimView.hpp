#ifndef SIMVIEW_H
#define SIMVIEW_H

#include <SFGUI/Widget.hpp>

#include "sbe/event/EventUser.hpp"

#include <string>
#include <vector>
#include <memory>

class Creature;
class Tile;

class SimView : public EventUser, public sfg::Widget
{
	public:

		SimView() : Name("SimView") {}

		typedef std::vector<std::shared_ptr<Creature>> CreatureRenderList;
		typedef std::vector<std::shared_ptr<Tile>> TileRenderList;

		/**		Event 					-- 		Data
			----------------------------------------------
			UpdateCreatureRenderList -> CreatureRenderList
			UpdateTileRenderList		 -> TileRenderlist
			Render
		*/
			
		void HandleEvent( Event& e );


		// from sfg::Widget

		virtual const std::string& GetName() const { return Name; }
		virtual sf::Vector2f CalculateRequisition();

		void Render();
	private:

		void ReadTileRenderList( TileRenderList& r );
		void ReadCreatureRenderList( CreatureRenderList& r );

		sf::VertexArray Creatures;
		sf::VertexArray Tiles;

		const std::string Name;
};

#endif // SIMVIEW_H

