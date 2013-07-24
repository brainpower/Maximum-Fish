#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "sbe/event/EventUser.hpp"
#include "world/Species.hpp"
#include "SimState.hpp"

#include "sbe/geom/Point.hpp"

class Tile;
class Terrain;
class Creature;
class StasisPod;

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
		~Simulator() {
			if ( state && state->numThreads > 1 ) stopThreads();
		}

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
			EVT_SAVEterrain	| save the terrain
			EVT_SAVE_WHOLE		| save the complete simulation
			EVT_LOAD_WHOLE		| load a simulation
			TERRAIN_CLICKED		| handle a click on the terrain
			SET_SIM_TPS			| set the simulation speed
			PLOT_COUNTS			| create a graphplotter with the population development; sent back with a DISPLAY_GRAPH event
			UPDATE_OVERLAYS		| create mapplotters for heightmap, humiditymap and populationmap; sent back with a DISPLAY_MAP event
		*/
		virtual void HandleEvent( Event& e);

		void init();
		void reset();
		/// Create a new Simulation from a seed
		void NewSimulation(std::shared_ptr<std::vector<std::shared_ptr<Species>>> _spec, std::shared_ptr<std::vector<int>> _specnum, int seed = 0);
		/// Create a new Simulation from the given rng, Terrain, species and Creatures
		void NewSimulation( std::shared_ptr<SimState> state);

		/// simulate one tick
		void advance();
		void forwardTo(const int tick);

		typedef std::list<std::shared_ptr<Tile>>::iterator TileIt;

		/// simulate a range of Tiles
		void tick(std::list<std::shared_ptr<Tile>>& list);
		void parallelTick();
		/// Cleanup after one tick has been simulated, remove dead creatures, count means of death, log counts
		void CleanupTick();


		std::shared_ptr<SimState> setState(const SimState &s){
			if ( state->numThreads > 1 )	stopThreads();
			auto old = state;
			state.reset( new SimState(s) );
			if ( state->numThreads > 1 )	initThreads();
			return old;
		}

		std::shared_ptr<SimState> setState( const std::shared_ptr<const SimState> s){
			if ( state->numThreads > 1 )	stopThreads();
			auto old = state;
			state = std::make_shared<SimState>(*s);
			if ( state->numThreads > 1 )	initThreads();
			return old;
		}

		std::shared_ptr<SimState> getState( ){
			return state;
		}

		static std::shared_ptr<Species> GetSpecies(const std::string& name)
		{
			return Instance->getSpecies(name);
		}
		static std::shared_ptr<Species> GetSpecies(const std::string& name, const std::shared_ptr<SimState> s );


		static std::shared_ptr<Terrain> GetTerrain()
		{
			return Instance->state->terrain;
		}

		std::mt19937& rnd()
		{
			state->numGenerated++;
			return *tid < 0 ? *(state->seeder) : *(state->gens[*tid]);
		}

		static std::mt19937& GetRnd()
		{
			return Instance->rnd();
		}

		static std::list<std::shared_ptr<Creature>>& GetCreatures() { return Instance->state->creatures; }

	private:

		typedef std::tuple<Geom::Pointf,int,Species*> CreatureRenderInfo;
		void UpdateCreatureRenderList();


		class ArrDeleter
		{
			public:
			void operator()(int* ptr) { delete[] ptr; }
		};


		friend class Generator;
		friend class SimState;
		//friend class StasisPod;


		std::shared_ptr<Species>& getSpecies( const std::string& name );

		void HandleClick( const Geom::Pointf& pos );

		void logTickStats();
		void CreatePlotters();
		void SendScaleUpdate();

		void registerIOPlugins();
		void saveWhole(const std::string &savePath);
		void loadWhole(const std::string &loadPath);

		/// create and initialize threads ( seed
		void initThreads();
		void stopThreads();
		/// thread entry point
		void thread(std::shared_ptr<std::list<std::shared_ptr<Tile>>> list, int seed);

		void resetStats();


        // ### MEMBERS ###

        /// for sending updates to the renderer
        sf::Clock RendererUpdate;
		/// measures how long it takes if the user requests to simulate X ticks
		sf::Clock TickTimer;

		/// holds the current state of the simulation
		std::shared_ptr<SimState> state;
		/// holds all saved States
		std::shared_ptr<StasisPod> _pod;

		///
        std::shared_ptr<std::vector<std::shared_ptr<Species>>> m_specs;
		std::shared_ptr<std::vector<int>> m_counts;


		int _freezeRate;
		/// how many ticks should be simulated? Not changed so the simulated amount is still remembered
		int TicksToSim;
		/// how many ticks are left to be simulated? 0 for infinite
		int simulateTicks;

		bool isPaused;
		bool isInitialized;

		std::vector<std::shared_ptr<std::list<std::shared_ptr<Tile>>>> CurrentLists;
		std::vector<std::shared_ptr<std::list<std::shared_ptr<Tile>>>> NextLists;
		std::vector<std::shared_ptr<boost::thread>> threads;
		std::shared_ptr<boost::barrier> startBarrier;
		std::shared_ptr<boost::barrier> endBarrier;

		boost::thread_specific_ptr<int> tid;

		static Simulator* Instance;

		// Statistics

		/// Counter for each type of Creature ( Carnivore, Herbivore, Herba )
		int CreatureCounts[3];
		/// Counter for each way a creature can die
        int MeansOfDeath[6];

		std::vector<int> CarnivoreCounts;
		std::vector<int> HerbivoreCounts;
		std::vector<int> HerbaeCounts;

		std::vector<int> EatenCounts;
		std::vector<int> StarvedCounts;
		std::vector<int> FrozenCounts;
		std::vector<int> ThirstCounts;
		std::vector<int> OldCounts;

		std::shared_ptr<sbe::GraphPlotter> CountGraph;
		std::shared_ptr<sbe::GraphPlotter> DeathGraph;

		std::vector<int> ProcessingTimes;



};

#endif // SIMULATOR_H
