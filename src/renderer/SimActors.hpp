#ifndef SimActors_HPP
#define SimActors_HPP

#include "sbe/event/EventUser.hpp"

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

class Tile;
class Creature;
class Actor;
class Renderer;

class SimActors : public EventUser
{
	public:
		SimActors(Renderer& R);

		/**		Event 					-- 		Data
			----------------------------------------------
			UpdateCreatureRenderList -> CreatureRenderList
			UpdateTileRenderList		 -> TileRenderlist
		*/
		void HandleEvent(Event& e);

	private:

		Renderer& Picasso;

		typedef std::list<std::shared_ptr<Creature>> CreatureRenderList;
		typedef std::vector<std::shared_ptr<Tile>> TileRenderList;

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
		std::shared_ptr<Actor> TileActor;
		std::shared_ptr<Actor> CreaturesActor;

		bool RenderGrid;
		sf::Color GridColor;
		std::shared_ptr<Actor> GridActor;

		/// how big should each tile be rendered edge length ( sprites are 32x32 )
		int TileSize;
		/// how big is the terrain ( it's assumed to be square right now )
		int TerrainSize;
};

#endif // SimActors_HPP
