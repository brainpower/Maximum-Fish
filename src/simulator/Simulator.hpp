#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "sbe/event/EventUser.hpp"
#include "Species.hpp"

#include "sbe/geom/Point.hpp"

class Tile;
class Terrain;
class Creature;

class GraphPlotter;

#include <list>
#include <random>

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
	EVT_SAVE_BAD				| -
	EVT_SAVE_GOOD				| -
*/
class Simulator : public EventUser, sf::NonCopyable
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
			EVT_SAVE_WHOLE_TEST | ???
			EVT_LOAD_WHOLE		| load a simulation
			TERRAIN_CLICKED		| handle a click on the terrain
			SET_SIM_TPS			| set the simulation speed
			PLOT_COUNTS			| create a graphplotter with the population development and mapplotters for heightmap, humiditymap and populationmap; sent back with a DISPLAY_GRAPH event
		*/
		virtual void HandleEvent( Event& e);

		void init();
		void NewSimulation( int seed = 0);

		void tick();

		static std::shared_ptr<Species> GetSpecies(const std::string& name)
		{
			return Instance->getSpecies(name);
		}

		static std::shared_ptr<Terrain> GetTerrain()
		{
			return Instance->Terra;
		}

		static std::default_random_engine& GetEngine()
		{
			return Instance->gen;
		}

	private:

		sf::Clock RendererUpdate;

		std::shared_ptr<Species>& getSpecies( const std::string& name );

		void HandleClick( const Geom::Pointf& pos );

		void CreateSpeciesWithCreatures(  Species::SPECIES_TYPE type, int SpeciesCount, int CreatureCount );
		void addRandomCreature();
		/// add a creature with the given species
		void addCreature( const std::string& specName );
		void addRandomSpecies();
		/** Create a new species with a given type.
			@param type the type of the new species
			@return the name of the new species
		*/
		std::string addSpecies( Species::SPECIES_TYPE type );


		void logTickStats();
		std::shared_ptr<GraphPlotter> CreateCountPlotter();

		void registerIOPlugins();
		void saveWhole(const std::string &savePath);
		void loadWhole(const std::string &loadPath);

		template<class T, class FwdIterator>
		std::list<T> a2list(FwdIterator begin, FwdIterator end);

		int currentSeed;
		int currentTick;
		unsigned int numGenerated; // number of generated random numbers
		std::default_random_engine gen;
		std::list<std::shared_ptr<Creature>> Creatures;
		std::vector<std::shared_ptr<Species>> SpeciesList;
		std::shared_ptr<Terrain> Terra;

		bool isPaused;

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
