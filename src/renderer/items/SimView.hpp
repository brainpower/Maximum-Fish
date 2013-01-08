#ifndef SIMVIEW_H
#define SIMVIEW_H

#include <SFGUI/Widget.hpp>

#include <SFML/System/Vector2.hpp>

#include "sbe/event/EventUser.hpp"

#include <string>
#include <vector>
#include <memory>
#include <random>

class Creature;
class Tile;

class SimView : public EventUser, public sfg::Widget
{
	public:

		SimView();

		typedef std::list<std::shared_ptr<Creature>> CreatureRenderList;
		typedef std::vector<std::shared_ptr<Tile>> TileRenderList;

		/**		Event 					-- 		Data
			----------------------------------------------
			UpdateCreatureRenderList -> CreatureRenderList
			UpdateTileRenderList		 -> TileRenderlist
			Render
		*/

		void HandleEvent( Event& e );
		// handles key input for scrolling/zooming/etc
		void HandleSfmlEvent ( const sf::Event& e);

		// from sfg::Widget

		virtual const std::string& GetName() const { return Name; }
		virtual sf::Vector2f CalculateRequisition();

		void Render();
	private:

		void ReadTileRenderList( TileRenderList& r );
		void ReadCreatureRenderList( CreatureRenderList& r );
		void CreateGrid( int TerrainSize );


		/**
			Determines which Sprite from the tileset should be used do render this tile.
			Right now this just uses a simple height formula to determine what to render.
		*/
		int DetermineTileSpriteIndex ( std::shared_ptr<Tile>& t);
		int DetermineCreatureSpriteIndex ( std::shared_ptr<Creature>& t);
		
			/// determines where a Tile should be rendered on the viewport
		sf::FloatRect DetermineTilePos ( std::shared_ptr<Tile>& t);
		sf::FloatRect DetermineCreaturePos ( std::shared_ptr<Creature>& c);

		void SetupCamera();
		void UpdateCamera();
		sf::View Camera;
		sf::Vector2f TargetSize;
		sf::Vector2f TargetCenter;

		sf::VertexArray Creatures;
		sf::VertexArray Tiles;

		bool RenderGrid;
		sf::Color GridColor;
		sf::VertexArray Grid;

		/// how big should each tile be rendered edge length ( sprites are 32x32 )
		int TileSize;
		const std::string Name;
};

#endif // SIMVIEW_H

