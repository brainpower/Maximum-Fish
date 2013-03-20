#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "simulator/world/Species.hpp"
#include "sbe/geom/Point.hpp"

#include <vector>
#include <list>
#include <memory>
#include <string>

class Simulator;
class Creature;
class Terrain;

class Generator
{
	public:
		Generator( std::vector<std::shared_ptr<Species>>& _Spec,
					std::list<std::shared_ptr<Creature>>& _Creatures,
					Terrain& _T,
					std::mt19937& _rnd );

		/// create a number of species with several creatures for each species
		void CreateSpeciesWithCreatures( Species::SPECIES_TYPE type, int SpeciesCount, int CreatureCount );
		/// create a creature with a random species
		std::shared_ptr<Creature> createRandomCreature();
		/// add a creature with the given species (by name)
		std::shared_ptr<Creature> createCreature( const std::string& specName );
		/// add a creature with the given species
		std::shared_ptr<Creature> createCreature( const std::shared_ptr<Species>& spec );
		/// create a random new species
		std::shared_ptr<Species> createRandomSpecies();
		/**
			Create a new species with a given type.
			@param type the type of the new species
		*/
		std::shared_ptr<Species> createSpecies( Species::SPECIES_TYPE type );

	private:
		std::vector<std::shared_ptr<Species>>& Spec;
		std::list<std::shared_ptr<Creature>>& Creatures;
		std::mt19937& rnd;
		Terrain& Terra;

};

#endif // GENERATOR_HPP

