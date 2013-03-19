#ifndef SimActors_HPP
#define SimActors_HPP

#include "sbe/event/EventUser.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Color.hpp>

#include <list>
#include <vector>
#include <memory>

enum RenderLayers {
	L_BACK = 0,
	L_TERRAIN,
	L_OVERLAY,
	L_CREATURES,
	L_END
};

namespace sbe
{
	class Actor;
	class Renderer;
	class GraphPlotter;
	class MapPlotter;
	class MessageHandler;
}
class Tile;
class Creature;
class Species;



/**
	Interface between Simulator and Renderer/UI.
	Converts most Events sent from the Simulator into actors or windows.

	Config values used:            | .
	-------------------------------|--------------------
	Size of each Tile (e.g. 32): | system.ui.simView.tileSize
	Terrain Tiles# ( e.g. 32x32):| system.sim.terragen.debug.size
	Size of the creatures:       | system.ui.simView.creatureSize
	Render the grid:             | system.ui.simView.renderGrid
	Use shader to render the tilemap: | system.ui.simView.useShaderTileMap
	Color of the Grid (rgb):     | system.ui.simView.gridColor.r\n system.ui.simView.gridColor.g\n system.ui.simView.gridColor.b

*/
class SimActors : public sbe::EventUser
{
	public:
		SimActors();
		~SimActors();

		/**
			Events Handled:
				Event 					| 		Data
			------------------|---------------------------
			UpdateCreatureRenderList	| CreatureRenderList
			UpdateTileRenderList		| TileRenderlist
			DISPLAY_GRAPH				| std::shared_ptr<GraphPlotter>
		*/
		void HandleEvent(Event& e);

	private:

		typedef std::list<std::shared_ptr<Creature>> CreatureRenderList;
		typedef std::vector<std::shared_ptr<Tile>> TileRenderList;

		void PlotGraph ( std::shared_ptr<sbe::GraphPlotter>& G );
		void ReadTileRenderList( TileRenderList& r );

		void ReadCreatureRenderList( CreatureRenderList& r );

		/// Creates an Actor for rendering the Grid
		void CreateGrid();
		/// Creates an Actor for rendering the Terrain as a VertexArray
		void CreateTerrainVertexArray(TileRenderList& r);
		/// Creates an Actor for rendering the Terrain via Shaders
		void CreateTerrainShaderMap();

		int DetermineCreatureSpriteIndex ( std::shared_ptr<Creature>& t);
		/// determines where a Tile should be rendered on the viewport
		sf::FloatRect DetermineTilePos ( std::shared_ptr<Tile>& t);
		/// determines where a Creature should be rendered on the viewport
		sf::FloatRect DetermineCreaturePos ( std::shared_ptr<Creature>& c);


		bool useShaderTileMap;
		// one pixel for each tile to determine the right sprite to render
		sf::Texture tilemapTexture;

		int CreatureSize;
		/// at which amount of creatures should we cull the renderlist
		int cullThreshold;
		std::shared_ptr<sbe::Actor> TileActor;
		std::shared_ptr<sbe::Actor> CreaturesActor;

		bool RenderGrid;
		sf::Color GridColor;
		std::shared_ptr<sbe::Actor> GridActor;

		/// how big should each tile be rendered edge length ( sprites are 32x32 )
		int TileSize;
		/// how big is the terrain ( it's assumed to be square right now )
		int TerrainSize;

		std::unique_ptr<sbe::MessageHandler> MessageHandler_;

		/// pointer to currently highlighted species a.o
		Species* m_highlight;
};

#endif // SimActors_HPP
