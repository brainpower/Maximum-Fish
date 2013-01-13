#ifndef TERRAIN_H
#define TERRAIN_H

#include <list>
#include <memory>

#include "sbe/util/QuadTree.hpp"
#include "sbe/Geom.hpp"
#include "Tile.hpp"


class Terrain
{
	public:

		void UpdateTerrain();
		void setHumidityFactor( const float hf ) { humidityFactor = hf;}
		void setMaxElevation(const float e) {maxElevation = e;}
		float getHumidityFactor( ) const { return humidityFactor;}

		float getTileElevation(Geom::Vec2f pos);
		float getMaxElevation();
		float getGlobalTemp();

		std::shared_ptr<Tile>& getTile( Geom::Vec2f pos );

		void CreateDebugTerrain();

	private:

		friend class TerrainIOPlugin;



		// calculate Rainmap
		void calculateHumidity();

		/// Size of the Terrain in tiles x/y
		Geom::Vec2 Size;

		float humidityFactor;
		float globalTemp;              //temperature at sea level
		float maxElevation;              //highest elevation (in m) on the map, lowest is 0 (sea level)

		std::vector<std::shared_ptr<Tile>> Tiles;
		QuadTreeNode<Tile> CullTree;
};

#endif // TERRAIN_H
