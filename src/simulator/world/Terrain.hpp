#ifndef TERRAIN_H
#define TERRAIN_H

#include "sbe/geom/Point.hpp"
#include "Tile.hpp"

namespace sf { class Image; }

#include <list>
#include <vector>
#include <memory>
#include <functional>

class Creature;

/// Holds or Generates a Terrain
class Terrain
{
	public:

		Terrain();

		Terrain(const Terrain &o);


		/// will do some weather calculations in the future, for now just sends the current terrain to the renderer
		void UpdateTerrain() const;
		void setHumidityFactor( const float hf ) { humidityFactor = hf;}
		void setMaxElevation(const float e) {maxElevation = e;}
		float getHumidityFactor( ) const { return humidityFactor;}

		/// returns a list of Creatures in the given radius around Position for which filter returns true
		std::list<std::shared_ptr<Creature>> getNearby(glm::vec2 Position, float radius, std::function<bool(const std::shared_ptr<Creature>&)> filter = [](const std::shared_ptr<Creature>&){return true;} ) const;
		std::list<std::shared_ptr<Creature>> getNearby(glm::vec2 Position, float radius, int type, std::function<bool(const std::shared_ptr<Creature>&)> filter = [](const std::shared_ptr<Creature>&){return true;} ) const;
		std::list<std::shared_ptr<Creature>> getNearby(glm::vec2 Position, float radius, std::shared_ptr<Species> species,  std::function<bool(const std::shared_ptr<Creature>&)> filter = [](const std::shared_ptr<Creature>&){return true;} ) const;
		/// returns the nearest Creatures in radius around Position for which the filter returns true
		std::shared_ptr<Creature> getNearest(glm::vec2 Position, float radius,  std::function<bool(const std::shared_ptr<Creature>&)> filter = [](const std::shared_ptr<Creature>&){return true;} ) const;
		std::shared_ptr<Creature> getNearest(glm::vec2 Position, float radius, int type, std::function<bool(const std::shared_ptr<Creature>&)> filter = [](const std::shared_ptr<Creature>&){return true;} ) const ;
		std::shared_ptr<Creature> getNearest(glm::vec2 Position, float radius, std::shared_ptr<Species> species,  std::function<bool(const std::shared_ptr<Creature>&)> filter = [](const std::shared_ptr<Creature>&){return true;} ) const;


//		/// returns a list of Creatures in the given radius around Position for which filter returns true
//		std::list<std::shared_ptr<Creature>> getNearbyType(glm::vec2 Position, float radius, Species::SPECIES_TYPE t, std::function<bool(const std::shared_ptr<Creature>&)> filter = [](const std::shared_ptr<Creature>&){return true;} );
//		/// returns the nearest Creatures in radius around Position for which the filter returns true
//		std::shared_ptr<Creature> getNearestType(glm::vec2 Position, float radius, Species::SPECIES_TYPE t,  std::function<bool(const std::shared_ptr<Creature>&)> filter = [](const std::shared_ptr<Creature>&){return true;} );
//
//		std::list<std::shared_ptr<Creature>> getNearbySpecies(glm::vec2 Position, float radius, std::function<bool(const std::shared_ptr<Creature>&)> filter = [](const std::shared_ptr<Creature>&){return true;} );
//		/// returns the nearest Creatures in radius around Position for which the filter returns true
//		std::shared_ptr<Creature> getNearestSpecies(glm::vec2 Position, float radius, std::shared_ptr<Species> std::function<bool(const std::shared_ptr<Creature>&)> filter = [](const std::shared_ptr<Creature>&){return true;} );


		/// returns the eight neighbours of a tile
		std::list<std::shared_ptr<Tile>> getNeighbours(Tile& T) const;

		float getTileElevation(glm::vec2 pos) const;
		float getMaxElevation() const ;
		float getGlobalTemp() const;
		const glm::ivec2& getSize() const { return Size; };

		bool validPos( const glm::point2& Pos ) const
		{
			return (!( Pos.x >= Size.x || Pos.x < 0 || Pos.y >= Size.y || Pos.y < 0));
		}

		const std::shared_ptr<Tile>& getTile( glm::vec2 pos ) const;

		void CreateDebugTerrain( int seed );
		void CreateMapPlotters();
		void CreateParallelisationGraph();

		void UpdateTileMap();

		std::vector<std::list<std::shared_ptr<Tile>>>& getColors() { return Colors; }
		std::list<std::shared_ptr<Tile>> getTileList() const;
	private:

		friend class TerrainIOPlugin;



		// calculate Rainmap
		void calculateHumidity();
        void calcMaxElevation();

		/// Size of the Terrain in tiles x/y
		glm::ivec2 Size;

		float humidityFactor;

		///temperature at sea level (default 20)
		float globalTemp;

		///highest elevation (in m) on the map, lowest is 0 (sea level)
		float maxElevation;

		// returned in some cases instead of a valid Tile ( e.g. tile index out of range )
		std::shared_ptr<Tile> InvalidTile;
		std::vector<std::shared_ptr<Tile>> Tiles;
		//QuadTreeNode<Tile> CullTree;
		/// stores the list of colors and their tiles for parallel simulation. all tiles of the same color are guaranteed to not influence each other when simulated in parallel
		std::vector< std::list<std::shared_ptr<Tile> > > Colors;


		std::shared_ptr<sf::Image> tilemapImage;
};

#endif // TERRAIN_H
