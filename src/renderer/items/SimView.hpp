#ifndef SIMVIEW_H
#define SIMVIEW_H

#include <SFGUI/Widget.hpp>

class SimView : public sfg::Widget
{

	public:
		typedef std::vector<std::shared_ptr<Creature>> CreatureRenderList;
		typedef std::vector<std::shared_ptr<Tile>> TileRenderList;

	/**		Event 					-- 		Data
		----------------------------------------------
		UpdateCreatureRenderList -> CreatureRenderList
		UpdateTileRenderList		 -> TileRenderlist
		Render
	*/
	void HandleEvent( Event& e )

	private:
		void Render();

		void ReadTileRenderList( TileRenderList& r );
		void ReadCreatureRenderList( CreatureRenderList& r );

		sf::VertexArray Creatures;
		sf::VertexArray Tiles;
}

#endif // SIMVIEW_H

