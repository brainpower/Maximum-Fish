#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "sbe/event/EventUser.hpp"
#include "Species.hpp"

class Tile;
class Terrain;
class Creature;

#include <list>

#include <SFML/System/NonCopyable.hpp>

class Simulator : public EventUser, public sf::NonCopyable
{
	public:
		Simulator();
		virtual ~Simulator() {};
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

		void registerIOPlugins();
		void saveEvent(const std::string &savePath);

		int currentSeed;
		std::default_random_engine gen;
		std::list<std::shared_ptr<Creature>> Creatures;
		std::vector<std::shared_ptr<Species>> SpeciesList;
		std::shared_ptr<Terrain> Terra;

		bool isPaused;

		static Simulator* Instance;
};

#endif // SIMULATOR_H
