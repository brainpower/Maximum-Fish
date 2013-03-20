#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "sbe/event/EventUser.hpp"
#include "world/Species.hpp"

#include "sbe/geom/Point.hpp"

class Tile;
class Terrain;
class Creature;

namespace sbe {
	class GraphPlotter;
}

#include <list>
#include <vector>
#include <random>
#include <memory>

#include <SFML/System/NonCopyable.hpp>

/**
	Management class for a simulation, responsible for initalising and running the Simulation.

	Sends the following events:
	Event						|		Data
	----------------------------|-----------------------------
	DISPLAY_GRAPH				| shared_ptr<GraphPlotter>
	UpdateCreatureRenderList	| list<shared_ptr<Creature>>
	TILE_CLICKED				| shared_ptr<Tile>
	CREATURE_CLICKED			| shared_ptr<Creature>
	EVT_SAVE_BAD				| std::string
	EVT_SAVE_GOOD				| -
*/
class Simulator : public sbe::EventUser, sf::NonCopyable
{
	public:
		Simulator();
		virtual ~Simulator() {};

		/**
			Handles the following events:
			Event				| Action
			--------------------|------------------
			EVT_TICK			| Frame Event, simulate a new tick if active
			EVT_QUIT			| End the simulation and quit
			TOGGLE_SIM_PAUSE	| Toggle pause
			SIM_PAUSE			| pause
			SIM_UNPAUSE			| unpause
			RESET_SIMULATION	| start a new simulation in paused mode ( default seed )
			EVT_SAVE_TERRAIN	| save the terrain
			EVT_SAVE_WHOLE		| save the complete simulation
			EVT_SAVE_WHOLE_TEST | do a saveWhole() to `sim.debugsavepath`
			EVT_LOAD_WHOLE_TEST | do a loadWhole() from `sim.debugsavepath`
			EVT_LOAD_WHOLE		| load a simulation
			TERRAIN_CLICKED		| handle a click on the terrain
			SET_SIM_TPS			| set the simulation speed
			PLOT_COUNTS			| create a graphplotter with the population development; sent back with a DISPLAY_GRAPH event
			UPDATE_OVERLAYS		| create mapplotters for heightmap, humiditymap and populationmap; sent back with a DISPLAY_MAP event
		*/
		virtual void HandleEvent( Event& e);

		void init();
		void NewSimulation( int seed = 0);

		/// simulate one tick
		void advance();

		typedef std::list<std::shared_ptr<Tile>>::iterator TileIt;

		/// simulate a range of Tiles
		void tick(std::shared_ptr<std::list<std::shared_ptr<Tile>>> list, std::shared_ptr<int> _CreatureCounts);
		void parallelTick();

		static std::shared_ptr<Species> GetSpecies(const std::string& name)
		{
			return Instance->getSpecies(name);
		}

		static std::shared_ptr<Terrain> GetTerrain()
		{
			return Instance->Terra;
		}

		std::mt19937& rnd()
		{
			numGenerated++;
			return *gen;
		}

		static std::mt19937& GetRnd()
		{
			return Instance->rnd();
		}

		static std::list<std::shared_ptr<Creature>>& GetCreatures() { return Instance->Creatures; }

	private:

		class ArrDeleter
		{
			public:
			void operator()(int* ptr) { delete[] ptr; }
		};


		friend class Generator;
		//friend class StasisPod;

		sf::Clock RendererUpdate;

		std::shared_ptr<Species>& getSpecies( const std::string& name );

		void HandleClick( const Geom::Pointf& pos );

		void logTickStats();
		std::shared_ptr<sbe::GraphPlotter> CreateCountPlotter();

		void registerIOPlugins();
		void saveWhole(const std::string &savePath);
		void loadWhole(const std::string &loadPath);

		int currentSeed;
		int currentTick;
		unsigned int numGenerated; // number of generated random numbers
		std::shared_ptr<std::mt19937> gen;

		std::list<std::shared_ptr<Creature>> Creatures;
		std::vector<std::shared_ptr<Species>> SpeciesList;
		std::shared_ptr<Terrain> Terra;

		bool isPaused;

		std::vector<std::shared_ptr<std::list<std::shared_ptr<Tile>>>> Lists;
		std::vector<std::shared_ptr<int>> CreatureCounters;
		std::vector<boost::thread> threads;
		std::shared_ptr<boost::barrier> startBarrier;
		std::shared_ptr<boost::barrier> endBarrier;
		/// thread entry point
		void initThreads();
		void stopThreads();
		void thread(std::shared_ptr<std::list<std::shared_ptr<Tile>>> list, std::shared_ptr<int> _CreatureCounts);

		int numThreads;
		bool multiThreaded;

		static Simulator* Instance;


		// Statistics
		/// Counter for each type of Creature ( Carnivore, Herbivore, Herba )
		int CreatureCounts[3];
		std::vector<int> CarnivoreCounts;
		std::vector<int> HerbivoreCounts;
		std::vector<int> HerbaeCounts;

		std::vector<int> ProcessingTimes;

};

#endif // SIMULATOR_H
