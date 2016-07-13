#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "simulator/world/Species.hpp"
#include "sbe/geom/Point.hpp"

#include <SFML/System/Clock.hpp>

#include <vector>
#include <list>
#include <memory>
#include <string>
#include <random>

class Simulator;
class SimState;
class Creature;
class Terrain;

class Generator
{
	public:
		//~ Generator( std::vector<std::shared_ptr<Species>>& _Spec,
					//~ std::list<std::shared_ptr<Creature>>& _Creatures,
					//~ Terrain& _T,
					//~ std::mt19937& _rnd );
		Generator( std::shared_ptr<SimState> _state);

		/// create a number of species with several creatures for each species
		void CreateSpeciesWithCreatures( Species::SPECIES_TYPE type, int SpeciesCount, int CreatureCount );

		void CreateCreatures( std::shared_ptr<std::vector<std::shared_ptr<Species>>> _sp, std::shared_ptr<std::vector<int>> _spc , int _mult);
		/// create a creature with a random species
		std::shared_ptr<Creature> createRandomCreature();
		/// add a creature with the given species (by name)
		std::shared_ptr<Creature> createCreature( const std::string& specName );
		/// add a creature with the given species
		std::shared_ptr<Creature> createCreature( const std::shared_ptr<Species>& spec );

		std::shared_ptr<Creature> createNonRandomCreature( const std::shared_ptr<Species>& spec );



		/// create a random new species
		static std::shared_ptr<Species> createRandomSpecies(Species::SPECIES_TYPE type, const std::string& name, std::mt19937& rnd);

		/**
			Create a new species with a given type.
			@param type the type of the new species
			@param name the name for the new species
		*/
		static std::shared_ptr<Species> createSpecies( Species::SPECIES_TYPE type, const std::string& name );


	private:
	    std::mt19937& rnd;
		std::shared_ptr<SimState> state;

};

#endif // GENERATOR_HPP
