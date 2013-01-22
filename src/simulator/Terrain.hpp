#ifndef TERRAIN_H
#define TERRAIN_H

#include "sbe/util/QuadTree.hpp"
#include "sbe/Geom.hpp"
#include "Tile.hpp"

#include <list>
#include <memory>
#include <functional>

class Creature;

class Terrain
{
	public:

		Terrain();

		/// will do some weather calculations in the future, for now just sends the current terrain to the renderer
		void UpdateTerrain();
		void setHumidityFactor( const float hf ) { humidityFactor = hf;}
		void setMaxElevation(const float e) {maxElevation = e;}
		float getHumidityFactor( ) const { return humidityFactor;}

		std::list<std::shared_ptr<Creature>> getNearby(Geom::Vec2f Position, float radius, std::function<bool(const std::shared_ptr<Creature>&)> filter = []{return true;} );
		std::list<std::shared_ptr<Tile>> getNeighbours(Tile& T);

		float getTileElevation(Geom::Vec2f pos);
		float getMaxElevation();
		float getGlobalTemp();
		const Geom::Vec2& getSize() const { return Size; };

		std::shared_ptr<Tile>& getTile( Geom::Vec2f pos );

		void CreateDebugTerrain();

	private:

		friend class TerrainIOPlugin;



		// calculate Rainmap
		void calculateHumidity();

		/// Size of the Terrain in tiles x/y
		Geom::Vec2 Size;

		float humidityFactor;
		
		///temperature at sea level (default 20)
		float globalTemp;              
		
		///highest elevation (in m) on the map, lowest is 0 (sea level)
		float maxElevation;              

		// returned in some cases instead of a valid Tile ( e.g. tile index out of range )
		std::shared_ptr<Tile> InvalidTile;
		std::vector<std::shared_ptr<Tile>> Tiles;
		QuadTreeNode<Tile> CullTree;
};

#endif // TERRAIN_H
